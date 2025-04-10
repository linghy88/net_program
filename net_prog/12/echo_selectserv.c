#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<sys/socket.h>
#include<sys/select.h>
#define BUFSIZE 100

void Error_handle(char *message);

int main(int argc,char *argv[])
{
    int sock_serv,sock_clnt;
    struct sockaddr_in serv_addr,clnt_addr;
    char buf[BUFSIZ];
    struct timeval timeout;//selectos超时结构体
    fd_set reads,cpy_read;//需要监听的套接字数组
    socklen_t adr_sz;
    int str_len;
    int fd_max,fd_num;

    if(argc!=2){
        printf("Usage:%s <port>\n",argv[0]);
        exit(1);
    }

    sock_serv=socket(AF_INET,SOCK_STREAM,0);
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(sock_serv,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
        Error_handle("bind() error!");
    if(listen(sock_serv,3)==-1)
        Error_handle("listen() error!");
    
    FD_ZERO(&reads);//全部置零
    FD_SET(sock_serv,&reads);//将读数组的套接字置为1
    fd_max=sock_serv;

    while(1){
        cpy_read=reads;//需要复制一份
        timeout.tv_sec=5;
        timeout.tv_usec=5000;
        if((fd_num=select(fd_max+1,&cpy_read,0,0,&timeout))==-1){//失败
            Error_handle("select() error!");
            break;
        }
            
        if(fd_num==0)//超时
            continue;

        for(int i=0;i<fd_max+1;i++){//遍历所有发生的事件
            if(FD_ISSET(i,&cpy_read)){
                if(i==sock_serv){//建立连接
                    adr_sz=sizeof(clnt_addr);
                    sock_clnt=accept(sock_serv,(struct sockaddr*)&clnt_addr,&adr_sz);
                    FD_SET(sock_clnt,&reads);
                    if(fd_max<sock_clnt)
                        fd_max=sock_clnt;
                    printf("connected client:%d \n",sock_clnt);
                }
                else {//不是服务器套接字
                    str_len=read(i,buf,BUFSIZE);
                    if(str_len==0){
                        FD_CLR(i,&reads);
                        close(i);
                        printf("close client:%d \n",i);
                    }
                    else{
                        write(i,buf,str_len);//回声，发回去
                    }
                }
            }
        }
        
        
    }
    close(sock_serv);
    return 0;
}

void Error_handle(char *message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}
