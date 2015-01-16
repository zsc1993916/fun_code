#include <stdio.h>
#include <pthread.h>

void thread(void){
    printf("hello thread!\n");
}

int main(){
    pthread_t id;
    /*
       创建一个线程

         第一个参数 为指向线程标识符等指针
         第二个通常位NULL
         第三个为线程运行函数的起始地址
         第四个位运行函数等参数
    */
    int ret = pthread_create(&id, NULL, (void *)thread, NULL);
    if(0 != ret) {
        printf("Create pthread error!\n");
        return 0;
    }
    printf("this is main thread\n");


    /*
       等待一个线程结束， 触发一个函数

       第一个为等待结束的线程标识符
       第二个为执行函数
    */
    pthread_join(id, NULL);

    return 0;
}
