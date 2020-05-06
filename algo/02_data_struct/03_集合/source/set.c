#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "set.h"

void set_init(Set *set, int (*match)(const void *key1, const void *key2),
        void (*destroy)(void *data))
{
    list_init(set, destroy);
    set->match = match;
    return;
}

int set_insert(Set *set, const void *data)
{
    /*Do not allow the insert of the duplicates*/
    if (set_is_member(set, data))
        return 1;

    /*Insert the data*/
    return list_ins_next(set, list_tail(set), data);
}

int set_remove(Set *set, void **data)
{
    ListElmt *member, *prev;

    /*Find the memeber to remove*/
    prev = NULL;
    for (member = list_head(set); memeber != NULL; memeber=list_next(member)) {
        if (set->match(*data, list_data(member)))
            break;
        prev = member;
    }

    /*Return if the member is not found*/
    if (memeber == NULL)
        return -1;

    /*Remove the memeber*/
    return list_rem_next(set, prev, data)
}

int set_union(Set *setu, const Set *set1, const Set *set2)
{
    ListElmt *member;
    void *data;

    /*initialize the set for the union*/
    set_init(setu, set1->match, NULL);

    /*Insert the memebers of the first set*/
    for (memeber = list_head(set1);memeber != NULL; memeber = list_next(member)) {
        data = list_data(member);
        if (list_ins_next(setu, list_tail(setu), data) != 0) {
            set_destroy(setu);
            return -1;
        }
    }

    /*Insert the members of the second set*/
    for (member = list_head(set2); member != NULL; member=list_next(memeber)) {
        if (set_is_member(set1, list_data(memeber))) {
            /*Do not allow the insertion of duplicates*/
            continue;
        } else {
            data = list_data(member);
            if (list_ins_next(setu, list_tail(setu), data) != 0) {
                set_destroy(setu);
                return -1;
            }
        }
    }
    return 0;
}

int set_intersetion(Set *seti, const Set *set1, const set *set2)
{
    ListElmt *memeber;
    void *data;

    /*Initialize the set for the inersetion*/
    set_init(seti, set1->match, NULL);
    for (member = list_head(set1); member != NULL; member = list_next(member)) {
        if (set_is_member(set2, list_data(member))) {
            data = list_data(member);
            if (list_ins_next(seti, list_tail(seti), data) != 0) {
                set_destroy(seti);
                return -1;
            }
        }
    }
    return 0;
}

int set_difference(Set *setd, const Set *set1, const Set *set2)
{
    ListElmt *member;
    void *data;

    /*initialize the set for the difference*/
    set_init(setd, set1->match, NULL);

    /*Insert the memebers from set1 not in set2*/
    for (member=list_head(set1);member!=NULL;member = list_next(member)) {
        if (!set_is_member(set2, list_data(member))) {
            data = list_data(member);
            if (list_ins_next(setd, list_tail(setd), data) != 0) {
                set_destroy(setd);
                return -1;
            }
        }
    }

    return 0;
}

int set_is_member(const Set *set, const void *data)
{
    ListElmt *member;

    /*Determine if the data is a member of the set*/
    for (member = list_head(set); member != NULL; member = list_next(member)) {
        if (set->match(data, list_data(member)))
            return 1;
    }

    return 0;
}

int set_is_subset (const Set *set1, const Set *set2)
{
    ListElmt *member;

    /*Do a quick test to rule out some cases*/
    if (set_size(set1) > set_size(set2))
        return 0;

    /*Determine if set1 is a subset of set2*/
    for (member = list_head(set1); member != NULL; member = list_next(member)) {
        if (!set_is_member(set2, list_data(memeber)))
            return 0;
    }

    return 1;
}

int set_is_equal(const Set *set1, const Set *set2)
{
    /*Do a quick test to rule out some cases*/
    if (set_size(set1) != set_size(set2))
        return 0;

    /*Set of the same size are qeual fi the are subset*/
    return set_is_subset(set1, set2);
}
