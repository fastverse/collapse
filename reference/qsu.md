# Fast (Grouped, Weighted) Summary Statistics for Cross-Sectional and Panel Data

`qsu`, shorthand for quick-summary, is an extremely fast summary command
inspired by the (xt)summarize command in the STATA statistical software.

It computes a set of 7 statistics (nobs, mean, sd, min, max, skewness
and kurtosis) using a numerically stable one-pass method generalized
from Welford's Algorithm. Statistics can be computed weighted, by
groups, and also within-and between entities (for panel data, see
Details).

## Usage

``` r
qsu(x, ...)

# Default S3 method
qsu(x, g = NULL, pid = NULL, w = NULL, higher = FALSE,
    array = TRUE, stable.algo = .op[["stable.algo"]], ...)

# S3 method for class 'matrix'
qsu(x, g = NULL, pid = NULL, w = NULL, higher = FALSE,
    array = TRUE, stable.algo = .op[["stable.algo"]], ...)

# S3 method for class 'data.frame'
qsu(x, by = NULL, pid = NULL, w = NULL, cols = NULL, higher = FALSE,
    array = TRUE, labels = FALSE, stable.algo = .op[["stable.algo"]], ...)

# S3 method for class 'grouped_df'
qsu(x, pid = NULL, w = NULL, higher = FALSE,
    array = TRUE, labels = FALSE, stable.algo = .op[["stable.algo"]], ...)

# Methods for indexed data / compatibility with plm:

# S3 method for class 'pseries'
qsu(x, g = NULL, w = NULL, effect = 1L, higher = FALSE,
    array = TRUE, stable.algo = .op[["stable.algo"]], ...)

# S3 method for class 'pdata.frame'
qsu(x, by = NULL, w = NULL, cols = NULL, effect = 1L, higher = FALSE,
    array = TRUE, labels = FALSE, stable.algo = .op[["stable.algo"]], ...)

# Methods for compatibility with sf:

# S3 method for class 'sf'
qsu(x, by = NULL, pid = NULL, w = NULL, cols = NULL, higher = FALSE,
    array = TRUE, labels = FALSE, stable.algo = .op[["stable.algo"]], ...)


# S3 method for class 'qsu'
as.data.frame(x, ..., gid = "Group", stringsAsFactors = TRUE)

# S3 method for class 'qsu'
print(x, digits = .op[["digits"]] + 2L, nonsci.digits = 9, na.print = "-",
      return = FALSE, print.gap = 2, ...)
```

## Arguments

- x:

  a vector, matrix, data frame, 'indexed_series' ('pseries') or
  'indexed_frame' ('pdata.frame').

