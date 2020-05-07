/*开地址哈希表*/

#include <stdlib.h>
#include <string.h>
#include "ohtbl.h"

/* Reserve a sentinel(哨兵) memory address for vacated elements*/
static char vacated;
int ohtbl_init(OHTbl *htbl, int positions, int (*h1)(const void *key),
        int (*h2)(const void *key), int (*match)(const void *key1,
            const void *key2), void (*destory)(void *data))
{
    int i;

    /*Allocate space for the hash table*/
    if ((htbl->table = (void **)malloc(positions * sizeof (void *))) == NULL)
        return -1;

    /*Initialize each position*/
    htbl->positions = positions;
    for (i = 0; i < htbl->positions; i++)
        htbl->table[i] = NULL;

    /*Set the vacated member to the sentinel memory address reserved for this*/
    htbl->vacated = &vacated;

    /*Encapsulate the function*/
    htbl->h1 = h1;
    htbl->h2 = h2;
    htbl->match = match;
    htbl->destroy = destory;

    /*Initialize the number of elements in the table*/
    htbl->size = 0;

    return 0;
}

void ohtbl_destroy(OHTbl *htbl)
{
    int i;
    
    if (htbl->destroy != NULL) {
        /*Call a user-defined function to free dynamically allocated data*/
        for (i = 0; i < htbl->positions; i++) {
            if (htbl->table[i] != NULL && htbl->table[i] != htbl->vacated)
                htbl->destroy(htbl->table[i]);
        }
    }

    /*Free the storage allocated for the hash table*/
    free(htbl->table);

    /*No operaitons are allowed now, but clear the structure as a precaution*/
    memset(htbl, 0, sizeof(OHTbl));

    return;
}

int ohtbl_insert (OHTbl *htbl, const void *data)
{
    void *temp;
    int postion, i;

    /*Do not exceed the number of postions in the table*/
    if (htbl->size == htbl->positions)
        return -1;

    /*Do nothing if the data is already in the table*/
    temp = (void *)data;
    if (ohtbl_lookup(htbl, &temp) == 0)
        return 1;

    for (i = 0; i < htbl->positions; i++) {
        postion = (htbl->h1(data) + (i * htbl->h2(data))) % htbl->positions;
        if (htbl->table[postion] == NULL 
                || htbl->table[postion] == htbl->vacated) {
            /*Insert the data into the table*/
            htbl->table[postion] = (void *)data;
            htbl->size ++;
            return 0;
        }
    }

    /*Return that the hash function were selected incorrectly*/
    return -1;
}

int ohtbl_remove (OHTbl *htbl, void **data)
{
    int postion, i;

    /*use double hashing to hash the key*/
    for (i = 0; i < htbl->positions; i++) {
        postion = (htbl->h1(*data) + (i * htbl->h2(*data))) % htbl->positions;
 
        if (htbl->table[postion] == NULL) {
            /*Return that the data was not found*/
            return -1;
        } else if (htbl->table[postion] == htbl->vacated) {
            /*Search beyong vacated postions*/
            continue;
        } else if (htbl->match(htbl->table[postion], *data)) {
            /*Pass back the data from the table*/
            *data = htbl->table[postion];
            htbl->table[postion] = htbl->vacated;
            htbl->size --;
            return 0;
        }
    }

    /*Return that the data was not found*/
    return -1;
}

int ohtbl_lookup (const OHTbl *htbl, void **data)
{
    int postion, i;

    /*Use double hashing to hash the key*/
    for (i = 0; i < htbl->positions; i++) {
        postion = (htbl->h1(*data) + (i * htbl->h2(*data))) % htbl->positions;
        if (htbl->table[postion] == NULL) {
            /*Return that the data was not found*/
            return -1;
        } else if (htbl->match(htbl->table[postion], *data)) {
            /*Pass back the data from the table*/
            *data = htbl->table[postion];
            return 0;
        }
    }

    /*Return that the data was not found*/
    return -1;
}
