/*****************************************************************************\
 *                                                                           *\
 *  This file is part of the Verificarlo project,                            *\
 *  under the Apache License v2.0 with LLVM Exceptions.                      *\
 *  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.                 *\
 *  See https://llvm.org/LICENSE.txt for license information.                *\
 *                                                                           *\
 *                                                                           *\
 *  Copyright (c) 2015                                                       *\
 *     Universite de Versailles St-Quentin-en-Yvelines                       *\
 *     CMLA, Ecole Normale Superieure de Cachan                              *\
 *                                                                           *\
 *  Copyright (c) 2018                                                       *\
 *     Universite de Versailles St-Quentin-en-Yvelines                       *\
 *                                                                           *\
 *  Copyright (c) 2019-2022                                                  *\
 *     Verificarlo Contributors                                              *\
 *                                                                           *\
 ****************************************************************************/

#ifndef __INTERFLOP_STDLIB_H__
#define __INTERFLOP_STDLIB_H__

#include <stdarg.h>

#define Null 0

typedef long unsigned int ISize_t;
typedef void File;
typedef int IBool;

#define ITrue 1
#define IFalse 0

#define EXIT_FAILURE 1

typedef void (*interflop_set_handler_t)(const char *name, void *function_ptr);

void interflop_set_handler(const char *name, void *function_ptr);

typedef void *(*interflop_malloc_t)(ISize_t);
typedef File *(*interflop_fopen_t)(const char *pathname, const char *mode,
                                   int *error);
typedef void (*interflop_panic_t)(const char *);
typedef int (*interflop_strcmp_t)(const char *s1, const char *s2);
typedef int (*interflop_strcasecmp_t)(const char *s1, const char *s2);
/* Do not follow libc API, use error pointer to pass errno result instead */
/* error = 0 if success */
typedef long (*interflop_strtol_t)(const char *nptr, char **endptr, int *error);
typedef char *(*interflop_getenv_t)(const char *name);
typedef int (*interflop_fprintf_t)(File *stream, const char *format, ...);
typedef char (*interflop_strcpy_t)(char *dest, const char *src);
typedef int (*interflop_fclose_t)(File *stream);
typedef int (*interflop_gettid_t)(void);
typedef char *(*interflop_strerror_t)(int error);
typedef int (*interflop_sprintf_t)(char *str, const char *format, ...);
typedef void (*interflop_vwarnx_t)(const char *fmt, va_list args);
typedef int (*interflop_vfprintf_t)(File *stream, const char *format,
                                    va_list ap);
typedef void (*interflop_exit_t)(int status);
typedef char *(*interflop_strtok_r_t)(char *str, const char *delim,
                                      char **saveptr);
typedef char *(*interflop_fgets_t)(char *s, int size, File *stream);
typedef void (*interflop_free_t)(void *ptr);
typedef void *(*interflop_calloc_t)(ISize_t nmemb, ISize_t size);
typedef int (*interflop_argp_parse_t)(void *__argp, int __argc, char **__argv,
                                      unsigned int __flags, int *__arg_index,
                                      void *__input);

extern interflop_malloc_t interflop_malloc;
extern interflop_fopen_t interflop_fopen;
extern interflop_panic_t interflop_panic;
extern interflop_strcmp_t interflop_strcmp;
extern interflop_strcasecmp_t interflop_strcasecmp;
extern interflop_strtol_t interflop_strtol;
extern interflop_getenv_t interflop_getenv;
extern interflop_fprintf_t interflop_fprintf;
extern interflop_strcpy_t interflop_strcpy;
extern interflop_fclose_t interflop_fclose;
extern interflop_gettid_t interflop_gettid;
extern interflop_strerror_t interflop_strerror;
extern interflop_sprintf_t interflop_sprintf;
extern interflop_vwarnx_t interflop_vwarnx;
extern interflop_vfprintf_t interflop_vfprintf;
extern interflop_exit_t interflop_exit;
extern interflop_strtok_r_t interflop_strtok_r;
extern interflop_fgets_t interflop_fgets;
extern interflop_free_t interflop_free;
extern interflop_calloc_t interflop_calloc;
extern interflop_argp_parse_t interflop_argp_parse;

float fpow2i(int i);
double pow2i(int i);
int isnanf(float x);
int isnand(double x);
int isinff(float x);
int isinfd(double x);
float interflop_floorf(float x);
double interflop_floord(double x);
float interflop_ceilf(float x);
double interflop_ceild(double x);

#define interflop_fpclassify(x)                                                \
  _Generic(x, float : interflop_fpclassifyf, double : interflop_fpclassifyd)(x)

#define interflop_isnan(x) _Generic(x, float : isnanf, double : isnand)(x)
#define interflop_isinf(x) _Generic(x, float : isinff, double : isinfd)(x)

#define interflop_floor(x)                                                     \
  _Generic(x, float : interflop_floorf, double : interflop_floord)(x)

#define interflop_ceil(x)                                                      \
  _Generic(x, float : interflop_ceilf, double : interflop_ceild)(x)

#endif /* __INTERFLOP_STDLIB_H__ */