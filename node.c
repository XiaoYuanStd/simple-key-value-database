

// copi143保留所有权利

// ----------------------------------------------------------------------------------------------------
#pragma once
// ----------------------------------------------------------------------------------------------------

#include <stdarg.h>

#include "basic.c"

#include "node.h"

// ----------------------------------------------------------------------------------------------------
// 节点数据储存方式
//
//   | type | key | [len] | value | child_len | child |
//   |   1  |  v  |  [4]  |   v   |     4     |   v   |
//

// ----------------------------------------------------------------------------------------------------
// 节点操作函数

// 内存操作

// 创建新节点
kv_node kv_node_new(char *key)
{
    kv_node node = calloc(1, sizeof(_kv_node));
    node->key = kv_strcpy(key);
    node->key_hash = kv_hash(key, -1);
    return node;
}
// 删除节点数据并设为null
void kv_node_set_null(kv_node node)
{
    if (!node)
        return;
    if (node->type == kv_type_data)
        free(node->v_data);
    else if (node->type == kv_type_str)
        free(node->v_str);
    else if (node->type == kv_type_link)
        free(node->v_link);
    node->type = kv_type_null;
}
// 删除节点 (仅释放当前节点与所有子节点内存)
void kv_node_del(kv_node node)
{
    if (!node)
        return;

    free(node->key);
    kv_node_set_null(node);
    kv_node next;
    for (kv_node child = node->child; child; child = next)
    {
        next = child->next;
        kv_node_del(child);
    }

    free(node);
}
// 删除节点
void kv_node_remove(kv_node node)
{
    kv_node_detach(node);
    kv_node_del(node);
}

// 节点操作

// 判断节点数据是否为null
int kv_node_is_null(kv_node node)
{
    if (!node || node->type == kv_type_null)
        return 1;
    return 0;
}
// 获取当前节点路径
void kv_node_path(kv_node node, char *buf)
{
    if (!node || !buf)
        return;
    if (node->parent)
    {
        kv_node_path(node->parent, buf);
        if (strcmp(buf, "/") == 0)
            sprintf(buf, "/%s", node->key);
        else
            sprintf(buf + strlen(buf), "/%s", node->key);
    }
    else
        strcpy(buf, "/");
}

// 兄弟节点、父节点操作

// 获取根节点
kv_node kv_node_root(kv_node node)
{
    if (!node)
        return NULL;
    while (node->parent)
        node = node->parent;
    return node;
}
// 分离当前节点与父节点、兄弟节点
void kv_node_detach(kv_node node)
{
    if (!node)
        return;

    if (node->next)
        node->next->prev = node->prev;
    if (node->prev)
        node->prev->next = node->next;
    if (node->parent)
    {
        if (node->parent->child == node)
            node->parent->child = node->next;
        if (node->parent->last_child == node)
            node->parent->last_child = node->prev;
        node->parent->child_num--;
    }

    // 一键清空
    node->next = node->prev = node->parent = NULL;
}

// 子节点操作

