/*
 * =====================================================================================
 *
 *       Filename:  app-button.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/09/2018 01:11:19 PM
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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

int main(void){
    int buttons_fd,key_value;
    button_fd=open("/dev/buttons",0);
    if(buttons_fd<0){
        perror("open device buttons");
        exit(1);
    }
    for(;;){
        fd_set rds;
        int ret;
        FD_ZERO(&rds);
        FD_SET(buttons_fd,&rds);
        ret=select(buttons_fd+1,&rds,NULL,NULL,NULL);
        if(ret<0){
            perror("select");
            exit(1);
        }
        if(ret=0){
            printf("time out\n ");
        }else if(FD_ISSET(buttons_fd,&rds)){
            int ret=read(buttons_fd,&key_value,sizeof(key_value));
            if(ret!=sizeof(key_value)){
                if(errno!=EAGAIN){
                    perror("read buttons\n");
                }
            }else{
                printf("buttons_value:%d\n",key_value+1);
            }
        }
    }
    close(buttons_fd);
    return 0;
}

