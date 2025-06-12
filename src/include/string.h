#ifndef __STRING_H__
#define __STRING_H__

/**
 * @file string.h
 * @brief 简化的字符串操作库
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef size_t
typedef unsigned int size_t;
#endif

/*----------------------------------------------------------------------------
  String Functions
 *----------------------------------------------------------------------------*/

// 字符串长度和比较
extern size_t strlen(const char *s);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern int strcasecmp(const char *s1, const char *s2);

// 字符串复制
extern char *strcpy(char *dest, const char *src);
extern char *strncpy(char *dest, const char *src, size_t n);

// 字符串连接
extern char *strcat(char *dest, const char *src);
extern char *strncat(char *dest, const char *src, size_t n);

// 字符串搜索
extern char *strchr(const char *s, int c);
extern char *strrchr(const char *s, int c);
extern char *strstr(const char *haystack, const char *needle);

// 内存操作
extern void *memcpy(void *dest, const void *src, size_t n);
extern void *memmove(void *dest, const void *src, size_t n);
extern void *memset(void *s, int c, size_t n);
extern int memcmp(const void *s1, const void *s2, size_t n);

// 弱符号声明
#pragma weak strlen
#pragma weak strcmp
#pragma weak strncmp
#pragma weak strcasecmp
#pragma weak strcpy
#pragma weak strncpy
#pragma weak strcat
#pragma weak strncat
#pragma weak strchr
#pragma weak strrchr
#pragma weak strstr
#pragma weak memcpy
#pragma weak memmove
#pragma weak memset
#pragma weak memcmp

#ifdef __cplusplus
}
#endif

#endif /* __STRING_H__ */