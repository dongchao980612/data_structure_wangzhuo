#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// 状态码定义（不变）
#define SUCCESS       1
#define FAILURE       0
#define STATUS_FULL   -1
#define OVERFLOW      -2
#define INVALID       -3

typedef int Status;

#define LIST_INIT_SIZE 10  // 初始容量（支持动态扩容）


// 核心修改：ElemType改为图书结构体
typedef struct {
	int id;         // 图书ID（唯一）
	char name[50];  // 书名
	char author[30];// 作者
	int stock;      // 库存
} ElemType;        // 现在直接存储图书信息

typedef struct {
	ElemType *elem;   // 存储图书结构体
	int length;       // 当前长度
	int capacity;     // 容量（新增：支持动态扩容）
} SqList;

// 新增：匹配函数指针（用于自定义查找规则）
typedef bool (*MatchFunc)(const ElemType* a, const ElemType* b);


// 实现图书打印（适配ElemType=图书结构体）
void print_ElemType(ElemType elem) {
	printf("ID：%d | 书名：%s | 作者：%s | 库存：%d\n",
	       elem.id, elem.name, elem.author, elem.stock);
}




Status SqList_Init(SqList *list) {
	if (list == NULL) return INVALID;
	list->elem = (ElemType*)malloc(sizeof(ElemType) * LIST_INIT_SIZE);
	if (list->elem == NULL) return OVERFLOW;
	list->length = 0;
	list->capacity = LIST_INIT_SIZE;  // 初始化容量
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
	return list->length == list->capacity;
}
static Status SqList_Expand(SqList *list) {
	if (list == NULL || list->elem == NULL) return INVALID;
	int new_cap = list->capacity * 2;
	ElemType *new_elem = (ElemType*)realloc(list->elem, sizeof(ElemType) * new_cap);
	if (new_elem == NULL) return OVERFLOW;
	list->elem = new_elem;
	list->capacity = new_cap;
	return SUCCESS;
}
Status SqList_Insert(SqList *list, int pos, ElemType elem) {
	if (list == NULL || list->elem == NULL) return INVALID;
	if (pos < 1 || pos > list->length + 1) return INVALID;
	// 满时扩容
	if (SqList_IsFull(list) && SqList_Expand(list) != SUCCESS) {
		return STATUS_FULL;
	}
	// 元素后移
	for (int i = list->length - 1; i >= pos - 1; i--) {
		list->elem[i + 1] = list->elem[i];
	}
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


// 新增接口：按自定义规则查找（支持按ID/书名等查找）
int SqList_LocateCustom(const SqList *list, const ElemType *target, MatchFunc match) {
	if (list == NULL || list->elem == NULL || target == NULL || match == NULL) {
		return 0;
	}
	for (int i = 0; i < list->length; i++) {
		if (match(&list->elem[i], target)) {
			return i + 1;  // 返回1-based位置
		}
	}
	return 0;
}


// 数据持久化：保存到文件
Status SqList_SaveToFile(const SqList *list, const char *filename) {
	if (list == NULL || filename == NULL) return INVALID;
	FILE *fp = fopen(filename, "wb");
	if (fp == NULL) return FAILURE;
	// 先写长度，再写元素
	fwrite(&list->length, sizeof(int), 1, fp);
	fwrite(list->elem, sizeof(ElemType), list->length, fp);
	fclose(fp);
	return SUCCESS;
}

// 数据持久化：从文件加载
Status SqList_LoadFromFile(SqList *list, const char *filename) {
	if (list == NULL || filename == NULL) return INVALID;
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) return FAILURE;
	// 先读长度，再扩容，再读元素
	int len;
	fread(&len, sizeof(int), 1, fp);
	// 确保容量足够
	while (list->capacity < len) {
		if (SqList_Expand(list) != SUCCESS) {
			fclose(fp);
			return OVERFLOW;
		}
	}
	list->length = len;
	fread(list->elem, sizeof(ElemType), len, fp);
	fclose(fp);
	return SUCCESS;
}



// 匹配函数：按ID查找（内部辅助）
bool match_by_id(const ElemType* a, const ElemType* b) {
	return a->id == b->id;
}
// 匹配函数：按书名查找（模糊匹配）
bool match_by_name(const ElemType* a, const ElemType* b) {
	return strstr(a->name, b->name) != NULL;  // 包含子串即匹配
}

