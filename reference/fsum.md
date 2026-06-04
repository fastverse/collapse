# Fast (Grouped, Weighted) Sum for Matrix-Like Objects

`fsum` is a generic function that computes the (column-wise) sum of all
values in `x`, (optionally) grouped by `g` and/or weighted by `w` (e.g.
to calculate survey totals). The
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) argument can
further be used to transform `x` using its (grouped, weighted) sum.

## Usage

``` r
fsum(x, ...)

# Default S3 method
fsum(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, fill = FALSE, nthreads = .op[["nthreads"]], ...)

# S3 method for class 'matrix'
fsum(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, drop = TRUE, fill = FALSE, nthreads = .op[["nthreads"]], ...)

# S3 method for class 'data.frame'
fsum(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, drop = TRUE, fill = FALSE, nthreads = .op[["nthreads"]], ...)

# S3 method for class 'grouped_df'
fsum(x, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = FALSE, keep.group_vars = TRUE, keep.w = TRUE, stub = .op[["stub"]],
     fill = FALSE, nthreads = .op[["nthreads"]], ...)
```

## Arguments

- x:

  a numeric vector, matrix, data frame or grouped data frame (class
  'grouped_df').

- g:

  a factor, `GRP` object, atomic vector (internally converted to factor)
  or a list of vectors / factors (internally converted to a `GRP`
  object) used to group `x`.

- w:

  a numeric vector of (non-negative) weights, may contain missing
  values.

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

- fill:

  logical. Initialize result with `0` instead of `NA` when
  `na.rm = TRUE` e.g. `fsum(NA, fill = TRUE)` returns `0` instead of
  `NA`.

- nthreads:

  integer. The number of threads to utilize. See Details.

- drop:

  *matrix and data.frame method:* Logical. `TRUE` drops dimensions and
  returns an atomic vector if `g = NULL` and `TRA = NULL`.

- keep.group_vars:

  *grouped_df method:* Logical. `FALSE` removes grouping variables after
  computation.

- keep.w:

  *grouped_df method:* Logical. Retain summed weighting variable after
  computation (if contained in `grouped_df`).

- stub:

  character. If `keep.w = TRUE` and `stub = TRUE` (default), the summed
  weights column is prefixed by `"sum."`. Users can specify a different
  prefix through this argument, or set it to `FALSE` to avoid prefixing.

- ...:

  arguments to be passed to or from other methods. If `TRA` is used,
  passing `set = TRUE` will transform data by reference and return the
  result invisibly.

## Details

The weighted sum (e.g. survey total) is computed as `sum(x * w)`, but in
one pass and about twice as efficient. If `na.rm = TRUE`, missing values
will be removed from both `x` and `w` i.e. utilizing only
`x[complete.cases(x,w)]` and `w[complete.cases(x,w)]`.

This all seamlessly generalizes to grouped computations, which are
performed in a single pass (without splitting the data) and are
therefore extremely fast. See Benchmark and Examples below.

When applied to data frames with groups or `drop = FALSE`, `fsum`
preserves all column attributes. The attributes of the data frame itself
are also preserved.

Since v1.6.0 `fsum` explicitly supports integers. Integers are summed
using the long long type in C which is bounded at
+-9,223,372,036,854,775,807 (so ~4.3 billion times greater than the
minimum/maximum R integer bounded at +-2,147,483,647). If the value of
the sum is outside +-2,147,483,647, a double containing the result is
returned, otherwise an integer is returned. With groups, an integer
results vector is initialized, and an integer overflow error is provided
if the sum in any group is outside +-2,147,483,647. Data needs to be
coerced to double beforehand in such cases.

Multithreading, added in v1.8.0, applies at the column-level unless
`g = NULL` and `nthreads > NCOL(x)`. Parallelism over groups is not
available because sums are computed simultaneously within each group.
`nthreads = 1L` uses a serial version of the code, not parallel code
running on one thread. This serial code is always used with less than
100,000 obs (`length(x) < 100000` for vectors and matrices), because
parallel execution itself has some overhead.

## Value

The (`w` weighted) sum of `x`, grouped by `g`, or (if
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) is used) `x`
transformed by its (grouped, weighted) sum.

## See Also

