# Fast (Weighted) F-test for Linear Models (with Factors)

`fFtest` computes an R-squared based F-test for the exclusion of the
variables in `exc`, where the full (unrestricted) model is defined by
variables supplied to both `exc` and `X`. The test is efficient and
designed for cases where both `exc` and `X` may contain multiple factors
and continuous variables. There is also an efficient 2-part formula
method.

## Usage

``` r
fFtest(...) # Internal method dispatch: formula if is.call(..1) || is.call(..2)

# Default S3 method
fFtest(y, exc, X = NULL, w = NULL, full.df = TRUE, ...)

# S3 method for class 'formula'
fFtest(formula, data = NULL, weights = NULL, ...)
```

## Arguments

- y:

  a numeric vector: the dependent variable.

- exc:

  a numeric vector, factor, numeric matrix or list / data frame of
  numeric vectors and/or factors: variables to test / exclude.

- X:

  a numeric vector, factor, numeric matrix or list / data frame of
  numeric vectors and/or factors: covariates to include in both the
  restricted (without `exc`) and unrestricted model. If left empty
  (`X = NULL`), the test amounts to the F-test of the regression of `y`
  on `exc`.

- w:

  numeric. A vector of (frequency) weights.

- formula:

  a 2-part formula: `y ~ exc | X`, where both `exc` and `X` are
  expressions connected with `+`, and `X` can be omitted. *Note* that
  other operators (`:`, `*`, `^`, `-`, etc.) are not supported, you can
  interact variables using standard functions like
  [`finteraction/itn`](https://fastverse.org/collapse/reference/qF.md)
  or
  [`magrittr::multiply_by`](https://magrittr.tidyverse.org/reference/aliases.html)
  inside the formula e.g. `log(y) ~ x1 + itn(x2, x3) | x4` or
  `log(y) ~ x1 + multiply_by(x2, x3) | x4`.

- data:

  a named list or data frame.

- weights:

  a weights vector or expression that results in a vector when evaluated
  in the `data` environment.

- full.df:

  logical. If `TRUE` (default), the degrees of freedom are calculated as
  if both restricted and unrestricted models were estimated using
  [`lm()`](https://rdrr.io/r/stats/lm.html) (i.e. as if factors were
  expanded to matrices of dummies). `FALSE` only uses one degree of
  freedom per factor.

- ...:

  other arguments passed to `fFtest.default` or to `fhdwithin`. Sensible
  options might be the `lm.method` argument or further control
  parameters to
  [`fixest::demean`](https://lrberge.github.io/fixest/reference/demean.html),
  the workhorse function underlying `fhdwithin` for higher-order
  centering tasks.

## Details

Factors and continuous regressors are efficiently projected out using
[`fhdwithin`](https://fastverse.org/collapse/reference/fhdbetween_fhdwithin.md),
and the option `full.df` regulates whether a degree of freedom is
subtracted for each used factor level (equivalent to dummy-variable
estimator / expanding factors), or only one degree of freedom per factor
(treating factors as variables). The test automatically removes missing
values and considers only the complete cases of `y, exc` and `X`. Unused
factor levels in `exc` and `X` are dropped.

*Note* that an intercept is always added by
[`fhdwithin`](https://fastverse.org/collapse/reference/fhdbetween_fhdwithin.md),
so it is not necessary to include an intercept in data supplied to `exc`
/ `X`.

## Value

A 5 x 3 numeric matrix of statistics. The columns contain statistics:

1.  the R-squared of the model

2.  the numerator degrees of freedom i.e. the number of variables (k)
    and used factor levels if `full.df = TRUE`

3.  the denominator degrees of freedom: N - k - 1.

4.  the F-statistic

5.  the corresponding P-value

The rows show these statistics for:

1.  the Full (unrestricted) Model (`y ~ exc + X`)

2.  the Restricted Model (`y ~ X`)

3.  the Exclusion Restriction of `exc`. The R-squared shown is simply
    the difference of the full and restricted R-Squared's, not the
    R-Squared of the model `y ~ exc`.

If `X = NULL`, only a vector of the same 5 statistics testing the model
(`y ~ exc`) is shown.

## See also

[`flm`](https://fastverse.org/collapse/reference/flm.md),
[`fhdwithin`](https://fastverse.org/collapse/reference/fhdbetween_fhdwithin.md),
[Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## We could use fFtest as a simple seasonality test:
fFtest(AirPassengers, qF(cycle(AirPassengers)))         # Testing for level-seasonality
#>   R-Sq.     DF1     DF2 F-Stat. P-value 
#>   0.106      11     132   1.424   0.169 
fFtest(AirPassengers, qF(cycle(AirPassengers)),         # Seasonality test around a cubic trend
        poly(seq_along(AirPassengers), 3))
#>                    R-Sq. DF1 DF2 F-Stat. P-Value
#> Full Model         0.965  14 129 250.585   0.000
#> Restricted Model   0.862   3 140 291.593   0.000
#> Exclusion Rest.    0.102  11 129  33.890   0.000
fFtest(fdiff(AirPassengers), qF(cycle(AirPassengers)))  # Seasonality in first-difference
#>   R-Sq.     DF1     DF2 F-Stat. P-value 
#>   0.749      11     131  35.487   0.000 

## A more classical example with only continuous variables
fFtest(mpg ~ cyl + vs | hp + carb, mtcars)
#>                   R-Sq. DF1 DF2 F-Stat. P-Value
#> Full Model        0.750   4  27  20.261   0.000
#> Restricted Model  0.605   2  29  22.175   0.000
#> Exclusion Rest.   0.145   2  27   7.858   0.002
fFtest(mtcars$mpg, mtcars[c("cyl","vs")], mtcars[c("hp","carb")])
#>                   R-Sq. DF1 DF2 F-Stat. P-Value
#> Full Model        0.750   4  27  20.261   0.000
#> Restricted Model  0.605   2  29  22.175   0.000
#> Exclusion Rest.   0.145   2  27   7.858   0.002
 
## Now encoding cyl and vs as factors
fFtest(mpg ~ qF(cyl) + qF(vs) | hp + carb, mtcars)
#>                   R-Sq. DF1 DF2 F-Stat. P-Value
#> Full Model        0.756   5  26  16.140   0.000
#> Restricted Model  0.605   2  29  22.175   0.000
#> Exclusion Rest.   0.152   3  26   5.395   0.005
fFtest(mtcars$mpg, lapply(mtcars[c("cyl","vs")], qF), mtcars[c("hp","carb")])
#>                   R-Sq. DF1 DF2 F-Stat. P-Value
#> Full Model        0.756   5  26  16.140   0.000
#> Restricted Model  0.605   2  29  22.175   0.000
#> Exclusion Rest.   0.152   3  26   5.395   0.005

## Using iris data: A factor and a continuous variable excluded
fFtest(Sepal.Length ~ Petal.Width + Species | Sepal.Width + Petal.Length, iris)
#>                    R-Sq. DF1 DF2 F-Stat. P-Value
#> Full Model         0.867   5 144 188.251   0.000
#> Restricted Model   0.840   2 147 386.386   0.000
#> Exclusion Rest.    0.027   3 144   9.816   0.000
fFtest(iris$Sepal.Length, iris[4:5], iris[2:3])
#>                    R-Sq. DF1 DF2 F-Stat. P-Value
#> Full Model         0.867   5 144 188.251   0.000
#> Restricted Model   0.840   2 147 386.386   0.000
#> Exclusion Rest.    0.027   3 144   9.816   0.000

## Testing the significance of country-FE in regression of GDP on life expectancy
fFtest(log(PCGDP) ~ iso3c | LIFEEX, wlddev)
#>                      R-Sq.   DF1   DF2   F-Stat.   P-Value
#> Full Model           0.955   199  8822   943.424     0.000
#> Restricted Model     0.602     1  9020 13653.865     0.000
#> Exclusion Rest.      0.353   198  8822   350.373     0.000
fFtest(log(wlddev$PCGDP), wlddev$iso3c, wlddev$LIFEEX)
#>                      R-Sq.   DF1   DF2   F-Stat.   P-Value
#> Full Model           0.955   199  8822   943.424     0.000
#> Restricted Model     0.602     1  9020 13653.865     0.000
#> Exclusion Rest.      0.353   198  8822   350.373     0.000
 
## Ok, country-FE are significant, what about adding time-FE
fFtest(log(PCGDP) ~ qF(year) | iso3c + LIFEEX, wlddev)
#>                     R-Sq.  DF1  DF2  F-Stat.  P-Value
#> Full Model          0.963  258 8763  876.312    0.000
#> Restricted Model    0.955  199 8822  943.424    0.000
#> Exclusion Rest.     0.008   59 8763   30.126    0.000
fFtest(log(wlddev$PCGDP), qF(wlddev$year), wlddev[c("iso3c","LIFEEX")])
#>                     R-Sq.  DF1  DF2  F-Stat.  P-Value
#> Full Model          0.963  258 8763  876.312    0.000
#> Restricted Model    0.955  199 8822  943.424    0.000
#> Exclusion Rest.     0.008   59 8763   30.126    0.000

# Same test done using lm:
data <- na_omit(get_vars(wlddev, c("iso3c","year","PCGDP","LIFEEX")))
full <- lm(PCGDP ~ LIFEEX + iso3c + qF(year), data)
rest <- lm(PCGDP ~ LIFEEX + iso3c, data)
anova(rest, full)
#> Analysis of Variance Table
#> 
#> Model 1: PCGDP ~ LIFEEX + iso3c
#> Model 2: PCGDP ~ LIFEEX + iso3c + qF(year)
#>   Res.Df        RSS Df  Sum of Sq     F    Pr(>F)    
#> 1   8822 3.0044e+11                                  
#> 2   8763 2.5097e+11 59 4.9475e+10 29.28 < 2.2e-16 ***
#> ---
#> Signif. codes:  0 ‘***’ 0.001 ‘**’ 0.01 ‘*’ 0.05 ‘.’ 0.1 ‘ ’ 1
```
