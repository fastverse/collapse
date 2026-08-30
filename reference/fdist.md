# Fast and Flexible Distance Computations

A fast and flexible replacement for
[`dist`](https://rdrr.io/r/stats/dist.html), to compute euclidean
distances.

## Usage

``` r
fdist(x, v = NULL, ..., method = "euclidean", nthreads = .op[["nthreads"]])
```

## Arguments

- x:

  a numeric vector or matrix. Data frames/lists can be passed but will
  be converted to matrix using
  [`qM`](https://fastverse.org/collapse/reference/quick-conversion.md).
  Non-numeric (double) inputs will be coerced.

- v:

  an (optional) numeric (double) vector such that
  `length(v) == NCOL(x)`, to compute distances with (the rows of) `x`.
  Other vector types will be coerced.

- ...:

  not used. A placeholder for possible future arguments.

- method:

  an integer or character string indicating the method of computing
  distances.

  |  |  |  |  |  |
  |----|----|----|----|----|
  | *Int.* |  | *String* |  | *Description* |
  | 1 |  | `"euclidean"` |  | euclidean distance |
  | 2 |  | `"euclidean_squared"` |  | squared euclidean distance (more efficient) |

- nthreads:

  integer. The number of threads to use. If `v = NULL` (full distance
  matrix), multithreading is along the distance matrix columns
  (decreasing thread loads as matrix is lower triangular). If `v` is
  supplied, multithreading is at the sub-column level (across elements).

## Value

If `v = NULL`, a full lower-triangular distance matrix between the rows
of `x` is computed and returned as a 'dist' object (all methods apply,
see [`dist`](https://rdrr.io/r/stats/dist.html)). Otherwise, a numeric
vector of distances of each row of `x` with `v` is returned. See
Examples.

## Note

`fdist` does not check for missing values, so `NA`'s will result in `NA`
distances.

[`kit::topn`](https://fastverse.org/kit/reference/topn.html) is a
suitable complimentary function to find nearest neighbors. It is very
efficient and skips missing values by default.

## See also

[`flm`](https://fastverse.org/collapse/reference/flm.md), [Fast
Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
# Distance matrix
m = as.matrix(mtcars)
str(fdist(m)) # Same as dist(m)
#>  'dist' num [1:496] 0.615 54.909 98.113 210.337 65.472 ...
#>  - attr(*, "Size")= int 32
#>  - attr(*, "Labels")= chr [1:32] "Mazda RX4" "Mazda RX4 Wag" "Datsun 710" "Hornet 4 Drive" ...
#>  - attr(*, "Diag")= logi FALSE
#>  - attr(*, "Upper")= logi FALSE
#>  - attr(*, "method")= chr "euclidean"

# Distance with vector
d = fdist(m, fmean(m))
kit::topn(d, 5)  # Index of 5 nearest neighbours
#> [1] 15 16 17 31 19

# Mahalanobis distance
m_mahal = t(forwardsolve(t(chol(cov(m))), t(m)))
fdist(m_mahal, fmean(m_mahal))
#>  [1] 2.991099 2.878877 2.989507 2.469155 2.330035 2.979523 3.022627 3.167072
#>  [9] 4.753222 3.520384 3.325489 3.078332 2.365275 2.454885 3.346836 2.944842
#> [17] 3.501088 3.013076 3.867089 3.208810 3.665023 2.495443 2.405554 3.417825
#> [25] 2.591927 1.909395 4.284409 3.741747 4.644675 3.339588 4.380911 3.144643
sqrt(unattrib(mahalanobis(m, fmean(m), cov(m))))
#>  [1] 2.991099 2.878877 2.989507 2.469155 2.330035 2.979523 3.022627 3.167072
#>  [9] 4.753222 3.520384 3.325489 3.078332 2.365275 2.454885 3.346836 2.944842
#> [17] 3.501088 3.013076 3.867089 3.208810 3.665023 2.495443 2.405554 3.417825
#> [25] 2.591927 1.909395 4.284409 3.741747 4.644675 3.339588 4.380911 3.144643
# \donttest{
# Distance of two vectors
x <- rnorm(1e6)
y <- rnorm(1e6)
microbenchmark::microbenchmark(
  fdist(x, y),
  fdist(x, y, nthreads = 2),
  sqrt(sum((x-y)^2))
)
#> Unit: microseconds
#>                       expr      min       lq     mean   median       uq
#>                fdist(x, y)  928.650 1009.461 1124.761 1057.390 1138.590
#>  fdist(x, y, nthreads = 2)  927.953  984.123 1299.654 1039.575 1138.283
#>       sqrt(sum((x - y)^2)) 2283.659 2638.391 3500.253 2858.992 3408.494
#>        max neval
#>   4156.293   100
#>  15685.165   100
#>  20420.378   100
# }
```
