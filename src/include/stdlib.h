#ifndef __STDLIB_H__
#define __STDLIB_H__

/**
 * @file stdlib.h
 * @brief 简化的标准库
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
  Memory Management
 *----------------------------------------------------------------------------*/

extern void *malloc(size_t size);
extern void *calloc(size_t num, size_t size);
extern void *realloc(void *ptr, size_t size);
extern void free(void *ptr);

/*----------------------------------------------------------------------------
  Number Conversion
 *----------------------------------------------------------------------------*/

extern int atoi(const char *nptr);
extern long atol(const char *nptr);
extern long long atoll(const char *nptr);
extern double atof(const char *nptr);

extern long strtol(const char *nptr, char **endptr, int base);
extern unsigned long strtoul(const char *nptr, char **endptr, int base);
extern long long strtoll(const char *nptr, char **endptr, int base);
extern unsigned long long strtoull(const char *nptr, char **endptr, int base);

/*----------------------------------------------------------------------------
  Random Numbers
 *----------------------------------------------------------------------------*/

extern int rand(void);
extern void srand(unsigned int seed);

#define RAND_MAX 32767

/*----------------------------------------------------------------------------
  System Functions
 *----------------------------------------------------------------------------*/

extern void abort(void);
extern void exit(int status);
extern int atexit(void (*function)(void));

/*----------------------------------------------------------------------------
  Weak symbols
 *----------------------------------------------------------------------------*/

#pragma weak malloc
#pragma weak calloc
#pragma weak realloc
#pragma weak free
#pragma weak atoi
#pragma weak atol
#pragma weak atoll
#pragma weak atof
#pragma weak strtol
#pragma weak strtoul
#pragma weak strtoll
#pragma weak strtoull
#pragma weak rand
#pragma weak srand
#pragma weak abort
#pragma weak exit
#pragma weak atexit

#ifdef __cplusplus
}
#endif

#endif /* __STDLIB_H__ */