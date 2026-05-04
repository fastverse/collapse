################################################################################
# Comprehensive benchmark: fsum multiple-accumulator optimization (#824)
#
# Compares current collapse fsum (baseline with #pragma omp simd) against
# the optimized version using FSUM_N_ACC=4 independent accumulators.
#
# Usage:
#   Rscript misc/benchmark_fsum.R            # uses all available threads
#   Rscript misc/benchmark_fsum.R 4          # uses at most 4 threads
#
# Requires: bench, collapse (installed from source with OpenMP)
################################################################################

library(collapse)
library(bench)

max_threads <- as.integer(commandArgs(trailingOnly = TRUE)[1])
if (is.na(max_threads)) max_threads <- get_collapse()$nthreads
cat(sprintf("collapse version: %s\n", packageVersion("collapse")))
cat(sprintf("OpenMP threads available: %d\n", get_collapse()$nthreads))
cat(sprintf("Benchmarking with up to %d threads\n\n", max_threads))

set.seed(42L)

sizes  <- as.integer(2L ^ c(10, 13, 16, 19, 22))   # 1K – 4M elements
n_iter <- 500L

fmt <- function(x) formatC(x, format = "f", digits = 3)

results <- list()

#===============================================================================
# 1. Double vector, ungrouped
#===============================================================================
cat("=== 1. Double vector, ungrouped ===\n\n")

for (n in sizes) {
  x_dbl     <- rnorm(n)
  x_dbl_na  <- replace(x_dbl, sample.int(n, max(1L, n %/% 10L)), NA_real_)

  cat(sprintf("n = %s\n", format(n, big.mark = ",")))

  bm <- bench::mark(
    "base::sum"              = sum(x_dbl),
    "fsum na.rm=FALSE t=1"   = fsum(x_dbl,    na.rm = FALSE, nthreads = 1L),
    "fsum na.rm=TRUE  t=1"   = fsum(x_dbl_na, na.rm = TRUE,  nthreads = 1L),
    "fsum na.rm=FALSE t=max" = fsum(x_dbl,    na.rm = FALSE, nthreads = max_threads),
    "fsum na.rm=TRUE  t=max" = fsum(x_dbl_na, na.rm = TRUE,  nthreads = max_threads),
    iterations = n_iter,
    check = FALSE
  )
  print(bm[, c("expression", "min", "median", "itr/sec", "mem_alloc")])
  cat("\n")
  results[[paste0("dbl_ungrouped_n", n)]] <- bm
}

#===============================================================================
# 2. Integer vector, ungrouped
#===============================================================================
cat("=== 2. Integer vector, ungrouped ===\n\n")

for (n in sizes) {
  x_int    <- sample(-1000L:1000L, n, replace = TRUE)
  x_int_na <- replace(x_int, sample.int(n, max(1L, n %/% 10L)), NA_integer_)

  cat(sprintf("n = %s\n", format(n, big.mark = ",")))

  bm <- bench::mark(
    "base::sum"              = sum(x_int),
    "fsum na.rm=FALSE t=1"   = fsum(x_int,    na.rm = FALSE, nthreads = 1L),
    "fsum na.rm=TRUE  t=1"   = fsum(x_int_na, na.rm = TRUE,  nthreads = 1L),
    "fsum na.rm=FALSE t=max" = fsum(x_int,    na.rm = FALSE, nthreads = max_threads),
    "fsum na.rm=TRUE  t=max" = fsum(x_int_na, na.rm = TRUE,  nthreads = max_threads),
    iterations = n_iter,
    check = FALSE
  )
  print(bm[, c("expression", "min", "median", "itr/sec", "mem_alloc")])
  cat("\n")
  results[[paste0("int_ungrouped_n", n)]] <- bm
}

#===============================================================================
# 3. Double vector, weighted, ungrouped
#===============================================================================
cat("=== 3. Double vector, weighted, ungrouped ===\n\n")

