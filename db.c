

// copi143保留所有权利

// ----------------------------------------------------------------------------------------------------
#pragma once
// ----------------------------------------------------------------------------------------------------

#include <unistd.h>
#include <fcntl.h>

#include "db.h"

#include "basic.c"

// ----------------------------------------------------------------------------------------------------
// 函数

void kv_print_child(kv_node node, int include_child)
{
    kv_node_print_child(node, 0, include_child);
}
void kv_print(kv_node node, int include_child)
{
    kv_node_print(node, 0, include_child);
}

#if 0
bool kv_load_archive(char **key, kv_node *value, struct archive *file)
{
#define key (*key)
#define value (*value)

again:
    key = value = NULL;

    struct archive_entry *entry;
    if (archive_read_next_header(file, &entry) < 0)
        goto error;
    if (archive_entry_filetype(entry) != AE_IFREG)
        goto again;

    // 读取 key
    if ((key = bot_strcpy(archive_entry_pathname(entry))) == NULL)
        goto error;
    if (key[strlen(key) - 1] == '/')
        goto again;

    // 读取 value
    value = kv_value_new();
    value->len = archive_entry_size(entry) - 1;
    archive_read_data(file, &value->type, 1);
    if (value->type && value->len)
    {
        if (value->type == kv_type_data)
        {
            value->v_data = bot_malloc(value->len);
            archive_read_data(file, value->v_data, value->len);
        }
        else if (value->type == kv_type_str)
        {
            value->v_str = bot_malloc(value->len + 1);
            value->v_str[value->len] = '\0';
            archive_read_data(file, value->v_str, value->len);
        }
        else
            archive_read_data(file, &value->v_uint, value->len);
    }

    return true;

error:
    free(key);
    kv_value_delete(value);
    return false;

#undef key
#undef value
}
void kv_save_archive(char *key, kv_node value, struct archive *file)
{
    if (!key || !value || value->type == 0 || !file)
        return;

    struct archive_entry *entry = archive_entry_new();
    archive_entry_set_pathname(entry, key);
    archive_entry_set_size(entry, value->len + 1);
    archive_entry_set_filetype(entry, AE_IFREG);
    archive_write_header(file, entry);

    archive_write_data(file, &value->type, 1);
    if (value->len)
    {
        if (value->type == kv_type_data)
            archive_write_data(file, value->v_data, value->len);
        else if (value->type == kv_type_str)
            archive_write_data(file, value->v_str, value->len);
        else
            archive_write_data(file, &value->value, value->len);
    }

    archive_entry_free(entry);
}

int kv_load_from_file(GHashTable *hash_table, char *filename)
{
    if (!hash_table || !filename)
        return -1;

    // 打开文件
    struct archive *file = archive_read_new();
    archive_read_support_filter_all(file);
    archive_read_support_format_all(file);
    if (archive_read_open_file(file, filename, 1024) < 0)
    {
        log_warning("archive 只读模式打开文件错误 %s", archive_error_string(file));
        archive_free(file);
        return -1;
    }

    // 读取每一对 key-value
    char *key;
    kv_node value;
    while (kv_load_archive(&key, &value, file))
        g_hash_table_insert(hash_table, key, value);

    // 释放资源
    archive_read_close(file);
    archive_free(file);

    return 0;
}
int kv_save_to_file(GHashTable *hash_table, char *filename)
{
    if (!hash_table || !filename)
        return -1;

    // 打开文件
    struct archive *file = archive_write_new();
    archive_write_add_filter_gzip(file);
    archive_write_set_format_pax(file);
    if (archive_write_open_file(file, filename) < 0)
    {
        log_warning("archive 只写模式打开文件错误 %s", archive_error_string(file));
        archive_free(file);
        return -1;
    }

    g_hash_table_foreach(hash_table, kv_save_archive, file);

    // 释放资源
    archive_write_close(file);
    archive_free(file);

    return 0;
}

kv_node kv_lookup_value(GHashTable *hash_table, char *key)
{
    if (!hash_table || !key)
        return NULL;
    return g_hash_table_lookup(hash_table, key);
}
kv_node kv_get_value(GHashTable *hash_table, char *key)
{
    if (!hash_table || !key)
        return NULL;
    kv_node value = g_hash_table_lookup(hash_table, key);
    if (value)
        return value;
    value = kv_value_new();
    g_hash_table_insert(hash_table, bot_strcpy(key), value);
    return value;
}
void kv_rm(GHashTable *hash_table, char *key)
{
    if (!hash_table || !key)
        return;
    g_hash_table_remove(hash_table, key);
}
#endif

// 加载 保存

kv_node kv_load(int fd)
{
    return kv_node_load(NULL, fd);
}
void kv_save(kv_node node, int fd)
{
    kv_node_save(node, fd);
}

kv_node kv_load_file(char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        return NULL;
    kv_node node = kv_load(fd);
    close(fd);
    return node;
}
int kv_save_file(kv_node node, char *filename)
{
    int fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd < 0)
        return -1;
    kv_save(node, fd);
    close(fd);
    return 0;
}

// ----------------------------------------------------------------------------------------------------
// end
