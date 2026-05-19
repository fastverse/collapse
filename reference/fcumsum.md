# Fast (Grouped, Ordered) Cumulative Sum for Matrix-Like Objects

`fcumsum` is a generic function that computes the (column-wise)
cumulative sum of `x`, (optionally) grouped by `g` and/or ordered by
`o`. Several options to deal with missing values are provided.

## Usage

``` r
fcumsum(x, ...)

# Default S3 method
fcumsum(x, g = NULL, o = NULL, na.rm = .op[["na.rm"]], fill = FALSE, check.o = TRUE, ...)

# S3 method for class 'matrix'
fcumsum(x, g = NULL, o = NULL, na.rm = .op[["na.rm"]], fill = FALSE, check.o = TRUE, ...)

# S3 method for class 'data.frame'
fcumsum(x, g = NULL, o = NULL, na.rm = .op[["na.rm"]], fill = FALSE, check.o = TRUE, ...)

# Methods for indexed data / compatibility with plm:

# S3 method for class 'pseries'
fcumsum(x, na.rm = .op[["na.rm"]], fill = FALSE, shift = "time", ...)

# S3 method for class 'pdata.frame'
fcumsum(x, na.rm = .op[["na.rm"]], fill = FALSE, shift = "time", ...)

# Methods for grouped data frame / compatibility with dplyr:

# S3 method for class 'grouped_df'
fcumsum(x, o = NULL, na.rm = .op[["na.rm"]], fill = FALSE, check.o = TRUE,
        keep.ids = TRUE, ...)
```

## Arguments

- x:

  a numeric vector / time series, (time series) matrix, data frame,
  'indexed_series' ('pseries'), 'indexed_frame' ('pdata.frame') or
  grouped data frame ('grouped_df').

