# Fast (Weighted) Linear Model Fitting

`flm` is a fast linear model command that (by default) only returns a
coefficient matrix. 6 different efficient fitting methods are
implemented: 4 using base R linear algebra, and 2 utilizing the
*RcppArmadillo* and *RcppEigen* packages. The function itself only has
an overhead of 5-10 microseconds, and is thus well suited as a bootstrap
workhorse.

## Usage

``` r
flm(...)  # Internal method dispatch: default if is.atomic(..1)

# Default S3 method
flm(y, X, w = NULL, add.icpt = FALSE, return.raw = FALSE,
    method = c("lm", "solve", "qr", "arma", "chol", "eigen"),
    eigen.method = 3L, ...)

# S3 method for class 'formula'
flm(formula, data = NULL, weights = NULL, add.icpt = TRUE, ...)
```

## Arguments

- y:

  a response vector or matrix. Multiple dependent variables are only
  supported by methods "lm", "solve", "qr" and "chol".

- X:

  a matrix of regressors.

- w:

  a weight vector.

- add.icpt:

  logical. `TRUE` adds an intercept column named '(Intercept)' to `X`.

- formula:

  a [`lm`](https://rdrr.io/r/stats/lm.html) formula, without factors,
  interaction terms or other operators (`:`, `*`, `^`, `-`, etc.), may
  include regular transformations e.g. `log(var)`, `cbind(y1, y2)`,
  `magrittr::multiply_by(var1, var2)`,
  `magrittr::raise_to_power(var, 2)`.

- data:

  a named list or data frame.

- weights:

  a weights vector or expression that results in a vector when evaluated
  in the `data` environment.

&nbsp;

- return.raw:

  logical. `TRUE` returns the original output from the different
  methods. For 'lm', 'arma' and 'eigen', this includes additional
  statistics such as residuals, fitted values or standard errors. The
  other methods just return coefficients but in different formats.

- method:

  an integer or character string specifying the method of computation:

  |  |  |  |  |  |
  |----|----|----|----|----|
  | *Int.* |  | *String* |  | *Description* |
  | 1 |  | "lm" |  | uses [`.lm.fit`](https://rdrr.io/r/stats/lmfit.html). |
  | 2 |  | "solve" |  | `solve(crossprod(X), crossprod(X, y))`. |
  | 3 |  | "qr" |  | `qr.coef(qr(X), y)`. |
  | 4 |  | "arma" |  | uses [`RcppArmadillo::fastLmPure`](https://rdrr.io/pkg/RcppArmadillo/man/fastLm.html). |
  | 5 |  | "chol" |  | `chol2inv(chol(crossprod(X))) %*% crossprod(X, y)` (quite fast, requires `crossprod(X)` to be positive definite i.e. problematic if multicollinearity). |
  | 6 |  | "eigen" |  | uses [`RcppEigen::fastLmPure`](https://rdrr.io/pkg/RcppEigen/man/fastLm.html) (very fast but, depending on the method, also unstable if multicollinearity). |

- eigen.method:

  integer. Select the method of computation used by
  [`RcppEigen::fastLmPure`](https://rdrr.io/pkg/RcppEigen/man/fastLm.html):

  |  |  |  |
  |----|----|----|
  | *Int.* |  | *Description* |
  | 0 |  | column-pivoted QR decomposition. |
  | 1 |  | unpivoted QR decomposition. |
  | 2 |  | LLT Cholesky. |
  | 3 |  | LDLT Cholesky. |
  | 4 |  | Jacobi singular value decomposition (SVD). |
  | 5 |  | method based on the eigenvalue-eigenvector decomposition of X'X. |

  See
  [`vignette("RcppEigen-Introduction", package = "RcppEigen")`](https://cran.rstudio.com/web/packages/RcppEigen/vignettes/RcppEigen-Introduction.pdf)
  for details on these methods and benchmark results. Run
  `source(system.file("examples", "lmBenchmark.R", package = "RcppEigen"))`
  to re-run the benchmark on your machine.

- ...:

  further arguments passed to other methods. For the formula method
  further arguments passed to the default method. Additional arguments
  can also be passed to the default method e.g. `tol = value` to set a
  numerical tolerance for the solution - applicable with methods "lm",
  "solve" and "qr" (default is `1e-7`), or `LAPACK = TRUE` with method
  "qr" to use LAPACK routines to for the qr decomposition (typically
  faster than the LINPACK default).

## Value

If `return.raw = FALSE`, a matrix of coefficients with the rows
corresponding to the columns of `X`, otherwise the raw results from the
various methods are returned.

## Note

Method "qr" supports sparse matrices, so for an `X` matrix with many
dummy variables consider method "qr" passing `as(X, "dgCMatrix")`
instead of just `X`.

## See also

[`fhdwithin/HDW`](https://fastverse.org/collapse/reference/fhdbetween_fhdwithin.md),
[`fFtest`](https://fastverse.org/collapse/reference/fFtest.md), [Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
# Simple usage
coef <- flm(mpg ~ hp + carb, mtcars, w = wt)

# Same thing in programming usage
flm(mtcars$mpg, qM(mtcars[c("hp","carb")]), mtcars$wt, add.icpt = TRUE)
#>                    [,1]
#> (Intercept) 28.48401839
#> hp          -0.06834996
#> carb         0.33207257

# Check this is correct
lmcoef <- coef(lm(mpg ~ hp + carb, weights = wt, mtcars))
all.equal(drop(coef), lmcoef)
#> [1] TRUE

# Multi-dependent variable (only some methods)
flm(cbind(mpg, qsec) ~ hp + carb, mtcars, w = wt)
#>                     mpg        qsec
#> (Intercept) 28.48401839 20.77946948
#> hp          -0.06834996 -0.01409167
#> carb         0.33207257 -0.25468102

# Returning raw results from solver: different for different methods
flm(mpg ~ hp + carb, mtcars, return.raw = TRUE)
#> $qr
#>       (Intercept)            hp          carb
#>  [1,]  -5.6568542 -829.78980772 -15.909902577
#>  [2,]   0.1767767  381.74189579   6.743103202
#>  [3,]   0.1767767    0.12620114   5.950257070
#>  [4,]   0.1767767    0.08166844   0.261830398
#>  [5,]   0.1767767   -0.08860368   0.245465211
#>  [6,]   0.1767767    0.09476629   0.250161569
#>  [7,]   0.1767767   -0.27197365   0.072708888
#>  [8,]   0.1767767    0.20740784  -0.018250328
#>  [9,]   0.1767767    0.12096200   0.058763944
#> [10,]   0.1767767    0.04761401  -0.212010544
#> [11,]   0.1767767    0.04761401  -0.212010544
#> [12,]   0.1767767   -0.10170154   0.089074074
#> [13,]   0.1767767   -0.10170154   0.089074074
#> [14,]   0.1767767   -0.10170154   0.089074074
#> [15,]   0.1767767   -0.16719081  -0.020641746
#> [16,]   0.1767767   -0.19338652   0.002695913
#> [17,]   0.1767767   -0.23268009   0.037702400
#> [18,]   0.1767767    0.19692956   0.159144701
#> [19,]   0.1767767    0.23360355  -0.041587987
#> [20,]   0.1767767    0.19954913   0.156810935
#> [21,]   0.1767767    0.11572286   0.231491442
#> [22,]   0.1767767   -0.02311440   0.187121065
#> [23,]   0.1767767   -0.02311440   0.187121065
#>  [ reached 'max' / getOption("max.print") -- omitted 9 rows ]
#> 
#> $coefficients
#> [1] 30.04025415 -0.07290396  0.26470042
#> 
#> $residuals
#>  [1] -2.07962064 -2.07962064 -0.72488664 -0.88551939  0.88853735 -4.55003917
#>  [7]  1.06241345 -1.64960970 -0.84377915 -2.93186921 -4.33186921 -1.31164329
#> [13] -0.41164329 -2.51164329 -5.75374480 -5.02470524  0.36885411  6.90670654
#> [19]  3.62135074  8.33380258 -1.73327082 -4.13406156 -4.43406156  0.06241345
#> [25]  1.38853735  1.80670654  2.06460503  8.06849206  3.94758862  0.82973568
#> [31]  7.26496784 -1.22312376
#> 
#> $effects
#>  [1] -113.64973741  -26.04559222    1.57503553   -0.39549102    1.09761463
#>  [6]   -4.04058135    0.94274199   -1.00142681   -0.32383064   -2.57746205
#> [11]   -3.97746205   -1.15036366   -0.25036366   -2.35036366   -5.71798064
#> [16]   -5.02779999    0.30747100    7.56771408    4.30839253    8.99869601
#> [21]   -1.19272588   -3.82783701   -4.12783701   -0.05725801    1.59761463
#> [26]    2.46771408    2.60009710    8.51849455    3.75408524    0.92534013
#> [31]    6.68209341   -0.75757771
#> 
#> $rank
#> [1] 3
#> 
#> $pivot
#> [1] 1 2 3
#> 
#> $qraux
#> [1] 1.176777 1.081668 1.222156
#> 
#> $tol
#> [1] 1e-07
#> 
#> $pivoted
#> [1] FALSE
#> 
flm(mpg ~ hp + carb, mtcars, method = "qr", return.raw = TRUE)
#> (Intercept)          hp        carb 
#> 30.04025415 -0.07290396  0.26470042 
 
# Test that all methods give the same result
all_obj_equal(lapply(1:6, function(i)
  flm(mpg ~ hp + carb, mtcars, w = wt, method = i)))
#> Registered S3 methods overwritten by 'RcppEigen':
#>   method               from         
#>   predict.fastLm       RcppArmadillo
#>   print.fastLm         RcppArmadillo
#>   summary.fastLm       RcppArmadillo
#>   print.summary.fastLm RcppArmadillo
#> [1] TRUE
```
