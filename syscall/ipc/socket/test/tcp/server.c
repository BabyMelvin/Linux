/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  tcp 连接服务端
 *
 *                  int socket(int domain,int type,int protocal);
 *                  domain:
 *                      AF_UNIX,AF_LOCAL
 *        Version:  1.0
 *        Created:  03/06/2018 11:40:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SERVER_PORT 8000
#define MAX_LINE 4096

int main(int argc, char* argv[])
{
    struct sockaddr_in server_addr, client_addr;
    int socket_fd, addr_len, con_fd, len, i;
    char ip_str[128], buf[MAX_LINE];
    //1.获得socket描述符
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //2.绑定ip和端口port
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET; //地址族协议IPv4
    server_addr.sin_addr.s_addr = htol(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
    //3.监听，128最大
    listen(socket_fd, 128);
    while (true) {
        //4.accept阻塞监听客服端连接请求
        addr_len = sizeof(client_addr);
        addr_len = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);
        //获取连接的客户端IP地址和端口号
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr.s_addr, ip_str, sizeof(ip_str));
        printf("客服端IP:%s\t端口:%d\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip_str, ip_str, sizeof(ip_str)), ntohs(client_addr.sin_port));
        //5.处理客户端请求，con_fd来自多个客户端
        len = read(con_fd, buf, sizeof(buf));
        i = 0;
        while (i < len) {
            buf[i] = toupper(buf[i]);
            i++;
        }
        write(con_fd, buf, len);
        close(con_fd);
    }
    close(socket_fd);
    return 0;
}
