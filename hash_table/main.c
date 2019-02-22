#include "hash_table.h"
#include <stdlib.h>
#include <string.h>

int tests_run = 0;

#define mu_assert(message,test) do{if(!(test)) return message;} while(0)
#define mu_run_test(test) do {char * message = test();tests_run++;\
            if(message) return message;}while(0)

static char * test_insert() {
    ht_hash_table * ht = ht_new();
    ht_insert(ht, "k", "v");
    int count = 0;
    for (int i = 0; i < ht->size; ++i) {
        if (ht->items[i] != NULL) {
            count++;
        }
    }
    mu_assert("error,num items in ht !=1", count == 1);

    for (int i = 0; i < ht->size; ++i)
    {
        if (ht->items[i] != NULL) {
            ht_item * cur_item = ht->items[i];
            mu_assert("error, key != k", strcmp(cur_item->key, "k") == 0);
            mu_assert("error, value != v", strcmp(cur_item->value, "v") == 0);
        }
    }
    ht_del_hash_table(ht);
    return 0;
}

int main()
{
    //ht_hash_table * ht = ht_new();
    //ht_del_hash_table(ht);
    test_insert();
}