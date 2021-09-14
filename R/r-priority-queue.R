
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

  rpq
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname rpq_create
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rpq_insert <- function(rpq, obj, priority) {

  rpq$obj      <- c(rpq$obj, list(obj))
  rpq$priority <- c(rpq$priority, priority)

  invisible(rpq)
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname rpq_create
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rpq_pop <- function(rpq) {
  if (length(rpq$obj) == 0L) {
    stop("Cannot pop from empty queue (rpq)")
  }

  idx <- which.max(rpq$priority)
  res <- rpq$obj[[idx]]

  rpq$obj      <- rpq$obj     [-idx]
  rpq$priority <- rpq$priority[-idx]

  res
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname rpq_create
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rpq_peek <- function(rpq) {
  if (length(rpq$obj) == 0L) {
    stop("Cannot peek into empty queue (rpq)")
  }

  rpq$obj[[which.max(rpq$priority)]]
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname rpq_create
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rpq_is_empty <- function(rpq) {
  length(rpq$obj) == 0L
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname rpq_create
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rpq_length <- function(rpq) {
  length(rpq$obj)
}

