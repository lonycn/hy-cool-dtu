#ifndef __STDIO_H__
#define __STDIO_H__

/**
 * @file stdio.h
 * @brief 简化的标准输入输出库
 * 为嵌入式环境提供基本的stdio函数声明
 */

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
  Basic Types
 *----------------------------------------------------------------------------*/
#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef size_t
typedef unsigned int size_t;
#endif

/*----------------------------------------------------------------------------
  Function Declarations
 *----------------------------------------------------------------------------*/

// 基本输出函数
extern int printf(const char *format, ...);
extern int sprintf(char *str, const char *format, ...);
extern int snprintf(char *str, size_t size, const char *format, ...);

// 基本输入函数
extern int scanf(const char *format, ...);
extern int sscanf(const char *str, const char *format, ...);

// 字符输入输出
extern int putchar(int c);
extern int getchar(void);
extern int puts(const char *s);
extern char *gets(char *s);

// 文件操作（简化版本）
typedef struct {
    int dummy;
} FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

extern int fputc(int c, FILE *stream);
extern int fputs(const char *s, FILE *stream);
extern int fgetc(FILE *stream);
extern char *fgets(char *s, int size, FILE *stream);

/*----------------------------------------------------------------------------
  Weak implementations for embedded environment
 *----------------------------------------------------------------------------*/

// 这些函数需要在其他地方实现，或者提供弱符号
#pragma weak printf
#pragma weak sprintf
#pragma weak snprintf
#pragma weak scanf
#pragma weak sscanf
#pragma weak putchar
#pragma weak getchar
#pragma weak puts
#pragma weak gets
#pragma weak fputc
#pragma weak fputs
#pragma weak fgetc
#pragma weak fgets

#ifdef __cplusplus
}
#endif

#endif /* __STDIO_H__ */