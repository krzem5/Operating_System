#include <command.h>
#include <cwd.h>
#include <input.h>
#include <sys/clock.h>
#include <sys/io.h>



void main(void){
	cwd_init();
	clock_init();
	while (1){
		input_get();
		command_execute(input);
	}
}
