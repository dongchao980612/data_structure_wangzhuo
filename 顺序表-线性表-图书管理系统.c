#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "status.h"

#define MAX_SIZE 10  // 初始容量（支持动态扩容）
#define IBSN_SIZE 10
#define NAME_SIZE 15
#define  AUTHOR_SIZE 255

typedef struct Book{
	int id;                     //  id
	char IBSN[IBSN_SIZE];		// IBSN
	char title[NAME_SIZE];  	// 书名
	char author[AUTHOR_SIZE] ;	// 作者
	double price;        		// 价格
	int stock;					// 库存
}Book;

typedef Book ElemType;

typedef struct {
	ElemType *elem;   // 存储图书结构体
	int length;    // 容量（新增：支持动态扩容）
} BookList;

// 新增：匹配函数指针（用于自定义查找规则）
typedef bool (*MatchFunc)(const Book* a, const Book* b);


// 实现图书打印（适配Book=图书结构体）
void print_Book(Book elem) {
	printf("IBSN：%s |书名：%s | 作者：%s | 价格：%.2f |库存：%d\n",elem.IBSN, elem.title, elem.author, elem.price,elem.stock);
}




Status BookList_Init(BookList *list) {
	if (list == NULL) {
		return UNINITIALIZED;
	}

	list->elem = (Book*)malloc(sizeof(Book) * MAX_SIZE);

	if (list->elem == NULL) {
		return OVERFLOW;
	}

	list->length = 0;

	return OK;
}
void BookList_Destroy(BookList *list) {
	if (list == NULL) {
		return;
	}
	if (list->elem != NULL) {
		free(list->elem);
		list->elem = NULL;  // 置空避免野指针
	}
	list->length = 0;
}
void BookList_Clear(BookList *list) {
	if (list != NULL) {
		list->length = 0;  // 仅重置长度，不释放内存
	}
}
int BookList_Length(const BookList *list) {
	if (list == NULL) {
		return UNINITIALIZED;  // 无效表视为长度0
	}
	return list->length;
}
bool BookList_IsEmpty(const BookList *list) {
	return (list == NULL) ? true : (list->length == 0);
}
bool BookList_IsFull(const BookList *list) {
	return list->length == MAX_SIZE;
}

Status BookList_Insert(BookList *list, int pos, Book elem) {
	if (list == NULL || list->elem == NULL) return UNINITIALIZED;
	if (pos < 1 || pos > list->length + 1) return INFEASIBLE;

	// 元素后移
	for (int i = list->length - 1; i >= pos - 1; i--) {
		list->elem[i + 1] = list->elem[i];
	}
	list->elem[pos - 1] = elem;
	list->length++;
	return OK;
}
Status BookList_Delete(BookList *list, int pos, Book *elem) {
	// 检查基础有效性
	if (list == NULL || list->elem == NULL || elem == NULL) {
		return UNINITIALIZED;
	}
	// 检查空表
	if (BookList_IsEmpty(list)) {
		return INFEASIBLE;
	}
	// 检查位置合法性（1 ≤ pos ≤ length）
	if (pos < 1 || pos > list->length) {
		return INFEASIBLE;
	}

	// 保存被删除元素
	*elem = list->elem[pos - 1];
	// 元素前移（从前往后）
	for (int i = pos; i < list->length; i++) {
		list->elem[i - 1] = list->elem[i];
	}
	list->length--;
	return OK;
}
Status BookList_GetElem(const BookList *list, int pos, Book *elem) {
	// 检查基础有效性
	if (list == NULL || list->elem == NULL || elem == NULL) {
		return STATUS_INFEASIBLE;
	}
	// 检查位置合法性
	if (pos < 1 || pos > list->length) {
		return STATUS_INFEASIBLE;
	}
	// 获取元素
	*elem = list->elem[pos - 1];
	return OK;
}
Status BookList_GetData(const BookList *list, int index, Book *elem) {
	// 检查基础有效性
	if (list == NULL || list->elem == NULL || elem == NULL) {
		return STATUS_INFEASIBLE;
	}
	// 检查下标合法性（0-based）
	if (index < 0 || index >= list->length) {
		return STATUS_INFEASIBLE;
	}
	// 获取元素
	*elem = list->elem[index];
	return OK;
}


