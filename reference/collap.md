# Advanced Data Aggregation

`collap` is a fast and versatile multi-purpose data aggregation command.

It performs simple and weighted aggregations, multi-type aggregations
automatically applying different functions to numeric and categorical
columns, multi-function aggregations applying multiple functions to each
column, and fully custom aggregations where the user passes a list
mapping functions to columns.

## Usage

``` r
# Main function: allows formula and data input to `by` and `w` arguments
collap(X, by, FUN = fmean, catFUN = fmode, cols = NULL, w = NULL, wFUN = fsum,
       custom = NULL, ..., keep.by = TRUE, keep.w = TRUE, keep.col.order = TRUE,
       sort = .op[["sort"]], decreasing = FALSE, na.last = TRUE, return.order = sort,
       method = "auto", drop = TRUE, parallel = FALSE, mc.cores = 2L,
       return = c("wide","list","long","long_dupl"), give.names = "auto")

# Programmer function: allows column names and indices input to `by` and `w` arguments
collapv(X, by, FUN = fmean, catFUN = fmode, cols = NULL, w = NULL, wFUN = fsum,
        custom = NULL, ..., keep.by = TRUE, keep.w = TRUE, keep.col.order = TRUE,
        sort = .op[["sort"]], decreasing = FALSE, na.last = TRUE, return.order = sort,
        method = "auto", drop = TRUE, parallel = FALSE, mc.cores = 2L,
        return = c("wide","list","long","long_dupl"), give.names = "auto")

# Auxiliary function: for grouped data ('grouped_df') input + non-standard evaluation
collapg(X, FUN = fmean, catFUN = fmode, cols = NULL, w = NULL, wFUN = fsum,
        custom = NULL, keep.group_vars = TRUE, ...)
```

## Arguments

