/*
 * =====================================================================================
 *
 *       Filename:  message_ctl.c
 *
 *    Description:  控制消息队列的行为
 *                  int msgctl(int msgqid,int cmd,struct msqid_ds*buf);
 *                  cmd:
 *                      IPC_STAT:复制和msgqid相关内核结构体数据到buf中，要有读权限。
 *                      IPC_SET :用buf更新msqid_ds结构体,更新msg_ctime，等
 *                      IPC_RMID:立即删除消息队列，并且通知读写队列进程
 *
 *
 *        Version:  1.0
 *        Created:  03/04/2018 11:40:04 AM
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
#include <sys/msg.h>
#include <sys/types.h>

int main(int argc, char* argv)
{
    struct msgqid_ds* qbuf;
    int msg_id;
    key_t key;
    //key=ftok(".",'a');创建的Key值不同。。。
    key = 0x152000f6;
    msg_id = msgget(key, IPC_CREAT | 0777);

    //IPC_STAT
    if (msgctl(msg_id, IPC_STAT, qbuf) == -1) {
        return -1;
    }
    return 0;
}
