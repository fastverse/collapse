# Efficiently Count Observations by Group

A much faster replacement for
[`dplyr::count`](https://dplyr.tidyverse.org/reference/count.html).

## Usage

``` r
fcount(x, ..., w = NULL, name = "N", add = FALSE,
      sort = FALSE, decreasing = FALSE, drop = TRUE)

fcountv(x, cols = NULL, w = NULL, name = "N", add = FALSE,
        sort = FALSE, drop = TRUE, ...)
```

## Arguments

- x:

  a data frame or list-like object, including 'grouped_df' or
  'indexed_frame'. Atomic vectors or matrices can also be passed, but
  will be sent through
  [`qDF`](https://fastverse.org/collapse/reference/quick-conversion.md).

- ...:

  for `fcount`: names or sequences of columns to count cases by - passed
  to
  [`fselect`](https://fastverse.org/collapse/reference/select_replace_vars.md).
  For `fcountv`: further arguments passed to
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md) (such as
  `decreasing`, `na.last`, `method`, `effect` etc.). Leaving this empty
  will count on all columns.

- cols:

  select columns to count cases by, using column names, indices, a
  logical vector or a selector function (e.g. `is_categorical`).

- w:

  a numeric vector of weights, may contain missing values. In `fcount`
  this can also be the (unquoted) name of a column in the data frame.
  `fcountv` also supports a single character name. *Note* that the
  corresponding argument in
  [`dplyr::count`](https://dplyr.tidyverse.org/reference/count.html) is
  called `wt`, but *collapse* has a global default for weights arguments
  to be called `w`.

- name:

  character. The name of the column containing the count or sum of
  weights.
  [`dplyr::count`](https://dplyr.tidyverse.org/reference/count.html) it
  is called `"n"`, but `"N"` is more consistent with the rest of
  *collapse* and *data.table*.

- add:

  `TRUE` adds the count column to `x`. Alternatively
  `add = "group_vars"` (or `add = "gv"` for parsimony) can be used to
  retain only the variables selected for counting in `x` and the count.

- sort, decreasing:

  arguments passed to
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md) affecting the
  order of rows in the output (if `add = FALSE`), and the algorithm used
  for counting. In general, `sort = FALSE` is faster unless data is
  already sorted by the columns used for counting.

- drop:

  logical. `FALSE` retains zero-count rows for unobserved combinations
  of factor levels (analogous to `dplyr::count(..., .drop = FALSE)`);
  applies only when at least one of the counted columns is a factor. See
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md) (`drop`
  argument of `GRP.default`).

## Value

If `x` is a list, an object of the same type as `x` with a column
(`name`) added at the end giving the count. Otherwise, if `x` is atomic,
a data frame returned from
[`qDF(x)`](https://fastverse.org/collapse/reference/quick-conversion.md)
with the count column added. By default (`add = FALSE`) only the unique
rows of `x` of the columns used for counting are returned.

## See also

[`GRPN`](https://fastverse.org/collapse/reference/GRP.md),
[`fnobs`](https://fastverse.org/collapse/reference/fnobs.md),
[`fndistinct`](https://fastverse.org/collapse/reference/fndistinct.md),
[Fast Grouping and
Ordering](https://fastverse.org/collapse/reference/fast-grouping-ordering.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
fcount(mtcars, cyl, vs, am)
#>   cyl vs am  N
#> 1   6  0  1  3
#> 2   4  1  1  7
#> 3   6  1  0  4
#> 4   8  0  0 12
#> 5   4  1  0  3
#> 6   4  0  1  1
#> 7   8  0  1  2
fcountv(mtcars, cols = .c(cyl, vs, am))
#>   cyl vs am  N
#> 1   6  0  1  3
#> 2   4  1  1  7
#> 3   6  1  0  4
#> 4   8  0  0 12
#> 5   4  1  0  3
#> 6   4  0  1  1
#> 7   8  0  1  2
fcount(mtcars, cyl, vs, am, sort = TRUE)
#>   cyl vs am  N
#> 1   4  0  1  1
#> 2   4  1  0  3
#> 3   4  1  1  7
#> 4   6  0  1  3
#> 5   6  1  0  4
#> 6   8  0  0 12
#> 7   8  0  1  2
fcount(mtcars, cyl, vs, am, add = TRUE)
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb  N
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46  0  1    4    4  3
#> Mazda RX4 Wag     21.0   6  160 110 3.90 2.875 17.02  0  1    4    4  3
#> Datsun 710        22.8   4  108  93 3.85 2.320 18.61  1  1    4    1  7
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44  1  0    3    1  4
#> Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2 12
#>  [ reached 'max' / getOption("max.print") -- omitted 27 rows ]
fcount(mtcars, cyl, vs, am, add = "group_vars")
#>                     cyl vs am  N
#> Mazda RX4             6  0  1  3
#> Mazda RX4 Wag         6  0  1  3
#> Datsun 710            4  1  1  7
#> Hornet 4 Drive        6  1  0  4
#> Hornet Sportabout     8  0  0 12
#> Valiant               6  1  0  4
#> Duster 360            8  0  0 12
#> Merc 240D             4  1  0  3
#> Merc 230              4  1  0  3
#> Merc 280              6  1  0  4
#> Merc 280C             6  1  0  4
#> Merc 450SE            8  0  0 12
#> Merc 450SL            8  0  0 12
#> Merc 450SLC           8  0  0 12
#> Cadillac Fleetwood    8  0  0 12
#> Lincoln Continental   8  0  0 12
#> Chrysler Imperial     8  0  0 12
#>  [ reached 'max' / getOption("max.print") -- omitted 15 rows ]

## With grouped data
mtcars |> fgroup_by(cyl, vs, am) |> fcount()
#>   cyl vs am  N
#> 1   4  0  1  1
#> 2   4  1  0  3
#> 3   4  1  1  7
#> 4   6  0  1  3
#> 5   6  1  0  4
#> 6   8  0  0 12
#> 7   8  0  1  2
mtcars |> fgroup_by(cyl, vs, am) |> fcount(add = TRUE)
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb  N
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46  0  1    4    4  3
#> Mazda RX4 Wag     21.0   6  160 110 3.90 2.875 17.02  0  1    4    4  3
#> Datsun 710        22.8   4  108  93 3.85 2.320 18.61  1  1    4    1  7
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44  1  0    3    1  4
#> Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2 12
#>  [ reached 'max' / getOption("max.print") -- omitted 27 rows ]
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 
mtcars |> fgroup_by(cyl, vs, am) |> fcount(add = "group_vars")
#>                     cyl vs am  N
#> Mazda RX4             6  0  1  3
#> Mazda RX4 Wag         6  0  1  3
#> Datsun 710            4  1  1  7
#> Hornet 4 Drive        6  1  0  4
#> Hornet Sportabout     8  0  0 12
#> Valiant               6  1  0  4
#> Duster 360            8  0  0 12
#> Merc 240D             4  1  0  3
#> Merc 230              4  1  0  3
#> Merc 280              6  1  0  4
#> Merc 280C             6  1  0  4
#> Merc 450SE            8  0  0 12
#> Merc 450SL            8  0  0 12
#> Merc 450SLC           8  0  0 12
#> Cadillac Fleetwood    8  0  0 12
#> Lincoln Continental   8  0  0 12
#> Chrysler Imperial     8  0  0 12
#>  [ reached 'max' / getOption("max.print") -- omitted 15 rows ]
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 

## With indexed data: by default counting on the first index variable
wlddev |> findex_by(country, year) |> fcount()
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE    NA
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE    NA
#> 5 Afghanistan   AFG 1965-01-01 1964   1960 South Asia Low income FALSE    NA
#>   LIFEEX GINI       ODA     POP N
#> 1 32.446   NA 116769997 8996973 1
#> 2 32.962   NA 232080002 9169410 1
#> 3 33.471   NA 112839996 9351441 1
#> 4 33.971   NA 237720001 9543205 1
#> 5 34.463   NA 295920013 9744781 1
#>  [ reached 'max' / getOption("max.print") -- omitted 13171 rows ]
#> 
#> Indexed by:  country [216] | year [61] 
wlddev |> findex_by(country, year) |> fcount(add = TRUE)
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE    NA
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE    NA
#> 5 Afghanistan   AFG 1965-01-01 1964   1960 South Asia Low income FALSE    NA
#>   LIFEEX GINI       ODA     POP N
#> 1 32.446   NA 116769997 8996973 1
#> 2 32.962   NA 232080002 9169410 1
#> 3 33.471   NA 112839996 9351441 1
#> 4 33.971   NA 237720001 9543205 1
#> 5 34.463   NA 295920013 9744781 1
#>  [ reached 'max' / getOption("max.print") -- omitted 13171 rows ]
#> 
#> Indexed by:  country [216] | year [61] 
# Use fcountv to pass additional arguments to GRP.pdata.frame,
# here using the effect argument to choose a different index variable
wlddev |> findex_by(country, year) |> fcountv(effect = "year")
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE    NA
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE    NA
#> 5 Afghanistan   AFG 1965-01-01 1964   1960 South Asia Low income FALSE    NA
#>   LIFEEX GINI       ODA     POP N
#> 1 32.446   NA 116769997 8996973 1
#> 2 32.962   NA 232080002 9169410 1
#> 3 33.471   NA 112839996 9351441 1
#> 4 33.971   NA 237720001 9543205 1
#> 5 34.463   NA 295920013 9744781 1
#>  [ reached 'max' / getOption("max.print") -- omitted 13171 rows ]
#> 
#> Indexed by:  country [216] | year [61] 
wlddev |> findex_by(country, year) |> fcountv(add = "group_vars", effect = "year")
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE    NA
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE    NA
#> 5 Afghanistan   AFG 1965-01-01 1964   1960 South Asia Low income FALSE    NA
#>   LIFEEX GINI       ODA     POP N
#> 1 32.446   NA 116769997 8996973 1
#> 2 32.962   NA 232080002 9169410 1
#> 3 33.471   NA 112839996 9351441 1
#> 4 33.971   NA 237720001 9543205 1
#> 5 34.463   NA 295920013 9744781 1
#>  [ reached 'max' / getOption("max.print") -- omitted 13171 rows ]
#> 
#> Indexed by:  country [216] | year [61] 
```
