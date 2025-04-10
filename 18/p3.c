#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
void *pthread_main(void * arg);
int sum=0;
int main(int argc,char *agrv[])
{
    pthread_t tid1,tid2;//typedef unsigned long int pthread_t;
    int range1[]={1,5};
    int range2[]={6,10};
    pthread_create(&tid1,NULL,pthread_main,(void*)range1);
    pthread_create(&tid2,NULL,pthread_main,(void*)range2);
    
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);//thr_ret记得free
    printf("%d\n",sum);
    return 0;
}

void *pthread_main(void *arg)
{
    int start=((int *)arg)[0];
    int end=((int*)arg)[1];
    for(;start<=end;++start){
        sum+=start;
    }

}