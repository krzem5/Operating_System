#include <kernel/acl/acl.h>
#include <kernel/cpu/cpu.h>
#include <kernel/error/error.h>
#include <kernel/format/format.h>
#include <kernel/fpu/fpu.h>
#include <kernel/handle/handle.h>
#include <kernel/lock/profiling.h>
#include <kernel/lock/spinlock.h>
#include <kernel/log/log.h>
#include <kernel/memory/amm.h>
#include <kernel/memory/omm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/smm.h>
#include <kernel/mmap/mmap.h>
#include <kernel/mp/event.h>
#include <kernel/mp/process.h>
#include <kernel/mp/thread.h>
#include <kernel/mp/thread_list.h>
#include <kernel/scheduler/load_balancer.h>
#include <kernel/scheduler/scheduler.h>
#include <kernel/syscall/syscall.h>
#include <kernel/types.h>
#include <kernel/util/util.h>
#define KERNEL_LOG_NAME "thread"



#define KERNEL_THREAD_STACK_SIZE 0x200000

#define SET_KERNEL_THREAD_ARG(register) \
	if ((arg_count)){ \
		(arg_count)--; \
		out->reg_state.gpr_state.register=__builtin_va_arg(va,u64); \
	}



static omm_allocator_t* _thread_allocator=NULL;
static omm_allocator_t* _thread_fpu_state_allocator=NULL;

KERNEL_PUBLIC handle_type_t thread_handle_type=0;



static void _thread_handle_destructor(handle_t* handle){
	thread_t* thread=handle->object;
	if (thread->state!=THREAD_STATE_TYPE_TERMINATED){
		panic("Unterminated thread not referenced");
	}
	process_t* process=thread->process;
	smm_dealloc(thread->name);
	thread->name=NULL;
	mmap_dealloc_region(process_kernel->mmap,thread->kernel_stack_region);
	mmap_dealloc_region(process_kernel->mmap,thread->pf_stack_region);
	omm_dealloc(_thread_fpu_state_allocator,thread->reg_state.fpu_state);
	if (thread_list_remove(&(process->thread_list),thread)){
		event_dispatch(process->event,EVENT_DISPATCH_FLAG_DISPATCH_ALL|EVENT_DISPATCH_FLAG_SET_ACTIVE|EVENT_DISPATCH_FLAG_BYPASS_ACL);
		handle_release(&(process->handle));
	}
	omm_dealloc(_thread_allocator,thread);
}



static thread_t* _thread_alloc(process_t* process){
	if (!_thread_fpu_state_allocator){
		_thread_fpu_state_allocator=omm_init("fpu_state",fpu_state_size,64,4,pmm_alloc_counter("omm_thread_fpu_state"));
		spinlock_init(&(_thread_fpu_state_allocator->lock));
	}
	thread_t* out=omm_alloc(_thread_allocator);
	mem_fill(out,0,sizeof(thread_t));
	out->header.current_thread=out;
	handle_new(out,thread_handle_type,&(out->handle));
	out->handle.acl=acl_create();
	acl_set(out->handle.acl,process,0,THREAD_STATE_TYPE_TERMINATED);
	spinlock_init(&(out->lock));
	out->process=process;
	char buffer[128];
	out->name=smm_alloc(buffer,format_string(buffer,128,"%s-thread-%u",process->name->data,HANDLE_ID_GET_INDEX(out->handle.rb_node.key)));
	out->kernel_stack_region=mmap_alloc(process_kernel->mmap,0,KERNEL_THREAD_STACK_SIZE,MMAP_REGION_FLAG_STACK|MMAP_REGION_FLAG_VMM_WRITE,NULL);
	if (!out->kernel_stack_region){
		panic("Unable to reserve thread stack");
	}
	out->pf_stack_region=mmap_alloc(process_kernel->mmap,0,CPU_PAGE_FAULT_STACK_PAGE_COUNT<<PAGE_SIZE_SHIFT,MMAP_REGION_FLAG_STACK|MMAP_REGION_FLAG_COMMIT|MMAP_REGION_FLAG_VMM_WRITE,NULL);
	if (!out->pf_stack_region){
		panic("Unable to reserve thread stack");
	}
	out->reg_state.reg_state_not_present=0;
	out->reg_state.fpu_state=omm_alloc(_thread_fpu_state_allocator);
	fpu_init(out->reg_state.fpu_state);
	out->priority=SCHEDULER_PRIORITY_NORMAL;
	out->state=THREAD_STATE_TYPE_NONE;
	out->event_sequence_id=0;
	out->scheduler_load_balancer_queue_index=0;
	out->scheduler_early_yield=0;
	out->scheduler_io_yield=0;
#ifndef KERNEL_RELEASE
	__lock_profiling_init_thread_data(&(out->__lock_profiling_data));
#endif
	thread_list_add(&(process->thread_list),out);
	return out;
}



KERNEL_EARLY_INIT(){
	LOG("Initializing thread allocator...");
	_thread_allocator=omm_init("thread",sizeof(thread_t),8,4,pmm_alloc_counter("omm_thread"));
	spinlock_init(&(_thread_allocator->lock));
	thread_handle_type=handle_alloc("thread",_thread_handle_destructor);
}



