#include <stdio.h>
#include <string.h>
#include <unistd.h>
void show_handler(int sig){
      printf("I get %d Signal!!!\n",sig);
      int i;
      for(int i=0;i<5;++i){
      	printf("i = %d\n",i);
      	sleep(1);
      }

}
int main(void ){
     int i=0;
   struct  sigaction act ,oldact;
   act.sa_handler = show_handler;
   sigaddset(&act.sa_mask,SIGQUIT);
   act.sa_flags = SA_RESETHAND|SA_NODEFER;
    sigaction(SIGINT,&act,&oldact);
    while(true){
    	sleep(1);
    	printf("sleeping %d\n",i);
    	i++;
    }
	return 0;
}