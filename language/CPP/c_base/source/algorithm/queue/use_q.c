#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int main(void)
{
    Queue line;
    Item temp;
    char ch;
    initialize_queue(&line);
    puts("Testing the queue interface.Type a to add a value");
    puts("type d to delete a value, and type q to quit");

    while((ch = getchar()) != 'q')
    {
        if(ch != 'a' && ch != 'd')
            continue;

        if(ch == 'a') {
            printf("Integer to add:");
            scanf("%d", &temp);
            if(!queue_is_full(&line)) {
                printf("Putting %d into queue\n", temp);
                en_queue(temp, &line);
            }  else {
                puts("Queue is full");
            }
        } else {
            if(queue_is_empty(&line))
                puts("Nothing to delete!");
            else {
                de_queue(&temp, &line);
                printf("Removing %d from queue\n", temp);
            }
        }
        
        printf("%d items in queue\n", queue_item_count(&line));
        puts("Type a to add, d to delete, q to quit:");
    }

    empty_queue(&line);
    puts("Bye!");

    return 0;
}
