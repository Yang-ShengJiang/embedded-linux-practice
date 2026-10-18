#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <sys/wait.h>

#define MQ_NAME "/my_queue"

int main(void){
    struct mq_attr attr;
    attr.mq_maxmsg=10;
    attr.mq_msgsize=100;
    attr.mq_flags=0;
    attr.mq_curmsgs=0;

    mqd_t mq=mq_open(MQ_NAME,O_CREAT|O_RDWR,0666,&attr);
    if(mq==(mqd_t)-1){
        perror("mq_open");
        exit(1);
    }

    pid_t pid=fork();
    if(pid==-1){
        perror("fork");
        exit(1);
    }
    else if(pid==0){
        char *msg1="Hello High";
        char *msg2="Hello Low";

        if(mq_send(mq,msg2,strlen(msg2),1)==-1){
            perror("mq_send Low");
            _exit(1);
        }

        if(mq_send(mq,msg1,strlen(msg1),5)==-1){
            perror("mq_send High");
            _exit(1);
        }

        _exit(0);
    }else{
        char buf[100];
        unsigned int prio;

        sleep(1);//保证子进程消息发完

        int n=mq_receive(mq,buf,sizeof(buf),&prio);
        if(n==-1){
            perror("mq_receive");
            exit(1);
        }
        buf[n]='\0';
        printf("收到消息:%s,优先级为%u\n",buf,prio);

        n=mq_receive(mq,buf,sizeof(buf),&prio);
        if(n==-1){
            perror("mq_receive");
            exit(1);
        }
        buf[n]='\0';
        printf("收到消息:%s,优先级为%u\n",buf,prio);

        mq_close(mq);
        mq_unlink(MQ_NAME);
        wait(NULL);
    }
    return 0;
}