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
// Changelog:
//
// 2018-07-7 Initial version from scratch
//
// 2019-11-25 Code refactoring, format conversions moved to
// ../../common/vprec_tools.c
//

#include <argp.h>
#include <err.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "common/float_const.h"
#include "common/float_struct.h"
#include "common/float_utils.h"
#include "common/interflop.h"
#include "common/logger.h"
#include "common/vfc_hashmap.h"
#include "common/vprec_tools.h"
#include "interflop_vprec.h"
#include "interflop_vprec_function_instrumentation.h"

static const char key_prec_b32_str[] = "precision-binary32";
static const char key_prec_b64_str[] = "precision-binary64";
static const char key_range_b32_str[] = "range-binary32";
static const char key_range_b64_str[] = "range-binary64";
static const char key_preset_str[] = "preset";
static const char key_mode_str[] = "mode";
static const char key_err_mode_str[] = "error-mode";
static const char key_err_exp_str[] = "max-abs-error-exponent";
static const char key_daz_str[] = "daz";
static const char key_ftz_str[] = "ftz";

/* variables that control precision, range and mode */

/* Modes' names */
static const char *VPREC_MODE_STR[] = {[vprecmode_ieee] = "ieee",
                                       [vprecmode_full] = "full",
                                       [vprecmode_ib] = "ib",
                                       [vprecmode_ob] = "ob"};

static const char *VPREC_ERR_MODE_STR[] = {[vprec_err_mode_rel] = "rel",
                                           [vprec_err_mode_abs] = "abs",
                                           [vprec_err_mode_all] = "all"};

static const char *VPREC_PRESET_STR[] = {[vprec_preset_binary16] = "binary16",
                                         [vprec_preset_binary32] = "binary32",
                                         [vprec_preset_bfloat16] = "bfloat16",
                                         [vprec_preset_tensorfloat] =
                                             "tensorfloat",
                                         [vprec_preset_fp24] = "fp24",
                                         [vprec_preset_PXR24] = "PXR24"};

static float _vprec_binary32_binary_op(float a, float b,
                                       const vprec_operation op, void *context);
static double _vprec_binary64_binary_op(double a, double b,
                                        const vprec_operation op,
                                        void *context);

/******************** VPREC CONTROL FUNCTIONS *******************
 * The following functions are used to set virtual precision,
 * VPREC mode of operation and instrumentation mode.
 ***************************************************************/

void _set_vprec_mode(vprec_mode mode, t_context *ctx) {
  if (mode >= _vprecmode_end_) {
    logger_error("invalid mode provided, must be one of: "
                 "{ieee, full, ib, ob}.");
  } else {
    ctx->mode = mode;
  }
}

void _set_vprec_precision_binary32(int precision, t_context *ctx) {
  if (precision < VPREC_PRECISION_BINARY32_MIN) {
    logger_error("invalid precision provided for binary32."
                 "Must be greater than %d",
                 VPREC_PRECISION_BINARY32_MIN);
  } else if (VPREC_PRECISION_BINARY32_MAX < precision) {
    logger_error("invalid precision provided for binary32."
                 "Must be lower than %d",
                 VPREC_PRECISION_BINARY32_MAX);
  } else {
    ctx->binary32_precision = precision;
  }
}

void _set_vprec_range_binary32(int range, t_context *ctx) {
  if (range < VPREC_RANGE_BINARY32_MIN) {
    logger_error("invalid range provided for binary32."
                 "Must be greater than %d",
                 VPREC_RANGE_BINARY32_MIN);
  } else if (VPREC_RANGE_BINARY32_MAX < range) {
    logger_error("invalid range provided for binary32."
                 "Must be lower than %d",
                 VPREC_RANGE_BINARY32_MAX);
  } else {
    ctx->binary32_range = range;
  }
}

