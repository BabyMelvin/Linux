/*
 * =====================================================================================
 *
 *       Filename:  semaphore.c
 *
 *    Description:  信号量操作
 *
 *        Version:  1.0
 *        Created:  03/04/2018 12:43:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/fcntl.h>

int main(void)
{
    pid_t pid;
    int sem_id, ret, fd, i, t;
    char str[20];
    key_t key;
    key = 0x400;
    t = rand() % 6;
    struct sembuf sb;
    //创建或者获取信号量id
    if ((sem_id = semget(key, 10, IPC_CTREAT | 0777)) < 0) {
        printf("semget error\n");
        exit(0);
    }
    
    //初始化信号量
    for (i = 0; i < 10; i++) {
        sprintf(str, "file_%d", i);
        if ((pid = fork()) == 0) {
            sleep(t);
            sb.sem_num = i;
            sb.sem_op = -1;
            sb.sem_flg = SEM_UNDO;
            if (semop(sem_id, &sb, 1) < 0) {
                perror("semop");
            }
            execl("/bin/rm", "rm", str, NULL);
            break;
        } else if (pid > 0) {
            fd = open(str, O_CREAT | O_RDWR, 0777);
            sb.sem_num = i;
            sb.sem_op = -1;
            sb.sem_flg = 0;
            if (semop(sem_id, &sb, 1) < 0) {
                perror("semop");
            }
            close(fd);
            sleep(t);
            sb.sem_num = i;
            sb.sem_op = 1;
            sb.sem_flg = 0;
            if (semop(sem_id, &sb, 1) < 0) {
                perror("semop");
            }
        } else {
            perror("fork");
        }
    }
    return 0;
}
