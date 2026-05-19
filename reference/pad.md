# Pad Matrix-Like Objects with a Value

The `pad` function inserts elements / rows filled with `value` into a
vector matrix or data frame `X` at positions given by `i`. It is
particularly useful to expand objects returned by statistical procedures
which remove missing values to the original data dimensions.

## Usage

``` r
pad(X, i, value = NA, method = c("auto", "xpos", "vpos"))
```

## Arguments

- X:

  a vector, matrix, data frame or list of equal-length columns.

- i:

  either an integer (positive or negative) or logical vector giving
  positions / rows of `X` into which `value`'s should be inserted, or,
  alternatively, a positive integer vector with `length(i) == NROW(X)`,
  but with some gaps in the indices into which `value`'s can be
  inserted, or a logical vector with `sum(i) == NROW(X)` such that
  `value`'s can be inserted for `FALSE` values in the logical vector.
  See also `method` and Examples.

- value:

  a scalar value to be replicated and inserted into `X` at positions /
  rows given by `i`. Default is `NA`.

- method:

  an integer or string specifying the use of `i`. The options are:

  |  |  |  |  |  |
  |----|----|----|----|----|
  | *Int.* |  | *String* |  | *Description* |
  | 1 |  | "auto" |  | automatic method selection: If `i` is positive integer and `length(i) == NROW(X)` or if `i` is logical and `sum(i) == NROW(X)`, choose method "xpos", else choose "vpos". |
  |  |  |  |  |  |
  |  |  |  |  |  |
  | 1 |  | "xpos" |  | `i` is a vector of positive integers or a logical vector giving the positions of the the elements / rows of `X`. `values`'s are inserted where there are gaps / `FALSE` values in `i`. |
  |  |  |  |  |  |
  |  |  |  |  |  |
  | 2 |  | "vpos" |  | `i` is a vector of positive / negative integers or a logical vector giving the positions at which `values`'s / rows should be inserted into `X`. |

## Value

`X` with elements / rows filled with `value` inserted at positions given
by `i`.

## See also

[`append`](https://rdrr.io/r/base/append.html), [Recode and Replace
Values](https://fastverse.org/collapse/reference/recode-replace.md),
[Small (Helper)
Functions](https://fastverse.org/collapse/reference/small-helpers.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
v <- 1:3

pad(v, 1:2)       # Automatic selection of method "vpos"
#> [1] NA NA  1  2  3
pad(v, -(1:2))    # Same thing
#> [1] NA NA  1  2  3
pad(v, c(TRUE, TRUE, FALSE, FALSE, FALSE)) # Same thing
#> [1] NA NA  1  2  3

pad(v, c(1, 3:4)) # Automatic selection of method "xpos"
#> [1]  1 NA  2  3
pad(v, c(TRUE, FALSE, TRUE, TRUE, FALSE))  # Same thing
#> [1]  1 NA  2  3 NA

head(pad(wlddev, 1:3)) # Insert 3 missing rows at the beginning of the data
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1        <NA>  <NA>       <NA>   NA     NA       <NA>       <NA>    NA    NA
#> 2        <NA>  <NA>       <NA>   NA     NA       <NA>       <NA>    NA    NA
#> 3        <NA>  <NA>       <NA>   NA     NA       <NA>       <NA>    NA    NA
#> 4 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 5 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#>   LIFEEX GINI       ODA     POP
#> 1     NA   NA        NA      NA
#> 2     NA   NA        NA      NA
#> 3     NA   NA        NA      NA
#> 4 32.446   NA 116769997 8996973
#> 5 32.962   NA 232080002 9169410
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
head(pad(wlddev, 2:4)) # ... at rows positions 2-4
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 2        <NA>  <NA>       <NA>   NA     NA       <NA>       <NA>    NA    NA
#> 3        <NA>  <NA>       <NA>   NA     NA       <NA>       <NA>    NA    NA
#> 4        <NA>  <NA>       <NA>   NA     NA       <NA>       <NA>    NA    NA
#> 5 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#>   LIFEEX GINI       ODA     POP
#> 1 32.446   NA 116769997 8996973
#> 2     NA   NA        NA      NA
#> 3     NA   NA        NA      NA
#> 4     NA   NA        NA      NA
#> 5 32.962   NA 232080002 9169410
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

# pad() is mostly useful for statistical models which only use the complete cases:
mod <- lm(LIFEEX ~ PCGDP, wlddev)
# Generating a residual column in the original data (automatic selection of method "vpos")
settfm(wlddev, resid = pad(resid(mod), mod$na.action))
#> Error in mod$na.action: object of type 'builtin' is not subsettable
# Another way to do it:
r <- resid(mod)
i <- as.integer(names(r))
resid2 <- pad(r, i)        # automatic selection of method "xpos"
# here we need to add some elements as flast(i) < nrow(wlddev)
resid2 <- c(resid2, rep(NA, nrow(wlddev)-length(resid2)))
# See that these are identical:
identical(unattrib(wlddev$resid), resid2)
#> [1] FALSE

# Can also easily get a model matrix at the dimensions of the original data
mm <- pad(model.matrix(mod), mod$na.action)
```
