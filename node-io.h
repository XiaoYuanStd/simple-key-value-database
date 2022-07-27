

// copi143保留所有权利

// ----------------------------------------------------------------------------------------------------
#pragma once
// ----------------------------------------------------------------------------------------------------

#include "node.h"

// ----------------------------------------------------------------------------------------------------
// key-value 数据库节点文件io

// 输出

void kv_node_print_child(kv_node node, int, int include_child);
void kv_node_print(kv_node node, int, int include_child);
#ifndef kv_database_src
#define kv_node_print_child(node, include_child) (kv_node_print_child(node, 0, include_child))
#define kv_node_print(node, include_child) (kv_node_print(node, 0, include_child))
#endif

// 加载 保存

kv_node kv_node_load(kv_node parent, int fd); // 从数据流加载
void kv_node_save(kv_node node, int fd);      // 保存到数据流

// ----------------------------------------------------------------------------------------------------
// end