// 添加子节点 (确保 child 未连接任何父节点或兄弟节点)
// 返回: 新添加的节点的位置
int kv_node_child_add(kv_node node, kv_node child)
{
    if (!node || !child)
        return -1;
    child->parent = node;
    if (node->last_child)
    {
        node->last_child->next = child;
        child->prev = node->last_child;
        node->last_child = child;
        node->child_num++;
    }
    else
    {
        node->child = node->last_child = child;
        node->child_num = 1;
    }
    return node->child_num - 1;
}
// 创建指定项的子项 (key可以为NULL, 但只在创建链表的子项时应该这么做)
// ps: 别故意创建不允许的名字
kv_node kv_node_child_new(kv_node node, char *key)
{
    kv_node child = kv_node_new(key);
    kv_node_child_add(node, child);
    return child;
}
// 获取指定位置的子节点
kv_node kv_node_child_idx(kv_node node, int idx)
{
    if (!node || idx < 0)
        return NULL;
    kv_node child = node->child;
    for (int i = 0; child && i < idx; i++)
        child = child->next;
    return child;
}
// 获取节点的指定子节点 (不存在则返回NULL)
// 不要用这个函数来查找链表的子节点
kv_node kv_node_child_lookup(kv_node node, char *key)
{
    if (!node || !key)
        return node;
    if (strcmp(key, ".") == 0)
        return node;
    if (strcmp(key, "..") == 0)
        return node->parent;
    uint hash = kv_hash(key, -1);
    for (kv_node child = node->child; child; child = child->next)
        if (child->key_hash == hash &&
            strcmp(child->key, key) == 0)
            return child;
    return NULL;
}
// 获取节点的指定子节点 (不存在则创建)
// 不要用这个函数来查找链表的子节点
kv_node kv_node_child_get(kv_node node, char *key)
{
    if (!node || !key)
        return node;
    kv_node child = kv_node_child_lookup(node, key);
    return child ? child : kv_node_child_new(node, key);
}
// 删除指定位置的子节点
void kv_node_child_rm_idx(kv_node node, int idx)
{
    kv_node_remove(kv_node_child_idx(node, idx));
}
// 删除节点的子节点
// 不要用这个函数来删除链表的子节点
// key传NULL会删除当前节点
void kv_node_child_rm(kv_node node, char *key)
{
    kv_node_remove(kv_node_child_lookup(node, key));
}

// 获取节点的指定子节点
kv_node kv_node_child_lookupv(kv_node node, ...)
{
    va_list va;
    va_start(va, node);
    if (node->type == kv_type_list)
        node = kv_node_child_idx(node, va_arg(va, int));
    else
        node = kv_node_child_lookup(node, va_arg(va, char *));
    va_end(va);
    return node;
}
// 删除节点的指定子节点
void kv_node_child_rmv(kv_node node, ...)
{
    va_list va;
    va_start(va, node);
    if (node->type == kv_type_list)
        kv_node_remove(kv_node_child_idx(node, va_arg(va, int)));
    else
        kv_node_remove(kv_node_child_lookup(node, va_arg(va, char *)));
    va_end(va);
}

// 多层操作

kv_node kv_node_lookupv(kv_node node, int deepth, va_list va)
{
    if (!node || deepth <= 0 || !va)
        return node;
    if (node->type == kv_type_list)
        node = kv_node_child_idx(node, va_arg(va, int));
    else
        node = kv_node_child_lookup(node, va_arg(va, char *));
    return kv_node_lookupv(node, deepth - 1, va);
}
kv_node _kv_node_lookupp(kv_node node, char *path)
{
    if (!node || !path || !*path)
        return node;
    if (*path == '/')
    {
        node = kv_node_root(node);
        while (*path == '/')
            path++;
        if (!*path)
            return node;
    }

    int _end = (*path == ':') ? 1 : 0;
    while (path[_end] != '\0' && path[_end] != '/' && path[_end] != ':')
        _end++;
    char c = path[_end];
    path[_end] = '\0';
    if (*path == ':')
    {
        int idx = -1;
        sscanf(path + 1, "%d", &idx);
        node = kv_node_child_idx(node, idx);
    }
    else
        node = kv_node_child_lookup(node, path);
    path[_end] = c;
    path += _end;

    while (*path == '/')
        path++;
    return _kv_node_lookupp(node, path);
}
kv_node kv_node_lookupp(kv_node node, char *path)
{
    path = kv_strcpy(path);
    node = _kv_node_lookupp(node, path);
    free(path);
    return node;
}
kv_node kv_node_lookup(kv_node node, int deepth, ...)
{
    va_list va;
    va_start(va, deepth);
    node = kv_node_lookupv(node, deepth, va);
    va_end(va);
    return node;
}

