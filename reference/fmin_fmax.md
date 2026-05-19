# Fast (Grouped) Maxima and Minima for Matrix-Like Objects

`fmax` and `fmin` are generic functions that compute the (column-wise)
maximum and minimum value of all values in `x`, (optionally) grouped by
`g`. The [`TRA`](https://fastverse.org/collapse/reference/TRA.md)
argument can further be used to transform `x` using its (grouped)
maximum or minimum value.

## Usage

``` r
fmax(x, ...)
fmin(x, ...)

# Default S3 method
fmax(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, ...)
# Default S3 method
fmin(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, ...)

# S3 method for class 'matrix'
fmax(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, drop = TRUE, ...)
# S3 method for class 'matrix'
fmin(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, drop = TRUE, ...)

# S3 method for class 'data.frame'
fmax(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, drop = TRUE, ...)
# S3 method for class 'data.frame'
fmin(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, drop = TRUE, ...)

# S3 method for class 'grouped_df'
fmax(x, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = FALSE, keep.group_vars = TRUE, ...)
# S3 method for class 'grouped_df'
fmin(x, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = FALSE, keep.group_vars = TRUE, ...)
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

- drop:

  *matrix and data.frame method:* Logical. `TRUE` drops dimensions and
  returns an atomic vector if `g = NULL` and `TRA = NULL`.

- keep.group_vars:

  *grouped_df method:* Logical. `FALSE` removes grouping variables after
  computation.

- ...:

  arguments to be passed to or from other methods. If `TRA` is used,
  passing `set = TRUE` will transform data by reference and return the
  result invisibly.

## Details

Missing-value removal as controlled by the `na.rm` argument is done at
no extra cost since in C++ any logical comparison involving `NA` or
`NaN` evaluates to `FALSE`. Large performance gains can nevertheless be
achieved in the presence of missing values if `na.rm = FALSE`, since
then the corresponding computation is terminated once a `NA` is
encountered and `NA` is returned (unlike
[`max`](https://rdrr.io/r/base/Extremes.html) and
[`min`](https://rdrr.io/r/base/Extremes.html) which just run through
without any checks).

For further computational details see
[`fsum`](https://fastverse.org/collapse/reference/fsum.md).

## Value

`fmax` returns the maximum value of `x`, grouped by `g`, or (if
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) is used) `x`
transformed by its (grouped) maximum value. Analogous, `fmin` returns
the minimum value ...

## See also

[Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## default vector method
mpg <- mtcars$mpg
fmax(mpg)                         # Maximum value
#> [1] 33.9
fmin(mpg)                         # Minimum value (all examples below use fmax but apply to fmin)
#> [1] 10.4
fmax(mpg, TRA = "%")              # Simple transformation: Take percentage of maximum value
#>  [1]  61.94690  61.94690  67.25664  63.12684  55.16224  53.39233  42.18289
#>  [8]  71.97640  67.25664  56.63717  52.50737  48.37758  51.03245  44.83776
#> [15]  30.67847  30.67847  43.36283  95.57522  89.67552 100.00000  63.42183
#> [22]  45.72271  44.83776  39.23304  56.63717  80.53097  76.69617  89.67552
#> [29]  46.60767  58.11209  44.24779  63.12684
fmax(mpg, mtcars$cyl)             # Grouped maximum value
#>    4    6    8 
#> 33.9 21.4 19.2 
fmax(mpg, mtcars[c(2,8:9)])       # More groups..
#> 4.0.1 4.1.0 4.1.1 6.0.1 6.1.0 8.0.0 8.0.1 
#>  26.0  24.4  33.9  21.0  21.4  19.2  15.8 
g <- GRP(mtcars, ~ cyl + vs + am) # Precomputing groups gives more speed !
fmax(mpg, g)
#> 4.0.1 4.1.0 4.1.1 6.0.1 6.1.0 8.0.0 8.0.1 
#>  26.0  24.4  33.9  21.0  21.4  19.2  15.8 
fmax(mpg, g, TRA = "%")           # Groupwise percentage of maximum value
#>  [1] 100.00000 100.00000  67.25664 100.00000  97.39583  84.57944  74.47917
#>  [8] 100.00000  93.44262  89.71963  83.17757  85.41667  90.10417  79.16667
#> [15]  54.16667  54.16667  76.56250  95.57522  89.67552 100.00000  88.11475
#> [22]  80.72917  79.16667  69.27083 100.00000  80.53097 100.00000  89.67552
#> [29] 100.00000  93.80952  94.93671  63.12684
fmax(mpg, g, TRA = "replace")     # Groupwise replace by maximum value
#>  [1] 21.0 21.0 33.9 21.4 19.2 21.4 19.2 24.4 24.4 21.4 21.4 19.2 19.2 19.2 19.2
#> [16] 19.2 19.2 33.9 33.9 33.9 24.4 19.2 19.2 19.2 19.2 33.9 26.0 33.9 15.8 21.0
#> [31] 15.8 33.9

## data.frame method
fmax(mtcars)
#>     mpg     cyl    disp      hp    drat      wt    qsec      vs      am    gear 
#>  33.900   8.000 472.000 335.000   4.930   5.424  22.900   1.000   1.000   5.000 
#>    carb 
#>   8.000 
head(fmax(mtcars, TRA = "%"))
#>                        mpg cyl     disp       hp     drat       wt     qsec  vs
#> Mazda RX4         61.94690  75 33.89831 32.83582 79.10751 48.30383 71.87773   0
#> Mazda RX4 Wag     61.94690  75 33.89831 32.83582 79.10751 53.00516 74.32314   0
#> Datsun 710        67.25664  50 22.88136 27.76119 78.09331 42.77286 81.26638 100
#> Hornet 4 Drive    63.12684  75 54.66102 32.83582 62.47465 59.27360 84.89083 100
#> Hornet Sportabout 55.16224 100 76.27119 52.23881 63.89452 63.42183 74.32314   0
#> Valiant           53.39233  75 47.66949 31.34328 55.98377 63.79056 88.29694 100
#>                    am gear carb
#> Mazda RX4         100   80 50.0
#> Mazda RX4 Wag     100   80 50.0
#> Datsun 710        100   80 12.5
#> Hornet 4 Drive      0   60 12.5
#> Hornet Sportabout   0   60 25.0
#> Valiant             0   60 12.5
fmax(mtcars, g)
#>        mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> 4.0.1 26.0   4 120.3  91 4.43 2.140 16.70  0  1    5    2
#> 4.1.0 24.4   4 146.7  97 3.92 3.190 22.90  1  0    4    2
#> 4.1.1 33.9   4 121.0 113 4.93 2.780 19.90  1  1    5    2
#> 6.0.1 21.0   6 160.0 175 3.90 2.875 17.02  0  1    5    6
#> 6.1.0 21.4   6 258.0 123 3.92 3.460 20.22  1  0    4    4
#> 8.0.0 19.2   8 472.0 245 3.73 5.424 18.00  0  0    3    4
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
fmax(mtcars, g, use.g.names = FALSE) # No row-names generated
#>    mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> 1 26.0   4 120.3  91 4.43 2.140 16.70  0  1    5    2
#> 2 24.4   4 146.7  97 3.92 3.190 22.90  1  0    4    2
#> 3 33.9   4 121.0 113 4.93 2.780 19.90  1  1    5    2
#> 4 21.0   6 160.0 175 3.90 2.875 17.02  0  1    5    6
#> 5 21.4   6 258.0 123 3.92 3.460 20.22  1  0    4    4
#> 6 19.2   8 472.0 245 3.73 5.424 18.00  0  0    3    4
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

## matrix method
m <- qM(mtcars)
fmax(m)
#>     mpg     cyl    disp      hp    drat      wt    qsec      vs      am    gear 
#>  33.900   8.000 472.000 335.000   4.930   5.424  22.900   1.000   1.000   5.000 
#>    carb 
#>   8.000 
head(fmax(m, TRA = "%"))
#>                        mpg cyl     disp       hp     drat       wt     qsec  vs
#> Mazda RX4         61.94690  75 33.89831 32.83582 79.10751 48.30383 71.87773   0
#> Mazda RX4 Wag     61.94690  75 33.89831 32.83582 79.10751 53.00516 74.32314   0
#> Datsun 710        67.25664  50 22.88136 27.76119 78.09331 42.77286 81.26638 100
#> Hornet 4 Drive    63.12684  75 54.66102 32.83582 62.47465 59.27360 84.89083 100
#> Hornet Sportabout 55.16224 100 76.27119 52.23881 63.89452 63.42183 74.32314   0
#> Valiant           53.39233  75 47.66949 31.34328 55.98377 63.79056 88.29694 100
#>                    am gear carb
#> Mazda RX4         100   80 50.0
#> Mazda RX4 Wag     100   80 50.0
#> Datsun 710        100   80 12.5
#> Hornet 4 Drive      0   60 12.5
#> Hornet Sportabout   0   60 25.0
#> Valiant             0   60 12.5
fmax(m, g) # etc..
#>        mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> 4.0.1 26.0   4 120.3  91 4.43 2.140 16.70  0  1    5    2
#> 4.1.0 24.4   4 146.7  97 3.92 3.190 22.90  1  0    4    2
#> 4.1.1 33.9   4 121.0 113 4.93 2.780 19.90  1  1    5    2
#> 6.0.1 21.0   6 160.0 175 3.90 2.875 17.02  0  1    5    6
#> 6.1.0 21.4   6 258.0 123 3.92 3.460 20.22  1  0    4    4
#> 8.0.0 19.2   8 472.0 245 3.73 5.424 18.00  0  0    3    4
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]

## method for grouped data frames - created with dplyr::group_by or fgroup_by
mtcars |> fgroup_by(cyl,vs,am) |> fmax()
#>   cyl vs am  mpg  disp  hp drat    wt  qsec gear carb
#> 1   4  0  1 26.0 120.3  91 4.43 2.140 16.70    5    2
#> 2   4  1  0 24.4 146.7  97 3.92 3.190 22.90    4    2
#> 3   4  1  1 33.9 121.0 113 4.93 2.780 19.90    5    2
#> 4   6  0  1 21.0 160.0 175 3.90 2.875 17.02    5    6
#> 5   6  1  0 21.4 258.0 123 3.92 3.460 20.22    4    4
#> 6   8  0  0 19.2 472.0 245 3.73 5.424 18.00    3    4
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
mtcars |> fgroup_by(cyl,vs,am) |> fmax("%")
#>                   cyl vs am       mpg      disp       hp      drat        wt
#> Mazda RX4           6  0  1 100.00000 100.00000 62.85714 100.00000  91.13043
#> Mazda RX4 Wag       6  0  1 100.00000 100.00000 62.85714 100.00000 100.00000
#> Datsun 710          4  1  1  67.25664  89.25620 82.30088  78.09331  83.45324
#> Hornet 4 Drive      6  1  0 100.00000 100.00000 89.43089  78.57143  92.91908
#> Hornet Sportabout   8  0  0  97.39583  76.27119 71.42857  84.45040  63.42183
#> Valiant             6  1  0  84.57944  87.20930 85.36585  70.40816 100.00000
#>                        qsec gear     carb
#> Mazda RX4          96.70975   80 66.66667
#> Mazda RX4 Wag     100.00000   80 66.66667
#> Datsun 710         93.51759   80 50.00000
#> Hornet 4 Drive     96.14243   75 25.00000
#> Hornet Sportabout  94.55556  100 50.00000
#> Valiant           100.00000   75 25.00000
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 
mtcars |> fgroup_by(cyl,vs,am) |> fselect(mpg) |> fmax()
#>   cyl vs am  mpg
#> 1   4  0  1 26.0
#> 2   4  1  0 24.4
#> 3   4  1  1 33.9
#> 4   6  0  1 21.0
#> 5   6  1  0 21.4
#> 6   8  0  0 19.2
#> 7   8  0  1 15.8
```
