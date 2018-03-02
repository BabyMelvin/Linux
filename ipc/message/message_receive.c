/*
 * =====================================================================================
 *
 *       Filename:  message_receive.c
 *
 *    Description:  接收消息队列
 *
 *        Version:  1.0
 *        Created:  03/02/2018 05:25:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
struct student{
    int num;
    char name[32];
};
struct msgbuf{
    long type;
    struct student stu;
    struct msgbuf* next;
}
int main(void){
    int msg_id,num;
    char ch;
    long type;
    char name[32];
    struct msgbuf*temp;
    temp=malloc(sizeof(struct msgbuf));
    struct msgbuf *head=NULL;
    
    key_t key;
    key=ftok(".",'a');
    msg_id=msget(key,IPC_CREAT,0777);

    msgrcv(msg_id,temp,sizeof(struct msgbuf)),2,0);
    printf("temp.type=%ld,temp.num=%d\n",temp->type,temp->stu.num);
    return 0;
}

