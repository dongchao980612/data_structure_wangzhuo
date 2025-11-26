#ifndef __STATUS_H
#define __STATUS_H

/**
 * @file status.h
 * @brief 数据结构通用状态码定义
 * @details 用于标识函数执行结果、逻辑判断结果等，适用于链表、栈、队列等数据结构操作
 * @note 状态码取值范围：1（成功/真）、0（失败/假）、-1~-3（错误类型）
 */

// 状态码枚举（增强类型语义）
typedef enum {
	STATUS_TRUE = 1,         // 真/是
	STATUS_FALSE = 0,        // 假/否
	STATUS_OK = 1,           // 操作成功
	STATUS_ERROR = 0,        // 操作失败
	STATUS_INFEASIBLE = -1,  // 操作不可行（参数矛盾等）
	STATUS_OVERFLOW = -2,    // 堆栈上溢
	STATUS_UNINITIALIZED = -3 // 未初始化（未调用Init函数等）
} Status;

// 兼容旧宏定义（确保现有代码兼容性）
#define TRUE        	STATUS_TRUE
#define FALSE       	STATUS_FALSE
#define OK          	STATUS_OK
#define ERROR       	STATUS_ERROR
#define INFEASIBLE  	STATUS_INFEASIBLE
#define OVERFLOW    	STATUS_OVERFLOW
#define UNINITIALIZED 	STATUS_UNINITIALIZED

// 兼容C/C++的NULL定义（避免与系统定义冲突）
#ifndef NULL
	#ifdef __cplusplus
		#define NULL 0
	#else
		#define NULL ((void*)0)
	#endif
#endif




#endif // __STATUS_H
