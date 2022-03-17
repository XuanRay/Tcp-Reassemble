/*   file    : file-descriptor
 *   author  : rayss
 *   date    : 2022.03.13
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : 哈希表接口                   
 */

#ifndef __HASH_H
#define __HASH_H


#include <stdlib.h>
#include <string.h>
#include "types.h"


#define DEFAULT_HASH_SIZE 10000


typedef size_t (*HASHFUNC)(const char *);     /* Function Pointer 实现类似多态效果 */
typedef void (*FREEFUNC)(void *);


/* 哈希表结点 */
typedef struct hashnode_s {
	char *key;
	void *data;
	struct hashnode_s *next;
} HASHNODE;


/* 哈希表结构 */
typedef struct hashtable_s {
	size_t size;
	HASHNODE **nodes;
	HASHFUNC hashfunc;
    FREEFUNC freefunc;
} HASHTBL;


/* 指向哈希表的结构 */
typedef struct hashitr_s{
    size_t cindex;
    size_t nindex;
    HASHTBL *hashtbl;
} HASHITR;


//默认的哈希函数
extern size_t default_hashfunc(const char *key);

//hashfunc作用在key上得到的hash value
extern size_t hash_key(HASHFUNC hashfunc, const char *key);

//free
extern void free_data(FREEFUNC freefunc, void *data);

//创建一个哈希表
extern HASHTBL *hashtbl_create(size_t size, HASHFUNC hashfunc, FREEFUNC freefunc);

//根据key获得哈希结点
extern HASHNODE *hashtbl_get(HASHTBL *hashtbl, const char *key);

//根据key获得哈希索引
extern size_t hashtbl_index(HASHTBL *hashtbl, const char *key);

//判断key是否存在哈希表中
extern tBool hashtbl_key_exist(HASHTBL *hashtbl, const char *key);

//插入
extern size_t hashtbl_insert(HASHTBL *hashtbl, const char *key, void *data);

//删除
extern size_t hashtbl_remove(HASHTBL *hashtbl, const char *key);

//删除一个node拉着的那个链表
extern size_t hashtbl_remove_n(HASHNODE *node, size_t count, FREEFUNC freefunc);

//置空哈希表
extern void hashtbl_empty(HASHTBL *hashtbl);

//销毁哈希表
extern void hashtbl_destroy(HASHTBL *hashtbl);

//
extern HASHITR hashtbl_iterator(HASHTBL *hashtbl);

//
extern HASHNODE *hashtbl_next(HASHITR *pitr);



/* ---------------------------------------------------------------- */
/*             global single hash table 对外提供的接口                */
/* ---------------------------------------------------------------- */
extern HASHTBL *get_hash_table();
extern HASHTBL *set_hash_table(HASHFUNC hashfunc, FREEFUNC freefunc);
extern HASHTBL *set_hash_hashfunc(HASHFUNC hashfunc);
extern HASHTBL *set_hash_freefunc(FREEFUNC freefunc);
extern HASHNODE *get_hash_nodes(const char *key);
extern size_t get_hash_index(const char *key);
extern tBool is_hash_key_exist(const char *key);
extern size_t insert_hash_node(const char *key, void *data);
extern size_t remove_hash_node(HASHNODE *node);
extern size_t remove_hash_nodes(const char *key);
extern void empty_hash_table();
extern void destroy_hash_table();



#endif //__HASH_H