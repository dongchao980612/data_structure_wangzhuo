#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "status.h"

#define MAX_SIZE 100      // 顺序表最大容量

typedef int ElemType;    // 元素类型（可按需修改）


typedef struct SqList{
	ElemType* elem; // 用指针指向动态分配的数组
	int length; // 顺序表的的长度
} SqList;

Status SqList_Init(SqList *L);
void SqList_Destroy(SqList *L);
void SqList_Clear(SqList *L);
void SqList_Visit(const SqList L);

Status SqList_Insert(SqList *L, int pos, ElemType elem) ;
Status SqList_Delete(SqList *L, int pos, ElemType *elem) ;


int SqList_Length(const SqList L) ;
bool SqList_IsEmpty(const SqList L) ;
bool SqList_IsFull(const SqList L);

int SqList_locateElem(SqList *L, ElemType e);
Status SqList_GetElem(const SqList *L, const int pos, ElemType *elem);



int main() {	
	SqList L;          // 栈上定义顺序表变量
	ElemType data;     // 存储获取/删除的元素
	int ret;           // 存储函数返回值
	
	printf("===== 测试1：初始化顺序表 =====\n");
	if (SqList_Init(&L) == OK) {
		printf("初始化成功！当前长度：%d\n", SqList_Length(L));  // 输出0
	} else {
		printf("初始化失败！\n");
		return EXIT_FAILURE;
	}
	
	printf("\n===== 测试2：插入元素 =====\n");
	// 测试插入到表头（pos=1）
	if (SqList_Insert(&L, 1, 10) == OK) {
		printf("插入10到位置1成功！");
	} else {
		printf("插入10失败！");
	}
	SqList_Visit(L);  // 输出：10
	
	// 测试插入到表尾（pos=当前长度+1）
	if (SqList_Insert(&L, 2, 20) == OK) {  // 此时长度1，pos=2是表尾
		printf("插入20到位置2成功！");
	} else {
		printf("插入20失败！");
	}
	SqList_Visit(L);  // 输出：10 20
	
	// 测试插入到中间位置
	if (SqList_Insert(&L, 2, 15) == OK) {  // 插入到10和20之间
		printf("插入15到位置2成功！");
	} else {
		printf("插入15失败！");
	}
	SqList_Visit(L);  // 输出：10 15 20
	printf("当前长度：%d\n", SqList_Length(L));  // 输出3
	
	// 测试插入非法位置（pos=0）
	if (SqList_Insert(&L, 0, 5) == ERROR) {
		printf("插入到位置0失败（符合预期）\n");
	}
	
	printf("\n===== 测试3：查找元素 =====\n");
	// 查找存在的元素
	ret = SqList_locateElem(&L, 15);
	if (ret > 0) {
		printf("元素15的位置是：%d\n", ret);  // 输出2
	}
	// 查找不存在的元素
	ret = SqList_locateElem(&L, 30);
	if (ret == ERROR) {
		printf("元素30不存在（符合预期）\n");
	}
	
	printf("\n===== 测试4：获取指定位置元素 =====\n");
	// 获取合法位置元素
	if (SqList_GetElem(&L, 3, &data) == OK) {
		printf("位置3的元素是：%d\n", data);  // 输出20
	}
	// 获取非法位置元素（pos=4，当前长度3）
	if (SqList_GetElem(&L, 4, &data) == ERROR) {
		printf("获取位置4的元素失败（符合预期）\n");
	}
	
	printf("\n===== 测试5：删除元素 =====\n");
	// 删除中间元素（pos=2）
	if (SqList_Delete(&L, 2, &data) == OK) {
		printf("删除位置2的元素：%d，删除后：", data);  // 输出删除的15
	}
	SqList_Visit(L);  // 输出：10 20
	printf("当前长度：%d\n", SqList_Length(L));  // 输出2
	
	// 删除表头元素（pos=1）
	if (SqList_Delete(&L, 1, &data) == OK) {
		printf("删除位置1的元素：%d，删除后：", data);  // 输出删除的10
	}
	SqList_Visit(L);  // 输出：20
	printf("当前长度：%d\n", SqList_Length(L));  // 输出1
	
	// 删除表尾元素（pos=1，当前长度1）
	if (SqList_Delete(&L, 1, &data) == OK) {
		printf("删除位置1的元素：%d，删除后：", data);  // 输出删除的20
	}
	SqList_Visit(L);  // 输出：顺序表为空！
	printf("当前长度：%d\n", SqList_Length(L));  // 输出0
	
	// 测试删除空表
	if (SqList_Delete(&L, 1, &data) == ERROR) {
		printf("删除空表失败（符合预期）\n");
	}
	
	printf("\n===== 测试6：判空和判满 =====\n");
	if (SqList_IsEmpty(L) == OK) {
		printf("当前表为空（符合预期）\n");
	}
	// 插入MAXSIZE个元素，测试判满
	for (int i = 0; i < MAX_SIZE; i++) {
		SqList_Insert(&L, i + 1, i * 10);  // 插入0,10,20,...,990（100个）
	}
	if (SqList_IsFull(L) == OK) {
		printf("插入%d个元素后，表已满（符合预期）\n", MAX_SIZE);
	}
	// 测试插入满表
	if (SqList_Insert(&L, MAX_SIZE + 1, 1000) == ERROR) {
		printf("向满表插入元素失败（符合预期）\n");
	}
	
	printf("\n===== 测试7：清空表 =====\n");
	SqList_Clear(&L);
	printf("清空表后长度：%d\n", SqList_Length(L));  // 输出0
	SqList_Visit(L);  // 输出：顺序表为空！
	
	printf("\n===== 测试8：销毁表 =====\n");
	SqList_Destroy(&L);
	ret = SqList_Length(L);
	if (ret == UNINITIALIZED) {
		printf("销毁表后，表处于未初始化状态（符合预期）\n");
	}
	
	printf("\n所有测试完成！\n");
	return EXIT_SUCCESS;
	
}

