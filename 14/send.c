#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#define TTL 64
#define BUFSIZE 30

void Errorhandle(char *message);

int main(int argc,char *argv[])
{
    int time_live=TTL;
    int sock_send;
    char buf[BUFSIZE];
    struct sockaddr_in mul_adr;
    FILE *fp;

    if(argc!=3){
        printf("Usage:%s <IP> <PORT>\n",argv[0]);
        exit(1);
    }

    sock_send=socket(AF_INET,SOCK_DGRAM,0);
    memset(&mul_adr,0,sizeof(mul_adr));
    mul_adr.sin_family=AF_INET;
    inet_pton(AF_INET,argv[1],&mul_adr.sin_addr);//argv[1]:是指针，mul_adr.sin_addr是结构体变量，mul_adr.sin_addr.s_addr是u32t
    mul_adr.sin_port=htons(atoi(argv[2]));

    setsockopt(sock_send,IPPROTO_IP,IP_MULTICAST_TTL,(void*)&time_live,sizeof(time_live));
    if((fp=fopen("new.txt","r"))==NULL)
        Errorhandle("fopen() error!");
    
    while(fgets(buf,BUFSIZE-1,fp)!=NULL){
        fputs(buf,stdout);
        sendto(sock_send,buf,sizeof(buf),0,(struct sockaddr*)&mul_adr,sizeof(mul_adr));
        sleep(3);
    }
    
    

    fclose(fp);
    shutdown(sock_send,SHUT_WR);
    sleep(2);
    close(sock_send);
    return 0;
}

void Errorhandle(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}