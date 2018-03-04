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
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc,char * argv){
    struct msgqid_ds *qbuf;
    //IPC_STAT
    if(msgctl(qid,IPC_STAT,qbuf)==-1){
        return -1;
    }
    return 0;
}

