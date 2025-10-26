#include <stdio.h>
#include "seqlist.h"

// 实现通用元素打印（适配int类型）
void print_ElemType(ElemType elem) {
	printf("%d", elem);  // 可根据ElemType修改格式
}

// 调用者自定义顺序表打印函数
void My_SqList_Print(const SqList *list) {
	int length;
	bool is_empty, is_full;
	// 获取元数据
	Status status = SqList_GetMeta(list, &length, &is_empty, &is_full);
	if (status != SUCCESS) {
		printf("打印失败：顺序表无效！\n");
		return;
	}
	// 处理未初始化/已销毁的情况
	if (list == NULL || list->elem == NULL) {
		printf("【打印】顺序表未初始化或已销毁\n");
		return;
	}
	// 空表处理
	if (is_empty) {
		printf("【打印】顺序表为空（长度：0）\n");
		return;
	}
	// 正常打印（带自定义格式）
	printf("【打印】顺序表（长度：%d，容量：%d，是否已满：%s）：",
		   length, LIST_INIT_SIZE, is_full ? "是" : "否");
	ElemType elem;
	for (int i = 0; i < length; i++) {
		SqList_GetData(list, i, &elem);
		print_ElemType(elem);
		if (i != length - 1) {
			printf(" -> ");  // 自定义元素分隔符
		}
	}
	printf("\n");
}

