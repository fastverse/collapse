# Fast (Grouped) Distinct Value Count for Matrix-Like Objects

`fndistinct` is a generic function that (column-wise) computes the
number of distinct values in `x`, (optionally) grouped by `g`. It is
significantly faster than `length(unique(x))`. The
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) argument can
further be used to transform `x` using its (grouped) distinct value
count.

## Usage

``` r
fndistinct(x, ...)

# Default S3 method
fndistinct(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
           use.g.names = TRUE, nthreads = .op[["nthreads"]], ...)

# S3 method for class 'matrix'
fndistinct(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
           use.g.names = TRUE, drop = TRUE, nthreads = .op[["nthreads"]], ...)

# S3 method for class 'data.frame'
fndistinct(x, g = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
           use.g.names = TRUE, drop = TRUE, nthreads = .op[["nthreads"]], ...)

# S3 method for class 'grouped_df'
fndistinct(x, TRA = NULL, na.rm = .op[["na.rm"]],
           use.g.names = FALSE, keep.group_vars = TRUE, nthreads = .op[["nthreads"]], ...)
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

- na.rm:

  logical. `TRUE`: Skip missing values in `x` (faster computation).
  `FALSE`: Also consider 'NA' as one distinct value.

- use.g.names:

  logical. Make group-names and add to the result as names (default
  method) or row-names (matrix and data frame methods). No row-names are
  generated for *data.table*'s.

- nthreads:

  integer. The number of threads to utilize. Parallelism is across
  groups for grouped computations and at the column-level otherwise.

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

`fndistinct` implements a pretty fast C-level hashing algorithm inspired
by the *kit* package to find the number of distinct values.

If `na.rm = TRUE` (the default), missing values will be skipped yielding
substantial performance gains in data with many missing values. If
`na.rm = FALSE`, missing values will simply be treated as any other
value and read into the hash-map. Thus with the former, a numeric vector
`c(1.25,NaN,3.56,NA)` will have a distinct value count of 2, whereas the
latter will return a distinct value count of 4.

`fndistinct` preserves all attributes of non-classed vectors / columns,
and only the 'label' attribute (if available) of classed vectors /
columns (i.e. dates or factors). When applied to data frames and
matrices, the row-names are adjusted as necessary.

## Value

Integer. The number of distinct values in `x`, grouped by `g`, or (if
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) is used) `x`
transformed by its distinct value count, grouped by `g`.

## See also

[`fnunique`](https://fastverse.org/collapse/reference/funique.md),
[`fnobs`](https://fastverse.org/collapse/reference/fnobs.md), [Fast
Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## default vector method
fndistinct(airquality$Solar.R)                   # Simple distinct value count
#> [1] 117
fndistinct(airquality$Solar.R, airquality$Month) # Grouped distinct value count
#>  5  6  7  8  9 
#> 27 28 29 27 27 

## data.frame method
fndistinct(airquality)
#>   Ozone Solar.R    Wind    Temp   Month     Day 
#>      67     117      31      40       5      31 
fndistinct(airquality, airquality$Month)
#>   Ozone Solar.R Wind Temp Month Day
#> 5    21      27   18   18     1  31
#> 6     9      28   16   19     1  30
#> 7    24      29   17   14     1  31
#> 8    24      27   18   19     1  31
#> 9    21      27   19   20     1  30
fndistinct(wlddev)                               # Works with data of all types!
#> country   iso3c    date    year  decade  region  income    OECD   PCGDP  LIFEEX 
#>     216     216      61      61       7       7       4       2    9470   10548 
#>    GINI     ODA     POP 
#>     368    7832   12877 
head(fndistinct(wlddev, wlddev$iso3c))
#>     country iso3c date year decade region income OECD PCGDP LIFEEX GINI ODA POP
#> ABW       1     1   61   61      7      1      1    1    32     60    0  20  60
#> AFG       1     1   61   61      7      1      1    1    18     60    0  60  60
#> AGO       1     1   61   61      7      1      1    1    40     59    3  58  60
#> ALB       1     1   61   61      7      1      1    1    40     59    9  32  60
#> AND       1     1   61   61      7      1      1    1    50      0    0   0  60
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

## matrix method
aqm <- qM(airquality)
fndistinct(aqm)                                  # Also works for character or logical matrices
#>   Ozone Solar.R    Wind    Temp   Month     Day 
#>      67     117      31      40       5      31 
fndistinct(aqm, airquality$Month)
#>   Ozone Solar.R Wind Temp Month Day
#> 5    21      27   18   18     1  31
#> 6     9      28   16   19     1  30
#> 7    24      29   17   14     1  31
#> 8    24      27   18   19     1  31
#> 9    21      27   19   20     1  30

## method for grouped data frames - created with dplyr::group_by or fgroup_by
airquality |> fgroup_by(Month) |> fndistinct()
#>   Month Ozone Solar.R Wind Temp Day
#> 1     5    21      27   18   18  31
#> 2     6     9      28   16   19  30
#> 3     7    24      29   17   14  31
#> 4     8    24      27   18   19  31
#> 5     9    21      27   19   20  30
wlddev |> fgroup_by(country) |>
             fselect(PCGDP,LIFEEX,GINI,ODA) |> fndistinct()
#>                country PCGDP LIFEEX GINI ODA
#> 1          Afghanistan    18     60    0  60
#> 2              Albania    40     59    9  32
#> 3              Algeria    60     60    3  60
#> 4       American Samoa    17      0    0   0
#> 5              Andorra    50      0    0   0
#> 6               Angola    40     59    3  58
#> 7  Antigua and Barbuda    43     60    0  47
#> 8            Argentina    60     60   29  60
#> 9              Armenia    30     59   20  29
#> 10               Aruba    32     60    0  20
#> 11           Australia    60     59    9   0
#> 12             Austria    60     60   16   0
#> 13          Azerbaijan    30     60    5  29
#> 14        Bahamas, The    60     59    0  41
#>  [ reached 'max' / getOption("max.print") -- omitted 202 rows ]
```