[`fprod`](https://fastverse.org/collapse/reference/fprod.md),
[`fmean`](https://fastverse.org/collapse/reference/fmean.md), [Fast
Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## default vector method
mpg <- mtcars$mpg
fsum(mpg)                         # Simple sum
#> [1] 642.9
fsum(mpg, w = mtcars$hp)          # Weighted sum (total): Weighted by hp
#> [1] 84362.7
fsum(mpg, TRA = "%")              # Simple transformation: obtain percentages of mpg
#>  [1] 3.266449 3.266449 3.546430 3.328667 2.908695 2.815368 2.224296 3.795303
#>  [9] 3.546430 2.986468 2.768704 2.550941 2.690932 2.364287 1.617670 1.617670
#> [17] 2.286514 5.039664 4.728574 5.272982 3.344221 2.410950 2.364287 2.068751
#> [25] 2.986468 4.246384 4.044175 4.728574 2.457614 3.064240 2.333178 3.328667
fsum(mpg, mtcars$cyl)             # Grouped sum
#>     4     6     8 
#> 293.3 138.2 211.4 
fsum(mpg, mtcars$cyl, mtcars$hp)  # Weighted grouped sum (total)
#>       4       6       8 
#> 23743.0 16873.0 43746.7 
fsum(mpg, mtcars[c(2,8:9)])       # More groups..
#> 4.0.1 4.1.0 4.1.1 6.0.1 6.1.0 8.0.0 8.0.1 
#>  26.0  68.7 198.6  61.7  76.5 180.6  30.8 
g <- GRP(mtcars, ~ cyl + vs + am) # Precomputing groups gives more speed !
fsum(mpg, g)
#> 4.0.1 4.1.0 4.1.1 6.0.1 6.1.0 8.0.0 8.0.1 
#>  26.0  68.7 198.6  61.7  76.5 180.6  30.8 
fmean(mpg, g) == fsum(mpg, g) / fnobs(mpg, g)
#> 4.0.1 4.1.0 4.1.1 6.0.1 6.1.0 8.0.0 8.0.1 
#>  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE 
fsum(mpg, g, TRA = "%")           # Percentages by group
#>  [1]  34.035656  34.035656  11.480363  27.973856  10.354374  23.660131
#>  [7]   7.918051  35.516739  33.187773  25.098039  23.267974   9.080842
#> [13]   9.579181   8.416390   5.758583   5.758583   8.139535  16.314199
#> [19]  15.307150  17.069486  31.295488   8.582503   8.416390   7.364341
#> [25]  10.631229  13.746224 100.000000  15.307150  51.298701  31.928687
#> [31]  48.701299  10.775428

## data.frame method
fsum(mtcars)
#>      mpg      cyl     disp       hp     drat       wt     qsec       vs 
#>  642.900  198.000 7383.100 4694.000  115.090  102.952  571.160   14.000 
#>       am     gear     carb 
#>   13.000  118.000   90.000 
fsum(mtcars, TRA = "%")
#>                        mpg      cyl     disp       hp     drat       wt
#> Mazda RX4         3.266449 3.030303 2.167111 2.343417 3.388652 2.544875
#> Mazda RX4 Wag     3.266449 3.030303 2.167111 2.343417 3.388652 2.792564
#> Datsun 710        3.546430 2.020202 1.462800 1.981253 3.345208 2.253477
#> Hornet 4 Drive    3.328667 3.030303 3.494467 2.343417 2.676166 3.122815
#> Hornet Sportabout 2.908695 4.040404 4.876001 3.728164 2.736988 3.341363
#> Valiant           2.815368 3.030303 3.047500 2.236898 2.398123 3.360789
#>                       qsec       vs       am     gear     carb
#> Mazda RX4         2.881854 0.000000 7.692308 3.389831 4.444444
#> Mazda RX4 Wag     2.979901 0.000000 7.692308 3.389831 4.444444
#> Datsun 710        3.258281 7.142857 7.692308 3.389831 1.111111
#> Hornet 4 Drive    3.403600 7.142857 0.000000 2.542373 1.111111
#> Hornet Sportabout 2.979901 0.000000 0.000000 2.542373 2.222222
#> Valiant           3.540164 7.142857 0.000000 2.542373 1.111111
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
fsum(mtcars, g)
#>         mpg cyl   disp   hp  drat     wt   qsec vs am gear carb
#> 4.0.1  26.0   4  120.3   91  4.43  2.140  16.70  0  1    5    2
#> 4.1.0  68.7  12  407.6  254 11.31  8.805  62.91  3  0   11    5
#> 4.1.1 198.6  28  628.6  564 29.04 14.198 130.90  7  7   29   10
#> 6.0.1  61.7  18  465.0  395 11.42  8.265  48.98  0  3   13   14
#> 6.1.0  76.5  24  818.2  461 13.68 13.555  76.86  4  0   14   10
#> 8.0.0 180.6  96 4291.4 2330 37.45 49.249 205.71  0  0   36   37
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
fsum(mtcars, g, TRA = "%")
#>                        mpg       cyl      disp       hp      drat        wt
#> Mazda RX4         34.03566 33.333333 34.408602 27.84810 34.150613 31.699940
#> Mazda RX4 Wag     34.03566 33.333333 34.408602 27.84810 34.150613 34.785239
#> Datsun 710        11.48036 14.285714 17.181037 16.48936 13.257576 16.340330
#> Hornet 4 Drive    27.97386 25.000000 31.532633 23.86117 22.514620 23.718185
#> Hornet Sportabout 10.35437  8.333333  8.388871  7.51073  8.411215  6.984913
#> Valiant           23.66013 25.000000 27.499389 22.77657 20.175439 25.525636
#>                        qsec       vs       am      gear      carb
#> Mazda RX4         33.605553      NaN 33.33333 30.769231 28.571429
#> Mazda RX4 Wag     34.748877      NaN 33.33333 30.769231 28.571429
#> Datsun 710        14.216960 14.28571 14.28571 13.793103 10.000000
#> Hornet 4 Drive    25.292740 25.00000      NaN 21.428571 10.000000
#> Hornet Sportabout  8.273783      NaN      NaN  8.333333  5.405405
#> Valiant           26.307572 25.00000      NaN 21.428571 10.000000
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]

## matrix method
m <- qM(mtcars)
fsum(m)
#>      mpg      cyl     disp       hp     drat       wt     qsec       vs 
#>  642.900  198.000 7383.100 4694.000  115.090  102.952  571.160   14.000 
#>       am     gear     carb 
#>   13.000  118.000   90.000 
fsum(m, TRA = "%")
#>                          mpg      cyl      disp       hp     drat       wt
#> Mazda RX4           3.266449 3.030303 2.1671114 2.343417 3.388652 2.544875
#> Mazda RX4 Wag       3.266449 3.030303 2.1671114 2.343417 3.388652 2.792564
#> Datsun 710          3.546430 2.020202 1.4628002 1.981253 3.345208 2.253477
#> Hornet 4 Drive      3.328667 3.030303 3.4944671 2.343417 2.676166 3.122815
#> Hornet Sportabout   2.908695 4.040404 4.8760006 3.728164 2.736988 3.341363
#> Valiant             2.815368 3.030303 3.0475004 2.236898 2.398123 3.360789
#>                         qsec       vs       am     gear     carb
#> Mazda RX4           2.881854 0.000000 7.692308 3.389831 4.444444
#> Mazda RX4 Wag       2.979901 0.000000 7.692308 3.389831 4.444444
#> Datsun 710          3.258281 7.142857 7.692308 3.389831 1.111111
#> Hornet 4 Drive      3.403600 7.142857 0.000000 2.542373 1.111111
#> Hornet Sportabout   2.979901 0.000000 0.000000 2.542373 2.222222
#> Valiant             3.540164 7.142857 0.000000 2.542373 1.111111
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
fsum(m, g)
#>         mpg cyl   disp   hp  drat     wt   qsec vs am gear carb
#> 4.0.1  26.0   4  120.3   91  4.43  2.140  16.70  0  1    5    2
#> 4.1.0  68.7  12  407.6  254 11.31  8.805  62.91  3  0   11    5
#> 4.1.1 198.6  28  628.6  564 29.04 14.198 130.90  7  7   29   10
#> 6.0.1  61.7  18  465.0  395 11.42  8.265  48.98  0  3   13   14
#> 6.1.0  76.5  24  818.2  461 13.68 13.555  76.86  4  0   14   10
#> 8.0.0 180.6  96 4291.4 2330 37.45 49.249 205.71  0  0   36   37
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
fsum(m, g, TRA = "%")
#>                            mpg        cyl       disp         hp       drat
#> Mazda RX4            34.035656  33.333333  34.408602  27.848101  34.150613
#> Mazda RX4 Wag        34.035656  33.333333  34.408602  27.848101  34.150613
#> Datsun 710           11.480363  14.285714  17.181037  16.489362  13.257576
#> Hornet 4 Drive       27.973856  25.000000  31.532633  23.861171  22.514620
#> Hornet Sportabout    10.354374   8.333333   8.388871   7.510730   8.411215
#> Valiant              23.660131  25.000000  27.499389  22.776573  20.175439
#>                             wt       qsec       vs        am       gear
#> Mazda RX4            31.699940  33.605553      NaN  33.33333  30.769231
#> Mazda RX4 Wag        34.785239  34.748877      NaN  33.33333  30.769231
#> Datsun 710           16.340330  14.216960 14.28571  14.28571  13.793103
#> Hornet 4 Drive       23.718185  25.292740 25.00000       NaN  21.428571
#> Hornet Sportabout     6.984913   8.273783      NaN       NaN   8.333333
#> Valiant              25.525636  26.307572 25.00000       NaN  21.428571
#>                           carb
#> Mazda RX4            28.571429
#> Mazda RX4 Wag        28.571429
#> Datsun 710           10.000000
#> Hornet 4 Drive       10.000000
#> Hornet Sportabout     5.405405
#> Valiant              10.000000
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]

