

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "utils_lists.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Borrowed code from R/plot3d.c//growList
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP grow_list(SEXP oldlist) {
  int len = LENGTH(oldlist);
  SEXP newlist = PROTECT(allocVector(VECSXP, 2 * len));
  for (int i=0; i<len; i++) {
    SET_VECTOR_ELT(newlist, i, VECTOR_ELT(oldlist, i));
  }

  UNPROTECT(1);
  return newlist;
}