void _set_vprec_precision_binary64(int precision, t_context *ctx) {
  if (precision < VPREC_PRECISION_BINARY64_MIN) {
    logger_error("invalid precision provided for binary64."
                 "Must be greater than %d",
                 VPREC_PRECISION_BINARY64_MIN);
  } else if (VPREC_PRECISION_BINARY64_MAX < precision) {
    logger_error("invalid precision provided for binary64."
                 "Must be lower than %d",
                 VPREC_PRECISION_BINARY64_MAX);
  } else {
    ctx->binary64_precision = precision;
  }
}

void _set_vprec_range_binary64(int range, t_context *ctx) {
  if (range < VPREC_RANGE_BINARY64_MIN) {
    logger_error("invalid range provided for binary64."
                 "Must be greater than %d",
                 VPREC_RANGE_BINARY64_MIN);
  } else if (VPREC_RANGE_BINARY64_MAX < range) {
    logger_error("invalid range provided for binary64."
                 "Must be lower than %d",
                 VPREC_RANGE_BINARY64_MAX);
  } else {
    ctx->binary64_range = range;
  }
}

void _set_vprec_error_mode(vprec_err_mode mode, t_context *ctx) {
  if (mode >= _vprec_err_mode_end_) {
    logger_error("invalid error mode provided, must be one of: "
                 "{rel, abs, all}.");
  } else {
    switch (mode) {
    case vprec_err_mode_rel:
      ctx->relErr = true;
      ctx->absErr = false;
      break;
    case vprec_err_mode_abs:
      ctx->relErr = false;
      ctx->absErr = true;
      break;
    case vprec_err_mode_all:
      ctx->relErr = true;
      ctx->absErr = true;
    default:
      break;
    }
  }
}

void _set_vprec_max_abs_err_exp(long exponent, t_context *ctx) {
  ctx->absErr_exp = exponent;
}

const char *_get_error_mode_str(t_context *ctx) {
  if (ctx->relErr && ctx->absErr) {
    return VPREC_ERR_MODE_STR[vprec_err_mode_all];
  } else if (ctx->relErr && !ctx->absErr) {
    return VPREC_ERR_MODE_STR[vprec_err_mode_rel];
  } else if (!ctx->relErr && ctx->absErr) {
    return VPREC_ERR_MODE_STR[vprec_err_mode_abs];
  } else {
    return NULL;
  }
}

void _set_vprec_daz(bool daz, t_context *ctx) { ctx->daz = daz; }

void _set_vprec_ftz(bool ftz, t_context *ctx) { ctx->ftz = ftz; }

/******************** VPREC HELPER FUNCTIONS *******************
 * The following functions are used to set virtual precision,
 * VPREC mode of operation and instrumentation mode.
 ***************************************************************/

inline int compute_absErr_vprec_binary32(bool isDenormal,
                                         t_context *currentContext, int expDiff,
                                         int binary32_precision) {
  /* this function is used only when in vprec error mode abs and all,
   * so there is no need to handle vprec error mode rel */
  if (isDenormal == true) {
    /* denormal, or underflow case */
    if (currentContext->relErr == true) {
      /* vprec error mode all */
      if (abs(currentContext->absErr_exp) < binary32_precision)
        return currentContext->absErr_exp;
      else
        return binary32_precision;
    } else {
      /* vprec error mode abs */
      return currentContext->absErr_exp;
    }
  } else {
    /* normal case */
    if (currentContext->relErr == true) {
      /* vprec error mode all */
      if (expDiff < binary32_precision)
        return expDiff;
      else {
        return binary32_precision;
      }
    } else {
      /* vprec error mode abs */
      if (expDiff < FLOAT_PMAN_SIZE) {
        return expDiff;
      } else {
        return FLOAT_PMAN_SIZE;
      }
    }
  }
}

inline int compute_absErr_vprec_binary64(bool isDenormal,
                                         t_context *currentContext, int expDiff,
                                         int binary64_precision) {
  /* this function is used only when in vprec error mode abs and all,
   * so there is no need to handle vprec error mode rel */
  if (isDenormal == true) {
    /* denormal, or underflow case */
    if (currentContext->relErr == true) {
      /* vprec error mode all */
      if (abs(currentContext->absErr_exp) < binary64_precision)
        return currentContext->absErr_exp;
      else
        return binary64_precision;
    } else {
      /* vprec error mode abs */
      return currentContext->absErr_exp;
    }
  } else {
    /* normal case */
    if (currentContext->relErr == true) {
      /* vprec error mode all */
      if (expDiff < binary64_precision)
        return expDiff;
      else {
        return binary64_precision;
      }
    } else {
      /* vprec error mode abs */
      if (expDiff < DOUBLE_PMAN_SIZE) {
        return expDiff;
      } else {
        return DOUBLE_PMAN_SIZE;
      }
    }
  }
}

