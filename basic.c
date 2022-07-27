

// copi143保留所有权利

// ----------------------------------------------------------------------------------------------------
#pragma once
// ----------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 对数据的快速hash
unsigned int kv_hash(void *data, int len)
{
    unsigned int hash = 0;
    if (data)
        if (len >= 0)
            for (int i = 0; i < len; i++)
                hash = hash * 131 + ((unsigned char *)data)[i];
        else
            for (; *(unsigned char *)data; data++)
                hash = hash * 131 + *(unsigned char *)data;
    return hash;
}

char *kv_strcpy(char *str)
{
    if (!str)
        return NULL;
    int len = strlen(str) + 1;
    char *str2 = malloc(len);
    memcpy(str2, str, len);
    return str2;
}
void *kv_datacpy(void *src, int len)
{
    if (!src)
        return NULL;
    void *dst = malloc(len);
    memcpy(dst, src, len);
    return dst;
}

char *kv_read_str(int fd)
{
    char str[2048] = {0};
    int offset = 0;
    while (read(fd, str + offset, 1) == 1 && str[offset] != '\0')
        offset++;
    return kv_strcpy(str);
}

// ----------------------------------------------------------------------------------------------------
// end
