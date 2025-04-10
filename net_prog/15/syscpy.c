#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#define BUFSIZE 3

int main()
{
    int fd1,fd2;
    int len;
    char buf[BUFSIZE];

    fd1=open("new.txt",O_RDONLY);
    fd2=open("cpy.txt",O_CREAT|O_WRONLY|O_TRUNC);

    while((len=read(fd1,buf,sizeof(buf)))>0)
        write(fd2,buf,len);
    close(fd1);
    close(fd2);
    return 0;
}