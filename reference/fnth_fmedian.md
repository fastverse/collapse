# Fast (Grouped, Weighted) N'th Element/Quantile for Matrix-Like Objects

`fnth` (column-wise) returns the n'th smallest element from a set of
unsorted elements `x` corresponding to an integer index (`n`), or to a
probability between 0 and 1. If `n` is passed as a probability, ties can
be resolved using the lower, upper, or average of the possible elements,
or (default) continuous quantile estimation. For `n > 1`, the lower
element is always returned (as in `sort(x, partial = n)[n]`). See
Details.

`fmedian` is a simple wrapper around `fnth`, which fixes `n = 0.5` and
(default) `ties = "mean"`, i.e., it averages eligible elements. See
Details.

## Usage

``` r
fnth(x, n = 0.5, ...)
fmedian(x, ...)

# Default S3 method
fnth(x, n = 0.5, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, ties = "q7", nthreads = .op[["nthreads"]],
     o = NULL, check.o = is.null(attr(o, "sorted")), ...)
# Default S3 method
fmedian(x, ..., ties = "mean")

# S3 method for class 'matrix'
fnth(x, n = 0.5, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, drop = TRUE, ties = "q7", nthreads = .op[["nthreads"]], ...)
# S3 method for class 'matrix'
fmedian(x, ..., ties = "mean")

# S3 method for class 'data.frame'
fnth(x, n = 0.5, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, drop = TRUE, ties = "q7", nthreads = .op[["nthreads"]], ...)
# S3 method for class 'data.frame'
fmedian(x, ..., ties = "mean")

# S3 method for class 'grouped_df'
fnth(x, n = 0.5, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = FALSE, keep.group_vars = TRUE, keep.w = TRUE, stub = .op[["stub"]],
     ties = "q7", nthreads = .op[["nthreads"]], ...)
# S3 method for class 'grouped_df'
fmedian(x, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
        use.g.names = FALSE, keep.group_vars = TRUE, keep.w = TRUE, stub = .op[["stub"]],
        ties = "mean", nthreads = .op[["nthreads"]], ...)
```

## Arguments

- x:

  a numeric vector, matrix, data frame or grouped data frame (class
  'grouped_df').

- n:

  the element to return using a single integer index such that
  `1 < n < NROW(x)`, or a probability `0 < n < 1`. See Details.

