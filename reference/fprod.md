# Fast (Grouped, Weighted) Product for Matrix-Like Objects

`fprod` is a generic function that computes the (column-wise) product of
all values in `x`, (optionally) grouped by `g` and/or weighted by `w`.
The [`TRA`](https://fastverse.org/collapse/reference/TRA.md) argument
can further be used to transform `x` using its (grouped, weighted)
product.

## Usage

``` r
fprod(x, ...)

# Default S3 method
fprod(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = TRUE, ...)

# S3 method for class 'matrix'
fprod(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = TRUE, drop = TRUE, ...)

# S3 method for class 'data.frame'
fprod(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = TRUE, drop = TRUE, ...)

# S3 method for class 'grouped_df'
fprod(x, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = FALSE, keep.group_vars = TRUE,
      keep.w = TRUE, stub = .op[["stub"]], ...)
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

- drop:

  *matrix and data.frame method:* Logical. `TRUE` drops dimensions and
  returns an atomic vector if `g = NULL` and `TRA = NULL`.

- keep.group_vars:

  *grouped_df method:* Logical. `FALSE` removes grouping variables after
  computation.

- keep.w:

  *grouped_df method:* Logical. Retain product of weighting variable
  after computation (if contained in `grouped_df`).

- stub:

  character. If `keep.w = TRUE` and `stub = TRUE` (default), the weights
  column is prefixed by `"prod."`. Users can specify a different prefix
  through this argument, or set it to `FALSE` to avoid prefixing.

- ...:

  arguments to be passed to or from other methods. If `TRA` is used,
  passing `set = TRUE` will transform data by reference and return the
  result invisibly.

## Details

Non-grouped product computations internally utilize long-doubles in C,
for additional numeric precision.

The weighted product is computed as `prod(x * w)`, using a single pass
in C. If `na.rm = TRUE`, missing values will be removed from both `x`
and `w` i.e. utilizing only `x[complete.cases(x,w)]` and
`w[complete.cases(x,w)]`.

For further computational details see
[`fsum`](https://fastverse.org/collapse/reference/fsum.md), which works
equivalently.

## Value

The (`w` weighted) product of `x`, grouped by `g`, or (if
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) is used) `x`
transformed by its (grouped, weighted) product.

## See also

[`fsum`](https://fastverse.org/collapse/reference/fsum.md), [Fast
Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## default vector method
mpg <- mtcars$mpg
fprod(mpg)                         # Simple product
#> [1] 1.264241e+41
fprod(mpg, w = mtcars$hp)          # Weighted product
#> [1] 8.870404e+108
fprod(mpg, TRA = "/")              # Simple transformation: Divide by product
#>  [1] 1.661076e-40 1.661076e-40 1.803454e-40 1.692716e-40 1.479149e-40
#>  [6] 1.431690e-40 1.131114e-40 1.930012e-40 1.803454e-40 1.518698e-40
#> [11] 1.407960e-40 1.297221e-40 1.368410e-40 1.202303e-40 8.226283e-41
#> [16] 8.226283e-41 1.162753e-40 2.562803e-40 2.404606e-40 2.681452e-40
#> [21] 1.700626e-40 1.226032e-40 1.202303e-40 1.052015e-40 1.518698e-40
#> [26] 2.159399e-40 2.056571e-40 2.404606e-40 1.249762e-40 1.558248e-40
#> [31] 1.186483e-40 1.692716e-40
fprod(mpg, mtcars$cyl)             # Grouped product
#>            4            6            8 
#> 4.204745e+15 1.150054e+09 2.614398e+16 
fprod(mpg, mtcars$cyl, mtcars$hp)  # Weighted grouped product
#>            4            6            8 
#> 3.686893e+36 4.255338e+23 5.653910e+48 
fprod(mpg, mtcars[c(2,8:9)])       # More groups..
#>        4.0.1        4.1.0        4.1.1        6.0.1        6.1.0        8.0.0 
#> 2.600000e+01 1.196088e+04 1.352082e+10 8.687700e+03 1.323773e+05 1.103122e+14 
#>        8.0.1 
#> 2.370000e+02 
g <- GRP(mtcars, ~ cyl + vs + am)  # Precomputing groups gives more speed !
fprod(mpg, g)
#>        4.0.1        4.1.0        4.1.1        6.0.1        6.1.0        8.0.0 
#> 2.600000e+01 1.196088e+04 1.352082e+10 8.687700e+03 1.323773e+05 1.103122e+14 
#>        8.0.1 
#> 2.370000e+02 
fprod(mpg, g, TRA = "/")           # Groupwise divide by product
#>  [1] 2.417211e-03 2.417211e-03 1.686288e-09 1.616591e-04 1.695190e-13
#>  [6] 1.367304e-04 1.296321e-13 2.039984e-03 1.906214e-03 1.450400e-04
#> [11] 1.344641e-04 1.486690e-13 1.568277e-13 1.377908e-13 9.427792e-14
#> [16] 9.427792e-14 1.332582e-13 2.396304e-09 2.248384e-09 2.507244e-09
#> [21] 1.797527e-03 1.405104e-13 1.377908e-13 1.205670e-13 1.740515e-13
#> [26] 2.019108e-09 1.000000e+00 2.248384e-09 6.666667e-02 2.267574e-03
#> [31] 6.329114e-02 1.582744e-09

## data.frame method
fprod(mtcars)
#>          mpg          cyl         disp           hp         drat           wt 
#> 1.264241e+41 5.163908e+24 2.789968e+73 7.016390e+67 4.366447e+17 3.884021e+15 
#>         qsec           vs           am         gear         carb 
#> 9.651882e+39 0.000000e+00 0.000000e+00 7.522960e+17 1.391569e+12 
head(fprod(mtcars, TRA = "/"))
#>                            mpg          cyl         disp           hp
#> Mazda RX4         1.661076e-40 1.161911e-24 5.734833e-72 1.567758e-66
#> Mazda RX4 Wag     1.661076e-40 1.161911e-24 5.734833e-72 1.567758e-66
#> Datsun 710        1.803454e-40 7.746072e-25 3.871012e-72 1.325468e-66
#> Hornet 4 Drive    1.692716e-40 1.161911e-24 9.247418e-72 1.567758e-66
#> Hornet Sportabout 1.479149e-40 1.549214e-24 1.290337e-71 2.494160e-66
#> Valiant           1.431690e-40 1.161911e-24 8.064609e-72 1.496496e-66
#>                           drat           wt         qsec  vs  am         gear
#> Mazda RX4         8.931747e-18 6.745586e-16 1.705367e-39 NaN Inf 5.317056e-18
#> Mazda RX4 Wag     8.931747e-18 7.402122e-16 1.763387e-39 NaN Inf 5.317056e-18
#> Datsun 710        8.817237e-18 5.973191e-16 1.928121e-39 Inf Inf 5.317056e-18
#> Hornet 4 Drive    7.053790e-18 8.277504e-16 2.014115e-39 Inf NaN 3.987792e-18
#> Hornet Sportabout 7.214103e-18 8.856800e-16 1.763387e-39 NaN NaN 3.987792e-18
#> Valiant           6.320928e-18 8.908293e-16 2.094928e-39 Inf NaN 3.987792e-18
#>                           carb
#> Mazda RX4         2.874452e-12
#> Mazda RX4 Wag     2.874452e-12
#> Datsun 710        7.186131e-13
#> Hornet 4 Drive    7.186131e-13
#> Hornet Sportabout 1.437226e-12
#> Valiant           7.186131e-13
fprod(mtcars, g)
#>                mpg         cyl         disp           hp         drat
#> 4.0.1 2.600000e+01           4 1.203000e+02 9.100000e+01      4.43000
#> 4.1.0 1.196088e+04          64 2.480709e+06 5.713300e+05     53.51976
#> 4.1.1 1.352082e+10       16384 4.158694e+13 1.686524e+13  20659.68036
#> 6.0.1 8.687700e+03         216 3.712000e+06 2.117500e+06     55.06020
#> 6.1.0 1.323773e+05        1296 1.630611e+09 1.747400e+08    130.62669
#> 8.0.0 1.103122e+14 68719476736 3.515354e+30 2.445279e+27 829662.18170
#>                 wt         qsec vs am   gear   carb
#> 4.0.1 2.140000e+00 1.670000e+01  0  1      5      2
#> 4.1.0 2.476955e+01 9.164580e+03  1  0     48      4
#> 4.1.1 1.231073e+02 7.933555e+08  1  1  20480      8
#> 6.0.1 2.086503e+01 4.342313e+03  0  1     80     96
#> 6.1.0 1.316358e+02 1.359535e+05  1  0    144     16
#> 8.0.0 1.914897e+07 6.360378e+14  0  0 531441 442368
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
fprod(mtcars, g, use.g.names = FALSE) # No row-names generated
#>            mpg         cyl         disp           hp         drat           wt
#> 1 2.600000e+01           4 1.203000e+02 9.100000e+01      4.43000 2.140000e+00
#> 2 1.196088e+04          64 2.480709e+06 5.713300e+05     53.51976 2.476955e+01
#> 3 1.352082e+10       16384 4.158694e+13 1.686524e+13  20659.68036 1.231073e+02
#> 4 8.687700e+03         216 3.712000e+06 2.117500e+06     55.06020 2.086503e+01
#> 5 1.323773e+05        1296 1.630611e+09 1.747400e+08    130.62669 1.316358e+02
#> 6 1.103122e+14 68719476736 3.515354e+30 2.445279e+27 829662.18170 1.914897e+07
#>           qsec vs am   gear   carb
#> 1 1.670000e+01  0  1      5      2
#> 2 9.164580e+03  1  0     48      4
#> 3 7.933555e+08  1  1  20480      8
#> 4 4.342313e+03  0  1     80     96
#> 5 1.359535e+05  1  0    144     16
#> 6 6.360378e+14  0  0 531441 442368
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

## matrix method
m <- qM(mtcars)
fprod(m)
#>          mpg          cyl         disp           hp         drat           wt 
#> 1.264241e+41 5.163908e+24 2.789968e+73 7.016390e+67 4.366447e+17 3.884021e+15 
#>         qsec           vs           am         gear         carb 
#> 9.651882e+39 0.000000e+00 0.000000e+00 7.522960e+17 1.391569e+12 
head(fprod(m, TRA = "/"))
#>                            mpg          cyl         disp           hp
#> Mazda RX4         1.661076e-40 1.161911e-24 5.734833e-72 1.567758e-66
#> Mazda RX4 Wag     1.661076e-40 1.161911e-24 5.734833e-72 1.567758e-66
#> Datsun 710        1.803454e-40 7.746072e-25 3.871012e-72 1.325468e-66
#> Hornet 4 Drive    1.692716e-40 1.161911e-24 9.247418e-72 1.567758e-66
#> Hornet Sportabout 1.479149e-40 1.549214e-24 1.290337e-71 2.494160e-66
#> Valiant           1.431690e-40 1.161911e-24 8.064609e-72 1.496496e-66
#>                           drat           wt         qsec  vs  am         gear
#> Mazda RX4         8.931747e-18 6.745586e-16 1.705367e-39 NaN Inf 5.317056e-18
#> Mazda RX4 Wag     8.931747e-18 7.402122e-16 1.763387e-39 NaN Inf 5.317056e-18
#> Datsun 710        8.817237e-18 5.973191e-16 1.928121e-39 Inf Inf 5.317056e-18
#> Hornet 4 Drive    7.053790e-18 8.277504e-16 2.014115e-39 Inf NaN 3.987792e-18
#> Hornet Sportabout 7.214103e-18 8.856800e-16 1.763387e-39 NaN NaN 3.987792e-18
#> Valiant           6.320928e-18 8.908293e-16 2.094928e-39 Inf NaN 3.987792e-18
#>                           carb
#> Mazda RX4         2.874452e-12
#> Mazda RX4 Wag     2.874452e-12
#> Datsun 710        7.186131e-13
#> Hornet 4 Drive    7.186131e-13
#> Hornet Sportabout 1.437226e-12
#> Valiant           7.186131e-13
fprod(m, g) # etc..
#>                mpg         cyl         disp           hp         drat
#> 4.0.1 2.600000e+01           4 1.203000e+02 9.100000e+01      4.43000
#> 4.1.0 1.196088e+04          64 2.480709e+06 5.713300e+05     53.51976
#> 4.1.1 1.352082e+10       16384 4.158694e+13 1.686524e+13  20659.68036
#> 6.0.1 8.687700e+03         216 3.712000e+06 2.117500e+06     55.06020
#> 6.1.0 1.323773e+05        1296 1.630611e+09 1.747400e+08    130.62669
#> 8.0.0 1.103122e+14 68719476736 3.515354e+30 2.445279e+27 829662.18170
#>                 wt         qsec vs am   gear   carb
#> 4.0.1 2.140000e+00 1.670000e+01  0  1      5      2
#> 4.1.0 2.476955e+01 9.164580e+03  1  0     48      4
#> 4.1.1 1.231073e+02 7.933555e+08  1  1  20480      8
#> 6.0.1 2.086503e+01 4.342313e+03  0  1     80     96
#> 6.1.0 1.316358e+02 1.359535e+05  1  0    144     16
#> 8.0.0 1.914897e+07 6.360378e+14  0  0 531441 442368
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]

## method for grouped data frames - created with dplyr::group_by or fgroup_by
mtcars |> fgroup_by(cyl,vs,am) |> fprod()
#>   cyl vs am          mpg         disp           hp         drat           wt
#> 1   4  0  1 2.600000e+01 1.203000e+02 9.100000e+01      4.43000 2.140000e+00
#> 2   4  1  0 1.196088e+04 2.480709e+06 5.713300e+05     53.51976 2.476955e+01
#> 3   4  1  1 1.352082e+10 4.158694e+13 1.686524e+13  20659.68036 1.231073e+02
#> 4   6  0  1 8.687700e+03 3.712000e+06 2.117500e+06     55.06020 2.086503e+01
#> 5   6  1  0 1.323773e+05 1.630611e+09 1.747400e+08    130.62669 1.316358e+02
#> 6   8  0  0 1.103122e+14 3.515354e+30 2.445279e+27 829662.18170 1.914897e+07
#>           qsec   gear   carb
#> 1 1.670000e+01      5      2
#> 2 9.164580e+03     48      4
#> 3 7.933555e+08  20480      8
#> 4 4.342313e+03     80     96
#> 5 1.359535e+05    144     16
#> 6 6.360378e+14 531441 442368
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
mtcars |> fgroup_by(cyl,vs,am) |> fprod(TRA = "/")
#>                   cyl vs am          mpg         disp           hp         drat
#> Mazda RX4           6  0  1 2.417211e-03 4.310345e-05 5.194805e-05 7.083156e-02
#> Mazda RX4 Wag       6  0  1 2.417211e-03 4.310345e-05 5.194805e-05 7.083156e-02
#> Datsun 710          4  1  1 1.686288e-09 2.596969e-12 5.514301e-12 1.863533e-04
#> Hornet 4 Drive      6  1  0 1.616591e-04 1.582229e-07 6.295069e-07 2.357864e-02
#> Hornet Sportabout   8  0  0 1.695190e-13 1.024079e-28 7.156647e-26 3.796726e-06
#> Valiant             6  1  0 1.367304e-04 1.379851e-07 6.008929e-07 2.112891e-02
#>                             wt         qsec         gear         carb
#> Mazda RX4         1.255690e-01 3.790607e-03 5.000000e-02 4.166667e-02
#> Mazda RX4 Wag     1.377904e-01 3.919570e-03 5.000000e-02 4.166667e-02
#> Datsun 710        1.884534e-02 2.345733e-08 1.953125e-04 1.250000e-01
#> Hornet 4 Drive    2.442345e-02 1.429901e-04 2.083333e-02 6.250000e-02
#> Hornet Sportabout 1.796441e-07 2.675942e-14 5.645029e-06 4.521123e-06
#> Valiant           2.628465e-02 1.487274e-04 2.083333e-02 6.250000e-02
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 
mtcars |> fgroup_by(cyl,vs,am) |> fselect(mpg) |> fprod()
#>   cyl vs am          mpg
#> 1   4  0  1 2.600000e+01
#> 2   4  1  0 1.196088e+04
#> 3   4  1  1 1.352082e+10
#> 4   6  0  1 8.687700e+03
#> 5   6  1  0 1.323773e+05
#> 6   8  0  0 1.103122e+14
#> 7   8  0  1 2.370000e+02
```
