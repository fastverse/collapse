# collapse for tidyverse Users

*collapse* is a C/C++ based package for data transformation and
statistical computing in R that aims to enable greater performance and
statistical complexity in data manipulation tasks and offers a stable,
class-agnostic, and lightweight API. It is part of the core
[*fastverse*](https://fastverse.org/fastverse/), a suite of lightweight
packages with similar objectives.

The [*tidyverse*](https://tidyverse.org/) set of packages provides a
rich, expressive, and consistent syntax for data manipulation in R
centering on the *tibble* object and tidy data principles (each
observation is a row, each variable is a column).

*collapse* fully supports the *tibble* object and provides many
*tidyverse*-like functions for data manipulation. It can thus be used to
write *tidyverse*-like data manipulation code that, thanks to low-level
vectorization of many statistical operations and optimized R code,
typically runs much faster than native *tidyverse* code, in addition to
being much more lightweight in dependencies.

Its aim is not to create a faster *tidyverse*, i.e., it does not
implements all aspects of the rich *tidyverse* grammar or changes to
it[^1], and also takes inspiration from other leading data manipulation
libraries to serve broad aims of performance, parsimony, complexity, and
robustness in data manipulation for R.

## Namespace and Global Options

*collapse* data manipulation functions familiar to *tidyverse* users
include `fselect`, `fgroup_by`, `fsummarise`, `fmutate`, `across`,
`frename`, `fslice`, and `fcount`. Other functions like `fsubset`,
`ftransform`, and `get_vars` are inspired by base R, while again other
functions like `join`, `pivot`, `roworder`, `colorder`, `rowbind`, etc.
are inspired by other data manipulation libraries such as *data.table*
and *polars*.

By virtue of the f- prefixes, the *collapse* namespace has no conflicts
with the *tidyverse*, and these functions can easily be substituted in a
*tidyverse* workflow.

R users willing to replace the *tidyverse* have the additional option to
mask functions and eliminate the prefixes with `set_collapse`. For
example

``` r

library(collapse)
set_collapse(mask = "manip") # version >= 2.0.0 
```

makes available functions `select`, `group_by`, `summarise`, `mutate`,
`rename`, `count`, `subset`, `slice`, and `transform` in the *collapse*
namespace and detaches and re-attaches the package, such that the
following code is executed by *collapse*:

``` r

mtcars |>
  subset(mpg > 11) |>
  group_by(cyl, vs, am) |>
  summarise(across(c(mpg, carb, hp), mean), 
            qsec_wt = weighted.mean(qsec, wt))
#   cyl vs am      mpg     carb        hp  qsec_wt
# 1   4  0  1 26.00000 2.000000  91.00000 16.70000
# 2   4  1  0 22.90000 1.666667  84.66667 21.04028
# 3   4  1  1 28.37143 1.428571  80.57143 18.75509
# 4   6  0  1 20.56667 4.666667 131.66667 16.33306
# 5   6  1  0 19.12500 2.500000 115.25000 19.21275
# 6   8  0  0 15.98000 2.900000 191.00000 17.01239
# 7   8  0  1 15.40000 6.000000 299.50000 14.55297
```

*Note* that the correct documentation still needs to be called with
prefixes, i.e.,
[`?fsubset`](https://fastverse.org/collapse/reference/fsubset.md). See
[`?set_collapse`](https://fastverse.org/collapse/reference/collapse-options.md)
for further options to the package, which also includes optimization
options such as `nthreads`, `na.rm`, `sort`, and `stable.algo`. *Note*
also that if you use *collapse*’s namespace masking, you can use
[`fastverse::fastverse_conflicts()`](https://fastverse.github.io/fastverse/reference/fastverse_conflicts.html)
to check for namespace conflicts with other packages.

## Using the *Fast Statistical Functions*

A key feature of *collapse* is that it not only provides functions for
data manipulation, but also a full set of statistical functions and
algorithms to speed up statistical calculations and perform more complex
statistical operations (e.g. involving weights or time series data).

Notably among these, the [*Fast Statistical
Functions*](https://fastverse.org/collapse/reference/fast-statistical-functions.html)
is a consistent set of S3-generic statistical functions providing fully
vectorized statistical operations in R.

Specifically, operations such as calculating the mean via the S3 generic
[`fmean()`](https://fastverse.org/collapse/reference/fmean.md) function
are vectorized across columns and groups and may also involve weights or
transformations of the original data:

``` r

fmean(mtcars$mpg)     # Vector
# [1] 20.09063
fmean(EuStockMarkets) # Matrix
#      DAX      SMI      CAC     FTSE 
# 2530.657 3376.224 2227.828 3565.643
fmean(mtcars)         # Data Frame
#        mpg        cyl       disp         hp       drat         wt       qsec         vs         am 
#  20.090625   6.187500 230.721875 146.687500   3.596563   3.217250  17.848750   0.437500   0.406250 
#       gear       carb 
#   3.687500   2.812500

fmean(mtcars$mpg, w = mtcars$wt)  # Weighted mean
# [1] 18.54993
fmean(mtcars$mpg, g = mtcars$cyl) # Grouped mean
#        4        6        8 
# 26.66364 19.74286 15.10000
fmean(mtcars$mpg, g = mtcars$cyl, w = mtcars$wt)   # Weighted group mean
#        4        6        8 
# 25.93504 19.64578 14.80643
fmean(mtcars[5:10], g = mtcars$cyl, w = mtcars$wt) # Of data frame
#       drat       wt     qsec        vs        am     gear
# 4 4.031264 2.414750 19.38044 0.9148868 0.6498031 4.047250
# 6 3.569170 3.152060 18.12198 0.6212191 0.3787809 3.821036
# 8 3.205658 4.133116 16.88529 0.0000000 0.1203808 3.240762
fmean(mtcars$mpg, g = mtcars$cyl, w = mtcars$wt, TRA = "fill") # Replace data by weighted group mean
#  [1] 19.64578 19.64578 25.93504 19.64578 14.80643 19.64578 14.80643 25.93504 25.93504 19.64578
# [11] 19.64578 14.80643 14.80643 14.80643 14.80643 14.80643 14.80643 25.93504 25.93504 25.93504
# [21] 25.93504 14.80643 14.80643 14.80643 14.80643 25.93504 25.93504 25.93504 14.80643 19.64578
# [31] 14.80643 25.93504
# etc...
```

The data manipulation functions of *collapse* are integrated with these
*Fast Statistical Functions* to enable vectorized statistical
operations. For example, the following code

``` r

mtcars |>
  subset(mpg > 11) |>
  group_by(cyl, vs, am) |>
  summarise(across(c(mpg, carb, hp), fmean), 
            qsec_wt = fmean(qsec, wt))
#   cyl vs am      mpg     carb        hp  qsec_wt
# 1   4  0  1 26.00000 2.000000  91.00000 16.70000
# 2   4  1  0 22.90000 1.666667  84.66667 21.04028
# 3   4  1  1 28.37143 1.428571  80.57143 18.75509
# 4   6  0  1 20.56667 4.666667 131.66667 16.33306
# 5   6  1  0 19.12500 2.500000 115.25000 19.21275
# 6   8  0  0 15.98000 2.900000 191.00000 17.01239
# 7   8  0  1 15.40000 6.000000 299.50000 14.55297
```

gives exactly the same result as above, but the execution is much faster
(especially on larger data), because with *Fast Statistical Functions*,
the data does not need to be split by groups, and there is no need to
call [`lapply()`](https://rdrr.io/r/base/lapply.html) inside the
[`across()`](https://fastverse.org/collapse/reference/across.md)
statement:
[`fmean.data.frame()`](https://fastverse.org/collapse/reference/fmean.md)
is simply applied to a subset of the data containing columns `mpg`,
`carb` and `hp`.

The *Fast Statistical Functions* also have a method for grouped data, so
if we did not want to calculate the weighted mean of `qsec`, the code
would simplify as follows:

``` r

mtcars |>
  subset(mpg > 11) |>
  group_by(cyl, vs, am) |>
  select(mpg, carb, hp) |> 
  fmean()
#   cyl vs am      mpg     carb        hp
# 1   4  0  1 26.00000 2.000000  91.00000
# 2   4  1  0 22.90000 1.666667  84.66667
# 3   4  1  1 28.37143 1.428571  80.57143
# 4   6  0  1 20.56667 4.666667 131.66667
# 5   6  1  0 19.12500 2.500000 115.25000
# 6   8  0  0 15.98000 2.900000 191.00000
# 7   8  0  1 15.40000 6.000000 299.50000
```

Note that all functions in *collapse*, including the *Fast Statistical
Functions*, have the default `na.rm = TRUE`, i.e., missing values are
skipped in calculations. This can be changed using
`set_collapse(na.rm = FALSE)` to give behavior more consistent with base
R.

Another thing to be aware of when using *Fast Statistical Functions*
inside data manipulation functions is that they toggle vectorized
execution wherever they are used. E.g.

``` r

mtcars |> group_by(cyl) |> summarise(mpg = fmean(mpg) + min(qsec)) # Vectorized
#   cyl      mpg
# 1   4 41.16364
# 2   6 34.24286
# 3   8 29.60000
```

calculates a grouped mean of `mpg` but adds the overall minimum of
`qsec` to the result, i.e., it is equivalent to
`fmean(mpg, g = cyl) + min(qsec)`. On the other hand

``` r

mtcars |> group_by(cyl) |> summarise(mpg = fmean(mpg) + fmin(qsec)) # Vectorized
#   cyl      mpg
# 1   4 43.36364
# 2   6 35.24286
# 3   8 29.60000
mtcars |> group_by(cyl) |> summarise(mpg = mean(mpg) + min(qsec))   # Not vectorized
#   cyl      mpg
# 1   4 43.36364
# 2   6 35.24286
# 3   8 29.60000
```

both give the mean + the minimum within each group, but calculated in
different ways: the former is equivalent to
`fmean(mpg, g = cyl) + fmin(qsec, g = cyl)`, whereas the latter is equal
to `sapply(gsplit(mpg, cyl), function(x) mean(x) + min(x))`.

See
[`?fsummarise`](https://fastverse.org/collapse/reference/fsummarise.md)
and [`?fmutate`](https://fastverse.org/collapse/reference/ftransform.md)
for more detailed examples. This *eager vectorization* approach is
intentional as it allows users to vectorize complex expressions and fall
back to base R if this is not desired. [This blog
post](https://andrewghazi.github.io/posts/collapse_is_sick/sick.html) by
Andrew Ghazi provides an excellent example of computing a p-value test
statistic by groups. *Note* that only expressions typed out can be
vectorized; expressions inside functions such as
`mean_plus_min <- function(x) fmean(x) + fmin(x)` are not
vectorized.[^2] To take full advantage of *collapse*, it is thus highly
recommended to use the *Fast Statistical Functions* as much as possible.

## Writing Efficient Code

It is also performance-critical to correctly sequence operations and
limit excess computations. *tidyverse* code is often inefficient simply
because the *tidyverse* allows you to do everything. For example,
`mtcars |> group_by(cyl) |> filter(mpg > 13) |> arrange(mpg)` is
permissible but inefficient code as it filters and reorders grouped
data, requiring modifications to both the data frame and the attached
grouping object. *collapse* does not allow calls to
[`fsubset()`](https://fastverse.org/collapse/reference/fsubset.md) on
grouped data, and messages about it in
[`roworder()`](https://fastverse.org/collapse/reference/roworder.md),
encouraging you to write more efficient code.

The above example can also be optimized because we are subsetting the
whole frame and then doing computations on a subset of columns. It would
be more efficient to select all required columns during the subset
operation:

``` r

mtcars |>
  subset(mpg > 11, cyl, vs, am, mpg, carb, hp, qsec, wt) |>
  group_by(cyl, vs, am) |>
  summarise(across(c(mpg, carb, hp), fmean), 
            qsec_wt = fmean(qsec, wt))
#   cyl vs am      mpg     carb        hp  qsec_wt
# 1   4  0  1 26.00000 2.000000  91.00000 16.70000
# 2   4  1  0 22.90000 1.666667  84.66667 21.04028
# 3   4  1  1 28.37143 1.428571  80.57143 18.75509
# 4   6  0  1 20.56667 4.666667 131.66667 16.33306
# 5   6  1  0 19.12500 2.500000 115.25000 19.21275
# 6   8  0  0 15.98000 2.900000 191.00000 17.01239
# 7   8  0  1 15.40000 6.000000 299.50000 14.55297
```

Without the weighted mean of `qsec`, this would simplify to

``` r

mtcars |>
  subset(mpg > 11, cyl, vs, am, mpg, carb, hp) |>
  group_by(cyl, vs, am) |> 
  fmean()
#   cyl vs am      mpg     carb        hp
# 1   4  0  1 26.00000 2.000000  91.00000
# 2   4  1  0 22.90000 1.666667  84.66667
# 3   4  1  1 28.37143 1.428571  80.57143
# 4   6  0  1 20.56667 4.666667 131.66667
# 5   6  1  0 19.12500 2.500000 115.25000
# 6   8  0  0 15.98000 2.900000 191.00000
# 7   8  0  1 15.40000 6.000000 299.50000
```

Finally, we could set the following options to toggle unsorted grouping,
no missing value skipping, and multithreading across the three columns
for more efficient execution.

``` r

mtcars |>
  subset(mpg > 11, cyl, vs, am, mpg, carb, hp) |>
  group_by(cyl, vs, am, sort = FALSE) |> 
  fmean(nthreads = 3, na.rm = FALSE)
#   cyl vs am      mpg     carb        hp
# 1   6  0  1 20.56667 4.666667 131.66667
# 2   4  1  1 28.37143 1.428571  80.57143
# 3   6  1  0 19.12500 2.500000 115.25000
# 4   8  0  0 15.98000 2.900000 191.00000
# 5   4  1  0 22.90000 1.666667  84.66667
# 6   4  0  1 26.00000 2.000000  91.00000
# 7   8  0  1 15.40000 6.000000 299.50000
```

Setting these options globally using
`set_collapse(sort = FALSE, nthreads = 3, na.rm = FALSE)` avoids the
need to set them repeatedly.

### Using Internal Grouping

Another key to writing efficient code with *collapse* is to avoid
[`fgroup_by()`](https://fastverse.org/collapse/reference/GRP.md) where
possible, especially for mutate operations. *collapse* does not
implement `.by` arguments to manipulation functions like *dplyr*, but
instead allows ad-hoc grouped transformations through its statistical
functions. For example, the easiest and fastest way to computed the
median of `mpg` by `cyl`, `vs`, and `am` is

``` r

mtcars |>
  mutate(mpg_median = fmedian(mpg, list(cyl, vs, am), TRA = "fill")) |> 
  head(3)
#                mpg cyl disp  hp drat    wt  qsec vs am gear carb mpg_median
# Mazda RX4     21.0   6  160 110 3.90 2.620 16.46  0  1    4    4       21.0
# Mazda RX4 Wag 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4       21.0
# Datsun 710    22.8   4  108  93 3.85 2.320 18.61  1  1    4    1       30.4
```

For the common case of averaging and centering data, *collapse* also
provides functions
[`fbetween()`](https://fastverse.org/collapse/reference/fbetween_fwithin.md)
for averaging and
[`fwithin()`](https://fastverse.org/collapse/reference/fbetween_fwithin.md)
for centering, i.e., `fbetween(mpg, list(cyl, vs, am))` is the same as
`fmean(mpg, list(cyl, vs, am), TRA = "fill")`. There is also
[`fscale()`](https://fastverse.org/collapse/reference/fscale.md) for
(grouped) scaling and centering.

This also applies to multiple columns, where we can use
`fmutate(across(...))` or
[`ftransformv()`](https://fastverse.org/collapse/reference/ftransform.md),
i.e. 

``` r

mtcars |>
  mutate(across(c(mpg, disp, qsec), fmedian, list(cyl, vs, am), TRA = "fill")) |> 
  head(2)
#               mpg cyl disp  hp drat    wt  qsec vs am gear carb
# Mazda RX4      21   6  160 110  3.9 2.620 16.46  0  1    4    4
# Mazda RX4 Wag  21   6  160 110  3.9 2.875 16.46  0  1    4    4

# Or 
mtcars |>
  transformv(c(mpg, disp, qsec), fmedian, list(cyl, vs, am), TRA = "fill") |> 
  head(2)
#               mpg cyl disp  hp drat    wt  qsec vs am gear carb
# Mazda RX4      21   6  160 110  3.9 2.620 16.46  0  1    4    4
# Mazda RX4 Wag  21   6  160 110  3.9 2.875 16.46  0  1    4    4
```

Of course, if we want to apply different functions using the same
grouping,
[`fgroup_by()`](https://fastverse.org/collapse/reference/GRP.md) is
sensible, but for mutate operations it also has the argument
`return.groups = FALSE`, which avoids materializing the unique grouping
columns, saving some memory.

``` r

mtcars |>
  group_by(cyl, vs, am, return.groups = FALSE) |> 
  mutate(mpg_median = fmedian(mpg), 
         mpg_mean = fmean(mpg), # Or fbetween(mpg)
         mpg_demean = fwithin(mpg), # Or fmean(mpg, TRA = "-")
         mpg_scale = fscale(mpg), 
         .keep = "used") |>
  ungroup() |>
  head(3)
#                mpg cyl vs am mpg_median mpg_mean mpg_demean  mpg_scale
# Mazda RX4     21.0   6  0  1       21.0 20.56667  0.4333333  0.5773503
# Mazda RX4 Wag 21.0   6  0  1       21.0 20.56667  0.4333333  0.5773503
# Datsun 710    22.8   4  1  1       30.4 28.37143 -5.5714286 -1.1710339
```

The `TRA` argument supports a whole array of operations, see
[`?TRA`](https://fastverse.org/collapse/reference/TRA.md). For example
`fsum(mtcars, TRA = "/")` turns the column vectors into proportions. As
an application of this, consider a generated dataset of sector-level
exports.

``` r

# c = country, s = sector, y = year, v = value
exports <- expand.grid(c = paste0("c", 1:8), s = paste0("s", 1:8), y = 1:15) |>
           mutate(v = round(abs(rnorm(length(c), mean = 5)), 2)) |>
           subset(-sample.int(length(v), 360)) # Making it unbalanced and irregular
head(exports)
#    c  s y    v
# 1 c2 s1 1 5.55
# 2 c3 s1 1 4.33
# 3 c4 s1 1 5.21
# 4 c5 s1 1 5.31
# 5 c6 s1 1 6.17
# 6 c7 s1 1 5.62
nrow(exports)
# [1] 600
```

It is very easy then to compute Balassa’s (1965) Revealed Comparative
Advantage (RCA) index, which is the share of a sector in country exports
divided by the share of the sector in world exports. An index above 1
indicates that a RCA of country c in sector s.

``` r

# Computing Balassa's (1965) RCA index: fast and memory efficient
# settfm() modifies exports and assigns it back to the global environment
settfm(exports, RCA = fsum(v, list(c, y), TRA = "/") %/=% fsum(fsum(v, y, TRA = "/"), list(s, y), TRA = "fill", set = TRUE))
```

Note that this involved a single expression with two different grouped
operations, which is only possible by incorporating grouping into
statistical functions themselves. Let’s summarise this dataset using
[`pivot()`](https://fastverse.org/collapse/reference/pivot.md) to
aggregate the RCA index across years. Here `"mean"` calls a highly
efficient internal mean function.

``` r

pivot(exports, ids = "c", values = "RCA", names = "s", 
      how = "wider", FUN = "mean", sort = TRUE)
#    c       s1       s2       s3       s4       s5       s6       s7       s8
# 1 c1 1.456983 1.674245 2.106907 1.715610 1.517669 2.058640 1.731403 1.533286
# 2 c2 2.196345 1.741839 1.925417 1.940657 1.422963 1.523795 1.385106 1.455789
# 3 c3 1.261560 1.552989 1.710201 1.420272 1.470105 1.531912 1.562338 1.307914
# 4 c4 1.455803 1.480939 1.558595 1.424664 1.213920 1.283873 1.631415 1.249383
# 5 c5 1.420965 1.616355 1.732715 1.465465 1.579685 1.252126 1.385581 1.359236
# 6 c6 1.445393 1.452775 1.872439 1.529396 1.464301 1.732497 1.331926 1.264625
# 7 c7 1.730497 1.627966 1.678039 1.710256 1.572039 1.798925 2.119763 1.451539
# 8 c8 1.763551 1.773720 1.730399 1.553112 1.419381 1.609315 1.715916 1.568516
```

We may also wish to investigate the growth rate of RCA. This can be done
using
[`fgrowth()`](https://fastverse.org/collapse/reference/fgrowth.md).
Since the panel is irregular, i.e., not every sector is observed in
every year, it is critical to also supply the time variable.

``` r

exports |> 
  mutate(RCA_growth = fgrowth(RCA, g = list(c, s), t = y)) |> 
  pivot(ids = "c", values = "RCA_growth", names = "s", 
        how = "wider", FUN = fmedian, sort = TRUE)
#    c         s1         s2          s3          s4         s5          s6         s7         s8
# 1 c1         NA -31.320346  33.2382015 -17.7150170 -19.521910   7.7699227 -11.166836   9.014163
# 2 c2   1.837294  60.313915   7.6639286 -36.3451812   7.657809   0.5202565 -17.252738  16.234799
# 3 c3 -17.644211  10.140848  39.3044351  -0.5140010 -27.571156 -15.3070853 -20.052042  -9.645808
# 4 c4  -3.619271  13.614077 -11.5213936 -29.1795219  12.698973  -2.8301315   9.579979   4.351506
# 5 c5 -11.267960   1.563708  49.2593990   0.6372803  12.894361 -10.7062506 -16.359597   1.331514
# 6 c6  -8.854774 -24.375237  -0.7098001  -0.6061250 -21.095221  17.3704638 -23.141631  -5.861039
# 7 c7   7.168700   9.169368 -51.7958299 -27.7699562  10.830523  23.9014624 -27.645297 -15.541500
# 8 c8  42.166200  -6.204723 114.3084929 -18.3894910 -17.674001  -3.4403949   1.342354 -38.826719
```

Lastly, since the panel is unbalanced, we may wish to create an RCA
index for only the last year, but balance the dataset a bit more by
taking the last available trade within the last three years. This can be
done using a single subset call

``` r

# Taking the latest observation within the last 3 years
exports_latest <- subset(exports, y > 12 & y == fmax(y, list(c, s), "fill"), -y)
# How many sectors do we observe for each country in the last 3 years?
with(exports_latest, fndistinct(s, c))
# c1 c2 c3 c4 c5 c6 c7 c8 
#  8  8  7  7  8  8  6  8
```

We can then compute the RCA index on this data

``` r

exports_latest |>
    mutate(RCA = fsum(v, c, TRA = "/") %/=% fsum(proportions(v), s, TRA = "fill")) |>
    pivot("c", "RCA", "s", how = "wider", sort = TRUE)
#    c        s1        s2        s3        s4        s5        s6        s7        s8
# 1 c1 0.9957444 1.0039325 1.2424563 0.9257392 0.8152179 1.3325429 0.7410637 1.0259104
# 2 c2 1.1416748 0.8007287 1.1660717 1.0364984 0.7154912 1.0625854 1.2649881 0.8687216
# 3 c3 1.1104473 0.9500677 1.3770016        NA 1.1941963 1.1301935 0.9773947 1.0015135
# 4 c4 0.8381306 1.2543034 1.1274679 1.3990983 1.3918678        NA 0.7364405 1.1539036
# 5 c5 0.8536024 0.8182961 0.9638389 1.6273503 1.0172714 0.8268992 1.0423516 1.0273071
# 6 c6 0.8465415 0.8878380 1.2123911 1.7417480 0.8812675 1.1393711 0.9840424 0.6626898
# 7 c7 1.0284817 1.2207153        NA        NA 1.2871187 1.4475702 1.2210074 1.3880608
# 8 c8 1.2217063 1.1452869 0.7166041 0.9448634 0.8388402 0.9760660 1.1123412 0.9686146
```

To summarise, *collapse* provides many options for ad-hoc or limited
grouping, which are faster than a full
[`fgroup_by()`](https://fastverse.org/collapse/reference/GRP.md), and
also syntactically efficient. Further efficiency gains are possible
using operations by reference, e.g., `%/=%` instead of `/` to avoid an
intermediate copy. It is also possible to transform by reference using
fast statistical functions by passing the `set = TRUE` argument, e.g.,
`with(mtcars, fmean(mpg, cyl, TRA = "fill", set = TRUE))` replaces `mpg`
by its group-averaged version (the transformed vector is returned
invisibly).

## Conclusion

*collapse* enhances R both statistically and computationally and is a
good option for *tidyverse* users searching for more efficient and
lightweight solutions to data manipulation and statistical computing
problems in R. For more information, I recommend starting with the short
vignette on [*Documentation
Resources*](https://fastverse.org/collapse/articles/collapse_documentation.html).

R users willing to write efficient/lightweight code and completely
replace the *tidyverse* in their workflow are also encouraged to closely
examine the [*fastverse*](https://fastverse.org/fastverse/) suite of
packages. *collapse* alone may not always suffice, but 99% of
*tidyverse* code can be replaced with an efficient and lightweight
*fastverse* solution.

[^1]: Notably, tidyselect, lambda expressions, and many of the smaller
    helper functions are left out.

[^2]: *collapse* can only read what you type,
    e.g. `exp <- substitute(fmean(mpg) + min(mpg))`, then
    `all_funs(exp)` gives `c("+", "fmean", "min")`, and
    `any(all_funs(exp) %in% .FAST_STAT_FUN)` returns `TRUE`, signifying
    to
    [`fsummarise()`](https://fastverse.org/collapse/reference/fsummarise.md)
    that the expression should be executed only once with the grouping
    object passed to the `g` argument of
    [`fmean()`](https://fastverse.org/collapse/reference/fmean.md),
    instead of it being executed once for every group.
