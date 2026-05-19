# Transform Data by (Grouped) Replacing or Sweeping out Statistics

`TRA` is an S3 generic that efficiently transforms data by either
(column-wise) replacing data values with supplied statistics or sweeping
the statistics out of the data. `TRA` supports grouped operations and
data transformation by reference, and is thus a generalization of
[`sweep`](https://rdrr.io/r/base/sweep.html).

## Usage

``` r
TRA(x, STATS, FUN = "-", ...)
setTRA(x, STATS, FUN = "-", ...) # Shorthand for invisible(TRA(..., set = TRUE))

# Default S3 method
TRA(x, STATS, FUN = "-", g = NULL, set = FALSE, ...)

# S3 method for class 'matrix'
TRA(x, STATS, FUN = "-", g = NULL, set = FALSE, ...)

# S3 method for class 'data.frame'
TRA(x, STATS, FUN = "-", g = NULL, set = FALSE, ...)

# S3 method for class 'grouped_df'
TRA(x, STATS, FUN = "-", keep.group_vars = TRUE, set = FALSE, ...)
```

## Arguments

- x:

  a atomic vector, matrix, data frame or grouped data frame (class
  'grouped_df').

- STATS:

  a matching set of summary statistics. See Details and Examples.

- FUN:

  an integer or character string indicating the operation to perform.
  There are 11 supported operations:

  |  |  |  |  |  |
  |----|----|----|----|----|
  | *Int.* |  | *String* |  | *Description* |
  | 0 |  | "na" or "replace_na" |  | replace missing values in `x` |
  | 1 |  | "fill" or "replace_fill" |  | replace data and missing values in `x` |
  | 2 |  | "replace" |  | replace data but preserve missing values in `x` |
  | 3 |  | "-" |  | subtract (center on `STATS`) |
  | 4 |  | "-+" |  | subtract group-statistics but add group-frequency weighted average of group statistics (i.e. center on overall average statistic) |
  | 5 |  | "/" |  | divide (i.e. scale. For mean-preserving scaling see also [`fscale`](https://fastverse.org/collapse/reference/fscale.md)) |
  | 6 |  | "%" |  | compute percentages (divide and multiply by 100) |
  | 7 |  | "+" |  | add |
  | 8 |  | "\*" |  | multiply |
  | 9 |  | "%%" |  | modulus (remainder from division by `STATS`) |
  | 10 |  | "-%%" |  | subtract modulus (make data divisible by `STATS`) |

- g:

  a factor, [`GRP`](https://fastverse.org/collapse/reference/GRP.md)
  object, atomic vector (internally converted to factor) or a list of
  vectors / factors (internally converted to a
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object) used
  to group `x`. Number of groups must match rows of `STATS`. See
  Details.

- set:

  logical. `TRUE` transforms data by reference i.e. performs in-place
  modification of the data without creating a copy.

- keep.group_vars:

  *grouped_df method:* Logical. `FALSE` removes grouping variables after
  computation. See Details and Examples.

- ...:

  arguments to be passed to or from other methods.

## Details

Without groups (`g = NULL`), `TRA` is little more than a column based
version of [`sweep`](https://rdrr.io/r/base/sweep.html), albeit many
times more efficient. In this case all methods support an atomic vector
of statistics of length `NCOL(x)` passed to `STATS`. The matrix and data
frame methods also support a 1-row matrix or 1-row data frame / list,
respectively. `TRA` always preserves all attributes of `x`.

With groups passed to `g`, `STATS` needs to be of the same type as `x`
and of appropriate dimensions \[such that `NCOL(x) == NCOL(STATS)` and
`NROW(STATS)` equals the number of groups (i.e. the number of levels if
`g` is a factor)\]. If this condition is satisfied, `TRA` will assume
that the first row of `STATS` is the set of statistics computed on the
first group/level of `g`, the second row on the second group/level etc.
and do groupwise replacing or sweeping out accordingly.

For example Let `x = c(1.2, 4.6, 2.5, 9.1, 8.7, 3.3)`, g is an integer
vector in 3 groups `g = c(1,3,3,2,1,2)` and
`STATS = fmean(x,g) = c(4.95, 6.20, 3.55)`. Then
`out = TRA(x,STATS,"-",g) = c(-3.75, 1.05, -1.05, 2.90, 3.75, -2.90)`
\[same as `fmean(x, g, TRA = "-")`\] does the equivalent of the
following for-loop: `for(i in 1:6) out[i] = x[i] - STATS[g[i]]`.

Correct computation requires that `g` as used in `fmean` and `g` passed
to `TRA` are exactly the same vector. Using `g = c(1,3,3,2,1,2)` for
`fmean` and `g = c(3,1,1,2,3,2)` for `TRA` will not give the right
result. The safest way of programming with `TRA` is thus to repeatedly
employ the same factor or
[`GRP`](https://fastverse.org/collapse/reference/GRP.md) object for all
grouped computations. Atomic vectors passed to `g` will be converted to
factors (see [`qF`](https://fastverse.org/collapse/reference/qF.md)) and
lists will be converted to
[`GRP`](https://fastverse.org/collapse/reference/GRP.md) objects. This
is also done by all [Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
and [`BY`](https://fastverse.org/collapse/reference/BY.md), thus
together with these functions, `TRA` can also safely be used with
atomic- or list-groups (as long as all functions apply sorted grouping,
which is the default in *collapse*).

If `x` is a grouped data frame ('grouped_df'), `TRA` matches the columns
of `x` and `STATS` and also checks for grouping columns in `x` and
`STATS`. `TRA.grouped_df` will then only transform those columns in `x`
for which matching counterparts were found in `STATS` (exempting
grouping columns) and return `x` again (with columns in the same order).
If `keep.group_vars = FALSE`, the grouping columns are dropped after
computation, however the "groups" attribute is not dropped (it can be
removed using
[`fungroup()`](https://fastverse.org/collapse/reference/GRP.md) or
[`dplyr::ungroup()`](https://dplyr.tidyverse.org/reference/group_by.html)).

## Value

`x` with columns replaced or swept out using `STATS`, (optionally)
grouped by `g`.

## Note

In most cases there is no need to call the `TRA()` function, because of
the TRA-argument to all [Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
(ensuring that the exact same grouping vector is used for computing
statistics and subsequent transformation). In addition the functions
[`fbetween/B`](https://fastverse.org/collapse/reference/fbetween_fwithin.md)
and
[`fwithin/W`](https://fastverse.org/collapse/reference/fbetween_fwithin.md)
and [`fscale/STD`](https://fastverse.org/collapse/reference/fscale.md)
provide optimized solutions for frequent scaling, centering and
averaging tasks.

## See also

[`sweep`](https://rdrr.io/r/base/sweep.html), [Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
v <- iris$Sepal.Length          # A numeric vector
f <- iris$Species               # A factor
dat <- num_vars(iris)           # Numeric columns
m <- qM(dat)                    # Matrix of numeric data

head(TRA(v, fmean(v)))                # Simple centering [same as fmean(v, TRA = "-") or W(v)]
#> [1] -0.7433333 -0.9433333 -1.1433333 -1.2433333 -0.8433333 -0.4433333
head(TRA(m, fmean(m)))                # [same as sweep(m, 2, fmean(m)), fmean(m, TRA = "-") or W(m)]
#>      Sepal.Length Sepal.Width Petal.Length Petal.Width
#> [1,]   -0.7433333  0.44266667       -2.358  -0.9993333
#> [2,]   -0.9433333 -0.05733333       -2.358  -0.9993333
#> [3,]   -1.1433333  0.14266667       -2.458  -0.9993333
#> [4,]   -1.2433333  0.04266667       -2.258  -0.9993333
#> [5,]   -0.8433333  0.54266667       -2.358  -0.9993333
#> [6,]   -0.4433333  0.84266667       -2.058  -0.7993333
head(TRA(dat, fmean(dat)))            # [same as fmean(dat, TRA = "-") or W(dat)]
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1   -0.7433333  0.44266667       -2.358  -0.9993333
#> 2   -0.9433333 -0.05733333       -2.358  -0.9993333
#> 3   -1.1433333  0.14266667       -2.458  -0.9993333
#> 4   -1.2433333  0.04266667       -2.258  -0.9993333
#> 5   -0.8433333  0.54266667       -2.358  -0.9993333
#> 6   -0.4433333  0.84266667       -2.058  -0.7993333
head(TRA(v, fmean(v), "replace"))     # Simple replacing [same as fmean(v, TRA = "replace") or B(v)]
#> [1] 5.843333 5.843333 5.843333 5.843333 5.843333 5.843333
head(TRA(m, fmean(m), "replace"))     # [same as sweep(m, 2, fmean(m)), fmean(m, TRA = 1L) or B(m)]
#>      Sepal.Length Sepal.Width Petal.Length Petal.Width
#> [1,]     5.843333    3.057333        3.758    1.199333
#> [2,]     5.843333    3.057333        3.758    1.199333
#> [3,]     5.843333    3.057333        3.758    1.199333
#> [4,]     5.843333    3.057333        3.758    1.199333
#> [5,]     5.843333    3.057333        3.758    1.199333
#> [6,]     5.843333    3.057333        3.758    1.199333
head(TRA(dat, fmean(dat), "replace")) # [same as fmean(dat, TRA = "replace") or B(dat)]
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1     5.843333    3.057333        3.758    1.199333
#> 2     5.843333    3.057333        3.758    1.199333
#> 3     5.843333    3.057333        3.758    1.199333
#> 4     5.843333    3.057333        3.758    1.199333
#> 5     5.843333    3.057333        3.758    1.199333
#> 6     5.843333    3.057333        3.758    1.199333
head(TRA(m, fsd(m), "/"))             # Simple scaling... [same as fsd(m, TRA = "/")]...
#>      Sepal.Length Sepal.Width Petal.Length Petal.Width
#> [1,]     6.158928    8.029986    0.7930671   0.2623854
#> [2,]     5.917402    6.882845    0.7930671   0.2623854
#> [3,]     5.675875    7.341701    0.7364195   0.2623854
#> [4,]     5.555112    7.112273    0.8497148   0.2623854
#> [5,]     6.038165    8.259414    0.7930671   0.2623854
#> [6,]     6.521218    8.947698    0.9630101   0.5247707

# Note: All grouped examples also apply for v and dat...
head(TRA(m, fmean(m, f), "-", f))       # Centering [same as fmean(m, f, TRA = "-") or W(m, f)]
#>      Sepal.Length Sepal.Width Petal.Length Petal.Width
#> [1,]        0.094       0.072       -0.062      -0.046
#> [2,]       -0.106      -0.428       -0.062      -0.046
#> [3,]       -0.306      -0.228       -0.162      -0.046
#> [4,]       -0.406      -0.328        0.038      -0.046
#> [5,]       -0.006       0.172       -0.062      -0.046
#> [6,]        0.394       0.472        0.238       0.154
head(TRA(m, fmean(m, f), "replace", f)) # Replacing [same fmean(m, f, TRA = "replace") or B(m, f)]
#>      Sepal.Length Sepal.Width Petal.Length Petal.Width
#> [1,]        5.006       3.428        1.462       0.246
#> [2,]        5.006       3.428        1.462       0.246
#> [3,]        5.006       3.428        1.462       0.246
#> [4,]        5.006       3.428        1.462       0.246
#> [5,]        5.006       3.428        1.462       0.246
#> [6,]        5.006       3.428        1.462       0.246
head(TRA(m, fsd(m, f), "/", f))         # Scaling [same as fsd(m, f, TRA = "/")]
#>      Sepal.Length Sepal.Width Petal.Length Petal.Width
#> [1,]     14.46851    9.233260     8.061544    1.897793
#> [2,]     13.90112    7.914223     8.061544    1.897793
#> [3,]     13.33372    8.441838     7.485720    1.897793
#> [4,]     13.05003    8.178031     8.637369    1.897793
#> [5,]     14.18481    9.497068     8.061544    1.897793
#> [6,]     15.31960   10.288490     9.789018    3.795585

head(TRA(m, fmean(m, f), "-+", f))      # Centering on the overall mean ...
#>      Sepal.Length Sepal.Width Petal.Length Petal.Width
#> [1,]     5.937333    3.129333        3.696    1.153333
#> [2,]     5.737333    2.629333        3.696    1.153333
#> [3,]     5.537333    2.829333        3.596    1.153333
#> [4,]     5.437333    2.729333        3.796    1.153333
#> [5,]     5.837333    3.229333        3.696    1.153333
#> [6,]     6.237333    3.529333        3.996    1.353333
                                        # [same as fmean(m, f, TRA = "-+") or
                                        #           W(m, f, mean = "overall.mean")]
head(TRA(TRA(m, fmean(m, f), "-", f),   # Also the same thing done manually !!
     fmean(m), "+"))
#>      Sepal.Length Sepal.Width Petal.Length Petal.Width
#> [1,]     5.937333    3.129333        3.696    1.153333
#> [2,]     5.737333    2.629333        3.696    1.153333
#> [3,]     5.537333    2.829333        3.596    1.153333
#> [4,]     5.437333    2.729333        3.796    1.153333
#> [5,]     5.837333    3.229333        3.696    1.153333
#> [6,]     6.237333    3.529333        3.996    1.353333

# Grouped data method
library(magrittr)
iris %>% fgroup_by(Species) %>% TRA(fmean(.))
#>    Sepal.Length Sepal.Width Petal.Length Petal.Width Species
#> 1         0.094       0.072       -0.062      -0.046  setosa
#> 2        -0.106      -0.428       -0.062      -0.046  setosa
#> 3        -0.306      -0.228       -0.162      -0.046  setosa
#> 4        -0.406      -0.328        0.038      -0.046  setosa
#> 5        -0.006       0.172       -0.062      -0.046  setosa
#> 6         0.394       0.472        0.238       0.154  setosa
#> 7        -0.406      -0.028       -0.062       0.054  setosa
#> 8        -0.006      -0.028        0.038      -0.046  setosa
#> 9        -0.606      -0.528       -0.062      -0.046  setosa
#> 10       -0.106      -0.328        0.038      -0.146  setosa
#> 11        0.394       0.272        0.038      -0.046  setosa
#> 12       -0.206      -0.028        0.138      -0.046  setosa
#> 13       -0.206      -0.428       -0.062      -0.146  setosa
#> 14       -0.706      -0.428       -0.362      -0.146  setosa
#>  [ reached 'max' / getOption("max.print") -- omitted 136 rows ]
#> 
#> Grouped by:  Species  [3 | 50 (0)] 
iris %>% fgroup_by(Species) %>% fmean(TRA = "-")        # Same thing
#>    Species Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1   setosa        0.094       0.072       -0.062      -0.046
#> 2   setosa       -0.106      -0.428       -0.062      -0.046
#> 3   setosa       -0.306      -0.228       -0.162      -0.046
#> 4   setosa       -0.406      -0.328        0.038      -0.046
#> 5   setosa       -0.006       0.172       -0.062      -0.046
#> 6   setosa        0.394       0.472        0.238       0.154
#> 7   setosa       -0.406      -0.028       -0.062       0.054
#> 8   setosa       -0.006      -0.028        0.038      -0.046
#> 9   setosa       -0.606      -0.528       -0.062      -0.046
#> 10  setosa       -0.106      -0.328        0.038      -0.146
#> 11  setosa        0.394       0.272        0.038      -0.046
#> 12  setosa       -0.206      -0.028        0.138      -0.046
#> 13  setosa       -0.206      -0.428       -0.062      -0.146
#> 14  setosa       -0.706      -0.428       -0.362      -0.146
#>  [ reached 'max' / getOption("max.print") -- omitted 136 rows ]
#> 
#> Grouped by:  Species  [3 | 50 (0)] 
iris %>% fgroup_by(Species) %>% TRA(fmean(.)[c(2,4)])   # Only transforming 2 columns
#>    Sepal.Length Sepal.Width Petal.Length Petal.Width Species
#> 1         0.094         3.5       -0.062         0.2  setosa
#> 2        -0.106         3.0       -0.062         0.2  setosa
#> 3        -0.306         3.2       -0.162         0.2  setosa
#> 4        -0.406         3.1        0.038         0.2  setosa
#> 5        -0.006         3.6       -0.062         0.2  setosa
#> 6         0.394         3.9        0.238         0.4  setosa
#> 7        -0.406         3.4       -0.062         0.3  setosa
#> 8        -0.006         3.4        0.038         0.2  setosa
#> 9        -0.606         2.9       -0.062         0.2  setosa
#> 10       -0.106         3.1        0.038         0.1  setosa
#> 11        0.394         3.7        0.038         0.2  setosa
#> 12       -0.206         3.4        0.138         0.2  setosa
#> 13       -0.206         3.0       -0.062         0.1  setosa
#> 14       -0.706         3.0       -0.362         0.1  setosa
#>  [ reached 'max' / getOption("max.print") -- omitted 136 rows ]
#> 
#> Grouped by:  Species  [3 | 50 (0)] 
iris %>% fgroup_by(Species) %>% TRA(fmean(.)[c(2,4)],   # Dropping species column
                                        keep.group_vars = FALSE)
#>    Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1         0.094         3.5       -0.062         0.2
#> 2        -0.106         3.0       -0.062         0.2
#> 3        -0.306         3.2       -0.162         0.2
#> 4        -0.406         3.1        0.038         0.2
#> 5        -0.006         3.6       -0.062         0.2
#> 6         0.394         3.9        0.238         0.4
#> 7        -0.406         3.4       -0.062         0.3
#> 8        -0.006         3.4        0.038         0.2
#> 9        -0.606         2.9       -0.062         0.2
#> 10       -0.106         3.1        0.038         0.1
#> 11        0.394         3.7        0.038         0.2
#> 12       -0.206         3.4        0.138         0.2
#> 13       -0.206         3.0       -0.062         0.1
#> 14       -0.706         3.0       -0.362         0.1
#> 15        0.794         4.0       -0.262         0.2
#> 16        0.694         4.4        0.038         0.4
#> 17        0.394         3.9       -0.162         0.4
#>  [ reached 'max' / getOption("max.print") -- omitted 133 rows ]
#> 
#> Grouped by:  Species  [3 | 50 (0)] 
```