- g:

  a factor, [`GRP`](https://fastverse.org/collapse/reference/GRP.md)
  object, or atomic vector / list of vectors (internally grouped with
  [`group`](https://fastverse.org/collapse/reference/group.md)) used to
  group `x`.

- o:

  a vector or list of vectors providing the order in which the elements
  of `x` are cumulatively summed. Will be passed to
  [`radixorderv`](https://fastverse.org/collapse/reference/radixorder.md)
  unless `check.o = FALSE`.

- na.rm:

  logical. Skip missing values in `x`. Defaults to `TRUE` and
  implemented at very little computational cost.

- fill:

  if `na.rm = TRUE`, setting `fill = TRUE` will overwrite missing values
  with the previous value of the cumulative sum, starting from 0.

- check.o:

  logical. Programmers option: `FALSE` prevents passing `o` to
  [`radixorderv`](https://fastverse.org/collapse/reference/radixorder.md),
  requiring `o` to be a valid ordering vector that is integer typed with
  each element in the range `[1, length(x)]`. This gives some extra
  speed, but will terminate R if any element of `o` is too large or too
  small.

- shift:

  *pseries / pdata.frame methods*: character. `"time"` or `"row"`. See
  [`flag`](https://fastverse.org/collapse/reference/flag.md) for
  details. The argument here does not control 'shifting' of data but
  rather the order in which elements are summed.

- keep.ids:

  *pdata.frame / grouped_df methods*: Logical. Drop all identifiers from
  the output (which includes all grouping variables and variables passed
  to `o`). *Note*: For grouped / panel data frames identifiers are
  dropped, but the `"groups"` / `"index"` attributes are kept.

- ...:

  arguments to be passed to or from other methods.

## Details

If `na.rm = FALSE`, `fcumsum` works like
[`cumsum`](https://rdrr.io/r/base/cumsum.html) and propagates missing
values. The default `na.rm = TRUE` skips missing values and computes the
cumulative sum on the non-missing values. Missing values are kept. If
`fill = TRUE`, missing values are replaced with the previous value of
the cumulative sum (starting from 0), computed on the non-missing
values.

By default the cumulative sum is computed in the order in which elements
appear in `x`. If `o` is provided, the cumulative sum is computed in the
order given by `radixorderv(o)`, without the need to first sort `x`.
This applies as well if groups are used (`g`), in which case the
cumulative sum is computed separately in each group.

The *pseries* and *pdata.frame* methods assume that the last factor in
the [index](https://fastverse.org/collapse/reference/indexing.md) is the
time-variable and the rest are grouping variables. The time-variable is
passed to `radixorderv` and used for ordered computation, so that
cumulative sums are accurately computed regardless of whether the
panel-data is ordered or balanced.

`fcumsum` explicitly supports integers. Integers in R are bounded at
bounded at +-2,147,483,647, and an integer overflow error will be
provided if the cumulative sum (within any group) exceeds
+-2,147,483,647. In that case data should be converted to double
beforehand.

## Value

the cumulative sum of values in `x`, (optionally) grouped by `g` and/or
ordered by `o`. See Details and Examples.

## See also

[`fdiff`](https://fastverse.org/collapse/reference/fdiff.md),
[`fgrowth`](https://fastverse.org/collapse/reference/fgrowth.md), [Time
Series and Panel
Series](https://fastverse.org/collapse/reference/time-series-panel-series.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Non-grouped
fcumsum(AirPassengers)
#>        Jan   Feb   Mar   Apr   May   Jun   Jul   Aug   Sep   Oct   Nov   Dec
#> 1949   112   230   362   491   612   747   895  1043  1179  1298  1402  1520
#> 1950  1635  1761  1902  2037  2162  2311  2481  2651  2809  2942  3056  3196
#> 1951  3341  3491  3669  3832  4004  4182  4381  4580  4764  4926  5072  5238
#> 1952  5409  5589  5782  5963  6146  6364  6594  6836  7045  7236  7408  7602
#> 1953  7798  7994  8230  8465  8694  8937  9201  9473  9710  9921 10101 10302
#>  [ reached 'max' / getOption("max.print") -- omitted 7 rows ]
head(fcumsum(EuStockMarkets))
#> Time Series:
#> Start = c(1991, 130) 
#> End = c(1991, 135) 
#> Frequency = 260 
#>              DAX     SMI     CAC    FTSE
#> 1991.496 1628.75  1678.1  1772.8  2443.6
#> 1991.500 3242.38  3366.6  3523.3  4903.8
#> 1991.504 4848.89  5045.2  5241.3  7352.0
#> 1991.508 6469.93  6729.3  6949.4  9822.4
#> 1991.512 8088.09  8415.9  8672.5 12307.1
#> 1991.515 9698.70 10087.5 10386.8 14773.9
fcumsum(mtcars)
#>                     mpg cyl disp  hp  drat     wt   qsec vs am gear carb
#> Mazda RX4          21.0   6  160 110  3.90  2.620  16.46  0  1    4    4
#> Mazda RX4 Wag      42.0  12  320 220  7.80  5.495  33.48  0  2    8    8
#> Datsun 710         64.8  16  428 313 11.65  7.815  52.09  1  3   12    9
#> Hornet 4 Drive     86.2  22  686 423 14.73 11.030  71.53  2  3   15   10
#> Hornet Sportabout 104.9  30 1046 598 17.88 14.470  88.55  2  3   18   12
#> Valiant           123.0  36 1271 703 20.64 17.930 108.77  3  3   21   13
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]

# Non-grouped but ordered
o <- order(rnorm(nrow(EuStockMarkets)))
all.equal(copyAttrib(fcumsum(EuStockMarkets[o, ], o = o)[order(o), ], EuStockMarkets),
          fcumsum(EuStockMarkets))
#> [1] TRUE

## Grouped
head(with(wlddev, fcumsum(PCGDP, iso3c)))
#> [1] NA NA NA NA NA NA

## Grouped and ordered
head(with(wlddev, fcumsum(PCGDP, iso3c, year)))
#> [1] NA NA NA NA NA NA
head(with(wlddev, fcumsum(PCGDP, iso3c, year, fill = TRUE)))
#> [1] 0 0 0 0 0 0
```
