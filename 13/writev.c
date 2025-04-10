#include<stdio.h>
#include<sys/uio.h>

int main()
{
    struct iovec vec[2];
    char *buf1="ABCSS";
    char *buf2="12345";
    vec[0].iov_base=buf1;
    vec[0].iov_len=3;
    vec[1].iov_base=buf2;
    vec[1].iov_len=3;
    int strlen;

    strlen=writev(1,vec,2);
    puts("");
    printf("write bytes:%d\n",strlen);
    return 0;
}