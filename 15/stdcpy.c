#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#define BUFSIZE 3

int main()
{
    FILE* fp1;
    FILE *fp2;
    char buf[BUFSIZE];

    fp1=fopen("new.txt","r");
    fp2=fopen("cpy.txt","w");

    while((fgets(buf,sizeof(buf),fp1))!=NULL)
        fputs(buf,fp2);
    fclose(fp1);
    fclose(fp2);
    return 0;
}