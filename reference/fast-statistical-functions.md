# Fast (Grouped, Weighted) Statistical Functions for Matrix-Like Objects

With [`fsum`](https://fastverse.org/collapse/reference/fsum.md),
[`fprod`](https://fastverse.org/collapse/reference/fprod.md),
[`fmean`](https://fastverse.org/collapse/reference/fmean.md),
[`fmedian`](https://fastverse.org/collapse/reference/fnth_fmedian.md),
[`fmode`](https://fastverse.org/collapse/reference/fmode.md),
[`fvar`](https://fastverse.org/collapse/reference/fvar_fsd.md),
[`fsd`](https://fastverse.org/collapse/reference/fvar_fsd.md),
[`fmin`](https://fastverse.org/collapse/reference/fmin_fmax.md),
[`fmax`](https://fastverse.org/collapse/reference/fmin_fmax.md),
[`fnth`](https://fastverse.org/collapse/reference/fnth_fmedian.md),
[`ffirst`](https://fastverse.org/collapse/reference/ffirst_flast.md),
[`flast`](https://fastverse.org/collapse/reference/ffirst_flast.md),
[`fnobs`](https://fastverse.org/collapse/reference/fnobs.md) and
[`fndistinct`](https://fastverse.org/collapse/reference/fndistinct.md),
*collapse* presents a coherent set of extremely fast and flexible
statistical functions (S3 generics) to perform column-wise, grouped and
weighted computations on vectors, matrices and data frames, with special
support for grouped data frames / tibbles (*dplyr*) and *data.table*'s.

## Usage


    ## All functions (FUN) follow a common syntax in 4 methods:
    FUN(x, ...)

    ## Default S3 method:
    FUN(x, g = NULL, [w = NULL,] TRA = NULL, [na.rm = TRUE,]
        use.g.names = TRUE, [nthreads = 1L,] ...)

    ## S3 method for class 'matrix'
    FUN(x, g = NULL, [w = NULL,] TRA = NULL, [na.rm = TRUE,]
        use.g.names = TRUE, drop = TRUE, [nthreads = 1L,] ...)

    ## S3 method for class 'data.frame'
    FUN(x, g = NULL, [w = NULL,] TRA = NULL, [na.rm = TRUE,]
        use.g.names = TRUE, drop = TRUE, [nthreads = 1L,] ...)

    ## S3 method for class 'grouped_df'
    FUN(x, [w = NULL,] TRA = NULL, [na.rm = TRUE,]
        use.g.names = FALSE, keep.group_vars = TRUE,
        [keep.w = TRUE,] [stub = TRUE,] [nthreads = 1L,] ...)

## Arguments

|  |  |  |
|----|----|----|
| `x` |  | a vector, matrix, data frame or grouped data frame (class 'grouped_df'). |
| `g` |  | a factor, [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object, atomic vector (internally converted to factor) or a list of vectors / factors (internally converted to a [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object) used to group `x`. |
| `w` |  | a numeric vector of (non-negative) weights, may contain missing values. Supported by [`fsum`](https://fastverse.org/collapse/reference/fsum.md), [`fprod`](https://fastverse.org/collapse/reference/fprod.md), [`fmean`](https://fastverse.org/collapse/reference/fmean.md), [`fmedian`](https://fastverse.org/collapse/reference/fnth_fmedian.md), [`fnth`](https://fastverse.org/collapse/reference/fnth_fmedian.md), [`fvar`](https://fastverse.org/collapse/reference/fvar_fsd.md), [`fsd`](https://fastverse.org/collapse/reference/fvar_fsd.md) and [`fmode`](https://fastverse.org/collapse/reference/fmode.md). |
| `TRA` |  | an integer or quoted operator indicating the transformation to perform: 0 - "na" \| 1 - "fill" \| 2 - "replace" \| 3 - "-" \| 4 - "-+" \| 5 - "/" \| 6 - "%" \| 7 - "+" \| 8 - "\*" \| 9 - "%%" \| 10 - "-%%". See [`TRA`](https://fastverse.org/collapse/reference/TRA.md). |
| `na.rm` |  | logical. Skip missing values in `x`. Defaults to `TRUE` in all functions and implemented at very little computational cost. Not available for [`fnobs`](https://fastverse.org/collapse/reference/fnobs.md). |
| `use.g.names` |  | logical. Make group-names and add to the result as names (default method) or row-names (matrix and data frame methods). No row-names are generated for *data.table*'s. |
| `nthreads` |  | integer. The number of threads to utilize. Supported by [`fsum`](https://fastverse.org/collapse/reference/fsum.md), [`fmean`](https://fastverse.org/collapse/reference/fmean.md), [`fmedian`](https://fastverse.org/collapse/reference/fnth_fmedian.md), [`fnth`](https://fastverse.org/collapse/reference/fnth_fmedian.md), [`fmode`](https://fastverse.org/collapse/reference/fmode.md) and [`fndistinct`](https://fastverse.org/collapse/reference/fndistinct.md). |
| `drop` |  | *matrix and data.frame methods:* Logical. `TRUE` drops dimensions and returns an atomic vector if `g = NULL` and `TRA = NULL`. |
| `keep.group_vars` |  | *grouped_df method:* Logical. `FALSE` removes grouping variables after computation. By default grouping variables are added, even if not present in the grouped_df. |
| `keep.w` |  | *grouped_df method:* Logical. `TRUE` (default) also aggregates weights and saves them in a column, `FALSE` removes weighting variable after computation (if contained in `grouped_df`). |
| `stub` |  | *grouped_df method:* Character. If `keep.w = TRUE` and `stub = TRUE` (default), the aggregated weights column is prefixed by the name of the aggregation function (mostly `"sum."`). Users can specify a different prefix through this argument, or set it to `FALSE` to avoid prefixing. |
| `...` |  | arguments to be passed to or from other methods. If `TRA` is used, passing `set = TRUE` will transform data by reference and return the result invisibly (except for the grouped_df method which always returns visible output). |

## Details

Please see the documentation of individual functions.

## Value

`x` suitably aggregated or transformed. Data frame column-attributes and
overall attributes are generally preserved if the output is of the same
data type.

## Related Functionality

- Functions
  [`fquantile`](https://fastverse.org/collapse/reference/fquantile.md)
  and [`frange`](https://fastverse.org/collapse/reference/fquantile.md)
  are for atomic vectors.

- Panel-decomposed (i.e. between and within) statistics as well as
  grouped and weighted skewness and kurtosis are implemented in
  [`qsu`](https://fastverse.org/collapse/reference/qsu.md).

- The vector-valued functions and operators
  [`fcumsum`](https://fastverse.org/collapse/reference/fcumsum.md),
  [`fscale/STD`](https://fastverse.org/collapse/reference/fscale.md),
  [`fbetween/B`](https://fastverse.org/collapse/reference/fbetween_fwithin.md),
  [`fhdbetween/HDB`](https://fastverse.org/collapse/reference/fhdbetween_fhdwithin.md),
  [`fwithin/W`](https://fastverse.org/collapse/reference/fbetween_fwithin.md),
  [`fhdwithin/HDW`](https://fastverse.org/collapse/reference/fhdbetween_fhdwithin.md),
  [`flag/L/F`](https://fastverse.org/collapse/reference/flag.md),
  [`fdiff/D/Dlog`](https://fastverse.org/collapse/reference/fdiff.md)
  and [`fgrowth/G`](https://fastverse.org/collapse/reference/fgrowth.md)
  are grouped under [Data
  Transformations](https://fastverse.org/collapse/reference/data-transformations.md)
  and [Time Series and Panel
  Series](https://fastverse.org/collapse/reference/time-series-panel-series.md).
  These functions also support [indexed
  data](https://fastverse.org/collapse/reference/indexing.md) (*plm*).

## See also

[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md),
[Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md),
[Time Series and Panel
Series](https://fastverse.org/collapse/reference/time-series-panel-series.md)

## Examples


    ## default vector method
    mpg <- mtcars$mpg
    fsum(mpg)                         # Simple sum
    fsum(mpg, TRA = "/")              # Simple transformation: divide all values by the sum
    fsum(mpg, mtcars$cyl)             # Grouped sum
    fmean(mpg, mtcars$cyl)            # Grouped mean
    fmean(mpg, w = mtcars$hp)         # Weighted mean, weighted by hp
    fmean(mpg, mtcars$cyl, mtcars$hp) # Grouped mean, weighted by hp
    fsum(mpg, mtcars$cyl, TRA = "/")  # Proportions / division by group sums
    fmean(mpg, mtcars$cyl, mtcars$hp, # Subtract weighted group means, see also ?fwithin
          TRA = "-")

    ## data.frame method
    fsum(mtcars)
    fsum(mtcars, TRA = "%")                  # This computes percentages
    fsum(mtcars, mtcars[c(2,8:9)])           # Grouped column sum
    g <- GRP(mtcars, ~ cyl + vs + am)        # Here precomputing the groups!
    fsum(mtcars, g)                          # Faster !!
    fmean(mtcars, g, mtcars$hp)
    fmean(mtcars, g, mtcars$hp, "-")         # Demeaning by weighted group means..
    fmean(fgroup_by(mtcars, cyl, vs, am), hp, "-")  # Another way of doing it..


    fmode(wlddev, drop = FALSE)              # Compute statistical modes of variables in this data
    fmode(wlddev, wlddev$income)             # Grouped statistical modes ..

    ## matrix method
    m <- qM(mtcars)
    fsum(m)
    fsum(m, g) # ..

    ## method for grouped data frames - created with dplyr::group_by or fgroup_by
    library(dplyr)
    mtcars |> group_by(cyl,vs,am) |> select(mpg,carb) |> fsum()
    mtcars |> fgroup_by(cyl,vs,am) |> fselect(mpg,carb) |> fsum() # equivalent and faster !!
    mtcars |> fgroup_by(cyl,vs,am) |> fsum(TRA = "%")
    mtcars |> fgroup_by(cyl,vs,am) |> fmean(hp)         # weighted grouped mean, save sum of weights
    mtcars |> fgroup_by(cyl,vs,am) |> fmean(hp, keep.group_vars = FALSE)

## Benchmark


    ## This compares fsum with data.table (2 threads) and base::rowsum
    # Starting with small data
    mtcDT <- qDT(mtcars)
    f <- qF(mtcars$cyl)

    library(microbenchmark)
    microbenchmark(mtcDT[, lapply(.SD, sum), by = f],
                   rowsum(mtcDT, f, reorder = FALSE),
                   fsum(mtcDT, f, na.rm = FALSE), unit = "relative")

    #                              expr        min         lq      mean    median        uq       max neval cld
    # mtcDT[, lapply(.SD, sum), by = f] 145.436928 123.542134 88.681111 98.336378 71.880479 85.217726   100   c
    # rowsum(mtcDT, f, reorder = FALSE)   2.833333   2.798203  2.489064  2.937889  2.425724  2.181173   100  b
    #     fsum(mtcDT, f, na.rm = FALSE)   1.000000   1.000000  1.000000  1.000000  1.000000  1.000000   100 a

    # Now larger data
    tdata <- qDT(replicate(100, rnorm(1e5), simplify = FALSE)) # 100 columns with 100.000 obs
    f <- qF(sample.int(1e4, 1e5, TRUE))                        # A factor with 10.000 groups

    microbenchmark(tdata[, lapply(.SD, sum), by = f],
                   rowsum(tdata, f, reorder = FALSE),
                   fsum(tdata, f, na.rm = FALSE), unit = "relative")

    #                              expr      min       lq     mean   median       uq       max neval cld
    # tdata[, lapply(.SD, sum), by = f] 2.646992 2.975489 2.834771 3.081313 3.120070 1.2766475   100   c
    # rowsum(tdata, f, reorder = FALSE) 1.747567 1.753313 1.629036 1.758043 1.839348 0.2720937   100  b
    #     fsum(tdata, f, na.rm = FALSE) 1.000000 1.000000 1.000000 1.000000 1.000000 1.0000000   100 a
