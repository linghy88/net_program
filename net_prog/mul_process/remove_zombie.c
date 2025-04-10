#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>

void read_child(int signo)
{
    int status;
    pid_t id=waitpid(-1,&status,WNOHANG);
    while ((id = waitpid(-1, &status, WNOHANG)) > 0) {  // 关键修改：使用 while 循环
        if (WIFEXITED(status)) {
            printf("Remove proc id:%d\n", id);
            printf("Child send:%d\n", WEXITSTATUS(status));
        }
    }
}

int main()  
{
    pid_t pid;
    struct sigaction act;
    act.sa_handler=read_child;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    sigaction(SIGCHLD,&act,0);

    pid=fork();
    if(pid==0){
        puts("HI I am child!");
        sleep(10);
        return 12;
    }
    else {
        printf("Child proc id:%d \n",pid);
        pid=fork();
        if(pid==0){
            puts("HI I am child!");
            sleep(10);
            exit(24);
        }
        else {
            int i;
            printf("Child proc id:%d \n",pid);
            for(i=0;i<5;i++){
                puts("wait...");
                sleep(5);
            }
        }
    }
    return 0;
}