kv_node kv_node_getv(kv_node node, int deepth, va_list va)
{
    if (!node || deepth <= 0 || !va)
        return node;
    if (node->type == kv_type_list)
        node = kv_node_child_idx(node, va_arg(va, int));
    else
        node = kv_node_child_get(node, va_arg(va, char *));
    return kv_node_getv(node, deepth - 1, va);
}
kv_node _kv_node_getp(kv_node node, char *path)
{
    if (!node || !path || !*path)
        return node;
    if (*path == '/')
    {
        node = kv_node_root(node);
        while (*path == '/')
            path++;
        if (!*path)
            return node;
    }

    int _end = (*path == ':') ? 1 : 0;
    while (path[_end] != '\0' && path[_end] != '/' && path[_end] != ':')
        _end++;
    char c = path[_end];
    path[_end] = '\0';
    if (*path == ':')
    {
        int idx = -1;
        sscanf(path + 1, "%d", &idx);
        node = kv_node_child_idx(node, idx);
    }
    else
        node = kv_node_child_get(node, path);
    path[_end] = c;
    path += _end;

    while (*path == '/')
        path++;
    return _kv_node_getp(node, path);
}
kv_node kv_node_getp(kv_node node, char *path)
{
    path = kv_strcpy(path);
    node = _kv_node_getp(node, path);
    free(path);
    return node;
}
kv_node kv_node_get(kv_node node, int deepth, ...)
{
    va_list va;
    va_start(va, deepth);
    node = kv_node_getv(node, deepth, va);
    va_end(va);
    return node;
}

kv_node kv_node_rmv(kv_node node, int deepth, va_list va)
{
    if (!node || deepth <= 0 || !va)
    {
        kv_node_remove(node);
        return;
    }
    if (node->type == kv_type_list)
        node = kv_node_child_idx(node, va_arg(va, int));
    else
        node = kv_node_child_lookup(node, va_arg(va, char *));
    kv_node_rmv(node, deepth - 1, va);
}
kv_node _kv_node_rmp(kv_node node, char *path)
{
    if (!node || !path || !*path)
    {
        kv_node_remove(node);
        return;
    }
    if (*path == '/')
    {
        node = kv_node_root(node);
        while (*path == '/')
            path++;
        if (!*path)
        {
            kv_node_remove(node);
            return;
        }
    }

    int _end = (*path == ':') ? 1 : 0;
    while (path[_end] != '\0' && path[_end] != '/' && path[_end] != ':')
        _end++;
    char c = path[_end];
    path[_end] = '\0';
    if (*path == ':')
    {
        int idx = -1;
        sscanf(path + 1, "%d", &idx);
        node = kv_node_child_idx(node, idx);
    }
    else
        node = kv_node_child_lookup(node, path);
    path[_end] = c;
    path += _end;

    while (*path == '/')
        path++;
    _kv_node_rmp(node, path);
}
kv_node kv_node_rmp(kv_node node, char *path)
{
    path = kv_strcpy(path);
    node = _kv_node_rmp(node, path);
    free(path);
    return node;
}
kv_node kv_node_rm(kv_node node, int deepth, ...)
{
    va_list va;
    va_start(va, deepth);
    kv_node_rmv(node, deepth, va);
    va_end(va);
}

