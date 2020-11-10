#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>

// struct sembuf{
//     short sem_num; // 除非使用一组信号量，否则它为0
//     short sem_op;  // 信号量在一次操作中需要改变的数据，通常是两个数，一个是-1，即P（等待）操作，
//                    // 一个是+1，即V（发送信号）操作。
//     short sem_flg; // 通常为SEM_UNDO,使操作系统跟踪信号，
//                    // 并在进程没有释放该信号量而终止时，操作系统释放信号量
// };
struct sembuf p1 = {0, -1, 0}, v1 = {0, 1, 0}, p2 = {1, -1, 0}, v2 = {1, 1, 0};
#define PATHNAME "./"
#define ID 0x27  //只取二进制后八位（16进制后两位）
#define MAXSIZE (10*1024*1024)

int main() {
    key_t key;
    //共享内存
    int shmid;
    if ((key = ftok(PATHNAME, ID)) < 0) {  //获取key
        std::cerr << "ftok error" << std::endl;
        return -1;
    }
    if ((shmid = shmget(key, MAXSIZE, IPC_CREAT | 0666)) < 0) {  //创建共享内存
        std::cerr << "shmget error" << std::endl;
        return -1;
    }
    char* p_addr = (char*)shmat(shmid, NULL, 0);  //进程和共享内存挂在 attach
    //信号量
    //int semid;
    //if ((semid = semget(key, 1, IPC_CREAT)) < 0) {  //创建共享内存
    //    std::cerr << "semget error" << std::endl;
    //    return -1;
    //}
    //if (semctl(semid, 0, SETVAL, 0) < 0) {
    //    std::cerr << "semctl set error0" << std::endl;
    //    return -1;
    //}
    // if (semctl(semid, 1, SETVAL, 1) < 0) {
    //     std::cerr << "semctl set error1" << std::endl;
    //     return -1;
    // }

    size_t flen;
    memcpy(&flen, p_addr, sizeof(size_t));
    char* audio = new char[flen];
    memcpy(audio, p_addr + flen, flen);
    auto fp = fopen("../audios/5057_1.wav", "wb");
    size_t fflen = fwrite(audio, sizeof(char), flen, fp);
    std::cout << "memory write done, flen: " << flen << std::endl;
    fclose(fp);
    delete[]audio;
    //读入数据
    //int test = -1;
    //do {
    //    semop(semid, &p1, 1);  //通过
    //    std::cout << "client get:" << p_addr << std::endl;
    //    test = p_addr[0] - '0';
    //    // semop(semid, &v1, 1);  //释放
    //} while (test);
     if (shmctl(shmid, IPC_RMID, 0) < 0) {
         std::cerr << "shmctl error" << std::endl;
         return -1;
     }
     std::cout << "remove share memory success!" << std::endl;
    // if (semctl(semid, IPC_RMID, 0) < 0) {
    //     std::cerr << "semctl error0" << std::endl;
    //     return -1;
    // }
    // if (semctl(semid, IPC_RMID, 1) < 0) {
    //     std::cerr << "semctl error1" << std::endl;
    //     return -1;
    // }
    // std::cout << "semctl success!" << std::endl;
    return 0;
}