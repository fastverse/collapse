# Benchmark: fmean multiple-accumulator SIMD optimization
#
# Compares the optimized fmean (FMEAN_N_ACC=4 independent accumulators) against
# the baseline (single accumulator + #pragma omp simd).
#
# The package must be built from source to pick up the fmean.c changes:
#   install.packages("collapse", type = "source")
# or from the branch:
#   devtools::install(upgrade = "never")
#
# Usage:
#   Rscript misc/benchmark_fmean.R [nthreads]

suppressPackageStartupMessages({
  library(collapse)
  library(bench)
})

args <- commandArgs(trailingOnly = TRUE)
nthreads <- if(length(args)) as.integer(args[1L]) else 1L

cat(sprintf("fmean benchmark  |  collapse %s  |  nthreads=%d\n\n",
            packageVersion("collapse"), nthreads))

set_collapse(nthreads = nthreads)

run_bench <- function(n, na_frac = 0.05, iters = 500L) {
  set.seed(42L)
  x_dbl  <- rnorm(n)
  x_int  <- sample(-1000L:1000L, n, replace = TRUE)
  w      <- abs(rnorm(n)) + 0.1

  # Inject NAs
  x_dbl_na <- x_dbl; x_dbl_na[sample(n, floor(n * na_frac))] <- NA
  x_int_na <- x_int; x_int_na[sample(n, floor(n * na_frac))] <- NA_integer_

  bm <- bench::mark(
    "fmean(dbl, na.rm=TRUE)"  = fmean(x_dbl_na, na.rm = TRUE),
    "fmean(dbl, na.rm=FALSE)" = fmean(x_dbl,    na.rm = FALSE),
    "fmean(int, na.rm=TRUE)"  = fmean(x_int_na, na.rm = TRUE),
    "fmean(dbl, weighted)"    = fmean(x_dbl_na, w = w, na.rm = TRUE),
    "base::mean(dbl)"         = base::mean(x_dbl_na, na.rm = TRUE),
    check    = FALSE,
    iterations = iters,
    time_unit  = "us"
  )

  bm$n <- n
  bm[, c("n", "expression", "min", "median", "itr/sec", "n_itr", "total_time")]
}

sizes <- as.integer(c(1e4, 1e5, 5e5, 1e6))

results <- lapply(sizes, run_bench)

for(i in seq_along(results)) {
  cat(sprintf("\n--- n = %s ---\n", format(sizes[i], big.mark = ",")))
  print(results[[i]][, -1L], n = Inf)
}