- g:

  a factor, [`GRP`](https://fastverse.org/collapse/reference/GRP.md)
  object, atomic vector (internally converted to factor) or a list of
  vectors / factors (internally converted to a
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object) used
  to group `x`.

- by:

  *(p)data.frame method*: Same as `g`, but also allows one- or two-sided
  formulas i.e. `~ group1 + group2` or `var1 + var2 ~ group1 + group2`.
  See Examples.

- pid:

  same input as `g/by`: Specify a panel-identifier to also compute
  statistics on between- and within- transformed data. Data frame method
  also supports one- or two-sided formulas, grouped_df method supports
  expressions evaluated in the data environment. Transformations are
  taken independently from grouping with `g/by` (grouped statistics are
  computed on the transformed data if `g/by` is also used). However,
  passing any LHS variables to `pid` will overwrite any `LHS` variables
  passed to `by`.

- w:

  a vector of (non-negative) weights. Adding weights will compute the
  weighted mean, sd, skewness and kurtosis, and transform the data using
  weighted individual means if `pid` is used. A `"WeightSum"` column
  will be added giving the sum of weights, see also Details. Data frame
  method supports formula, grouped_df method supports expression.

- cols:

  select columns to summarize using column names, indices, a logical
  vector or a function (e.g. `is.numeric`). Two-sided formulas passed to
  `by` or `pid` overwrite `cols`.

- higher:

  logical. Add higher moments (skewness and kurtosis).

- array:

  logical. If computations have more than 2 dimensions (up to a maximum
  of 4D: variables, statistics, groups and panel-decomposition) `TRUE`
  returns an array, while `FALSE` returns a (nested) list of matrices.

- stable.algo:

  logical. `FALSE` uses a faster but less stable method to calculate the
  standard deviation (see Details of
  [`fsd`](https://fastverse.org/collapse/reference/fvar_fsd.md)). Only
  available if `w = NULL` and `higher = FALSE`.

- labels:

  logical `TRUE` or a function: to display variable labels in the
  summary. See Details.

- effect:

  *plm* methods: Select which panel identifier should be used for
  between and within transformations of the data. 1L takes the first
  variable in the
  [index](https://fastverse.org/collapse/reference/indexing.md), 2L the
  second etc.. Index variables can also be called by name using a
  character string. More than one variable can be supplied.

- ...:

  arguments to be passed to or from other methods.

- gid:

  character. Name assigned to the group-id column, when summarising
  variables by groups.

- stringsAsFactors:

  logical. Make factors from dimension names of 'qsu' array. Same as
  option to [`as.data.frame.table`](https://rdrr.io/r/base/table.html).

- digits:

  the number of digits to print after the comma/dot.

- nonsci.digits:

  the number of digits to print before resorting to scientific notation
  (default is to print out numbers with up to 9 digits and print larger
  numbers scientifically).

- na.print:

  character string to substitute for missing values.

- return:

  logical. Don't print but instead return the formatted object.

- print.gap:

  integer. Spacing between printed columns. Passed to `print.default`.

## Details

The algorithm used to compute statistics is well described
[here](https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance)
\[see sections *Welford's online algorithm*, *Weighted incremental
algorithm* and *Higher-order statistics*. Skewness and kurtosis are
calculated as described in *Higher-order statistics* and are
mathematically identical to those implemented in the *moments* package.
Just note that `qsu` computes the kurtosis (like `momens::kurtosis`),
not the excess-kurtosis (= kurtosis - 3) defined in *Higher-order
statistics*. The *Weighted incremental algorithm* described can easily
be generalized to higher-order statistics\].

Grouped computations specified with `g/by` are carried out extremely
efficiently as in `fsum` (in a single pass, without splitting the data).

If `pid` is used, `qsu` performs a panel-decomposition of each variable
and computes 3 sets of statistics: Statistics computed on the 'Overall'
(raw) data, statistics computed on the 'Between' - transformed (pid -
averaged) data, and statistics computed on the 'Within' - transformed
(pid - demeaned) data.

More formally, let **`x`** (bold) be a panel vector of data for `N`
individuals indexed by `i`, recorded for `T` periods, indexed by `t`.
`xit` then denotes a single data-point belonging to individual `i` in
time-period `t` (`t/T` must not represent time). Then `xi.` denotes the
average of all values for individual `i` (averaged over `t`), and by
extension **`xN.`** is the vector (length `N`) of such averages for all
individuals. If no groups are supplied to `g/by`, the 'Between'
statistics are computed on **`xN.`**, the vector of individual averages.
(This means that for a non-balanced panel or in the presence of missing
values, the 'Overall' mean computed on **`x`** can be slightly different
than the 'Between' mean computed on **`xN.`**, and the variance
decomposition is not exact). If groups are supplied to `g/by`, **`xN.`**
is expanded to the vector **`xi.`** (length `N x T`) by replacing each
value `xit` in **`x`** with `xi.`, while preserving missing values in
**`x`**. Grouped Between-statistics are then computed on **`xi.`**, with
the only difference that the number of observations ('Between-N')
reported for each group is the number of distinct non-missing values of
**`xi.`** in each group (not the total number of non-missing values of
**`xi.`** in each group, which is already reported in 'Overall-N'). See
Examples.

'Within' statistics are always computed on the vector
**`x - xi. + x..`**, where **`x..`** is simply the 'Overall' mean
computed from **`x`**, which is added back to preserve the level of the
data. The 'Within' mean computed on this data will always be identical
to the 'Overall' mean. In the summary output, `qsu` reports not 'N',
which would be identical to the 'Overall-N', but 'T', the average number
of time-periods of data available for each individual obtained as 'T' =
'Overall-N / 'Between-N'. When using weights (`w`) with panel data
(`pid`), the 'Between' sum of weights is also simply the number of
groups, and the 'Within' sum of weights is the 'Overall' sum of weights
divided by the number of groups. See Examples.

Apart from 'N/T' and the extrema, the standard-deviations ('SD')
computed on between- and within- transformed data are extremely valuable
because they indicate how much of the variation in a panel-variable is
between-individuals and how much of the variation is within-individuals
(over time). At the extremes, variables that have common values across
individuals (such as the time-variable(s) 't' in a balanced panel), can
readily be identified as individual-invariant because the 'Between-SD'
on this variable is 0 and the 'Within-SD' is equal to the 'Overall-SD'.
Analogous, time-invariant individual characteristics (such as the
individual-id 'i') have a 0 'Within-SD' and a 'Between-SD' equal to the
'Overall-SD'. See Examples.

For data frame methods, if `labels = TRUE`, `qsu` uses
`function(x) paste(names(x), setv(vlabels(x), NA, ""), sep = ": ")` to
combine variable names and labels for display. Alternatively, the user
can pass a custom function which will be applied to the data frame, e.g.
using `labels = vlabels` just displays the labels. See also
[`vlabels`](https://fastverse.org/collapse/reference/small-helpers.md).

`qsu` comes with its own print method which by default writes out up to
9 digits at 4 decimal places. Larger numbers are printed in scientific
format. for numbers between 7 and 9 digits, an apostrophe (') is placed
after the 6th digit to designate the millions. Missing values are
printed using '-'.

The *sf* method simply ignores the geometry column.

## Value

A vector, matrix, array or list of matrices of summary statistics. All
matrices and arrays have a class 'qsu' and a class 'table' attached.

## Note

In weighted summaries, observations with missing or zero weights are
skipped, and thus do not affect any of the calculated statistics,
including the observation count. This also implies that a logical vector
passed to `w` can be used to efficiently summarize a subset of the data.

If weights `w` are used together with `pid`, transformed data is
computed using weighted individual means i.e. weighted **`xi.`** and
weighted **`x..`**. Weighted statistics are subsequently computed on
this weighted-transformed data.

## References

Welford, B. P. (1962). Note on a method for calculating corrected sums
of squares and products. *Technometrics*. 4 (3): 419-420.
doi:10.2307/1266577.

## See also

[`descr`](https://fastverse.org/collapse/reference/descr.md), [Summary
Statistics](https://fastverse.org/collapse/reference/summary-statistics.md),
[Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r

## World Development Panel Data
# Simple Summaries -------------------------
qsu(wlddev)                                 # Simple summary
#>              N         Mean          SD          Min             Max
#> country  13176            -           -            -               -
#> iso3c    13176            -           -            -               -
#> date     13176            -           -            -               -
#> year     13176         1990     17.6075         1960            2020
#> decade   13176    1985.5738     17.5117         1960            2020
#> region   13176            -           -            -               -
#> income   13176            -           -            -               -
#> OECD     13176            -           -            -               -
#> PCGDP     9470    12048.778  19077.6416     132.0776      196061.417
#> LIFEEX   11670      64.2963     11.4764       18.907         85.4171
#> GINI      1744      38.5341      9.2006         20.7            65.8
#> ODA       8608   454'720131  868'712654  -997'679993  2.56715605e+10
#> POP      12919  24'245971.6  102'120674         2833  1.39771500e+09
qsu(wlddev, labels = TRUE)                  # Display variable labels
#>                                                                                             N
#> country: Country Name                                                                   13176
#> iso3c: Country Code                                                                     13176
#> date: Date Recorded (Fictitious)                                                        13176
#> year: Year                                                                              13176
#> decade: Decade                                                                          13176
#> region: Region                                                                          13176
#> income: Income Level                                                                    13176
#> OECD: Is OECD Member Country?                                                           13176
#> PCGDP: GDP per capita (constant 2010 US$)                                                9470
#> LIFEEX: Life expectancy at birth, total (years)                                         11670
#> GINI: Gini index (World Bank estimate)                                                   1744
#> ODA: Net official development assistance and official aid received (constant 2018 US$)   8608
#> POP: Population, total                                                                  12919
#>                                                                                                Mean
#> country: Country Name                                                                             -
#> iso3c: Country Code                                                                               -
#> date: Date Recorded (Fictitious)                                                                  -
#> year: Year                                                                                     1990
#> decade: Decade                                                                            1985.5738
#> region: Region                                                                                    -
#> income: Income Level                                                                              -
#> OECD: Is OECD Member Country?                                                                     -
#> PCGDP: GDP per capita (constant 2010 US$)                                                 12048.778
#> LIFEEX: Life expectancy at birth, total (years)                                             64.2963
#> GINI: Gini index (World Bank estimate)                                                      38.5341
#> ODA: Net official development assistance and official aid received (constant 2018 US$)   454'720131
#> POP: Population, total                                                                  24'245971.6
#>                                                                                                 SD
#> country: Country Name                                                                            -
#> iso3c: Country Code                                                                              -
#> date: Date Recorded (Fictitious)                                                                 -
#> year: Year                                                                                 17.6075
#> decade: Decade                                                                             17.5117
#> region: Region                                                                                   -
#> income: Income Level                                                                             -
#> OECD: Is OECD Member Country?                                                                    -
#> PCGDP: GDP per capita (constant 2010 US$)                                               19077.6416
#> LIFEEX: Life expectancy at birth, total (years)                                            11.4764
#> GINI: Gini index (World Bank estimate)                                                      9.2006
#> ODA: Net official development assistance and official aid received (constant 2018 US$)  868'712654
#> POP: Population, total                                                                  102'120674
#>                                                                                                 Min
#> country: Country Name                                                                             -
#> iso3c: Country Code                                                                               -
#> date: Date Recorded (Fictitious)                                                                  -
#> year: Year                                                                                     1960
#> decade: Decade                                                                                 1960
#> region: Region                                                                                    -
#> income: Income Level                                                                              -
#> OECD: Is OECD Member Country?                                                                     -
#> PCGDP: GDP per capita (constant 2010 US$)                                                  132.0776
#> LIFEEX: Life expectancy at birth, total (years)                                              18.907
#> GINI: Gini index (World Bank estimate)                                                         20.7
#> ODA: Net official development assistance and official aid received (constant 2018 US$)  -997'679993
#> POP: Population, total                                                                         2833
#>                                                                                                    Max
#> country: Country Name                                                                                -
#> iso3c: Country Code                                                                                  -
#> date: Date Recorded (Fictitious)                                                                     -
#> year: Year                                                                                        2020
#> decade: Decade                                                                                    2020
#> region: Region                                                                                       -
#> income: Income Level                                                                                 -
#> OECD: Is OECD Member Country?                                                                        -
#> PCGDP: GDP per capita (constant 2010 US$)                                                   196061.417
#> LIFEEX: Life expectancy at birth, total (years)                                                85.4171
#> GINI: Gini index (World Bank estimate)                                                            65.8
#> ODA: Net official development assistance and official aid received (constant 2018 US$)  2.56715605e+10
#> POP: Population, total                                                                  1.39771500e+09
qsu(wlddev, higher = TRUE)                  # Add skewness and kurtosis
#>              N         Mean          SD          Min             Max     Skew
#> country  13176            -           -            -               -        -
#> iso3c    13176            -           -            -               -        -
#> date     13176            -           -            -               -        -
#> year     13176         1990     17.6075         1960            2020       -0
#> decade   13176    1985.5738     17.5117         1960            2020   0.0326
#> region   13176            -           -            -               -        -
#> income   13176            -           -            -               -        -
#> OECD     13176            -           -            -               -        -
#> PCGDP     9470    12048.778  19077.6416     132.0776      196061.417   3.1276
#> LIFEEX   11670      64.2963     11.4764       18.907         85.4171  -0.6748
#>              Kurt
#> country         -
#> iso3c           -
#> date            -
#> year       1.7994
#> decade     1.7917
#> region          -
#> income          -
#> OECD            -
#> PCGDP     17.1154
#> LIFEEX     2.6718
#>  [ reached 'max' / getOption("max.print") -- omitted 3 rows ]

# Grouped Summaries ------------------------
qsu(wlddev, ~ region, labels = TRUE)        # Statistics by World Bank Region
#> , , country: Country Name
#> 
#>                                N  Mean  SD  Min  Max
#> East Asia & Pacific         2196     -   -    -    -
#> Europe & Central Asia       3538     -   -    -    -
#> Latin America & Caribbean   2562     -   -    -    -
#> Middle East & North Africa  1281     -   -    -    -
#> North America                183     -   -    -    -
#> South Asia                   488     -   -    -    -
#> Sub-Saharan Africa          2928     -   -    -    -
#> 
#> , , iso3c: Country Code
#> 
#>                                N  Mean  SD  Min  Max
#> East Asia & Pacific         2196     -   -    -    -
#> Europe & Central Asia       3538     -   -    -    -
#> Latin America & Caribbean   2562     -   -    -    -
#> Middle East & North Africa  1281     -   -    -    -
#> North America                183     -   -    -    -
#> South Asia                   488     -   -    -    -
#> Sub-Saharan Africa          2928     -   -    -    -
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 10 slices ] 
qsu(wlddev, PCGDP + LIFEEX ~ income)        # Summarize GDP per Capita and Life Expectancy by
#> , , PCGDP
#> 
#>                         N        Mean          SD       Min         Max
#> High income          3179  30280.7283  23847.0483  932.0417  196061.417
#> Low income           1311    597.4053    288.4392  164.3366   1864.7925
#> Lower middle income  2246   1574.2535    858.7183  144.9863   4818.1922
#> Upper middle income  2734   4945.3258   2979.5609  132.0776  20532.9523
#> 
#> , , LIFEEX
#> 
#>                         N     Mean      SD     Min      Max
#> High income          3831  73.6246  5.6693  42.672  85.4171
#> Low income           1800  49.7301  9.0944  26.172    74.43
#> Lower middle income  2790  58.1481  9.3115  18.907   76.699
#> Upper middle income  3249  66.6466   7.537  36.535   80.279
#> 
stats <- qsu(wlddev, ~ region + income,     # World Bank Income Level
             cols = 9:10, higher = TRUE)    # Same variables, by both region and income
aperm(stats)                                # A different perspective on the same stats
#> , , East Asia & Pacific.High income
#> 
#>           N        Mean          SD       Min         Max     Skew    Kurt
#> PCGDP   487  26766.9163  14823.5175  932.0417  71992.1517   0.2811  2.5566
#> LIFEEX  664     73.3724       6.659     54.81      85.078  -0.4945  2.6876
#> 
#> , , East Asia & Pacific.Lower middle income
#> 
#>           N       Mean        SD       Min        Max     Skew    Kurt
#> PCGDP   562  1599.4748  895.5278  144.9863  4503.1454   0.4987  3.0945
#> LIFEEX  780    59.1597    9.6542    18.907       75.4  -1.0076  4.1679
#> 
#> , , East Asia & Pacific.Upper middle income
#> 
#>           N       Mean         SD       Min         Max     Skew    Kurt
#> PCGDP   418  3561.0911  2468.1843  132.0776  12486.6788   1.4194    4.89
#> LIFEEX  363    66.9366     5.6713    43.725       77.15  -0.9194  4.8567
#> 
#> , , Europe & Central Asia.High income
#> 
#>            N        Mean          SD        Min         Max    Skew     Kurt
#> PCGDP   1551  35718.5696  26466.4643  4500.7362  196061.417  2.2834  10.2728
#> LIFEEX  1845     74.8234      4.3845    63.0749     85.4171   0.042   2.1824
#> 
#> , , Europe & Central Asia.Low income
#> 
#>          N      Mean        SD       Min        Max    Skew    Kurt
#> PCGDP   35  809.4753  336.5425  366.9354  1458.9932   0.426  1.9987
#> LIFEEX  60   60.1129     6.147    50.613     71.097  0.3991  1.9875
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 18 slices ] 

# Grouped summary
wlddev |> fgroup_by(region) |> fselect(PCGDP, LIFEEX) |> qsu()
#> , , PCGDP
#> 
#>                                N        Mean          SD         Min
#> East Asia & Pacific         1467  10513.2441  14383.5507    132.0776
#> Europe & Central Asia       2243  25992.9618  26435.1316    366.9354
#> Latin America & Caribbean   1976   7628.4477   8818.5055   1005.4085
#> Middle East & North Africa   842  13878.4213  18419.7912    578.5996
#> North America                180    48699.76  24196.2855  16405.9053
#> South Asia                   382   1235.9256   1611.2232    265.9625
#> Sub-Saharan Africa          2380   1840.0259   2596.0104    164.3366
#>                                    Max
#> East Asia & Pacific         71992.1517
#> Europe & Central Asia       196061.417
#> Latin America & Caribbean   88391.3331
#> Middle East & North Africa  116232.753
#> North America               113236.091
#> South Asia                    8476.564
#> Sub-Saharan Africa          20532.9523
#> 
#> , , LIFEEX
#> 
#>                                N     Mean       SD      Min      Max
#> East Asia & Pacific         1807  65.9445  10.1633   18.907   85.078
#> Europe & Central Asia       3046  72.1625   5.7602   45.369  85.4171
#> Latin America & Caribbean   2107  68.3486   7.3768   41.762  82.1902
#> Middle East & North Africa  1226  66.2508   9.8306   29.919  82.8049
#> North America                144  76.2867   3.5734  68.8978  82.0488
#> South Asia                   480  57.5585  11.3004   32.446   78.921
#> Sub-Saharan Africa          2860   51.581   8.6876   26.172  74.5146
#> 

# Panel Data Summaries ---------------------
qsu(wlddev, pid = ~ iso3c, labels = TRUE)   # Adding between and within countries statistics
#> , , country: Country Name
#> 
#>            N/T  Mean  SD  Min  Max
#> Overall  13176     -   -    -    -
#> Between    216     -   -    -    -
#> Within      61     -   -    -    -
#> 
#> , , date: Date Recorded (Fictitious)
#> 
#>            N/T  Mean  SD  Min  Max
#> Overall  13176     -   -    -    -
#> Between    216     -   -    -    -
#> Within      61     -   -    -    -
#> 
#> , , year: Year
#> 
#>            N/T  Mean       SD   Min   Max
#> Overall  13176  1990  17.6075  1960  2020
#> Between    216  1990        0  1990  1990
#> Within      61  1990  17.6075  1960  2020
#> 
#> , , decade: Decade
#> 
#>            N/T       Mean       SD        Min        Max
#> Overall  13176  1985.5738  17.5117       1960       2020
#> Between    216  1985.5738        0  1985.5738  1985.5738
#> Within      61  1985.5738  17.5117       1960       2020
#> 
#> , , region: Region
#> 
#>            N/T  Mean  SD  Min  Max
#> Overall  13176     -   -    -    -
#> Between    216     -   -    -    -
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 7 slices ] 
# -> They show amongst other things that year and decade are individual-invariant,
# that we have GINI-data on only 161 countries, with only 8.42 observations per country on average,
# and that GDP, LIFEEX and GINI vary more between-countries, but ODA received varies more within
# countries over time.

# Let's do this manually for PCGDP:
x <- wlddev$PCGDP
g <- wlddev$iso3c

# This is the exact variance decomposion
all.equal(fvar(x), fvar(B(x, g)) + fvar(W(x, g)))
#> [1] TRUE

# What qsu does is calculate
r <- rbind(Overall = qsu(x),
           Between = qsu(fmean(x, g)), # Aggregation instead of between-transform
           Within = qsu(fwithin(x, g, mean = "overall.mean"))) # Same as qsu(W(x, g) + fmean(x))
r[3, 1] <- r[1, 1] / r[2, 1]
print.qsu(r)
#>                N        Mean          SD          Min         Max
#> Overall     9470   12048.778  19077.6416     132.0776  196061.417
#> Between      206  12962.6054  20189.9007     253.1886   141200.38
#> Within   45.9709   12048.778   6723.6808  -33504.8721  76767.5254
# Proof:
qsu(x, pid = g)
#>              N/T        Mean          SD          Min         Max
#> Overall     9470   12048.778  19077.6416     132.0776  196061.417
#> Between      206  12962.6054  20189.9007     253.1886   141200.38
#> Within   45.9709   12048.778   6723.6808  -33504.8721  76767.5254

# Using indexed data:
wldi <- findex_by(wlddev, iso3c, year)   # Creating a Indexed Data Frame frame from this data
qsu(wldi)                                # Summary for pdata.frame -> qsu(wlddev, pid = ~ iso3c)
#> , , country
#> 
#>            N/T  Mean  SD  Min  Max
#> Overall  13176     -   -    -    -
#> Between    216     -   -    -    -
#> Within      61     -   -    -    -
#> 
#> , , iso3c
#> 
#>            N/T  Mean  SD  Min  Max
#> Overall  13176     -   -    -    -
#> Between    216     -   -    -    -
#> Within      61     -   -    -    -
#> 
#> , , date
#> 
#>            N/T  Mean  SD  Min  Max
#> Overall  13176     -   -    -    -
#> Between    216     -   -    -    -
#> Within      61     -   -    -    -
#> 
#> , , year
#> 
#>            N/T  Mean       SD   Min   Max
#> Overall  13176  1990  17.6075  1960  2020
#> Between    216  1990        0  1990  1990
#> Within      61  1990  17.6075  1960  2020
#> 
#> , , decade
#> 
#>            N/T       Mean       SD        Min        Max
#> Overall  13176  1985.5738  17.5117       1960       2020
#> Between    216  1985.5738        0  1985.5738  1985.5738
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 8 slices ] 
qsu(wldi$PCGDP)                          # Default summary for Panel Series
#>              N/T        Mean          SD          Min         Max
#> Overall     9470   12048.778  19077.6416     132.0776  196061.417
#> Between      206  12962.6054  20189.9007     253.1886   141200.38
#> Within   45.9709   12048.778   6723.6808  -33504.8721  76767.5254
qsu(G(wldi$PCGDP))                       # Summarizing GDP growth, see also ?G
#>              N/T    Mean      SD       Min       Max
#> Overall     9264  2.0762  6.0081  -64.9924  140.3708
#> Between      202  2.0752  1.8684   -7.6806   10.3106
#> Within   45.8614  2.0762   5.785   -67.359  133.0971

# Grouped Panel Data Summaries -------------
qsu(wlddev, ~ region, ~ iso3c, cols = 9:12) # Panel-Statistics by region
#> , , Overall, PCGDP
#> 
#>                              N/T        Mean          SD         Min
#> East Asia & Pacific         1467  10513.2441  14383.5507    132.0776
#> Europe & Central Asia       2243  25992.9618  26435.1316    366.9354
#> Latin America & Caribbean   1976   7628.4477   8818.5055   1005.4085
#> Middle East & North Africa   842  13878.4213  18419.7912    578.5996
#> North America                180    48699.76  24196.2855  16405.9053
#> South Asia                   382   1235.9256   1611.2232    265.9625
#> Sub-Saharan Africa          2380   1840.0259   2596.0104    164.3366
#>                                    Max
#> East Asia & Pacific         71992.1517
#> Europe & Central Asia       196061.417
#> Latin America & Caribbean   88391.3331
#> Middle East & North Africa  116232.753
#> North America               113236.091
#> South Asia                    8476.564
#> Sub-Saharan Africa          20532.9523
#> 
#> , , Between, PCGDP
#> 
#>                             N/T        Mean          SD         Min         Max
#> East Asia & Pacific          34  10513.2441   12771.742    444.2899  39722.0077
#> Europe & Central Asia        56  25992.9618   24051.035    809.4753   141200.38
#> Latin America & Caribbean    38   7628.4477   8470.9708   1357.3326  77403.7443
#> Middle East & North Africa   20  13878.4213  17251.6962   1069.6596  64878.4021
#> North America                 3    48699.76  18604.4369  35260.4708  74934.5874
#> South Asia                    8   1235.9256   1488.3669      413.68   6621.5002
#> Sub-Saharan Africa           47   1840.0259   2234.3254    253.1886   9922.0052
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 10 slices ] 
psr <- qsu(wldi, ~ region, cols = 9:12)     # Same on indexed data
psr                                         # -> Gives a 4D array
#> , , Overall, PCGDP
#> 
#>                              N/T        Mean          SD         Min
#> East Asia & Pacific         1467  10513.2441  14383.5507    132.0776
#> Europe & Central Asia       2243  25992.9618  26435.1316    366.9354
#> Latin America & Caribbean   1976   7628.4477   8818.5055   1005.4085
#> Middle East & North Africa   842  13878.4213  18419.7912    578.5996
#> North America                180    48699.76  24196.2855  16405.9053
#> South Asia                   382   1235.9256   1611.2232    265.9625
#> Sub-Saharan Africa          2380   1840.0259   2596.0104    164.3366
#>                                    Max
#> East Asia & Pacific         71992.1517
#> Europe & Central Asia       196061.417
#> Latin America & Caribbean   88391.3331
#> Middle East & North Africa  116232.753
#> North America               113236.091
#> South Asia                    8476.564
#> Sub-Saharan Africa          20532.9523
#> 
#> , , Between, PCGDP
#> 
#>                             N/T        Mean          SD         Min         Max
#> East Asia & Pacific          34  10513.2441   12771.742    444.2899  39722.0077
#> Europe & Central Asia        56  25992.9618   24051.035    809.4753   141200.38
#> Latin America & Caribbean    38   7628.4477   8470.9708   1357.3326  77403.7443
#> Middle East & North Africa   20  13878.4213  17251.6962   1069.6596  64878.4021
#> North America                 3    48699.76  18604.4369  35260.4708  74934.5874
#> South Asia                    8   1235.9256   1488.3669      413.68   6621.5002
#> Sub-Saharan Africa           47   1840.0259   2234.3254    253.1886   9922.0052
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 10 slices ] 
psr[,"N/T",,]                               # Checking out the number of observations:
#> , , PCGDP
#> 
#>                             Overall  Between   Within
#> East Asia & Pacific            1467       34  43.1471
#> Europe & Central Asia          2243       56  40.0536
#> Latin America & Caribbean      1976       38       52
#> Middle East & North Africa      842       20     42.1
#> North America                   180        3       60
#> South Asia                      382        8    47.75
#> Sub-Saharan Africa             2380       47  50.6383
#> 
#> , , LIFEEX
#> 
#>                             Overall  Between   Within
#> East Asia & Pacific            1807       32  56.4688
#> Europe & Central Asia          3046       55  55.3818
#> Latin America & Caribbean      2107       40   52.675
#> Middle East & North Africa     1226       21   58.381
#> North America                   144        3       48
#> South Asia                      480        8       60
#> Sub-Saharan Africa             2860       48  59.5833
#> 
#> , , GINI
#> 
#>                             Overall  Between   Within
#> East Asia & Pacific             154       23   6.6957
#> Europe & Central Asia           798       49  16.2857
#> Latin America & Caribbean       413       25    16.52
#> Middle East & North Africa       91       15   6.0667
#> North America                    49        2     24.5
#> South Asia                       46        7   6.5714
#> Sub-Saharan Africa              193       46   4.1957
#> 
#> , , ODA
#> 
#>                             Overall  Between   Within
#> East Asia & Pacific            1537       31  49.5806
#> Europe & Central Asia           787       32  24.5938
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ] 
# In North america we only have 3 countries, for the GINI we only have 3.91 observations on average
# for 45 Sub-Saharan-African countries, etc..
psr[,"SD",,]                                # Considering only standard deviations
#> , , PCGDP
#> 
#>                                Overall     Between      Within
#> East Asia & Pacific         14383.5507   12771.742   6615.8248
#> Europe & Central Asia       26435.1316   24051.035  10971.0483
#> Latin America & Caribbean    8818.5055   8470.9708   2451.2636
#> Middle East & North Africa  18419.7912  17251.6962   6455.0512
#> North America               24196.2855  18604.4369  15470.4609
#> South Asia                   1611.2232   1488.3669    617.0934
#> Sub-Saharan Africa           2596.0104   2234.3254    1321.764
#> 
#> , , LIFEEX
#> 
#>                             Overall  Between  Within
#> East Asia & Pacific         10.1633   7.6833  6.6528
#> Europe & Central Asia        5.7602   4.4378  3.6723
#> Latin America & Caribbean    7.3768   4.9199  5.4965
#> Middle East & North Africa   9.8306    5.922  7.8467
#> North America                3.5734   1.3589  3.3049
#> South Asia                  11.3004   5.6158  9.8062
#> Sub-Saharan Africa           8.6876    5.657  6.5933
#> 
#> , , GINI
#> 
#>                             Overall  Between  Within
#> East Asia & Pacific          5.0318   4.3005  2.6125
#> Europe & Central Asia        4.5809   4.0611  2.1195
#> Latin America & Caribbean    5.4821   4.0492  3.6955
#> Middle East & North Africa   5.2073   4.7002  2.2415
#> North America                3.6972   3.3563  1.5507
#> South Asia                   3.9898   3.0052  2.6244
#> Sub-Saharan Africa           8.2003   6.8844  4.4553
#> 
#> , , ODA
#> 
#>                                    Overall         Between          Within
#> East Asia & Pacific             622'847624      457'183279      422'992450
#> Europe & Central Asia           568'237036      438'074771      361'916875
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ] 
# -> In all regions variations in inequality (GINI) between countries are greater than variations
# in inequality within countries. The opposite is true for Life-Expectancy in all regions apart
# from Europe, etc..

# Again let's do this manually for PDGCP:
d <- cbind(Overall = x,
           Between = fbetween(x, g),
           Within = fwithin(x, g, mean = "overall.mean"))

r <- qsu(d, g = wlddev$region)
r[,"N","Between"] <- fndistinct(g[!is.na(x)], wlddev$region[!is.na(x)])
r[,"N","Within"] <- r[,"N","Overall"] / r[,"N","Between"]
r
#> , , Overall
#> 
#>                                N        Mean          SD         Min
#> East Asia & Pacific         1467  10513.2441  14383.5507    132.0776
#> Europe & Central Asia       2243  25992.9618  26435.1316    366.9354
#> Latin America & Caribbean   1976   7628.4477   8818.5055   1005.4085
#> Middle East & North Africa   842  13878.4213  18419.7912    578.5996
#> North America                180    48699.76  24196.2855  16405.9053
#> South Asia                   382   1235.9256   1611.2232    265.9625
#> Sub-Saharan Africa          2380   1840.0259   2596.0104    164.3366
#>                                    Max
#> East Asia & Pacific         71992.1517
#> Europe & Central Asia       196061.417
#> Latin America & Caribbean   88391.3331
#> Middle East & North Africa  116232.753
#> North America               113236.091
#> South Asia                    8476.564
#> Sub-Saharan Africa          20532.9523
#> 
#> , , Between
#> 
#>                              N        Mean          SD         Min         Max
#> East Asia & Pacific         34  10513.2441   12771.742    444.2899  39722.0077
#> Europe & Central Asia       56  25992.9618   24051.035    809.4753   141200.38
#> Latin America & Caribbean   38   7628.4477   8470.9708   1357.3326  77403.7443
#> Middle East & North Africa  20  13878.4213  17251.6962   1069.6596  64878.4021
#> North America                3    48699.76  18604.4369  35260.4708  74934.5874
#> South Asia                   8   1235.9256   1488.3669      413.68   6621.5002
#> Sub-Saharan Africa          47   1840.0259   2234.3254    253.1886   9922.0052
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 1 slice ] 

