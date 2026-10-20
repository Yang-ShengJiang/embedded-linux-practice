#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/mman.h>
#include<string.h>
#include<semaphore.h>

#define SHM_NAME "/count_shm"
#define SEM_NAME "/mutex_sem"

typedef struct {
    int count;
}SharedCounter;

int g_shm_fd=-1;
sem_t *g_sem=SEM_FAILED;

SharedCounter *counter=MAP_FAILED;

void cleanup(void){
    if(counter!=MAP_FAILED){
        munmap(counter,sizeof(SharedCounter));
        counter=MAP_FAILED;
    }
    if(g_sem!=SEM_FAILED){
        sem_close(g_sem);
        sem_unlink(SEM_NAME);
        g_sem=SEM_FAILED;
    }
    if(g_shm_fd!=-1){
        close(g_shm_fd);
        shm_unlink(SHM_NAME);
        g_shm_fd=-1;
    }
}

int main(void){
    atexit(cleanup);
    sem_unlink(SEM_NAME);
    shm_unlink(SHM_NAME);

    g_shm_fd=shm_open(SHM_NAME,O_CREAT|O_RDWR,0666);
    if(g_shm_fd==-1){
        perror("shm_open");
        exit(1);
    }
    if(ftruncate(g_shm_fd,sizeof(SharedCounter))==-1){
        perror("ftruncate");
        exit(1);
    }

    g_sem=sem_open(SEM_NAME,O_CREAT,0666,1);
    if(g_sem==SEM_FAILED){
        perror("sem_open");
        exit(1);
    }

    counter=(SharedCounter*)mmap(NULL,sizeof(SharedCounter),PROT_READ|PROT_WRITE,MAP_SHARED,g_shm_fd,0);
    if(counter==MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    memset(counter,0,sizeof(SharedCounter));

    for(int i=0;i<3;i++){
        pid_t pid=fork();
        if(pid==0){
            for(int j=0;j<10000;j++){
                sem_wait(g_sem);
                counter->count++;
                sem_post(g_sem);
            }
            _exit(0);
        }      
    }

    for(int j=0;j<10000;j++){
        sem_wait(g_sem);
        counter->count++;
        sem_post(g_sem);
    }

    for(int i=0;i<3;i++){
        wait(NULL);
    }

    printf("%d\n",counter->count);
    return 0;
}