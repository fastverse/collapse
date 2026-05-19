# Fast (Grouped) Observation Count for Matrix-Like Objects

`fnobs` is a generic function that (column-wise) computes the number of
non-missing values in `x`, (optionally) grouped by `g`. It is much
faster than `sum(!is.na(x))`. The
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) argument can
further be used to transform `x` using its (grouped) observation count.

## Usage

``` r
fnobs(x, ...)

# Default S3 method
fnobs(x, g = NULL, TRA = NULL, use.g.names = TRUE, ...)

# S3 method for class 'matrix'
fnobs(x, g = NULL, TRA = NULL, use.g.names = TRUE, drop = TRUE, ...)

# S3 method for class 'data.frame'
fnobs(x, g = NULL, TRA = NULL, use.g.names = TRUE, drop = TRUE, ...)

# S3 method for class 'grouped_df'
fnobs(x, TRA = NULL, use.g.names = FALSE, keep.group_vars = TRUE, ...)
```

## Arguments

- x:

  a vector, matrix, data frame or grouped data frame (class
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

`fnobs` preserves all attributes of non-classed vectors / columns, and
only the 'label' attribute (if available) of classed vectors / columns
(i.e. dates or factors). When applied to data frames and matrices, the
row-names are adjusted as necessary.

## Value

Integer. The number of non-missing observations in `x`, grouped by `g`,
or (if [`TRA`](https://fastverse.org/collapse/reference/TRA.md) is used)
`x` transformed by its number of non-missing observations, grouped by
`g`.

## See also

[`fndistinct`](https://fastverse.org/collapse/reference/fndistinct.md),
[Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## default vector method
fnobs(airquality$Solar.R)                   # Simple Nobs
#> [1] 146
fnobs(airquality$Solar.R, airquality$Month) # Grouped Nobs
#>  5  6  7  8  9 
#> 27 30 31 28 30 

## data.frame method
fnobs(airquality)
#>   Ozone Solar.R    Wind    Temp   Month     Day 
#>     116     146     153     153     153     153 
fnobs(airquality, airquality$Month)
#>   Ozone Solar.R Wind Temp Month Day
#> 5    26      27   31   31    31  31
#> 6     9      30   30   30    30  30
#> 7    26      31   31   31    31  31
#> 8    26      28   31   31    31  31
#> 9    29      30   30   30    30  30
fnobs(wlddev)                               # Works with data of all types!
#> country   iso3c    date    year  decade  region  income    OECD   PCGDP  LIFEEX 
#>   13176   13176   13176   13176   13176   13176   13176   13176    9470   11670 
#>    GINI     ODA     POP 
#>    1744    8608   12919 
head(fnobs(wlddev, wlddev$iso3c))
#>     country iso3c date year decade region income OECD PCGDP LIFEEX GINI ODA POP
#> ABW      61    61   61   61     61     61     61   61    32     60    0  20  60
#> AFG      61    61   61   61     61     61     61   61    18     60    0  60  60
#> AGO      61    61   61   61     61     61     61   61    40     60    3  58  60
#> ALB      61    61   61   61     61     61     61   61    40     60    9  32  60
#> AND      61    61   61   61     61     61     61   61    50      0    0   0  60
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

## matrix method
aqm <- qM(airquality)
fnobs(aqm)                                  # Also works for character or logical matrices
#>   Ozone Solar.R    Wind    Temp   Month     Day 
#>     116     146     153     153     153     153 
fnobs(aqm, airquality$Month)
#>   Ozone Solar.R Wind Temp Month Day
#> 5    26      27   31   31    31  31
#> 6     9      30   30   30    30  30
#> 7    26      31   31   31    31  31
#> 8    26      28   31   31    31  31
#> 9    29      30   30   30    30  30

## method for grouped data frames - created with dplyr::group_by or fgroup_by
airquality |> fgroup_by(Month) |> fnobs()
#>   Month Ozone Solar.R Wind Temp Day
#> 1     5    26      27   31   31  31
#> 2     6     9      30   30   30  30
#> 3     7    26      31   31   31  31
#> 4     8    26      28   31   31  31
#> 5     9    29      30   30   30  30
wlddev |> fgroup_by(country) |>
           fselect(PCGDP,LIFEEX,GINI,ODA) |> fnobs()
#>                country PCGDP LIFEEX GINI ODA
#> 1          Afghanistan    18     60    0  60
#> 2              Albania    40     60    9  32
#> 3              Algeria    60     60    3  60
#> 4       American Samoa    17      0    0   0
#> 5              Andorra    50      0    0   0
#> 6               Angola    40     60    3  58
#> 7  Antigua and Barbuda    43     60    0  47
#> 8            Argentina    60     60   31  60
#> 9              Armenia    30     60   20  29
#> 10               Aruba    32     60    0  20
#> 11           Australia    60     60   10   0
#> 12             Austria    60     60   21   0
#> 13          Azerbaijan    30     60    6  29
#> 14        Bahamas, The    60     60    0  43
#>  [ reached 'max' / getOption("max.print") -- omitted 202 rows ]
```