# Proof:
qsu(wlddev, PCGDP ~ region, ~ iso3c)
#> , , Overall
#> 
#>                              N/T        Mean          SD         Min
#> East Asia & Pacific         1467  10513.2441  14383.5507    132.0776
#> Europe & Central Asia       2243  25992.9618  26435.1316    366.9354
#> Latin America & Caribbean   1976   7628.4477   8818.5055   1005.4085
#> Middle East & North Africa   842  13878.4213  18419.7912    578.5996
#> North America                180    48699.76  24196.2855  16405.9053
#> South Asia                   382   1235.9256   1611.2232    265.9625
#> Sub-Saharan Africa          2380   1840.0259   2596.0104    164.3366
#>                                    Max
#> East Asia & Pacific         71992.1517
#> Europe & Central Asia       196061.417
#> Latin America & Caribbean   88391.3331
#> Middle East & North Africa  116232.753
#> North America               113236.091
#> South Asia                    8476.564
#> Sub-Saharan Africa          20532.9523
#> 
#> , , Between
#> 
#>                             N/T        Mean          SD         Min         Max
#> East Asia & Pacific          34  10513.2441   12771.742    444.2899  39722.0077
#> Europe & Central Asia        56  25992.9618   24051.035    809.4753   141200.38
#> Latin America & Caribbean    38   7628.4477   8470.9708   1357.3326  77403.7443
#> Middle East & North Africa   20  13878.4213  17251.6962   1069.6596  64878.4021
#> North America                 3    48699.76  18604.4369  35260.4708  74934.5874
#> South Asia                    8   1235.9256   1488.3669      413.68   6621.5002
#> Sub-Saharan Africa           47   1840.0259   2234.3254    253.1886   9922.0052
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 1 slice ] 

