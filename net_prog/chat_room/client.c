#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#define BUFSIZE 1024

int sock_clnt;

void *recv_msg(void *arg){
    char buf[BUFSIZE];
    
    while(1){
        int receive=recv(sock_clnt,buf,BUFSIZE-1,0);
        if(receive<=0){
            perror("与服务求断开连接");
            exit(1);
        }
        buf[receive]='\0';
        printf("%s\n",buf);
    }
}

int main(int argc,char *argv[])
{
    char name[20];
    char buf[BUFSIZE];
    struct sockaddr_in clnt_adr;
    
    if(argc!=3){
        printf("Usage: %s <IP> <PORT>",argv[0]);
        exit(1);
    }

    sock_clnt=socket(AF_INET,SOCK_STREAM,0);
    clnt_adr.sin_family=AF_INET;
    clnt_adr.sin_port=htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&clnt_adr.sin_addr);

    if(connect(sock_clnt,(struct sockaddr*)&clnt_adr,sizeof(clnt_adr))==-1){
        perror("connect() error");
        exit(1);
    }
        
    fputs("请输入name:",stdout);
    fgets(name,sizeof(name),stdin);
    name[strcspn(name, "\n")] = '\0'; // 移除换行符，strcspn(name, "\n")找到\n的位置，并替换为\0
    //name[strlen(name)-1]='\0';
    if(send(sock_clnt,name,strlen(name),0)<0){
        perror("发送name失败\n");
        exit(1);
    }
    
    printf("——————welcome to join chatroom!——————\n");
    pthread_t tid;
    if(pthread_create(&tid,NULL,&recv_msg,NULL)!=0){
        perror("创建线程失败\n");
        exit(1);
    
    //pthread_join(tid,NULL);
    }

    
    fputs("请输入内容:\n",stdout);
    //发送消息
    while(1){
        fgets(buf,BUFSIZE,stdin);
        //buf[strcspn(buf, "\n")] = '\0'; // 移除换行符
        if(!strcmp(buf,"exit\n") || !strcmp(buf,"EXIT\n")){
            printf("退出聊天室\n");
            close(sock_clnt);
            exit(0);
        }

        if(strlen(buf)>0)
            if(send(sock_clnt,buf,strlen(buf),0)<0){
                perror("发送失败");
                break;
            }

    }

    close(sock_clnt);
    return 0;
}