// 菜单显示（沿用你的原始格式，仅补全逻辑）
void show_menu() {
	printf("\n===== 图书馆管理系统 =====\n");
	printf("1. 新增图书\n");
	printf("2. 删除图书（按ID）\n");
	printf("3. 查找图书（按ID）\n");
	printf("4. 查找图书（按name）\n");
	printf("5. 更新库存\n");
	printf("6. 显示所有图书\n");
	printf("7. 清空图书库\n");
	printf("8. 退出系统\n");
	printf("==========================\n");
	printf("请输入操作编号：");
}

// 1. 新增图书
void add_book(SqList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法新增图书！\n");
		return;
	}

	ElemType book = {0};  // 初始化图书信息
	printf("===== 新增图书 =====\n");
	printf("输入图书ID（唯一标识）：");
	scanf("%d", &book.id);

	// 检查ID是否已存在
	ElemType target = {.id = book.id};
	if (SqList_LocateCustom(list, &target, match_by_id) != 0) {
		printf("新增失败！该ID已存在\n");
		return;
	}

	printf("输入书名：");
	getchar();  // 吸收scanf残留的换行符
	fgets(book.name, sizeof(book.name), stdin);
	book.name[strcspn(book.name, "\n")] = '\0';  // 去除fgets读取的换行符

	printf("输入作者：");
	fgets(book.author, sizeof(book.author), stdin);
	book.author[strcspn(book.author, "\n")] = '\0';

	// 插入顺序表（自动扩容）
	Status status = SqList_Insert(list, list->length + 1, book);
	if (status == SUCCESS) {
		printf("新增成功！\n");
		// 自动保存到文件
		if (SqList_SaveToFile(list, "books.dat") == SUCCESS) {
			printf("数据已自动保存\n");
		}
	} else {
		printf("新增失败！（错误原因：%s）\n",
		       status == STATUS_FULL ? "扩容失败" :
		       status == INVALID ? "参数无效" : "未知错误");
	}
}

// 2. 删除图书（按ID）
void delete_book(SqList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法删除图书！\n");
		return;
	}
	if (SqList_IsEmpty(list)) {
		printf("图书库为空，无图书可删除！\n");
		return;
	}

	ElemType target = {0};
	printf("===== 删除图书 =====\n");
	printf("输入要删除的图书ID：");
	scanf("%d", &target.id);

	// 查找图书位置
	int pos = SqList_LocateCustom(list, &target, match_by_id);
	if (pos == 0) {
		printf("删除失败！未找到该ID对应的图书\n");
		return;
	}

	// 执行删除
	ElemType deleted_book;
	Status status = SqList_Delete(list, pos, &deleted_book);
	if (status == SUCCESS) {
		printf("删除成功！删除的图书信息：\n");
		print_ElemType(deleted_book);
		// 自动保存到文件
		if (SqList_SaveToFile(list, "books.dat") == SUCCESS) {
			printf("数据已自动保存\n");
		}
	} else {
		printf("删除失败！（错误码：%d）\n", status);
	}
}

// 3. 查找图书（按ID）
void find_book_by_id(SqList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法查找图书！\n");
		return;
	}
	if (SqList_IsEmpty(list)) {
		printf("图书库为空，无图书可查找！\n");
		return;
	}

	ElemType target = {0};
	printf("===== 查找图书（按ID） =====\n");
	printf("输入要查找的图书ID：");
	scanf("%d", &target.id);

	// 查找位置
	int pos = SqList_LocateCustom(list, &target, match_by_id);
	if (pos == 0) {
		printf("未找到该ID对应的图书\n");
		return;
	}

	// 获取并打印图书信息
	ElemType book;
	SqList_GetElem(list, pos, &book);
	printf("找到图书（位置：%d）：\n", pos);
	print_ElemType(book);
}

void find_book_by_name(SqList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法查找图书！\n");
		return;
	}
	if (SqList_IsEmpty(list)) {
		printf("图书库为空，无图书可查找！\n");
		return;
	}


	ElemType target = {0};
	printf("===== 查找图书（按书名） =====\n");  // 补充标题，用户体验更好
	scanf("%s", target.name);
	printf("查找结果：\n");
	bool found = false;  // 新增：标记是否找到，优化提示


	for (int i = 0; i < list->length; i++) {
		ElemType book;
		SqList_GetData(list, i, &book);
		if (match_by_name(&book, &target)) {
			print_ElemType(book);
		}
	}
	if (!found) {  // 新增：未找到时提示，避免用户误以为无响应
		printf("未找到包含该关键字的图书\n");
	}
}

