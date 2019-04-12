
#include <signal.h>

/**
 * The shared data
 */

 int start_stop = 3;
void handler(int signum){
	if(signum = SIGUSR1){
		start_stop = 1;
		while(start_stop = 1){
			// wait
		}
	}
	if(signum = SIGUSR2){
		start_stop = 2;
		while(start_stop = 2){
			//wait
		}
	}
}
int size;
