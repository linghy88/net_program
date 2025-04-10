#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<signal.h>
#define BUFSIZE 30

void Error_handle(char *message);
void read_childproc(int signo);

int main(int argc,char *argv[])
{
    int sock_serv,sock_clnt;
    struct sockaddr_in serv_addr,clnt_addr;
    char buf[BUFSIZ];
    struct sigaction act;
    socklen_t adr_sz;
    pid_t pid;
    int str_len,state;
    
    if(argc!=2){
        printf("Usage:%s <port>\n",argv[0]);
        exit(1);
    }

    act.sa_handler=read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    state=sigaction(SIGCHLD,&act,0);

    sock_serv=socket(AF_INET,SOCK_STREAM,0);
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(sock_serv,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
        Error_handle("bind() error!");
    if(listen(sock_serv,3)==-1)
        Error_handle("listen() error!");
    
    while(1){
        adr_sz=sizeof(clnt_addr);
        sock_clnt=accept(sock_serv,(struct sockaddr*)&clnt_addr,&adr_sz);
        if(sock_clnt==-1){
            Error_handle("accept() error!");
            continue;    
        }
        else
            puts("new client connected________");
        
        pid=fork();
        if(pid==-1){
            close(sock_clnt);
            continue;
        }
        else if(pid==0){
            close(sock_serv);//关闭服务器套接字，用子进程套接字通信
            while((str_len=read(sock_clnt,buf,BUFSIZ))!=0){
                write(sock_clnt,buf,str_len);
            }
            close(sock_clnt);
            puts("client disconnected_________");
            return 0;
        }
        else
            close(sock_clnt);
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
void read_childproc(int signo)
{
    pid_t id;
    int status;
    id=waitpid(-1,&status,WNOHANG);
    printf("removed proc id:%d\n",id);
}