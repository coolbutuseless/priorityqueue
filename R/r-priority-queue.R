
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' A max priority queue for R objects (implemented in R)
#'
#' @param rpq priority queue object as created by \code{rpq_create()}
#' @param obj Any R object
#' @param priority numeric prioirty
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rpq_create <- function() {
  rpq <- new.env()
  class(rpq) <- 'rpq'

  rpq$obj      <- list()
  rpq$priority <- double()
  rpq$sorted   <- FALSE
  rpq$first    <- 0L

  rpq
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname rpq_create
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rpq_insert <- function(rpq, obj, priority) {
  stopifnot(inherits(rpq, 'rpq'))

  rpq$obj      <- c(rpq$obj, list(obj))
  rpq$priority <- c(rpq$priority, priority)
  rpq$sorted   <- FALSE

  invisible(rpq)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Internal sort used by both 'pop' and 'peek'
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rpq_sort <- function(rpq) {

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # if 'first' points to any other item but zero, then it means that
  # a few items have already been extracted off the queue, so do some
  # truncation
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (rpq$first > 0L) {
    rpq$priority <- rpq$priority[-seq.int(rpq$first)]
    rpq$obj      <- rpq$obj     [-seq.int(rpq$first)]
    rpq$first    <- 0L
  }

  idx          <- order(rpq$priority, decreasing = TRUE)
  rpq$priority <- rpq$priority[idx]
  rpq$obj      <- rpq$obj[idx]
  rpq$sorted   <- TRUE
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname rpq_create
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rpq_pop <- function(rpq) {
  stopifnot(inherits(rpq, 'rpq'))
  if (rpq_is_empty(rpq)) {
    stop("Cannot pop from empty queue (rpq)")
  }

  if (!rpq$sorted) {
    rpq_sort(rpq)
  }

  # If the obj list is sorted, then 'first' points to the object to be
  # returned at the next pop.
  # Doing this means that the 'obj' and 'priority' vectors don't need to
  # be resized after every 'pop', but can be done all at once next time
  # an object is inserted
  rpq$first <- rpq$first + 1L

  rpq$obj[[rpq$first]]
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname rpq_create
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rpq_peek <- function(rpq) {
  stopifnot(inherits(rpq, 'rpq'))
  if (rpq_is_empty(rpq)) {
    stop("Cannot peek into empty queue (rpq)")
  }

  if (!rpq$sorted) {
    rpq_sort(rpq)
  }

  rpq$obj[[rpq$first + 1L]]
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname rpq_create
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rpq_is_empty <- function(rpq) {
  stopifnot(inherits(rpq, 'rpq'))
  rpq_length(rpq) == 0L
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname rpq_create
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rpq_length <- function(rpq) {
  stopifnot(inherits(rpq, 'rpq'))
  length(rpq$obj) - rpq$first
}