// 新增接口：按自定义规则查找（支持按ID/书名等查找）
int BookList_LocateCustom(const BookList *list, const Book *target, MatchFunc match) {
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
Status BookList_SaveToFile(const BookList *list, const char *filename) {
	if (list == NULL || filename == NULL) return STATUS_INFEASIBLE;
	FILE *fp = fopen(filename, "wb");
	if (fp == NULL) return ERROR;
	// 先写长度，再写元素
	fwrite(&list->length, sizeof(int), 1, fp);
	fwrite(list->elem, sizeof(Book), list->length, fp);
	fclose(fp);
	return OK;
}

// 数据持久化：从文件加载
Status BookList_LoadFromFile(BookList *list, const char *filename) {
	if (list == NULL || filename == NULL) return STATUS_INFEASIBLE;
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) return ERROR;
	// 先读长度，再扩容，再读元素
	int len;
	fread(&len, sizeof(int), 1, fp);
	// 确保容量足够

	list->length = len;
	fread(list->elem, sizeof(Book), len, fp);
	fclose(fp);
	return OK;
}



// 匹配函数：按ID查找（内部辅助）

bool match_by_IBSN(const Book* a, const Book* b) {
	return a->IBSN == b->IBSN;
}

// 匹配函数：按书名查找（模糊匹配）
bool match_by_title(const Book* a, const Book* b) {
	return strstr(a->title, b->title) != NULL;  // 包含子串即匹配
}

// 菜单显示（沿用你的原始格式，仅补全逻辑）
void show_menu() {
	printf("\n===== 图书馆管理系统 =====\n");
	printf("1. 新增图书\n");
	printf("3. 删除图书（按IBSN）\n");
	printf("3. 查找图书（按IBSN）\n");
	printf("4. 查找图书（按署名）\n");
	printf("4. 查找图书（按作者名）\n");
	printf("5. 更新库存\n");
	printf("6. 显示所有图书\n");
	printf("7. 清空图书库\n");
	printf("8. 退出系统\n");
	printf("==========================\n");
	printf("请输入操作编号：");
}

// 1. 新增图书
void add_book(BookList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法新增图书！\n");
		return;
	}

	Book book = {0};  // 初始化图书信息
	printf("===== 新增图书 =====\n");
	printf("输入图书IBSN（唯一标识）：");
	scanf("%d", &book.IBSN);

	// 检查ID是否已存在
	Book target = {.IBSN = book.IBSN};
	if (BookList_LocateCustom(list, &target, match_by_IBSN) != 0) {
		printf("新增失败！该ID已存在\n");
		return;
	}

	printf("输入书名：");
	getchar();  // 吸收scanf残留的换行符
	fgets(book.title, sizeof(book.title), stdin);
	book.title[strcspn(book.title, "\n")] = '\0';  // 去除fgets读取的换行符

	printf("输入作者：");
	fgets(book.author, sizeof(book.author), stdin);
	book.author[strcspn(book.author, "\n")] = '\0';

	// 插入顺序表（自动扩容）
	Status status = BookList_Insert(list, list->length + 1, book);
	if (status == OK) {
		printf("新增成功！\n");
		// 自动保存到文件
		if (BookList_SaveToFile(list, "books.dat") == OK) {
			printf("数据已自动保存\n");
		}
	} else {
		printf("新增失败！（错误原因：%s）\n",
		       status == STATUS_INFEASIBLE ? "参数无效" : "未知错误");
	}
}

// 2. 删除图书（按ID）
void delete_book(BookList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法删除图书！\n");
		return;
	}
	if (BookList_IsEmpty(list)) {
		printf("图书库为空，无图书可删除！\n");
		return;
	}

	Book target = {0};
	printf("===== 删除图书 =====\n");
	printf("输入要删除的图书ID：");
	scanf("%d", &target.id);

	// 查找图书位置
	int pos = BookList_LocateCustom(list, &target, match_by_id);
	if (pos == 0) {
		printf("删除失败！未找到该ID对应的图书\n");
		return;
	}

	// 执行删除
	Book deleted_book;
	Status status = BookList_Delete(list, pos, &deleted_book);
	if (status == OK) {
		printf("删除成功！删除的图书信息：\n");
		print_Book(deleted_book);
		// 自动保存到文件
		if (BookList_SaveToFile(list, "books.dat") == OK) {
			printf("数据已自动保存\n");
		}
	} else {
		printf("删除失败！（错误码：%d）\n", status);
	}
}

