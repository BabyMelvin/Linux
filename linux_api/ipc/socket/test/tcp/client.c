/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description:  客服端连接
 *
 *        Version:  1.0
 *        Created:  03/06/2018 12:58:56 PM
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
#include <unistd.h>
#include <fcntl.h>

#define SERVER_PORT 8000

int main(int argc,char *argv[]){
    struct sockaddr_in server_addr;
    int con_fd,len;
    char ip_str[]="192.168.61.240",buf[MAX_LINE];
    if(argc<2){
        printf("Usage:./clinet str\n");
        exit(1);
    }
    //1.创建一个socket
    con_fd=socket(AF_INET,SOCK_STREAM,0);
    //2.初始化服务器地址
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    inet_pton(AF_INET,ip_str,&server_addr.sin_addr.s_addr);
    server_addr.sin_port=htons(SERVER_PORT);
    //3.连接服务器
    connect(con_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    //4.请求服务器处理数据
    write(con_fd,argv[1],strlen(argv[1]));
    len=read(con_fd,buf,sizeof(buf));
    write(STDOUT_FILENO,buf,len);

    //5.关闭socket
    close(conf_fd);
    return 0;
}

