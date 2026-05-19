# Fast (Grouped, Weighted) Variance and Standard Deviation for Matrix-Like Objects

`fvar` and `fsd` are generic functions that compute the (column-wise)
variance and standard deviation of `x`, (optionally) grouped by `g`
and/or frequency-weighted by `w`. The
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) argument can
further be used to transform `x` using its (grouped, weighted)
variance/sd.

## Usage

``` r
fvar(x, ...)
fsd(x, ...)

# Default S3 method
fvar(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, stable.algo = .op[["stable.algo"]], ...)
# Default S3 method
fsd(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
    use.g.names = TRUE, stable.algo = .op[["stable.algo"]], ...)

# S3 method for class 'matrix'
fvar(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, drop = TRUE, stable.algo = .op[["stable.algo"]], ...)
# S3 method for class 'matrix'
fsd(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
    use.g.names = TRUE, drop = TRUE, stable.algo = .op[["stable.algo"]], ...)

# S3 method for class 'data.frame'
fvar(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = TRUE, drop = TRUE, stable.algo = .op[["stable.algo"]], ...)
# S3 method for class 'data.frame'
fsd(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
    use.g.names = TRUE, drop = TRUE, stable.algo = .op[["stable.algo"]], ...)

# S3 method for class 'grouped_df'
fvar(x, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
     use.g.names = FALSE, keep.group_vars = TRUE, keep.w = TRUE,
     stub = .op[["stub"]], stable.algo = .op[["stable.algo"]], ...)
# S3 method for class 'grouped_df'
fsd(x, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
    use.g.names = FALSE, keep.group_vars = TRUE, keep.w = TRUE,
    stub = .op[["stub"]], stable.algo = .op[["stable.algo"]], ...)
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

  *grouped_df method:* Logical. Retain summed weighting variable after
  computation (if contained in `grouped_df`).

- stub:

  character. If `keep.w = TRUE` and `stub = TRUE` (default), the summed
  weights column is prefixed by `"sum."`. Users can specify a different
  prefix through this argument, or set it to `FALSE` to avoid prefixing.

- stable.algo:

  logical. `TRUE` (default) use Welford's numerically stable online
  algorithm. `FALSE` implements a faster but numerically unstable
  one-pass method. See Details.

- ...:

  arguments to be passed to or from other methods. If `TRA` is used,
  passing `set = TRUE` will transform data by reference and return the
  result invisibly.

## Details

*Welford's online algorithm* used by default to compute the variance is
well described
[here](https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance)
(the section *Weighted incremental algorithm* also shows how the
weighted variance is obtained by this algorithm).

If `stable.algo = FALSE`, the variance is computed in one-pass as
`(sum(x^2)-n*mean(x)^2)/(n-1)`, where `sum(x^2)` is the sum of squares
from which the expected sum of squares `n*mean(x)^2` is subtracted,
normalized by `n-1` (Bessel's correction). This is numerically unstable
if `sum(x^2)` and `n*mean(x)^2` are large numbers very close together,
which will be the case for large `n`, large `x`-values and small
variances (catastrophic cancellation occurs, leading to a loss of
numeric precision). Numeric precision is however still maximized through
the internal use of long doubles in C++, and the fast algorithm can be
up to 4-times faster compared to Welford's method.

The weighted variance is computed with frequency weights as
`(sum(x^2*w)-sum(w)*weighted.mean(x,w)^2)/(sum(w)-1)`. If
`na.rm = TRUE`, missing values will be removed from both `x` and `w`
i.e. utilizing only `x[complete.cases(x,w)]` and
`w[complete.cases(x,w)]`.

For further computational detail see
[`fsum`](https://fastverse.org/collapse/reference/fsum.md).

## Value

`fvar` returns the (`w` weighted) variance of `x`, grouped by `g`, or
(if [`TRA`](https://fastverse.org/collapse/reference/TRA.md) is used)
`x` transformed by its (grouped, weighted) variance. `fsd` computes the
standard deviation of `x` in like manor.

## References

Welford, B. P. (1962). Note on a method for calculating corrected sums
of squares and products. *Technometrics*. 4 (3): 419-420.
doi:10.2307/1266577.

## See also

[Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## default vector method
fvar(mtcars$mpg)                            # Simple variance (all examples also hold for fvar!)
#> [1] 36.3241
fsd(mtcars$mpg)                             # Simple standard deviation
#> [1] 6.026948
fsd(mtcars$mpg, w = mtcars$hp)              # Weighted sd: Weighted by hp
#> [1] 5.150858
fsd(mtcars$mpg, TRA = "/")                  # Simple transformation: scaling (See also ?fscale)
#>  [1] 3.484351 3.484351 3.783009 3.550719 3.102731 3.003178 2.372677 4.048484
#>  [9] 3.783009 3.185692 2.953402 2.721112 2.870441 2.522006 1.725583 1.725583
#> [17] 2.439045 5.375855 5.044012 5.624737 3.567311 2.571783 2.522006 2.206755
#> [25] 3.185692 4.529656 4.313958 5.044012 2.621559 3.268653 2.488822 3.550719
fsd(mtcars$mpg, mtcars$cyl)                 # Grouped sd
#>        4        6        8 
#> 4.509828 1.453567 2.560048 
fsd(mtcars$mpg, mtcars$cyl, mtcars$hp)      # Grouped weighted sd
#>        4        6        8 
#> 4.250863 1.294689 2.390448 
fsd(mtcars$mpg, mtcars$cyl, TRA = "/")      # Scaling by group
#>  [1] 14.447218 14.447218  5.055626 14.722403  7.304550 12.452126  5.585833
#>  [8]  5.410406  5.055626 13.208885 12.245737  6.406130  6.757686  5.937388
#> [15]  4.062424  4.062424  5.742080  7.184310  6.740834  7.516917  4.767366
#> [22]  6.054574  5.937388  5.195215  7.499859  6.053446  5.765187  6.740834
#> [29]  6.171759 13.552866  5.859265  4.745192
fsd(mtcars$mpg, mtcars$cyl, mtcars$hp, "/") # Group-scaling using weighted group sds
#>  [1] 16.220111 16.220111  5.363617 16.529066  7.822800 13.980191  5.982141
#>  [8]  5.740011  5.363617 14.829816 13.748475  6.860638  7.237136  6.358640
#> [15]  4.350648  4.350648  6.149474  7.621982  7.151489  7.974852  5.057797
#> [22]  6.484139  6.358640  5.563810  8.031966  6.422226  6.116405  7.151489
#> [29]  6.609639 15.216009  6.274973  5.034272

## data.frame method
fsd(iris)                           # This works, although 'Species' is a factor variable
#> Sepal.Length  Sepal.Width Petal.Length  Petal.Width      Species 
#>    0.8280661    0.4358663    1.7652982    0.7622377    0.8192319 
fsd(mtcars, drop = FALSE)           # This works, all columns are numeric variables
#>        mpg      cyl     disp       hp      drat        wt     qsec        vs
#> 1 6.026948 1.785922 123.9387 68.56287 0.5346787 0.9784574 1.786943 0.5040161
#>          am      gear   carb
#> 1 0.4989909 0.7378041 1.6152
fsd(iris[-5], iris[5])              # By Species: iris[5] is still a list, and thus passed to GRP()
#>            Sepal.Length Sepal.Width Petal.Length Petal.Width
#> setosa        0.3524897   0.3790644    0.1736640   0.1053856
#> versicolor    0.5161711   0.3137983    0.4699110   0.1977527
#> virginica     0.6358796   0.3224966    0.5518947   0.2746501
fsd(iris[-5], iris[[5]])            # Same thing much faster: fsd recognizes 'Species' is a factor
#>            Sepal.Length Sepal.Width Petal.Length Petal.Width
#> setosa        0.3524897   0.3790644    0.1736640   0.1053856
#> versicolor    0.5161711   0.3137983    0.4699110   0.1977527
#> virginica     0.6358796   0.3224966    0.5518947   0.2746501
head(fsd(iris[-5], iris[[5]], TRA = "/")) # Data scaled by species (see also fscale)
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1     14.46851    9.233260     8.061544    1.897793
#> 2     13.90112    7.914223     8.061544    1.897793
#> 3     13.33372    8.441838     7.485720    1.897793
#> 4     13.05003    8.178031     8.637369    1.897793
#> 5     14.18481    9.497068     8.061544    1.897793
#> 6     15.31960   10.288490     9.789018    3.795585

## matrix method
m <- qM(mtcars)
fsd(m)
#>         mpg         cyl        disp          hp        drat          wt 
#>   6.0269481   1.7859216 123.9386938  68.5628685   0.5346787   0.9784574 
#>        qsec          vs          am        gear        carb 
#>   1.7869432   0.5040161   0.4989909   0.7378041   1.6152000 
fsd(m, mtcars$cyl) # etc..
#>        mpg cyl     disp       hp      drat        wt     qsec        vs
#> 4 4.509828   0 26.87159 20.93453 0.3654711 0.5695637 1.682445 0.3015113
#> 6 1.453567   0 41.56246 24.26049 0.4760552 0.3563455 1.706866 0.5345225
#> 8 2.560048   0 67.77132 50.97689 0.3723618 0.7594047 1.196014 0.0000000
#>          am      gear     carb
#> 4 0.4670994 0.5393599 0.522233
#> 6 0.5345225 0.6900656 1.812654
#> 8 0.3631365 0.7262730 1.556624

## method for grouped data frames - created with dplyr::group_by or fgroup_by
mtcars |> fgroup_by(cyl,vs,am) |> fsd()
#>   cyl vs am       mpg      disp       hp      drat        wt      qsec
#> 1   4  0  1        NA        NA       NA        NA        NA        NA
#> 2   4  1  0 1.4525839 13.969371 19.65536 0.1300000 0.4075230 1.6714365
#> 3   4  1  1 4.7577005 18.802128 24.14441 0.3783926 0.4400840 0.9454628
#> 4   6  0  1 0.7505553  8.660254 37.52777 0.1616581 0.1281601 0.7687219
#> 5   6  1  0 1.6317169 44.742634  9.17878 0.5919459 0.1162164 0.8159044
#> 6   8  0  0 2.7743959 71.823494 33.35984 0.2302749 0.7683069 0.8016475
#>        gear      carb
#> 1        NA        NA
#> 2 0.5773503 0.5773503
#> 3 0.3779645 0.5345225
#> 4 0.5773503 1.1547005
#> 5 0.5773503 1.7320508
#> 6 0.0000000 0.9003366
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
mtcars |> fgroup_by(cyl,vs,am) |> fsd(keep.group_vars = FALSE) # Remove grouping columns
#>         mpg      disp       hp      drat        wt       qsec      gear
#> 1        NA        NA       NA        NA        NA         NA        NA
#> 2 1.4525839 13.969371 19.65536 0.1300000 0.4075230 1.67143651 0.5773503
#> 3 4.7577005 18.802128 24.14441 0.3783926 0.4400840 0.94546285 0.3779645
#> 4 0.7505553  8.660254 37.52777 0.1616581 0.1281601 0.76872188 0.5773503
#> 5 1.6317169 44.742634  9.17878 0.5919459 0.1162164 0.81590441 0.5773503
#> 6 2.7743959 71.823494 33.35984 0.2302749 0.7683069 0.80164745 0.0000000
#> 7 0.5656854 35.355339 50.20458 0.4808326 0.2828427 0.07071068 0.0000000
#>        carb
#> 1        NA
#> 2 0.5773503
#> 3 0.5345225
#> 4 1.1547005
#> 5 1.7320508
#> 6 0.9003366
#> 7 2.8284271
mtcars |> fgroup_by(cyl,vs,am) |> fsd(hp)      # Weighted by hp
#>   cyl vs am sum.hp       mpg      disp      drat         wt      qsec      gear
#> 1   4  0  1     91 0.0000000  0.000000 0.0000000 0.00000000 0.0000000 0.0000000
#> 2   4  1  0    254 1.1242936 11.439070 0.1086204 0.34150141 1.4030344 0.4868090
#> 3   4  1  1    564 4.5643045 17.861603 0.3117619 0.44698787 0.9335417 0.4006210
#> 4   6  0  1    395 0.6465871  7.460621 0.1392649 0.09592878 0.6512685 0.4973747
#> 5   6  1  0    461 1.3956451 38.774265 0.5094278 0.09888402 0.7006934 0.4994102
#> 6   8  0  0   2330 2.6621685 68.845963 0.2331010 0.75554498 0.8299946 0.0000000
#>        carb
#> 1 0.0000000
#> 2 0.4868090
#> 3 0.5002424
#> 4 0.9947494
#> 5 1.4982306
#> 6 0.8510728
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
mtcars |> fgroup_by(cyl,vs,am) |> fsd(hp, "/") # Weighted scaling transformation
#>                   cyl vs am  hp       mpg      disp      drat        wt
#> Mazda RX4           6  0  1 110 32.478221 21.445937 28.004181 27.311929
#> Mazda RX4 Wag       6  0  1 110 32.478221 21.445937 28.004181 29.970151
#> Datsun 710          4  1  1  93  4.995285  6.046490 12.349167  5.190297
#> Hornet 4 Drive      6  1  0 110 15.333411  6.653898  6.045999 32.512836
#> Hornet Sportabout   8  0  0 175  7.024349  5.229065 13.513454  4.553005
#> Valiant             6  1  0 105 12.968913  5.802818  5.417844 34.990486
#>                       qsec     gear     carb
#> Mazda RX4         25.27376 8.042226 4.021113
#> Mazda RX4 Wag     26.13362 8.042226 4.021113
#> Datsun 710        19.93483 9.984498 1.999031
#> Hornet 4 Drive    27.74395 6.007086 0.667454
#> Hornet Sportabout 20.50616      Inf 2.349975
#> Valiant           28.85713 6.007086 0.667454
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 
```