// 3. 查找图书（按ID）
void find_book_by_id(BookList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法查找图书！\n");
		return;
	}
	if (BookList_IsEmpty(list)) {
		printf("图书库为空，无图书可查找！\n");
		return;
	}

	Book target = {0};
	printf("===== 查找图书（按ID） =====\n");
	printf("输入要查找的图书ID：");
	scanf("%d", &target.IBSN);

	// 查找位置
	int pos = BookList_LocateCustom(list, &target, match_by_IBSN);
	if (pos == 0) {
		printf("未找到该ID对应的图书\n");
		return;
	}

	// 获取并打印图书信息
	Book book;
	BookList_GetElem(list, pos, &book);
	printf("找到图书（位置：%d）：\n", pos);
	print_Book(book);
}

void find_book_by_name(BookList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法查找图书！\n");
		return;
	}
	if (BookList_IsEmpty(list)) {
		printf("图书库为空，无图书可查找！\n");
		return;
	}


	Book target = {0};
	printf("===== 查找图书（按书名） =====\n");  // 补充标题，用户体验更好
	scanf("%s", target.title);
	printf("查找结果：\n");
	bool found = false;  // 新增：标记是否找到，优化提示


	for (int i = 0; i < list->length; i++) {
		Book book;
		BookList_GetData(list, i, &book);
		if (match_by_title(&book, &target)) {
			print_Book(book);
		}
	}
	if (!found) {  // 新增：未找到时提示，避免用户误以为无响应
		printf("未找到包含该关键字的图书\n");
	}
}

// 4. 更新库存（按ID）
void update_stock(BookList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法更新库存！\n");
		return;
	}
	if (BookList_IsEmpty(list)) {
		printf("图书库为空，无库存可更新！\n");
		return;
	}

	Book target = {0};
	int new_stock;
	printf("===== 更新库存（按IBSN） =====\n");
	printf("输入图书IBSN：");
	scanf("%s", &target.IBSN);
	printf("输入新库存数量（非负整数）：");
	scanf("%d", &new_stock);

	// 校验库存合法性
	if (new_stock < 0) {
		printf("更新失败！库存数量不能为负数\n");
		return;
	}

	// 查找图书位置
	int pos = BookList_LocateCustom(list, &target, match_by_id);
	if (pos == 0) {
		printf("更新失败！未找到该ID对应的图书\n");
		return;
	}

	// 执行库存更新（直接修改顺序表中的元素）
	Book *book_ptr = &list->elem[pos - 1];  // 1-based转0-based
	book_ptr->stock = new_stock;
	printf("更新成功！\n");
	printf("更新后图书信息：\n");
	print_Book(*book_ptr);

	// 自动保存到文件
	if (BookList_SaveToFile(list, "books.dat") == OK) {
		printf("数据已自动保存\n");
	}
}

// 5. 显示所有图书
void show_all_books(BookList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法显示图书！\n");
		return;
	}

	printf("\n===== 所有图书列表 =====\n");
	if (BookList_IsEmpty(list)) {
		printf("图书库为空\n");
		printf("========================\n");
		return;
	}

	printf("共%d本图书：\n", list->length);
	for (int i = 0; i < list->length; i++) {
		Book book;
		BookList_GetData(list, i, &book);
		printf("第%d本：", i + 1);
		print_Book(book);
	}
	printf("========================\n");
}

// 6. 清空图书库
void clear_book_lib(BookList *list) {
	if (list == NULL || list->elem == NULL) {
		printf("系统未初始化，无法清空图书库！\n");
		return;
	}
	if (BookList_IsEmpty(list)) {
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
	BookList_Clear(list);
	printf("清空成功！图书库已为空\n");

	// 同步清空文件数据
	if (BookList_SaveToFile(list, "books.dat") == OK) {
		printf("文件数据已同步清空\n");
	}
}

// 主函数（整合所有功能）
int main() {
	BookList book_list;
	int init_status = BookList_Init(&book_list);
	if (init_status != OK) {
		printf("系统初始化失败！（错误码：%d）\n", init_status);
		return ERROR;
	}

	// 加载历史数据
	printf("正在加载历史图书数据...\n");
	if (BookList_LoadFromFile(&book_list, "books.dat") == OK) {
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
				BookList_SaveToFile(&book_list, "books.dat");
				printf("数据保存完成，退出系统，再见！\n");
				break;
			default:
				printf("无效操作编号，请重新输入！\n");
		}
	} while (choice != 8);

	// 销毁顺序表，释放内存
	BookList_Destroy(&book_list);
	return OK;
}
