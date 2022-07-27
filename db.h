

// copi143保留所有权利

// ----------------------------------------------------------------------------------------------------
#pragma once
// ----------------------------------------------------------------------------------------------------
// include

#include "type.h"
#include "node.h"
#include "node-data.h"
#include "node-io.h"

void kv_print_child(kv_node node, int include_child);
void kv_print(kv_node node, int include_child);

// 获取 设置

#define kv_getv(node, type, def, ...) (kv_value_get_##type(kv_node_lookupv(node, __VA_ARGS__), def))
#define kv_setv(node, type, value, ...) (kv_value_set_##type(kv_node_getv(node, __VA_ARGS__), value))

#define kv_getp(node, type, def, path) (kv_value_get_##type(kv_node_lookupp(node, path), def))
#define kv_setp(node, type, value, path) (kv_value_set_##type(kv_node_getp(node, path), value))

// 加载 保存

kv_node kv_load(int fd);
void kv_save(kv_node node, int fd);

kv_node kv_load_file(char *filename);
int kv_save_file(kv_node node, char *filename);

// ----------------------------------------------------------------------------------------------------
// end
