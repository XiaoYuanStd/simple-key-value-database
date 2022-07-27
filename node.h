

// copi143保留所有权利

// ----------------------------------------------------------------------------------------------------
#pragma once
// ----------------------------------------------------------------------------------------------------

#include <stdarg.h>

#include "type.h"

// ----------------------------------------------------------------------------------------------------
// key-value 数据库节点

//#ifdef kv_database_src
#if 1

typedef struct kv_node *kv_node; // 指向 key-value数据库节点 的指针
typedef struct kv_node _kv_node; // key-value数据库节点

#else

typedef __kv_ptr kv_node; // 指向 key-value数据库节点 的指针
typedef struct            // key-value数据库节点
{
    __kv_4byte _0; // 键的hash值
    __kv_ptr _1;   // 键
    __kv_ptr _2;   // 父节点
    __kv_ptr _3;   // 上一个节点
    __kv_ptr _4;   // 下一个节点
    __kv_ptr _5;   // 第一个子节点
    __kv_ptr _6;   // 最后一个子节点
} _kv_node;

#endif

// 结构体

struct kv_node // key-value数据库节点
{
    unsigned int key_hash; // 键的hash值
    char *key;             // 键
#define kv_node_key_hash(node) ((node) ? ((struct kv_node *)(node))->key_hash : NULL)
#define kv_node_key(node) ((node) ? ((struct kv_node *)(node))->key : NULL)

    //
    kv_node parent; // 父节点
#define kv_node_parent(node) ((node) ? ((struct kv_node *)(node))->parent : NULL)

    //
    kv_node prev; // 上一个节点
    kv_node next; // 下一个节点
#define kv_node_next(node) ((node) ? ((struct kv_node *)(node))->next : NULL)
#define kv_node_prev(node) ((node) ? ((struct kv_node *)(node))->prev : NULL)

    // child 部分
    kv_node child;      // 第一个子节点
    kv_node last_child; // 最后一个子节点
    int child_num;      // 子节点的数量
#define kv_node_child(node) ((node) ? ((struct kv_node *)(node))->child : NULL)
#define kv_node_last_child(node) ((node) ? ((struct kv_node *)(node))->last_child : NULL)
#define kv_node_child_num(node) ((node) ? ((struct kv_node *)(node))->child_num : 0)

    // value 部分
    int type; // 值的类型
    int len;  // 值的长度
    union
    {
        unsigned long long value; // 节点的值

        void *v_data;              //
        long long v_int;           //
        unsigned long long v_uint; //
        double v_float;            //
        int v_bool;                //
        char *v_link;              //
        char *v_str;               //
    };
#define kv_node_type(node) ((node) ? ((struct kv_node *)(node))->type : 0)
#define kv_node_len(node) ((node) ? ((struct kv_node *)(node))->len : 0)
#define kv_node_value(type, node) ((node) ? ((struct kv_node *)(node))->v_##type : 0)
};

#ifdef kv_database_src
#undef kv_node_key_hash
#undef kv_node_key
#undef kv_node_parent
#undef kv_node_next
#undef kv_node_prev
#undef kv_node_child
#undef kv_node_last_child
#undef kv_node_child_num
#undef kv_node_type
#undef kv_node_len
#undef kv_node_value
#endif

// ----------------------------------------------------------------------------------------------------
// key-value 数据库节点操作函数

// 内存操作

kv_node kv_node_new(char *key);      // 创建新节点
void kv_node_set_null(kv_node node); // 删除节点数据并设为null
void kv_node_del(kv_node node);      // 删除节点 (仅释放当前节点与所有子节点内存)
void kv_node_remove(kv_node node);   // 删除节点

// 节点操作

int kv_node_is_null(kv_node node);          // 判断节点数据是否为null
void kv_node_path(kv_node node, char *buf); // 获取当前节点路径

// 兄弟节点、父节点操作

kv_node kv_node_root(kv_node node); // 获取根节点
void kv_node_detach(kv_node node);  // 分离当前节点与父节点、兄弟节点

// 子节点操作

int kv_node_child_add(kv_node node, kv_node child);    // 添加子节点 (确保 child 未连接任何父节点或兄弟节点)
kv_node kv_node_child_new(kv_node node, char *key);    // 创建指定项的子项 (key可以为NULL, 但只在创建链表的子项时应该这么做)
kv_node kv_node_child_idx(kv_node node, int idx);      // 获取指定位置的子节点
kv_node kv_node_child_lookup(kv_node node, char *key); // 获取节点的指定子节点 (不存在则返回NULL)
kv_node kv_node_child_get(kv_node node, char *key);    // 获取节点的指定子节点 (不存在则创建)
void kv_node_child_rm_idx(kv_node node, int idx);      //
void kv_node_child_rm(kv_node node, char *key);        // 删除节点的指定子节点

kv_node kv_node_child_lookupv(kv_node node, ...);
void kv_node_child_rmv(kv_node node, ...);

// 多层操作

kv_node kv_node_lookupv(kv_node node, int deepth, va_list va);
kv_node kv_node_lookupp(kv_node node, char *path);
kv_node kv_node_lookup(kv_node node, int deepth, ...);

kv_node kv_node_getv(kv_node node, int deepth, va_list va);
kv_node kv_node_getp(kv_node node, char *path);
kv_node kv_node_get(kv_node node, int deepth, ...);

kv_node kv_node_rmv(kv_node node, int deepth, va_list va);
kv_node kv_node_rmp(kv_node node, char *path);
kv_node kv_node_rm(kv_node node, int deepth, ...);

// ----------------------------------------------------------------------------------------------------
// end
