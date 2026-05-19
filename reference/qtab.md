# Fast (Weighted) Cross Tabulation

A versatile and computationally more efficient replacement for
[`table`](https://rdrr.io/r/base/table.html). Notably, it also supports
tabulations with frequency weights, and computation of a statistic over
combinations of variables.

## Usage

``` r
qtab(..., w = NULL, wFUN = NULL, wFUN.args = NULL,
     dnn = "auto", sort = .op[["sort"]], na.exclude = TRUE,
     drop = FALSE, method = "auto")

qtable(...) # Long-form. Use set_collapse(mask = "table") to replace table()
```

## Arguments

- ...:

  atomic vectors or factors spanning the table dimensions, (optionally)
  with tags for the dimension names, or a data frame / list of these.
  See Examples.

- w:

  a single vector to aggregate over the table dimensions e.g. a vector
  of frequency weights.

- wFUN:

  a function used to aggregate `w` over the table dimensions. The
  default `NULL` computes the sum of the non-missing weights via an
  optimized internal algorithm. [Fast Statistical
  Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
  also receive vectorized execution.

- wFUN.args:

  a list of (optional) further arguments passed to `wFUN`. See Examples.

- dnn:

  the names of the table dimensions. Either passed directly as a
  character vector or list (internally
  [`unlist`](https://rdrr.io/r/base/unlist.html)'ed), a function applied
  to the `...` list (e.g. [`names`](https://rdrr.io/r/base/names.html),
  or
  [`vlabels`](https://fastverse.org/collapse/reference/small-helpers.md)),
  or one of the following options:

  - `"auto"` constructs names based on the `...` arguments, or calls
    [`names`](https://rdrr.io/r/base/names.html) if a single list is
    passed as input.

  - `"namlab"` does the same as `"auto"`, but also calls
    [`vlabels`](https://fastverse.org/collapse/reference/small-helpers.md)
    on the list and appends the names by the variable labels.

  `dnn = NULL` will return a table without dimension names.

- sort, na.exclude, drop, method:

  arguments passed down to
  [`qF`](https://fastverse.org/collapse/reference/qF.md):

  - `sort = FALSE` orders table dimensions in first-appearance order of
    items in the data (can be more efficient if vectors are not factors
    already). Note that for factors this option will both recast levels
    in first-appearance order and drop unused levels.

  - `na.exclude = FALSE` includes `NA`'s in the table (equivalent to
    [`table`](https://rdrr.io/r/base/table.html)'s `useNA = "ifany"`).

  - `drop = TRUE` removes any unused factor levels (= zero frequency
    rows or columns).

  - `method %in% c("radix", "hash")` provides additional control over
    the algorithm used to convert atomic vectors to factors.

## Value

An array of class 'qtab' that inherits from 'table'. Thus all 'table'
methods apply to it.

## See also

[`descr`](https://fastverse.org/collapse/reference/descr.md), [Summary
Statistics](https://fastverse.org/collapse/reference/summary-statistics.md),
[Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Basic use
qtab(iris$Species)
#> iris$Species
#>     setosa versicolor  virginica 
#>         50         50         50 
with(mtcars, qtab(vs, am))
#>    am
#> vs   0  1
#>   0 12  6
#>   1  7  7
qtab(mtcars[.c(vs, am)])
#>    am
#> vs   0  1
#>   0 12  6
#>   1  7  7

library(magrittr)
iris %$% qtab(Sepal.Length > mean(Sepal.Length), Species)
#>                                  Species
#> Sepal.Length > mean(Sepal.Length) setosa versicolor virginica
#>                             FALSE     50         24         6
#>                             TRUE       0         26        44
iris %$% qtab(AMSL = Sepal.Length > mean(Sepal.Length), Species)
#>        Species
#> AMSL    setosa versicolor virginica
#>   FALSE     50         24         6
#>   TRUE       0         26        44

## World after 2015
wlda15 <- wlddev |> fsubset(year >= 2015) |> collap(~ iso3c)

# Regions and income levels (country frequency)
wlda15 %$% qtab(region, income)
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific                 13          0                  13
#>   Europe & Central Asia               37          1                   4
#>   Latin America & Caribbean           17          1                   4
#>   Middle East & North Africa           8          2                   5
#>   North America                        3          0                   0
#>   South Asia                           0          2                   4
#>   Sub-Saharan Africa                   1         24                  17
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                         10
#>   Europe & Central Asia                       16
#>   Latin America & Caribbean                   20
#>   Middle East & North Africa                   6
#>   North America                                0
#>   South Asia                                   2
#>   Sub-Saharan Africa                           6
wlda15 %$% qtab(region, income, dnn = vlabels)
#>                             Income Level
#> Region                       High income Low income Lower middle income
#>   East Asia & Pacific                 13          0                  13
#>   Europe & Central Asia               37          1                   4
#>   Latin America & Caribbean           17          1                   4
#>   Middle East & North Africa           8          2                   5
#>   North America                        3          0                   0
#>   South Asia                           0          2                   4
#>   Sub-Saharan Africa                   1         24                  17
#>                             Income Level
#> Region                       Upper middle income
#>   East Asia & Pacific                         10
#>   Europe & Central Asia                       16
#>   Latin America & Caribbean                   20
#>   Middle East & North Africa                   6
#>   North America                                0
#>   South Asia                                   2
#>   Sub-Saharan Africa                           6
wlda15 %$% qtab(region, income, dnn = "namlab")
#>                             income: Income Level
#> region: Region               High income Low income Lower middle income
#>   East Asia & Pacific                 13          0                  13
#>   Europe & Central Asia               37          1                   4
#>   Latin America & Caribbean           17          1                   4
#>   Middle East & North Africa           8          2                   5
#>   North America                        3          0                   0
#>   South Asia                           0          2                   4
#>   Sub-Saharan Africa                   1         24                  17
#>                             income: Income Level
#> region: Region               Upper middle income
#>   East Asia & Pacific                         10
#>   Europe & Central Asia                       16
#>   Latin America & Caribbean                   20
#>   Middle East & North Africa                   6
#>   North America                                0
#>   South Asia                                   2
#>   Sub-Saharan Africa                           6

# Population (millions)
wlda15 %$% qtab(region, income, w = POP) |> divide_by(1e6)
#>                             income
#> region                        High income   Low income Lower middle income
#>   East Asia & Pacific         222.3763078    0.0000000         554.5787740
#>   Europe & Central Asia       499.6178162    8.8839460          86.1694676
#>   Latin America & Caribbean    32.1055806   10.9808262          33.3947544
#>   Middle East & North Africa   64.6289084   45.1314580         148.8518602
#>   North America               361.3653800    0.0000000           0.0000000
#>   South Asia                    0.0000000   63.9814276        1706.8229478
#>   Sub-Saharan Africa            0.0956652  530.2676994         450.7138318
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific               1486.9566064
#>   Europe & Central Asia              319.5918390
#>   Latin America & Caribbean          557.9670800
#>   Middle East & North Africa         182.6471952
#>   North America                        0.0000000
#>   South Asia                          21.9126958
#>   Sub-Saharan Africa                  66.1922298

# Life expectancy (years)
wlda15 %$% qtab(region, income, w = LIFEEX, wFUN = fmean)
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific           81.12986                       69.32372
#>   Europe & Central Asia         80.39692   70.63140            71.44826
#>   Latin America & Caribbean     77.57964   63.26640            73.18650
#>   Middle East & North Africa    78.38796   68.61450            72.73452
#>   North America                 80.67948                               
#>   South Asia                               67.13770            69.81250
#>   Sub-Saharan Africa            73.93805   60.63336            62.37481
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                   73.32000
#>   Europe & Central Asia                 74.39151
#>   Latin America & Caribbean             74.69349
#>   Middle East & North Africa            74.78043
#>   North America                                 
#>   South Asia                            77.48130
#>   Sub-Saharan Africa                    65.54593

# Life expectancy (years), weighted by population
wlda15 %$% qtab(region, income, w = LIFEEX, wFUN = fmean,
                  wFUN.args = list(w = POP))
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific           83.47390                       71.19076
#>   Europe & Central Asia         81.31296   70.63140            71.46540
#>   Latin America & Caribbean     78.99973   63.26640            73.01552
#>   Middle East & North Africa    76.84332   68.02666            73.12402
#>   North America                 78.97586                               
#>   South Asia                               66.73455            69.15123
#>   Sub-Saharan Africa            73.93805   62.01679            58.97093
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                   76.43505
#>   Europe & Central Asia                 73.96995
#>   Latin America & Caribbean             75.44067
#>   Middle East & North Africa            74.93824
#>   North America                                 
#>   South Asia                            76.68486
#>   Sub-Saharan Africa                    63.79719

# GDP per capita (constant 2010 US$): median
wlda15 %$% qtab(region, income, w = PCGDP, wFUN = fmedian,
                  wFUN.args = list(na.rm = TRUE))
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific         37527.1689                      1863.8773
#>   Europe & Central Asia       44340.1356  1026.2292           2654.3445
#>   Latin America & Caribbean   16514.1173  1266.0910           2323.2438
#>   Middle East & North Africa  31003.0939   677.4104           3134.9231
#>   North America               53790.1303                               
#>   South Asia                               677.2265           1558.9751
#>   Sub-Saharan Africa          14057.6452   635.8458           1657.2758
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                  5364.0199
#>   Europe & Central Asia                6552.2411
#>   Latin America & Caribbean            6824.2245
#>   Middle East & North Africa           5920.2114
#>   North America                                 
#>   South Asia                           5883.8904
#>   Sub-Saharan Africa                   8578.7116

# GDP per capita (constant 2010 US$): median, weighted by population
wlda15 %$% qtab(region, income, w = PCGDP, wFUN = fmedian,
                  wFUN.args = list(w = POP))
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific         48194.0408                      3038.6464
#>   Europe & Central Asia       42912.4250  1026.2292           3010.4318
#>   Latin America & Caribbean   14888.7707  1266.0910           2483.3969
#>   Middle East & North Africa  20945.0874   677.4104           2840.9586
#>   North America               53790.1303                               
#>   South Asia                               570.9192           1970.4413
#>   Sub-Saharan Africa          14057.6452   543.6803           2430.1368
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                  7360.8953
#>   Europe & Central Asia               11623.6264
#>   Latin America & Caribbean           10231.2777
#>   Middle East & North Africa           6255.4776
#>   North America                                 
#>   South Asia                           3846.9157
#>   Sub-Saharan Africa                   7457.1928

# Including OECD membership
tab <- wlda15 %$% qtab(region, income, OECD)
tab
#> , , OECD = FALSE
#> 
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific                  9          0                  13
#>   Europe & Central Asia               11          1                   4
#>   Latin America & Caribbean           16          1                   4
#>   Middle East & North Africa           7          2                   5
#>   North America                        1          0                   0
#>   South Asia                           0          2                   4
#>   Sub-Saharan Africa                   1         24                  17
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                         10
#>   Europe & Central Asia                       15
#>   Latin America & Caribbean                   19
#>   Middle East & North Africa                   6
#>   North America                                0
#>   South Asia                                   2
#>   Sub-Saharan Africa                           6
#> 
#> , , OECD = TRUE
#> 
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific                  4          0                   0
#>   Europe & Central Asia               26          0                   0
#>   Latin America & Caribbean            1          0                   0
#>   Middle East & North Africa           1          0                   0
#>   North America                        2          0                   0
#>   South Asia                           0          0                   0
#>   Sub-Saharan Africa                   0          0                   0
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                          0
#>   Europe & Central Asia                        1
#>   Latin America & Caribbean                    1
#>   Middle East & North Africa                   0
#>   North America                                0
#>   South Asia                                   0
#>   Sub-Saharan Africa                           0
#> 

# Various 'table' methods
tab |> addmargins()
#> , , OECD = FALSE
#> 
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific                  9          0                  13
#>   Europe & Central Asia               11          1                   4
#>   Latin America & Caribbean           16          1                   4
#>   Middle East & North Africa           7          2                   5
#>   North America                        1          0                   0
#>   South Asia                           0          2                   4
#>   Sub-Saharan Africa                   1         24                  17
#>   Sum                                 45         30                  47
#>                             income
#> region                       Upper middle income Sum
#>   East Asia & Pacific                         10  32
#>   Europe & Central Asia                       15  31
#>   Latin America & Caribbean                   19  40
#>   Middle East & North Africa                   6  20
#>   North America                                0   1
#>   South Asia                                   2   8
#>   Sub-Saharan Africa                           6  48
#>   Sum                                         58 180
#> 
#> , , OECD = TRUE
#> 
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific                  4          0                   0
#>   Europe & Central Asia               26          0                   0
#>   Latin America & Caribbean            1          0                   0
#>   Middle East & North Africa           1          0                   0
#>   North America                        2          0                   0
#>   South Asia                           0          0                   0
#>                             income
#> region                       Upper middle income Sum
#>   East Asia & Pacific                          0   4
#>   Europe & Central Asia                        1  27
#>   Latin America & Caribbean                    1   2
#>   Middle East & North Africa                   0   1
#>   North America                                0   2
#>   South Asia                                   0   0
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 1 slice ] 
tab |> marginSums(margin = c("region", "income"))
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific                 13          0                  13
#>   Europe & Central Asia               37          1                   4
#>   Latin America & Caribbean           17          1                   4
#>   Middle East & North Africa           8          2                   5
#>   North America                        3          0                   0
#>   South Asia                           0          2                   4
#>   Sub-Saharan Africa                   1         24                  17
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                         10
#>   Europe & Central Asia                       16
#>   Latin America & Caribbean                   20
#>   Middle East & North Africa                   6
#>   North America                                0
#>   South Asia                                   2
#>   Sub-Saharan Africa                           6
tab |> proportions()
#> , , OECD = FALSE
#> 
#>                             income
#> region                       High income  Low income Lower middle income
#>   East Asia & Pacific        0.041666667 0.000000000         0.060185185
#>   Europe & Central Asia      0.050925926 0.004629630         0.018518519
#>   Latin America & Caribbean  0.074074074 0.004629630         0.018518519
#>   Middle East & North Africa 0.032407407 0.009259259         0.023148148
#>   North America              0.004629630 0.000000000         0.000000000
#>   South Asia                 0.000000000 0.009259259         0.018518519
#>   Sub-Saharan Africa         0.004629630 0.111111111         0.078703704
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                0.046296296
#>   Europe & Central Asia              0.069444444
#>   Latin America & Caribbean          0.087962963
#>   Middle East & North Africa         0.027777778
#>   North America                      0.000000000
#>   South Asia                         0.009259259
#>   Sub-Saharan Africa                 0.027777778
#> 
#> , , OECD = TRUE
#> 
#>                             income
#> region                       High income  Low income Lower middle income
#>   East Asia & Pacific        0.018518519 0.000000000         0.000000000
#>   Europe & Central Asia      0.120370370 0.000000000         0.000000000
#>   Latin America & Caribbean  0.004629630 0.000000000         0.000000000
#>   Middle East & North Africa 0.004629630 0.000000000         0.000000000
#>   North America              0.009259259 0.000000000         0.000000000
#>   South Asia                 0.000000000 0.000000000         0.000000000
#>   Sub-Saharan Africa         0.000000000 0.000000000         0.000000000
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                0.000000000
#>   Europe & Central Asia              0.004629630
#>   Latin America & Caribbean          0.004629630
#>   Middle East & North Africa         0.000000000
#>   North America                      0.000000000
#>   South Asia                         0.000000000
#>   Sub-Saharan Africa                 0.000000000
#> 
tab |> proportions(margin = "income")
#> , , OECD = FALSE
#> 
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific         0.11392405 0.00000000          0.27659574
#>   Europe & Central Asia       0.13924051 0.03333333          0.08510638
#>   Latin America & Caribbean   0.20253165 0.03333333          0.08510638
#>   Middle East & North Africa  0.08860759 0.06666667          0.10638298
#>   North America               0.01265823 0.00000000          0.00000000
#>   South Asia                  0.00000000 0.06666667          0.08510638
#>   Sub-Saharan Africa          0.01265823 0.80000000          0.36170213
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                 0.16666667
#>   Europe & Central Asia               0.25000000
#>   Latin America & Caribbean           0.31666667
#>   Middle East & North Africa          0.10000000
#>   North America                       0.00000000
#>   South Asia                          0.03333333
#>   Sub-Saharan Africa                  0.10000000
#> 
#> , , OECD = TRUE
#> 
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific         0.05063291 0.00000000          0.00000000
#>   Europe & Central Asia       0.32911392 0.00000000          0.00000000
#>   Latin America & Caribbean   0.01265823 0.00000000          0.00000000
#>   Middle East & North Africa  0.01265823 0.00000000          0.00000000
#>   North America               0.02531646 0.00000000          0.00000000
#>   South Asia                  0.00000000 0.00000000          0.00000000
#>   Sub-Saharan Africa          0.00000000 0.00000000          0.00000000
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                 0.00000000
#>   Europe & Central Asia               0.01666667
#>   Latin America & Caribbean           0.01666667
#>   Middle East & North Africa          0.00000000
#>   North America                       0.00000000
#>   South Asia                          0.00000000
#>   Sub-Saharan Africa                  0.00000000
#> 
as.data.frame(tab) |> head(10)
#>                        region      income  OECD Freq
#> 1         East Asia & Pacific High income FALSE    9
#> 2       Europe & Central Asia High income FALSE   11
#> 3   Latin America & Caribbean High income FALSE   16
#> 4  Middle East & North Africa High income FALSE    7
#> 5               North America High income FALSE    1
#> 6                  South Asia High income FALSE    0
#> 7          Sub-Saharan Africa High income FALSE    1
#> 8         East Asia & Pacific  Low income FALSE    0
#> 9       Europe & Central Asia  Low income FALSE    1
#> 10  Latin America & Caribbean  Low income FALSE    1
ftable(tab, row.vars = c("region", "OECD"))
#>                                  income High income Low income Lower middle income Upper middle income
#> region                     OECD                                                                       
#> East Asia & Pacific        FALSE                  9          0                  13                  10
#>                            TRUE                   4          0                   0                   0
#> Europe & Central Asia      FALSE                 11          1                   4                  15
#>                            TRUE                  26          0                   0                   1
#> Latin America & Caribbean  FALSE                 16          1                   4                  19
#>                            TRUE                   1          0                   0                   1
#> Middle East & North Africa FALSE                  7          2                   5                   6
#>                            TRUE                   1          0                   0                   0
#> North America              FALSE                  1          0                   0                   0
#>                            TRUE                   2          0                   0                   0
#> South Asia                 FALSE                  0          2                   4                   2
#>                            TRUE                   0          0                   0                   0
#> Sub-Saharan Africa         FALSE                  1         24                  17                   6
#>                            TRUE                   0          0                   0                   0

# Other options
tab |> fsum(TRA = "%")    # Percentage table (on a matrix use fsum.default)
#> , , OECD = FALSE
#> 
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific          4.1666667  0.0000000           6.0185185
#>   Europe & Central Asia        5.0925926  0.4629630           1.8518519
#>   Latin America & Caribbean    7.4074074  0.4629630           1.8518519
#>   Middle East & North Africa   3.2407407  0.9259259           2.3148148
#>   North America                0.4629630  0.0000000           0.0000000
#>   South Asia                   0.0000000  0.9259259           1.8518519
#>   Sub-Saharan Africa           0.4629630 11.1111111           7.8703704
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                  4.6296296
#>   Europe & Central Asia                6.9444444
#>   Latin America & Caribbean            8.7962963
#>   Middle East & North Africa           2.7777778
#>   North America                        0.0000000
#>   South Asia                           0.9259259
#>   Sub-Saharan Africa                   2.7777778
#> 
#> , , OECD = TRUE
#> 
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific          1.8518519  0.0000000           0.0000000
#>   Europe & Central Asia       12.0370370  0.0000000           0.0000000
#>   Latin America & Caribbean    0.4629630  0.0000000           0.0000000
#>   Middle East & North Africa   0.4629630  0.0000000           0.0000000
#>   North America                0.9259259  0.0000000           0.0000000
#>   South Asia                   0.0000000  0.0000000           0.0000000
#>   Sub-Saharan Africa           0.0000000  0.0000000           0.0000000
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                  0.0000000
#>   Europe & Central Asia                0.4629630
#>   Latin America & Caribbean            0.4629630
#>   Middle East & North Africa           0.0000000
#>   North America                        0.0000000
#>   South Asia                           0.0000000
#>   Sub-Saharan Africa                   0.0000000
#> 
tab %/=% (sum(tab)/100)    # Another way (division by reference, preserves integers)
tab
#> , , OECD = FALSE
#> 
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific                  4          0                   6
#>   Europe & Central Asia                5          0                   2
#>   Latin America & Caribbean            8          0                   2
#>   Middle East & North Africa           3          1                   2
#>   North America                        0          0                   0
#>   South Asia                           0          1                   2
#>   Sub-Saharan Africa                   0         12                   8
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                          5
#>   Europe & Central Asia                        7
#>   Latin America & Caribbean                    9
#>   Middle East & North Africa                   3
#>   North America                                0
#>   South Asia                                   1
#>   Sub-Saharan Africa                           3
#> 
#> , , OECD = TRUE
#> 
#>                             income
#> region                       High income Low income Lower middle income
#>   East Asia & Pacific                  2          0                   0
#>   Europe & Central Asia               13          0                   0
#>   Latin America & Caribbean            0          0                   0
#>   Middle East & North Africa           0          0                   0
#>   North America                        1          0                   0
#>   South Asia                           0          0                   0
#>   Sub-Saharan Africa                   0          0                   0
#>                             income
#> region                       Upper middle income
#>   East Asia & Pacific                          0
#>   Europe & Central Asia                        0
#>   Latin America & Caribbean                    0
#>   Middle East & North Africa                   0
#>   North America                                0
#>   South Asia                                   0
#>   Sub-Saharan Africa                           0
#> 

rm(tab, wlda15)
```
