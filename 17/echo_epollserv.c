#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>

#define BUFSIZE 4
#define EPOLLSZ 50

int main(int argc,char *argv[])
{
    char buf[BUFSIZE];
    struct sockaddr_in serv_addr,clnt_addr;
    int sock_serv,sock_clnt;
    int strlen,adr_sz;

    int epfd,epoll_cnt;
    struct epoll_event *ep_events;//结构体指针
    struct epoll_event event;//结构体
    if(argc!=2){
        printf("Usage:%s <port>\n",argv[0]);
        exit(1);
    }

    sock_serv=socket(AF_INET,SOCK_STREAM,0);
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(sock_serv,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
        puts("bind() error!");
        exit(1);
    }
    if(listen(sock_serv,5)==-1){
        puts("listen() error!");
        exit(1);
    }
    epfd=epoll_create(EPOLLSZ);//创建例程
    ep_events=malloc(sizeof(struct epoll_event)*EPOLLSZ);//分配例程空间
    event.events=EPOLLIN;//事件为例程可读
    event.data.fd=sock_serv;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sock_serv,&event);//op：增加事件，传入event结构体地址

    while(1){
        epoll_cnt=epoll_wait(epfd,ep_events,EPOLLSZ,-1);//执行事件监听
        if(epoll_cnt==-1){
            puts("epoll_wait() error!");
            exit(1);
        }
        puts("return epoll_wait");
        for(int i=0;i<epoll_cnt;++i){
            if(ep_events[i].data.fd==sock_serv){
                adr_sz=sizeof(clnt_addr);
                sock_clnt=accept(sock_serv,(struct sockaddr*)&clnt_addr,&adr_sz);
                event.events=EPOLLIN|EPOLLET;
                event.data.fd=sock_clnt;
                epoll_ctl(epfd,EPOLL_CTL_ADD,sock_clnt,&event);
                printf("connected client:%d \n",sock_clnt);
            }
            else{
                strlen=read(ep_events[i].data.fd,buf,BUFSIZE);
                if(strlen==0){
                    epoll_ctl(epfd,EPOLL_CTL_DEL,ep_events[i].data.fd,NULL);
                    close(ep_events[i].data.fd);
                    printf("close client:%d\n",ep_events[i].data.fd);
                }
                else{
                    write(ep_events[i].data.fd,buf,strlen);
                }
            }
        }
    }
    close(sock_serv);
    close(epfd);
    free(ep_events);
    return 0;
}
