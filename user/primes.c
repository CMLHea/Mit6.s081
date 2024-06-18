#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define RD 0
#define WR 1

const uint INT_LEN = sizeof(int);

int lpipe_first_data(int lpipe[2], int *dst){
    if(read(lpipe[RD], dst, sizeof(int)) == sizeof(int)){
        printf("prime %d\n", *dst);
        return 0;
    }
    return -1;
}

void transmit_data(int lpipe[2], int rpipe[2], int first){
    int data;
    while(read(lpipe[RD], &data, sizeof(int)) == sizeof(int)){
        if(data % first)
            write(rpipe[WR], &data, sizeof(int));
    }
    close(lpipe[RD]);
    close(rpipe[WR]);
}

void primes(int lpipe[2]){
    close(lpipe[WR]);
    int first;
    if(lpipe_first_data(lpipe, &first) == 0){
        int p[2];
        pipe(p);
        transmit_data(lpipe, p, first);

        if(fork() == 0){
            primes(p);
        }else{
            close(p[RD]);
            wait(0);
        }
    }
    exit(0);
}

int main(int argc, char const *argv[]){
    int p[2];
    pipe(p);

    for(int i = 2; i < 35; i++){
        write(p[WR], &i, INT_LEN);
    }
    if(fork() == 0){
        primes(p);
    }else{
        close(p[WR]);
        close(p[RD]);
        wait(0);
    }
    exit(0);

}

// void runprocess(int listenfd){
//     int my_num = 0;
//     int forked = 0;
//     int passed_num = 0;
//     int pipes[2];
//     while(1){
//         int read_bytes = read(listenfd, &passed_num, 4);

//         if(read_bytes == 0){
//             close(listenfd);
//             if(forked){
//                close(pipes[1]);
//                int child_pid;
//                wait(&child_pid); 
//             }
//             exit(0);
//         }

//         if(my_num == 0){
//             my_num = passed_num;
//             printf("prime %d\n", my_num);
//         }

//         if(passed_num % my_num != 0){
//             if(!forked){
//                 pipe(pipes);
//                 forked = 1;
//                 int ret = fork();
//                 if(ret == 0){
//                     close(pipes[1]);
//                     close(listenfd);
//                     runprocess(pipes[0]);
//                 }else{
//                     close(pipes[0]);
//                 }
//             }
//             write(pipes[1], &passed_num, 4);
//         }
//     }
// }
// int main(int argc, char *argv[]) {
//     int pipes[2];
//     pipe(pipes);
//     for (int i = 2; i <= 35; i++) {
//         write(pipes[1], &i, 4);
//     }
//     close(pipes[1]);
//     runprocess(pipes[0]);
//     exit(0);
// }