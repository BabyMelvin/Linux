/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  udp 服务端
 *
 *        Version:  1.0
 *        Created:  03/07/2018 10:41:43 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define MAX_LINE 80
#define SERVER_PORT 8000

int main(int argc, char* argv[])
{
    struct sockaddr_in server_addr, client_addr;
    int socket_fd, server_len, client_len, n, i;
    char buffer[MAX_LINE];
    char str[32];
    socket_fd = socket(AF_INET, SOCKET_DGRAM, 0);
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    bind(socket_fd, (struct scokaddr*)&server_addr, sizeof(server_addr));
    printf("客服端等待连接...\n");
    while (1) {
        client_len = sizeof(client_addr);
        n = recvfrom(sockfd, buffer, MAX_LINE, 0, (struct sockaddr*)&clinet_addr, &client_len);
        if (n == -1) {
            perror("recvfrom");
            exit(1);
        }
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, str, sizeof(str));
        printf("收到地址:%s 端口号:%d\n", str, ntohs(client_addr.sin_port));
        for (i = 0; i < n; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
        if (n == -1) {
            perror("sendto");
            exit(1);
        }
    }
    return 0;
}
