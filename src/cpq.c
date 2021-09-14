

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "utils_lists.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Storage type
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef struct cpq_t {
  double *priority;
  uint32_t *elem_idx;
  uint32_t size;
  uint32_t len;
  uint32_t max_list_idx;
} cpq_t;



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Macros
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define INITQUEUESIZE 128
#define LCHILD(i) (((i) << 1) + 1)
#define RCHILD(i) (((i) << 1) + 2)
#define PARENT(i)  ((i - 1) >> 1)

#define INITLISTSIZE 128


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Unbox an ExternalPtr to an 'cpq_t *' pointer
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cpq_t *unbox_cpq(SEXP cpq_) {

  if (!inherits(cpq_, "cpq")) error("Expecting 'cpq' to be an 'cpq_t *' ExternalPtr");

  cpq_t *cpq = TYPEOF(cpq_) != EXTPTRSXP ? NULL : (cpq_t *)R_ExternalPtrAddr(cpq_);
  if (cpq == NULL) error("'myqq_t *cpq' pointer is invalid/NULL");

  return cpq;
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Free the allocated resources
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void cpq_finalizer(SEXP cpq_) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack the external pointer
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  cpq_t *pq = (cpq_t *)R_ExternalPtrAddr(cpq_);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Free the priority vector and the element index vector
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  free(pq->priority);
  free(pq->elem_idx);

  free(pq);
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialise a cpq_t pointer
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP cpq_create_() {
  cpq_t *pq = calloc(1, sizeof(cpq_t));

  pq->size         = INITQUEUESIZE;
  pq->priority     = calloc(pq->size, sizeof(double));
  pq->elem_idx     = calloc(pq->size, sizeof(uint32_t));
  pq->len          = 0;
  pq->max_list_idx = 0;

  for (int i=0; i<pq->size; i++) {
    pq->priority[i] = 0;
    pq->elem_idx[i] = 0;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Wrap everything for the priority queue in C
  // Keep a normal R list as a protected member of this pointer
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  SEXP cpq_ = PROTECT(R_MakeExternalPtr(
    pq,                               // ptr
    R_NilValue,                       // tag
    allocVector(VECSXP, INITLISTSIZE) // prot
  ));

  R_RegisterCFinalizer(cpq_, cpq_finalizer);
  SET_CLASS(cpq_, mkString("cpq"));

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unprotect and return
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  UNPROTECT(1);
  return cpq_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Length of priority queue
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP cpq_length_(SEXP cpq_) {
  cpq_t *pq = unbox_cpq(cpq_);
  return (ScalarInteger(pq->len));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sift up a newly added member at the end of this queue
//
// Assumes:
//   * new member has been added
//   * 'len' correctly reflects this new addition
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void cpq_sift_up(cpq_t *pq) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Find the last item and its parent
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int this_idx = pq->len - 1;
  int parent_idx = PARENT(this_idx);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // while we are not at the root, and the parent priority is less than
  // the current prioirty, swap parent and child.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  while (parent_idx >= 0 && (pq->priority[parent_idx] < pq->priority[this_idx])) {

    // Swap prioirities
    double tmp_priority = pq->priority[parent_idx];
    pq->priority[parent_idx] = pq->priority[this_idx];
    pq->priority[this_idx] = tmp_priority;

    // Swap element indices corresponding to the priorities
    uint32_t tmp_elem_idx = pq->elem_idx[parent_idx];
    pq->elem_idx[parent_idx] = pq->elem_idx[this_idx];
    pq->elem_idx[this_idx] = tmp_elem_idx;

    // try with the next index/parent up the tree
    this_idx   = parent_idx;
    parent_idx = PARENT(parent_idx);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sift down
//
// Assumes
//   - top node is no longer value.
//   - top node can be freed (if it was allocated)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void cpq_sift_down(cpq_t *pq) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // 1. Store the first time in the tree in some temporary variable.
  // 2. Replace the first node with the item in the last node.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  pq->len--;
  pq->priority[0] = pq->priority[pq->len];
  pq->elem_idx[0] = pq->elem_idx[pq->len];

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // 3. Check the first node with its children nodes.
  //    If the left child is larger, we swap it with the left child.
  //    If the right node is larger, we swap it with the right node.
  // 4. Repeat step 3 until the parent node is larger than the left and right child node.
  // 5. Return the maximum value (stored in the temporary variable).
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int parent_idx = 0;
  while (1) {
    double parent_pri = pq->priority[parent_idx];
    int lchild = LCHILD(parent_idx);
    int rchild = RCHILD(parent_idx);
    int swapchild = 0;
    if (rchild < pq->len) {
      double lpri = pq->priority[lchild];
      double rpri = pq->priority[rchild];
      if (lpri > rpri && lpri > parent_pri) {
        swapchild = lchild;
      } else if (rpri > lpri && rpri > parent_pri) {
        swapchild = rchild;
      }
    } else if (lchild < pq->len) {
      double lpri = pq->priority[lchild];
      if (lpri > parent_pri) {
        swapchild = lchild;
      }
    }

    if (swapchild == 0) {
      // no child is worth swapping
      break;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Sift down the priority
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double tmp_priority = pq->priority[parent_idx];
    pq->priority[parent_idx] = pq->priority[swapchild];
    pq->priority[swapchild] = tmp_priority;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Sift down the element index which corresponds to this priority
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    uint32_t tmp_elem_idx = pq->elem_idx[parent_idx];
    pq->elem_idx[parent_idx] = pq->elem_idx[swapchild];
    pq->elem_idx[swapchild] = tmp_elem_idx;


    parent_idx = swapchild;
  }


}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Insert
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP cpq_insert_(SEXP cpq_, SEXP obj_, SEXP priority_) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack the external pointer and the protection list
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  cpq_t *pq = unbox_cpq(cpq_);
  SEXP prot_list = R_ExternalPtrProtected(cpq_);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Check if the queue needs more memory
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (pq->len == pq->size) {
    // alloc more memory for the priority queue
    pq->size *= 2;
    // Rprintf("insert: double size to %i\n", pq->size);
    pq->priority = realloc(pq->priority, pq->size * sizeof(double));
    pq->elem_idx = realloc(pq->elem_idx, pq->size * sizeof(uint32_t));
    if (pq->priority == NULL|| pq->elem_idx == NULL) {
      error("insert: growth realloc failed");
    }
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Check if the protection list needs more memory
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (pq->max_list_idx == length(prot_list)) {
    // grow list
    // Rprintf("Growing R list from length = %i\n", length(prot_list));
    prot_list = PROTECT(grow_list(prot_list));
    R_SetExternalPtrProtected(cpq_, prot_list);
    UNPROTECT(1);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Insert the new object into the protection list
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  SET_VECTOR_ELT(prot_list, pq->max_list_idx, obj_);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Insert the new prioirty + index in the priority queue, then bubble up
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  pq->priority[pq->len] = asReal(priority_);
  pq->elem_idx[pq->len] = pq->max_list_idx;

  pq->len++;
  pq->max_list_idx++;

  cpq_sift_up(pq);


  return R_NilValue;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Pop
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP cpq_pop_(SEXP cpq_) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack the external pointer and the protection list
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  cpq_t *pq = unbox_cpq(cpq_);
  if (pq->len == 0) {
    error("Cannot 'pop' from empty queue");
  }
  SEXP prot_list = R_ExternalPtrProtected(cpq_);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // get the list index of the highest priority element
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  uint32_t idx = pq->elem_idx[0];

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // bubble down.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  cpq_sift_down(pq);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // get the top element from the protected list
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  SEXP top = VECTOR_ELT(prot_list, idx);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Remove element from protection so it can be garbage collected
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  SET_VECTOR_ELT(prot_list, idx, R_NilValue);


  return top;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Peek
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP cpq_peek_(SEXP cpq_) {

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unpack the external pointer and the protection list
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  cpq_t *pq = unbox_cpq(cpq_);
  if (pq->len == 0) {
    error("Cannot 'peek' into empty queue");
  }
  SEXP prot_list = R_ExternalPtrProtected(cpq_);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Find the index of the highest prioirty item, fetch it from the
  // protection list and return it.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  return VECTOR_ELT(prot_list, pq->elem_idx[0]);
}

