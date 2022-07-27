

// copi143保留所有权利

// ----------------------------------------------------------------------------------------------------
#pragma once
// ----------------------------------------------------------------------------------------------------

#include "node.h"

// ----------------------------------------------------------------------------------------------------
// key-value 数据库节点数据操作

int kv_value_copy_data(kv_node node, void *data, int len); // 复制节点数据
void kv_value_copy_str(kv_node node, void *str, int len);  // 复制节点字符串数据

#define __kv_value_get__(__type__, __rets_type__) \
    __rets_type__ kv_value_get_##__type__(kv_node node, __rets_type__ def)

__kv_value_get__(data, void *);
__kv_value_get__(int, long long);
__kv_value_get__(uint, unsigned long long);
__kv_value_get__(float, double);
__kv_value_get__(bool, int);
__kv_value_get__(str, char *);

#undef __kv_value_get__

#define __kv_value_set__(__type__, ...) \
    void kv_value_set_##__type__(kv_node node, __VA_ARGS__)

__kv_value_set__(data, void *data, int len);
__kv_value_set__(int, long long value);
__kv_value_set__(uint, unsigned long long value);
__kv_value_set__(float, double value);
__kv_value_set__(bool, int value);
__kv_value_set__(str, char *str);
__kv_value_set__(null, void *useless_arg);

#undef __kv_value_set__

#define kv_value_get(type, node, def) (kv_value_get_##type(node, def))
#define kv_value_set(type, node, value) (kv_value_set_##type(node, value))

// ----------------------------------------------------------------------------------------------------
// end