// 旧的查找函数
#if 0
#define __kv_node__p__(_funcname_, rets_type)                   \
    rets_type kv_node_##_funcname_##p(kv_node node, char *path) \
    {                                                           \
        if (!node || !path)                                     \
            return node;                                        \
        path = bot_strcpy(path);                                \
        int begin = 0, end = 0;                                 \
        if (path[begin] == '/')                                 \
        {                                                       \
            node = kv_node_root(node);                          \
            begin++;                                            \
        }
#define __kv_node__p__node_get__(_how_)                              \
    while (path[begin])                                              \
    {                                                                \
        for (; path[end] != '\0' && path[end] != '/'; end++)         \
            ;                                                        \
        char c = path[end];                                          \
                                                                     \
        if (begin != end)                                            \
        {                                                            \
            path[end] = '\0';                                        \
            if (!(node = kv_node_child_##_how_(node, path + begin))) \
                break;                                               \
        }                                                            \
                                                                     \
        if (!c)                                                      \
            break;                                                   \
        begin = end += 1;                                            \
    }
#define __kv_node__p__end__(...) \
    free(path);                  \
    return __VA_ARGS__;          \
    }

__kv_node__p__(lookup, kv_node);
__kv_node__p__node_get__(lookup);
__kv_node__p__end__(node);

__kv_node__p__(get, kv_node);
__kv_node__p__node_get__(get);
__kv_node__p__end__(node);

__kv_node__p__(rm, void);
__kv_node__p__node_get__(lookup);
if (node)
    kv_node_child_rm(node->parent, node->key);
__kv_node__p__end__();
#endif
#if 0
#define __kv_node__v__(_funcname_, rets_type)                       \
    rets_type kv_node_##_funcname_##v(kv_node node, char *fmt, ...) \
    {                                                               \
        if (!node)                                                  \
            return NULL;                                            \
        if (!fmt || *fmt == '\0')                                   \
            return node;                                            \
        va_list va;                                                 \
        int va_len = strlen(fmt);                                   \
        va_start(va, va_len);
#define __kv_node__v__node_get__(_how_)                             \
    for (int i = 0; i < va_len && node != NULL; i++)                \
    {                                                               \
        if (fmt[i] == 's')                                          \
            node = kv_node_child_##_how_(node, va_arg(va, char *)); \
        else                                                        \
        {                                                           \
            char key[64] = {0};                                     \
            if (fmt[i] == 'i')                                      \
                sprintf(key, "%d", va_arg(va, int));                \
            else if (fmt[i] == 'u')                                 \
                sprintf(key, "%u", va_arg(va, uint));               \
            else if (fmt[i] == 'l')                                 \
                sprintf(key, "%lld", va_arg(va, Long));             \
            else if (fmt[i] == 'U')                                 \
                sprintf(key, "%llu", va_arg(va, ulong));            \
            else if (fmt[i] == 'x')                                 \
                sprintf(key, "%x", va_arg(va, uint));               \
            else if (fmt[i] == 'X')                                 \
                sprintf(key, "%llx", va_arg(va, ulong));            \
            else                                                    \
                sprintf(key, "0x%x", va_arg(va, int));              \
            node = kv_node_child_##_how_(node, key);                \
        }                                                           \
    }
#define __kv_node__v__end__(...) \
    va_end(va);                  \
    return __VA_ARGS__;          \
    }

__kv_node__v__(lookup, kv_node);
__kv_node__v__node_get__(lookup);
__kv_node__v__end__(node);

__kv_node__v__(get, kv_node);
__kv_node__v__node_get__(get);
__kv_node__v__end__(node);

__kv_node__v__(rm, void);
va_len--;
__kv_node__v__node_get__(lookup);

if (fmt[va_len] == 's')
    kv_node_child_rm(node, va_arg(va, char *));
else
{
    char key[64] = {0};
    if (fmt[va_len] == 'i')
        sprintf(key, "%d", va_arg(va, int));
    else if (fmt[va_len] == 'u')
        sprintf(key, "%u", va_arg(va, uint));
    else if (fmt[va_len] == 'l')
        sprintf(key, "%lld", va_arg(va, Long));
    else if (fmt[va_len] == 'U')
        sprintf(key, "%llu", va_arg(va, ulong));
    else if (fmt[va_len] == 'x')
        sprintf(key, "%x", va_arg(va, uint));
    else if (fmt[va_len] == 'X')
        sprintf(key, "%llx", va_arg(va, ulong));
    else
        sprintf(key, "0x%x", va_arg(va, int));
    kv_node_child_rm(node, key);
}

__kv_node__v__end__();
#endif

// ----------------------------------------------------------------------------------------------------
// end