// 初始化线性表
Status SqList_Init(SqList *L) {
	// 正确计算内存大小：每个ElemType的字节数 × 最大元素个数
	L->elem = (ElemType*)malloc(sizeof(ElemType) * MAX_SIZE);
	
	// 内存分配失败（malloc返回NULL），直接终止程序
	if (L->elem == NULL) {
		exit(OVERFLOW);
	}
	
	L->length = 0; // 初始化长度为0
	return OK;
}

// 销毁线性表
void SqList_Destroy(SqList *L) {
	if (L->elem) {
		free(L->elem);
		L->elem = NULL;
		L->length = 0;
	}
	
	
}

// 清空线性表
void SqList_Clear(SqList *L) {
	L->length = 0;
}


Status SqList_Insert(SqList *L, int pos, ElemType elem) {
	if (L->elem == NULL) {
		return ERROR;
	}
	
	if (SqList_IsFull(*L)) {
		return ERROR;
	}
	
	if (pos <= 0 || pos > L->length + 1) {
		return INFEASIBLE;
	}
	
	for (int i = L->length; i >= pos; i--) {
		L->elem[i] = L->elem[i - 1];  // 后移操作
	}
	
	L->elem[pos - 1] = elem;
	
	L->length++;
	
	return OK;
}
Status SqList_Delete(SqList *L, int pos, ElemType *elem) {
	if (L->elem == NULL) {
		return ERROR;
	}
	
	if (SqList_IsEmpty(*L)) {
		return ERROR;
	}
	
	if (pos <= 0 || pos > L->length) {
		return INFEASIBLE;
	}
	
	*elem = L->elem[pos - 1];
	
	for (int i = pos; i < L->length; i++) {
		L->elem[i - 1] = L->elem[i];  // 前移操作
	}
	
	
	L->length--;
	
	return OK;
}

//  获取长度
int SqList_Length(const SqList L) {
	if (L.elem == NULL) {
		return UNINITIALIZED;
	}
	
	
	return L.length;
}

// 判空
bool SqList_IsEmpty(const SqList L) {
	return L.length == 0;
}

//  判满
bool SqList_IsFull(const SqList L) {
	return L.length == MAX_SIZE;
}

int SqList_locateElem(SqList *L, ElemType e) {
	if (L->elem == NULL) {
		return UNINITIALIZED;  // 未初始化
	}
	for (int i = 0; i < L->length; i++) {
		if (L->elem[i] == e) {
			return  i + 1;
		}
	}
	
	return ERROR;
}

// 根据位置获取元素
Status SqList_GetElem(const SqList *L, const int pos, ElemType *elem) {
	
	if (L->elem == NULL) {  // 新增：检查是否初始化
		return UNINITIALIZED;
	}
	
	
	if (pos < 1 || pos > L->length) {
		return -1;  // 未初始化
	}
	
	*elem = L->elem[pos - 1];
	
	return OK;
}

void SqList_Visit(const SqList L) {
	printf("顺序表输出如下\n\t");
	for (int i = 0; i < L.length; i++) {
		printf("%d ", L.elem[i]);
	}
	printf("\n");
}
