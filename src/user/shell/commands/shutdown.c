#include <command.h>
#include <user/shutdown.h>



void shutdown_main(int argc,const char*const* argv){
	shutdown(0);
}



void reboot_main(int argc,const char*const* argv){
	shutdown(1);
}



void exit_main(int argc,const char*const* argv){
	shutdown(0);
}



DECLARE_COMMAND(shutdown);
DECLARE_COMMAND(reboot);
DECLARE_COMMAND(exit);
