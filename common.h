#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

// 状态码定义（统一规范）
#define SUCCESS       1   // 操作成功
#define FAILURE       0   // 通用失败
#define STATUS_FULL   -1  // 顺序表已满
#define OVERFLOW      -2  // 内存分配失败
#define INVALID       -3  // 参数无效（越界、空指针等）

// 元素类型（可根据业务修改，保持库通用性）
typedef int ElemType;
typedef int Status;

// 通用元素打印函数（由调用者实现或适配）
void print_ElemType(ElemType elem);

#endif  // COMMON_H
