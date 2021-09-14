


test_that("rpq baseline works", {

  rpq <- rpq_create()
  expect_true(rpq_is_empty(rpq))
  rpq_insert(rpq, 5.5,  5)
  rpq_insert(rpq, 6.5,  6)
  rpq_insert(rpq, 7.5,  7)
  rpq_insert(rpq, 8.5,  8)
  rpq_insert(rpq, 9.5,  9)
  rpq_insert(rpq, "hello", 10)
  rpq_insert(rpq, 1.5,  1)
  rpq_insert(rpq, 2.5,  2)
  rpq_insert(rpq, 3.5,  3)
  rpq_insert(rpq, 4.5,  4)
  expect_false(rpq_is_empty(rpq))

  expect_equal(rpq_peek(rpq), "hello")

  expect_equal(rpq_pop(rpq), "hello")
  expect_equal(rpq_pop(rpq), 9.5)
  expect_equal(rpq_pop(rpq), 8.5)
  expect_equal(rpq_pop(rpq), 7.5)
  expect_equal(rpq_pop(rpq), 6.5)
  expect_equal(rpq_pop(rpq), 5.5)
  expect_equal(rpq_pop(rpq), 4.5)
  expect_equal(rpq_pop(rpq), 3.5)
  expect_equal(rpq_pop(rpq), 2.5)
  expect_equal(rpq_pop(rpq), 1.5)
  expect_error(rpq_pop(rpq))
  expect_true(rpq_is_empty(rpq))

  expect_error(rpq_peek(rpq), "empty queue")


  rpq <- rpq_create()
  expect_true(rpq_is_empty(rpq))
  rpq_insert(rpq, 5.5,  5)
  expect_equal(rpq_pop(rpq), 5.5)
  rpq_insert(rpq, 6.5,  6)
  expect_equal(rpq_pop(rpq), 6.5)
  expect_true(rpq_is_empty(rpq))
})






test_that("cpq baseline works", {

  cpq <- cpq_create()
  expect_true(cpq_is_empty(cpq))
  cpq_insert(cpq, 5.5,  5)
  cpq_insert(cpq, 6.5,  6)
  cpq_insert(cpq, 7.5,  7)
  cpq_insert(cpq, 8.5,  8)
  cpq_insert(cpq, 9.5,  9)
  cpq_insert(cpq, "hello", 10)
  cpq_insert(cpq, 1.5,  1)
  cpq_insert(cpq, 2.5,  2)
  cpq_insert(cpq, 3.5,  3)
  cpq_insert(cpq, 4.5,  4)
  expect_false(cpq_is_empty(cpq))

  expect_equal(cpq_peek(cpq), "hello")

  expect_equal(cpq_pop(cpq), "hello")
  expect_equal(cpq_pop(cpq), 9.5)
  expect_equal(cpq_pop(cpq), 8.5)
  expect_equal(cpq_pop(cpq), 7.5)
  expect_equal(cpq_pop(cpq), 6.5)
  expect_equal(cpq_pop(cpq), 5.5)
  expect_equal(cpq_pop(cpq), 4.5)
  expect_equal(cpq_pop(cpq), 3.5)
  expect_equal(cpq_pop(cpq), 2.5)
  expect_equal(cpq_pop(cpq), 1.5)
  expect_error(cpq_pop(cpq))
  expect_true(cpq_is_empty(cpq))

  expect_error(cpq_peek(cpq), "empty queue")


  cpq <- cpq_create()
  expect_true(cpq_is_empty(cpq))
  cpq_insert(cpq, 5.5,  5)
  expect_equal(cpq_pop(cpq), 5.5)
  cpq_insert(cpq, 6.5,  6)
  expect_equal(cpq_pop(cpq), 6.5)
  expect_true(cpq_is_empty(cpq))
})