for (n in sizes) {
  x_dbl <- rnorm(n)
  w_dbl <- runif(n) + 0.1
  x_na  <- replace(x_dbl, sample.int(n, max(1L, n %/% 10L)), NA_real_)

  cat(sprintf("n = %s\n", format(n, big.mark = ",")))

  bm <- bench::mark(
    "fsum w na.rm=FALSE t=1"   = fsum(x_dbl, w = w_dbl, na.rm = FALSE, nthreads = 1L),
    "fsum w na.rm=TRUE  t=1"   = fsum(x_na,  w = w_dbl, na.rm = TRUE,  nthreads = 1L),
    "fsum w na.rm=FALSE t=max" = fsum(x_dbl, w = w_dbl, na.rm = FALSE, nthreads = max_threads),
    "fsum w na.rm=TRUE  t=max" = fsum(x_na,  w = w_dbl, na.rm = TRUE,  nthreads = max_threads),
    iterations = n_iter,
    check = FALSE
  )
  print(bm[, c("expression", "min", "median", "itr/sec", "mem_alloc")])
  cat("\n")
  results[[paste0("dbl_weighted_n", n)]] <- bm
}

#===============================================================================
# 4. Double vector, grouped (ng = 100 and ng = 1000)
#===============================================================================
cat("=== 4. Double vector, grouped ===\n\n")

n <- 1e6L
x_dbl <- rnorm(n)

for (ng in c(100L, 1000L, 10000L)) {
  g <- GRP(rep_len(seq_len(ng), n))

  cat(sprintf("n = %s, ng = %s\n", format(n, big.mark = ","), format(ng, big.mark = ",")))

  bm <- bench::mark(
    "fsum grp na.rm=FALSE t=1"   = fsum(x_dbl, g = g, na.rm = FALSE, nthreads = 1L),
    "fsum grp na.rm=TRUE  t=1"   = fsum(x_dbl, g = g, na.rm = TRUE,  nthreads = 1L),
    "fsum grp na.rm=FALSE t=max" = fsum(x_dbl, g = g, na.rm = FALSE, nthreads = max_threads),
    "fsum grp na.rm=TRUE  t=max" = fsum(x_dbl, g = g, na.rm = TRUE,  nthreads = max_threads),
    iterations = n_iter,
    check = FALSE
  )
  print(bm[, c("expression", "min", "median", "itr/sec", "mem_alloc")])
  cat("\n")
  results[[paste0("dbl_grouped_ng", ng)]] <- bm
}

#===============================================================================
# 5. Thread scaling: double, n=1M, na.rm=FALSE
#===============================================================================
cat("=== 5. Thread scaling (double, n=1M, na.rm=FALSE) ===\n\n")

n     <- 1e6L
x_dbl <- rnorm(n)

thread_counts <- c(1L, 2L, 4L, 8L)
thread_counts <- thread_counts[thread_counts <= max_threads]

for (nt in thread_counts) {
  cat(sprintf("threads = %d\n", nt))
  bm <- bench::mark(
    fsum = fsum(x_dbl, na.rm = FALSE, nthreads = nt),
    iterations = n_iter,
    check = FALSE
  )
  print(bm[, c("expression", "min", "median", "itr/sec")])
}
cat("\n")

#===============================================================================
# 6. Correctness spot-check
#===============================================================================
cat("=== 6. Correctness spot-check ===\n\n")

set.seed(1L)
n <- 1e5L
x_d  <- rnorm(n)
x_dn <- replace(x_d, sample.int(n, 1000L), NA_real_)
x_i  <- sample(-1000L:1000L, n, replace = TRUE)
x_in <- replace(x_i, sample.int(n, 1000L), NA_integer_)
w    <- runif(n) + 0.1

tol <- 1e-9

stopifnot(
  abs(fsum(x_d,  na.rm = FALSE) - sum(x_d))           < tol,
  abs(fsum(x_dn, na.rm = TRUE)  - sum(x_dn, na.rm=TRUE)) < tol,
  fsum(x_i,  na.rm = FALSE) == sum(x_i),
  fsum(x_in, na.rm = TRUE)  == sum(x_in, na.rm = TRUE),
  abs(fsum(x_d, w = w, na.rm = FALSE) - sum(x_d * w)) < tol
)

cat("All correctness checks passed.\n\n")

cat("Benchmark complete.\n")