inline float handle_binary32_normal_absErr(float a, int32_t aexp,
                                           int binary32_precision,
                                           t_context *currentContext) {
  /* absolute error mode, or both absolute and relative error modes */
  int expDiff = aexp - currentContext->absErr_exp;
  float retVal;

  if (expDiff < -1) {
    /* equivalent to underflow on the precision given by absolute error */
    retVal = 0;
  } else if (expDiff == -1) {
    /* case when the number is just below the absolute error threshold,
      but will round to one ulp on the format given by the absolute error;
      this needs to be handled separately, as round_binary32_normal cannot
      generate this number */
    retVal = copysignf(exp2f(currentContext->absErr_exp), a);
  } else {
    /* normal case for the absolute error mode */
    int binary32_precision_adjusted = compute_absErr_vprec_binary32(
        false, currentContext, expDiff, binary32_precision);
    retVal = round_binary32_normal(a, binary32_precision_adjusted);
  }

  return retVal;
}

inline double handle_binary64_normal_absErr(double a, int64_t aexp,
                                            int binary64_precision,
                                            t_context *currentContext) {
  /* absolute error mode, or both absolute and relative error modes */
  int expDiff = aexp - currentContext->absErr_exp;
  double retVal;

  if (expDiff < -1) {
    /* equivalent to underflow on the precision given by absolute error */
    retVal = 0;
  } else if (expDiff == -1) {
    /* case when the number is just below the absolute error threshold,
      but will round to one ulp on the format given by the absolute error;
      this needs to be handled separately, as round_binary32_normal cannot
      generate this number */
    retVal = copysign(exp2(currentContext->absErr_exp), a);
  } else {
    /* normal case for the absolute error mode */
    int binary64_precision_adjusted = compute_absErr_vprec_binary64(
        false, currentContext, expDiff, binary64_precision);
    retVal = round_binary64_normal(a, binary64_precision_adjusted);
  }

  return retVal;
}

/******************** VPREC ARITHMETIC FUNCTIONS ********************
 * The following set of functions perform the VPREC operation. Operands
 * are first correctly rounded to the target precison format if inbound
 * is set, the operation is then perform using IEEE hw and
 * correct rounding to the target precision format is done if outbound
 * is set.
 *******************************************************************/

/* perform_bin_op: applies the binary operator (op) to (a) and (b) */
/* and stores the result in (res) */
#define perform_binary_op(op, res, a, b)                                       \
  switch (op) {                                                                \
  case vprec_add:                                                              \
    res = (a) + (b);                                                           \
    break;                                                                     \
  case vprec_mul:                                                              \
    res = (a) * (b);                                                           \
    break;                                                                     \
  case vprec_sub:                                                              \
    res = (a) - (b);                                                           \
    break;                                                                     \
  case vprec_div:                                                              \
    res = (a) / (b);                                                           \
    break;                                                                     \
  default:                                                                     \
    logger_error("invalid operator %c", op);                                   \
  };

