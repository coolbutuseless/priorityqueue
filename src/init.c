
// #define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

extern SEXP cpq_create_();
extern SEXP cpq_length_();
extern SEXP cpq_insert_();
extern SEXP cpq_pop_();
extern SEXP cpq_peek_();
extern SEXP cpq_dump_();

static const R_CallMethodDef CEntries[] = {
  {"cpq_create_", (DL_FUNC) &cpq_create_, 0},
  {"cpq_length_", (DL_FUNC) &cpq_length_, 1},
  {"cpq_insert_", (DL_FUNC) &cpq_insert_, 3},
  {"cpq_pop_"   , (DL_FUNC) &cpq_pop_   , 1},
  {"cpq_peek_"  , (DL_FUNC) &cpq_peek_  , 1},
  {"cpq_dump_"  , (DL_FUNC) &cpq_dump_  , 1},
  {NULL , NULL, 0}
};


void R_init_priorityqueue(DllInfo *info) {
  R_registerRoutines(
    info,      // DllInfo
    NULL,      // .C
    CEntries,  // .Call
    NULL,      // Fortran
    NULL       // External
  );
  R_useDynamicSymbols(info, FALSE);
}



