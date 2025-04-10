#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/socket.h>
#include<pthread.h>
#define BUFSIZE 100
#define NAME_SIZE 50
void *send_msg(void *arg);
void *recv_msg(void *arg);
void errorhandle(char *msg);
char name[NAME_SIZE]="Default";
char msg[BUFSIZE];

int main(int argc,char *argv[])
{
    pthread_t send,recv;
    int sock;
    struct sockaddr_in serv_adr;
    void *th_return;
    if(argc!=4){
        printf("Usage:%s <IP> <port> <name>\n",argv[0]);
        exit(1);
    }

    sprintf(name,"[%s]",argv[3]);
    sock=socket(AF_INET,SOCK_STREAM,0);

    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    inet_pton(AF_INET,argv[1],&serv_adr.sin_addr);
    serv_adr.sin_port=htons(atoi(argv[2]));

    if(connect(sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1)
        errorhandle("connect() error!");
    
    pthread_create(&send,NULL,send_msg,(void*)&sock);
    pthread_create(&recv,NULL,recv_msg,(void*)&sock);
    pthread_join(send,&th_return);
    pthread_join(recv,&th_return);
    close(sock);
    return 0;
}
void *send_msg(void *arg){
    int sock=*((int*)arg);
    char name_msg[BUFSIZE+NAME_SIZE];
    while(1){
        fgets(msg,BUFSIZE,stdin);
        if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n")){
            close(sock);
            exit(1);
        }
        sprintf(name_msg,"%s %s",name,msg);
        write(sock,name_msg,strlen(name_msg));
    }
    return NULL;
}
void *recv_msg(void *arg){
    int sock=*((int*)arg);
    char name_msg[BUFSIZE+NAME_SIZE];
    int str_len;
    while (1)
    {
        str_len=read(sock,name_msg,NAME_SIZE+BUFSIZE-1);
        if(str_len==-1)
            return (void*)-1;
        name_msg[str_len]=0;
        fputs(name_msg,stdout);
    }
    return NULL;
}
void errorhandle(char *msg){
    fputs(msg,stderr);
    fputs("\n",stderr);
    exit(1);
}