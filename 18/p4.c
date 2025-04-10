#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

#define P_NUM 100
void *pthread_add(void *arg);
void *pthread_sub(void *arg);
long long num=0;
pthread_mutex_t mt;

int main(int argc,char *agrv[])
{
    
    pthread_t tid[P_NUM];
    pthread_mutex_init(&mt,NULL);
    printf("num size:%ld\n",sizeof(long long));
    for(int i=0;i<P_NUM;++i){
        if(i%2==0)
            pthread_create(&(tid[i]),NULL,pthread_add,NULL);
        else
            pthread_create(&(tid[i]),NULL,pthread_sub,NULL);
    }
   
    for(int j=0;j<P_NUM;++j){
        pthread_join(tid[j],NULL);
    }
    printf("num:%lld\n",num);
    pthread_mutex_destroy(&mt);
    return 0;
}

void *pthread_add(void *arg){
    pthread_mutex_lock(&mt);
    for(int i=0;i<5000;++i)
        num+=1;
    pthread_mutex_unlock(&mt);
    
    return NULL;
}

void *pthread_sub(void *arg){
    pthread_mutex_lock(&mt);
    for(int j=0;j<5000;++j)
        num-=1;
    pthread_mutex_unlock(&mt);
    
    return NULL;
}