/*
 * =====================================================================================
 *
 *       Filename:  gethostbyname.c
 *
 *    Description:  通过网址查询详细信息
 *
 *        Version:  1.0
 *        Created:  03/07/2018 11:14:46 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/fcntl.h>

int main(int argc, char* argv[])
{
    char **pptr, str[46];
    //域名结构体
    struct hostent* hptr;
    if (argc < 2) {
        fprintf(stderr, "Usage:domain(域名),如:www.google.com或192.168.0.1\n");
        return -1;
    }
    if ((hptr = gethostbyname(argv[1])) == NULL) {
        fprintf(stderr, "gethostbyname call failed:%s\n", hstrerror(h_errno));
        return -1;
    }
    printf("offical name:%s\n", hptr->h_name);
    for (pptr = hptr->h_aliases; *pptr != NULL; pptr++) {
        printf("\talias(别名):%s\n", *pptr);
    }
    if (hptr->h_addrtype != AF_INET) {
        fprintf(stderr, "Invaliable address type:%d\n", hptr->h_addrtype);
        return -1;
    }
    pptr = hptr->h_addr_list;
    for (; *pptr != NULL; pptr++) {
        printf("\taddress:%s\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
    }
    return 0;
}
