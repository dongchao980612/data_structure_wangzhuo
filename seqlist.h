#ifndef SEQLIST_H
#define SEQLIST_H

#include "common.h"
#include <stdlib.h>

// 顺序表初始容量（固定容量场景）
#define LIST_INIT_SIZE 5

// 顺序表结构体定义（仅含核心数据）
typedef struct {
	ElemType *elem;   // 动态数组指针
	int length;       // 当前长度
} SqList;

/*************************************************************************
 * 基础操作接口
 *************************************************************************/
// 初始化顺序表
Status SqList_Init(SqList *list);

// 销毁顺序表（释放内存）
void SqList_Destroy(SqList *list);

// 清空顺序表（保留容量，重置长度）
void SqList_Clear(SqList *list);

// 获取顺序表当前长度
int SqList_Length(const SqList *list);

// 判断顺序表是否为空
bool SqList_IsEmpty(const SqList *list);

// 判断顺序表是否已满
bool SqList_IsFull(const SqList *list);

/*************************************************************************
 * 元素操作接口
 *************************************************************************/
// 在指定位置（1-based）插入元素
Status SqList_Insert(SqList *list, int pos, ElemType elem);

// 删除指定位置（1-based）的元素，通过elem带出删除值
Status SqList_Delete(SqList *list, int pos, ElemType *elem);

// 查找元素首次出现的位置（1-based，未找到返回0）
int SqList_LocateElem(const SqList *list, ElemType elem);

// 获取指定位置（1-based）的元素值
Status SqList_GetElem(const SqList *list, int pos, ElemType *elem);

/*************************************************************************
 * 元数据与遍历接口（供调用者实现打印等功能）
 *************************************************************************/
// 获取指定下标（0-based）的元素（遍历用）
Status SqList_GetData(const SqList *list, int index, ElemType *elem);

// 获取顺序表元数据（长度、空/满状态）
Status SqList_GetMeta(const SqList *list, int *length, bool *is_empty, bool *is_full);


/*************************************************************************
 * 基础操作实现
 *************************************************************************/
Status SqList_Init(SqList *list) {
	if (list == NULL) {
		return INVALID;
	}
	list->elem = (ElemType*)malloc(sizeof(ElemType) * LIST_INIT_SIZE);
	if (list->elem == NULL) {
		return OVERFLOW;  // 内存分配失败
	}
	list->length = 0;
	return SUCCESS;
}

void SqList_Destroy(SqList *list) {
	if (list == NULL) {
		return;
	}
	if (list->elem != NULL) {
		free(list->elem);
		list->elem = NULL;  // 置空避免野指针
	}
	list->length = 0;
}

void SqList_Clear(SqList *list) {
	if (list != NULL) {
		list->length = 0;  // 仅重置长度，不释放内存
	}
}

int SqList_Length(const SqList *list) {
	if (list == NULL) {
		return 0;  // 无效表视为长度0
	}
	return list->length;
}

bool SqList_IsEmpty(const SqList *list) {
	return (list == NULL) ? true : (list->length == 0);
}

bool SqList_IsFull(const SqList *list) {
	if (list == NULL || list->elem == NULL) {
		return false;  // 无效表视为未满
	}
	return (list->length == LIST_INIT_SIZE);
}

/*************************************************************************
 * 元素操作实现
 *************************************************************************/
Status SqList_Insert(SqList *list, int pos, ElemType elem) {
	// 检查基础有效性
	if (list == NULL || list->elem == NULL) {
		return INVALID;
	}
	// 检查位置合法性（1 ≤ pos ≤ length+1）
	if (pos < 1 || pos > list->length + 1) {
		return INVALID;
	}
	// 检查是否已满
	if (SqList_IsFull(list)) {
		return STATUS_FULL;
	}
	
	// 元素后移（从后往前）
	for (int i = list->length - 1; i >= pos - 1; i--) {
		list->elem[i + 1] = list->elem[i];
	}
	// 插入新元素
	list->elem[pos - 1] = elem;
	list->length++;
	return SUCCESS;
}

Status SqList_Delete(SqList *list, int pos, ElemType *elem) {
	// 检查基础有效性
	if (list == NULL || list->elem == NULL || elem == NULL) {
		return INVALID;
	}
	// 检查空表
	if (SqList_IsEmpty(list)) {
		return INVALID;
	}
	// 检查位置合法性（1 ≤ pos ≤ length）
	if (pos < 1 || pos > list->length) {
		return INVALID;
	}
	
	// 保存被删除元素
	*elem = list->elem[pos - 1];
	// 元素前移（从前往后）
	for (int i = pos; i < list->length; i++) {
		list->elem[i - 1] = list->elem[i];
	}
	list->length--;
	return SUCCESS;
}

int SqList_LocateElem(const SqList *list, ElemType elem) {
	if (list == NULL || list->elem == NULL || SqList_IsEmpty(list)) {
		return 0;  // 无效表或空表返回0（未找到）
	}
	// 遍历查找
	for (int i = 0; i < list->length; i++) {
		if (list->elem[i] == elem) {
			return i + 1;  // 返回1-based位置
		}
	}
	return 0;  // 未找到
}

Status SqList_GetElem(const SqList *list, int pos, ElemType *elem) {
	// 检查基础有效性
	if (list == NULL || list->elem == NULL || elem == NULL) {
		return INVALID;
	}
	// 检查位置合法性
	if (pos < 1 || pos > list->length) {
		return INVALID;
	}
	// 获取元素
	*elem = list->elem[pos - 1];
	return SUCCESS;
}

/*************************************************************************
 * 元数据与遍历接口实现（供调用者扩展）
 *************************************************************************/
Status SqList_GetData(const SqList *list, int index, ElemType *elem) {
	// 检查基础有效性
	if (list == NULL || list->elem == NULL || elem == NULL) {
		return INVALID;
	}
	// 检查下标合法性（0-based）
	if (index < 0 || index >= list->length) {
		return INVALID;
	}
	// 获取元素
	*elem = list->elem[index];
	return SUCCESS;
}

Status SqList_GetMeta(const SqList *list, int *length, bool *is_empty, bool *is_full) {
	// 检查参数有效性
	if (list == NULL || length == NULL || is_empty == NULL || is_full == NULL) {
		return INVALID;
	}
	// 填充元数据
	*length = list->length;
	*is_empty = (list->length == 0);
	*is_full = (list->length == LIST_INIT_SIZE);
	return SUCCESS;
}


#endif  // SEQLIST_H
