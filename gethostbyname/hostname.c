#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>

int main(int argc,char *argv[])
{
    struct hostent *host;
    if(argc!=2){
        printf("Usage:%s <addr>\n",argv[0]);
    }

    host=gethostbyname(argv[1]);
    printf("office name:%s\n",host->h_name);

    for(int i=0;host->h_aliases[i];i++){
        printf("alias list%d:%s\n",i+1,host->h_aliases[i]);
    }
    
    printf("address type:%s\n",(host->h_addrtype==AF_INET)?"AF_INET":"AF_INET6");

    for(int i=0;host->h_addr_list[i];i++){
        printf("IP addr%d: %s\n",i+1,inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
    }
    return 0;
}