// Round the float with the given precision
float _vprec_round_binary32(float a, char is_input, void *context,
                            int binary32_range, int binary32_precision) {
  t_context *currentContext = (t_context *)context;

  /* test if 'a' is a special case */
  if (!isfinite(a)) {
    return a;
  }

  /* round to zero or set to infinity if underflow or overflow compared to
   * ctx->binary32_range */
  int emax = (1 << (binary32_range - 1)) - 1;
  /* here emin is the smallest exponent in the *normal* range */
  int emin = 1 - emax;

  binary32 aexp = {.f32 = a};
  aexp.s32 = ((FLOAT_GET_EXP & aexp.u32) >> FLOAT_PMAN_SIZE) - FLOAT_EXP_COMP;

  /* check for overflow in target range */
  if (aexp.s32 > emax) {
    a = a * INFINITY;
    return a;
  }

  /* check for underflow in target range */
  if (aexp.s32 < emin) {
    /* underflow case: possibly a denormal */
    if ((currentContext->daz && is_input) ||
        (currentContext->ftz && !is_input)) {
      return a * 0; // preserve sign
    } else if (FP_ZERO == fpclassify(a)) {
      return a;
    } else {
      if (currentContext->absErr == true) {
        /* absolute error mode, or both absolute and relative error modes */
        int binary32_precision_adjusted = compute_absErr_vprec_binary32(
            true, currentContext, 0, binary32_precision);
        a = handle_binary32_denormal(a, emin, binary32_precision_adjusted);
      } else {
        /* relative error mode */
        a = handle_binary32_denormal(a, emin, binary32_precision);
      }
    }
  } else {
    /* else, normal case: can be executed even if a
     previously rounded and truncated as denormal */
    if (currentContext->absErr == true) {
      /* absolute error mode, or both absolute and relative error modes */
      a = handle_binary32_normal_absErr(a, aexp.s32, binary32_precision,
                                        currentContext);
    } else {
      /* relative error mode */
      a = round_binary32_normal(a, binary32_precision);
    }
  }

  return a;
}

// Round the double with the given precision
double _vprec_round_binary64(double a, char is_input, void *context,
                             int binary64_range, int binary64_precision) {
  t_context *currentContext = (t_context *)context;

  /* test if 'a' is a special case */
  if (!isfinite(a)) {
    return a;
  }

  /* round to zero or set to infinity if underflow or overflow compare to
   * ctx->binary64_range */
  int emax = (1 << (binary64_range - 1)) - 1;
  /* here emin is the smallest exponent in the *normal* range */
  int emin = 1 - emax;

  binary64 aexp = {.f64 = a};
  aexp.s64 =
      ((DOUBLE_GET_EXP & aexp.u64) >> DOUBLE_PMAN_SIZE) - DOUBLE_EXP_COMP;

  /* check for overflow in target range */
  if (aexp.s64 > emax) {
    a = a * INFINITY;
    return a;
  }

  /* check for underflow in target range */
  if (aexp.s64 < emin) {
    /* underflow case: possibly a denormal */
    if ((currentContext->daz && is_input) ||
        (currentContext->ftz && !is_input)) {
      return a * 0; // preserve sign
    } else if (FP_ZERO == fpclassify(a)) {
      return a;
    } else {
      if (currentContext->absErr == true) {
        /* absolute error mode, or both absolute and relative error modes */
        int binary64_precision_adjusted = compute_absErr_vprec_binary64(
            true, currentContext, 0, binary64_precision);
        a = handle_binary64_denormal(a, emin, binary64_precision_adjusted);
      } else {
        /* relative error mode */
        a = handle_binary64_denormal(a, emin, binary64_precision);
      }
    }
  } else {
    /* else, normal case: can be executed even if a
     previously rounded and truncated as denormal */
    if (currentContext->absErr == true) {
      /* absolute error mode, or both absolute and relative error modes */
      a = handle_binary64_normal_absErr(a, aexp.s64, binary64_precision,
                                        currentContext);
    } else {
      /* relative error mode */
      a = round_binary64_normal(a, binary64_precision);
    }
  }

  return a;
}

static inline float _vprec_binary32_binary_op(float a, float b,
                                              const vprec_operation op,
                                              void *context) {
  t_context *ctx = (t_context *)context;
  float res = 0;

  if ((ctx->mode == vprecmode_full) || (ctx->mode == vprecmode_ib)) {
    a = _vprec_round_binary32(a, 1, context, ctx->binary32_range,
                              ctx->binary32_precision);
    b = _vprec_round_binary32(b, 1, context, ctx->binary32_range,
                              ctx->binary32_precision);
  }

  perform_binary_op(op, res, a, b);

  if ((ctx->mode == vprecmode_full) || (ctx->mode == vprecmode_ob)) {
    res = _vprec_round_binary32(res, 0, context, ctx->binary32_range,
                                ctx->binary32_precision);
  }

  return res;
}

