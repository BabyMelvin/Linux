/*
 * =====================================================================================
 *
 *       Filename:  wait.c
 *
 *    Description: 等待子进程改变状态,SIGCHLD信号发送给父进程
 *                 子进程改变：
 *                      1.子进程正常结束(释放相关的资源,如果没有wait将进入僵尸状态zombie)
 *                      2.子进程被信号停止
 *                      3.被信号重启
 *                 pid_t wait(int *status); 
 *                      阻塞知道其中一个子进程结束
 *                 pid_t waitpid(pid_t pid,int* status,int options);
 *                      指定子进程pid，options默认只是子进程结束，其他选择
 *                      pid:
 *                          < -1:任何子进程的组进程id绝对值
 *                          -1  :任何子进程结束
 *                          0   :任何子进程和调用相等的组进程
 *                          >0  :子进程和pid相等
 *                      options:
 *                          WNOHANG   :如果没有子进程退出立马返回
 *                          WUNTRACAED:子进程暂停返回. (traced自己成，即使没有这个状态也返回)
 *                          WCONINUED:暂停进程被重启后返回。并SIGCONT信号发出。
 *                      status:如果不是NULL，可用宏检测
 *                         WIFEXITED(status):子进程正常返回true ,exit(2)或_exit(2)
 *                         WEXITSTATUS(status):返回子进程退出状态.
 *                  $kill -STOP pid
 *                  $kill -CONT pid
 *                  $kill -TERM pid
 *
 *        Version:  1.0
 *        Created:  03/02/2018 11:36:13 AM
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
#include <sys/wait.h>
#include <unistd.h>
int main(int argc, char* argv[])
{
    pid_t cpid, w;
    int status;
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {
        //被任何一个子进程执行
        printf("子进程 pid=%ld\n", (long)getpid());
        if (argc == 1) {
            //等待信号
            pause();
            _exit(atoi(argv[1]));
        }
    } else {
        do {
            w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
            if (w == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
            if (WIFEXITED(status)) {
                printf("exited ,status=%d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("killed by signal %d\n", WTERMSIG(status));
            } else if (WIFCONTINUED(status)) {
                printf("continued\n");
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        exit(EXIT_SUCCESS);
    }
    return 0;
}
