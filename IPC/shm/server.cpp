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
#include <math.h>
#include <iostream>
#include <string>

// struct sembuf{
//     short sem_num; // 除非使用一组信号量，否则它为0
//     short sem_op;  // 信号量在一次操作中需要改变的数据，通常是两个数，一个是-1，即P（等待）操作，
//                    // 一个是+1，即V（发送信号）操作。
//     short sem_flg; // 通常为SEM_UNDO,使操作系统跟踪信号，
//                    // 并在进程没有释放该信号量而终止时，操作系统释放信号量
// };
struct sembuf Pept = {0, -1, 0}, Vept = {0, 1, 0}, Pfll = {1, -1, 0}, Vfll = {1, 1, 0};
#define PATHNAME "./" //key的低五位
#define PATHNAME2 "../audios"
#define ID 0x27 //key的高4位,虽然为int，0-127间的一个整数值
#define ID2 0x11
#define MAXSIZE (10*1024*1024)
#define math (int)(ceil(16*1000*60/4096.0)*4096)

int main() {
    std::cout << "16*1000*60/4096 = " << 16 * 1000 * 60 / 4096 << std::endl;
    std::cout << "ceil(16*1000*60/4096) = " << ceil(16 * 1000 * 60 / 4096) << std::endl;
    std::cout << "start success " << math << std::endl;
    key_t key;
    /*key_t key2;
    key_t key3;*/
    //共享内存
    if ((key = ftok(PATHNAME, ID)) < 0) {  //获取key
        std::cerr << "ftok error" << std::endl;
        return -1;
    }
    std::cout << "ftok success" << std::endl;
    //if ((key2 = ftok(PATHNAME, ID2)) < 0) {  //获取key
    //    std::cerr << "ftok error2" << std::endl;
    //    return -1;
    //}
    //if ((key3 = ftok(PATHNAME2, ID2)) < 0) {  //获取key
    //    std::cerr << "ftok error3" << std::endl;
    //    return -1;
    //}
    //std::cout << key << " " << key2 << " " << key3 << std::endl;
    int shmid, shmid2;
    if ((shmid = shmget(key, MAXSIZE, IPC_CREAT | 0666)) < 0) {  //创建共享内存
        std::cerr << "shmget error" << std::endl;
        return -1;
    }
    if ((shmid2 = shmget(key, MAXSIZE, IPC_CREAT | 0666)) < 0) {  //创建共享内存
        std::cerr << "shmget error2" << std::endl;
        return -1;
    }
    std::cout << shmid << " " << shmid2 << std::endl;
    std::cout << "shmget success" << std::endl;

    char* p_addr = NULL;
    if ((p_addr = (char*)shmat(shmid, NULL, 0)) == (void*)-1) {
        std::cerr << "shmat error" << std::endl;
        return -1;
    }
    std::cout << "shmat success: " << (int*)p_addr << std::endl;
    memset(p_addr, 0, MAXSIZE);
    //信号量
    //int semid;
    //if ((semid = semget(key, 2, IPC_CREAT)) < 0) {  //创建共享内存
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

    //写入数据
    char* audio = new char[MAXSIZE - sizeof(size_t)];
    auto fp = fopen("../audios/5057.wav", "rb");
    size_t flen = fread(audio, sizeof(char), MAXSIZE-sizeof(size_t), fp);
    memcpy(p_addr, &flen, sizeof(size_t));
    memcpy(p_addr + sizeof(size_t), audio, flen);
    std::cout << "memory copy done, flen: " << flen << std::endl;
    fclose(fp);
    delete[]audio;
    //int test = -1;
    //do {
    //    printf("************start test mem share***************\n");
    //    printf("0 is exit!\n");
    //    gets(input);
    //    test = input[0] - '0';
    //    strncpy(p_addr, input, MAXSIZE);
    //    semop(semid, &Vept, 1);  //释放 原v1
    //    // semop(semid, &p1, 1);  //通过 原p2
    //} while (test);
   /* if (shmctl(shmid, IPC_RMID, 0) < 0) {
        std::cerr << "shmctl error" << std::endl;
        return -1;
    }
    std::cout << "remove share memory success!" << std::endl;*/
    /*if (semctl(semid, IPC_RMID, 0) < 0) {
        std::cerr << "semctl error0" << std::endl;
        return -1;
    }*/
    // if (semctl(semid, IPC_RMID, 1) < 0) {
    //     std::cerr << "semctl error1" << std::endl;
    //     return -1;
    // }
    //std::cout << "semctl success!" << std::endl;
    return 0;
}