# Fast (Grouped, Weighted) Mean for Matrix-Like Objects

`fmean` is a generic function that computes the (column-wise) mean of
`x`, (optionally) grouped by `g` and/or weighted by `w`. The
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) argument can
further be used to transform `x` using its (grouped, weighted) mean.

## Usage

``` r
fmean(x, ...)

# Default S3 method
fmean(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = TRUE, nthreads = .op[["nthreads"]], ...)

# S3 method for class 'matrix'
fmean(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = TRUE, drop = TRUE, nthreads = .op[["nthreads"]], ...)

# S3 method for class 'data.frame'
fmean(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = TRUE, drop = TRUE, nthreads = .op[["nthreads"]], ...)

# S3 method for class 'grouped_df'
fmean(x, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = FALSE, keep.group_vars = TRUE,
      keep.w = TRUE, stub = .op[["stub"]], nthreads = .op[["nthreads"]], ...)
```

## Arguments

- x:

  a numeric vector, matrix, data frame or grouped data frame (class
  'grouped_df').

- g:

  a factor, [`GRP`](https://fastverse.org/collapse/reference/GRP.md)
  object, atomic vector (internally converted to factor) or a list of
  vectors / factors (internally converted to a
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object) used
  to group `x`.

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

- nthreads:

  integer. The number of threads to utilize. See Details of
  [`fsum`](https://fastverse.org/collapse/reference/fsum.md).

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

The weighted mean is computed as `sum(x * w) / sum(w)`, using a single
pass in C. If `na.rm = TRUE`, missing values will be removed from both
`x` and `w` i.e. utilizing only `x[complete.cases(x,w)]` and
`w[complete.cases(x,w)]`.

For further computational details see
[`fsum`](https://fastverse.org/collapse/reference/fsum.md), which works
equivalently.

## Value

The (`w` weighted) mean of `x`, grouped by `g`, or (if
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) is used) `x`
transformed by its (grouped, weighted) mean.

## See also

[`fmedian`](https://fastverse.org/collapse/reference/fnth_fmedian.md),
[`fmode`](https://fastverse.org/collapse/reference/fmode.md), [Fast
Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## default vector method
mpg <- mtcars$mpg
fmean(mpg)                         # Simple mean
#> [1] 20.09063
fmean(mpg, w = mtcars$hp)          # Weighted mean: Weighted by hp
#> [1] 17.97245
fmean(mpg, TRA = "-")              # Simple transformation: demeaning (See also ?W)
#>  [1]  0.909375  0.909375  2.709375  1.309375 -1.390625 -1.990625 -5.790625
#>  [8]  4.309375  2.709375 -0.890625 -2.290625 -3.690625 -2.790625 -4.890625
#> [15] -9.690625 -9.690625 -5.390625 12.309375 10.309375 13.809375  1.409375
#> [22] -4.590625 -4.890625 -6.790625 -0.890625  7.209375  5.909375 10.309375
#> [29] -4.290625 -0.390625 -5.090625  1.309375
fmean(mpg, mtcars$cyl)             # Grouped mean
#>        4        6        8 
#> 26.66364 19.74286 15.10000 
fmean(mpg, mtcars[8:9])            # another grouped mean.
#>      0.0      0.1      1.0      1.1 
#> 15.05000 19.75000 20.74286 28.37143 
g <- GRP(mtcars[c(2,8:9)])
fmean(mpg, g)                      # Pre-computing groups speeds up the computation
#>    4.0.1    4.1.0    4.1.1    6.0.1    6.1.0    8.0.0    8.0.1 
#> 26.00000 22.90000 28.37143 20.56667 19.12500 15.05000 15.40000 
fmean(mpg, g, mtcars$hp)           # Grouped weighted mean
#>    4.0.1    4.1.0    4.1.1    6.0.1    6.1.0    8.0.0    8.0.1 
#> 26.00000 22.69409 27.68209 20.42405 19.10087 14.82854 15.35259 
fmean(mpg, g, TRA = "-")           # Demeaning by group
#>  [1]  0.4333333  0.4333333 -5.5714286  2.2750000  3.6500000 -1.0250000
#>  [7] -0.7500000  1.5000000 -0.1000000  0.0750000 -1.3250000  1.3500000
#> [13]  2.2500000  0.1500000 -4.6500000 -4.6500000 -0.3500000  4.0285714
#> [19]  2.0285714  5.5285714 -1.4000000  0.4500000  0.1500000 -1.7500000
#> [25]  4.1500000 -1.0714286  0.0000000  2.0285714  0.4000000 -0.8666667
#> [31] -0.4000000 -6.9714286
fmean(mpg, g, mtcars$hp, "-")      # Group-demeaning using weighted group means
#>  [1]  0.57594937  0.57594937 -4.88209220  2.29913232  3.87145923 -1.00086768
#>  [7] -0.52854077  1.70590551  0.10590551  0.09913232 -1.30086768  1.57145923
#> [13]  2.47145923  0.37145923 -4.42854077 -4.42854077 -0.12854077  4.71790780
#> [19]  2.71790780  6.21790780 -1.19409449  0.67145923  0.37145923 -1.52854077
#> [25]  4.37145923 -0.38209220  0.00000000  2.71790780  0.44741235 -0.72405063
#> [31] -0.35258765 -6.28209220

## data.frame method
fmean(mtcars)
#>        mpg        cyl       disp         hp       drat         wt       qsec 
#>  20.090625   6.187500 230.721875 146.687500   3.596563   3.217250  17.848750 
#>         vs         am       gear       carb 
#>   0.437500   0.406250   3.687500   2.812500 
fmean(mtcars, g)
#>            mpg cyl     disp        hp     drat       wt     qsec vs am     gear
#> 4.0.1 26.00000   4 120.3000  91.00000 4.430000 2.140000 16.70000  0  1 5.000000
#> 4.1.0 22.90000   4 135.8667  84.66667 3.770000 2.935000 20.97000  1  0 3.666667
#> 4.1.1 28.37143   4  89.8000  80.57143 4.148571 2.028286 18.70000  1  1 4.142857
#> 6.0.1 20.56667   6 155.0000 131.66667 3.806667 2.755000 16.32667  0  1 4.333333
#> 6.1.0 19.12500   6 204.5500 115.25000 3.420000 3.388750 19.21500  1  0 3.500000
#> 8.0.0 15.05000   8 357.6167 194.16667 3.120833 4.104083 17.14250  0  0 3.000000
#>           carb
#> 4.0.1 2.000000
#> 4.1.0 1.666667
#> 4.1.1 1.428571
#> 6.0.1 4.666667
#> 6.1.0 2.500000
#> 8.0.0 3.083333
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
fmean(fgroup_by(mtcars, cyl, vs, am))  # Another way of doing it..
#>   cyl vs am      mpg     disp        hp     drat       wt     qsec     gear
#> 1   4  0  1 26.00000 120.3000  91.00000 4.430000 2.140000 16.70000 5.000000
#> 2   4  1  0 22.90000 135.8667  84.66667 3.770000 2.935000 20.97000 3.666667
#> 3   4  1  1 28.37143  89.8000  80.57143 4.148571 2.028286 18.70000 4.142857
#> 4   6  0  1 20.56667 155.0000 131.66667 3.806667 2.755000 16.32667 4.333333
#> 5   6  1  0 19.12500 204.5500 115.25000 3.420000 3.388750 19.21500 3.500000
#> 6   8  0  0 15.05000 357.6167 194.16667 3.120833 4.104083 17.14250 3.000000
#>       carb
#> 1 2.000000
#> 2 1.666667
#> 3 1.428571
#> 4 4.666667
#> 5 2.500000
#> 6 3.083333
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
head(fmean(mtcars, g, TRA = "-"))      # etc..
#>                          mpg cyl      disp        hp        drat         wt
#> Mazda RX4          0.4333333   0  5.000000 -21.66667  0.09333333 -0.1350000
#> Mazda RX4 Wag      0.4333333   0  5.000000 -21.66667  0.09333333  0.1200000
#> Datsun 710        -5.5714286   0 18.200000  12.42857 -0.29857143  0.2917143
#> Hornet 4 Drive     2.2750000   0 53.450000  -5.25000 -0.34000000 -0.1737500
#> Hornet Sportabout  3.6500000   0  2.383333 -19.16667  0.02916667 -0.6640833
#> Valiant           -1.0250000   0 20.450000 -10.25000 -0.66000000  0.0712500
#>                         qsec vs am       gear       carb
#> Mazda RX4          0.1333333  0  0 -0.3333333 -0.6666667
#> Mazda RX4 Wag      0.6933333  0  0 -0.3333333 -0.6666667
#> Datsun 710        -0.0900000  0  0 -0.1428571 -0.4285714
#> Hornet 4 Drive     0.2250000  0  0 -0.5000000 -1.5000000
#> Hornet Sportabout -0.1225000  0  0  0.0000000 -1.0833333
#> Valiant            1.0050000  0  0 -0.5000000 -1.5000000

## matrix method
m <- qM(mtcars)
fmean(m)
#>        mpg        cyl       disp         hp       drat         wt       qsec 
#>  20.090625   6.187500 230.721875 146.687500   3.596563   3.217250  17.848750 
#>         vs         am       gear       carb 
#>   0.437500   0.406250   3.687500   2.812500 
fmean(m, g)
#>            mpg cyl     disp        hp     drat       wt     qsec vs am     gear
#> 4.0.1 26.00000   4 120.3000  91.00000 4.430000 2.140000 16.70000  0  1 5.000000
#> 4.1.0 22.90000   4 135.8667  84.66667 3.770000 2.935000 20.97000  1  0 3.666667
#> 4.1.1 28.37143   4  89.8000  80.57143 4.148571 2.028286 18.70000  1  1 4.142857
#> 6.0.1 20.56667   6 155.0000 131.66667 3.806667 2.755000 16.32667  0  1 4.333333
#> 6.1.0 19.12500   6 204.5500 115.25000 3.420000 3.388750 19.21500  1  0 3.500000
#> 8.0.0 15.05000   8 357.6167 194.16667 3.120833 4.104083 17.14250  0  0 3.000000
#>           carb
#> 4.0.1 2.000000
#> 4.1.0 1.666667
#> 4.1.1 1.428571
#> 6.0.1 4.666667
#> 6.1.0 2.500000
#> 8.0.0 3.083333
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
head(fmean(m, g, TRA = "-")) # etc..
#>                          mpg cyl      disp        hp        drat         wt
#> Mazda RX4          0.4333333   0  5.000000 -21.66667  0.09333333 -0.1350000
#> Mazda RX4 Wag      0.4333333   0  5.000000 -21.66667  0.09333333  0.1200000
#> Datsun 710        -5.5714286   0 18.200000  12.42857 -0.29857143  0.2917143
#> Hornet 4 Drive     2.2750000   0 53.450000  -5.25000 -0.34000000 -0.1737500
#> Hornet Sportabout  3.6500000   0  2.383333 -19.16667  0.02916667 -0.6640833
#> Valiant           -1.0250000   0 20.450000 -10.25000 -0.66000000  0.0712500
#>                         qsec vs am       gear       carb
#> Mazda RX4          0.1333333  0  0 -0.3333333 -0.6666667
#> Mazda RX4 Wag      0.6933333  0  0 -0.3333333 -0.6666667
#> Datsun 710        -0.0900000  0  0 -0.1428571 -0.4285714
#> Hornet 4 Drive     0.2250000  0  0 -0.5000000 -1.5000000
#> Hornet Sportabout -0.1225000  0  0  0.0000000 -1.0833333
#> Valiant            1.0050000  0  0 -0.5000000 -1.5000000

## method for grouped data frames - created with dplyr::group_by or fgroup_by
mtcars |> fgroup_by(cyl,vs,am) |> fmean()         # Ordinary
#>   cyl vs am      mpg     disp        hp     drat       wt     qsec     gear
#> 1   4  0  1 26.00000 120.3000  91.00000 4.430000 2.140000 16.70000 5.000000
#> 2   4  1  0 22.90000 135.8667  84.66667 3.770000 2.935000 20.97000 3.666667
#> 3   4  1  1 28.37143  89.8000  80.57143 4.148571 2.028286 18.70000 4.142857
#> 4   6  0  1 20.56667 155.0000 131.66667 3.806667 2.755000 16.32667 4.333333
#> 5   6  1  0 19.12500 204.5500 115.25000 3.420000 3.388750 19.21500 3.500000
#> 6   8  0  0 15.05000 357.6167 194.16667 3.120833 4.104083 17.14250 3.000000
#>       carb
#> 1 2.000000
#> 2 1.666667
#> 3 1.428571
#> 4 4.666667
#> 5 2.500000
#> 6 3.083333
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
mtcars |> fgroup_by(cyl,vs,am) |> fmean(hp)       # Weighted
#>   cyl vs am sum.hp      mpg      disp     drat       wt     qsec     gear
#> 1   4  0  1     91 26.00000 120.30000 4.430000 2.140000 16.70000 5.000000
#> 2   4  1  0    254 22.69409 134.33504 3.779843 2.898169 21.08846 3.618110
#> 3   4  1  1    564 27.68209  93.87482 4.080266 2.067223 18.54041 4.200355
#> 4   6  0  1    395 20.42405 153.35443 3.775949 2.757468 16.19063 4.443038
#> 5   6  1  0    461 19.10087 202.24425 3.455358 3.390868 19.16941 3.533623
#> 6   8  0  0   2330 14.82854 363.10815 3.143090 4.158685 17.08489 3.000000
#>       carb
#> 1 2.000000
#> 2 1.618110
#> 3 1.485816
#> 4 4.886076
#> 5 2.600868
#> 6 3.210300
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
mtcars |> fgroup_by(cyl,vs,am) |> fmean(hp, "-")  # Weighted Transform
#>                   cyl vs am  hp        mpg      disp         drat          wt
#> Mazda RX4           6  0  1 110  0.5759494  6.645570  0.124050633 -0.13746835
#> Mazda RX4 Wag       6  0  1 110  0.5759494  6.645570  0.124050633  0.11753165
#> Datsun 710          4  1  1  93 -4.8820922 14.125177 -0.230265957  0.25277660
#> Hornet 4 Drive      6  1  0 110  2.2991323 55.755748 -0.375357918 -0.17586768
#> Hornet Sportabout   8  0  0 175  3.8714592 -3.108155  0.006909871 -0.71868455
#> Valiant             6  1  0 105 -1.0008677 22.755748 -0.695357918  0.06913232
#>                         qsec       gear       carb
#> Mazda RX4          0.2693671 -0.4430380 -0.8860759
#> Mazda RX4 Wag      0.8293671 -0.4430380 -0.8860759
#> Datsun 710         0.0695922 -0.2003546 -0.4858156
#> Hornet 4 Drive     0.2705857 -0.5336226 -1.6008677
#> Hornet Sportabout -0.0648927  0.0000000 -1.2103004
#> Valiant            1.0505857 -0.5336226 -1.6008677
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 
mtcars |> fgroup_by(cyl,vs,am) |>
               fselect(mpg,hp) |> fmean(hp, "-")  # Only mpg
#>                      hp         mpg
#> Mazda RX4           110  0.57594937
#> Mazda RX4 Wag       110  0.57594937
#> Datsun 710           93 -4.88209220
#> Hornet 4 Drive      110  2.29913232
#> Hornet Sportabout   175  3.87145923
#> Valiant             105 -1.00086768
#> Duster 360          245 -0.52854077
#> Merc 240D            62  1.70590551
#> Merc 230             95  0.10590551
#> Merc 280            123  0.09913232
#> Merc 280C           123 -1.30086768
#> Merc 450SE          180  1.57145923
#> Merc 450SL          180  2.47145923
#> Merc 450SLC         180  0.37145923
#> Cadillac Fleetwood  205 -4.42854077
#> Lincoln Continental 215 -4.42854077
#> Chrysler Imperial   230 -0.12854077
#> Fiat 128             66  4.71790780
#> Honda Civic          52  2.71790780
#> Toyota Corolla       65  6.21790780
#> Toyota Corona        97 -1.19409449
#> Dodge Challenger    150  0.67145923
#> AMC Javelin         150  0.37145923
#> Camaro Z28          245 -1.52854077
#> Pontiac Firebird    175  4.37145923
#> Fiat X1-9            66 -0.38209220
#> Porsche 914-2        91  0.00000000
#> Lotus Europa        113  2.71790780
#> Ford Pantera L      264  0.44741235
#> Ferrari Dino        175 -0.72405063
#> Maserati Bora       335 -0.35258765
#> Volvo 142E          109 -6.28209220
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 
```