- g:

  a factor, [`GRP`](https://fastverse.org/collapse/reference/GRP.md)
  object, atomic vector (internally converted to factor) or a list of
  vectors / factors (internally converted to a
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object) used
  to group `x`.

- w:

  a numeric vector of (non-negative) weights, may contain missing values
  only where `x` is also missing.

- TRA:

  an integer or quoted operator indicating the transformation to
  perform: 0 - "na" \| 1 - "fill" \| 2 - "replace" \| 3 - "-" \| 4 -
  "-+" \| 5 - "/" \| 6 - "%" \| 7 - "+" \| 8 - "\*" \| 9 - "%%" \| 10 -
  "-%%". See [`TRA`](https://fastverse.org/collapse/reference/TRA.md).

- na.rm:

  logical. Skip missing values in `x`. Defaults to `TRUE` and
  implemented at very little computational cost. If `na.rm = FALSE` a
  `NA` is returned when encountered.

- use.g.names:

  logical. Make group-names and add to the result as names (default
  method) or row-names (matrix and data frame methods). No row-names are
  generated for *data.table*'s.

- ties:

  an integer or character string specifying the method to resolve ties
  between adjacent qualifying elements:

  |  |  |  |  |  |
  |----|----|----|----|----|
  | *Int.* |  | *String* |  | *Description* |
  | 1 |  | "mean" |  | take the arithmetic mean of all qualifying elements. |
  | 2 |  | "min" |  | take the smallest of the elements. |
  | 3 |  | "max" |  | take the largest of the elements. |
  | 4-9 |  | "qn" |  | continuous quantile types 4-9, see [`fquantile`](https://fastverse.org/collapse/reference/fquantile.md). |

- nthreads:

  integer. The number of threads to utilize. Parallelism is across
  groups for grouped computations on vectors and data frames, and at the
  column-level otherwise. See Details.

- o:

  integer. A valid ordering of `x`, e.g. `radixorder(x)`. With groups,
  the grouping needs to be accounted e.g. `radixorder(g, x)`.

- check.o:

  logical. `TRUE` checks that each element of `o` is within
  `[1, length(x)]`. The default uses the fact that orderings from
  [`radixorder`](https://fastverse.org/collapse/reference/radixorder.md)
  have a `"sorted"` attribute which let's `fnth` infer that the ordering
  is valid. The length and data type of `o` is always checked,
  regardless of `check.o`.

- drop:

  *matrix and data.frame method:* Logical. `TRUE` drops dimensions and
  returns an atomic vector if `g = NULL` and `TRA = NULL`.

- keep.group_vars:

  *grouped_df method:* Logical. `FALSE` removes grouping variables after
  computation.

- keep.w:

  *grouped_df method:* Logical. Retain `sum` of weighting variable after
  computation (if contained in `grouped_df`).

- stub:

  character. If `keep.w = TRUE` and `stub = TRUE` (default), the summed
  weights column is prefixed by `"sum."`. Users can specify a different
  prefix through this argument, or set it to `FALSE` to avoid prefixing.

- ...:

  for `fmedian`: further arguments passed to `fnth` (apart from `n`). If
  `TRA` is used, passing `set = TRUE` will transform data by reference
  and return the result invisibly.

## Details

`fnth` uses a combination of quickselect, quicksort, and radixsort
algorithms, combined with several (weighted) quantile estimation methods
and, where possible, OpenMP multithreading:

- without weights, quickselect is used to determine a (lower) order
  statistic. If `ties %!in% c("min", "max")` a second order statistic is
  found by taking the max of the upper part of the partitioned array,
  and the two statistics are averaged using a simple mean
  (`ties = "mean"`), or weighted average according to a
  [`quantile`](https://rdrr.io/r/stats/quantile.html) method
  (`ties = "q4"-"q9"`). For `n = 0.5`, all supported quantile methods
  give the sample median. With matrices, multithreading is always across
  columns, for vectors and data frames it is across groups unless
  `is.null(g)` for data frames.

- with weights and no groups (`is.null(g)`),
  [`radixorder`](https://fastverse.org/collapse/reference/radixorder.md)
  is called internally (on each column of `x`). The ordering is used to
  sum the weights in order of `x` and determine weighted order
  statistics or quantiles. See details below. Multithreading is disabled
  as
  [`radixorder`](https://fastverse.org/collapse/reference/radixorder.md)
  cannot be called concurrently on the same memory stack.

- with weights and groups (`!is.null(g)`), R's quicksort algorithm is
  used to sort the data in each group and return an index which can be
  used to sum the weights in order and proceed as before. This is
  multithreaded across columns for matrices, and across groups
  otherwise.

- in `fnth.default`, an ordering of `x` can be supplied to '`o`' e.g.
  `fnth(x, 0.75, o = radixorder(x))`. This dramatically speeds up the
  estimation both with and without weights, and is useful if `fnth` is
  to be invoked repeatedly on the same data. With groups, `o` needs to
  also account for the grouping e.g.
  `fnth(x, 0.75, g, o = radixorder(g, x))`. Multithreading is possible
  across groups. See Examples.

If `n > 1`, the result is equivalent to (column-wise)
`sort(x, partial = n)[n]`. Internally, `n` is converted to a probability
using `p = (n-1)/(NROW(x)-1)`, and that probability is applied to the
set of non-missing elements to find the
`as.integer(p*(fnobs(x)-1))+1L`'th element (which corresponds to option
`ties = "min"`). When using grouped computations with `n > 1`, `n` is
transformed to a probability `p = (n-1)/(NROW(x)/ng-1)` (where `ng`
contains the number of unique groups in `g`).

If weights are used and `ties = "q4"-"q9"`, weighted continuous quantile
estimation is done as described in
[`fquantile`](https://fastverse.org/collapse/reference/fquantile.md).

For `ties %in% c("mean", "min", "max")`, a target partial sum of weights
`p*sum(w)` is calculated, and the weighted n'th element is the element k
such that all elements smaller than k have a sum of weights
`<= p*sum(w)`, and all elements larger than k have a sum of weights
`<= (1 - p)*sum(w)`. If the partial-sum of weights (`p*sum(w)`) is
reached exactly for some element k, then (summing from the lower end)
both k and k+1 would qualify as the weighted n'th element. If the weight
of element k+1 is zero, k, k+1 and k+2 would qualify... . If `n > 1`, k
is chosen (consistent with the unweighted behavior). If `0 < n < 1`, the
`ties` option regulates how to resolve such conflicts, yielding lower
(`ties = "min"`: k), upper (`ties = "max"`: k+2) or average weighted
(`ties = "mean"`: mean(k, k+1, k+2)) n'th elements.

Thus, in the presence of zero weights, the weighted median (default
`ties = "mean"`) can be an arithmetic average of \>2 qualifying
elements.

For data frames, column-attributes and overall attributes are preserved
if `g` is used or `drop = FALSE`.

## Value

The (`w` weighted) n'th element/quantile of `x`, grouped by `g`, or (if
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) is used) `x`
transformed by its (grouped, weighted) n'th element/quantile.

## See also

[`fquantile`](https://fastverse.org/collapse/reference/fquantile.md),
[`fmean`](https://fastverse.org/collapse/reference/fmean.md),
[`fmode`](https://fastverse.org/collapse/reference/fmode.md), [Fast
Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## default vector method
mpg <- mtcars$mpg
fnth(mpg)                         # Simple nth element: Median (same as fmedian(mpg))
#> [1] 19.2
fnth(mpg, 5)                      # 5th smallest element
#> [1] 14.7
sort(mpg, partial = 5)[5]         # Same using base R, fnth is 2x faster.
#> [1] 14.7
fnth(mpg, 0.75)                   # Third quartile
#> [1] 22.8
fnth(mpg, 0.75, w = mtcars$hp)    # Weighted third quartile: Weighted by hp
#> [1] 20.79909
fnth(mpg, 0.75, TRA = "-")        # Simple transformation: Subtract third quartile
#>  [1]  -1.8  -1.8   0.0  -1.4  -4.1  -4.7  -8.5   1.6   0.0  -3.6  -5.0  -6.4
#> [13]  -5.5  -7.6 -12.4 -12.4  -8.1   9.6   7.6  11.1  -1.3  -7.3  -7.6  -9.5
#> [25]  -3.6   4.5   3.2   7.6  -7.0  -3.1  -7.8  -1.4
fnth(mpg, 0.75, mtcars$cyl)             # Grouped third quartile
#>     4     6     8 
#> 30.40 21.00 16.25 
fnth(mpg, 0.75, mtcars[c(2,8:9)])       # More groups..
#>  4.0.1  4.1.0  4.1.1  6.0.1  6.1.0  8.0.0  8.0.1 
#> 26.000 23.600 31.400 21.000 19.750 16.625 15.600 
g <- GRP(mtcars, ~ cyl + vs + am)       # Precomputing groups gives more speed !
fnth(mpg, 0.75, g)
#>  4.0.1  4.1.0  4.1.1  6.0.1  6.1.0  8.0.0  8.0.1 
#> 26.000 23.600 31.400 21.000 19.750 16.625 15.600 
fnth(mpg, 0.75, g, mtcars$hp)           # Grouped weighted third quartile
#>    4.0.1    4.1.0    4.1.1    6.0.1    6.1.0    8.0.0    8.0.1 
#> 26.00000 23.17474 30.51538 21.00000 19.65610 16.36250 15.54621 
fnth(mpg, 0.75, g, TRA = "-")           # Groupwise subtract third quartile
#>  [1]   0.000   0.000  -8.600   1.650   2.075  -1.650  -2.325   0.800  -0.800
#> [10]  -0.550  -1.950  -0.225   0.675  -1.425  -6.225  -6.225  -1.925   1.000
#> [19]  -1.000   2.500  -2.100  -1.125  -1.425  -3.325   2.575  -4.100   0.000
#> [28]  -1.000   0.200  -1.300  -0.600 -10.000
fnth(mpg, 0.75, g, mtcars$hp, "-")      # Groupwise subtract weighted third quartile
#>  [1]  0.0000000  0.0000000 -7.7153846  1.7439024  2.3375000 -1.5560976
#>  [7] -2.0625000  1.2252632 -0.3747368 -0.4560976 -1.8560976  0.0375000
#> [13]  0.9375000 -1.1625000 -5.9625000 -5.9625000 -1.6625000  1.8846154
#> [19] -0.1153846  3.3846154 -1.6747368 -0.8625000 -1.1625000 -3.0625000
#> [25]  2.8375000 -3.2153846  0.0000000 -0.1153846  0.2537879 -1.3000000
#> [31] -0.5462121 -9.1153846

## data.frame method
fnth(mtcars, 0.75)
#>    mpg    cyl   disp     hp   drat     wt   qsec     vs     am   gear   carb 
#>  22.80   8.00 326.00 180.00   3.92   3.61  18.90   1.00   1.00   4.00   4.00 
head(fnth(mtcars, 0.75, TRA = "-"))
#>                    mpg cyl disp  hp  drat     wt  qsec vs am gear carb
#> Mazda RX4         -1.8  -2 -166 -70 -0.02 -0.990 -2.44 -1  0    0    0
#> Mazda RX4 Wag     -1.8  -2 -166 -70 -0.02 -0.735 -1.88 -1  0    0    0
#> Datsun 710         0.0  -4 -218 -87 -0.07 -1.290 -0.29  0  0    0   -3
#> Hornet 4 Drive    -1.4  -2  -68 -70 -0.84 -0.395  0.54  0 -1   -1   -3
#> Hornet Sportabout -4.1   0   34  -5 -0.77 -0.170 -1.88 -1 -1   -1   -2
#> Valiant           -4.7  -2 -101 -75 -1.16 -0.150  1.32  0 -1   -1   -3
fnth(mtcars, 0.75, g)
#>          mpg cyl   disp     hp  drat     wt   qsec vs am gear carb
#> 4.0.1 26.000   4 120.30  91.00 4.430 2.1400 16.700  0  1  5.0    2
#> 4.1.0 23.600   4 143.75  96.00 3.810 3.1700 21.455  1  0  4.0    2
#> 4.1.1 31.400   4 101.55 101.00 4.165 2.2600 19.185  1  1  4.0    2
#> 6.0.1 21.000   6 160.00 142.50 3.900 2.8225 16.740  0  1  4.5    5
#> 6.1.0 19.750   6 233.25 123.00 3.920 3.4450 19.635  1  0  4.0    4
#> 8.0.0 16.625   8 410.00 218.75 3.165 4.3650 17.655  0  0  3.0    4
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
fnth(fgroup_by(mtcars, cyl, vs, am), 0.75)   # Another way of doing it..
#>   cyl vs am    mpg   disp     hp  drat     wt   qsec gear carb
#> 1   4  0  1 26.000 120.30  91.00 4.430 2.1400 16.700  5.0    2
#> 2   4  1  0 23.600 143.75  96.00 3.810 3.1700 21.455  4.0    2
#> 3   4  1  1 31.400 101.55 101.00 4.165 2.2600 19.185  4.0    2
#> 4   6  0  1 21.000 160.00 142.50 3.900 2.8225 16.740  4.5    5
#> 5   6  1  0 19.750 233.25 123.00 3.920 3.4450 19.635  4.0    4
#> 6   8  0  0 16.625 410.00 218.75 3.165 4.3650 17.655  3.0    4
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
fnth(mtcars, 0.75, g, use.g.names = FALSE)   # No row-names generated
#>      mpg cyl   disp     hp  drat     wt   qsec vs am gear carb
#> 1 26.000   4 120.30  91.00 4.430 2.1400 16.700  0  1  5.0    2
#> 2 23.600   4 143.75  96.00 3.810 3.1700 21.455  1  0  4.0    2
#> 3 31.400   4 101.55 101.00 4.165 2.2600 19.185  1  1  4.0    2
#> 4 21.000   6 160.00 142.50 3.900 2.8225 16.740  0  1  4.5    5
#> 5 19.750   6 233.25 123.00 3.920 3.4450 19.635  1  0  4.0    4
#> 6 16.625   8 410.00 218.75 3.165 4.3650 17.655  0  0  3.0    4
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

## matrix method
m <- qM(mtcars)
fnth(m, 0.75)
#>    mpg    cyl   disp     hp   drat     wt   qsec     vs     am   gear   carb 
#>  22.80   8.00 326.00 180.00   3.92   3.61  18.90   1.00   1.00   4.00   4.00 
head(fnth(m, 0.75, TRA = "-"))
#>                    mpg cyl disp  hp  drat     wt  qsec vs am gear carb
#> Mazda RX4         -1.8  -2 -166 -70 -0.02 -0.990 -2.44 -1  0    0    0
#> Mazda RX4 Wag     -1.8  -2 -166 -70 -0.02 -0.735 -1.88 -1  0    0    0
#> Datsun 710         0.0  -4 -218 -87 -0.07 -1.290 -0.29  0  0    0   -3
#> Hornet 4 Drive    -1.4  -2  -68 -70 -0.84 -0.395  0.54  0 -1   -1   -3
#> Hornet Sportabout -4.1   0   34  -5 -0.77 -0.170 -1.88 -1 -1   -1   -2
#> Valiant           -4.7  -2 -101 -75 -1.16 -0.150  1.32  0 -1   -1   -3
fnth(m, 0.75, g) # etc..
#>          mpg cyl   disp     hp  drat     wt   qsec vs am gear carb
#> 4.0.1 26.000   4 120.30  91.00 4.430 2.1400 16.700  0  1  5.0    2
#> 4.1.0 23.600   4 143.75  96.00 3.810 3.1700 21.455  1  0  4.0    2
#> 4.1.1 31.400   4 101.55 101.00 4.165 2.2600 19.185  1  1  4.0    2
#> 6.0.1 21.000   6 160.00 142.50 3.900 2.8225 16.740  0  1  4.5    5
#> 6.1.0 19.750   6 233.25 123.00 3.920 3.4450 19.635  1  0  4.0    4
#> 8.0.0 16.625   8 410.00 218.75 3.165 4.3650 17.655  0  0  3.0    4
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]

## method for grouped data frames - created with dplyr::group_by or fgroup_by
mtcars |> fgroup_by(cyl,vs,am) |> fnth(0.75)
#>   cyl vs am    mpg   disp     hp  drat     wt   qsec gear carb
#> 1   4  0  1 26.000 120.30  91.00 4.430 2.1400 16.700  5.0    2
#> 2   4  1  0 23.600 143.75  96.00 3.810 3.1700 21.455  4.0    2
#> 3   4  1  1 31.400 101.55 101.00 4.165 2.2600 19.185  4.0    2
#> 4   6  0  1 21.000 160.00 142.50 3.900 2.8225 16.740  4.5    5
#> 5   6  1  0 19.750 233.25 123.00 3.920 3.4450 19.635  4.0    4
#> 6   8  0  0 16.625 410.00 218.75 3.165 4.3650 17.655  3.0    4
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
mtcars |> fgroup_by(cyl,vs,am) |> fnth(0.75, hp)         # Weighted
#>   cyl vs am sum.hp      mpg     disp     drat       wt     qsec     gear
#> 1   4  0  1     91 26.00000 120.3000 4.430000 2.140000 16.70000 5.000000
#> 2   4  1  0    254 23.17474 142.1818 3.827947 3.159368 21.69076 4.000000
#> 3   4  1  1    564 30.51538 106.7863 4.113280 2.308710 18.95614 4.000000
#> 4   6  0  1    395 21.00000 160.0000 3.900000 2.806989 16.65727 4.685714
#> 5   6  1  0    461 19.65610 231.6000 3.920000 3.443333 19.56232 4.000000
#> 6   8  0  0   2330 16.36250 421.7391 3.198673 4.753537 17.67291 3.000000
#>       carb
#> 1 2.000000
#> 2 2.000000
#> 3 2.000000
#> 4 5.371429
#> 5 4.000000
#> 6 4.000000
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
mtcars |> fgroup_by(cyl,vs,am) |> fnth(0.75, TRA = "/")  # Divide by third quartile
#>                   cyl vs am       mpg      disp        hp      drat        wt
#> Mazda RX4           6  0  1 1.0000000 1.0000000 0.7719298 1.0000000 0.9282551
#> Mazda RX4 Wag       6  0  1 1.0000000 1.0000000 0.7719298 1.0000000 1.0186005
#> Datsun 710          4  1  1 0.7261146 1.0635155 0.9207921 0.9243697 1.0265487
#> Hornet 4 Drive      6  1  0 1.0835443 1.1061093 0.8943089 0.7857143 0.9332366
#> Hornet Sportabout   8  0  0 1.1248120 0.8780488 0.8000000 0.9952607 0.7880871
#> Valiant             6  1  0 0.9164557 0.9646302 0.8536585 0.7040816 1.0043541
#>                        qsec      gear carb
#> Mazda RX4         0.9832736 0.8888889 0.80
#> Mazda RX4 Wag     1.0167264 0.8888889 0.80
#> Datsun 710        0.9700287 1.0000000 0.50
#> Hornet 4 Drive    0.9900688 0.7500000 0.25
#> Hornet Sportabout 0.9640329 1.0000000 0.50
#> Valiant           1.0297937 0.7500000 0.25
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 
mtcars |> fgroup_by(cyl,vs,am) |> fselect(mpg, hp) |>    # Faster selecting
      fnth(0.75, hp, "/")  # Divide mpg by its third weighted group-quartile, using hp as weights
#>                      hp       mpg
#> Mazda RX4           110 1.0000000
#> Mazda RX4 Wag       110 1.0000000
#> Datsun 710           93 0.7471641
#> Hornet 4 Drive      110 1.0887207
#> Hornet Sportabout   175 1.1428571
#> Valiant             105 0.9208339
#> Duster 360          245 0.8739496
#> Merc 240D            62 1.0528706
#> Merc 230             95 0.9838299
#> Merc 280            123 0.9767961
#> Merc 280C           123 0.9055714
#> Merc 450SE          180 1.0022918
#> Merc 450SL          180 1.0572956
#> Merc 450SLC         180 0.9289534
#> Cadillac Fleetwood  205 0.6355997
#> Lincoln Continental 215 0.6355997
#> Chrysler Imperial   230 0.8983957
#> Fiat 128             66 1.0617595
#> Honda Civic          52 0.9962188
#> Toyota Corolla       65 1.1109150
#> Toyota Corona        97 0.9277344
#> Dodge Challenger    150 0.9472880
#> AMC Javelin         150 0.9289534
#> Camaro Z28          245 0.8128342
#> Pontiac Firebird    175 1.1734148
#> Fiat X1-9            66 0.8946307
#> Porsche 914-2        91 1.0000000
#> Lotus Europa        113 0.9962188
#> Ford Pantera L      264 1.0163247
#> Ferrari Dino        175 0.9380952
#> Maserati Bora       335 0.9648653
#> Volvo 142E          109 0.7012856
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 

# Efficient grouped estimation of multiple quantiles
mtcars |> fgroup_by(cyl,vs,am) |>
    fmutate(o = radixorder(GRPid(), mpg)) |>
    fsummarise(mpg_Q1 = fnth(mpg, 0.25, o = o),
               mpg_median = fmedian(mpg, o = o),
               mpg_Q3 = fnth(mpg, 0.75, o = o))
#>   cyl vs am mpg_Q1 mpg_median mpg_Q3
#> 1   4  0  1 26.000      26.00 26.000
#> 2   4  1  0 22.150      22.80 23.600
#> 3   4  1  1 25.050      30.40 31.400
#> 4   6  0  1 20.350      21.00 21.000
#> 5   6  1  0 18.025      18.65 19.750
#> 6   8  0  0 14.050      15.20 16.625
#> 7   8  0  1 15.200      15.40 15.600

## fmedian()
fmedian(mpg)                         # Simple median value
#> [1] 19.2
fmedian(mpg, w = mtcars$hp)          # Weighted median: Weighted by hp
#> [1] 16.4
fmedian(mpg, TRA = "-")              # Simple transformation: Subtract median value
#>  [1]  1.8  1.8  3.6  2.2 -0.5 -1.1 -4.9  5.2  3.6  0.0 -1.4 -2.8 -1.9 -4.0 -8.8
#> [16] -8.8 -4.5 13.2 11.2 14.7  2.3 -3.7 -4.0 -5.9  0.0  8.1  6.8 11.2 -3.4  0.5
#> [31] -4.2  2.2
fmedian(mpg, mtcars$cyl)             # Grouped median value
#>    4    6    8 
#> 26.0 19.7 15.2 
fmedian(mpg, mtcars[c(2,8:9)])       # More groups..
#> 4.0.1 4.1.0 4.1.1 6.0.1 6.1.0 8.0.0 8.0.1 
#> 26.00 22.80 30.40 21.00 18.65 15.20 15.40 
fmedian(mpg, g)
#> 4.0.1 4.1.0 4.1.1 6.0.1 6.1.0 8.0.0 8.0.1 
#> 26.00 22.80 30.40 21.00 18.65 15.20 15.40 
fmedian(mpg, g, mtcars$hp)           # Grouped weighted median
#> 4.0.1 4.1.0 4.1.1 6.0.1 6.1.0 8.0.0 8.0.1 
#>  26.0  22.8  30.4  21.0  19.2  15.2  15.0 
fmedian(mpg, g, TRA = "-")           # Groupwise subtract median value
#>  [1]  0.00  0.00 -7.60  2.75  3.50 -0.55 -0.90  1.60  0.00  0.55 -0.85  1.20
#> [13]  2.10  0.00 -4.80 -4.80 -0.50  2.00  0.00  3.50 -1.30  0.30  0.00 -1.90
#> [25]  4.00 -3.10  0.00  0.00  0.40 -1.30 -0.40 -9.00
fmedian(mpg, g, mtcars$hp, "-")      # Groupwise subtract weighted median value
#>  [1]  0.0  0.0 -7.6  2.2  3.5 -1.1 -0.9  1.6  0.0  0.0 -1.4  1.2  2.1  0.0 -4.8
#> [16] -4.8 -0.5  2.0  0.0  3.5 -1.3  0.3  0.0 -1.9  4.0 -3.1  0.0  0.0  0.8 -1.3
#> [31]  0.0 -9.0

## data.frame method
fmedian(mtcars)
#>     mpg     cyl    disp      hp    drat      wt    qsec      vs      am    gear 
#>  19.200   6.000 196.300 123.000   3.695   3.325  17.710   0.000   0.000   4.000 
#>    carb 
#>   2.000 
head(fmedian(mtcars, TRA = "-"))
#>                    mpg cyl  disp  hp   drat     wt  qsec vs am gear carb
#> Mazda RX4          1.8   0 -36.3 -13  0.205 -0.705 -1.25  0  1    0    2
#> Mazda RX4 Wag      1.8   0 -36.3 -13  0.205 -0.450 -0.69  0  1    0    2
#> Datsun 710         3.6  -2 -88.3 -30  0.155 -1.005  0.90  1  1    0   -1
#> Hornet 4 Drive     2.2   0  61.7 -13 -0.615 -0.110  1.73  1  0   -1   -1
#> Hornet Sportabout -0.5   2 163.7  52 -0.545  0.115 -0.69  0  0   -1    0
#> Valiant           -1.1   0  28.7 -18 -0.935  0.135  2.51  1  0   -1   -1
fmedian(mtcars, g)
#>         mpg cyl  disp    hp  drat    wt  qsec vs am gear carb
#> 4.0.1 26.00   4 120.3  91.0 4.430 2.140 16.70  0  1  5.0  2.0
#> 4.1.0 22.80   4 140.8  95.0 3.700 3.150 20.01  1  0  4.0  2.0
#> 4.1.1 30.40   4  79.0  66.0 4.080 1.935 18.61  1  1  4.0  1.0
#> 6.0.1 21.00   6 160.0 110.0 3.900 2.770 16.46  0  1  4.0  4.0
#> 6.1.0 18.65   6 196.3 116.5 3.500 3.440 19.17  1  0  3.5  2.5
#> 8.0.0 15.20   8 355.0 180.0 3.075 3.810 17.35  0  0  3.0  3.0
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
fmedian(fgroup_by(mtcars, cyl, vs, am))   # Another way of doing it..
#>   cyl vs am   mpg  disp    hp  drat    wt  qsec gear carb
#> 1   4  0  1 26.00 120.3  91.0 4.430 2.140 16.70  5.0  2.0
#> 2   4  1  0 22.80 140.8  95.0 3.700 3.150 20.01  4.0  2.0
#> 3   4  1  1 30.40  79.0  66.0 4.080 1.935 18.61  4.0  1.0
#> 4   6  0  1 21.00 160.0 110.0 3.900 2.770 16.46  4.0  4.0
#> 5   6  1  0 18.65 196.3 116.5 3.500 3.440 19.17  3.5  2.5
#> 6   8  0  0 15.20 355.0 180.0 3.075 3.810 17.35  3.0  3.0
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
fmedian(mtcars, g, use.g.names = FALSE)   # No row-names generated
#>     mpg cyl  disp    hp  drat    wt  qsec vs am gear carb
#> 1 26.00   4 120.3  91.0 4.430 2.140 16.70  0  1  5.0  2.0
#> 2 22.80   4 140.8  95.0 3.700 3.150 20.01  1  0  4.0  2.0
#> 3 30.40   4  79.0  66.0 4.080 1.935 18.61  1  1  4.0  1.0
#> 4 21.00   6 160.0 110.0 3.900 2.770 16.46  0  1  4.0  4.0
#> 5 18.65   6 196.3 116.5 3.500 3.440 19.17  1  0  3.5  2.5
#> 6 15.20   8 355.0 180.0 3.075 3.810 17.35  0  0  3.0  3.0
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

## matrix method
fmedian(m)
#>     mpg     cyl    disp      hp    drat      wt    qsec      vs      am    gear 
#>  19.200   6.000 196.300 123.000   3.695   3.325  17.710   0.000   0.000   4.000 
#>    carb 
#>   2.000 
head(fmedian(m, TRA = "-"))
#>                    mpg cyl  disp  hp   drat     wt  qsec vs am gear carb
#> Mazda RX4          1.8   0 -36.3 -13  0.205 -0.705 -1.25  0  1    0    2
#> Mazda RX4 Wag      1.8   0 -36.3 -13  0.205 -0.450 -0.69  0  1    0    2
#> Datsun 710         3.6  -2 -88.3 -30  0.155 -1.005  0.90  1  1    0   -1
#> Hornet 4 Drive     2.2   0  61.7 -13 -0.615 -0.110  1.73  1  0   -1   -1
#> Hornet Sportabout -0.5   2 163.7  52 -0.545  0.115 -0.69  0  0   -1    0
#> Valiant           -1.1   0  28.7 -18 -0.935  0.135  2.51  1  0   -1   -1
fmedian(m, g) # etc..
#>         mpg cyl  disp    hp  drat    wt  qsec vs am gear carb
#> 4.0.1 26.00   4 120.3  91.0 4.430 2.140 16.70  0  1  5.0  2.0
#> 4.1.0 22.80   4 140.8  95.0 3.700 3.150 20.01  1  0  4.0  2.0
#> 4.1.1 30.40   4  79.0  66.0 4.080 1.935 18.61  1  1  4.0  1.0
#> 6.0.1 21.00   6 160.0 110.0 3.900 2.770 16.46  0  1  4.0  4.0
#> 6.1.0 18.65   6 196.3 116.5 3.500 3.440 19.17  1  0  3.5  2.5
#> 8.0.0 15.20   8 355.0 180.0 3.075 3.810 17.35  0  0  3.0  3.0
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]

## method for grouped data frames - created with dplyr::group_by or fgroup_by
mtcars |> fgroup_by(cyl,vs,am) |> fmedian()
#>   cyl vs am   mpg  disp    hp  drat    wt  qsec gear carb
#> 1   4  0  1 26.00 120.3  91.0 4.430 2.140 16.70  5.0  2.0
#> 2   4  1  0 22.80 140.8  95.0 3.700 3.150 20.01  4.0  2.0
#> 3   4  1  1 30.40  79.0  66.0 4.080 1.935 18.61  4.0  1.0
#> 4   6  0  1 21.00 160.0 110.0 3.900 2.770 16.46  4.0  4.0
#> 5   6  1  0 18.65 196.3 116.5 3.500 3.440 19.17  3.5  2.5
#> 6   8  0  0 15.20 355.0 180.0 3.075 3.810 17.35  3.0  3.0
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
mtcars |> fgroup_by(cyl,vs,am) |> fmedian(hp)           # Weighted
#>   cyl vs am sum.hp  mpg  disp drat    wt  qsec gear carb
#> 1   4  0  1     91 26.0 120.3 4.43 2.140 16.70    5    2
#> 2   4  1  0    254 22.8 140.8 3.70 3.150 20.01    4    2
#> 3   4  1  1    564 30.4  95.1 4.08 1.935 18.61    4    1
#> 4   6  0  1    395 21.0 160.0 3.90 2.770 16.46    4    4
#> 5   6  1  0    461 19.2 167.6 3.92 3.440 18.90    4    4
#> 6   8  0  0   2330 15.2 360.0 3.08 3.840 17.40    3    3
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
mtcars |> fgroup_by(cyl,vs,am) |> fmedian(TRA = "-")    # De-median
#>                   cyl vs am   mpg disp    hp   drat     wt  qsec gear carb
#> Mazda RX4           6  0  1  0.00  0.0   0.0  0.000 -0.150  0.00  0.0  0.0
#> Mazda RX4 Wag       6  0  1  0.00  0.0   0.0  0.000  0.105  0.56  0.0  0.0
#> Datsun 710          4  1  1 -7.60 29.0  27.0 -0.230  0.385  0.00  0.0  0.0
#> Hornet 4 Drive      6  1  0  2.75 61.7  -6.5 -0.420 -0.225  0.27 -0.5 -1.5
#> Hornet Sportabout   8  0  0  3.50  5.0  -5.0  0.075 -0.370 -0.33  0.0 -1.0
#> Valiant             6  1  0 -0.55 28.7 -11.5 -0.740  0.020  1.05 -0.5 -1.5
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 
mtcars |> fgroup_by(cyl,vs,am) |> fselect(mpg, hp) |>   # Faster selecting
      fmedian(hp, "-")  # Weighted de-median mpg, using hp as weights
#>                      hp  mpg
#> Mazda RX4           110  0.0
#> Mazda RX4 Wag       110  0.0
#> Datsun 710           93 -7.6
#> Hornet 4 Drive      110  2.2
#> Hornet Sportabout   175  3.5
#> Valiant             105 -1.1
#> Duster 360          245 -0.9
#> Merc 240D            62  1.6
#> Merc 230             95  0.0
#> Merc 280            123  0.0
#> Merc 280C           123 -1.4
#> Merc 450SE          180  1.2
#> Merc 450SL          180  2.1
#> Merc 450SLC         180  0.0
#> Cadillac Fleetwood  205 -4.8
#> Lincoln Continental 215 -4.8
#> Chrysler Imperial   230 -0.5
#> Fiat 128             66  2.0
#> Honda Civic          52  0.0
#> Toyota Corolla       65  3.5
#> Toyota Corona        97 -1.3
#> Dodge Challenger    150  0.3
#> AMC Javelin         150  0.0
#> Camaro Z28          245 -1.9
#> Pontiac Firebird    175  4.0
#> Fiat X1-9            66 -3.1
#> Porsche 914-2        91  0.0
#> Lotus Europa        113  0.0
#> Ford Pantera L      264  0.8
#> Ferrari Dino        175 -1.3
#> Maserati Bora       335  0.0
#> Volvo 142E          109 -9.0
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 
```
