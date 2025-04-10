#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

void *read(void *arg);
void *accu(void *arg);
int num=0;
static sem_t sem_one,sem_two;

int main()
{
    pthread_t t1,t2;
    sem_init(&sem_one,0,0);
    sem_init(&sem_two,0,1);
    
    pthread_create(&t1,NULL,read,NULL);
    pthread_create(&t2,NULL,accu,NULL);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    sem_destroy(&sem_one);
    sem_destroy(&sem_two);
    return 0;
}

void *read(void *arg){
    for(int i=0;i<5;++i){
        sem_wait(&sem_two);
        fputs("input:",stdout);
        scanf("%d",&num);
        sem_post(&sem_one);
    }
}
void *accu(void *arg){
    int sum=0;
    for(int i=0;i<5;++i){
        sem_wait(&sem_one);
        sum+=num;
        sem_post(&sem_two);
        printf("sum:%d\n",sum);
    }
}