#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#define BUFSIZE 30
#define clnt_max 256
void errorhandle(char *msg);
void *send_msg(char *msg,int len);
void *clnt_handle(void *arg);

int clnt_cnt=0;
pthread_mutex_t mutex;
int clnt_socks[clnt_max];

int main(int argc,char *argv[])
{
    struct sockaddr_in serv_adr,clnt_adr;
    int serv_sock,clnt_sock;
    pthread_t tid;

    if(argc!=2){
        printf("Usage:%s <port>\n",argv[0]);
        exit(1);
    }

    serv_sock=socket(AF_INET,SOCK_STREAM,0);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1){
        errorhandle("bind() error!");
    }
    if(listen(serv_sock,5)==-1){
        errorhandle("listen() error!");
    }

    while(1){
        int clnt_sz=sizeof(clnt_adr);
        if((clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_sz))==-1)
            errorhandle("accept() error!");
        
        pthread_mutex_lock(&mutex);//上锁
        clnt_socks[clnt_cnt++]=clnt_sock;
        pthread_mutex_unlock(&mutex);

        pthread_create(&tid,NULL,clnt_handle,(void*)&clnt_sock);
        pthread_detach(tid);
        //pthread_join(tid,NULL);
        printf("Conneted client IP:%s \n",inet_ntoa(clnt_adr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

void *clnt_handle(void *arg){
    int clnt_sock=*((int*)arg);
    int str_len;
    char msg[BUFSIZE];

    while((str_len=read(clnt_sock,msg,sizeof(msg)))!=0)//读取消息来自客户端
        send_msg(msg,str_len);//发送给每个客户端

    pthread_mutex_lock(&mutex);//上锁 
    for(int i=0;i<clnt_cnt;++i){//线程结束时，就会把断开连接的客户端从客户数组删除
        if(clnt_sock==clnt_socks[i]){
            while(i++<clnt_cnt-1)
                clnt_socks[i]=clnt_socks[i+1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutex);//解锁
    close(clnt_sock);
    return NULL;
}

void *send_msg(char *msg,int len){
    pthread_mutex_lock(&mutex);
    for(int i=0;i<clnt_cnt;++i)
        write(clnt_socks[i],msg,len);
    pthread_mutex_unlock(&mutex);
}

void errorhandle(char *msg){
    fputs(msg,stderr);
    fputs("\n",stderr);
    exit(1);
}