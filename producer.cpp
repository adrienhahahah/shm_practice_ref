#define TEXT_SZ 2048
struct shared_use_st {
    int written;
    char text[TEXT_SZ];
};


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>

int main()
{
    int shmid;
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    void* shared_memory = (void*) 0;
    shared_memory = shmat(shmid, shared_memory, 0); //SHM_RDONLY
    if(shared_memory == (void*)-1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Memory attached at %X\n", (long)shared_memory);
    struct shared_use_st *shared_stuff;
    shared_stuff = (shared_use_st*)shared_memory;
    int running = 1;
    char buffer[BUFSIZ]; // stdio.h
    while(running )
    {
        while(shared_stuff->written == 1){
            sleep(1);
            printf("waiting for client\n");
        }
        printf("Enter some test:");
        fgets(buffer, BUFSIZ, stdin);
        strncpy(shared_stuff->text, buffer, TEXT_SZ);
        shared_stuff->written = 1;
        if(strncmp(buffer, "end", 3) == 0)
        {
            running = 0;
        }
    }

    if (shmdt(shared_memory) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
