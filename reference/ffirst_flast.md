# Fast (Grouped) First and Last Value for Matrix-Like Objects

`ffirst` and `flast` are S3 generic functions that (column-wise) returns
the first and last values in `x`, (optionally) grouped by `g`. The
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) argument can
further be used to transform `x` using its (groupwise) first and last
values.

## Usage

``` r
ffirst(x, ...)
flast(x, ...)

# Default S3 method
ffirst(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
       use.g.names = TRUE, ...)
# Default S3 method
flast(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = TRUE, ...)

# S3 method for class 'matrix'
ffirst(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
       use.g.names = TRUE, drop = TRUE, ...)
# S3 method for class 'matrix'
flast(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = TRUE, drop = TRUE, ...)

# S3 method for class 'data.frame'
ffirst(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
       use.g.names = TRUE, drop = TRUE, ...)
# S3 method for class 'data.frame'
flast(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = TRUE, drop = TRUE, ...)

# S3 method for class 'grouped_df'
ffirst(x, TRA = NULL, na.rm = .op[["na.rm"]],
       use.g.names = FALSE, keep.group_vars = TRUE, ...)
# S3 method for class 'grouped_df'
flast(x, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = FALSE, keep.group_vars = TRUE, ...)
```

## Arguments

- x:

  a vector, matrix, data frame or grouped data frame (class
  'grouped_df').

- g:

  a factor, [`GRP`](https://fastverse.org/collapse/reference/GRP.md)
  object, atomic vector (internally converted to factor) or a list of
  vectors / factors (internally converted to a
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object) used
  to group `x`.

- TRA:

  an integer or quoted operator indicating the transformation to
  perform: 0 - "na" \| 1 - "fill" \| 2 - "replace" \| 3 - "-" \| 4 -
  "-+" \| 5 - "/" \| 6 - "%" \| 7 - "+" \| 8 - "\*" \| 9 - "%%" \| 10 -
  "-%%". See [`TRA`](https://fastverse.org/collapse/reference/TRA.md).

- na.rm:

  logical. `TRUE` skips missing values and returns the first / last
  non-missing value i.e. if the first (1) / last (n) value is `NA`, take
  the second (2) / second-to-last (n-1) value etc..

- use.g.names:

  logical. Make group-names and add to the result as names (default
  method) or row-names (matrix and data frame methods). No row-names are
  generated for *data.table*'s.

- drop:

  *matrix and data.frame method:* Logical. `TRUE` drops dimensions and
  returns an atomic vector if `g = NULL` and `TRA = NULL`.

- keep.group_vars:

  *grouped_df method:* Logical. `FALSE` removes grouping variables after
  computation.

- ...:

  arguments to be passed to or from other methods. If `TRA` is used,
  passing `set = TRUE` will transform data by reference and return the
  result invisibly.

## Value

`ffirst` returns the first value in `x`, grouped by `g`, or (if
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) is used) `x`
transformed by its first value, grouped by `g`. Similarly `flast`
returns the last value in `x`, ...

## Note

Both functions are significantly faster if `na.rm = FALSE`, particularly
`ffirst` which can take direct advantage of the 'group.starts' elements
in [`GRP`](https://fastverse.org/collapse/reference/GRP.md) objects.

## See also

[Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## default vector method
ffirst(airquality$Ozone)                   # Simple first value
#> [1] 41
ffirst(airquality$Ozone, airquality$Month) # Grouped first value
#>   5   6   7   8   9 
#>  41  29 135  39  96 
ffirst(airquality$Ozone, airquality$Month,
       na.rm = FALSE)                      # Grouped first, but without skipping initial NA's
#>   5   6   7   8   9 
#>  41  NA 135  39  96 

## data.frame method
ffirst(airquality)
#>   Ozone Solar.R    Wind    Temp   Month     Day 
#>    41.0   190.0     7.4    67.0     5.0     1.0 
ffirst(airquality, airquality$Month)
#>   Ozone Solar.R Wind Temp Month Day
#> 5    41     190  7.4   67     5   1
#> 6    29     286  8.6   78     6   1
#> 7   135     269  4.1   84     7   1
#> 8    39      83  6.9   81     8   1
#> 9    96     167  6.9   91     9   1
ffirst(airquality, airquality$Month, na.rm = FALSE) # Again first Ozone measurement in month 6 is NA
#>   Ozone Solar.R Wind Temp Month Day
#> 5    41     190  7.4   67     5   1
#> 6    NA     286  8.6   78     6   1
#> 7   135     269  4.1   84     7   1
#> 8    39      83  6.9   81     8   1
#> 9    96     167  6.9   91     9   1

## matrix method
aqm <- qM(airquality)
ffirst(aqm)
#>   Ozone Solar.R    Wind    Temp   Month     Day 
#>    41.0   190.0     7.4    67.0     5.0     1.0 
ffirst(aqm, airquality$Month) # etc..
#>   Ozone Solar.R Wind Temp Month Day
#> 5    41     190  7.4   67     5   1
#> 6    29     286  8.6   78     6   1
#> 7   135     269  4.1   84     7   1
#> 8    39      83  6.9   81     8   1
#> 9    96     167  6.9   91     9   1
 
## method for grouped data frames - created with dplyr::group_by or fgroup_by
library(dplyr)
airquality |> group_by(Month) |> ffirst()
#> # A tibble: 5 × 6
#>   Month Ozone Solar.R  Wind  Temp   Day
#>   <int> <int>   <int> <dbl> <int> <int>
#> 1     5    41     190   7.4    67     1
#> 2     6    29     286   8.6    78     1
#> 3     7   135     269   4.1    84     1
#> 4     8    39      83   6.9    81     1
#> 5     9    96     167   6.9    91     1
airquality |> group_by(Month) |> select(Ozone) |> ffirst(na.rm = FALSE)
#> Adding missing grouping variables: `Month`
#> # A tibble: 5 × 2
#>   Month Ozone
#>   <int> <int>
#> 1     5    41
#> 2     6    NA
#> 3     7   135
#> 4     8    39
#> 5     9    96

# Note: All examples generalize to flast.
```
