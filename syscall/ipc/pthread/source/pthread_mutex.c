/*
 * =====================================================================================
 *
 *       Filename:  phread_mutex.c
 *
 *    Description:  线程锁
 *
 *        Version:  1.0
 *        Created:  03/04/2018 12:56:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <pthread.h>
#include <stdlib.h>

char buffer[128];
int buffer_data = 0;
pthread_mutex_t mutex;
void write_buffer(char* data)
{
    pthread_mutex_lock(&mutex);
    if (buffer_data == 0) {
        sprintf(buffer, "%s", data);
        buffer_data = 1;
    }
    pthread_mutex_unlock(&mutex);
}
void* read_buffer(void)
{
    while (1) {
        pthread_mutex_lock(&mutex);
        if (buffer_data == 1) {
            printf("readbuffer data =[%s]\n", buffer);
            buffer_data = 0;
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}
int main(void)
{
    char input[128];
    pthread_t reader;
    //用默认属性初始化互斥锁
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&reader, NULL, (void*)read_buffer, NULL);
    while (1) {
        scanf("%s", input);
        write_buffer(input);
    }
    return 0;
}
