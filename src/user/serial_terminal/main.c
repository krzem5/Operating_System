#include <readline/readline.h>
#include <sys/error/error.h>
#include <sys/fd/fd.h>
#include <sys/format/format.h>
#include <sys/io/io.h>
#include <sys/mp/process.h>
#include <sys/mp/process_group.h>
#include <sys/mp/thread.h>
#include <sys/pipe/pipe.h>
#include <sys/signal/signal.h>
#include <sys/socket/socket.h>
#include <sys/string/string.h>
#include <sys/syscall/syscall.h>
#include <sys/system/system.h>
#include <sys/types.h>
#include <sys/util/options.h>
#include <terminal/server.h>
#include <terminal/session.h>
#include <terminal/terminal.h>



#define TERMINAL_SIZE_PROBE_MAX_INPUT_LENGTH 64



static sys_fd_t in_fd=0;
static sys_fd_t child_in_fd=0;
static sys_fd_t out_fd=0;
static sys_fd_t child_out_fd=0;
static sys_fd_t ctrl_fd=0;
static bool ctrl_direct_io=0;
static bool ctrl_echo_input=1;
static bool ctrl_autocomplete_enabled=1;
static u32 ctrl_terminal_size[2]={80,24};



static bool _parse_cursor_position(sys_fd_t fd,u32* out){
	out[0]=0;
	out[1]=0;
	u32 state=0;
	for (u32 i=0;i<TERMINAL_SIZE_PROBE_MAX_INPUT_LENGTH;i++){
		char c;
		if (!sys_fd_read(fd,&c,1,0)){
			return 0;
		}
_reset_state:
		if (!state){
			state+=(c=='\x1b');
		}
		else if (state==1){
			if (c=='['){
				state++;
			}
			else{
				state=0;
				goto _reset_state;
			}
		}
		else if (state==2){
			if (c==';'){
				state++;
			}
			else if (c<'0'||c>'9'){
				state=0;
				goto _reset_state;
			}
			else{
				out[1]=out[1]*10+c-48;
			}
		}
		else if (state==3){
			if (c=='R'){
				return 1;
			}
			else if (c<'0'||c>'9'){
				state=0;
				goto _reset_state;
			}
			else{
				out[0]=out[0]*10+c-48;
			}
		}
	}
	return 0;
}



static void _probe_terminal_size(sys_fd_t in,sys_fd_t out){
	u32 xy[2];
	u32 wh[2];
	if (sys_fd_write(out,"\x1b[6n",4,0)!=4||!_parse_cursor_position(in,xy)||sys_fd_write(out,"\x1b[9999;9999H\x1b[6n",16,0)!=16||!_parse_cursor_position(in,wh)){
		return;
	}
	char buffer[32];
	u32 buffer_length=sys_format_string(buffer,sizeof(buffer),"\x1b[%u;%uH",xy[1],xy[0]);
	if (sys_fd_write(out,buffer,buffer_length,0)!=buffer_length){
		return;
	}
	ctrl_terminal_size[0]=wh[0];
	ctrl_terminal_size[1]=wh[1];
}



static void _autocomplete_callback(readline_state_t* state,const char* prefix){
	if (!ctrl_autocomplete_enabled){
		return;
	}
	char path[4096];
	s32 j=-1;
	for (u32 i=0;prefix[i]&&i<sizeof(path)-1;i++){
		path[i]=prefix[i];
		if (path[i]=='/'){
			j=i;
		}
	}
	path[j+1]=0;
	u32 length=sys_string_length(prefix)-j-1;
	sys_fd_t cwd_fd=sys_fd_dup(SYS_FD_DUP_CWD,0);
	sys_fd_t fd=sys_fd_open(cwd_fd,(path[0]?path:"."),0);
	sys_fd_close(cwd_fd);
	for (sys_fd_iterator_t iter=sys_fd_iter_start(fd);!SYS_IS_ERROR(iter);iter=sys_fd_iter_next(iter)){
		char name[256];
		if (SYS_IS_ERROR(sys_fd_iter_get(iter,name,256))||sys_string_compare_up_to(name,prefix+j+1,length)){
			continue;
		}
		sys_string_copy(name,path+j+1);
		readline_add_autocomplete(state,path);
	}
	sys_fd_close(fd);
}



static void _input_thread(void* ctx){
	readline_state_t state;
	readline_state_init(out_fd,4096,1024,_autocomplete_callback,&state);
	while (1){
		u8 buffer[4096];
		sys_error_t length=sys_fd_read(in_fd,buffer,sizeof(buffer),0);
		if (!length||SYS_IS_ERROR(length)){
			goto _error;
		}
		if (ctrl_direct_io){
			if (state.line_length){
				sys_error_t ret=sys_fd_write(child_in_fd,state.line,state.line_length,0);
				if (!ret||SYS_IS_ERROR(ret)){
					goto _error;
				}
				readline_state_reset(&state);
			}
			if (ctrl_echo_input){
				sys_error_t ret=sys_fd_write(out_fd,buffer,length,0);
				if (!ret||SYS_IS_ERROR(ret)){
					goto _error;
				}
			}
			sys_error_t ret=sys_fd_write(child_in_fd,buffer,length,0);
			if (!ret||SYS_IS_ERROR(ret)){
				goto _error;
			}
			continue;
		}
		state.echo_input=ctrl_echo_input;
		const char* ptr=(void*)buffer;
		while (length){
			u64 count=readline_process(&state,ptr,length);
			ptr+=count;
			length-=count;
			if (state.event==READLINE_EVENT_CANCEL){
				sys_signal_dispatch(sys_process_group_get(0),SYS_SIGNAL_INTERRUPT);
			}
			else if (state.event==READLINE_EVENT_LINE){
				sys_error_t ret=sys_fd_write(child_in_fd,state.line,state.line_length,0);
				if (!ret||SYS_IS_ERROR(ret)){
					goto _error;
				}
			}
		}
	}
_error:
	sys_pipe_close(child_in_fd);
	readline_state_deinit(&state);
}