# Weighted Summaries -----------------------
n <- nrow(wlddev)
weights <- abs(rnorm(n))                    # Generate random weights
qsu(wlddev, w = weights, higher = TRUE)     # Computed weighted mean, SD, skewness and kurtosis
#>              N   WeightSum         Mean           SD          Min
#> country  13176  10446.1645            -            -            -
#> iso3c    13176  10446.1645            -            -            -
#> date     13176  10446.1645            -            -            -
#> year     13176  10446.1645    1990.0653       17.583         1960
#> decade   13176  10446.1645     1985.626      17.4879         1960
#> region   13176  10446.1645            -            -            -
#> income   13176  10446.1645            -            -            -
#> OECD     13176  10446.1645            -            -            -
#>                     Max     Skew      Kurt
#> country               -        -         -
#> iso3c                 -        -         -
#> date                  -        -         -
#> year               2020   -0.007    1.8062
#> decade             2020   0.0297    1.8019
#> region                -        -         -
#> income                -        -         -
#> OECD                  -        -         -
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
weightsNA <- weights                        # Weights may contain missing values.. inserting 1000
weightsNA[sample.int(n, 1000)] <- NA
qsu(wlddev, w = weightsNA, higher = TRUE)   # But now these values are removed from all variables
#>              N  WeightSum         Mean          SD          Min             Max
#> country  12176  9626.6245            -           -            -               -
#> iso3c    12176  9626.6245            -           -            -               -
#> date     12176  9626.6245            -           -            -               -
#> year     12176  9626.6245    1990.1234      17.579         1960            2020
#> decade   12176  9626.6245    1985.6685     17.4903         1960            2020
#> region   12176  9626.6245            -           -            -               -
#> income   12176  9626.6245            -           -            -               -
#> OECD     12176  9626.6245            -           -            -               -
#>             Skew     Kurt
#> country        -        -
#> iso3c          -        -
#> date           -        -
#> year     -0.0103    1.801
#> decade    0.0266   1.7994
#> region         -        -
#> income         -        -
#> OECD           -        -
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]

