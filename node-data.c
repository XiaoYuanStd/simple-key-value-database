

// copi143保留所有权利

// ----------------------------------------------------------------------------------------------------
#pragma once
// ----------------------------------------------------------------------------------------------------

#include "basic.c"

#include "node-data.h"

// ----------------------------------------------------------------------------------------------------
// key-value 数据库节点数据操作

// 数据读取部分

#define __kv_value_get__(__type__, __rets_type__)                          \
    __rets_type__ kv_value_get_##__type__(kv_node node, __rets_type__ def) \
    {                                                                      \
        if (node)                                                          \
            switch (node->type)                                            \
            {
#define __kv_value_get_end__ \
    }                        \
    ;                        \
    return def;              \
    }

// 复制节点数据
int kv_value_copy_data(kv_node node, void *data, int len)
{
    if (!node)
        return -1;
    len = len < node->len ? len : node->len;
    switch (node->type)
    {
    case kv_type_null:
        return 0;
    case kv_type_data:
        memcpy(data, node->v_data, len);
        break;
    case kv_type_str:
        memcpy(data, node->v_str, len);
        break;
    default:
        memcpy(data, &node->value, len);
        break;
    };
    return len;
}
// 复制节点字符串数据
void kv_value_copy_str(kv_node node, void *str, int len)
{
}

// 获取节点数据
__kv_value_get__(data, void *);
case kv_type_null:
return def;
case kv_type_data:
return node->v_data;
case kv_type_str:
return node->v_str;
default:
return &node->value;
__kv_value_get_end__;

// 获取 整数
__kv_value_get__(int, long long);
case kv_type_int:
return node->v_int;
case kv_type_uint:
return node->v_uint;
case kv_type_float:
return node->v_float;
case kv_type_bool:
return kv_bool(node->v_bool);
case kv_type_str:
long long value = def;
sscanf(node->v_str, "%lld", &value);
return value;
__kv_value_get_end__;

// 获取 无符号整数
__kv_value_get__(uint, unsigned long long);
case kv_type_int:
return node->v_int;
case kv_type_uint:
return node->v_uint;
case kv_type_float:
return node->v_float;
case kv_type_bool:
return kv_bool(node->v_bool);
case kv_type_str:
unsigned long long value = def;
sscanf(node->v_str, "%llu", &value);
return value;
__kv_value_get_end__;

// 获取 浮点数
__kv_value_get__(float, double);
case kv_type_int:
return node->v_int;
case kv_type_uint:
return node->v_uint;
case kv_type_float:
return node->v_float;
case kv_type_bool:
return kv_bool(node->v_bool);
case kv_type_str:
double value = def;
sscanf(node->v_str, "%lf", &value);
return value;
__kv_value_get_end__;

__kv_value_get__(bool, int);
case kv_type_int:
return kv_bool(node->v_int);
case kv_type_uint:
return kv_bool(node->v_uint);
case kv_type_float:
return kv_bool(node->v_float);
case kv_type_bool:
return kv_bool(node->v_bool);
case kv_type_str:
if (strcmp(node->v_str, "true") == 0)
    return 1;
else if (strcmp(node->v_str, "false") == 0)
    return 0;
else
{
    int value = def;
    sscanf(node->v_str, "%d", &value);
    return kv_bool(value);
}
__kv_value_get_end__;

char *kv_value_get_str(kv_node node, char *def)
{
    if (node && node->type == kv_type_str)
        return node->v_str;
    return def;
}

// 数据写入部分

#define __kv_value_set__(__type__, ...)                     \
    void kv_value_set_##__type__(kv_node node, __VA_ARGS__) \
    {                                                       \
        if (!node)                                          \
            return;                                         \
        kv_node_set_null(node);                             \
        node->type = kv_type_##__type__;

#define __kv_value_set_end__ }

__kv_value_set__(data, void *data, int len);
node->v_data = kv_datacpy(data, len);
node->len = len;
__kv_value_set_end__;

__kv_value_set__(int, long long value);
node->v_int = value;
node->len = sizeof(long long);
__kv_value_set_end__;

__kv_value_set__(uint, unsigned long long value);
node->v_uint = value;
node->len = sizeof(unsigned long long);
__kv_value_set_end__;

__kv_value_set__(float, double value);
node->v_float = value;
node->len = sizeof(double);
__kv_value_set_end__;

__kv_value_set__(bool, int value);
node->v_uint = kv_bool(value);
node->len = 1;
__kv_value_set_end__;

__kv_value_set__(str, char *str);
node->v_str = kv_strcpy(str);
node->len = str ? strlen(str) : 0;
__kv_value_set_end__;

__kv_value_set__(null, void *useless_arg);
__kv_value_set_end__;
