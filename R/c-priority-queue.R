

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' My Priority Queue
#'
#' @param cpq my priority queue object
#' @param obj any R object
#' @param priority numeric priority (must be finite and not NA or NaN)
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cpq_create <- function() {
  .Call(cpq_create_)
}


#' @rdname cpq_create
#' @export
cpq_length <- function(cpq) {
  .Call(cpq_length_, cpq)
}


#' @rdname cpq_create
#' @export
cpq_is_empty <- function(cpq) {
  .Call(cpq_length_, cpq) == 0L
}


#' @rdname cpq_create
#' @export
cpq_insert <- function(cpq, obj, priority) {
  .Call(cpq_insert_, cpq, obj, priority)
  invisible(cpq)
}


#' @rdname cpq_create
#' @export
cpq_pop <- function(cpq) {
  .Call(cpq_pop_, cpq)
}


#' @rdname cpq_create
#' @export
cpq_peek <- function(cpq) {
  .Call(cpq_peek_, cpq)
}


#' @rdname cpq_create
#' @export
cpq_length <- function(cpq) {
  .Call(cpq_length_, cpq)
}


#' @rdname cpq_create
#' @export
cpq_dump <- function(cpq) {
  .Call(cpq_dump_, cpq)
}