# Grouped and panel-summaries can also be weighted in the same manner

# Alternative Output Formats ---------------
# Simple case
as.data.frame(qsu(mtcars))
#>    Variable  N       Mean          SD    Min     Max
#> 1       mpg 32  20.090625   6.0269481 10.400  33.900
#> 2       cyl 32   6.187500   1.7859216  4.000   8.000
#> 3      disp 32 230.721875 123.9386938 71.100 472.000
#> 4        hp 32 146.687500  68.5628685 52.000 335.000
#> 5      drat 32   3.596563   0.5346787  2.760   4.930
#> 6        wt 32   3.217250   0.9784574  1.513   5.424
#> 7      qsec 32  17.848750   1.7869432 14.500  22.900
#> 8        vs 32   0.437500   0.5040161  0.000   1.000
#> 9        am 32   0.406250   0.4989909  0.000   1.000
#> 10     gear 32   3.687500   0.7378041  3.000   5.000
#> 11     carb 32   2.812500   1.6152000  1.000   8.000
# For matrices can also use qDF/qDT/qTBL to assign custom name and get a character-id
qDF(qsu(mtcars), "car")
#>     car  N       Mean          SD    Min     Max
#> 1   mpg 32  20.090625   6.0269481 10.400  33.900
#> 2   cyl 32   6.187500   1.7859216  4.000   8.000
#> 3  disp 32 230.721875 123.9386938 71.100 472.000
#> 4    hp 32 146.687500  68.5628685 52.000 335.000
#> 5  drat 32   3.596563   0.5346787  2.760   4.930
#> 6    wt 32   3.217250   0.9784574  1.513   5.424
#> 7  qsec 32  17.848750   1.7869432 14.500  22.900
#> 8    vs 32   0.437500   0.5040161  0.000   1.000
#> 9    am 32   0.406250   0.4989909  0.000   1.000
#> 10 gear 32   3.687500   0.7378041  3.000   5.000
#> 11 carb 32   2.812500   1.6152000  1.000   8.000
# DF from 3D array: do not combine with aperm(), might introduce wrong column labels
as.data.frame(stats, gid = "Region_Income")
#>   Variable                             Region_Income    N       Mean         SD
#> 1    PCGDP           East Asia & Pacific.High income  487 26766.9163 14823.5175
#> 2    PCGDP   East Asia & Pacific.Lower middle income  562  1599.4748   895.5278
#> 3    PCGDP   East Asia & Pacific.Upper middle income  418  3561.0911  2468.1843
#> 4    PCGDP         Europe & Central Asia.High income 1551 35718.5696 26466.4643
#> 5    PCGDP          Europe & Central Asia.Low income   35   809.4753   336.5425
#> 6    PCGDP Europe & Central Asia.Lower middle income  125  1804.0338   964.3793
#> 7    PCGDP Europe & Central Asia.Upper middle income  532  4979.0901  2720.4775
#>         Min        Max      Skew      Kurt
#> 1  932.0417  71992.152 0.2811494  2.556594
#> 2  144.9863   4503.145 0.4987346  3.094502
#> 3  132.0776  12486.679 1.4193597  4.889957
#> 4 4500.7362 196061.417 2.2834403 10.272790
#> 5  366.9354   1458.993 0.4260281  1.998718
#> 6  534.9587   4243.774 0.6385915  2.298402
#> 7  700.7008  15190.099 1.0954612  4.170587
#>  [ reached 'max' / getOption("max.print") -- omitted 39 rows ]
# DF from 4D array: also no aperm()
as.data.frame(qsu(wlddev, ~ income, ~ iso3c, cols = 9:10), gid = "Region")
#>   Variable              Region   Trans        N/T       Mean         SD
#> 1    PCGDP         High income Overall 3179.00000 30280.7283 23847.0483
#> 2    PCGDP         High income Between   71.00000 30280.7283 20908.5323
#> 3    PCGDP         High income  Within   44.77465 12048.7780 11467.9987
#> 4    PCGDP          Low income Overall 1311.00000   597.4053   288.4392
#> 5    PCGDP          Low income Between   28.00000   597.4053   243.8219
#> 6    PCGDP          Low income  Within   46.82143 12048.7780   154.1039
#> 7    PCGDP Lower middle income Overall 2246.00000  1574.2535   858.7183
#> 8    PCGDP Lower middle income Between   47.00000  1574.2535   676.3157
#>           Min        Max
#> 1    932.0417 196061.417
#> 2   5413.4495 141200.380
#> 3 -33504.8721  76767.525
#> 4    164.3366   1864.793
#> 5    253.1886   1357.333
#> 6  11606.2382  12698.296
#> 7    144.9863   4818.192
#> 8    444.2899   2896.868
#>  [ reached 'max' / getOption("max.print") -- omitted 16 rows ]