static void _output_thread(void* ctx){
	while (1){
		if (SYS_IS_ERROR(sys_fd_stream(child_out_fd,&out_fd,1,0))){
			sys_pipe_close(child_out_fd);
			break;
		}
	}
}



static u32 _control_flag_update_callback(u32 clear,u32 set,u32 all){
	if (clear&TERMINAL_FLAG_DIRECT_IO){
		ctrl_direct_io=0;
	}
	else if (set&TERMINAL_FLAG_DIRECT_IO){
		ctrl_direct_io=1;
	}
	if (clear&TERMINAL_FLAG_ECHO_INPUT){
		ctrl_echo_input=0;
	}
	else if (set&TERMINAL_FLAG_ECHO_INPUT){
		ctrl_echo_input=1;
	}
	if (clear&TERMINAL_FLAG_DISABLE_AUTOCOMPLETE){
		ctrl_autocomplete_enabled=1;
	}
	else if (set&TERMINAL_FLAG_DISABLE_AUTOCOMPLETE){
		ctrl_autocomplete_enabled=0;
	}
	return 0;
}



static void _control_size_inquiry_callback(u32* out){
	out[0]=ctrl_terminal_size[0];
	out[1]=ctrl_terminal_size[1];
}



static void _control_thread(void* ctx){
	terminal_session_t session;
	if (!terminal_session_open_from_fd(ctrl_fd,&session)){
		sys_fd_close(ctrl_fd);
		return;
	}
	terminal_server_state_t state={
		.flags=TERMINAL_FLAG_ECHO_INPUT,
		.flag_update_callback=_control_flag_update_callback,
		.size_inquiry_callback=_control_size_inquiry_callback
	};
	while (terminal_server_process_packet(&session,&state));
	terminal_session_close(&session);
}



s64 main(u32 argc,const char*const* argv){
	const char* in="/dev/ser/in";
	const char* out="/dev/ser/out";
	bool shutdown_after_process=0;
	bool probe_size=0;
	u32 first_argument_index=0;
	if (!sys_options_parse(argc,argv,"{i:input}s{o:output}s{s:shutdown-on-close}y{p:probe-size}y{-}!a",&in,&out,&shutdown_after_process,&probe_size,&first_argument_index)){
		return 1;
	}
	in_fd=sys_fd_open(0,in,SYS_FD_FLAG_READ);
	out_fd=sys_fd_open(0,out,SYS_FD_FLAG_WRITE|SYS_FD_FLAG_APPEND);
	if (SYS_IS_ERROR(in_fd)||SYS_IS_ERROR(out_fd)){
		return 1;
	}
	if (probe_size){
		_probe_terminal_size(in_fd,out_fd);
	}
	sys_fd_lock(in_fd,sys_process_get_handle());
	sys_fd_lock(out_fd,sys_process_get_handle());
	sys_fd_t child_pipes[3];
	u64 devfs_syscall_table_offset=sys_syscall_get_table_offset("devfs");
	if (SYS_IS_ERROR(devfs_syscall_table_offset)||SYS_IS_ERROR(_sys_syscall1(devfs_syscall_table_offset|0x00000001,(u64)child_pipes))){
		child_pipes[0]=sys_pipe_create(NULL);
		child_pipes[1]=sys_pipe_create(NULL);
		child_pipes[2]=0;
	}
	child_in_fd=sys_fd_dup(child_pipes[0],SYS_FD_FLAG_WRITE);
	sys_fd_t stdin=sys_fd_dup(child_pipes[0],SYS_FD_FLAG_READ);
	child_out_fd=sys_fd_dup(child_pipes[1],SYS_FD_FLAG_READ);
	sys_fd_t stdout_stderr=sys_fd_dup(child_pipes[1],SYS_FD_FLAG_WRITE);
	sys_fd_close(child_pipes[0]);
	sys_fd_close(child_pipes[1]);
	sys_thread_t thread=sys_thread_create(_input_thread,NULL,NULL);
	sys_thread_set_priority(thread,SYS_THREAD_PRIORITY_REALTIME);
	sys_thread_set_name(thread,"serial_terminal.input");
	thread=sys_thread_create(_output_thread,NULL,NULL);
	sys_thread_set_name(thread,"serial_terminal.output");
	sys_thread_set_priority(thread,SYS_THREAD_PRIORITY_REALTIME);
	if (child_pipes[2]){
		ctrl_fd=child_pipes[2];
		thread=sys_thread_create(_control_thread,NULL,NULL);
		sys_thread_set_name(thread,"serial_terminal.control");
		sys_thread_set_priority(thread,SYS_THREAD_PRIORITY_HIGH);
	}
	sys_signal_set_mask(1<<SYS_SIGNAL_INTERRUPT,1);
	sys_process_t process=sys_process_start(argv[first_argument_index],argc-first_argument_index,argv+first_argument_index,NULL,0,stdin,stdout_stderr,stdout_stderr);
	sys_thread_set_priority(sys_thread_get_handle(),SYS_THREAD_PRIORITY_LOW);
	sys_thread_await_event(sys_process_get_termination_event(process));
	sys_fd_close(in_fd);
	sys_fd_close(child_in_fd);
	sys_fd_close(out_fd);
	sys_fd_close(child_out_fd);
	sys_fd_close(ctrl_fd);
	if (shutdown_after_process){
		sys_system_shutdown(0);
	}
	return 0;
}
