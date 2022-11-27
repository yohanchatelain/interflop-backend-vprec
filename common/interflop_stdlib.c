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
 *  Copyright (c) 2022                                                       *\
 *     Verificarlo Contributors                                              *\
 *                                                                           *\
 ****************************************************************************/

#include "interflop_stdlib.h"

static int __string_equal(const char *str1, const char *str2) {
  const int length_limit = 2048;

  if (str1 == Null || str2 == Null) {
    return 0;
  }

  int i = 0;
  while (str1[i] == str2[i] && str1[i] != '\0' && str2[i] != '\0' &&
         i <= length_limit) {
    i++;
  }

  if (str1[i] == '\0' && str2[i] == '\0') {
    return 1;
  } else {
    return 0;
  }
}

#define SET_HANDLER(F)                                                         \
  else if (__string_equal(name, #F)) interflop_##F =                           \
      (interflop_##F##_t)function_ptr;

interflop_malloc_t interflop_malloc = Null;
interflop_fopen_t interflop_fopen = Null;
interflop_panic_t interflop_panic = Null;
interflop_strcmp_t interflop_strcmp = Null;
interflop_strcasecmp_t interflop_strcasecmp = Null;
interflop_strtol_t interflop_strtol = Null;
interflop_getenv_t interflop_getenv = Null;
interflop_fprintf_t interflop_fprintf = Null;
interflop_strcpy_t interflop_strcpy = Null;
interflop_fclose_t interflop_fclose = Null;
interflop_gettid_t interflop_gettid = Null;
interflop_strerror_t interflop_strerror = Null;
interflop_sprintf_t interflop_sprintf = Null;
interflop_vwarnx_t interflop_vwarnx = Null;
interflop_vfprintf_t interflop_vfprintf = Null;
interflop_exit_t interflop_exit = Null;
interflop_strtok_r_t interflop_strtok_r = Null;
interflop_fgets_t interflop_fgets = Null;
interflop_free_t interflop_free = Null;
interflop_calloc_t interflop_calloc = Null;
interflop_argp_parse_t interflop_argp_parse = Null;

void interflop_set_handler(const char *name, void *function_ptr) {
  if (name == Null) {
    return;
  }
  SET_HANDLER(malloc)
  SET_HANDLER(fopen)
  SET_HANDLER(panic)
  SET_HANDLER(strcmp)
  SET_HANDLER(strcasecmp)
  SET_HANDLER(strtol)
  SET_HANDLER(getenv)
  SET_HANDLER(fprintf)
  SET_HANDLER(strcpy)
  SET_HANDLER(fclose)
  SET_HANDLER(gettid)
  SET_HANDLER(strerror)
  SET_HANDLER(sprintf)
  SET_HANDLER(vwarnx)
  SET_HANDLER(vfprintf)
  SET_HANDLER(exit)
  SET_HANDLER(strtok_r)
  SET_HANDLER(fgets)
  SET_HANDLER(free)
  SET_HANDLER(calloc)
  SET_HANDLER(argp_parse)
}

#include "float_const.h"

/* lib math */

typedef union {
  int i;
  float f;
} interflop_b32;

typedef union {
  long i;
  double d;
} interflop_b64;

typedef enum {
  IFP_NAN,
  IFP_INFINITE,
  IFP_ZERO,
  IFP_SUBNORMAL,
  IFS_NORMAL,
} interflop_fpclassify_e;

float interflop_fpclassifyf(float x) {
  interflop_b32 u = {.f = x};
  int exp = u.i & FLOAT_GET_EXP;
  int mant = u.i & FLOAT_GET_PMAN;
  if (exp == 0 && mant == 0) {
    return IFP_ZERO;
  } else if (exp == FLOAT_EXP_MAX && mant == 0) {
    return IFP_INFINITE;
  } else if (exp == FLOAT_EXP_MAX && mant != 0) {
    return IFP_NAN;
  } else if (exp == 0 && mant != 0) {
    return IFP_SUBNORMAL;
  } else {
    return IFS_NORMAL;
  }
}

double interflop_fpclassifyd(double x) {
  interflop_b64 u = {.d = x};
  long exp = u.i & DOUBLE_GET_EXP;
  long mant = u.i & DOUBLE_GET_PMAN;
  if (exp == 0 && mant == 0) {
    return IFP_ZERO;
  } else if (exp == FLOAT_EXP_MAX && mant == 0) {
    return IFP_INFINITE;
  } else if (exp == FLOAT_EXP_MAX && mant != 0) {
    return IFP_NAN;
  } else if (exp == 0 && mant != 0) {
    return IFP_SUBNORMAL;
  } else {
    return IFS_NORMAL;
  }
}

/* returns 2**i convert to a float */
float fpow2i(int i) {
  interflop_b32 x = {.f = 0};

  /* 2**-149 <= result <= 2**127 */
  int exp = i + FLOAT_EXP_COMP;
  if (exp <= -FLOAT_PMAN_SIZE) {
    /* underflow */
    x.f = 0.0f;
  } else if (exp >= FLOAT_EXP_MAX) {
    /* overflow */
    x.i = FLOAT_PLUS_INF;
  } else if (exp <= 0) {
    /* subnormal result */
    /* -149 + 127 = -22 */
    x.i = 1 << (FLOAT_PMAN_SIZE - 1 + exp);
  } else {
    /* normal result */
    x.i = exp << FLOAT_PMAN_SIZE;
  }
  return x.f;
}

/* returns 2**i convert to a double */
double pow2i(int i) {
  interflop_b64 x = {.d = 0};

  /* 2**-1074 <= result <= 2**1023 */
  long int exp = i + DOUBLE_EXP_COMP;
  if (exp <= -DOUBLE_PMAN_SIZE) {
    /* underflow */
    x.d = 0.0;
  } else if (exp >= DOUBLE_EXP_MAX) {
    /* overflow */
    x.i = DOUBLE_PLUS_INF;
  } else if (exp <= 0) {
    /* subnormal result */
    /* -1074 + 1023 = -51 */
    x.i = 1 << (DOUBLE_PMAN_SIZE - 1 + exp);
  } else {
    /* normal result */
    x.i = exp << DOUBLE_PMAN_SIZE;
  }
  return x.d;
}

int isnanf(float x) { return interflop_fpclassifyf(x) == IFP_NAN; }

int isnand(double x) { return interflop_fpclassifyd(x) == IFP_NAN; }

int isinff(float x) { return interflop_fpclassifyf(x) == IFP_INFINITE; }

int isinfd(double x) { return interflop_fpclassifyd(x) == IFP_INFINITE; }
#include <math.h>

float interflop_floorf(float x) {
  return __builtin_floor(x);
  interflop_b32 u = {.f = x};
  int mant = u.i & FLOAT_GET_PMAN;
  interflop_fpclassify_e class = interflop_fpclassifyf(x);
  if (class == IFP_ZERO || class == IFP_INFINITE || class == IFP_NAN ||
      mant == 0) {
    return x;
  }

  int exp = (u.i & FLOAT_GET_EXP) - FLOAT_EXP_COMP;

  if (exp < 0) {
    return (x < 0) ? -1 : 0;
  }

  int ri = 1 << exp;
  float rf = (x < 0) ? -ri - 1 : ri;
  return rf;
}

double interflop_floord(double x) {
  return __builtin_floor(x);
  interflop_b64 u = {.d = x};
  long mant = u.i & DOUBLE_GET_PMAN;
  interflop_fpclassify_e class = interflop_fpclassifyf(x);
  if (class == IFP_ZERO || class == IFP_INFINITE || class == IFP_NAN ||
      mant == 0) {
    return x;
  }

  /* get unbiased exponent */
  int exp = (u.i & FLOAT_GET_EXP) - FLOAT_EXP_COMP;

  /* return 0 or 1 if |x| < 1 */
  if (exp < 0) {
    return (x < 0) ? 0 : 1;
  }

  /* if exponent is large enough to have integral part, returns x */
  unsigned int msb = (FLOAT_GET_PMAN >> 1) ^ FLOAT_GET_PMAN;

  /* add one to x if x is positive */
  if (x > 0) {
    u.i += msb >> exp;
  }

  /* erases bits in mantissa after position k (=exp) which (exp * 2**-k) < 1  */
  u.i &= ~(FLOAT_GET_PMAN >> exp);

  return u.d;
}

float interflop_ceilf(float x) {
  return __builtin_ceil(x);
  interflop_b32 u = {.f = x};
  int mant = u.i & FLOAT_GET_PMAN;
  interflop_fpclassify_e class = interflop_fpclassifyf(x);
  if (class == IFP_ZERO || class == IFP_INFINITE || class == IFP_NAN ||
      mant == 0) {
    return x;
  }

  /* get unbiased exponent */
  int exp = (u.i & FLOAT_GET_EXP) - FLOAT_EXP_COMP;

  /* return 0 or 1 if |x| < 1 */
  if (exp < 0) {
    return (x < 0) ? 0 : 1;
  }

  /* if exponent is large enough to have integral part, returns x */
  unsigned int msb = (FLOAT_GET_PMAN >> 1) ^ FLOAT_GET_PMAN;

  /* add one to x if x is negative */
  if (x < 0) {
    u.i += msb >> exp;
  }

  /* erases bits in mantissa after position k (=exp) which (exp * 2**-k) < 1  */
  u.i &= ~(FLOAT_GET_PMAN >> exp);

  return u.f;
}

double interflop_ceild(double x) {
  return __builtin_ceil(x);
  interflop_b64 u = {.d = x};
  long mant = u.i & DOUBLE_GET_PMAN;
  interflop_fpclassify_e class = interflop_fpclassifyf(x);
  if (class == IFP_ZERO || class == IFP_INFINITE || class == IFP_NAN ||
      mant == 0) {
    return x;
  }

  /* get unbiased exponent */
  long exp = (u.i & DOUBLE_GET_EXP) - DOUBLE_EXP_COMP;

  /* return 0 or 1 if |x| < 1 */
  if (exp < 0) {
    return (x < 0) ? 0 : 1;
  }

  /* if exponent is large enough to have integral part, returns x */
  if (exp - DOUBLE_PMAN_SIZE >= 0) {
    return x;
  }

  /* get mantissa's most significant bit to 1 */
  unsigned long msb = (DOUBLE_GET_PMAN >> 1ULL) ^ DOUBLE_GET_PMAN;

  /* add one to x if x is negative */
  if (x < 0) {
    u.i += msb >> exp;
  }

  /* erases bits in mantissa after position k (=exp) which (exp * 2**-k) < 1  */
  u.i &= ~(DOUBLE_GET_PMAN >> exp);

  return u.d;
}
