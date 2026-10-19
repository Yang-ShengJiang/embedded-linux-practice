#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include<semaphore.h>
#include<string.h>

#define SHM_NAME "/ring_shm"
#define SEM_EMPTY_NAME "/sem_empty"
#define SEM_FULL_NAME "/sem_full"
#define BUF_SIZE 10

typedef struct {
    int write_pos;
    int read_pos;
    char data[BUF_SIZE];
}SharedRingBuf;

int g_shm_fd=-1;
sem_t *g_sem_empty=SEM_FAILED;
sem_t *g_sem_full=SEM_FAILED;
SharedRingBuf *buf=MAP_FAILED;

void cleanup(void){
    if(buf!=MAP_FAILED){
        munmap(buf,sizeof(SharedRingBuf));
        buf=MAP_FAILED;
    }
    if(g_sem_empty!=SEM_FAILED){
        sem_close(g_sem_empty);
        sem_unlink(SEM_EMPTY_NAME);
        g_sem_empty=SEM_FAILED;
    }
    if(g_sem_full!=SEM_FAILED){
        sem_close(g_sem_full);
        sem_unlink(SEM_FULL_NAME);
        g_sem_full=SEM_FAILED;
    }
    if(g_shm_fd!=-1){
        close(g_shm_fd);
        shm_unlink(SHM_NAME);
        g_shm_fd=-1;
    }

}

int main(void){
    atexit(cleanup);
    sem_unlink(SEM_EMPTY_NAME);
    sem_unlink(SEM_FULL_NAME);
    shm_unlink(SHM_NAME);

    g_shm_fd=shm_open(SHM_NAME,O_RDWR|O_CREAT,0666);
    if(g_shm_fd==-1){
        perror("shm_open");
        exit(1);
    }
    if(ftruncate(g_shm_fd,sizeof(SharedRingBuf))==-1){
        perror("ftruncate");
        exit(1);
    }

    buf=(SharedRingBuf*)mmap(NULL,sizeof(SharedRingBuf),PROT_WRITE|PROT_READ,MAP_SHARED,g_shm_fd,0);
    if(buf==MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    memset(buf,0,sizeof(SharedRingBuf));

    g_sem_empty=sem_open(SEM_EMPTY_NAME,O_CREAT,0666,10);
    if(g_sem_empty==SEM_FAILED){
        perror("sem_open empty");
        exit(1);
    }
    g_sem_full=sem_open(SEM_FULL_NAME,O_CREAT,0666,0);
    if(g_sem_full==SEM_FAILED){
        perror("sem_open full");
        exit(1);
    }

    pid_t pid=fork();
    if(pid==-1){
        perror("fork");
        exit(1);
    }
    else if(pid==0){
        for(int i=0;i<5;i++){
            sem_wait(g_sem_full);
            char c=buf->data[buf->read_pos];
            printf("子进程读到:%c\n",buf->data[buf->read_pos]);
            buf->read_pos=(buf->read_pos+1)%BUF_SIZE;
            sem_post(g_sem_empty);
            usleep(200000);
        }
        _exit(0);
    }else{
        for(int i=0;i<5;i++){
            sem_wait(g_sem_empty);
            buf->data[buf->write_pos]='A'+i;
            printf("父进程写入:%c\n",buf->data[buf->write_pos]);
            buf->write_pos=(buf->write_pos+1)%BUF_SIZE;
            sem_post(g_sem_full);
            usleep(100000);
        }
        wait(NULL);
    }
    return 0;
}