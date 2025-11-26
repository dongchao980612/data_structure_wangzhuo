#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TRUE 1
#define FALSE 0


#define OK 1
#define ERROR 0


#define INFEASIBLE -1    // 操作不可行（如参数矛盾）
#define OVERFLOW -2      // 内存溢出（动态分配失败）
#define UNINITIALIZED -3 // 未初始化（如未调用Init函数）


#define MAXSIZE 100      // 顺序表最大容量

typedef int ElemType;    // 元素类型（可按需修改）
typedef int Status;      // 函数返回状态类型



typedef struct LNode {
	ElemType data;          // 节点存储的数据
	struct LNode* next;     // 指向后继节点的指针
} LNode, *LinkList;


Status LinkList_Init(LinkList* L) ;
bool  LinkList_IsEmpty(LinkList L);

Status LinkList_Destroy(LinkList* L);


// https://www.cnblogs.com/htu2021/p/15410276.html

int main() {
	LinkList L = NULL;
	// 初始化链表

	printf("===== 初始化链表 =====\n");
	if (LinkList_Init(&L) == OK) {
		printf("初始化成功\n");  // 输出0
		printf("  头节点地址：%p\n", (void*)L);
		printf("  头节点后继：%p（应为NULL）\n", (void*)(L->next));
		printf("  是否为空：%d（应为1）\n", LinkList_IsEmpty(L));
	} else {
		printf("初始化失败！\n");
		return EXIT_FAILURE;
	}
	return 0;

}

Status LinkList_Init(LinkList* L) {
	// 为头节点分配内存（头节点不存储实际数据，仅用于统一操作）
	*L = (LinkList)malloc(sizeof(LNode));
	if (*L == NULL) {
		return OVERFLOW;  // 内存分配失败
	}

	(*L)->next = NULL;    // 空链表：头节点的next为NULL


	return OK;
}
bool  LinkList_IsEmpty(LinkList L) {
	return L->next == NULL;
}

Status LinkList_Destroy(LinkList* L) {
	return 0;

}
