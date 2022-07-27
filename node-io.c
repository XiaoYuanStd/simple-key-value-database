

// copi143保留所有权利

// ----------------------------------------------------------------------------------------------------
#pragma once
// ----------------------------------------------------------------------------------------------------

#include "basic.c"

#include "node-io.h"

// ----------------------------------------------------------------------------------------------------
// key-value 数据库节点文件io

// 输出

void kv_node_print_child(kv_node node, int level, int include_child)
{
    for (node = node ? node->child : NULL; node; node = node->next)
        kv_node_print(node, level, include_child);
}
void kv_node_print(kv_node node, int level, int include_child)
{
    printf("\033[0m");
    if (!node)
        return;

    for (int i = 0; i < level; i++)
        printf("  ");

    if (level % 3 == 0)
        printf("\033[38;2;255;0;128m");
    else if (level % 3 == 1)
        printf("\033[38;2;255;0;255m");
    else
        printf("\033[38;2;128;0;255m");
    printf("%s", node->key);

    if (node->type == kv_type_data)
        printf("\033[0m p:\033[38;2;128;128;255m%s", node->v_data);
    else if (node->type == kv_type_link)
        printf("\033[0m -> \033[38;2;128;128;255m%s", node->v_link);
    else
    {
        printf("\033[0m : ");
        switch (node->type)
        {
        case kv_type_int:
            printf("\033[38;2;0;0;255m%lld", node->v_int);
            break;
        case kv_type_uint:
            printf("\033[38;2;0;0;255m%lluu", node->v_uint);
            break;
        case kv_type_float:
            printf("\033[38;2;0;128;255m%lf", node->v_float);
            break;
        case kv_type_bool:
            printf("\033[38;2;255;128;0m");
            if (node->v_bool)
                printf("true");
            else
                printf("false");
            break;
        case kv_type_str:
            printf("\033[38;2;255;64;0m%s", node->v_str);
            break;
        };
    }
    printf("\033[0m");

    if (!include_child || !node->child)
        goto end;

    printf(" {\n");
    for (node = node->child; node; node = node->next)
        kv_node_print(node, level + 1, 1);
    for (int i = 0; i < level; i++)
        printf("  ");
    printf("}");

end:
    printf("\n");
}

// 加载 保存

// 从数据流加载
kv_node kv_node_load(kv_node parent, int fd)
{
    if (fd < 0)
        return NULL;
    kv_node node = kv_node_new(NULL);
    kv_node_child_add(parent, node);

    read(fd, &node->type, 1);

    node->key = kv_read_str(fd);
    node->key_hash = kv_hash(node->key, -1);

    switch (node->type)
    {
    case kv_type_data:
        read(fd, &node->len, 4);
        read(fd, node->v_data, node->len);
        break;
    case kv_type_str:
        node->v_str = kv_read_str(fd);
        node->len = strlen(node->v_str);
        break;
    case kv_type_link:
        node->v_link = kv_read_str(fd);
        node->len = strlen(node->v_link);
        break;
    case kv_type_int:
        read(fd, &node->v_int, 8);
        node->len = 8;
        break;
    case kv_type_uint:
        read(fd, &node->v_uint, 8);
        node->len = 8;
        break;
    case kv_type_float:
        read(fd, &node->v_float, 8);
        node->len = 8;
        break;
    case kv_type_bool:
        read(fd, &node->v_bool, 1);
        node->len = 1;
        break;
    };

    int childnum;
    read(fd, &childnum, 4);

    for (int i = 0; i < childnum; i++)
        kv_node_load(node, fd);

    return node;
}
// 保存到数据流
void kv_node_save(kv_node node, int fd)
{
    if (!node || fd < 0)
        return;

    write(fd, &node->type, 1);

    if (node->key)
        write(fd, node->key, strlen(node->key) + 1);
    else
        write(fd, "", 1);

    switch (node->type)
    {
    case kv_type_data:
        write(fd, &node->len, 4);
        write(fd, node->v_data, node->len);
        break;
    case kv_type_str:
        if (node->v_str)
            write(fd, node->v_str, strlen(node->v_str) + 1);
        else
            write(fd, "", 1);
        break;
    case kv_type_link:
        if (node->v_link)
            write(fd, node->v_link, strlen(node->v_link) + 1);
        else
            write(fd, "", 1);
        break;
    case kv_type_int:
        write(fd, &node->v_int, 8);
        break;
    case kv_type_uint:
        write(fd, &node->v_uint, 8);
        break;
    case kv_type_float:
        write(fd, &node->v_float, 8);
        break;
    case kv_type_bool:
        write(fd, &node->v_bool, 1);
        break;
    };

    write(fd, &node->child_num, 4);

    for (node = node->child; node; node = node->next)
        kv_node_save(node, fd);
}

// ----------------------------------------------------------------------------------------------------
// end
