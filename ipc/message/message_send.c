/*
 * =====================================================================================
 *
 *       Filename:  message.c
 *
 *    Description: 创建消息队列发送端 
 *                 $ipcs 查看
 *
 *        Version:  1.0
 *        Created:  03/02/2018 04:12:42 PM
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
    struct msgbuf *next;
};

struct msgbuf* create_node(long type,int num,char*name){
    struct msgbuf * temp;
    temp=malloc(sizeof(struct msgbuf));
    temp->type=type;
    temp->stu.num=num;
    strcpy(temp->stu.name,name);
    temp->next=NULL;
    return temp;
}
struct msgbuf* insert_node_head(struct msgbuf*head,struct msgbuf*temp){
   if(temp==NULL){
     head=temp;
   }else{
       temp->next=head;
       head=temp;
   } 
   return head;
}
void print_node(struct msgbuf*head){
    while(head){
        printf("type=%ld,num=%d,name=%s\n",head->type,head->stu.num,head->stu.name);
        head=head->next;
    }
}
int main(void){
    int msg_id,num;
    char ch;
    long type;
    char name[32];
    struct msgbuf*temp;
    struct msgbuf *p;
    struct msgbuf *head=NULL;

    key_t key;
    //通过文件路径获得唯一的KEY值标识,或者指定key=0x2000;
    key=ftok(".","a");

    //创建消息队列
    msg_id=msgget(key,IPC_CREAT|0777);

    //生成创建消息链表
    while(1){
        printf("输入 type,num,name\n");
        scanf("%ld%d%s",%type,&num,name);
        temp=create_node(type,num,name);
        printf("y/n,insert?\n");
        getchar();
        ch=getchar();
        if(ch=='Y'||ch=='y'){
            if(temp){
                head=insert_node_head(head,temp);
            }else{
                break;
            }
        }
        
    }
    print_node(head);
    p=head;
    while(p){
        //flag:IPC_NOWAIT
        //struct msgbuf{long type,char *data};
        msgsnd(msg_id,p,sizeof(struct msgbuf)-sizeof(long),0);
        p=p->next;
    }
    return 0;
}


