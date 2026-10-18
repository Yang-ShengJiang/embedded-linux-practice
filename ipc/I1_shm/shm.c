#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <string.h>

#define SHM_NAME "/my_shm"
#define SEM_NAME "/my_sem"
#define SHM_SIZE 1024

int g_shm_fd=-1;
sem_t *g_sem=SEM_FAILED;
char *g_ptr=MAP_FAILED;

void cleanup(void){
    if(g_ptr!=MAP_FAILED){
        munmap(g_ptr,SHM_SIZE);
        g_ptr=MAP_FAILED;
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
    g_shm_fd=shm_open(SHM_NAME,O_RDWR|O_CREAT,0666);
    if(g_shm_fd==-1){
        perror("shm_open");
        exit(1);
    }
    if((ftruncate(g_shm_fd,SHM_SIZE))==-1){
        perror("ftruncate");
        exit(1);
    }
    
    g_sem=sem_open(SEM_NAME,O_CREAT,0666,0);
    if(g_sem==SEM_FAILED){
        perror("sem_open");
        exit(1);
    }

    pid_t pid=fork();
    if(pid==-1){
        perror("fork");
        exit(1);
    }
    else if(pid==0){
        g_ptr=mmap(NULL,SHM_SIZE,PROT_READ,MAP_SHARED,g_shm_fd,0);
        if(g_ptr==MAP_FAILED){
            perror("mmap");
            _exit(1);
        }
        sem_wait(g_sem);
        printf("子进程读到:%s\n",g_ptr);

        munmap(g_ptr,SHM_SIZE);
        _exit(0);
    }else{
        g_ptr=mmap(NULL,SHM_SIZE,PROT_WRITE,MAP_SHARED,g_shm_fd,0);
        if(g_ptr==MAP_FAILED){
            perror("mmap");
            exit(1);
        }
        
        strcpy(g_ptr,"hello");
        printf("父进程已写入完毕\n");

        sem_post(g_sem);
        if((waitpid(-1,NULL,0))==-1){
            perror("waitpid");
        }
    }
    return 0;
}