- X:

  a data frame, or an object coercible to data frame using
  [`qDF`](https://fastverse.org/collapse/reference/quick-conversion.md).

- by:

  for `collap`: a one-or two sided formula, i.e. `~ group1` or
  `var1 + var2 ~ group1 + group2`, or a atomic vector, list of vectors
  or [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object
  used to group `X`. For `collapv`: names or indices of grouping
  columns, or a logical vector or selector function such as
  [`is_categorical`](https://fastverse.org/collapse/reference/small-helpers.md)
  selecting grouping columns.

- FUN:

  a function, list of functions (i.e. `list(fsum, fmean, fsd)` or
  `list(sd = fsd, myfun1 = function(x)..)`), or a character vector of
  function names, which are automatically applied only to numeric
  variables.

- catFUN:

  same as `FUN`, but applied only to categorical (non-numeric) typed
  columns
  ([`is_categorical`](https://fastverse.org/collapse/reference/small-helpers.md)).

- cols:

  select columns to aggregate using a function, column names, indices or
  logical vector. *Note*: `cols` is ignored if a two-sided formula is
  passed to `by`.

- w:

  weights. Can be passed as numeric vector or alternatively as formula
  i.e. `~ weightvar` in `collap` or column name / index etc. i.e.
  `"weightvar"` in `collapv`. `collapg` supports non-standard
  evaluations so `weightvar` can be indicated without quotes.

- wFUN:

  same as `FUN`: Function(s) to aggregate weight variable if
  `keep.w = TRUE`. By default the sum of the weights is computed in each
  group.

- custom:

  a named list specifying a fully customized aggregation task. The names
  of the list are function names and the content columns to aggregate
  using this function (same input as `cols`). For example
  `custom = list(fmean = 1:6, fsd = 7:9, fmode = 10:11)` tells `collap`
  to aggregate columns 1-6 of `X` using the mean, columns 7-9 using the
  standard deviation etc. *Notes*: `custom` lets `collap` ignore any
  inputs passed to `FUN`, `catFUN` or `cols`. Since v1.6.0 you can also
  rename columns e.g.
  `custom = list(fmean = c(newname = "col1", "col2"), fmode = c(newname = 3))`.

- keep.by, keep.group_vars:

  logical. `FALSE` will omit grouping variables from the output. `TRUE`
  keeps the variables, even if passed externally in a list or vector
  (unlike other *collapse* functions).

- keep.w:

  logical. `FALSE` will omit weight variable from the output i.e. no
  aggregation of the weights. `TRUE` aggregates and adds weights, even
  if passed externally as a vector (unlike other *collapse* functions).

- keep.col.order:

  logical. Retain original column order post-aggregation.

- sort, decreasing, na.last, return.order, method:

  logical / character. Arguments passed to
  [`GRP.default`](https://fastverse.org/collapse/reference/GRP.md) and
  affecting the row-order in the aggregated data frame and the grouping
  algorithm.

- drop:

  logical. `FALSE` retains zero-count rows for unobserved combinations
  of factor levels among the grouping columns (analogous to
  `dplyr::group_by(.drop = FALSE)`). The corresponding rows in the
  aggregated output will contain values produced by the aggregation
  functions for empty groups (e.g. `NA` for `fmean`, `0` for `fsum`).
  See [`GRP`](https://fastverse.org/collapse/reference/GRP.md) (`drop`
  argument of `GRP.default`).

- parallel:

  logical. Use `mclapply` instead of `lapply` to parallelize the
  computation at the column level. Not available for Windows.

- mc.cores:

  integer. Argument to `mclapply` setting the number of cores to use,
  default is 2.

- return:

  character. Control the output format when aggregating with multiple
  functions or performing custom aggregation. "wide" (default) returns a
  wider data frame with added columns for each additional function.
  "list" returns a list of data frames - one for each function. "long"
  adds a column "Function" and row-binds the results from different
  functions using
  [`data.table::rbindlist`](https://rdrr.io/pkg/data.table/man/rbindlist.html).
  "long_dupl" is a special option for aggregating multi-type data using
  multiple `FUN` but only one `catFUN` or vice-versa. In that case the
  format is long and data aggregated using only one function is
  duplicated. See Examples.

- give.names:

  logical. Create unique names of aggregated columns by adding a prefix
  'FUN.var'. `'auto'` will automatically create such prefixes whenever
  multiple functions are applied to a column.

- ...:

  additional arguments passed to all functions supplied to `FUN`,
  `catFUN`, `wFUN` or `custom`. Since v1.9.0 these are also split by
  groups for non-[Fast Statistical
  Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md).
  The behavior of [Fast Statistical
  Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
  with unused arguments is regulated by
  `option("collapse_unused_arg_action")` and defaults to `"warning"`.
  `collapg` also allows other arguments to `collap` except for
  `sort, decreasing, na.last, return.order, method` and `keep.by`.

## Details

`collap` automatically checks each function passed to it whether it is a
[Fast Statistical
Function](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
(i.e. whether the function name is contained in `.FAST_STAT_FUN`). If
the function is a fast statistical function, `collap` only does the
grouping and then calls the function to carry out the grouped
computations (vectorized in C/C++), resulting in high aggregation
speeds, even with weights. If the function is not one of
`.FAST_STAT_FUN`, [`BY`](https://fastverse.org/collapse/reference/BY.md)
is called internally to perform the computation. The resulting
computations from each function are put into a list and recombined to
produce the desired output format as controlled by the `return`
argument. This is substantially slower, particularly with many groups.

When setting `parallel = TRUE` on a non-windows computer, aggregations
will efficiently be parallelized at the column level using `mclapply`
utilizing `mc.cores` cores. Some [Fast Statistical
Function](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
support multithreading i.e. have an `nthreads` argument that can be
passed to `collap`. Using C-level multithreading is much more effective
than R-level parallelism, and also works on Windows, but the two should
never be combined.

When the `w` argument is used, the weights are passed to all functions
except for `wFUN`. This may be undesirable in settings like
`collap(data, ~ id, custom = list(fsum = ..., fmean = ...), w = ~ weights)`
where we wish to aggregate some columns using the weighted mean, and
others using a simple sum or another unweighted statistic. Therefore it
is possible to append [Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
by `_uw` to yield an unweighted computation. So for the above example
one can specify:
`collap(data, ~ id, custom = list(fsum_uw = ..., fmean = ...), w = ~ weights)`
to get the weighted mean and the simple sum. *Note* that the `_uw`
functions are not available for use outside collap. Thus one also needs
to quote them when passing to the `FUN` or `catFUN` arguments, e.g. use
`collap(data, ~ id, fmean, "fmode_uw", w = ~ weights)`.

## Value

`X` aggregated. If `X` is not a data frame it is coerced to one using
[`qDF`](https://fastverse.org/collapse/reference/quick-conversion.md)
and then aggregated.

## See also

[`fsummarise`](https://fastverse.org/collapse/reference/fsummarise.md),
[`BY`](https://fastverse.org/collapse/reference/BY.md), [Fast
Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## A Simple Introduction --------------------------------------
head(iris)
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width Species
#> 1          5.1         3.5          1.4         0.2  setosa
#> 2          4.9         3.0          1.4         0.2  setosa
#> 3          4.7         3.2          1.3         0.2  setosa
#> 4          4.6         3.1          1.5         0.2  setosa
#> 5          5.0         3.6          1.4         0.2  setosa
#> 6          5.4         3.9          1.7         0.4  setosa
collap(iris, ~ Species)                                        # Default: FUN = fmean for numeric
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width    Species
#> 1        5.006       3.428        1.462       0.246     setosa
#> 2        5.936       2.770        4.260       1.326 versicolor
#> 3        6.588       2.974        5.552       2.026  virginica
collapv(iris, 5)                                               # Same using collapv
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width    Species
#> 1        5.006       3.428        1.462       0.246     setosa
#> 2        5.936       2.770        4.260       1.326 versicolor
#> 3        6.588       2.974        5.552       2.026  virginica
collap(iris, ~ Species, fmedian)                               # Using the median
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width    Species
#> 1          5.0         3.4         1.50         0.2     setosa
#> 2          5.9         2.8         4.35         1.3 versicolor
#> 3          6.5         3.0         5.55         2.0  virginica
collap(iris, ~ Species, fmedian, keep.col.order = FALSE)       # Groups in-front
#>      Species Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1     setosa          5.0         3.4         1.50         0.2
#> 2 versicolor          5.9         2.8         4.35         1.3
#> 3  virginica          6.5         3.0         5.55         2.0
collap(iris, Sepal.Width + Petal.Width ~ Species, fmedian)     # Only '.Width' columns
#>   Sepal.Width Petal.Width    Species
#> 1         3.4         0.2     setosa
#> 2         2.8         1.3 versicolor
#> 3         3.0         2.0  virginica
collapv(iris, 5, cols = c(2, 4))                               # Same using collapv
#>   Sepal.Width Petal.Width    Species
#> 1       3.428       0.246     setosa
#> 2       2.770       1.326 versicolor
#> 3       2.974       2.026  virginica
collap(iris, ~ Species, list(fmean, fmedian))                  # Two functions
#>   fmean.Sepal.Length fmedian.Sepal.Length fmean.Sepal.Width fmedian.Sepal.Width
#> 1              5.006                  5.0             3.428                 3.4
#> 2              5.936                  5.9             2.770                 2.8
#> 3              6.588                  6.5             2.974                 3.0
#>   fmean.Petal.Length fmedian.Petal.Length fmean.Petal.Width fmedian.Petal.Width
#> 1              1.462                 1.50             0.246                 0.2
#> 2              4.260                 4.35             1.326                 1.3
#> 3              5.552                 5.55             2.026                 2.0
#>      Species
#> 1     setosa
#> 2 versicolor
#> 3  virginica
collap(iris, ~ Species, list(fmean, fmedian), return = "long") # Long format
#>   Function Sepal.Length Sepal.Width Petal.Length Petal.Width    Species
#> 1    fmean        5.006       3.428        1.462       0.246     setosa
#> 2    fmean        5.936       2.770        4.260       1.326 versicolor
#> 3    fmean        6.588       2.974        5.552       2.026  virginica
#> 4  fmedian        5.000       3.400        1.500       0.200     setosa
#> 5  fmedian        5.900       2.800        4.350       1.300 versicolor
#> 6  fmedian        6.500       3.000        5.550       2.000  virginica
collapv(iris, 5, custom = list(fmean = 1:2, fmedian = 3:4))    # Custom aggregation
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width    Species
#> 1        5.006       3.428         1.50         0.2     setosa
#> 2        5.936       2.770         4.35         1.3 versicolor
#> 3        6.588       2.974         5.55         2.0  virginica
collapv(iris, 5, custom = list(fmean = 1:2, fmedian = 3:4),    # Raw output, no column reordering
        return = "list")
#> $fmean
#>      Species Sepal.Length Sepal.Width
#> 1     setosa        5.006       3.428
#> 2 versicolor        5.936       2.770
#> 3  virginica        6.588       2.974
#> 
#> $fmedian
#>      Species Petal.Length Petal.Width
#> 1     setosa         1.50         0.2
#> 2 versicolor         4.35         1.3
#> 3  virginica         5.55         2.0
#> 
collapv(iris, 5, custom = list(fmean = 1:2, fmedian = 3:4),    # A strange choice..
        return = "long")
#>   Function Sepal.Length Sepal.Width Petal.Length Petal.Width    Species
#> 1    fmean        5.006       3.428           NA          NA     setosa
#> 2    fmean        5.936       2.770           NA          NA versicolor
#> 3    fmean        6.588       2.974           NA          NA  virginica
#> 4  fmedian           NA          NA         1.50         0.2     setosa
#> 5  fmedian           NA          NA         4.35         1.3 versicolor
#> 6  fmedian           NA          NA         5.55         2.0  virginica
collap(iris, ~ Species, w = ~ Sepal.Length)                    # Using Sepal.Length as weights, ..
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width    Species
#> 1        250.3    3.447423     1.465202   0.2480224     setosa
#> 2        296.8    2.784063     4.290195   1.3352089 versicolor
#> 3        329.4    2.987948     5.597116   2.0333030  virginica
weights <- abs(rnorm(fnrow(iris)))
collap(iris, ~ Species, w = weights)                           # Some random weights..
#>    weights Sepal.Length Sepal.Width Petal.Length Petal.Width    Species
#> 1 49.80442     5.008467    3.439900     1.475742   0.2414042     setosa
#> 2 38.71629     5.918589    2.770690     4.235565   1.3112717 versicolor
#> 3 43.96431     6.530104    2.968984     5.520582   2.0136350  virginica
collap(iris, iris$Species, w = weights)                        # Note this behavior..
#>      Species  weights Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1     setosa 49.80442     5.008467    3.439900     1.475742   0.2414042
#> 2 versicolor 38.71629     5.918589    2.770690     4.235565   1.3112717
#> 3  virginica 43.96431     6.530104    2.968984     5.520582   2.0136350
#>      Species
#> 1     setosa
#> 2 versicolor
#> 3  virginica
collap(iris, iris$Species, w = weights,
       keep.by = FALSE, keep.w = FALSE)
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width    Species
#> 1     5.008467    3.439900     1.475742   0.2414042     setosa
#> 2     5.918589    2.770690     4.235565   1.3112717 versicolor
#> 3     6.530104    2.968984     5.520582   2.0136350  virginica



## Multi-Type Aggregation --------------------------------------
head(wlddev)                                                    # World Development Panel Data
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE    NA
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE    NA
#> 5 Afghanistan   AFG 1965-01-01 1964   1960 South Asia Low income FALSE    NA
#>   LIFEEX GINI       ODA     POP
#> 1 32.446   NA 116769997 8996973
#> 2 32.962   NA 232080002 9169410
#> 3 33.471   NA 112839996 9351441
#> 4 33.971   NA 237720001 9543205
#> 5 34.463   NA 295920013 9744781
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
head(collap(wlddev, ~ country + decade))                        # Aggregate by country and decade
#>       country iso3c       date   year decade     region     income  OECD
#> 1 Afghanistan   AFG 1961-01-01 1964.5   1960 South Asia Low income FALSE
#> 2 Afghanistan   AFG 1971-01-01 1974.5   1970 South Asia Low income FALSE
#> 3 Afghanistan   AFG 1981-01-01 1984.5   1980 South Asia Low income FALSE
#> 4 Afghanistan   AFG 1991-01-01 1994.5   1990 South Asia Low income FALSE
#> 5 Afghanistan   AFG 2001-01-01 2004.5   2000 South Asia Low income FALSE
#>     PCGDP  LIFEEX GINI        ODA      POP
#> 1      NA 34.6908   NA  222288999  9886773
#> 2      NA 39.9053   NA  236169998 12451803
#> 3      NA 46.4176   NA   71666001 12291854
#> 4      NA 53.0097   NA  317255000 16931903
#> 5 379.373 58.0881   NA 3054051961 24870022
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
head(collap(wlddev, ~ country + decade, fmedian, ffirst))       # Different functions
#>       country iso3c       date   year decade     region     income  OECD
#> 1 Afghanistan   AFG 1961-01-01 1964.5   1960 South Asia Low income FALSE
#> 2 Afghanistan   AFG 1971-01-01 1974.5   1970 South Asia Low income FALSE
#> 3 Afghanistan   AFG 1981-01-01 1984.5   1980 South Asia Low income FALSE
#> 4 Afghanistan   AFG 1991-01-01 1994.5   1990 South Asia Low income FALSE
#> 5 Afghanistan   AFG 2001-01-01 2004.5   2000 South Asia Low income FALSE
#>      PCGDP  LIFEEX GINI        ODA      POP
#> 1       NA 34.7055   NA  234900002  9850550
#> 2       NA 39.8430   NA  246509995 12551055
#> 3       NA 46.4005   NA   48539999 12071250
#> 4       NA 53.1200   NA  285175003 17593192
#> 5 361.2596 58.0310   NA 2984469971 25190480
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
head(collap(wlddev, ~ country + decade, cols = is.numeric))     # Aggregate only numeric columns
#>       country   year decade decade    PCGDP  LIFEEX GINI        ODA      POP
#> 1 Afghanistan 1964.5   1960   1960       NA 34.6908   NA  222288999  9886773
#> 2 Afghanistan 1974.5   1970   1970       NA 39.9053   NA  236169998 12451803
#> 3 Afghanistan 1984.5   1980   1980       NA 46.4176   NA   71666001 12291854
#> 4 Afghanistan 1994.5   1990   1990       NA 53.0097   NA  317255000 16931903
#> 5 Afghanistan 2004.5   2000   2000 379.3730 58.0881   NA 3054051961 24870022
#> 6 Afghanistan 2014.5   2010   2010 567.4047 63.0715   NA 5023859033 33741195
head(collap(wlddev, ~ country + decade, cols = 9:13))           # Only the 5 series
#>       country decade    PCGDP  LIFEEX GINI        ODA      POP
#> 1 Afghanistan   1960       NA 34.6908   NA  222288999  9886773
#> 2 Afghanistan   1970       NA 39.9053   NA  236169998 12451803
#> 3 Afghanistan   1980       NA 46.4176   NA   71666001 12291854
#> 4 Afghanistan   1990       NA 53.0097   NA  317255000 16931903
#> 5 Afghanistan   2000 379.3730 58.0881   NA 3054051961 24870022
#> 6 Afghanistan   2010 567.4047 63.0715   NA 5023859033 33741195
head(collap(wlddev, PCGDP + LIFEEX ~ country + decade))         # Only GDP and life-expactancy
#>       country decade    PCGDP  LIFEEX
#> 1 Afghanistan   1960       NA 34.6908
#> 2 Afghanistan   1970       NA 39.9053
#> 3 Afghanistan   1980       NA 46.4176
#> 4 Afghanistan   1990       NA 53.0097
#> 5 Afghanistan   2000 379.3730 58.0881
#> 6 Afghanistan   2010 567.4047 63.0715
head(collap(wlddev, PCGDP + LIFEEX ~ country + decade, fsum))   # Using the sum instead
#>       country decade    PCGDP  LIFEEX
#> 1 Afghanistan   1960       NA 346.908
#> 2 Afghanistan   1970       NA 399.053
#> 3 Afghanistan   1980       NA 464.176
#> 4 Afghanistan   1990       NA 530.097
#> 5 Afghanistan   2000 3034.984 580.881
#> 6 Afghanistan   2010 5674.047 630.715
head(collap(wlddev, PCGDP + LIFEEX ~ country + decade, sum,     # Same using base::sum -> slower!
            na.rm = TRUE))
#>       country decade    PCGDP  LIFEEX
#> 1 Afghanistan   1960    0.000 346.908
#> 2 Afghanistan   1970    0.000 399.053
#> 3 Afghanistan   1980    0.000 464.176
#> 4 Afghanistan   1990    0.000 530.097
#> 5 Afghanistan   2000 3034.984 580.881
#> 6 Afghanistan   2010 5674.047 630.715
head(collap(wlddev, wlddev[c("country","decade")], fsum,        # Same, exploring different inputs
            cols = 9:10))
#>       country decade    PCGDP  LIFEEX
#> 1 Afghanistan   1960       NA 346.908
#> 2 Afghanistan   1970       NA 399.053
#> 3 Afghanistan   1980       NA 464.176
#> 4 Afghanistan   1990       NA 530.097
#> 5 Afghanistan   2000 3034.984 580.881
#> 6 Afghanistan   2010 5674.047 630.715
head(collap(wlddev[9:10], wlddev[c("country","decade")], fsum))
#>       country decade    PCGDP  LIFEEX
#> 1 Afghanistan   1960       NA 346.908
#> 2 Afghanistan   1970       NA 399.053
#> 3 Afghanistan   1980       NA 464.176
#> 4 Afghanistan   1990       NA 530.097
#> 5 Afghanistan   2000 3034.984 580.881
#> 6 Afghanistan   2010 5674.047 630.715
head(collapv(wlddev, c("country","decade"), fsum))              # ..names/indices with collapv
#>       country iso3c       date  year decade     region     income  OECD
#> 1 Afghanistan   AFG 1961-01-01 19645   1960 South Asia Low income FALSE
#> 2 Afghanistan   AFG 1971-01-01 19745   1970 South Asia Low income FALSE
#> 3 Afghanistan   AFG 1981-01-01 19845   1980 South Asia Low income FALSE
#> 4 Afghanistan   AFG 1991-01-01 19945   1990 South Asia Low income FALSE
#> 5 Afghanistan   AFG 2001-01-01 20045   2000 South Asia Low income FALSE
#>      PCGDP  LIFEEX GINI         ODA       POP
#> 1       NA 346.908   NA  2222889992  98867731
#> 2       NA 399.053   NA  2361699982 124518028
#> 3       NA 464.176   NA   716660007 122918537
#> 4       NA 530.097   NA  3172550003 169319030
#> 5 3034.984 580.881   NA 30540519608 248700217
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
head(collapv(wlddev, c(1,5), fsum))
#>       country iso3c       date  year decade     region     income  OECD
#> 1 Afghanistan   AFG 1961-01-01 19645   1960 South Asia Low income FALSE
#> 2 Afghanistan   AFG 1971-01-01 19745   1970 South Asia Low income FALSE
#> 3 Afghanistan   AFG 1981-01-01 19845   1980 South Asia Low income FALSE
#> 4 Afghanistan   AFG 1991-01-01 19945   1990 South Asia Low income FALSE
#> 5 Afghanistan   AFG 2001-01-01 20045   2000 South Asia Low income FALSE
#>      PCGDP  LIFEEX GINI         ODA       POP
#> 1       NA 346.908   NA  2222889992  98867731
#> 2       NA 399.053   NA  2361699982 124518028
#> 3       NA 464.176   NA   716660007 122918537
#> 4       NA 530.097   NA  3172550003 169319030
#> 5 3034.984 580.881   NA 30540519608 248700217
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

g <- GRP(wlddev, ~ country + decade)                            # Precomputing the grouping
head(collap(wlddev, g, keep.by = FALSE))                        # This is slightly faster now
#>       country iso3c       date   year decade     region     income  OECD
#> 1 Afghanistan   AFG 1961-01-01 1964.5   1960 South Asia Low income FALSE
#> 2 Afghanistan   AFG 1971-01-01 1974.5   1970 South Asia Low income FALSE
#> 3 Afghanistan   AFG 1981-01-01 1984.5   1980 South Asia Low income FALSE
#> 4 Afghanistan   AFG 1991-01-01 1994.5   1990 South Asia Low income FALSE
#> 5 Afghanistan   AFG 2001-01-01 2004.5   2000 South Asia Low income FALSE
#>     PCGDP  LIFEEX GINI        ODA      POP
#> 1      NA 34.6908   NA  222288999  9886773
#> 2      NA 39.9053   NA  236169998 12451803
#> 3      NA 46.4176   NA   71666001 12291854
#> 4      NA 53.0097   NA  317255000 16931903
#> 5 379.373 58.0881   NA 3054051961 24870022
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
# Aggregate categorical data using not the mode but the last element
head(collap(wlddev, ~ country + decade, fmean, flast))
#>       country iso3c       date   year decade     region     income  OECD
#> 1 Afghanistan   AFG 1970-01-01 1964.5   1960 South Asia Low income FALSE
#> 2 Afghanistan   AFG 1980-01-01 1974.5   1970 South Asia Low income FALSE
#> 3 Afghanistan   AFG 1990-01-01 1984.5   1980 South Asia Low income FALSE
#> 4 Afghanistan   AFG 2000-01-01 1994.5   1990 South Asia Low income FALSE
#> 5 Afghanistan   AFG 2010-01-01 2004.5   2000 South Asia Low income FALSE
#>     PCGDP  LIFEEX GINI        ODA      POP
#> 1      NA 34.6908   NA  222288999  9886773
#> 2      NA 39.9053   NA  236169998 12451803
#> 3      NA 46.4176   NA   71666001 12291854
#> 4      NA 53.0097   NA  317255000 16931903
#> 5 379.373 58.0881   NA 3054051961 24870022
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
head(collap(wlddev, ~ country + decade, catFUN = flast,         # Aggregate only categorical data
            cols = is_categorical))
#>       country     country iso3c       date decade     region     income  OECD
#> 1 Afghanistan Afghanistan   AFG 1970-01-01   1960 South Asia Low income FALSE
#> 2 Afghanistan Afghanistan   AFG 1980-01-01   1970 South Asia Low income FALSE
#> 3 Afghanistan Afghanistan   AFG 1990-01-01   1980 South Asia Low income FALSE
#> 4 Afghanistan Afghanistan   AFG 2000-01-01   1990 South Asia Low income FALSE
#> 5 Afghanistan Afghanistan   AFG 2010-01-01   2000 South Asia Low income FALSE
#> 6 Afghanistan Afghanistan   AFG 2020-01-01   2010 South Asia Low income FALSE


## Weighted Aggregation ----------------------------------------
# We aggregate to region level using population weights
head(collap(wlddev, ~ region + year, w = ~ POP))                # Takes weighted mean for numeric..
#>   country iso3c       date year decade              region              income
#> 1   China   CHN 1961-01-01 1960   1960 East Asia & Pacific Upper middle income
#> 2   China   CHN 1962-01-01 1961   1960 East Asia & Pacific Upper middle income
#> 3   China   CHN 1963-01-01 1962   1960 East Asia & Pacific Upper middle income
#> 4   China   CHN 1964-01-01 1963   1960 East Asia & Pacific Upper middle income
#> 5   China   CHN 1965-01-01 1964   1960 East Asia & Pacific Upper middle income
#>    OECD    PCGDP   LIFEEX GINI       ODA        POP
#> 1 FALSE 1313.760 48.20996   NA 764164132 1018832214
#> 2 FALSE 1395.228 48.73451   NA 774544481 1021806689
#> 3 FALSE 1463.441 49.39960   NA 915939856 1035694621
#> 4 FALSE 1540.621 50.37529   NA 748978431 1060888744
#> 5 FALSE 1665.385 51.57330   NA 619226983 1085690423
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
# ..and weighted mode for categorical data. The weight vector is aggregated using fsum

head(collap(wlddev, ~ region + year, w = ~ POP,                 # Aggregating weights using sum
            wFUN = list(sum = fsum, max = fmax)))               # and max (corresponding to mode)
#>   country iso3c       date year decade              region              income
#> 1   China   CHN 1961-01-01 1960   1960 East Asia & Pacific Upper middle income
#> 2   China   CHN 1962-01-01 1961   1960 East Asia & Pacific Upper middle income
#> 3   China   CHN 1963-01-01 1962   1960 East Asia & Pacific Upper middle income
#> 4   China   CHN 1964-01-01 1963   1960 East Asia & Pacific Upper middle income
#> 5   China   CHN 1965-01-01 1964   1960 East Asia & Pacific Upper middle income
#>    OECD    PCGDP   LIFEEX GINI       ODA    sum.POP   max.POP
#> 1 FALSE 1313.760 48.20996   NA 764164132 1018832214 667070000
#> 2 FALSE 1395.228 48.73451   NA 774544481 1021806689 660330000
#> 3 FALSE 1463.441 49.39960   NA 915939856 1035694621 665770000
#> 4 FALSE 1540.621 50.37529   NA 748978431 1060888744 682335000
#> 5 FALSE 1665.385 51.57330   NA 619226983 1085690423 698355000
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]


## Multi-Function Aggregation ----------------------------------
head(collap(wlddev, ~ country + decade, list(mean = fmean, N = fnobs),  # Saving mean and Nobs
            cols = 9:13))
#>       country decade mean.PCGDP N.PCGDP mean.LIFEEX N.LIFEEX mean.GINI N.GINI
#> 1 Afghanistan   1960         NA       0     34.6908       10        NA      0
#> 2 Afghanistan   1970         NA       0     39.9053       10        NA      0
#> 3 Afghanistan   1980         NA       0     46.4176       10        NA      0
#> 4 Afghanistan   1990         NA       0     53.0097       10        NA      0
#> 5 Afghanistan   2000    379.373       8     58.0881       10        NA      0
#>     mean.ODA N.ODA mean.POP N.POP
#> 1  222288999    10  9886773    10
#> 2  236169998    10 12451803    10
#> 3   71666001    10 12291854    10
#> 4  317255000    10 16931903    10
#> 5 3054051961    10 24870022    10
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

head(collap(wlddev, ~ country + decade,                         # Same using base R -> slower
            list(mean = mean,
                 N = function(x, ...) sum(!is.na(x))),
            cols = 9:13, na.rm = TRUE))
#>       country decade mean.PCGDP N.PCGDP mean.LIFEEX N.LIFEEX mean.GINI N.GINI
#> 1 Afghanistan   1960        NaN       0     34.6908       10       NaN      0
#> 2 Afghanistan   1970        NaN       0     39.9053       10       NaN      0
#> 3 Afghanistan   1980        NaN       0     46.4176       10       NaN      0
#> 4 Afghanistan   1990        NaN       0     53.0097       10       NaN      0
#> 5 Afghanistan   2000    379.373       8     58.0881       10       NaN      0
#>     mean.ODA N.ODA mean.POP N.POP
#> 1  222288999    10  9886773    10
#> 2  236169998    10 12451803    10
#> 3   71666001    10 12291854    10
#> 4  317255000    10 16931903    10
#> 5 3054051961    10 24870022    10
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

lapply(collap(wlddev, ~ country + decade,                       # List output format
       list(mean = fmean, N = fnobs), cols = 9:13, return = "list"), head)
#> $mean
#>       country decade    PCGDP  LIFEEX GINI        ODA      POP
#> 1 Afghanistan   1960       NA 34.6908   NA  222288999  9886773
#> 2 Afghanistan   1970       NA 39.9053   NA  236169998 12451803
#> 3 Afghanistan   1980       NA 46.4176   NA   71666001 12291854
#> 4 Afghanistan   1990       NA 53.0097   NA  317255000 16931903
#> 5 Afghanistan   2000 379.3730 58.0881   NA 3054051961 24870022
#> 6 Afghanistan   2010 567.4047 63.0715   NA 5023859033 33741195
#> 
#> $N
#>       country decade PCGDP LIFEEX GINI ODA POP
#> 1 Afghanistan   1960     0     10    0  10  10
#> 2 Afghanistan   1970     0     10    0  10  10
#> 3 Afghanistan   1980     0     10    0  10  10
#> 4 Afghanistan   1990     0     10    0  10  10
#> 5 Afghanistan   2000     8     10    0  10  10
#> 6 Afghanistan   2010    10     10    0  10  10
#> 

head(collap(wlddev, ~ country + decade,                         # Long output format
     list(mean = fmean, N = fnobs), cols = 9:13, return = "long"))
#>   Function     country decade    PCGDP  LIFEEX GINI        ODA      POP
#> 1     mean Afghanistan   1960       NA 34.6908   NA  222288999  9886773
#> 2     mean Afghanistan   1970       NA 39.9053   NA  236169998 12451803
#> 3     mean Afghanistan   1980       NA 46.4176   NA   71666001 12291854
#> 4     mean Afghanistan   1990       NA 53.0097   NA  317255000 16931903
#> 5     mean Afghanistan   2000 379.3730 58.0881   NA 3054051961 24870022
#> 6     mean Afghanistan   2010 567.4047 63.0715   NA 5023859033 33741195

head(collap(wlddev, ~ country + decade,                         # Also aggregating categorical data,
     list(mean = fmean, N = fnobs), return = "long_dupl"))      # and duplicating it 2 times
#>   Function     country iso3c       date   year decade     region     income
#> 1     mean Afghanistan   AFG 1961-01-01 1964.5   1960 South Asia Low income
#> 2     mean Afghanistan   AFG 1971-01-01 1974.5   1970 South Asia Low income
#> 3     mean Afghanistan   AFG 1981-01-01 1984.5   1980 South Asia Low income
#> 4     mean Afghanistan   AFG 1991-01-01 1994.5   1990 South Asia Low income
#> 5     mean Afghanistan   AFG 2001-01-01 2004.5   2000 South Asia Low income
#>    OECD   PCGDP  LIFEEX GINI        ODA      POP
#> 1 FALSE      NA 34.6908   NA  222288999  9886773
#> 2 FALSE      NA 39.9053   NA  236169998 12451803
#> 3 FALSE      NA 46.4176   NA   71666001 12291854
#> 4 FALSE      NA 53.0097   NA  317255000 16931903
#> 5 FALSE 379.373 58.0881   NA 3054051961 24870022
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

head(collap(wlddev, ~ country + decade,                         # Now also using 2 functions on
     list(mean = fmean, N = fnobs), list(mode = fmode, last = flast),   # categorical data
            keep.col.order = FALSE))
#>       country decade mean.year mean.PCGDP mean.LIFEEX mean.GINI  mean.ODA
#> 1 Afghanistan   1960    1964.5         NA     34.6908        NA 222288999
#> 2 Afghanistan   1970    1974.5         NA     39.9053        NA 236169998
#>   mean.POP N.year N.PCGDP N.LIFEEX N.GINI N.ODA N.POP mode.iso3c  mode.date
#> 1  9886773     10       0       10      0    10    10        AFG 1961-01-01
#> 2 12451803     10       0       10      0    10    10        AFG 1971-01-01
#>   mode.region mode.income mode.OECD last.iso3c  last.date last.region
#> 1  South Asia  Low income     FALSE        AFG 1970-01-01  South Asia
#> 2  South Asia  Low income     FALSE        AFG 1980-01-01  South Asia
#>   last.income last.OECD
#> 1  Low income     FALSE
#> 2  Low income     FALSE
#>  [ reached 'max' / getOption("max.print") -- omitted 4 rows ]

head(collap(wlddev, ~ country + decade,                         # More functions, string input,
            c("fmean","fsum","fnobs","fsd","fvar"),             # parallelized execution
            c("fmode","ffirst","flast","fndistinct"),           # (choose more than 1 cores,
            parallel = TRUE, mc.cores = 1L,                     # depending on your machine)
            keep.col.order = FALSE))
#>       country decade fmean.year fmean.PCGDP fmean.LIFEEX fmean.GINI fmean.ODA
#> 1 Afghanistan   1960     1964.5          NA      34.6908         NA 222288999
#>   fmean.POP fsum.year fsum.PCGDP fsum.LIFEEX fsum.GINI   fsum.ODA fsum.POP
#> 1   9886773     19645         NA     346.908        NA 2222889992 98867731
#>   fnobs.year fnobs.PCGDP fnobs.LIFEEX fnobs.GINI fnobs.ODA fnobs.POP fsd.year
#> 1         10           0           10          0        10        10  3.02765
#>   fsd.PCGDP fsd.LIFEEX fsd.GINI  fsd.ODA  fsd.POP fvar.year fvar.PCGDP
#> 1        NA   1.490964       NA 80884369 637640.4  9.166667         NA
#>   fvar.LIFEEX fvar.GINI     fvar.ODA     fvar.POP fmode.iso3c fmode.date
#> 1    2.222975        NA 6.542281e+15 406585329709         AFG 1961-01-01
#>   fmode.region fmode.income fmode.OECD ffirst.iso3c ffirst.date ffirst.region
#> 1   South Asia   Low income      FALSE          AFG  1961-01-01    South Asia
#>   ffirst.income ffirst.OECD flast.iso3c flast.date flast.region flast.income
#> 1    Low income       FALSE         AFG 1970-01-01   South Asia   Low income
#>   flast.OECD fndistinct.iso3c fndistinct.date fndistinct.region
#> 1      FALSE                1              10                 1
#>   fndistinct.income fndistinct.OECD
#> 1                 1               1
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]


## Custom Aggregation ------------------------------------------
head(collap(wlddev, ~ country + decade,                         # Custom aggregation
            custom = list(fmean = 11:13, fsd = 9:10, fmode = 7:8)))
#>       country decade     income  OECD    PCGDP   LIFEEX GINI        ODA
#> 1 Afghanistan   1960 Low income FALSE       NA 1.490964   NA  222288999
#> 2 Afghanistan   1970 Low income FALSE       NA 1.738383   NA  236169998
#> 3 Afghanistan   1980 Low income FALSE       NA 2.161460   NA   71666001
#> 4 Afghanistan   1990 Low income FALSE       NA 1.695424   NA  317255000
#> 5 Afghanistan   2000 Low income FALSE 53.66524 1.565630   NA 3054051961
#> 6 Afghanistan   2010 Low income FALSE 18.07999 1.274644   NA 5023859033
#>        POP
#> 1  9886773
#> 2 12451803
#> 3 12291854
#> 4 16931903
#> 5 24870022
#> 6 33741195

head(collap(wlddev, ~ country + decade,                         # Using column names
            custom = list(fmean = "PCGDP", fsd = c("LIFEEX","GINI"),
                          flast = "date")))
#>       country       date decade    PCGDP   LIFEEX GINI
#> 1 Afghanistan 1970-01-01   1960       NA 1.490964   NA
#> 2 Afghanistan 1980-01-01   1970       NA 1.738383   NA
#> 3 Afghanistan 1990-01-01   1980       NA 2.161460   NA
#> 4 Afghanistan 2000-01-01   1990       NA 1.695424   NA
#> 5 Afghanistan 2010-01-01   2000 379.3730 1.565630   NA
#> 6 Afghanistan 2020-01-01   2010 567.4047 1.274644   NA

head(collap(wlddev, ~ country + decade,                         # Weighted parallelized custom
            custom = list(fmean = 9:12, fsd = 9:10,             # aggregation
                          fmode = 7:8), w = ~ POP,
            wFUN = list(fsum, fmax),
            parallel = TRUE, mc.cores = 1L))
#>       country decade fmode.income fmode.OECD fmean.PCGDP fsd.PCGDP fmean.LIFEEX
#> 1 Afghanistan   1960   Low income      FALSE          NA        NA     34.77716
#> 2 Afghanistan   1970   Low income      FALSE          NA        NA     40.00367
#> 3 Afghanistan   1980   Low income      FALSE          NA        NA     46.32098
#> 4 Afghanistan   1990   Low income      FALSE          NA        NA     53.25897
#> 5 Afghanistan   2000   Low income      FALSE    382.5583  51.11423     58.23630
#>   fsd.LIFEEX fmean.GINI  fmean.ODA  fsum.POP fmax.POP
#> 1   1.413369         NA  223006447  98867731 10893776
#> 2   1.643608         NA  236798314 124518028 13411056
#> 3   2.061847         NA   70613923 122918537 13356511
#> 4   1.556185         NA  306818649 169319030 20170844
#> 5   1.476279         NA 3240143310 248700217 28394813
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

head(collap(wlddev, ~ country + decade,                         # No column reordering
            custom = list(fmean = 9:12, fsd = 9:10,
                          fmode = 7:8), w = ~ POP,
            wFUN = list(fsum, fmax),
            parallel = TRUE, mc.cores = 1L, keep.col.order = FALSE))
#>       country decade  fsum.POP fmax.POP fmean.PCGDP fmean.LIFEEX fmean.GINI
#> 1 Afghanistan   1960  98867731 10893776          NA     34.77716         NA
#> 2 Afghanistan   1970 124518028 13411056          NA     40.00367         NA
#> 3 Afghanistan   1980 122918537 13356511          NA     46.32098         NA
#> 4 Afghanistan   1990 169319030 20170844          NA     53.25897         NA
#> 5 Afghanistan   2000 248700217 28394813    382.5583     58.23630         NA
#>    fmean.ODA fsd.PCGDP fsd.LIFEEX fmode.income fmode.OECD
#> 1  223006447        NA   1.413369   Low income      FALSE
#> 2  236798314        NA   1.643608   Low income      FALSE
#> 3   70613923        NA   2.061847   Low income      FALSE
#> 4  306818649        NA   1.556185   Low income      FALSE
#> 5 3240143310  51.11423   1.476279   Low income      FALSE
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

## Piped Use --------------------------------------------------
iris |> fgroup_by(Species) |> collapg()
#>      Species Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1     setosa        5.006       3.428        1.462       0.246
#> 2 versicolor        5.936       2.770        4.260       1.326
#> 3  virginica        6.588       2.974        5.552       2.026
wlddev |> fgroup_by(country, decade) |> collapg() |> head()
#>       country decade iso3c       date   year     region     income  OECD
#> 1 Afghanistan   1960   AFG 1961-01-01 1964.5 South Asia Low income FALSE
#> 2 Afghanistan   1970   AFG 1971-01-01 1974.5 South Asia Low income FALSE
#> 3 Afghanistan   1980   AFG 1981-01-01 1984.5 South Asia Low income FALSE
#> 4 Afghanistan   1990   AFG 1991-01-01 1994.5 South Asia Low income FALSE
#> 5 Afghanistan   2000   AFG 2001-01-01 2004.5 South Asia Low income FALSE
#>     PCGDP  LIFEEX GINI        ODA      POP
#> 1      NA 34.6908   NA  222288999  9886773
#> 2      NA 39.9053   NA  236169998 12451803
#> 3      NA 46.4176   NA   71666001 12291854
#> 4      NA 53.0097   NA  317255000 16931903
#> 5 379.373 58.0881   NA 3054051961 24870022
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
wlddev |> fgroup_by(region, year) |> collapg(w = POP) |> head()
#>                region year        POP country iso3c       date decade
#> 1 East Asia & Pacific 1960 1018832214   China   CHN 1961-01-01   1960
#> 2 East Asia & Pacific 1961 1021806689   China   CHN 1962-01-01   1960
#> 3 East Asia & Pacific 1962 1035694621   China   CHN 1963-01-01   1960
#> 4 East Asia & Pacific 1963 1060888744   China   CHN 1964-01-01   1960
#> 5 East Asia & Pacific 1964 1085690423   China   CHN 1965-01-01   1960
#>                income  OECD    PCGDP   LIFEEX GINI       ODA
#> 1 Upper middle income FALSE 1313.760 48.20996   NA 764164132
#> 2 Upper middle income FALSE 1395.228 48.73451   NA 774544481
#> 3 Upper middle income FALSE 1463.441 49.39960   NA 915939856
#> 4 Upper middle income FALSE 1540.621 50.37529   NA 748978431
#> 5 Upper middle income FALSE 1665.385 51.57330   NA 619226983
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
wlddev |> fgroup_by(country, decade) |> collapg(fmedian, flast) |> head()
#>       country decade iso3c       date   year     region     income  OECD
#> 1 Afghanistan   1960   AFG 1970-01-01 1964.5 South Asia Low income FALSE
#> 2 Afghanistan   1970   AFG 1980-01-01 1974.5 South Asia Low income FALSE
#> 3 Afghanistan   1980   AFG 1990-01-01 1984.5 South Asia Low income FALSE
#> 4 Afghanistan   1990   AFG 2000-01-01 1994.5 South Asia Low income FALSE
#> 5 Afghanistan   2000   AFG 2010-01-01 2004.5 South Asia Low income FALSE
#>      PCGDP  LIFEEX GINI        ODA      POP
#> 1       NA 34.7055   NA  234900002  9850550
#> 2       NA 39.8430   NA  246509995 12551055
#> 3       NA 46.4005   NA   48539999 12071250
#> 4       NA 53.1200   NA  285175003 17593192
#> 5 361.2596 58.0310   NA 2984469971 25190480
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
wlddev |> fgroup_by(country, decade) |>
  collapg(custom = list(fmean = 9:12, fmode = 5:7, flast = 3)) |> head()
#>       country decade       date decade     region     income    PCGDP  LIFEEX
#> 1 Afghanistan   1960 1970-01-01   1960 South Asia Low income       NA 34.6908
#> 2 Afghanistan   1970 1980-01-01   1970 South Asia Low income       NA 39.9053
#> 3 Afghanistan   1980 1990-01-01   1980 South Asia Low income       NA 46.4176
#> 4 Afghanistan   1990 2000-01-01   1990 South Asia Low income       NA 53.0097
#> 5 Afghanistan   2000 2010-01-01   2000 South Asia Low income 379.3730 58.0881
#> 6 Afghanistan   2010 2020-01-01   2010 South Asia Low income 567.4047 63.0715
#>   GINI        ODA
#> 1   NA  222288999
#> 2   NA  236169998
#> 3   NA   71666001
#> 4   NA  317255000
#> 5   NA 3054051961
#> 6   NA 5023859033
```
