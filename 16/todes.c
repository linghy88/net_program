#include<stdio.h>
#include<fcntl.h>

int main()
{
    int fd=open("data.txt",O_CREAT|O_WRONLY|O_TRUNC);
    printf("FIRST fd:%d\n",fd);
    FILE *fp=fdopen(fd,"w");
    fputs("hello!",fp);
    printf("second fd:%d\n",fileno(fp));
    fclose(fp);
    return  0;
}