# Output as nested list
psrl <- qsu(wlddev, ~ income, ~ iso3c, cols = 9:10, array = FALSE)
psrl
#> $PCGDP
#> $PCGDP$Overall
#>                         N        Mean          SD       Min         Max
#> High income          3179  30280.7283  23847.0483  932.0417  196061.417
#> Low income           1311    597.4053    288.4392  164.3366   1864.7925
#> Lower middle income  2246   1574.2535    858.7183  144.9863   4818.1922
#> Upper middle income  2734   4945.3258   2979.5609  132.0776  20532.9523
#> 
#> $PCGDP$Between
#>                       N        Mean          SD        Min         Max
#> High income          71  30280.7283  20908.5323  5413.4495   141200.38
#> Low income           28    597.4053    243.8219   253.1886   1357.3326
#> Lower middle income  47   1574.2535    676.3157   444.2899   2896.8682
#> Upper middle income  60   4945.3258   2327.3834   1604.595  13344.5423
#> 
#> $PCGDP$Within
#>                            N       Mean          SD          Min         Max
#> High income          44.7746  12048.778  11467.9987  -33504.8721  76767.5254
#> Low income           46.8214  12048.778    154.1039   11606.2382   12698.296
#> Lower middle income  47.7872  12048.778    529.1449   10377.7234  14603.1055
#> Upper middle income  45.5667  12048.778    1860.395    4846.3834  24883.1246
#> 
#> 
#> $LIFEEX
#> $LIFEEX$Overall
#>                         N     Mean      SD     Min      Max
#> High income          3831  73.6246  5.6693  42.672  85.4171
#> Low income           1800  49.7301  9.0944  26.172    74.43
#> Lower middle income  2790  58.1481  9.3115  18.907   76.699
#> Upper middle income  3249  66.6466   7.537  36.535   80.279
#> 
#> $LIFEEX$Between
#>                       N     Mean      SD      Min      Max
#> High income          73  73.6246  3.3499  64.0302  85.4171
#> Low income           30  49.7301  4.8321  40.9663   66.945
#> Lower middle income  47  58.1481  5.9945  45.7687  71.6078
#> Upper middle income  57  66.6466  4.9955   48.057  74.0504
#> 
#> $LIFEEX$Within
#>                            N     Mean      SD      Min      Max
#> High income          52.4795  64.2963  4.5738  42.9381  78.1271
#> Low income                60  64.2963  7.7045  41.5678  84.4198
#> Lower middle income  59.3617  64.2963  7.1253  32.9068  83.9918
#> Upper middle income       57  64.2963  5.6437  41.4342  83.0122
#> 
#> 

# We can now use unlist2d to create a tidy data frame
unlist2d(psrl, c("Variable", "Trans"), row.names = "Income")
#>   Variable   Trans              Income    N       Mean         SD       Min
#> 1    PCGDP Overall         High income 3179 30280.7283 23847.0483  932.0417
#> 2    PCGDP Overall          Low income 1311   597.4053   288.4392  164.3366
#> 3    PCGDP Overall Lower middle income 2246  1574.2535   858.7183  144.9863
#> 4    PCGDP Overall Upper middle income 2734  4945.3258  2979.5609  132.0776
#> 5    PCGDP Between         High income   71 30280.7283 20908.5323 5413.4495
#> 6    PCGDP Between          Low income   28   597.4053   243.8219  253.1886
#> 7    PCGDP Between Lower middle income   47  1574.2535   676.3157  444.2899
#> 8    PCGDP Between Upper middle income   60  4945.3258  2327.3834 1604.5950
#>          Max
#> 1 196061.417
#> 2   1864.793
#> 3   4818.192
#> 4  20532.952
#> 5 141200.380
#> 6   1357.333
#> 7   2896.868
#> 8  13344.542
#>  [ reached 'max' / getOption("max.print") -- omitted 16 rows ]
```
