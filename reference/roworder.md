# Fast Reordering of Data Frame Rows

A fast substitute for
[`dplyr::arrange`](https://dplyr.tidyverse.org/reference/arrange.html),
based on
[`radixorder(v)`](https://fastverse.org/collapse/reference/radixorder.md)
and inspired by `data.table::setorder(v)`. It returns a sorted copy of
the data frame, unless the data is already sorted in which case no copy
is made. In addition, rows can be manually re-ordered. `roworderv` is a
programmers version that takes vectors/variables as input.

Use `data.table::setorder(v)` to sort a data frame without creating a
copy.

## Usage

``` r
roworder(X, ..., na.last = TRUE, verbose = .op[["verbose"]])

roworderv(X, cols = NULL, neworder = NULL, decreasing = FALSE,
          na.last = TRUE, pos = "front", verbose = .op[["verbose"]])
```

## Arguments

- X:

  a data frame or list of equal-length columns.

- ...:

  comma-separated columns of `X` to sort by e.g. `var1, var2`. Negatives
  i.e. `-var1, var2` can be used to sort in decreasing order of `var1`.
  Internally all expressions are turned into strings and
  `startsWith(expr, "-")` is used to detect this, thus it does not
  negate the actual values (which may as well be strings), and you
  cannot apply any other functions to columns inside `roworder()` to
  induce different sorting behavior.

- cols:

  select columns to sort by using a function, column names, indices or a
  logical vector. The default `NULL` sorts by all columns in order of
  occurrence (from left to right).

- na.last:

  logical. If `TRUE`, missing values in the sorting columns are placed
  last; if `FALSE`, they are placed first; if `NA` they are removed
  (argument passed to
  [`radixorder`](https://fastverse.org/collapse/reference/radixorder.md)).

- decreasing:

  logical. Should the sort order be increasing or decreasing? Can also
  be a vector of length equal to the number of arguments in `cols`
  (argument passed to
  [`radixorder`](https://fastverse.org/collapse/reference/radixorder.md)).

- neworder:

  an ordering vector, can be `< nrow(X)`. if `pos = "front"` or
  `pos = "end"`, a logical vector can also be supplied. This argument
  overwrites `cols`.

- pos:

  integer or character. Different arrangement options if
  `!is.null(neworder) && length(neworder) < nrow(X)`.

  |  |  |  |  |  |
  |----|----|----|----|----|
  | *Int.* |  | *String* |  | *Description* |
  | 1 |  | "front" |  | move rows in `neworder` to the front (top) of `X` (the default). |
  | 2 |  | "end" |  | move rows in `neworder` to the end (bottom) of `X`. |
  | 3 |  | "exchange" |  | just exchange the order of rows in `neworder`, other rows remain in the same position. |
  | 4 |  | "after" |  | place all further selected rows behind the first selected row. |

- verbose:

  logical. `1L` (default) prints a message when ordering a grouped or
  indexed frame, indicating that this is not efficient and encouraging
  reordering the data prior to the grouping/indexing step. Users can
  also set `verbose = 2L` to also toggle a message if `x` is already
  sorted, implying that no copy was made and the call to `roworder(v)`
  is redundant.

## Value

A copy of `X` with rows reordered. If `X` is already sorted, `X` is
simply returned.

## Note

If you don't require a copy of the data, use
[`data.table::setorder`](https://rdrr.io/pkg/data.table/man/setorder.html)
(you can also use it in a piped call as it invisibly returns the data).

`roworder(v)` has internal facilities to deal with
[grouped](https://fastverse.org/collapse/reference/GRP.md) and
[indexed](https://fastverse.org/collapse/reference/indexing.md) data.
This is however inefficient (since in most cases data could be reordered
before grouping/indexing), and therefore issues a message if
`verbose > 0L`.

## See also

[`colorder`](https://fastverse.org/collapse/reference/colorder.md),
[Data Frame
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Fast Grouping and
Ordering](https://fastverse.org/collapse/reference/fast-grouping-ordering.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
head(roworder(airquality, Month, -Ozone))
#>   Ozone Solar.R Wind Temp Month Day
#> 1   115     223  5.7   79     5  30
#> 2    45     252 14.9   81     5  29
#> 3    41     190  7.4   67     5   1
#> 4    37     279  7.4   76     5  31
#> 5    36     118  8.0   72     5   2
#> 6    34     307 12.0   66     5  17
head(roworder(airquality, Month, -Ozone, na.last = NA))  # Removes the missing values in Ozone
#>   Ozone Solar.R Wind Temp Month Day
#> 1   115     223  5.7   79     5  30
#> 2    45     252 14.9   81     5  29
#> 3    41     190  7.4   67     5   1
#> 4    37     279  7.4   76     5  31
#> 5    36     118  8.0   72     5   2
#> 6    34     307 12.0   66     5  17

## Same in standard evaluation
head(roworderv(airquality, c("Month", "Ozone"), decreasing = c(FALSE, TRUE)))
#>   Ozone Solar.R Wind Temp Month Day
#> 1   115     223  5.7   79     5  30
#> 2    45     252 14.9   81     5  29
#> 3    41     190  7.4   67     5   1
#> 4    37     279  7.4   76     5  31
#> 5    36     118  8.0   72     5   2
#> 6    34     307 12.0   66     5  17
head(roworderv(airquality, c("Month", "Ozone"), decreasing = c(FALSE, TRUE), na.last = NA))
#>   Ozone Solar.R Wind Temp Month Day
#> 1   115     223  5.7   79     5  30
#> 2    45     252 14.9   81     5  29
#> 3    41     190  7.4   67     5   1
#> 4    37     279  7.4   76     5  31
#> 5    36     118  8.0   72     5   2
#> 6    34     307 12.0   66     5  17

## Custom reordering
head(roworderv(mtcars, neworder = 3:4))               # Bring rows 3 and 4 to the front
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Datsun 710        22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag     21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#> Valiant           18.1   6  225 105 2.76 3.460 20.22  1  0    3    1
head(roworderv(mtcars, neworder = 3:4, pos = "end"))  # Bring them to the end
#>                    mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6 160.0 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag     21.0   6 160.0 110 3.90 2.875 17.02  0  1    4    4
#> Hornet Sportabout 18.7   8 360.0 175 3.15 3.440 17.02  0  0    3    2
#> Valiant           18.1   6 225.0 105 2.76 3.460 20.22  1  0    3    1
#> Duster 360        14.3   8 360.0 245 3.21 3.570 15.84  0  0    3    4
#> Merc 240D         24.4   4 146.7  62 3.69 3.190 20.00  1  0    4    2
head(roworderv(mtcars, neworder = mtcars$vs == 1))    # Bring rows with vs == 1 to the top
#>                 mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Datsun 710     22.8   4 108.0  93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive 21.4   6 258.0 110 3.08 3.215 19.44  1  0    3    1
#> Valiant        18.1   6 225.0 105 2.76 3.460 20.22  1  0    3    1
#> Merc 240D      24.4   4 146.7  62 3.69 3.190 20.00  1  0    4    2
#> Merc 230       22.8   4 140.8  95 3.92 3.150 22.90  1  0    4    2
#> Merc 280       19.2   6 167.6 123 3.92 3.440 18.30  1  0    4    4
```
