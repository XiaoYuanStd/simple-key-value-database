

// copi143保留所有权利

// ----------------------------------------------------------------------------------------------------
#pragma once
// ----------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------
// key-value 数据库内值的类型枚举

enum // key-value 数据库内值的类型枚举
{
    kv_type_null = 0, // 空项, 不会被保存
    kv_type_data,     // 二进制数据
    kv_type_int,      // 整数
    kv_type_uint,     // 无符号整数
    kv_type_float,    // 浮点
    kv_type_bool,     // 布尔
    kv_type_str,      // 字符串
    kv_type_link,     // 链接
    kv_type_list,     // 链表
};

#define kv_bool(value) ((value) ? 1 : 0)

typedef unsigned char __kv_byte;
typedef unsigned short __kv_2byte;
typedef unsigned int __kv_4byte;
typedef unsigned long long __kv_8byte;
typedef void *__kv_ptr;

// ----------------------------------------------------------------------------------------------------
// end
