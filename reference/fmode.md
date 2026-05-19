# Fast (Grouped, Weighted) Statistical Mode for Matrix-Like Objects

`fmode` is a generic function and returns the (column-wise) statistical
mode i.e. the most frequent value of `x`, (optionally) grouped by `g`
and/or weighted by `w`. The
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) argument can
further be used to transform `x` using its (grouped, weighted) mode.
Ties between multiple possible modes can be resolved by taking the
minimum, maximum, (default) first or last occurring mode.

## Usage

``` r
fmode(x, ...)

# Default S3 method
fmode(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = TRUE, ties = "first", nthreads = .op[["nthreads"]], ...)

# S3 method for class 'matrix'
fmode(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = TRUE, drop = TRUE, ties = "first", nthreads = .op[["nthreads"]], ...)

# S3 method for class 'data.frame'
fmode(x, g = NULL, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = TRUE, drop = TRUE, ties = "first", nthreads = .op[["nthreads"]], ...)

# S3 method for class 'grouped_df'
fmode(x, w = NULL, TRA = NULL, na.rm = .op[["na.rm"]],
      use.g.names = FALSE, keep.group_vars = TRUE, keep.w = TRUE, stub = .op[["stub"]],
      ties = "first", nthreads = .op[["nthreads"]], ...)
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
  implemented at very little computational cost. If `na.rm = FALSE`,
  `NA` is treated as any other value.

- use.g.names:

  logical. Make group-names and add to the result as names (default
  method) or row-names (matrix and data frame methods). No row-names are
  generated for *data.table*'s.

- ties:

  an integer or character string specifying the method to resolve ties
  between multiple possible modes i.e. multiple values with the maximum
  frequency or sum of weights:

  |        |     |          |     |                                          |
  |--------|-----|----------|-----|------------------------------------------|
  | *Int.* |     | *String* |     | *Description*                            |
  | 1      |     | "first"  |     | take the first occurring mode.           |
  | 2      |     | "min"    |     | take the smallest of the possible modes. |
  | 3      |     | "max"    |     | take the largest of the possible modes.  |
  | 4      |     | "last"   |     | take the last occurring mode.            |

  *Note:* `"min"/"max"` don't work with character data. See also
  Details.

- nthreads:

  integer. The number of threads to utilize. Parallelism is across
  groups for grouped computations and at the column-level otherwise.

- drop:

  *matrix and data.frame method:* Logical. `TRUE` drops dimensions and
  returns an atomic vector if `g = NULL` and `TRA = NULL`.

- keep.group_vars:

  *grouped_df method:* Logical. `FALSE` removes grouping variables after
  computation.

- keep.w:

  *grouped_df method:* Logical. Retain `sum` of weighting variable after
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

`fmode` implements a pretty fast C-level hashing algorithm inspired by
the *kit* package to find the statistical mode.

If `na.rm = FALSE`, `NA` is not removed but treated as any other value
(i.e. its frequency is counted). If all values are `NA`, `NA` is always
returned.

The weighted mode is computed by summing up the weights for all distinct
values and choosing the value with the largest sum. If `na.rm = TRUE`,
missing values will be removed from both `x` and `w` i.e. utilizing only
`x[complete.cases(x,w)]` and `w[complete.cases(x,w)]`.

It is possible that multiple values have the same mode (the maximum
frequency or sum of weights). Typical cases are simply when all values
are either all the same or all distinct. In such cases, the default
option `ties = "first"` returns the first occurring value in the data
reaching the maximum frequency count or sum of weights. For example in a
sample `x = c(1, 3, 2, 2, 4, 4, 1, 7)`, the first mode is 2 as `fmode`
goes through the data from left to right. `ties = "last"` on the other
hand gives 1. It is also possible to take the minimum or maximum mode,
i.e. `fmode(x, ties = "min")` returns 1, and `fmode(x, ties = "max")`
returns 4. It should be noted that options `ties = "min"` and
`ties = "max"` give unintuitive results for character data (no strict
alphabetic sorting, similar to using `<` and `>` to compare character
values in R). These options are also best avoided if missing values are
counted (`na.rm = FALSE`) since no proper logical comparison with
missing values is possible: With numeric data it depends, since in C++
any comparison with `NA_real_` evaluates to `FALSE`, `NA_real_` is
chosen as the min or max mode only if it is also the first mode, and
never otherwise. For integer data, `NA_integer_` is stored as the
smallest integer in C++, so it will always be chosen as the min mode and
never as the max mode. For character data, `NA_character_` is stored as
the string `"NA"` in C++ and thus the behavior depends on the other
character content.

`fmode` preserves all the attributes of the objects it is applied to
(apart from names or row-names which are adjusted as necessary in
grouped operations). If a data frame is passed to `fmode` and
`drop = TRUE` (the default),
[`unlist`](https://rdrr.io/r/base/unlist.html) will be called on the
result, which might not be sensible depending on the data at hand.

## Value

The (`w` weighted) statistical mode of `x`, grouped by `g`, or (if
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) is used) `x`
transformed by its (grouped, weighed) mode.

## See also

[`fmean`](https://fastverse.org/collapse/reference/fmean.md),
[`fmedian`](https://fastverse.org/collapse/reference/fnth_fmedian.md),
[Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
x <- c(1, 3, 2, 2, 4, 4, 1, 7, NA, NA, NA)
fmode(x)                            # Default is ties = "first"
#> [1] 2
fmode(x, ties = "last")
#> [1] 1
fmode(x, ties = "min")
#> [1] 1
fmode(x, ties = "max")
#> [1] 4
fmode(x, na.rm = FALSE)             # Here NA is the mode, regardless of ties option
#> [1] NA
fmode(x[-length(x)], na.rm = FALSE) # Not anymore..
#> [1] 2

## World Development Data
attach(wlddev)
## default vector method
fmode(PCGDP)                      # Numeric mode
#> [1] 330.3036
#> attr(,"label")
#> [1] "GDP per capita (constant 2010 US$)"
head(fmode(PCGDP, iso3c))         # Grouped numeric mode
#>         ABW         AFG         AGO         ALB         AND         ARE 
#>  15669.6160    330.3036   3193.4039   1992.2919  41701.5444 103604.9068 
head(fmode(PCGDP, iso3c, LIFEEX)) # Grouped and weighted numeric mode
#>        ABW        AFG        AGO        ALB        AND        ARE 
#> 26630.2053   573.2876  3111.1577  5210.6883         NA 41420.4830 
fmode(region)                     # Factor mode
#> [1] Europe & Central Asia
#> attr(,"label")
#> [1] Region
#> 7 Levels: East Asia & Pacific ... Sub-Saharan Africa
fmode(date)                       # Date mode (defaults to first value since panel is balanced)
#> [1] "1961-01-01"
fmode(country)                    # Character mode (also defaults to first value)
#> [1] "Afghanistan"
#> attr(,"label")
#> [1] "Country Name"
fmode(OECD)                       # Logical mode
#> [1] FALSE
#> attr(,"label")
#> [1] "Is OECD Member Country?"
                                  # ..all the above can also be performed grouped and weighted
## matrix method
m <- qM(airquality)
fmode(m)
#>   Ozone Solar.R    Wind    Temp   Month     Day 
#>    23.0   259.0    11.5    81.0     5.0     1.0 
fmode(m, na.rm = FALSE)         # NA frequency is also counted
#>   Ozone Solar.R    Wind    Temp   Month     Day 
#>      NA      NA    11.5    81.0     5.0     1.0 
fmode(m, airquality$Month)      # Groupwise
#>   Ozone Solar.R Wind Temp Month Day
#> 5    11     190  9.7   66     5   1
#> 6    29     250 11.5   76     6   1
#> 7    97     175  7.4   81     7   1
#> 8    44     255 11.5   86     8   1
#> 9    13     238 10.3   71     9   1
fmode(m, w = airquality$Day)    # Weighted: Later days in the month are given more weight
#>   Ozone Solar.R    Wind    Temp   Month     Day 
#>    23.0   223.0    11.5    76.0     5.0    30.0 
fmode(m>50, airquality$Month)   # Groupwise logical mode
#>   Ozone Solar.R  Wind Temp Month   Day
#> 5 FALSE    TRUE FALSE TRUE FALSE FALSE
#> 6 FALSE    TRUE FALSE TRUE FALSE FALSE
#> 7  TRUE    TRUE FALSE TRUE FALSE FALSE
#> 8 FALSE    TRUE FALSE TRUE FALSE FALSE
#> 9 FALSE    TRUE FALSE TRUE FALSE FALSE
                                # etc..
## data.frame method
fmode(wlddev)                      # Calling unlist -> coerce to character vector
#>            country              iso3c               date               year 
#>      "Afghanistan"                "2"            "-3287"             "1960" 
#>             decade             region             income               OECD 
#>             "1960"                "2"                "1"            "FALSE" 
#>              PCGDP             LIFEEX               GINI                ODA 
#> "330.303552908057"           "62.869"             "26.8" "70000.0002980232" 
#>                POP 
#>            "61786" 
fmode(wlddev, drop = FALSE)        # Gives one row
#>       country iso3c       date year decade                region      income
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 Europe & Central Asia High income
#>    OECD    PCGDP LIFEEX GINI   ODA   POP
#> 1 FALSE 330.3036 62.869 26.8 70000 61786
head(fmode(wlddev, iso3c))         # Grouped mode
#>         country iso3c       date year decade                    region
#> ABW       Aruba   ABW 1961-01-01 1960   1960 Latin America & Caribbean
#> AFG Afghanistan   AFG 1961-01-01 1960   1960                South Asia
#> AGO      Angola   AGO 1961-01-01 1960   1960        Sub-Saharan Africa
#> ALB     Albania   ALB 1961-01-01 1960   1960     Europe & Central Asia
#> AND     Andorra   AND 1961-01-01 1960   1960     Europe & Central Asia
#>                  income  OECD      PCGDP LIFEEX GINI       ODA     POP
#> ABW         High income FALSE 15669.6160 65.662   NA  36860001   54211
#> AFG          Low income FALSE   330.3036 32.446   NA 116769997 8996973
#> AGO Lower middle income FALSE  3193.4039 45.201   52   -390000 5454933
#> ALB Upper middle income FALSE  1992.2919 71.860   27   9310000 1608800
#> AND         High income FALSE 41701.5444     NA   NA        NA   13411
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
head(fmode(wlddev, iso3c, LIFEEX)) # Grouped and weighted mode
#>         country iso3c       date year decade                    region
#> ABW       Aruba   ABW 2020-01-01 2019   2010 Latin America & Caribbean
#> AFG Afghanistan   AFG 2020-01-01 2019   2010                South Asia
#> AGO      Angola   AGO 2020-01-01 2019   2010        Sub-Saharan Africa
#> ALB     Albania   ALB 2020-01-01 2019   2010     Europe & Central Asia
#> AND     Andorra   AND 2021-01-01 2020   2020     Europe & Central Asia
#>                  income  OECD      PCGDP LIFEEX GINI        ODA      POP
#> ABW         High income FALSE 26630.2053 76.293   NA  -12840000   106314
#> AFG          Low income FALSE   573.2876 64.833   NA 4339979980 38041754
#> AGO Lower middle income FALSE  3111.1577 45.201 51.3   49230000 31825295
#> ALB Upper middle income FALSE  5210.6883 71.860 33.2   31410000  2854191
#> AND         High income FALSE         NA     NA   NA         NA       NA
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

detach(wlddev)
```