// 4. 更新库存（按ID）
void update_stock(SqList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法更新库存！\n");
		return;
	}
	if (SqList_IsEmpty(list)) {
		printf("图书库为空，无库存可更新！\n");
		return;
	}

	ElemType target = {0};
	int new_stock;
	printf("===== 更新库存（按ID） =====\n");
	printf("输入图书ID：");
	scanf("%d", &target.id);
	printf("输入新库存数量（非负整数）：");
	scanf("%d", &new_stock);

	// 校验库存合法性
	if (new_stock < 0) {
		printf("更新失败！库存数量不能为负数\n");
		return;
	}

	// 查找图书位置
	int pos = SqList_LocateCustom(list, &target, match_by_id);
	if (pos == 0) {
		printf("更新失败！未找到该ID对应的图书\n");
		return;
	}

	// 执行库存更新（直接修改顺序表中的元素）
	ElemType *book_ptr = &list->elem[pos - 1];  // 1-based转0-based
	book_ptr->stock = new_stock;
	printf("更新成功！\n");
	printf("更新后图书信息：\n");
	print_ElemType(*book_ptr);

	// 自动保存到文件
	if (SqList_SaveToFile(list, "books.dat") == SUCCESS) {
		printf("数据已自动保存\n");
	}
}

// 5. 显示所有图书
void show_all_books(SqList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法显示图书！\n");
		return;
	}

	printf("\n===== 所有图书列表 =====\n");
	if (SqList_IsEmpty(list)) {
		printf("图书库为空\n");
		printf("========================\n");
		return;
	}

	printf("共%d本图书：\n", list->length);
	for (int i = 0; i < list->length; i++) {
		ElemType book;
		SqList_GetData(list, i, &book);
		printf("第%d本：", i + 1);
		print_ElemType(book);
	}
	printf("========================\n");
}

// 6. 清空图书库
void clear_book_lib(SqList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法清空图书库！\n");
		return;
	}
	if (SqList_IsEmpty(list)) {
		printf("图书库已为空，无需清空！\n");
		return;
	}

	// 二次确认
	char confirm;
	printf("===== 清空图书库 =====\n");
	printf("警告：此操作将删除所有图书数据，是否确认？（y/n）：");
	scanf(" %c", &confirm);  // 空格避免读取残留换行符

	if (confirm != 'y' && confirm != 'Y') {
		printf("已取消清空操作\n");
		return;
	}

	// 执行清空
	SqList_Clear(list);
	printf("清空成功！图书库已为空\n");

	// 同步清空文件数据
	if (SqList_SaveToFile(list, "books.dat") == SUCCESS) {
		printf("文件数据已同步清空\n");
	}
}

// 主函数（整合所有功能）
int main() {
	SqList book_list;
	Status init_status = SqList_Init(&book_list);
	if (init_status != SUCCESS) {
		printf("系统初始化失败！（错误码：%d）\n", init_status);
		return FAILURE;
	}

	// 加载历史数据
	printf("正在加载历史图书数据...\n");
	if (SqList_LoadFromFile(&book_list, "books.dat") == SUCCESS) {
		printf("加载成功！当前图书库共%d本图书\n", book_list.length);
	} else {
		printf("无历史数据或加载失败，将创建新图书库\n");
	}

	int choice;
	do {
		show_menu();
		scanf("%d", &choice);
		switch (choice) {
			case 1:
				add_book(&book_list);
				break;
			case 2:
				delete_book(&book_list);
				break;
			case 3:
				find_book_by_id(&book_list);
				break;
			case 4:
				find_book_by_name(&book_list);
				break;
			case 5:
				update_stock(&book_list);
				break;
			case 6:
				show_all_books(&book_list);
				break;
			case 7:
				clear_book_lib(&book_list);
				break;
			case 8:
				printf("正在保存数据...\n");
				SqList_SaveToFile(&book_list, "books.dat");
				printf("数据保存完成，退出系统，再见！\n");
				break;
			default:
				printf("无效操作编号，请重新输入！\n");
		}
	} while (choice != 8);

	// 销毁顺序表，释放内存
	SqList_Destroy(&book_list);
	return SUCCESS;
}
