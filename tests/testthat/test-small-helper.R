context("small helpers")


test_that("missing_cases works", {
  x <- c(1, NA, 3, NaN)
  expect_equal(missing_cases(x), c(FALSE, TRUE, FALSE, TRUE))
  expect_true(any(missing_cases(data.frame(a = x, b = c(1, 2, NA, 4))[3:4, , drop = FALSE])))
  expect_equal(missing_cases(x, count = TRUE), c(0L, 1L, 0L, 1L))
})

test_that("na_omit removes missing rows", {
  d <- data.frame(a = c(1, NA, 3), b = c(1, 2, NA))
  expect_equal(nrow(na_omit(d)), 1L)
  expect_equal(na_omit(c(1, NA, 3)), c(1, 3))
})

test_that("massign and %=% assign multiple names", {
  env <- new.env()
  massign(c("x", "y"), list(3, 4), envir = env)
  expect_equal(env$x, 3)
  expect_equal(env$y, 4)
  local({
    c("a", "b") %=% list(1, 2)
    expect_equal(a, 1)
    expect_equal(b, 2)
  })
})

test_that("copyv and setv modify data", {
  d <- data.frame(a = 1:3, b = 4:6)
  dc <- copyv(d, 0, 0)
  expect_equal(dc, d)
  setv(d, 2L, 0L)
  expect_equal(d$a[2], 0L)
})

test_that("na_omit() and missing_cases() work on single-row matrix with cols", {
  X <- matrix(c(1, 2, 3), nrow = 1, dimnames = list("a", c("A", "B", "C")))
  expect_identical(na_omit(X, cols = c("A", "B")), X)
  X[1, "B"] <- NA
  expect_identical(missing_cases(X, cols = c("A", "B")), TRUE)
  expect_equal(dim(na_omit(X, cols = c("A", "B"))), c(0L, 3L))
})
