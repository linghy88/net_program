#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#define BUFSIZE 1000
#define MAX_CLIENTS 50
#define PORT 8888

typedef struct{
    int socket;//socket
    struct sockaddr_in clnt_addr;//地址
    char name[20];//姓名
    int uid;//用户编号
}client_t;

client_t *clients[MAX_CLIENTS];//定义为数组指针，每个元素都是指针，方便动态管理
pthread_mutex_t mt=PTHREAD_MUTEX_INITIALIZER;//静态初始化，适用于全局或静态static
int clnt_cnt=0;
int uid=1;

void Error_handle(char *msg);
void add_clnt(client_t *client);
void del_clnt(int uid_d);
void *handle_clnt(void *arg);

int main(int argc,char *argv[])
{
    char buf[BUFSIZE];
    struct sockaddr_in serv_addr,clnt_addr;
    int clnt_size;
    int sock_serv,sock_clnt;
    pthread_t tid;

    sock_serv=socket(AF_INET,SOCK_STREAM,0);
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(PORT);
    
    if(bind(sock_serv,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
        Error_handle("bind() error!");
    
    if(listen(sock_serv,10)==-1)
        Error_handle("listen() error!");
    
    while(1){//持续接收客户端的连接
        clnt_size=sizeof(sock_clnt);
        if((sock_clnt=accept(sock_serv,(struct sockaddr*)&clnt_addr,&clnt_size))==-1)//分配新的客户端套接字
        Error_handle("accept() error!");


        if(clnt_cnt==MAX_CLIENTS){
            Error_handle("超出客户最大限制");
            close(sock_clnt);
            continue;
        }

        //创建客户端
        client_t *client=(client_t*)malloc(sizeof(client_t));//要用malloc分配空间
        client->socket=sock_clnt;
        client->clnt_addr=clnt_addr;
        client->uid=uid++;
        add_clnt(client);

        //创建客户端线程
        if(pthread_create(&tid,NULL,&handle_clnt,(void*)client)!=0){//传入client给handle_clnt
            Error_handle("创建线程失败");
            free(client);//失败一定要free，不然会内存泄露
            continue;
        }
        pthread_detach(tid);
        // printf("Conneted client IP:%s \n",inet_ntoa(clnt_addr.sin_addr));      
    }
    
    close(sock_serv);
    return 0;
}


void Error_handle(char *msg){
    fputs(msg,stderr);
    fputs("\n",stderr);
    exit(1);
}

//添加客户
void add_clnt(client_t *client){
    pthread_mutex_lock(&mt);
    for(int i=0;i<MAX_CLIENTS;i++){//重头开始遍历，将已删除客户的位置填上
        if(!clients[i]){
            clients[i]=client;
            break;
        }    
    }
    clnt_cnt++;
    printf("添加客户成功\n");
    pthread_mutex_unlock(&mt);
}

//删除客户
void del_clnt(int uid_d){
    pthread_mutex_lock(&mt);
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i] && clients[i]->uid==uid_d){
            close(clients[i]->socket);
            free(clients[i]);
            clients[i]=NULL;//将要删除的位置置空
            break;
        } 
    }
    clnt_cnt--;
    printf("删除客户:%d\n",uid_d);
    pthread_mutex_unlock(&mt);
}

//广播实现
void broadcast(char *msg,int sendid){
    //printf("开始广播\n");
    pthread_mutex_lock(&mt);
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i] && clients[i]->uid!=sendid){
            if(send(clients[i]->socket,msg,strlen(msg),0)<=0){//sizeof(msg):是一个指针的大小
                perror("发送失败！");
                del_clnt(clients[i]->uid);
            }
        }
    }
    pthread_mutex_unlock(&mt);
}

//处理线程函数
void *handle_clnt(void *arg){
    client_t *client=(client_t*)arg;
    char buf[BUFSIZE];
    int recv_len;

    //读取name
    if((recv_len=recv(client->socket,buf,sizeof(buf),0))<=0){
        printf("客户端 %d 断开连接\n",client->uid);
        close(client->socket);
        del_clnt(client->uid);
    }
    strcpy(client->name,buf);//赋值name
    printf("%s(%d) 加入聊天室\n",client->name,client->uid);
    
    while(1){
        int recv_len=read(client->socket,buf,BUFSIZE-1);//预留一个位置给存放\0，很重要!
        
        if(recv_len<=0){//等于0，代表对方发送了FIN包
            printf("[%s](%d)离开聊天室\n",client->name,client->uid);
            del_clnt(client->uid);
            pthread_exit(NULL);
        }

        buf[recv_len]='\0';
        //char name_msg[BUFSIZE+20];扩容32个位置，避免溢出
        char name_msg[BUFSIZE + strlen(client->name) + 20];
        sprintf(name_msg,"%s(%d):%s",client->name,client->uid,buf);//需要正确终止buf
        broadcast(name_msg,client->uid);
    }
    return NULL;
}