#include<stdio.h>
#include<unistd.h>

int main()
{
    int pid=fork();
    if(pid==0){
        puts("i am child process");
    }
    else
    {
        printf("Child process pid:%d\n",pid);
        sleep(30);
    }

    if(pid==0)
        puts("End child process");
    else
        puts("End parent process");
    return 0;
}