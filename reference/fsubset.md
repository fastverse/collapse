# Fast Subsetting Matrix-Like Objects

`fsubset` returns subsets of vectors, matrices or data frames which meet
conditions. It is programmed very efficiently and uses C source code
from the *data.table* package. The methods also provide enhanced
functionality compared to
[`subset`](https://rdrr.io/r/base/subset.html). The function `ss`
provides an (internal generic) programmers alternative to `[` that does
not drop dimensions and is significantly faster than `[` for data
frames.

## Usage

``` r
fsubset(.x, ...)
sbt(.x, ...)     # Shorthand for fsubset

# Default S3 method
fsubset(.x, subset, ...)

# S3 method for class 'matrix'
fsubset(.x, subset, ..., drop = FALSE)

# S3 method for class 'data.frame'
fsubset(.x, subset, ...)

# Methods for indexed data / compatibility with plm:

# S3 method for class 'pseries'
fsubset(.x, subset, ..., drop.index.levels = "id")

# S3 method for class 'pdata.frame'
fsubset(.x, subset, ..., drop.index.levels = "id")


# Fast subsetting (replaces `[` with drop = FALSE, programmers choice)
ss(x, i, j, check = TRUE)
```

## Arguments

- .x:

  object to be subsetted according to different methods.

- x:

  a data frame / list, matrix or vector/array (only `i`).

- subset:

  logical expression indicating elements or rows to keep: missing values
  are taken as `FALSE`. The default, matrix and pseries methods only
  support logical vectors or row-indices (or a character vector of
  rownames if the matrix has rownames).

- ...:

  For the matrix or data frame method: multiple comma-separated
  expressions indicating columns to select. Otherwise: further arguments
  to be passed to or from other methods.

- drop:

  passed on to `[` indexing operator. Only available for the matrix
  method.

- i:

  positive or negative row-indices or a logical vector to subset the
  rows of `x`.

- j:

  a vector of column names, positive or negative indices or a suitable
  logical vector to subset the columns of `x`. *Note:* Negative indices
  are converted to positive ones using `j <- seq_along(x)[j]`.

- check:

  logical. `FALSE` skips checks on `i` and `j`, e.g. whether indices are
  negative. This offers a speedup to programmers, but can terminate R if
  zero or negative indices are passed.

- drop.index.levels:

  character. Either `"id"`, `"time"`, `"all"` or `"none"`. See
  [indexing](https://fastverse.org/collapse/reference/indexing.md).

## Details

`fsubset` is a generic function, with methods supplied for vectors,
matrices, and data frames (including lists). It represents an
improvement over [`subset`](https://rdrr.io/r/base/subset.html) in terms
of both speed and functionality. The function `ss` is an improvement of
`[` to subset (vectors) matrices and data frames without dropping
dimensions. It is significantly faster than `[.data.frame`.

For ordinary vectors, `subset` can be integer or logical, subsetting is
done in C and more efficient than `[` for large vectors.

For matrices the implementation is all base-R but slightly more
efficient and more versatile than
[`subset.matrix`](https://rdrr.io/r/base/subset.html). Thus it is
possible to `subset` matrix rows using logical or integer vectors, or
character vectors matching rownames. The `drop` argument is passed on to
the `[` method for matrices.

For both matrices and data frames, the `...` argument can be used to
subset columns, and is evaluated in a non-standard way. Thus it can
support vectors of column names, indices or logical vectors, but also
multiple comma separated column names passed without quotes, each of
which may also be replaced by a sequence of columns i.e. `col1:coln`,
and new column names may be assigned e.g.
`fsubset(data, col1 > 20, newname = col2, col3:col6)` (see examples).

For data frames, the `subset` argument is also evaluated in a
non-standard way. Thus next to vector of row-indices or logical vectors,
it supports logical expressions of the form `col2 > 5 & col2 < col3`
etc. (see examples). The data frame method is implemented in C, hence it
is significantly faster than
[`subset.data.frame`](https://rdrr.io/r/base/subset.html). If fast data
frame subsetting is required but no non-standard evaluation, the
function `ss` is slightly simpler and faster.

Factors may have empty levels after subsetting; unused levels are not
automatically removed. See
[`fdroplevels`](https://fastverse.org/collapse/reference/fdroplevels.md)
to drop all unused levels from a data frame.

## Value

An object similar to `.x/x` containing just the selected elements (for a
vector), rows and columns (for a matrix or data frame).

## Note

`ss` offers no support for indexed data. Use `fsubset` with indices
instead.

No replacement method `fsubset<-` or `ss<-` is offered in *collapse*.
For efficient subset replacement (without copying) use
[`data.table::set`](https://rdrr.io/pkg/data.table/man/assign.html),
which can also be used with data frames and tibbles. To search and
replace certain elements without copying, and to efficiently copy
elements / rows from an equally sized vector / data frame, see
[`setv`](https://fastverse.org/collapse/reference/efficient-programming.md).

For subsetting columns alone, please also see [selecting and replacing
columns](https://fastverse.org/collapse/reference/select_replace_vars.md).

Note that the use of
[`%==%`](https://fastverse.org/collapse/reference/efficient-programming.md)
can yield significant performance gains on large data.

## See also

[`fselect`](https://fastverse.org/collapse/reference/select_replace_vars.md),
[`get_vars`](https://fastverse.org/collapse/reference/select_replace_vars.md),
[`ftransform`](https://fastverse.org/collapse/reference/ftransform.md),
[Data Frame
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
fsubset(airquality, Temp > 90, Ozone, Temp)
#>    Ozone Temp
#> 1     NA   93
#> 2     NA   92
#> 3     97   92
#> 4     97   92
#> 5     NA   91
#> 6     NA   92
#> 7     76   97
#> 8    118   94
#> 9     84   96
#> 10    85   94
#> 11    96   91
#> 12    78   92
#> 13    73   93
#> 14    91   93
fsubset(airquality, Temp > 90, OZ = Ozone, Temp) # With renaming
#>     OZ Temp
#> 1   NA   93
#> 2   NA   92
#> 3   97   92
#> 4   97   92
#> 5   NA   91
#> 6   NA   92
#> 7   76   97
#> 8  118   94
#> 9   84   96
#> 10  85   94
#> 11  96   91
#> 12  78   92
#> 13  73   93
#> 14  91   93
fsubset(airquality, Day == 1, -Temp)
#>   Ozone Solar.R Wind Month Day
#> 1    41     190  7.4     5   1
#> 2    NA     286  8.6     6   1
#> 3   135     269  4.1     7   1
#> 4    39      83  6.9     8   1
#> 5    96     167  6.9     9   1
fsubset(airquality, Day == 1, -(Day:Temp))
#>   Ozone Solar.R Wind
#> 1    41     190  7.4
#> 2    NA     286  8.6
#> 3   135     269  4.1
#> 4    39      83  6.9
#> 5    96     167  6.9
fsubset(airquality, Day == 1, Ozone:Wind)
#>   Ozone Solar.R Wind
#> 1    41     190  7.4
#> 2    NA     286  8.6
#> 3   135     269  4.1
#> 4    39      83  6.9
#> 5    96     167  6.9
fsubset(airquality, Day == 1 & !is.na(Ozone), Ozone:Wind, Month)
#>   Ozone Solar.R Wind Month
#> 1    41     190  7.4     5
#> 2   135     269  4.1     7
#> 3    39      83  6.9     8
#> 4    96     167  6.9     9
fsubset(airquality, Day %==% 1, -Temp)  # Faster for big data, as %==% directly returns indices
#>   Ozone Solar.R Wind Month Day
#> 1    41     190  7.4     5   1
#> 2    NA     286  8.6     6   1
#> 3   135     269  4.1     7   1
#> 4    39      83  6.9     8   1
#> 5    96     167  6.9     9   1

ss(airquality, 1:10, 2:3)         # Significantly faster than airquality[1:10, 2:3]
#>    Solar.R Wind
#> 1      190  7.4
#> 2      118  8.0
#> 3      149 12.6
#> 4      313 11.5
#> 5       NA 14.3
#> 6       NA 14.9
#> 7      299  8.6
#> 8       99 13.8
#> 9       19 20.1
#> 10     194  8.6
fsubset(airquality, 1:10, 2:3)    # This is possible but not advised
#>    Solar.R Wind
#> 1      190  7.4
#> 2      118  8.0
#> 3      149 12.6
#> 4      313 11.5
#> 5       NA 14.3
#> 6       NA 14.9
#> 7      299  8.6
#> 8       99 13.8
#> 9       19 20.1
#> 10     194  8.6
```