static inline double _vprec_binary64_binary_op(double a, double b,
                                               const vprec_operation op,
                                               void *context) {
  t_context *ctx = (t_context *)context;
  double res = 0;

  if ((ctx->mode == vprecmode_full) || (ctx->mode == vprecmode_ib)) {
    a = _vprec_round_binary64(a, 1, context, ctx->binary64_range,
                              ctx->binary64_precision);
    b = _vprec_round_binary64(b, 1, context, ctx->binary64_range,
                              ctx->binary64_precision);
  }

  perform_binary_op(op, res, a, b);

  if ((ctx->mode == vprecmode_full) || (ctx->mode == vprecmode_ob)) {
    res = _vprec_round_binary64(res, 0, context, ctx->binary64_range,
                                ctx->binary64_precision);
  }

  return res;
}

// Set precision for internal operations and round input arguments for a given
// function call
void _interflop_enter_function(interflop_function_stack_t *stack, void *context,
                               int nb_args, va_list ap) {
  _vfi_enter_function(stack, context, nb_args, ap);
}

// Set precision for internal operations and round output arguments for a given
// function call
void _interflop_exit_function(interflop_function_stack_t *stack, void *context,
                              int nb_args, va_list ap) {
  _vfi_exit_function(stack, context, nb_args, ap);
}

/************************* FPHOOKS FUNCTIONS *************************
 * These functions correspond to those inserted into the source code
 * during source to source compilation and are replacement to floating
 * point operators
 **********************************************************************/

static void _interflop_add_float(float a, float b, float *c, void *context) {
  *c = _vprec_binary32_binary_op(a, b, vprec_add, context);
}

static void _interflop_sub_float(float a, float b, float *c, void *context) {
  *c = _vprec_binary32_binary_op(a, b, vprec_sub, context);
}

static void _interflop_mul_float(float a, float b, float *c, void *context) {
  *c = _vprec_binary32_binary_op(a, b, vprec_mul, context);
}

static void _interflop_div_float(float a, float b, float *c, void *context) {
  *c = _vprec_binary32_binary_op(a, b, vprec_div, context);
}

static void _interflop_add_double(double a, double b, double *c,
                                  void *context) {
  *c = _vprec_binary64_binary_op(a, b, vprec_add, context);
}

static void _interflop_sub_double(double a, double b, double *c,
                                  void *context) {
  *c = _vprec_binary64_binary_op(a, b, vprec_sub, context);
}

static void _interflop_mul_double(double a, double b, double *c,
                                  void *context) {
  *c = _vprec_binary64_binary_op(a, b, vprec_mul, context);
}

static void _interflop_div_double(double a, double b, double *c,
                                  void *context) {
  *c = _vprec_binary64_binary_op(a, b, vprec_div, context);
}

void _interflop_user_call(void *context, interflop_call_id id, va_list ap) {
  t_context *ctx = (t_context *)context;
  switch (id) {
  case INTERFLOP_SET_PRECISION_BINARY32:
    _set_vprec_precision_binary32(va_arg(ap, int), ctx);
    break;
  case INTERFLOP_SET_PRECISION_BINARY64:
    _set_vprec_precision_binary64(va_arg(ap, int), ctx);
    break;
  case INTERFLOP_SET_RANGE_BINARY32:
    _set_vprec_range_binary32(va_arg(ap, int), ctx);
    break;
  case INTERFLOP_SET_RANGE_BINARY64:
    _set_vprec_range_binary64(va_arg(ap, int), ctx);
    break;
  default:
    logger_warning("Unknown interflop_call id (=%d)", id);
    break;
  }
}

