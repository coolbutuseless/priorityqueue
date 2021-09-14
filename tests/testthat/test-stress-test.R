

test_that("stress test with integer priorities", {

  set.seed(1)

  N   <- 150
  pri <- sample(N)
  val <- runif(N)

  rpq  <- rpq_create()
  cpq <- cpq_create()

  expect_true( rpq_is_empty(rpq))
  expect_true(cpq_is_empty(cpq))

  expect_equal( rpq_length( rpq), 0)
  expect_equal(cpq_length(cpq), 0)


  for (i in 1:N) {
    rpq_insert ( rpq, val[i], pri[i])
    cpq_insert(cpq, val[i], pri[i])
  }

  expect_false( rpq_is_empty( rpq))
  expect_false(cpq_is_empty(cpq))

  expect_equal( rpq_length( rpq), N)
  expect_equal(cpq_length(cpq), N)

  idx <- order(pri, decreasing = TRUE)
  ref <- val[idx]


  for (i in 1:N) {
    expect_equal(ref[i], rpq_pop(rpq))
  }

  for (i in 1:N) {
    expect_equal(ref[i], cpq_pop(cpq))
  }

})





test_that("stress test with double() priorities", {

  set.seed(1)

  N   <- 1e2
  pri <- runif(N)
  val <- runif(N)

  cpq <- cpq_create()
  rpq <- rpq_create()

  for (i in 1:N) {
    cpq_insert(cpq, val[i], pri[i])
    rpq_insert(rpq, val[i], pri[i])
  }

  idx <- order(pri, decreasing = TRUE)
  ref <- val[idx]

  for (i in 1:N) {
    expect_equal(ref[i], cpq_pop(cpq))
  }

  for (i in 1:N) {
    expect_equal(ref[i], rpq_pop(rpq))
  }


})
