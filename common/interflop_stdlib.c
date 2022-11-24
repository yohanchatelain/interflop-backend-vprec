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

Bool False = 0;
Bool True = 1;

static Bool __string_equal(const char *str1, const char *str2) {
  if (str1 == Null || str2 == Null) {
    return False;
  }

  int i = 0;
  while (str1[i] == str2[i] && str1[i] != '\0' && str2[i] != '\0') {
    i++;
  }

  if (str1[i] != '\0' || str2[i] != '\0') {
    return False;
  } else {
    return True;
  }
}

interflop_malloc_t interflop_malloc = Null;

void interflop_set_handler(const char *name, void *(*function_ptr)()) {
  if (__string_equal(name, "malloc")) {
    interflop_malloc = (interflop_malloc_t)function_ptr;
  }
}
