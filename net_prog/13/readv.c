#include<stdio.h>
#include<sys/uio.h>
#define BUFSIZE 100


int main()
{
    struct iovec vec[2];
    char buf1[BUFSIZE]={0};
    char buf2[BUFSIZE]={0};
    vec[0].iov_base=buf1;
    vec[0].iov_len=5;
    vec[1].iov_base=buf2;
    vec[1].iov_len=10;

    int strlen=readv(0,vec,2);//0:标准输入
    printf("data bytes:%d\n",strlen);
    printf("buf1 data:%s\n",buf1);
    printf("buf2 data:%s\n",buf2);
    return 0;
}
