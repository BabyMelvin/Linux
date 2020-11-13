/*
 * =====================================================================================
 *
 *       Filename:  shmat.c
 *
 *    Description:  共享内存
 *
 *        Version:  1.0
 *        Created:  03/04/2018 12:34:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

int main(void)
{
    int shm_id, ret;
    key_t key;
    key = 0x5000;
    char* p;
    shm_id = shmget(key, 1000, IPC_CREAT | 0777);
    if (shm_id < 0) {
        printf("shmget error\n");
        return -1;
    }
    p = shmat(shm_id, NULL, 0);
    p = "123456";
    return 0;
}
