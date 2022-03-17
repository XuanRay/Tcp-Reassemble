/*   file    : file-descriptor
 *   author  : rayss
 *   date    : 2022.03.13
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : 哈希表的实现，冲突解决采用rehash法(非拉链法)                   
 */


#include <stdio.h>
#include "util.h"
#include "hash.h"


size_t def_hashfunc(const char *key)
{
    u_int64_t hash = 5381;
    _int32_t c;

    while (0 != (c = *key++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}


inline size_t hash_key(HASHFUNC hashfunc, const char *key)
{
    hashfunc = hashfunc ? hashfunc : def_hashfunc;
    return hashfunc(key);
}


inline void free_data(FREEFUNC freefunc, void *data)
{
    if (freefunc)
        freefunc(data);  
    else
        free(data);
}


HASHTBL *hashtbl_create(size_t size, HASHFUNC hashfunc, FREEFUNC freefunc)
{
    HASHTBL *hashtbl = (HASHTBL *)mymalloc(sizeof(HASHTBL));
    hashtbl->nodes = (HASHNODE **)mycalloc(size, sizeof(HASHNODE *));
    hashtbl->size = size;
    hashtbl->hashfunc = hashfunc;
    hashtbl->freefunc = freefunc;
    return hashtbl;
}


size_t hashtbl_capacity(HASHTBL *hashtbl)
{
    size_t count = 0;
    for (size_t i = 0; i < hashtbl->size; i++)
        if (hashtbl->nodes[i])
            count++;
    return count;
}


size_t hashtbl_nodes_length(HASHNODE *node)
{
    size_t count = 0;
    for (; node; node = node->next)
        count++;
    return count;
}


void hashtbl_empty(HASHTBL *hashtbl)
{
    for (size_t i = 0; i < hashtbl->size; ++i)
    {
        HASHNODE *node = hashtbl->nodes[i];
        if (!node)
            continue;
        hashtbl_remove_n(node, 0, hashtbl->freefunc);
    }
    free(hashtbl->nodes);
}

void hashtbl_destroy(HASHTBL *hashtbl)
{
    hashtbl_empty(hashtbl);
    free(hashtbl);
}

size_t hashtbl_index(HASHTBL *hashtbl, const char *key)
{
    size_t hash = hash_key(hashtbl->hashfunc, key) % hashtbl->size;
    HASHNODE *node = hashtbl->nodes[hash];

    if (node && !strcmp(node->key, key))
        return hash;
    return (size_t)(-1);
}

inline tBool hashtbl_key_exist(HASHTBL *hashtbl, const char *key)
{
    return hashtbl_index(hashtbl, key) != (size_t)(-1);
}

HASHNODE *hashtbl_get(HASHTBL *hashtbl, const char *key)
{
    size_t hash = hashtbl_index(hashtbl, key);
    if (hash == (size_t)(-1))
        return NULL;
    return hashtbl->nodes[hash];
}

void hashtbl_rehash(HASHTBL *hashtbl)
{
    size_t newsize = 2 * hashtbl->size;
    HASHNODE **oldnodes = hashtbl->nodes;
    HASHNODE **newnodes = (HASHNODE **)mycalloc(newsize, sizeof(HASHNODE *));

    for (int i = 0; i < (int)hashtbl->size; i++)
    {
        if (!oldnodes[i])
            continue;
        size_t hash = hash_key(hashtbl->hashfunc, oldnodes[i]->key) % newsize;
        if (newnodes[hash])      /* 再哈希 */
        {
            i = -1;
            newsize *= 2;
            free(newnodes);
            newnodes = (HASHNODE **)mycalloc(newsize, sizeof(HASHNODE *));
        }
        else
        {
            newnodes[hash] = oldnodes[i];
        }
    }
    hashtbl->nodes = newnodes;
    hashtbl->size = newsize;
    free(oldnodes);
}

size_t hashtbl_insert(HASHTBL *hashtbl, const char *key, void *data)
{
    size_t hash = hash_key(hashtbl->hashfunc, key) % hashtbl->size;
    HASHNODE *node = hashtbl->nodes[hash];

    while (node)
    {
        if (!strcmp(node->key, key))
            break;
        hashtbl_rehash(hashtbl);
        hash = hash_key(hashtbl->hashfunc, key) % hashtbl->size;
        node = hashtbl->nodes[hash];
    }

    node = (HASHNODE *)mymalloc(sizeof(HASHNODE));
    node->key = mystrdup(key);

    node->data = data;
    node->next = hashtbl->nodes[hash];     /* 把key相同的结点拉链起来 */
    hashtbl->nodes[hash] = node;

    return hash;
}

size_t hashtbl_remove_n(HASHNODE *node, size_t count, FREEFUNC freefunc)
{
    size_t del_count = 0;
    while (node)
    {
        HASHNODE *next = node->next;
        free(node->key);
        free_data(freefunc, node->data);
        free(node);
        node = next;

        del_count++;
        if (!(--count))
            break;
    }
    return del_count;
}

size_t hashtbl_remove(HASHTBL *hashtbl, const char *key)
{
    size_t hash = hash_key(hashtbl->hashfunc, key) % hashtbl->size;
    HASHNODE *node = hashtbl->nodes[hash];
    size_t del_count = hashtbl_remove_n(node, 0, hashtbl->freefunc);
    hashtbl->nodes[hash] = NULL;
    return del_count;
}

HASHITR hashtbl_iterator(HASHTBL *hashtbl)
{
    HASHITR itr;
    size_t i;
    for (i = 0; i < hashtbl->size; i++)
        if (hashtbl->nodes[i])
            break;
    itr.cindex = i;
    itr.nindex = i;
    itr.hashtbl = hashtbl;
    return itr;
}

HASHNODE *hashtbl_next(HASHITR *pitr)
{
    HASHTBL *hashtbl = pitr->hashtbl;
    size_t i;
    for (i = pitr->nindex; i < hashtbl->size; i++)
        if (hashtbl->nodes[i])
            break;
    pitr->cindex = i;
    pitr->nindex = i + 1;
    if (pitr->nindex >= hashtbl->size)
        return NULL;
    return hashtbl->nodes[i];
}



/*
 * return a single instance of hash table
 */
HASHTBL *get_hash_table()
{
    static HASHTBL *hashtbl = NULL;  /* 静态变量保证程序执行过程中一直存在，即实现singleton */
    if (hashtbl == NULL)
        hashtbl = hashtbl_create(DEFAULT_HASH_SIZE, def_hashfunc, NULL);
    else if (hashtbl->size == 0)
        hashtbl = hashtbl_create(DEFAULT_HASH_SIZE, hashtbl->hashfunc, hashtbl->freefunc);
    return hashtbl;
}

inline HASHTBL *set_hash_table(HASHFUNC hashfunc, FREEFUNC freefunc)
{
    HASHTBL *hashtbl = get_hash_table();
    hashtbl->hashfunc = hashfunc;
    hashtbl->freefunc = freefunc;
    return hashtbl;
}

inline HASHTBL *set_hash_hashfunc(HASHFUNC hashfunc)
{
    HASHTBL *hashtbl = get_hash_table();
    hashtbl->hashfunc = hashfunc;
    return hashtbl;
}

inline HASHTBL *set_hash_freefunc(FREEFUNC freefunc)
{
    HASHTBL *hashtbl = get_hash_table();
    hashtbl->freefunc = freefunc;
    return hashtbl;
}

inline void empty_hash_table()
{
    HASHTBL *hashtbl = get_hash_table();
    hashtbl_empty(hashtbl);
    hashtbl->size = 0;
}

inline void destroy_hash_table()
{
    empty_hash_table();
    free(get_hash_table());
}

inline size_t get_hash_index(const char *key)
{
    return hashtbl_index(get_hash_table(), key);
}

inline tBool is_hash_key_exist(const char *key)
{
    return hashtbl_key_exist(get_hash_table(), key);
}

inline HASHNODE *get_hash_nodes(const char *key)
{
    return hashtbl_get(get_hash_table(), key);
}

inline size_t insert_hash_node(const char *key, void *data)
{
    return hashtbl_insert(get_hash_table(), key, data);
}

inline size_t remove_hash_node(HASHNODE *node)
{
    return hashtbl_remove_n(node, 1, get_hash_table()->freefunc); // TODO error!!! delete a node in a linkedlist
} 

inline size_t remove_hash_nodes(const char *key)
{
    return hashtbl_remove(get_hash_table(), key);
}


#ifdef TEST_HASH

#include <string.h>
#include <assert.h>

int main(int argc, char **argv)
{
    int num = 10;

    HASHTBL *hashtbl = get_hash_table();
    set_hash_table(hashtbl->hashfunc, myfree);

    for (int i = 0; i < num; i++) {
        char *key = (char *)mymalloc(1000);
        char *data = (char *)mymalloc(1000);
        sprintf(key, "%d", i);
        sprintf(data, "%d", num - i);
        insert_hash_node(key, data);
    }

    HASHITR hashitr = hashtbl_iterator(hashtbl);
    HASHNODE *node;
    while (node = hashtbl_next(&hashitr)) {
        size_t i = hashitr.cindex;
        size_t hash = get_hash_index(node->key);
        char *key = node->key;
        assert(!strcmp(key, hashtbl->nodes[i]->key));
        assert(!strcmp((char *)node->data, (char *)hashtbl->nodes[i]->data));
        node = hashtbl->nodes[hash];
        assert(!strcmp(key, node->key));
        printf("%s\n", node->key);
    }

    empty_hash_table();
    destroy_hash_table();
    return 0;
}
#endif /* TEST_HASH */