static struct argp_option options[] = {
    /* --debug, sets the variable debug = true */
    {key_prec_b32_str, KEY_PREC_B32, "PRECISION", 0,
     "select precision for binary32 (PRECISION >= 0)", 0},
    {key_prec_b64_str, KEY_PREC_B64, "PRECISION", 0,
     "select precision for binary64 (PRECISION >= 0)", 0},
    {key_range_b32_str, KEY_RANGE_B32, "RANGE", 0,
     "select range for binary32 (0 < RANGE && RANGE <= 8)", 0},
    {key_range_b64_str, KEY_RANGE_B64, "RANGE", 0,
     "select range for binary64 (0 < RANGE && RANGE <= 11)", 0},
    {key_preset_str, KEY_PRESET, "PRESET", 0,
     "select a default PRESET setting among {binary16, binary32, binary64, "
     "bfloat16, tensorfloat, fp24, PXR24}\n"
     "Format (range, precision) : "
     "binary16 (5, 10), binary32 (8, 23), "
     "bfloat16 (8, 7), tensorfloat (8, 10), "
     "fp24 (7, 16), PXR24 (8, 15)",
     0},
    {key_mode_str, KEY_MODE, "MODE", 0,
     "select VPREC mode among {ieee, full, ib, ob}", 0},
    {key_err_mode_str, KEY_ERR_MODE, "ERROR_MODE", 0,
     "select error mode among {rel, abs, all}", 0},
    {key_err_exp_str, KEY_ERR_EXP, "MAX_ABS_ERROR_EXPONENT", 0,
     "select magnitude of the maximum absolute error", 0},
    {key_daz_str, KEY_DAZ, 0, 0,
     "denormals-are-zero: sets denormals inputs to zero", 0},
    {key_ftz_str, KEY_FTZ, 0, 0, "flush-to-zero: sets denormal output to zero",
     0},
    {0}};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  t_context *ctx = (t_context *)state->input;
  state->child_inputs[0] = ctx;
  char *endptr;
  int val = -1;
  int precision = 0;
  int range = 0;

  switch (key) {
  case KEY_PREC_B32:
    /* precision */
    errno = 0;
    val = strtol(arg, &endptr, 10);
    if (errno != 0 || val < VPREC_PRECISION_BINARY32_MIN) {
      logger_error("--%s invalid value provided, must be a "
                   "positive integer.",
                   key_prec_b32_str);
    } else if (val > VPREC_PRECISION_BINARY32_MAX) {
      logger_error("--%s invalid value provided, "
                   "must lower than IEEE binary32 precision (%d)",
                   key_prec_b32_str, VPREC_PRECISION_BINARY32_MAX);
    } else {
      _set_vprec_precision_binary32(val, ctx);
    }
    break;
  case KEY_PREC_B64:
    /* precision */
    errno = 0;
    val = strtol(arg, &endptr, 10);
    if (errno != 0 || val < VPREC_PRECISION_BINARY64_MIN) {
      logger_error("--%s invalid value provided, must be a "
                   "positive integer.",
                   key_prec_b64_str);
    } else if (val > VPREC_PRECISION_BINARY64_MAX) {
      logger_error("--%s invalid value provided, "
                   "must be lower than IEEE binary64 precision (%d)",
                   key_prec_b64_str, VPREC_PRECISION_BINARY64_MAX);
    } else {
      _set_vprec_precision_binary64(val, ctx);
    }
    break;
  case KEY_RANGE_B32:
    /* precision */
    errno = 0;
    val = strtol(arg, &endptr, 10);
    if (errno != 0 || val < VPREC_RANGE_BINARY32_MIN) {
      logger_error("--%s invalid value provided, must be a "
                   "positive integer.",
                   key_range_b32_str);
    } else if (val > VPREC_RANGE_BINARY32_MAX) {
      logger_error("--%s invalid value provided, "
                   "must be lower than IEEE binary32 range size (%d)",
                   key_range_b32_str, VPREC_RANGE_BINARY32_MAX);
    } else {
      _set_vprec_range_binary32(val, ctx);
    }
    break;
  case KEY_RANGE_B64:
    /* precision */
    errno = 0;
    val = strtol(arg, &endptr, 10);
    if (errno != 0 || val < VPREC_RANGE_BINARY64_MIN) {
      logger_error("--%s invalid value provided, must be a "
                   "positive integer.",
                   key_range_b64_str);
    } else if (val > VPREC_RANGE_BINARY64_MAX) {
      logger_error("--%s invalid value provided, "
                   "must be lower than IEEE binary64 range size (%d)",
                   key_range_b64_str, VPREC_RANGE_BINARY64_MAX);
    } else {
      _set_vprec_range_binary64(val, ctx);
    }
    break;
  case KEY_MODE:
    /* mode */
    if (strcasecmp(VPREC_MODE_STR[vprecmode_ieee], arg) == 0) {
      _set_vprec_mode(vprecmode_ieee, ctx);
    } else if (strcasecmp(VPREC_MODE_STR[vprecmode_full], arg) == 0) {
      _set_vprec_mode(vprecmode_full, ctx);
    } else if (strcasecmp(VPREC_MODE_STR[vprecmode_ib], arg) == 0) {
      _set_vprec_mode(vprecmode_ib, ctx);
    } else if (strcasecmp(VPREC_MODE_STR[vprecmode_ob], arg) == 0) {
      _set_vprec_mode(vprecmode_ob, ctx);
    } else {
      logger_error("--%s invalid value provided, must be one of: "
                   "{ieee, full, ib, ob}.",
                   key_mode_str);
    }
    break;
  case KEY_ERR_MODE:
    /* vprec error mode */
    if (strcasecmp(VPREC_ERR_MODE_STR[vprec_err_mode_rel], arg) == 0) {
      _set_vprec_error_mode(vprec_err_mode_rel, ctx);
    } else if (strcasecmp(VPREC_ERR_MODE_STR[vprec_err_mode_abs], arg) == 0) {
      _set_vprec_error_mode(vprec_err_mode_abs, ctx);
    } else if (strcasecmp(VPREC_ERR_MODE_STR[vprec_err_mode_all], arg) == 0) {
      _set_vprec_error_mode(vprec_err_mode_all, ctx);
    } else {
      logger_error("--%s invalid value provided, must be one of: "
                   "{rel, abs, all}.",
                   key_err_mode_str);
    }
    break;
  case KEY_ERR_EXP:
    /* exponent of the maximum absolute error */
    errno = 0;
    long exp = strtol(arg, &endptr, 10);
    if (errno != 0) {
      logger_error("--%s invalid value provided, must be an integer",
                   key_err_exp_str);
    } else {
      _set_vprec_max_abs_err_exp(exp, ctx);
    }
    break;
  case KEY_DAZ:
    /* denormals-are-zero */
    _set_vprec_daz(true, ctx);
    break;
  case KEY_FTZ:
    /* flush-to-zero */
    _set_vprec_ftz(true, ctx);
    break;
  case KEY_PRESET:
    /* preset */
    if (strcmp(VPREC_PRESET_STR[vprec_preset_binary16], arg) == 0) {
      precision = vprec_preset_precision_binary16;
      range = vprec_preset_range_binary16;
    } else if (strcmp(VPREC_PRESET_STR[vprec_preset_binary32], arg) == 0) {
      precision = vprec_preset_precision_binary32;
      range = vprec_preset_range_binary32;
    } else if (strcmp(VPREC_PRESET_STR[vprec_preset_bfloat16], arg) == 0) {
      precision = vprec_preset_precision_bfloat16;
      range = vprec_preset_range_bfloat16;
    } else if (strcmp(VPREC_PRESET_STR[vprec_preset_tensorfloat], arg) == 0) {
      precision = vprec_preset_precision_tensorfloat;
      range = vprec_preset_range_tensorfloat;
    } else if (strcmp(VPREC_PRESET_STR[vprec_preset_fp24], arg) == 0) {
      precision = vprec_preset_precision_fp24;
      range = vprec_preset_range_fp24;
    } else if (strcmp(VPREC_PRESET_STR[vprec_preset_PXR24], arg) == 0) {
      precision = vprec_preset_precision_PXR24;
      range = vprec_preset_range_PXR24;
    } else {
      logger_error("--%s invalid preset provided, must be one of: "
                   "{binary16, binary32, binary64, bfloat16, tensorfloat, "
                   "fp24, PXR24}",
                   key_preset_str);
      break;
    }

    /* set precision */
    _set_vprec_precision_binary32(precision, ctx);
    _set_vprec_precision_binary64(precision, ctx);

    /* set range */
    _set_vprec_range_binary32(range, ctx);
    _set_vprec_range_binary64(range, ctx);

    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp_child argpc[] = {
    {&vfi_argp, 0, "vprec function instrumentation", 0}, {0}};

static struct argp argp = {options, parse_opt, "", "", argpc, NULL, NULL};

/* allocate the context */
void _vprec_alloc_context(void **context) {
  t_context *ctx = (t_context *)malloc(sizeof(t_context));
  _vfi_alloc_context(ctx);
  *context = ctx;
}

/* intialize the context */
void init_context(t_context *ctx) {
  ctx->binary32_precision = VPREC_PRECISION_BINARY32_DEFAULT;
  ctx->binary32_range = VPREC_RANGE_BINARY32_DEFAULT;
  ctx->binary64_precision = VPREC_PRECISION_BINARY64_DEFAULT;
  ctx->binary64_range = VPREC_RANGE_BINARY64_DEFAULT;
  ctx->mode = VPREC_MODE_DEFAULT;
  ctx->relErr = true;
  ctx->absErr = false;
  ctx->absErr_exp = -DOUBLE_EXP_MIN;
  ctx->daz = false;
  ctx->ftz = false;
  _vfi_init_context(ctx);
}

void print_information_header(void *context) {
  /* Environnement variable to disable loading message */
  char *silent_load_env = getenv("VFC_BACKENDS_SILENT_LOAD");
  bool silent_load =
      ((silent_load_env == NULL) || (strcasecmp(silent_load_env, "True") != 0))
          ? false
          : true;

  if (silent_load)
    return;

  t_context *ctx = (t_context *)context;

  logger_info("load backend with: \n");
  logger_info("\t%s = %d\n", key_prec_b32_str, ctx->binary32_precision);
  logger_info("\t%s = %d\n", key_range_b32_str, ctx->binary32_range);
  logger_info("\t%s = %d\n", key_prec_b64_str, ctx->binary64_precision);
  logger_info("\t%s = %d\n", key_range_b64_str, ctx->binary64_range);
  logger_info("\t%s = %s\n", key_mode_str, VPREC_MODE_STR[ctx->mode]);
  logger_info("\t%s = %s\n", key_err_mode_str, _get_error_mode_str(ctx));
  if (ctx->absErr)
    logger_info("\t%s = %d\n", key_err_exp_str, ctx->absErr_exp);
  logger_info("\t%s = %s\n", key_daz_str, ctx->daz ? "true" : "false");
  logger_info("\t%s = %s\n", key_ftz_str, ctx->ftz ? "true" : "false");
  _vfi_print_information_header(context);
}

void _interflop_finalize(void *context) {
  t_context *ctx = (t_context *)context;
  _vfi_finalize(ctx);
}

struct interflop_backend_interface_t interflop_init(int argc, char **argv,
                                                    void **context) {

  /* Initialize the logger */
  logger_init();

  _vprec_alloc_context(context);
  t_context *ctx = *context;
  init_context(ctx);

  /* parse backend arguments */
  argp_parse(&argp, argc, argv, 0, 0, ctx);

  /* initialize vprec function instrumentation context */
  _vfi_init(ctx);

  print_information_header(ctx);

  struct interflop_backend_interface_t interflop_backend_vprec = {
      _interflop_add_float,
      _interflop_sub_float,
      _interflop_mul_float,
      _interflop_div_float,
      NULL,
      _interflop_add_double,
      _interflop_sub_double,
      _interflop_mul_double,
      _interflop_div_double,
      NULL,
      _interflop_enter_function,
      _interflop_exit_function,
      _interflop_user_call,
      _interflop_finalize};

  return interflop_backend_vprec;
}
