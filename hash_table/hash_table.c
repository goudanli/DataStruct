#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"
#include "prime.h"

static const int HT_PRIME_1 = 151;
static const int HT_PRIME_2 = 163;
static ht_item HT_DELETED_ITEM = { NULL,NULL };

//创建元素
static ht_item* ht_new_item(const char *k, const char * v)
{
    ht_item * i = malloc(sizeof(ht_item));
    i->key = _strdup(k);
    i->value = _strdup(v);
    return i;
}

static ht_hash_table* ht_new_sized(const int size_index)
{
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    ht->size_index = size_index;
    int size = 50 << ht->size_index;
    ht->size = next_prime(size);
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

//调整hash_table
static void ht_resize(ht_hash_table*ht, const int direction)
{
    const int new_size_index = ht->size_index + direction;
    if (new_size_index < 0)
        return;
    ht_hash_table *new_ht = ht_new_sized(new_size_index);
    
    //将原来的数据复制到新的hash_table
    for (int i = 0; i < ht->size; ++i) {
        ht_item * item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, item->key, item->value);
        }
    }

    ht->size_index = new_ht->size_index;
    ht->count = new_ht->count;

    //new_ht->size换为原来的size，后面会把new_ht指向原来的ht，然后删除new_ht
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}

ht_hash_table * ht_new()
{
    //ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    //ht->size = 53;
    //ht->count = 0;
    //申请53个sizeof(ht_item*)空间
    //sizeof(ht_item*) 大小为4，是一个指针
    //ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht_new_sized(0);
}

static void ht_del_item(ht_item * i)
{
    free(i->key);
    free(i->value);
    free(i);
}

void ht_del_hash_table(ht_hash_table * ht)
{
    for (int i = 0; i < ht->size; ++i)
    {
        ht_item * item = ht->items[i];
        if (item != NULL) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}

//m是哈希表的大小，a是初始值
static int ht_hash(const char * s, const int a, const int m)
{
    long hash = 0;
    int len = strlen(s);
    for (int i = 0; i < len; ++i)
    {
        hash += (long)pow(a, len - (i + 1)) * s[i];
        hash = hash % m;
    }
    return (int)hash;
}

static int ht_get_hash(const char * s, const int num_buckets, const int attempt)
{
    int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

//插入
void ht_insert(ht_hash_table *ht, const char * key, const char * value)
{
    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        ht_resize(ht, 1);
    }
    ht_item *item = ht_new_item(key, value);
    int index = ht_get_hash(item->key,ht->size,0);
    ht_item * cur_item = ht->items[index];
    int i = 1;
    //查找空闲buket
    while (cur_item != NULL) {
        if (cur_item != &HT_DELETED_ITEM)
        {
            if (strcmp(cur_item->key, key) == 0) {
                ht_del_item(cur_item);
                ht->items[index] = item;
                return;
            }
        }
        index = ht_get_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        ++i;
    }
    ht->items[index] = item;
    ht->count++;
}

char * ht_search(ht_hash_table * ht, const char * key)
{
    int index = ht_get_hash(key,ht->size,0);
    ht_item * item =  ht->items[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                return item->value;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        ++i;
    }
    return NULL;
}



//删除
void ht_delete(ht_hash_table * ht, const char * key)
{
    //缩小hash_table
    const int load = ht->count * 100 / ht->size;
    if (load < 10) {
        ht_resize(ht, -1);
    }
    int index = ht_get_hash(key, ht->size, 0);
    ht_item * item =  ht->items[index];
    int i = 1;
    while (item != NULL)
    {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        ++i;
    }
    ht->count--;
}





