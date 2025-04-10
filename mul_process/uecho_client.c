#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#define BUF_SIZE 30

void Error_handle(char *message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

int main(int argc,char *argv[])
{
    int sock_serv;
    int str_len;
    socklen_t adr_sz;
    char message[BUF_SIZE];

    struct sockaddr_in serv_addr,from_addr;
    if(argc!=3)
    {
        printf("Usage :%s <IP>  <port> \n",argv[0]);
    }

    if((sock_serv=socket(AF_INET,SOCK_STREAM,0))==-1)
        Error_handle("socket() error!");

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock_serv,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
        Error_handle("connect() error!");

     while (1)
     {
        fputs("Insert message('q' or 'Q'to quit):",stdout);
        fgets(message,sizeof(message),stdin);
        if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
            break;

        write(sock_serv,message,sizeof(message));
        read(sock_serv,message,BUF_SIZE);
        printf("message from server:%s\n",message);
     }
     close(sock_serv);
    return 0;
}