## method for grouped data frames - created with dplyr::group_by or fgroup_by
mtcars |> fgroup_by(cyl,vs,am) |> fsum(hp)  # Weighted grouped sum (total)
#>   cyl vs am sum.hp     mpg     disp    drat       wt     qsec gear carb
#> 1   4  0  1     91  2366.0  10947.3  403.13  194.740  1519.70  455  182
#> 2   4  1  0    254  5764.3  34121.1  960.08  736.135  5356.47  919  411
#> 3   4  1  1    564 15612.7  52945.4 2301.27 1165.914 10456.79 2369  838
#> 4   6  0  1    395  8067.5  60575.0 1491.50 1089.200  6395.30 1755 1930
#> 5   6  1  0    461  8805.5  93234.6 1592.92 1563.190  8837.10 1629 1199
#> 6   8  0  0   2330 34550.5 846042.0 7323.40 9689.735 39807.80 6990 7480
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
mtcars |> fgroup_by(cyl,vs,am) |> fsum(TRA = "%")
#>                   cyl vs am      mpg      disp       hp      drat        wt
#> Mazda RX4           6  0  1 34.03566 34.408602 27.84810 34.150613 31.699940
#> Mazda RX4 Wag       6  0  1 34.03566 34.408602 27.84810 34.150613 34.785239
#> Datsun 710          4  1  1 11.48036 17.181037 16.48936 13.257576 16.340330
#> Hornet 4 Drive      6  1  0 27.97386 31.532633 23.86117 22.514620 23.718185
#> Hornet Sportabout   8  0  0 10.35437  8.388871  7.51073  8.411215  6.984913
#> Valiant             6  1  0 23.66013 27.499389 22.77657 20.175439 25.525636
#>                        qsec      gear      carb
#> Mazda RX4         33.605553 30.769231 28.571429
#> Mazda RX4 Wag     34.748877 30.769231 28.571429
#> Datsun 710        14.216960 13.793103 10.000000
#> Hornet 4 Drive    25.292740 21.428571 10.000000
#> Hornet Sportabout  8.273783  8.333333  5.405405
#> Valiant           26.307572 21.428571 10.000000
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 
mtcars |> fgroup_by(cyl,vs,am) |> fselect(mpg) |> fsum()
#>   cyl vs am   mpg
#> 1   4  0  1  26.0
#> 2   4  1  0  68.7
#> 3   4  1  1 198.6
#> 4   6  0  1  61.7
#> 5   6  1  0  76.5
#> 6   8  0  0 180.6
#> 7   8  0  1  30.8

 
## This compares fsum with data.table and base::rowsum
# Starting with small data
library(data.table)
#> 
#> Attaching package: ‘data.table’
#> The following objects are masked from ‘package:dplyr’:
#> 
#>     between, first, last
#> The following object is masked from ‘package:collapse’:
#> 
#>     fdroplevels
#> The following object is masked from ‘package:base’:
#> 
#>     %notin%
opts <- set_collapse(nthreads = getDTthreads())
mtcDT <- qDT(mtcars)
f <- qF(mtcars$cyl)

