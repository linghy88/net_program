#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#define BUFSIZE 30

void Errorhandle(char *message);

int main(int argc,char *argv[])
{
    struct sockaddr_in adr;
    struct ip_mreq join_adr;
    char buf[BUFSIZE];
    int strlen;
    int recv_sock;
    if(argc!=3){
        printf("Usage:%s<IP> <PORT>\n",argv[0]);
        exit(1);
    }

    recv_sock=socket(AF_INET,SOCK_DGRAM,0);
    memset(&adr,0,sizeof(adr));
    adr.sin_family=AF_INET;
    adr.sin_addr.s_addr=htonl(INADDR_ANY);
    adr.sin_port=htons(atoi(argv[2]));

    if(bind(recv_sock,(struct sockaddr*)&adr,sizeof(adr))==-1)//接收数据需要绑定端口
        Errorhandle("bind() error!");
    
    inet_pton(AF_INET,argv[1],&join_adr.imr_multiaddr);
    //join_adr.imr_multiaddr.s_addr=inet_addr(argv[1]);
    join_adr.imr_interface.s_addr=htonl(INADDR_ANY);

    setsockopt(recv_sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,(void*)&join_adr,sizeof(join_adr));
    //加入多播组

    while(1){
        strlen=recvfrom(recv_sock,buf,BUFSIZE-1,0,0,0);
        if(strlen<0)
            break;
        buf[strlen]=0;
        fputs(buf,stdout);
    }
    close(recv_sock);
    return 0;
}

void Errorhandle(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}