KERNEL_PUBLIC thread_t* thread_create_user_thread(process_t* process,u64 rip,u64 rsp){
	thread_t* out=_thread_alloc(process);
	out->header.kernel_rsp=out->kernel_stack_region->rb_node.key+KERNEL_THREAD_STACK_SIZE;
	out->reg_state.gpr_state.rip=rip;
	out->reg_state.gpr_state.rsp=rsp;
	out->reg_state.gpr_state.cs=0x23;
	out->reg_state.gpr_state.ds=0x1b;
	out->reg_state.gpr_state.es=0x1b;
	out->reg_state.gpr_state.ss=0x1b;
	out->reg_state.gpr_state.rflags=0x0000000202;
	out->reg_state.fs_gs_state.fs=0;
	out->reg_state.fs_gs_state.gs=0;
	handle_finish_setup(&(out->handle));
	return out;
}



KERNEL_PUBLIC thread_t* thread_create_kernel_thread(process_t* process,const char* name,void* func,u8 arg_count,...){
	if (arg_count>6){
		panic("Too many kernel thread arguments");
	}
	_Bool start_thread=!process;
	thread_t* out=_thread_alloc((process?process:process_kernel));
	if (name){
		smm_dealloc(out->name);
		out->name=smm_alloc(name,0);
	}
	out->reg_state.gpr_state.rip=(u64)_thread_bootstrap_kernel_thread;
	out->reg_state.gpr_state.rax=(u64)func;
	out->reg_state.gpr_state.rsp=out->kernel_stack_region->rb_node.key+KERNEL_THREAD_STACK_SIZE;
	__builtin_va_list va;
	__builtin_va_start(va,arg_count);
	SET_KERNEL_THREAD_ARG(rdi);
	SET_KERNEL_THREAD_ARG(rsi);
	SET_KERNEL_THREAD_ARG(rdx);
	SET_KERNEL_THREAD_ARG(rcx);
	SET_KERNEL_THREAD_ARG(r8);
	SET_KERNEL_THREAD_ARG(r9);
	__builtin_va_end(va);
	out->reg_state.gpr_state.cs=0x08;
	out->reg_state.gpr_state.ds=0x10;
	out->reg_state.gpr_state.es=0x10;
	out->reg_state.gpr_state.ss=0x10;
	out->reg_state.gpr_state.rflags=0x0000000202;
	out->reg_state.fs_gs_state.fs=0;
	out->reg_state.fs_gs_state.gs=(u64)out;
	handle_finish_setup(&(out->handle));
	if (start_thread){
		scheduler_enqueue_thread(out);
	}
	return out;
}



KERNEL_PUBLIC void thread_delete(thread_t* thread){
	handle_release(&(thread->handle));
	// spinlock_acquire_exclusive(&(thread->lock));
	// if (handle_release(&(thread->handle))){
	// 	spinlock_release_exclusive(&(thread->lock));
	// }
}



KERNEL_PUBLIC void KERNEL_NORETURN thread_terminate(void){
	scheduler_pause();
	thread_t* thread=CPU_HEADER_DATA->current_thread;
	spinlock_acquire_exclusive(&(thread->lock));
	thread->state=THREAD_STATE_TYPE_TERMINATED;
	spinlock_release_exclusive(&(thread->lock));
	scheduler_yield();
	for (;;);
}



error_t syscall_thread_get_tid(void){
	return THREAD_DATA->handle.rb_node.key;
}



error_t syscall_thread_create(u64 rip,u64 rdi,u64 rsi,u64 rdx,u64 rsp){
	if (!syscall_get_user_pointer_max_length((void*)rip)){
		return ERROR_INVALID_ARGUMENT(0);
	}
	thread_t* thread=thread_create_user_thread(THREAD_DATA->process,rip,rsp);
	thread->reg_state.gpr_state.rdi=rdi;
	thread->reg_state.gpr_state.rsi=rsi;
	thread->reg_state.gpr_state.rdx=rdx;
	scheduler_enqueue_thread(thread);
	return thread->handle.rb_node.key;
}



error_t syscall_thread_stop(handle_id_t thread_handle){
	if (thread_handle){
		panic("syscall_thread_stop: stop other thread");
	}
	thread_terminate();
}



error_t syscall_thread_get_priority(handle_id_t thread_handle){
	handle_t* handle=handle_lookup_and_acquire(thread_handle,thread_handle_type);
	if (!handle){
		return ERROR_INVALID_HANDLE;
	}
	u64 out=((thread_t*)(handle->object))->priority;
	handle_release(handle);
	return out;
}



error_t syscall_thread_set_priority(handle_id_t thread_handle,u64 priority){
	if (priority<SCHEDULER_PRIORITY_MIN||priority>SCHEDULER_PRIORITY_MAX){
		return ERROR_INVALID_ARGUMENT(1);
	}
	handle_t* handle=handle_lookup_and_acquire(thread_handle,thread_handle_type);
	if (!handle){
		return ERROR_INVALID_HANDLE;
	}
	thread_t* thread=handle->object;
	if (thread->state==THREAD_STATE_TYPE_TERMINATED){
		handle_release(handle);
		return ERROR_UNSUPPORTED_OPERATION;
	}
	thread->priority=priority;
	handle_release(handle);
	return ERROR_OK;
}



error_t syscall_thread_await_events(KERNEL_USER_POINTER const void* events,u64 event_count){
	if (!event_count){
		return ERROR_INVALID_ARGUMENT(1);
	}
	if (event_count*sizeof(handle_id_t)>syscall_get_user_pointer_max_length((const void*)events)){
		return ERROR_INVALID_ARGUMENT(0);
	}
	handle_id_t* buffer=amm_alloc(event_count*sizeof(handle_id_t));
	mem_copy(buffer,(const void*)events,event_count*sizeof(handle_id_t));
	u32 out=event_await_multiple_handles(buffer,event_count);
	amm_dealloc(buffer);
	return out;
}