library(microbenchmark)
microbenchmark(mtcDT[, lapply(.SD, sum), by = f],
               rowsum(mtcDT, f, reorder = FALSE),
               fsum(mtcDT, f, na.rm = FALSE), unit = "relative")
#> Unit: relative
#>                               expr       min        lq      mean   median
#>  mtcDT[, lapply(.SD, sum), by = f] 98.870968 82.568282 112.64257 73.79615
#>  rowsum(mtcDT, f, reorder = FALSE)  3.774194  3.515419   3.19039  3.40000
#>      fsum(mtcDT, f, na.rm = FALSE)  1.000000  1.000000   1.00000  1.00000
#>         uq         max neval
#>  69.702614 1189.700855   100
#>   3.232026    5.641026   100
#>   1.000000    1.000000   100

# Now larger data
tdata <- qDT(replicate(100, rnorm(1e5), simplify = FALSE)) # 100 columns with 100.000 obs
f <- qF(sample.int(1e4, 1e5, TRUE))                        # A factor with 10.000 groups

microbenchmark(tdata[, lapply(.SD, sum), by = f],
               rowsum(tdata, f, reorder = FALSE),
               fsum(tdata, f, na.rm = FALSE), unit = "relative")
#> Unit: relative
#>                               expr      min       lq     mean   median       uq
#>  tdata[, lapply(.SD, sum), by = f] 3.239244 3.174959 3.224813 3.232994 3.574582
#>  rowsum(tdata, f, reorder = FALSE) 2.252901 2.195694 2.647779 2.214542 2.417913
#>      fsum(tdata, f, na.rm = FALSE) 1.000000 1.000000 1.000000 1.000000 1.000000
#>        max neval
#>   4.840187   100
#>  11.715803   100
#>   1.000000   100
# Reset options
set_collapse(opts)
```
