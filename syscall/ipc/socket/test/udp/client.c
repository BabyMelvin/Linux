/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description:  udp 客户端
 *
 *        Version:  1.0
 *        Created:  03/07/2018 10:54:04 AM
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
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>

#define MAX_LINE 80
#define SERVER_PORT 8000
#define INET_ADDRSTRLEN 36

int main(int argc,char* argv[]){
    struct sockaddr_in server_addr;
    int socket_fd,n;
    char buffer[MAX_LINE],str[INET_ADDRSTRLEN];
    socklen_t serveraddr_len;
    socket_fd=socket(AF_INET,SOCK_DGRAM,0);
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    //p:presentation 和 n:numerical
    inet_pton(AF_NET,"127.0.0.1",&server_addr.sin_addr.s_addr.s_addr);
    server_addr.sin_port=htons(SERVER_PORT);
    while(fgets(buffer,MAX_LINE,stdin)!=NULL){
        n=sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr*)&server_addr,sizeof(server_addr));
        if(n==-1){
            perror("sendto");
            exit(1);
        }
        n=recvfrom(socket_fd,buffer,MAX_LINE,0,NULL,0);
        if(n==-1){
            perror("recvfrom");
            exit(1);
        }
        write(STDOUT_FILENO,buffer,n);
    }
    close(socket_fd);
    return 0;
}

