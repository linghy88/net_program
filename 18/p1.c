#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
void *pthread_main(void * arg);

int main(int argc,char *agrv[])
{
    pthread_t t_id;//typedef unsigned long int pthread_t;
    int thred_param=5;
    void *thr_ret;//线程的返回值
    if(pthread_create(&t_id,NULL,pthread_main,(void*)&thred_param)!=0){
        perror("pthread_creat error!");
        exit(1);
    }
    if(pthread_join(t_id,&thr_ret)!=0){//thr_ret记得free
        perror("pthread_join() error!");
        exit(1);
    }
    printf("pthread return message:%s\n",(char*)thr_ret);
    free(thr_ret);
    return 0;
}

void *pthread_main(void *arg)
{
    int i;
    int cnt=*(int *)arg;//强制转化
    char *msg=(char*)malloc(sizeof(char)*50);
    strcpy(msg,"hello world!");
    for(i=0;i<cnt;++i){
        sleep(1);
        puts("running thread!");  
    }
    return (void *)msg;
}