// 测试函数（全面覆盖所有场景）
int main() {
	SqList list;
	Status status;
	ElemType del_elem, get_elem;
	int pos;
	
	printf("===== 1. 测试初始化与基础状态 =====\n");
	// 1.1 正常初始化
	status = SqList_Init(&list);
	if (status == SUCCESS) {
		printf("初始化成功！\n");
	} else {
		printf("初始化失败，错误码：%d\n", status);
		return FAILURE;
	}
	// 1.2 初始化后空表判断
	if (SqList_IsEmpty(&list)) {
		printf("初始化后为空空表，判断正确\n");
	} else {
		printf("初始化后空表判断错误！\n");
	}
	// 1.3 初始化后长度获取
	printf("初始化后长度：%d（预期0）\n", SqList_Length(&list));
	My_SqList_Print(&list);  // 打印空表状态
	
	printf("\n===== 2. 测试插入操作（正常+边界+异常） =====\n");
	// 2.1 正常插入（表头、表中、表尾）
	status = SqList_Insert(&list, 1, 10);  // 表头
	status = SqList_Insert(&list, 2, 20);  // 表中
	status = SqList_Insert(&list, 3, 30);  // 表中
	status = SqList_Insert(&list, 4, 40);  // 表尾
	printf("插入4个元素后：\n");
	My_SqList_Print(&list);  // 预期：10 -> 20 -> 30 -> 40
	
	// 2.2 边界插入（当前长度+1，表尾）
	status = SqList_Insert(&list, 5, 50);  // 长度4→插入位置5（达容量上限）
	if (status == SUCCESS) {
		printf("边界插入（位置5）成功：\n");
		My_SqList_Print(&list);  // 预期：10 -> 20 -> 30 -> 40 -> 50
	} else {
		printf("边界插入（位置5）失败，错误码：%d\n", status);
	}
	
	// 2.3 异常插入：位置<1（越界）
	status = SqList_Insert(&list, 0, 60);
	if (status == INVALID) {
		printf("插入位置0（越界），返回INVALID，处理正确\n");
	} else {
		printf("插入位置0错误，未返回INVALID！错误码：%d\n", status);
	}
	
	// 2.4 异常插入：位置>长度+1（越界）
	status = SqList_Insert(&list, 7, 70);  // 当前长度5，最大合法位置6
	if (status == INVALID) {
		printf("插入位置7（越界），返回INVALID，处理正确\n");
	} else {
		printf("插入位置7错误，未返回INVALID！错误码：%d\n", status);
	}
	
	// 2.5 异常插入：顺序表已满
	status = SqList_Insert(&list, 6, 80);  // 容量5，已存5个元素
	if (status == STATUS_FULL) {
		printf("顺序表已满，插入失败，返回STATUS_FULL，处理正确\n");
	} else {
		printf("顺序表已满插入错误，未返回STATUS_FULL！错误码：%d\n", status);
	}
	
	printf("\n===== 3. 测试查找操作（存在+不存在） =====\n");
	// 3.1 查找存在的元素（表中）
	pos = SqList_LocateElem(&list, 30);
	if (pos != 0) {
		printf("查找元素30，位置：%d（预期3），查找成功\n", pos);
	} else {
		printf("查找元素30失败！\n");
	}
	
	// 3.2 查找不存在的元素
	pos = SqList_LocateElem(&list, 100);
	if (pos == 0) {
		printf("查找元素100（不存在），返回0，处理正确\n");
	} else {
		printf("查找不存在元素错误，返回值：%d（预期0）\n", pos);
	}
	
	printf("\n===== 4. 测试获取元素（SqList_GetElem） =====\n");
	// 4.1 正常获取（位置3）
	status = SqList_GetElem(&list, 3, &get_elem);
	if (status == SUCCESS && get_elem == 30) {
		printf("获取位置3的元素：%d（预期30），获取成功\n", get_elem);
	} else {
		printf("获取位置3元素失败！错误码：%d，获取值：%d\n", status, get_elem);
	}
	
	// 4.2 异常获取：位置越界（>长度）
	status = SqList_GetElem(&list, 6, &get_elem);  // 当前长度5，最大位置5
	if (status == INVALID) {
		printf("获取位置6（越界），返回INVALID，处理正确\n");
	} else {
		printf("获取越界位置错误，未返回INVALID！错误码：%d\n", status);
	}
	
	// 4.3 异常获取：位置<1（越界）
	status = SqList_GetElem(&list, 0, &get_elem);
	if (status == INVALID) {
		printf("获取位置0（越界），返回INVALID，处理正确\n");
	} else {
		printf("获取位置0错误，未返回INVALID！错误码：%d\n", status);
	}
	
	printf("\n===== 5. 测试删除操作（正常+边界+异常） =====\n");
	// 5.1 正常删除（表尾）
	status = SqList_Delete(&list, 5, &del_elem);  // 删除元素50
	if (status == SUCCESS && del_elem == 50) {
		printf("删除位置5的元素：%d（预期50），删除成功\n", del_elem);
		My_SqList_Print(&list);  // 预期：10 -> 20 -> 30 -> 40
	} else {
		printf("删除位置5元素失败！错误码：%d，删除值：%d\n", status, del_elem);
	}
	
	// 5.2 边界删除（表头）
	status = SqList_Delete(&list, 1, &del_elem);  // 删除元素10
	if (status == SUCCESS && del_elem == 10) {
		printf("删除位置1的元素：%d（预期10），删除成功\n", del_elem);
		My_SqList_Print(&list);  // 预期：20 -> 30 -> 40
	} else {
		printf("删除位置1元素失败！错误码：%d，删除值：%d\n", status, del_elem);
	}
	
	// 5.3 异常删除：位置越界
	status = SqList_Delete(&list, 5, &del_elem);  // 当前长度3，最大位置3
	if (status == INVALID) {
		printf("删除位置5（越界），返回INVALID，处理正确\n");
	} else {
		printf("删除越界位置错误，未返回INVALID！错误码：%d\n", status);
	}
	
	// 5.4 异常删除：空表删除（先清空表）
	SqList_Clear(&list);
	printf("清空后表长度：%d（预期0）\n", SqList_Length(&list));
	status = SqList_Delete(&list, 1, &del_elem);  // 空表删除
	if (status == INVALID) {
		printf("空表删除，返回INVALID，处理正确\n");
	} else {
		printf("空表删除错误，未返回INVALID！错误码：%d\n", status);
	}
	
	printf("\n===== 6. 测试清空操作（SqList_Clear） =====\n");
	// 重新插入元素后清空
	SqList_Insert(&list, 1, 100);
	SqList_Insert(&list, 2, 200);
	printf("清空前表：\n");
	My_SqList_Print(&list);  // 预期：100 -> 200
	SqList_Clear(&list);
	if (SqList_IsEmpty(&list)) {
		printf("清空后为空空表，清空成功\n");
		My_SqList_Print(&list);  // 预期：空表提示
	} else {
		printf("清空操作失败！当前长度：%d\n", SqList_Length(&list));
	}
	
	printf("\n===== 7. 测试销毁后误用（异常场景防护） =====\n");
	SqList_Destroy(&list);
	// 销毁后修改长度并打印
	list.length = 10;  // 非法修改，验证防护
	printf("销毁后修改长度为10，调用打印：\n");
	My_SqList_Print(&list);  // 预期：提示“未初始化或已销毁”
	
	// 销毁后尝试插入
	status = SqList_Insert(&list, 1, 300);
	if (status == INVALID || status == FAILURE) {
		printf("销毁后插入失败，处理正确（错误码：%d）\n", status);
	} else {
		printf("销毁后插入错误，未拦截！错误码：%d\n", status);
	}
	
	// 销毁后尝试获取元素
	status = SqList_GetElem(&list, 1, &get_elem);
	if (status == INVALID) {
		printf("销毁后获取元素失败，处理正确（错误码：%d）\n", status);
	} else {
		printf("销毁后获取元素错误，未拦截！错误码：%d\n", status);
	}
	
	printf("\n===== 所有测试用例执行完毕 =====\n");
	return SUCCESS;
}
