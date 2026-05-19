# Detailed Statistical Description of Data Frame

`descr` offers a fast and detailed description of each variable in a
data frame. Since v1.9.0 it fully supports grouped and weighted
computations.

## Usage

``` r
descr(X, ...)

# Default S3 method
descr(X, by = NULL, w = NULL, cols = NULL,
      Ndistinct = TRUE, higher = TRUE, table = TRUE, sort.table = "freq",
      Qprobs = c(0.01, 0.05, 0.1, 0.25, 0.5, 0.75, 0.9, 0.95, 0.99), Qtype = 7L,
      label.attr = "label", stepwise = FALSE, ...)

# S3 method for class 'grouped_df'
descr(X, w = NULL,
      Ndistinct = TRUE, higher = TRUE, table = TRUE, sort.table = "freq",
      Qprobs = c(0.01, 0.05, 0.1, 0.25, 0.5, 0.75, 0.9, 0.95, 0.99), Qtype = 7L,
      label.attr = "label", stepwise = FALSE, ...)

# S3 method for class 'descr'
as.data.frame(x, ..., gid = "Group")

# S3 method for class 'descr'
print(x, n = 14, perc = TRUE, digits = .op[["digits"]], t.table = TRUE, total = TRUE,
      compact = FALSE, summary = !compact, reverse = FALSE, stepwise = FALSE, ...)
```

## Arguments

- X:

  a (grouped) data frame or list of atomic vectors. Atomic vectors,
  matrices or arrays can be passed but will first be coerced to data
  frame using
  [`qDF`](https://fastverse.org/collapse/reference/quick-conversion.md).

- by:

  a factor, [`GRP`](https://fastverse.org/collapse/reference/GRP.md)
  object, or atomic vector / list of vectors (internally grouped with
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md)), or a one-
  or two-sided formula e.g. `~ group1` or
  `var1 + var2 ~ group1 + group2` to group `X`. See Examples.

- w:

  a numeric vector of (non-negative) weights. the default method also
  supports a one-sided formulas i.e. `~ weightcol` or
  `~ log(weightcol)`. The `grouped_df` method supports lazy-expressions
  (same without `~`). See Examples.

- cols:

  select columns to describe using column names, indices a logical
  vector or selector function (e.g. `is.numeric`). *Note*: `cols` is
  ignored if a two-sided formula is passed to `by`.

- Ndistinct:

  logical. `TRUE` (default) computes the number of distinct values on
  all variables using
  [`fndistinct`](https://fastverse.org/collapse/reference/fndistinct.md).

- higher:

  logical. Argument is passed down to
  [`qsu`](https://fastverse.org/collapse/reference/qsu.md): `TRUE`
  (default) computes the skewness and the kurtosis.

- table:

  logical. `TRUE` (default) computes a (sorted) frequency table for all
  categorical variables (excluding
  [Date](https://fastverse.org/collapse/reference/small-helpers.md)
  variables).

- sort.table:

  an integer or character string specifying how the frequency table
  should be presented:

  |  |  |  |  |  |
  |----|----|----|----|----|
  | *Int.* |  | *String* |  | *Description* |
  | 1 |  | "value" |  | sort table by values. |
  | 2 |  | "freq" |  | sort table by frequencies. |
  | 3 |  | "none" |  | return table in first-appearance order of values, or levels for factors (most efficient). |

- Qprobs:

  double. Probabilities for quantiles to compute on numeric variables,
  passed down to
  [`.quantile`](https://fastverse.org/collapse/reference/fquantile.html).
  If something non-numeric is passed (i.e. `NULL`, `FALSE`, `NA`, `""`
  etc.), no quantiles are computed.

- Qtype:

  integer. Quantile types 5-9 following Hyndman and Fan (1996) who
  recommended type 8, default 7 as in
  [`quantile`](https://rdrr.io/r/stats/quantile.html).

- label.attr:

  character. The name of a label attribute to display for each variable
  (if variables are labeled).

- ...:

  for `descr`: other arguments passed to
  [`qsu.default`](https://fastverse.org/collapse/reference/qsu.md). For
  `[.descr`: variable names or indices passed to `[.list`. The argument
  is unused in the `print` and `as.data.frame` methods.

- x:

  an object of class 'descr'.

- n:

  integer. The maximum number of table elements to print for categorical
  variables. If the number of distinct elements is `<= n`, the whole
  table is printed. Otherwise the remaining items are summed into an
  '... %s Others' category.

- perc:

  logical. `TRUE` (default) adds percentages to the frequencies in the
  table for categorical variables, and, if `!is.null(by)`, the
  percentage of observations in each group.

- digits:

  integer. The number of decimals to print in statistics, quantiles and
  percentage tables.

- t.table:

  logical. `TRUE` (default) prints a transposed table.

- total:

  logical. `TRUE` (default) adds a 'Total' column for grouped tables
  (when using `by` argument).

- compact:

  logical. `TRUE` combines statistics and quantiles to generate a more
  compact printout. Especially useful with groups (`by`).

- summary:

  logical. `TRUE` (default) computes and displays a summary of the
  frequencies, if the size of the table for a categorical variable
  exceeds `n`.

- reverse:

  logical. `TRUE` prints contents in reverse order, starting with the
  last column, so that the dataset can be analyzed by scrolling up the
  console after calling `descr`.

- stepwise:

  logical. `TRUE` prints one variable at a time. The user needs to press
  \[enter\] to see the printout for the next variable. If called from
  `descr`, the computation is also done one variable at a time, and the
  finished 'descr' object is returned invisibly.

- gid:

  character. Name assigned to the group-id column, when describing data
  by groups.

## Details

`descr` was heavily inspired by `Hmisc::describe`, but is much faster
and has more advanced statistical capabilities. It is principally a
wrapper around [`qsu`](https://fastverse.org/collapse/reference/qsu.md),
[`fquantile`](https://fastverse.org/collapse/reference/fquantile.md)
(`.quantile`), and
[`fndistinct`](https://fastverse.org/collapse/reference/fndistinct.md)
for numeric variables, and computes frequency tables for categorical
variables using
[`qtab`](https://fastverse.org/collapse/reference/qtab.md). Date
variables are summarized with
[`fnobs`](https://fastverse.org/collapse/reference/fnobs.md),
[`fndistinct`](https://fastverse.org/collapse/reference/fndistinct.md)
and [`frange`](https://fastverse.org/collapse/reference/fquantile.md).

Since v1.9.0 grouped and weighted computations are fully supported. The
use of sampling weights will produce a weighted mean, sd, skewness and
kurtosis, and weighted quantiles for numeric data. For categorical data,
tables will display the sum of weights instead of the frequencies, and
percentage tables as well as the percentage of missing values indicated
next to 'Statistics' in print, be relative to the total sum of weights.
All this can be done by groups. Grouped (weighted) quantiles are
computed using [`BY`](https://fastverse.org/collapse/reference/BY.md).

For larger datasets, calling the `stepwise` option directly from
`descr()` is recommended, as precomputing the statistics for all
variables before digesting the results can be time consuming.

The list-object returned from `descr` can efficiently be converted to a
tidy data frame using the `as.data.frame` method. This representation
will not include frequency tables computed for categorical variables.

## Value

A 2-level nested list-based object of class 'descr'. The list has the
same size as the dataset, and contains the statistics computed for each
variable, which are themselves stored in a list containing the class,
the label, the basic statistics and quantiles / tables computed for the
variable (in matrix form).

The object has attributes attached providing the 'name' of the dataset,
the number of rows in the dataset ('N'), an attribute 'arstat'
indicating whether arrays of statistics where generated by passing
arguments (e.g. `pid`) down to `qsu.default`, an attribute 'table'
indicating whether `table = TRUE` (i.e. the object could contain tables
for categorical variables), and attributes 'groups' and/or 'weights'
providing a [`GRP`](https://fastverse.org/collapse/reference/GRP.md)
object and/or weight vector for grouped and/or weighted data
descriptions.

## See also

[`qsu`](https://fastverse.org/collapse/reference/qsu.md),
[`qtab`](https://fastverse.org/collapse/reference/qtab.md),
[`fquantile`](https://fastverse.org/collapse/reference/fquantile.md),
[`pwcor`](https://fastverse.org/collapse/reference/pwcor_pwcov_pwnobs.md),
[Summary
Statistics](https://fastverse.org/collapse/reference/summary-statistics.md),
[Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Simple Use
descr(iris)
#> Dataset: iris, 5 Variables, N = 150
#> --------------------------------------------------------------------------------
#> Sepal.Length (numeric): 
#> Statistics
#>     N  Ndist  Mean    SD  Min  Max  Skew  Kurt
#>   150     35  5.84  0.83  4.3  7.9  0.31  2.43
#> Quantiles
#>    1%   5%  10%  25%  50%  75%  90%   95%  99%
#>   4.4  4.6  4.8  5.1  5.8  6.4  6.9  7.25  7.7
#> --------------------------------------------------------------------------------
#> Sepal.Width (numeric): 
#> Statistics
#>     N  Ndist  Mean    SD  Min  Max  Skew  Kurt
#>   150     23  3.06  0.44    2  4.4  0.32  3.18
#> Quantiles
#>    1%    5%  10%  25%  50%  75%   90%  95%   99%
#>   2.2  2.34  2.5  2.8    3  3.3  3.61  3.8  4.15
#> --------------------------------------------------------------------------------
#> Petal.Length (numeric): 
#> Statistics
#>     N  Ndist  Mean    SD  Min  Max   Skew  Kurt
#>   150     43  3.76  1.77    1  6.9  -0.27   1.6
#> Quantiles
#>     1%   5%  10%  25%   50%  75%  90%  95%  99%
#>   1.15  1.3  1.4  1.6  4.35  5.1  5.8  6.1  6.7
#> --------------------------------------------------------------------------------
#> Petal.Width (numeric): 
#> Statistics
#>     N  Ndist  Mean    SD  Min  Max  Skew  Kurt
#>   150     22   1.2  0.76  0.1  2.5  -0.1  1.66
#> Quantiles
#>    1%   5%  10%  25%  50%  75%  90%  95%  99%
#>   0.1  0.2  0.2  0.3  1.3  1.8  2.2  2.3  2.5
#> --------------------------------------------------------------------------------
#> Species (factor): 
#> Statistics
#>     N  Ndist
#>   150      3
#> Table
#>             Freq   Perc
#> setosa        50  33.33
#> versicolor    50  33.33
#> virginica     50  33.33
#> --------------------------------------------------------------------------------
descr(wlddev)
#> Dataset: wlddev, 13 Variables, N = 13176
#> --------------------------------------------------------------------------------
#> country (character): Country Name
#> Statistics
#>       N  Ndist
#>   13176    216
#> Table
#>                       Freq   Perc
#> Afghanistan             61   0.46
#> Albania                 61   0.46
#> Algeria                 61   0.46
#> American Samoa          61   0.46
#> Andorra                 61   0.46
#> Angola                  61   0.46
#> Antigua and Barbuda     61   0.46
#> Argentina               61   0.46
#> Armenia                 61   0.46
#> Aruba                   61   0.46
#> Australia               61   0.46
#> Austria                 61   0.46
#> Azerbaijan              61   0.46
#> Bahamas, The            61   0.46
#> ... 202 Others       12322  93.52
#> 
#> Summary of Table Frequencies
#>    Min. 1st Qu.  Median    Mean 3rd Qu.    Max. 
#>      61      61      61      61      61      61 
#> --------------------------------------------------------------------------------
#> iso3c (factor): Country Code
#> Statistics
#>       N  Ndist
#>   13176    216
#> Table
#>                  Freq   Perc
#> ABW                61   0.46
#> AFG                61   0.46
#> AGO                61   0.46
#> ALB                61   0.46
#> AND                61   0.46
#> ARE                61   0.46
#> ARG                61   0.46
#> ARM                61   0.46
#> ASM                61   0.46
#> ATG                61   0.46
#> AUS                61   0.46
#> AUT                61   0.46
#> AZE                61   0.46
#> BDI                61   0.46
#> ... 202 Others  12322  93.52
#> 
#> Summary of Table Frequencies
#>    Min. 1st Qu.  Median    Mean 3rd Qu.    Max. 
#>      61      61      61      61      61      61 
#> --------------------------------------------------------------------------------
#> date (Date): Date Recorded (Fictitious)
#> Statistics
#>          N       Ndist         Min         Max  
#>      13176          61  1961-01-01  2021-01-01  
#> --------------------------------------------------------------------------------
#> year (integer): Year
#> Statistics
#>       N  Ndist  Mean     SD   Min   Max  Skew  Kurt
#>   13176     61  1990  17.61  1960  2020    -0   1.8
#> Quantiles
#>     1%    5%   10%   25%   50%   75%   90%   95%   99%
#>   1960  1963  1966  1975  1990  2005  2014  2017  2020
#> --------------------------------------------------------------------------------
#> decade (integer): Decade
#> Statistics
#>       N  Ndist     Mean     SD   Min   Max  Skew  Kurt
#>   13176      7  1985.57  17.51  1960  2020  0.03  1.79
#> Quantiles
#>     1%    5%   10%   25%   50%   75%   90%   95%   99%
#>   1960  1960  1960  1970  1990  2000  2010  2010  2020
#> --------------------------------------------------------------------------------
#> region (factor): Region
#> Statistics
#>       N  Ndist
#>   13176      7
#> Table
#>                             Freq   Perc
#> Europe & Central Asia       3538  26.85
#> Sub-Saharan Africa          2928  22.22
#> Latin America & Caribbean   2562  19.44
#> East Asia & Pacific         2196  16.67
#> Middle East & North Africa  1281   9.72
#> South Asia                   488   3.70
#> North America                183   1.39
#> --------------------------------------------------------------------------------
#> income (factor): Income Level
#> Statistics
#>       N  Ndist
#>   13176      4
#> Table
#>                      Freq   Perc
#> High income          4819  36.57
#> Upper middle income  3660  27.78
#> Lower middle income  2867  21.76
#> Low income           1830  13.89
#> --------------------------------------------------------------------------------
#> OECD (logical): Is OECD Member Country?
#> Statistics
#>       N  Ndist
#>   13176      2
#> Table
#>         Freq   Perc
#> FALSE  10980  83.33
#> TRUE    2196  16.67
#> --------------------------------------------------------------------------------
#> PCGDP (numeric): GDP per capita (constant 2010 US$)
#> Statistics (28.13% NAs)
#>      N  Ndist      Mean        SD     Min        Max  Skew   Kurt
#>   9470   9470  12048.78  19077.64  132.08  196061.42  3.13  17.12
#> Quantiles
#>       1%      5%     10%      25%      50%       75%       90%       95%
#>   227.71  399.62  555.55  1303.19  3767.16  14787.03  35646.02  48507.84
#>        99%
#>   92340.28
#> --------------------------------------------------------------------------------
#> LIFEEX (numeric): Life expectancy at birth, total (years)
#> Statistics (11.43% NAs)
#>       N  Ndist  Mean     SD    Min    Max   Skew  Kurt
#>   11670  10548  64.3  11.48  18.91  85.42  -0.67  2.67
#> Quantiles
#>      1%     5%    10%    25%    50%    75%    90%    95%    99%
#>   35.83  42.77  46.83  56.36  67.44  72.95  77.08  79.34  82.36
#> --------------------------------------------------------------------------------
#> GINI (numeric): Gini index (World Bank estimate)
#> Statistics (86.76% NAs)
#>      N  Ndist   Mean   SD   Min   Max  Skew  Kurt
#>   1744    368  38.53  9.2  20.7  65.8   0.6  2.53
#> Quantiles
#>     1%    5%   10%   25%   50%  75%   90%    95%   99%
#>   24.6  26.3  27.6  31.5  36.4   45  52.6  55.98  60.5
#> --------------------------------------------------------------------------------
#> ODA (numeric): Net official development assistance and official aid received (constant 2018 US$)
#> Statistics (34.67% NAs)
#>      N  Ndist        Mean          SD          Min             Max  Skew
#>   8608   7832  454'720131  868'712654  -997'679993  2.56715605e+10  6.98
#>     Kurt
#>   114.89
#> Quantiles
#>             1%           5%          10%          25%         50%         75%
#>   -12'593999.7  1'363500.01  8'347000.31  44'887499.8  165'970001  495'042503
#>              90%             95%             99%
#>   1.18400697e+09  1.93281696e+09  3.73380782e+09
#> --------------------------------------------------------------------------------
#> POP (numeric): Population, total
#> Statistics (1.95% NAs)
#>       N  Ndist         Mean          SD   Min             Max  Skew    Kurt
#>   12919  12877  24'245971.6  102'120674  2833  1.39771500e+09  9.75  108.91
#> Quantiles
#>        1%       5%      10%     25%       50%        75%          90%
#>   8698.84  31083.3  62268.4  443791  4'072517  12'816178  46'637331.4
#>           95%         99%
#>   81'177252.5  308'862641
#> --------------------------------------------------------------------------------
descr(GGDC10S)
#> Dataset: GGDC10S, 16 Variables, N = 5027
#> --------------------------------------------------------------------------------
#> Country (character): Country
#> Statistics
#>      N  Ndist
#>   5027     43
#> Table
#>                Freq   Perc
#> USA             129   2.57
#> EGY             129   2.57
#> MOR             128   2.55
#> IDN             126   2.51
#> PHL             126   2.51
#> TWN             126   2.51
#> DNK             126   2.51
#> ESP             126   2.51
#> FRA             126   2.51
#> GBR             126   2.51
#> ITA             126   2.51
#> NLD             126   2.51
#> SWE             126   2.51
#> CHN             125   2.49
#> ... 29 Others  3256  64.77
#> 
#> Summary of Table Frequencies
#>    Min. 1st Qu.  Median    Mean 3rd Qu.    Max. 
#>       4     105     124     117     126     129 
#> --------------------------------------------------------------------------------
#> Regioncode (character): Region code
#> Statistics
#>      N  Ndist
#>   5027      6
#> Table
#>       Freq   Perc
#> ASI   1372  27.29
#> SSA   1148  22.84
#> LAM   1117  22.22
#> EUR   1004  19.97
#> MENA   257   5.11
#> NAM    129   2.57
#> --------------------------------------------------------------------------------
#> Region (character): Region
#> Statistics
#>      N  Ndist
#>   5027      6
#> Table
#>                               Freq   Perc
#> Asia                          1372  27.29
#> Sub-saharan Africa            1148  22.84
#> Latin America                 1117  22.22
#> Europe                        1004  19.97
#> Middle East and North Africa   257   5.11
#> North America                  129   2.57
#> --------------------------------------------------------------------------------
#> Variable (character): Variable
#> Statistics
#>      N  Ndist
#>   5027      2
#> Table
#>      Freq   Perc
#> EMP  2516  50.05
#> VA   2511  49.95
#> --------------------------------------------------------------------------------
#> Year (numeric): Year
#> Statistics
#>      N  Ndist     Mean     SD   Min   Max   Skew  Kurt
#>   5027     67  1981.58  17.57  1947  2013  -0.05  1.86
#> Quantiles
#>     1%    5%   10%   25%   50%   75%   90%   95%   99%
#>   1950  1953  1957  1967  1982  1997  2006  2009  2011
#> --------------------------------------------------------------------------------
#> AGR (numeric): Agriculture 
#> Statistics (13.19% NAs)
#>      N  Ndist        Mean           SD  Min             Max   Skew    Kurt
#>   4364   4353  2'526696.5  37'129098.1    0  1.19187778e+09  23.95  642.16
#> Quantiles
#>     1%     5%     10%    25%      50%       75%     90%          95%
#>   0.09  23.18  144.56  930.7  4394.52  29781.04  315403  2'393977.49
#>           99%
#>   24'932575.1
#> --------------------------------------------------------------------------------
#> MIN (numeric): Mining
#> Statistics (13.37% NAs)
#>      N  Ndist         Mean           SD  Min             Max   Skew    Kurt
#>   4355   4224  1'867908.95  32'334251.7    0  1.10344053e+09  25.27  712.33
#> Quantiles
#>     1%   5%   10%    25%     50%      75%       90%        95%          99%
#>   0.02  1.2  3.78  38.95  173.22  4841.26  64810.08  713420.36  14'309891.9
#> --------------------------------------------------------------------------------
#> MAN (numeric): Manufacturing
#> Statistics (13.37% NAs)
#>      N  Ndist         Mean           SD  Min             Max   Skew   Kurt
#>   4355   4353  5'538491.36  63'090998.4    0  1.86843541e+09  20.71  498.7
#> Quantiles
#>     1%     5%     10%     25%     50%       75%        90%          95%
#>   0.05  27.31  103.84  620.44  3718.1  52805.35  516077.08  2'978846.99
#>          99%
#>   108'499037
#> --------------------------------------------------------------------------------
#> PU (numeric): Utilities
#> Statistics (13.39% NAs)
#>      N  Ndist       Mean           SD  Min          Max  Skew    Kurt
#>   4354   4237  335679.47  2'576027.41    0  65'324543.8  13.5  244.29
#> Quantiles
#>   1%    5%  10%    25%     50%      75%       90%        95%          99%
#>    0  2.16  6.3  25.74  167.95  4892.25  63004.56  291356.48  11'866259.3
#> --------------------------------------------------------------------------------
#> CON (numeric): Construction
#> Statistics (13.37% NAs)
#>      N  Ndist         Mean         SD  Min         Max   Skew    Kurt
#>   4355   4339  1'801597.63  24'382598    0  860'638677  26.15  774.73
#> Quantiles
#>     1%     5%    10%     25%      50%       75%        90%        95%
#>   0.02  15.03  43.37  215.57  1473.45  13514.84  132609.51  829361.57
#>           99%
#>   37'430603.6
#> --------------------------------------------------------------------------------
#> WRT (numeric): Trade, restaurants and hotels
#> Statistics (13.37% NAs)
#>      N  Ndist         Mean           SD  Min             Max   Skew    Kurt
#>   4355   4344  3'392909.52  36'950812.9    0  1.15497404e+09  21.19  530.06
#> Quantiles
#>     1%     5%    10%     25%      50%       75%        90%          95%
#>   0.03  25.07  96.85  650.38  3773.64  41648.17  475116.68  2'646521.57
#>           99%
#>   79'618054.2
#> --------------------------------------------------------------------------------
#> TRA (numeric): Transport, storage and communication
#> Statistics (13.37% NAs)
#>      N  Ndist         Mean           SD  Min         Max   Skew    Kurt
#>   4355   4334  1'473269.72  16'815143.2    0  547'047040  22.77  604.58
#> Quantiles
#>     1%     5%    10%     25%     50%       75%        90%          95%
#>   0.05  12.28  37.35  205.79  1174.8  18927.21  195055.31  1'059843.16
#>           99%
#>   31'750009.1
#> --------------------------------------------------------------------------------
#> FIRE (numeric): Finance, insurance, real estate and business services
#> Statistics (13.37% NAs)
#>      N  Ndist         Mean           SD       Min         Max   Skew    Kurt
#>   4355   4349  1'657114.84  13'709981.9  -2848.81  387'997506  16.48  356.43
#> Quantiles
#>   1%    5%   10%     25%     50%      75%        90%          95%        99%
#>    0  3.87  14.3  128.18  960.13  13460.4  252299.08  1'599086.92  55'536957
#> --------------------------------------------------------------------------------
#> GOV (numeric): Government services
#> Statistics (30.73% NAs)
#>      N  Ndist         Mean           SD  Min         Max   Skew    Kurt
#>   3482   3470  1'712300.28  16'967383.7    0  485'535400  18.67  430.18
#> Quantiles
#>     1%     5%     10%     25%      50%       75%        90%          95%
#>   0.02  48.14  121.87  723.98  3928.51  37689.12  331990.24  1'400263.37
#>           99%
#>   56'340246.3
#> --------------------------------------------------------------------------------
#> OTH (numeric): Community, social and personal services
#> Statistics (15.5% NAs)
#>      N  Ndist         Mean           SD  Min         Max   Skew    Kurt
#>   4248   4238  1'684527.32  15'613923.6    0  402'671182  14.93  273.79
#> Quantiles
#>     1%     5%    10%     25%      50%       75%        90%        95%
#>   0.02  15.92  49.56  310.09  1433.17  13321.29  107230.29  605013.39
#>           99%
#>   42'264477.4
#> --------------------------------------------------------------------------------
#> SUM (numeric): Summation of sector GDP
#> Statistics (13.19% NAs)
#>      N  Ndist         Mean          SD  Min             Max   Skew    Kurt
#>   4364   4364  21'566436.8  251'812500    0  8.06794210e+09  22.53  589.58
#> Quantiles
#>     1%      5%      10%      25%       50%        75%          90%          95%
#>   0.38  269.63  1242.98  4803.94  23186.19  284646.08  2'644610.11  15'030223.5
#>          99%
#>   435'513356
#> --------------------------------------------------------------------------------

# Some useful print options (also try stepwise argument)
print(descr(GGDC10S), reverse = TRUE, t.table = FALSE)
#> SUM (numeric): Summation of sector GDP
#> Statistics (13.19% NAs)
#>      N  Ndist         Mean          SD  Min             Max   Skew    Kurt
#>   4364   4364  21'566436.8  251'812500    0  8.06794210e+09  22.53  589.58
#> Quantiles
#>     1%      5%      10%      25%       50%        75%          90%          95%
#>   0.38  269.63  1242.98  4803.94  23186.19  284646.08  2'644610.11  15'030223.5
#>          99%
#>   435'513356
#> --------------------------------------------------------------------------------
#> OTH (numeric): Community, social and personal services
#> Statistics (15.5% NAs)
#>      N  Ndist         Mean           SD  Min         Max   Skew    Kurt
#>   4248   4238  1'684527.32  15'613923.6    0  402'671182  14.93  273.79
#> Quantiles
#>     1%     5%    10%     25%      50%       75%        90%        95%
#>   0.02  15.92  49.56  310.09  1433.17  13321.29  107230.29  605013.39
#>           99%
#>   42'264477.4
#> --------------------------------------------------------------------------------
#> GOV (numeric): Government services
#> Statistics (30.73% NAs)
#>      N  Ndist         Mean           SD  Min         Max   Skew    Kurt
#>   3482   3470  1'712300.28  16'967383.7    0  485'535400  18.67  430.18
#> Quantiles
#>     1%     5%     10%     25%      50%       75%        90%          95%
#>   0.02  48.14  121.87  723.98  3928.51  37689.12  331990.24  1'400263.37
#>           99%
#>   56'340246.3
#> --------------------------------------------------------------------------------
#> FIRE (numeric): Finance, insurance, real estate and business services
#> Statistics (13.37% NAs)
#>      N  Ndist         Mean           SD       Min         Max   Skew    Kurt
#>   4355   4349  1'657114.84  13'709981.9  -2848.81  387'997506  16.48  356.43
#> Quantiles
#>   1%    5%   10%     25%     50%      75%        90%          95%        99%
#>    0  3.87  14.3  128.18  960.13  13460.4  252299.08  1'599086.92  55'536957
#> --------------------------------------------------------------------------------
#> TRA (numeric): Transport, storage and communication
#> Statistics (13.37% NAs)
#>      N  Ndist         Mean           SD  Min         Max   Skew    Kurt
#>   4355   4334  1'473269.72  16'815143.2    0  547'047040  22.77  604.58
#> Quantiles
#>     1%     5%    10%     25%     50%       75%        90%          95%
#>   0.05  12.28  37.35  205.79  1174.8  18927.21  195055.31  1'059843.16
#>           99%
#>   31'750009.1
#> --------------------------------------------------------------------------------
#> WRT (numeric): Trade, restaurants and hotels
#> Statistics (13.37% NAs)
#>      N  Ndist         Mean           SD  Min             Max   Skew    Kurt
#>   4355   4344  3'392909.52  36'950812.9    0  1.15497404e+09  21.19  530.06
#> Quantiles
#>     1%     5%    10%     25%      50%       75%        90%          95%
#>   0.03  25.07  96.85  650.38  3773.64  41648.17  475116.68  2'646521.57
#>           99%
#>   79'618054.2
#> --------------------------------------------------------------------------------
#> CON (numeric): Construction
#> Statistics (13.37% NAs)
#>      N  Ndist         Mean         SD  Min         Max   Skew    Kurt
#>   4355   4339  1'801597.63  24'382598    0  860'638677  26.15  774.73
#> Quantiles
#>     1%     5%    10%     25%      50%       75%        90%        95%
#>   0.02  15.03  43.37  215.57  1473.45  13514.84  132609.51  829361.57
#>           99%
#>   37'430603.6
#> --------------------------------------------------------------------------------
#> PU (numeric): Utilities
#> Statistics (13.39% NAs)
#>      N  Ndist       Mean           SD  Min          Max  Skew    Kurt
#>   4354   4237  335679.47  2'576027.41    0  65'324543.8  13.5  244.29
#> Quantiles
#>   1%    5%  10%    25%     50%      75%       90%        95%          99%
#>    0  2.16  6.3  25.74  167.95  4892.25  63004.56  291356.48  11'866259.3
#> --------------------------------------------------------------------------------
#> MAN (numeric): Manufacturing
#> Statistics (13.37% NAs)
#>      N  Ndist         Mean           SD  Min             Max   Skew   Kurt
#>   4355   4353  5'538491.36  63'090998.4    0  1.86843541e+09  20.71  498.7
#> Quantiles
#>     1%     5%     10%     25%     50%       75%        90%          95%
#>   0.05  27.31  103.84  620.44  3718.1  52805.35  516077.08  2'978846.99
#>          99%
#>   108'499037
#> --------------------------------------------------------------------------------
#> MIN (numeric): Mining
#> Statistics (13.37% NAs)
#>      N  Ndist         Mean           SD  Min             Max   Skew    Kurt
#>   4355   4224  1'867908.95  32'334251.7    0  1.10344053e+09  25.27  712.33
#> Quantiles
#>     1%   5%   10%    25%     50%      75%       90%        95%          99%
#>   0.02  1.2  3.78  38.95  173.22  4841.26  64810.08  713420.36  14'309891.9
#> --------------------------------------------------------------------------------
#> AGR (numeric): Agriculture 
#> Statistics (13.19% NAs)
#>      N  Ndist        Mean           SD  Min             Max   Skew    Kurt
#>   4364   4353  2'526696.5  37'129098.1    0  1.19187778e+09  23.95  642.16
#> Quantiles
#>     1%     5%     10%    25%      50%       75%     90%          95%
#>   0.09  23.18  144.56  930.7  4394.52  29781.04  315403  2'393977.49
#>           99%
#>   24'932575.1
#> --------------------------------------------------------------------------------
#> Year (numeric): Year
#> Statistics
#>      N  Ndist     Mean     SD   Min   Max   Skew  Kurt
#>   5027     67  1981.58  17.57  1947  2013  -0.05  1.86
#> Quantiles
#>     1%    5%   10%   25%   50%   75%   90%   95%   99%
#>   1950  1953  1957  1967  1982  1997  2006  2009  2011
#> --------------------------------------------------------------------------------
#> Variable (character): Variable
#> Statistics
#>      N  Ndist
#>   5027      2
#> Table
#>         EMP     VA
#> Freq   2516   2511
#> Perc  50.05  49.95
#> --------------------------------------------------------------------------------
#> Region (character): Region
#> Statistics
#>      N  Ndist
#>   5027      6
#> Table
#>        Asia  Sub-saharan Africa  Latin America  Europe
#> Freq   1372                1148           1117    1004
#> Perc  27.29               22.84          22.22   19.97
#>       Middle East and North Africa  North America
#> Freq                           257            129
#> Perc                          5.11           2.57
#> --------------------------------------------------------------------------------
#> Regioncode (character): Region code
#> Statistics
#>      N  Ndist
#>   5027      6
#> Table
#>         ASI    SSA    LAM    EUR  MENA   NAM
#> Freq   1372   1148   1117   1004   257   129
#> Perc  27.29  22.84  22.22  19.97  5.11  2.57
#> --------------------------------------------------------------------------------
#> Country (character): Country
#> Statistics
#>      N  Ndist
#>   5027     43
#> Table
#>        USA   EGY   MOR   IDN   PHL   TWN   DNK   ESP   FRA   GBR   ITA   NLD
#> Freq   129   129   128   126   126   126   126   126   126   126   126   126
#> Perc  2.57  2.57  2.55  2.51  2.51  2.51  2.51  2.51  2.51  2.51  2.51  2.51
#>        SWE   CHN  ... 29 Others
#> Freq   126   125           3256
#> Perc  2.51  2.49          64.77
#> 
#> Summary of Table Frequencies
#>    Min. 1st Qu.  Median    Mean 3rd Qu.    Max. 
#>       4     105     124     117     126     129 
#> --------------------------------------------------------------------------------
#> Dataset: GGDC10S, 16 Variables, N = 5027
# For bigger data consider: descr(big_data, stepwise = TRUE)

# Generating a data frame
as.data.frame(descr(wlddev, table = FALSE))
#>   Variable     Class                      Label     N Ndist   Min   Max Mean SD
#> 1  country character               Country Name 13176   216    NA    NA   NA NA
#> 2    iso3c    factor               Country Code 13176   216    NA    NA   NA NA
#> 3     date      Date Date Recorded (Fictitious) 13176    61 -3287 18628   NA NA
#>   Skew Kurt 1% 5% 10% 25% 50% 75% 90% 95% 99%
#> 1   NA   NA NA NA  NA  NA  NA  NA  NA  NA  NA
#> 2   NA   NA NA NA  NA  NA  NA  NA  NA  NA  NA
#> 3   NA   NA NA NA  NA  NA  NA  NA  NA  NA  NA
#>  [ reached 'max' / getOption("max.print") -- omitted 10 rows ]

## Weighted Desciptions
descr(wlddev, w = ~ replace_na(POP)) # replacing NA's with 0's for fquantile()
#> Dataset: wlddev, 12 Variables, N = 13176, WeightSum = 313233706778
#> --------------------------------------------------------------------------------
#> country (character): Country Name
#> Statistics
#>        WeightSum  Ndist
#>   3.13233707e+11    216
#> Table
#>                        WeightSum   Perc
#> China                65272180000  20.84
#> India                52835203044  16.87
#> United States        15226426293   4.86
#> Indonesia            10681870259   3.41
#> Brazil                8711884458   2.78
#> Russian Federation    8388319293   2.68
#> Japan                 7088669911   2.26
#> Pakistan              6865420747   2.19
#> Bangladesh            6217567789   1.98
#> Nigeria               6191168112   1.98
#> Mexico                4948012523   1.58
#> Germany               4773054666   1.52
#> Vietnam               3955178878   1.26
#> Philippines           3805345278   1.21
#> ... 202 Others      108273405527  34.57
#> 
#> Summary of Table WeightSums
#>    Min. 1st Qu.  Median    Mean 3rd Qu.    Max. 
#> 5.0e+05 3.1e+07 2.6e+08 1.5e+09 8.3e+08 6.5e+10 
#> --------------------------------------------------------------------------------
#> iso3c (factor): Country Code
#> Statistics
#>        WeightSum  Ndist
#>   3.13233707e+11    216
#> Table
#>                    WeightSum   Perc
#> CHN              65272180000  20.84
#> IND              52835203044  16.87
#> USA              15226426293   4.86
#> IDN              10681870259   3.41
#> BRA               8711884458   2.78
#> RUS               8388319293   2.68
#> JPN               7088669911   2.26
#> PAK               6865420747   2.19
#> BGD               6217567789   1.98
#> NGA               6191168112   1.98
#> MEX               4948012523   1.58
#> DEU               4773054666   1.52
#> VNM               3955178878   1.26
#> PHL               3805345278   1.21
#> ... 202 Others  108273405527  34.57
#> 
#> Summary of Table WeightSums
#>    Min. 1st Qu.  Median    Mean 3rd Qu.    Max. 
#> 5.0e+05 3.1e+07 2.6e+08 1.5e+09 8.3e+08 6.5e+10 
#> --------------------------------------------------------------------------------
#> date (Date): Date Recorded (Fictitious)
#> Statistics
#>          N       Ndist         Min         Max  
#>      13176          61  1961-01-01  2021-01-01  
#> --------------------------------------------------------------------------------
#> year (integer): Year
#> Statistics (1.95% NAs)
#>       N  Ndist       WeightSum    Mean     SD   Min   Max   Skew  Kurt
#>   12919     61  3.13233707e+11  1994.1  16.75  1960  2019  -0.32  1.97
#> Quantiles
#>     1%    5%   10%   25%   50%   75%   90%   95%   99%
#>   1961  1965  1969  1981  1996  2009  2015  2017  2019
#> --------------------------------------------------------------------------------
#> decade (integer): Decade
#> Statistics (1.95% NAs)
#>       N  Ndist       WeightSum     Mean     SD   Min   Max   Skew  Kurt
#>   12919      7  3.13233707e+11  1989.47  16.52  1960  2010  -0.33  1.91
#> Quantiles
#>     1%    5%   10%   25%   50%   75%   90%   95%   99%
#>   1960  1960  1960  1980  1990  2000  2010  2010  2010
#> --------------------------------------------------------------------------------
#> region (factor): Region
#> Statistics
#>        WeightSum  Ndist
#>   3.13233707e+11      7
#> Table
#>                                WeightSum   Perc
#> East Asia & Pacific         103222500256  32.95
#> South Asia                   69206603131  22.09
#> Europe & Central Asia        49081067014  15.67
#> Sub-Saharan Africa           33308413094  10.63
#> Latin America & Caribbean    26135854126   8.34
#> North America                16881058226   5.39
#> Middle East & North Africa   15398210931   4.92
#> --------------------------------------------------------------------------------
#> income (factor): Income Level
#> Statistics
#>        WeightSum  Ndist
#>   3.13233707e+11      4
#> Table
#>                         WeightSum   Perc
#> Upper middle income  119606023798  38.18
#> Lower middle income  113837684528  36.34
#> High income           58840837058  18.78
#> Low income            20949161394   6.69
#> --------------------------------------------------------------------------------
#> OECD (logical): Is OECD Member Country?
#> Statistics
#>        WeightSum  Ndist
#>   3.13233707e+11      2
#> Table
#>           WeightSum  Perc
#> FALSE  249344473835  79.6
#> TRUE    63889232943  20.4
#> --------------------------------------------------------------------------------
#> PCGDP (numeric): GDP per capita (constant 2010 US$)
#> Statistics (28.13% NAs)
#>      N  Ndist       WeightSum     Mean        SD     Min        Max  Skew  Kurt
#>   9470   9470  2.95445830e+11  7956.24  12984.91  132.08  196061.42  2.19  7.22
#> Quantiles
#>       1%      5%     10%     25%      50%      75%       90%       95%
#>   164.32  263.48  370.57  711.93  1875.79  7786.69  29257.66  41278.55
#>        99%
#>   51732.26
#> --------------------------------------------------------------------------------
#> LIFEEX (numeric): Life expectancy at birth, total (years)
#> Statistics (11.51% NAs)
#>       N  Ndist       WeightSum   Mean    SD    Min    Max   Skew  Kurt
#>   11659  10548  3.12878084e+11  65.88  9.75  18.91  85.42  -0.73  2.96
#> Quantiles
#>      1%     5%    10%    25%    50%    75%    90%    95%   99%
#>   41.73  46.11  50.79  60.14  68.29  72.98  76.58  78.69  82.5
#> --------------------------------------------------------------------------------
#> GINI (numeric): Gini index (World Bank estimate)
#> Statistics (86.76% NAs)
#>      N  Ndist       WeightSum   Mean    SD   Min   Max  Skew  Kurt
#>   1744    368  8.26010770e+10  39.52  7.61  20.7  65.8  0.88  3.63
#> Quantiles
#>   1%    5%   10%   25%    50%   75%    90%   95%    99%
#>   26  29.2  31.3  34.3  39.16  42.2  52.03  55.6  59.98
#> --------------------------------------------------------------------------------
#> ODA (numeric): Net official development assistance and official aid received (constant 2018 US$)
#> Statistics (34.75% NAs)
#>      N  Ndist       WeightSum            Mean              SD          Min
#>   8597   7832  2.31451603e+11  1.61325042e+09  1.63323654e+09  -997'679993
#>              Max  Skew   Kurt
#>   2.56715605e+10  1.75  11.89
#> Quantiles
#>            1%           5%          10%         25%             50%
#>   -907'107452  -327'497633  56'138226.8  332'261483  1.34542532e+09
#>              75%             90%             95%             99%
#>   2.51256879e+09  3.52051981e+09  4.48714786e+09  8.06805848e+09
#> --------------------------------------------------------------------------------

## Grouped Desciptions
descr(GGDC10S, ~ Variable)
#> Dataset: GGDC10S, 15 Variables, N = 5027
#> Grouped by: Variable [2]
#>         N   Perc
#> EMP  2516  50.05
#> VA   2511  49.95
#> --------------------------------------------------------------------------------
#> Country (character): Country
#> Statistics (N = 5027)
#>         N   Perc  Ndist
#> EMP  2516  50.05     42
#> VA   2511  49.95     43
#> 
#> Table (Freq Perc)
#>                      EMP         VA      Total
#> USA              64  2.5    65  2.6   129  2.6
#> EGY              65  2.6    64  2.5   129  2.6
#> MOR              65  2.6    63  2.5   128  2.5
#> IDN              63  2.5    63  2.5   126  2.5
#> PHL              63  2.5    63  2.5   126  2.5
#> TWN              63  2.5    63  2.5   126  2.5
#> DNK              64  2.5    62  2.5   126  2.5
#> ESP              64  2.5    62  2.5   126  2.5
#> FRA              64  2.5    62  2.5   126  2.5
#> GBR              64  2.5    62  2.5   126  2.5
#> ITA              64  2.5    62  2.5   126  2.5
#> NLD              64  2.5    62  2.5   126  2.5
#> SWE              64  2.5    62  2.5   126  2.5
#> CHN              62  2.5    63  2.5   125  2.5
#> ... 29 Others  1623 64.5  1633 65.0  3256 64.8
#> 
#> Summary of Table Frequencies
#>       EMP              VA           Total      
#>  Min.   : 0.00   Min.   : 4.0   Min.   :  4.0  
#>  1st Qu.:52.00   1st Qu.:53.0   1st Qu.:105.0  
#>  Median :62.00   Median :62.0   Median :124.0  
#>  Mean   :58.51   Mean   :58.4   Mean   :116.9  
#>  3rd Qu.:63.00   3rd Qu.:62.0   3rd Qu.:126.0  
#>  Max.   :65.00   Max.   :65.0   Max.   :129.0  
#> --------------------------------------------------------------------------------
#> Regioncode (character): Region code
#> Statistics (N = 5027)
#>         N   Perc  Ndist
#> EMP  2516  50.05      6
#> VA   2511  49.95      6
#> 
#> Table (Freq Perc)
#>             EMP         VA      Total
#> ASI    684 27.2   688 27.4  1372 27.3
#> SSA    571 22.7   577 23.0  1148 22.8
#> LAM    558 22.2   559 22.3  1117 22.2
#> EUR    509 20.2   495 19.7  1004 20.0
#> MENA   130  5.2   127  5.1   257  5.1
#> NAM     64  2.5    65  2.6   129  2.6
#> --------------------------------------------------------------------------------
#> Region (character): Region
#> Statistics (N = 5027)
#>         N   Perc  Ndist
#> EMP  2516  50.05      6
#> VA   2511  49.95      6
#> 
#> Table (Freq Perc)
#>                                     EMP         VA      Total
#> Asia                           684 27.2   688 27.4  1372 27.3
#> Sub-saharan Africa             571 22.7   577 23.0  1148 22.8
#> Latin America                  558 22.2   559 22.3  1117 22.2
#> Europe                         509 20.2   495 19.7  1004 20.0
#> Middle East and North Africa   130  5.2   127  5.1   257  5.1
#> North America                   64  2.5    65  2.6   129  2.6
#> --------------------------------------------------------------------------------
#> Year (numeric): Year
#> Statistics (N = 5027)
#>         N   Perc  Ndist     Mean     SD   Min   Max   Skew  Kurt
#> EMP  2516  50.05     66  1981.38  17.61  1947  2012  -0.05  1.86
#> VA   2511  49.95     67  1981.78  17.53  1947  2013  -0.05  1.85
#> 
#> Quantiles
#>        1%    5%   10%   25%   50%   75%   90%   95%   99%
#> EMP  1950  1953  1957  1967  1982  1997  2006  2009  2011
#> VA   1950  1953  1958  1967  1982  1997  2006  2009  2011
#> --------------------------------------------------------------------------------
#> AGR (numeric): Agriculture 
#> Statistics (N = 4364, 13.19% NAs)
#>         N   Perc  Ndist         Mean           SD   Min             Max   Skew
#> EMP  2225  50.99   2219     16746.43     55644.84  5.24          390980   4.58
#> VA   2139  49.01   2135  5'137560.88  52'913681.8     0  1.19187778e+09  16.74
#>        Kurt
#> EMP   23.76
#> VA   314.45
#> 
#> Quantiles
#>        1%     5%     10%     25%       50%        75%          90%          95%
#> EMP  7.67  67.33  187.94  752.73   2168.96    5762.23     18285.84      48898.1
#> VA      0   3.11   72.85  1976.5  21040.45  156589.16  2'563619.99  8'693829.74
#>              99%
#> EMP     310671.2
#> VA   47'607622.9
#> --------------------------------------------------------------------------------
#> MIN (numeric): Mining
#> Statistics (N = 4355, 13.37% NAs)
#>         N   Perc  Ndist         Mean           SD   Min             Max   Skew
#> EMP  2216  50.88   2153       359.61      1295.29  0.11        12908.36   6.64
#> VA   2139  49.12   2072  3'802686.57  46'062895.7     0  1.10344053e+09  17.68
#>        Kurt
#> EMP   50.77
#> VA   349.32
#> 
#> Quantiles
#>        1%    5%    10%     25%    50%       75%       90%          95%
#> EMP  0.21  1.19   2.37   18.05  56.44    144.22    675.54      1145.26
#> VA      0  1.49  12.29  327.55   4642  35481.94  746910.9  1'843491.16
#>              99%
#> EMP      8839.77
#> VA   35'804017.4
#> --------------------------------------------------------------------------------
#> MAN (numeric): Manufacturing
#> Statistics (N = 4355, 13.37% NAs)
#>         N   Perc  Ndist         Mean           SD   Min             Max  Skew
#> EMP  2216  50.88   2214      5204.33     13924.82  1.04        145898.4  6.18
#> VA   2139  49.12   2139  11'270966.4  89'674720.3     0  1.86843541e+09  14.5
#>        Kurt
#> EMP   48.12
#> VA   245.16
#> 
#> Quantiles
#>         1%     5%     10%      25%       50%        75%         90%        95%
#> EMP  10.85  63.22  114.55   439.01   1188.59    4235.75    11914.54   18920.24
#> VA       0      3   51.07  3220.02  48182.47  267410.31  3'034481.4  24'608297
#>             99%
#> EMP    84869.43
#> VA   220'767719
#> --------------------------------------------------------------------------------
#> PU (numeric): Utilities
#> Statistics (N = 4354, 13.39% NAs)
#>         N   Perc  Ndist       Mean           SD   Min          Max  Skew
#> EMP  2215  50.87   2141     153.42       365.12  0.12      3903.81  6.47
#> VA   2139  49.13   2097  683126.98  3'643270.26     0  65'324543.8  9.43
#>        Kurt
#> EMP    54.8
#> VA   120.69
#> 
#> Quantiles
#>        1%    5%   10%     25%      50%       75%        90%          95%
#> EMP  1.28  3.97  6.19   14.85    40.69    144.25     356.06        589.6
#> VA      0  0.37   7.7  329.13  5185.74  34756.69  305247.84  1'843310.67
#>              99%
#> EMP      1661.89
#> VA   17'121707.3
#> --------------------------------------------------------------------------------
#> CON (numeric): Construction
#> Statistics (N = 4355, 13.37% NAs)
#>         N   Perc  Ndist         Mean         SD   Min         Max   Skew
#> EMP  2216  50.88   2209      1793.61    5114.13  1.71    69887.56   7.17
#> VA   2139  49.12   2130  3'666191.22  34'696912     0  860'638677  18.32
#>        Kurt
#> EMP   63.74
#> VA   380.89
#> 
#> Quantiles
#>         1%     5%    10%     25%       50%       75%        90%         95%
#> EMP  11.65  32.58  45.26  140.32    450.36   1664.01    3991.46     5910.12
#> VA       0   1.34  30.97  964.91  12628.28  80096.67  871467.04  7'884387.9
#>              99%
#> EMP     29914.57
#> VA   58'732994.5
#> --------------------------------------------------------------------------------
#> WRT (numeric): Trade, restaurants and hotels
#> Statistics (N = 4355, 13.37% NAs)
#>         N   Perc  Ndist        Mean           SD   Min             Max   Skew
#> EMP  2216  50.88   2212     4368.38      8616.85  1.64        84165.11   4.29
#> VA   2139  49.12   2132  6'903431.8  52'500538.5     0  1.15497404e+09  14.85
#>        Kurt
#> EMP   25.93
#> VA   260.94
#> 
#> Quantiles
#>         1%     5%     10%      25%       50%        75%          90%
#> EMP  15.15  58.31  111.22   459.61   1447.36     4228.6     11405.12
#> VA       0    3.6   65.03  2955.05  39897.72  256568.47  2'712856.32
#>              95%          99%
#> EMP     18215.36     49580.74
#> VA   18'710060.6  94'112882.1
#> --------------------------------------------------------------------------------
#> TRA (numeric): Transport, storage and communication
#> Statistics (N = 4355, 13.37% NAs)
#>         N   Perc  Ndist         Mean           SD   Min         Max   Skew
#> EMP  2216  50.88   2203      1442.44      3289.42  1.73    31222.74   5.31
#> VA   2139  49.12   2131  2'998080.02  23'900671.1     0  547'047040  15.96
#>        Kurt
#> EMP   36.86
#> VA   297.62
#> 
#> Quantiles
#>        1%     5%    10%      25%      50%        75%          90%         95%
#> EMP  5.53  22.39  38.99   135.87   406.94     1178.6      3545.03     5644.88
#> VA      0   1.72  28.86  1374.15  18552.4  113658.26  1'098376.42  7'637158.2
#>              99%
#> EMP        20475
#> VA   53'478514.1
#> --------------------------------------------------------------------------------
#> FIRE (numeric): Finance, insurance, real estate and business services
#> Statistics (N = 4355, 13.37% NAs)
#>         N   Perc  Ndist         Mean         SD       Min         Max   Skew
#> EMP  2216  50.88   2216      1330.68    3113.74      0.78    28092.69   5.07
#> VA   2139  49.12   2133  3'372504.14  19'416463  -2848.81  387'997506  11.55
#>        Kurt
#> EMP   35.06
#> VA   176.34
#> 
#> Quantiles
#>          1%    5%    10%     25%       50%        75%          90%          95%
#> EMP    2.56  7.03  14.42   68.77     298.8    1034.59      3356.25      6749.82
#> VA   -48.78  0.14  13.28  894.75  12776.55  143622.56  1'627030.46  11'657805.1
#>              99%
#> EMP     18330.87
#> VA   67'958580.7
#> --------------------------------------------------------------------------------
#> GOV (numeric): Government services
#> Statistics (N = 3482, 30.73% NAs)
#>         N   Perc  Ndist         Mean           SD  Min         Max   Skew
#> EMP  1780  51.12   1772      4196.81      7278.04    0    44817.34   3.14
#> VA   1702  48.88   1698  3'498683.46  24'143501.2    0  485'535400  13.04
#>        Kurt
#> EMP   13.76
#> VA   210.91
#> 
#> Quantiles
#>        1%     5%     10%      25%       50%      75%          90%          95%
#> EMP  20.8  57.72  120.85    409.9   1413.06  4413.38     10919.28     20592.43
#> VA      0  13.88  129.17  3029.88  37694.55   232193  1'448746.76  4'846006.68
#>            99%
#> EMP   38132.65
#> VA   81'906146
#> --------------------------------------------------------------------------------
#> OTH (numeric): Community, social and personal services
#> Statistics (N = 4248, 15.5% NAs)
#>         N   Perc  Ndist         Mean           SD   Min         Max   Skew
#> EMP  2109  49.65   2106      2268.11      8022.24  4.07   104517.87   9.48
#> VA   2139  50.35   2132  3'343192.42  21'880087.7     0  402'671182  10.55
#>        Kurt
#> EMP  102.76
#> VA   137.84
#> 
#> Quantiles
#>         1%     5%    10%     25%       50%       75%        90%          95%
#> EMP  20.32  34.82  84.19  233.75    699.45    1672.3    4121.24      7461.79
#> VA       0    2.1  20.09  787.54  10963.92  65040.92  598083.51  8'741638.08
#>              99%
#> EMP     23123.63
#> VA   91'548932.3
#> --------------------------------------------------------------------------------
#> SUM (numeric): Summation of sector GDP
#> Statistics (N = 4364, 13.19% NAs)
#>         N   Perc  Ndist         Mean          SD     Min             Max   Skew
#> EMP  2225  50.99   2225     36846.87    96318.65  173.88          764200   5.02
#> VA   2139  49.01   2139  43'961639.1  358'350627       0  8.06794210e+09  15.77
#>        Kurt
#> EMP   30.98
#> VA   289.46
#> 
#> Quantiles
#>          1%      5%      10%      25%        50%          75%          90%
#> EMP  256.12  599.38  1599.27  3555.62    9593.98      24801.5     66975.01
#> VA        0   25.01   444.54    21302  243186.47  1'396139.11  15'926968.3
#>             95%         99%
#> EMP   152402.28    550909.6
#> VA   104'405351  692'993893
#> --------------------------------------------------------------------------------
descr(wlddev, ~ income)
#> Dataset: wlddev, 12 Variables, N = 13176
#> Grouped by: income [4]
#>                         N   Perc
#> High income          4819  36.57
#> Low income           1830  13.89
#> Lower middle income  2867  21.76
#> Upper middle income  3660  27.78
#> --------------------------------------------------------------------------------
#> country (character): Country Name
#> Statistics (N = 13176)
#>                         N   Perc  Ndist
#> High income          4819  36.57     79
#> Low income           1830  13.89     30
#> Lower middle income  2867  21.76     47
#> Upper middle income  3660  27.78     60
#> 
#> Table (Freq Perc)
#>                      High income   Low income  Lower middle income
#> Afghanistan              0  0.00     61  3.33              0  0.00
#> Albania                  0  0.00      0  0.00              0  0.00
#> Algeria                  0  0.00      0  0.00              0  0.00
#> American Samoa           0  0.00      0  0.00              0  0.00
#> Andorra                 61  1.27      0  0.00              0  0.00
#> Angola                   0  0.00      0  0.00             61  2.13
#> Antigua and Barbuda     61  1.27      0  0.00              0  0.00
#> Argentina                0  0.00      0  0.00              0  0.00
#> Armenia                  0  0.00      0  0.00              0  0.00
#> Aruba                   61  1.27      0  0.00              0  0.00
#> Australia               61  1.27      0  0.00              0  0.00
#> Austria                 61  1.27      0  0.00              0  0.00
#> Azerbaijan               0  0.00      0  0.00              0  0.00
#> Bahamas, The            61  1.27      0  0.00              0  0.00
#>                      Upper middle income        Total
#> Afghanistan                      0  0.00     61  0.46
#> Albania                         61  1.67     61  0.46
#> Algeria                         61  1.67     61  0.46
#> American Samoa                  61  1.67     61  0.46
#> Andorra                          0  0.00     61  0.46
#> Angola                           0  0.00     61  0.46
#> Antigua and Barbuda              0  0.00     61  0.46
#> Argentina                       61  1.67     61  0.46
#> Armenia                         61  1.67     61  0.46
#> Aruba                            0  0.00     61  0.46
#> Australia                        0  0.00     61  0.46
#> Austria                          0  0.00     61  0.46
#> Azerbaijan                      61  1.67     61  0.46
#> Bahamas, The                     0  0.00     61  0.46
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> 
#> Summary of Table Frequencies
#>   High income      Low income     Lower middle income Upper middle income
#>  Min.   : 0.00   Min.   : 0.000   Min.   : 0.00       Min.   : 0.00      
#>  1st Qu.: 0.00   1st Qu.: 0.000   1st Qu.: 0.00       1st Qu.: 0.00      
#>  Median : 0.00   Median : 0.000   Median : 0.00       Median : 0.00      
#>  Mean   :22.31   Mean   : 8.472   Mean   :13.27       Mean   :16.94      
#>  3rd Qu.:61.00   3rd Qu.: 0.000   3rd Qu.: 0.00       3rd Qu.:61.00      
#>  Max.   :61.00   Max.   :61.000   Max.   :61.00       Max.   :61.00      
#>      Total   
#>  Min.   :61  
#>  1st Qu.:61  
#>  Median :61  
#>  Mean   :61  
#>  3rd Qu.:61  
#>  Max.   :61  
#> --------------------------------------------------------------------------------
#> iso3c (factor): Country Code
#> Statistics (N = 13176)
#>                         N   Perc  Ndist
#> High income          4819  36.57     79
#> Low income           1830  13.89     30
#> Lower middle income  2867  21.76     47
#> Upper middle income  3660  27.78     60
#> 
#> Table (Freq Perc)
#>                 High income   Low income  Lower middle income
#> ABW                61  1.27      0  0.00              0  0.00
#> AFG                 0  0.00     61  3.33              0  0.00
#> AGO                 0  0.00      0  0.00             61  2.13
#> ALB                 0  0.00      0  0.00              0  0.00
#> AND                61  1.27      0  0.00              0  0.00
#> ARE                61  1.27      0  0.00              0  0.00
#> ARG                 0  0.00      0  0.00              0  0.00
#> ARM                 0  0.00      0  0.00              0  0.00
#> ASM                 0  0.00      0  0.00              0  0.00
#> ATG                61  1.27      0  0.00              0  0.00
#> AUS                61  1.27      0  0.00              0  0.00
#> AUT                61  1.27      0  0.00              0  0.00
#> AZE                 0  0.00      0  0.00              0  0.00
#> BDI                 0  0.00     61  3.33              0  0.00
#>                 Upper middle income        Total
#> ABW                         0  0.00     61  0.46
#> AFG                         0  0.00     61  0.46
#> AGO                         0  0.00     61  0.46
#> ALB                        61  1.67     61  0.46
#> AND                         0  0.00     61  0.46
#> ARE                         0  0.00     61  0.46
#> ARG                        61  1.67     61  0.46
#> ARM                        61  1.67     61  0.46
#> ASM                        61  1.67     61  0.46
#> ATG                         0  0.00     61  0.46
#> AUS                         0  0.00     61  0.46
#> AUT                         0  0.00     61  0.46
#> AZE                        61  1.67     61  0.46
#> BDI                         0  0.00     61  0.46
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> 
#> Summary of Table Frequencies
#>   High income      Low income     Lower middle income Upper middle income
#>  Min.   : 0.00   Min.   : 0.000   Min.   : 0.00       Min.   : 0.00      
#>  1st Qu.: 0.00   1st Qu.: 0.000   1st Qu.: 0.00       1st Qu.: 0.00      
#>  Median : 0.00   Median : 0.000   Median : 0.00       Median : 0.00      
#>  Mean   :22.31   Mean   : 8.472   Mean   :13.27       Mean   :16.94      
#>  3rd Qu.:61.00   3rd Qu.: 0.000   3rd Qu.: 0.00       3rd Qu.:61.00      
#>  Max.   :61.00   Max.   :61.000   Max.   :61.00       Max.   :61.00      
#>      Total   
#>  Min.   :61  
#>  1st Qu.:61  
#>  Median :61  
#>  Mean   :61  
#>  3rd Qu.:61  
#>  Max.   :61  
#> --------------------------------------------------------------------------------
#> date (Date): Date Recorded (Fictitious)
#> Statistics (N = 13176)
#>                         N   Perc  Ndist         Min         Max
#> High income          4819  36.57     61  1961-01-01  2021-01-01
#> Low income           1830  13.89     61  1961-01-01  2021-01-01
#> Lower middle income  2867  21.76     61  1961-01-01  2021-01-01
#> Upper middle income  3660  27.78     61  1961-01-01  2021-01-01
#> --------------------------------------------------------------------------------
#> year (integer): Year
#> Statistics (N = 13176)
#>                         N   Perc  Ndist  Mean     SD   Min   Max  Skew  Kurt
#> High income          4819  36.57     61  1990  17.61  1960  2020     0   1.8
#> Low income           1830  13.89     61  1990  17.61  1960  2020    -0   1.8
#> Lower middle income  2867  21.76     61  1990  17.61  1960  2020    -0   1.8
#> Upper middle income  3660  27.78     61  1990  17.61  1960  2020     0   1.8
#> 
#> Quantiles
#>                        1%    5%   10%   25%   50%   75%   90%   95%   99%
#> High income          1960  1963  1966  1975  1990  2005  2014  2017  2020
#> Low income           1960  1963  1966  1975  1990  2005  2014  2017  2020
#> Lower middle income  1960  1963  1966  1975  1990  2005  2014  2017  2020
#> Upper middle income  1960  1963  1966  1975  1990  2005  2014  2017  2020
#> --------------------------------------------------------------------------------
#> decade (integer): Decade
#> Statistics (N = 13176)
#>                         N   Perc  Ndist     Mean     SD   Min   Max  Skew  Kurt
#> High income          4819  36.57      7  1985.57  17.51  1960  2020  0.03  1.79
#> Low income           1830  13.89      7  1985.57  17.52  1960  2020  0.03  1.79
#> Lower middle income  2867  21.76      7  1985.57  17.51  1960  2020  0.03  1.79
#> Upper middle income  3660  27.78      7  1985.57  17.51  1960  2020  0.03  1.79
#> 
#> Quantiles
#>                        1%    5%   10%   25%   50%   75%   90%   95%   99%
#> High income          1960  1960  1960  1970  1990  2000  2010  2010  2020
#> Low income           1960  1960  1960  1970  1990  2000  2010  2010  2020
#> Lower middle income  1960  1960  1960  1970  1990  2000  2010  2010  2020
#> Upper middle income  1960  1960  1960  1970  1990  2000  2010  2010  2020
#> --------------------------------------------------------------------------------
#> region (factor): Region
#> Statistics (N = 13176)
#>                         N   Perc  Ndist
#> High income          4819  36.57      6
#> Low income           1830  13.89      5
#> Lower middle income  2867  21.76      6
#> Upper middle income  3660  27.78      6
#> 
#> Table (Freq Perc)
#>                             High income  Low income  Lower middle income
#> Europe & Central Asia         2257 46.8     61  3.3             244  8.5
#> Sub-Saharan Africa              61  1.3   1464 80.0            1037 36.2
#> Latin America & Caribbean     1037 21.5     61  3.3             244  8.5
#> East Asia & Pacific            793 16.5      0  0.0             793 27.7
#> Middle East & North Africa     488 10.1    122  6.7             305 10.6
#> South Asia                       0  0.0    122  6.7             244  8.5
#> North America                  183  3.8      0  0.0               0  0.0
#>                             Upper middle income      Total
#> Europe & Central Asia                  976 26.7  3538 26.9
#> Sub-Saharan Africa                     366 10.0  2928 22.2
#> Latin America & Caribbean             1220 33.3  2562 19.4
#> East Asia & Pacific                    610 16.7  2196 16.7
#> Middle East & North Africa             366 10.0  1281  9.7
#> South Asia                             122  3.3   488  3.7
#> North America                            0  0.0   183  1.4
#> --------------------------------------------------------------------------------
#> OECD (logical): Is OECD Member Country?
#> Statistics (N = 13176)
#>                         N   Perc  Ndist
#> High income          4819  36.57      2
#> Low income           1830  13.89      1
#> Lower middle income  2867  21.76      1
#> Upper middle income  3660  27.78      2
#> 
#> Table (Freq Perc)
#>        High income   Low income  Lower middle income  Upper middle income
#> FALSE   2745  57.0   1830 100.0           2867 100.0           3538  96.7
#> TRUE    2074  43.0      0   0.0              0   0.0            122   3.3
#>              Total
#> FALSE  10980  83.3
#> TRUE    2196  16.7
#> --------------------------------------------------------------------------------
#> PCGDP (numeric): GDP per capita (constant 2010 US$)
#> Statistics (N = 9470, 28.13% NAs)
#>                         N   Perc  Ndist      Mean        SD     Min        Max
#> High income          3179  33.57   3179  30280.73  23847.05  932.04  196061.42
#> Low income           1311  13.84   1311    597.41    288.44  164.34    1864.79
#> Lower middle income  2246  23.72   2246   1574.25    858.72  144.99    4818.19
#> Upper middle income  2734  28.87   2734   4945.33   2979.56  132.08   20532.95
#>                      Skew   Kurt
#> High income          2.17  10.34
#> Low income           1.24   4.71
#> Lower middle income  0.91   3.72
#> Upper middle income  1.23   4.94
#> 
#> Quantiles
#>                           1%       5%      10%       25%       50%       75%
#> High income          3053.83  5395.18  7768.74  14369.61  24745.65  38936.22
#> Low income            191.73   234.77   289.48    396.52    535.96    745.29
#> Lower middle income   194.43   398.88   585.24    961.12   1437.78   1987.89
#> Upper middle income   466.58  1248.19  1835.98   2864.47   4219.97   6452.07
#>                          90%       95%        99%
#> High income            57259   75529.1  116493.28
#> Low income            985.45   1180.37    1513.83
#> Lower middle income  2829.09   3192.75     4191.8
#> Upper middle income  8966.02  10867.95   14416.71
#> --------------------------------------------------------------------------------
#> LIFEEX (numeric): Life expectancy at birth, total (years)
#> Statistics (N = 11670, 11.43% NAs)
#>                         N   Perc  Ndist   Mean    SD    Min    Max   Skew  Kurt
#> High income          3831  32.83   3566  73.62  5.67  42.67  85.42  -1.01  5.56
#> Low income           1800  15.42   1751  49.73  9.09  26.17  74.43   0.27  2.67
#> Lower middle income  2790  23.91   2694  58.15  9.31  18.91   76.7  -0.34  2.68
#> Upper middle income  3249  27.84   3083  66.65  7.54  36.53  80.28   -1.1  4.23
#> 
#> Quantiles
#>                         1%     5%    10%    25%    50%    75%    90%    95%
#> High income          55.62  63.95  67.12   70.5  73.93  77.61  80.67  81.79
#> Low income            31.5  35.78   38.2  43.52  49.04  56.06  61.98  65.87
#> Lower middle income  36.62  42.66  45.73   51.5  58.53  65.79     70  71.72
#> Upper middle income  42.66  51.22  55.93   62.8  68.36  71.95  74.66  75.94
#>                        99%
#> High income          83.22
#> Low income           71.71
#> Lower middle income  74.91
#> Upper middle income  78.42
#> --------------------------------------------------------------------------------
#> GINI (numeric): Gini index (World Bank estimate)
#> Statistics (N = 1744, 86.76% NAs)
#>                        N   Perc  Ndist   Mean    SD   Min   Max  Skew  Kurt
#> High income          680  38.99    213   33.3  6.79  20.7  58.9  1.49  5.68
#> Low income           107   6.14     88  41.13  6.58  29.5  65.8  0.75  4.24
#> Lower middle income  369  21.16    219  40.05   9.3    24  63.2  0.44  2.22
#> Upper middle income  588  33.72    280  43.16  8.95  25.2  64.8  0.08  2.35
#> 
#> Quantiles
#>                         1%     5%    10%    25%    50%   75%    90%    95%
#> High income          23.42   25.2  26.39  28.48  32.35  35.5  41.01  48.72
#> Low income           29.81  32.35  33.26  35.65   41.1  44.9  48.26  51.61
#> Lower middle income  24.77  26.84  28.88   32.7   38.7  46.6  54.52  56.94
#> Upper middle income  26.21  27.87   30.6  36.77  42.45  49.5  54.83  58.56
#>                        99%
#> High income          56.62
#> Low income           60.99
#> Lower middle income  59.82
#> Upper middle income     63
#> --------------------------------------------------------------------------------
#> ODA (numeric): Net official development assistance and official aid received (constant 2018 US$)
#> Statistics (N = 8608, 34.67% NAs)
#>                         N   Perc  Ndist        Mean              SD
#> High income          1575   18.3   1407  153'663194      425'918409
#> Low income           1692  19.66   1678  631'660165      941'498380
#> Lower middle income  2544  29.55   2503  692'072692  1.02452490e+09
#> Upper middle income  2797  32.49   2700  301'326218      765'116131
#>                              Min             Max   Skew    Kurt
#> High income          -464'709991  4.34612988e+09   5.25   36.27
#> Low income               -500000  1.04032100e+10   4.46   32.13
#> Lower middle income  -605'969971  1.18790801e+10   3.79   25.24
#> Upper middle income  -997'679993  2.56715605e+10  16.31  464.86
#> 
#> Quantiles
#>                                1%           5%          10%         25%
#> High income          -54'802401.1   -755999.99       264000  4'400000.1
#> Low income            1'100000.02  33'997999.8  71'296000.7  151'814999
#> Lower middle income     209999.99  14'721500.3  41'358000.2  100'485003
#> Upper middle income  -73'793201.9  4'558000.18    12'666000   38'000000
#>                              50%         75%             90%             95%
#> High income          21'209999.1  104'934998      375'347992      661'426996
#> Low income            332'904999  692'777496  1.47914895e+09  2.14049348e+09
#> Lower middle income   336'494995  810'707520  1.84614302e+09  2.59226945e+09
#> Upper middle income   105'139999  311'519989      714'823975  1.18504797e+09
#>                                 99%
#> High income          2.31632209e+09
#> Low income           4.82899863e+09
#> Lower middle income  4.69573516e+09
#> Upper middle income  2.98750435e+09
#> --------------------------------------------------------------------------------
#> POP (numeric): Population, total
#> Statistics (N = 12919, 1.95% NAs)
#>                         N   Perc  Ndist         Mean           SD     Min
#> High income          4737  36.67   4712  12'421540.4  34'160829.5    2833
#> Low income           1792  13.87   1791  11'690380.2  13'942313.8  365047
#> Lower middle income  2790   21.6   2790  40'802037.5   137'302296   41202
#> Upper middle income  3600  27.87   3596  33'223895.5   143'647992    4375
#>                                 Max  Skew   Kurt
#> High income              328'239523   5.5  39.75
#> Low income               112'078730  3.22  16.57
#> Lower middle income  1.36641775e+09   6.7   52.4
#> Upper middle income  1.39771500e+09  7.53  61.78
#> 
#> Quantiles
#>                             1%           5%         10%          25%
#> High income            7467.08      18432.4       30517        84449
#> Low income           594755.89  1'206251.55  1'919035.6  3'842838.75
#> Lower middle income   58452.97    105620.75    224651.9     1'188469
#> Upper middle income    7357.28     47202.15     93885.2    609166.25
#>                             50%          75%          90%          95%
#> High income            1'632114     8'336605  37'508393.4  58'933084.2
#> Low income             7'181772  13'579920.8  25'964845.3  36'596246.7
#> Lower middle income  5'914923.5  25'966431.8    81'157844   146'949299
#> Upper middle income    3'763490  16'347212.5  47'556659.8   104'771148
#>                                 99%
#> High income              209'091400
#> Low income              76'539171.5
#> Lower middle income      893'256928
#> Upper middle income  1.02344515e+09
#> --------------------------------------------------------------------------------
print(descr(wlddev, ~ income), compact = TRUE)
#> Dataset: wlddev, 12 Variables, N = 13176
#> Grouped by: income [4]
#>                         N   Perc
#> High income          4819  36.57
#> Low income           1830  13.89
#> Lower middle income  2867  21.76
#> Upper middle income  3660  27.78
#> --------------------------------------------------------------------------------
#> country (character): Country Name
#> Statistics (N = 13176)
#>                         N   Perc  Ndist
#> High income          4819  36.57     79
#> Low income           1830  13.89     30
#> Lower middle income  2867  21.76     47
#> Upper middle income  3660  27.78     60
#> 
#> Table (Freq Perc)
#>                      High income   Low income  Lower middle income
#> Afghanistan              0  0.00     61  3.33              0  0.00
#> Albania                  0  0.00      0  0.00              0  0.00
#> Algeria                  0  0.00      0  0.00              0  0.00
#> American Samoa           0  0.00      0  0.00              0  0.00
#> Andorra                 61  1.27      0  0.00              0  0.00
#> Angola                   0  0.00      0  0.00             61  2.13
#> Antigua and Barbuda     61  1.27      0  0.00              0  0.00
#> Argentina                0  0.00      0  0.00              0  0.00
#> Armenia                  0  0.00      0  0.00              0  0.00
#> Aruba                   61  1.27      0  0.00              0  0.00
#> Australia               61  1.27      0  0.00              0  0.00
#> Austria                 61  1.27      0  0.00              0  0.00
#> Azerbaijan               0  0.00      0  0.00              0  0.00
#> Bahamas, The            61  1.27      0  0.00              0  0.00
#>                      Upper middle income        Total
#> Afghanistan                      0  0.00     61  0.46
#> Albania                         61  1.67     61  0.46
#> Algeria                         61  1.67     61  0.46
#> American Samoa                  61  1.67     61  0.46
#> Andorra                          0  0.00     61  0.46
#> Angola                           0  0.00     61  0.46
#> Antigua and Barbuda              0  0.00     61  0.46
#> Argentina                       61  1.67     61  0.46
#> Armenia                         61  1.67     61  0.46
#> Aruba                            0  0.00     61  0.46
#> Australia                        0  0.00     61  0.46
#> Austria                          0  0.00     61  0.46
#> Azerbaijan                      61  1.67     61  0.46
#> Bahamas, The                     0  0.00     61  0.46
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> --------------------------------------------------------------------------------
#> iso3c (factor): Country Code
#> Statistics (N = 13176)
#>                         N   Perc  Ndist
#> High income          4819  36.57     79
#> Low income           1830  13.89     30
#> Lower middle income  2867  21.76     47
#> Upper middle income  3660  27.78     60
#> 
#> Table (Freq Perc)
#>                 High income   Low income  Lower middle income
#> ABW                61  1.27      0  0.00              0  0.00
#> AFG                 0  0.00     61  3.33              0  0.00
#> AGO                 0  0.00      0  0.00             61  2.13
#> ALB                 0  0.00      0  0.00              0  0.00
#> AND                61  1.27      0  0.00              0  0.00
#> ARE                61  1.27      0  0.00              0  0.00
#> ARG                 0  0.00      0  0.00              0  0.00
#> ARM                 0  0.00      0  0.00              0  0.00
#> ASM                 0  0.00      0  0.00              0  0.00
#> ATG                61  1.27      0  0.00              0  0.00
#> AUS                61  1.27      0  0.00              0  0.00
#> AUT                61  1.27      0  0.00              0  0.00
#> AZE                 0  0.00      0  0.00              0  0.00
#> BDI                 0  0.00     61  3.33              0  0.00
#>                 Upper middle income        Total
#> ABW                         0  0.00     61  0.46
#> AFG                         0  0.00     61  0.46
#> AGO                         0  0.00     61  0.46
#> ALB                        61  1.67     61  0.46
#> AND                         0  0.00     61  0.46
#> ARE                         0  0.00     61  0.46
#> ARG                        61  1.67     61  0.46
#> ARM                        61  1.67     61  0.46
#> ASM                        61  1.67     61  0.46
#> ATG                         0  0.00     61  0.46
#> AUS                         0  0.00     61  0.46
#> AUT                         0  0.00     61  0.46
#> AZE                        61  1.67     61  0.46
#> BDI                         0  0.00     61  0.46
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> --------------------------------------------------------------------------------
#> date (Date): Date Recorded (Fictitious)
#> Statistics (N = 13176)
#>                         N   Perc  Ndist         Min         Max
#> High income          4819  36.57     61  1961-01-01  2021-01-01
#> Low income           1830  13.89     61  1961-01-01  2021-01-01
#> Lower middle income  2867  21.76     61  1961-01-01  2021-01-01
#> Upper middle income  3660  27.78     61  1961-01-01  2021-01-01
#> --------------------------------------------------------------------------------
#> year (integer): Year
#> Statistics (N = 13176)
#>                         N   Perc  Ndist  Mean     SD   Min   Max  Skew  Kurt
#> High income          4819  36.57     61  1990  17.61  1960  2020     0   1.8
#> Low income           1830  13.89     61  1990  17.61  1960  2020    -0   1.8
#> Lower middle income  2867  21.76     61  1990  17.61  1960  2020    -0   1.8
#>                        1%    5%   10%   25%   50%   75%   90%   95%   99%
#> High income          1960  1963  1966  1975  1990  2005  2014  2017  2020
#> Low income           1960  1963  1966  1975  1990  2005  2014  2017  2020
#> Lower middle income  1960  1963  1966  1975  1990  2005  2014  2017  2020
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> --------------------------------------------------------------------------------
#> decade (integer): Decade
#> Statistics (N = 13176)
#>                         N   Perc  Ndist     Mean     SD   Min   Max  Skew  Kurt
#> High income          4819  36.57      7  1985.57  17.51  1960  2020  0.03  1.79
#> Low income           1830  13.89      7  1985.57  17.52  1960  2020  0.03  1.79
#> Lower middle income  2867  21.76      7  1985.57  17.51  1960  2020  0.03  1.79
#>                        1%    5%   10%   25%   50%   75%   90%   95%   99%
#> High income          1960  1960  1960  1970  1990  2000  2010  2010  2020
#> Low income           1960  1960  1960  1970  1990  2000  2010  2010  2020
#> Lower middle income  1960  1960  1960  1970  1990  2000  2010  2010  2020
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> --------------------------------------------------------------------------------
#> region (factor): Region
#> Statistics (N = 13176)
#>                         N   Perc  Ndist
#> High income          4819  36.57      6
#> Low income           1830  13.89      5
#> Lower middle income  2867  21.76      6
#> Upper middle income  3660  27.78      6
#> 
#> Table (Freq Perc)
#>                             High income  Low income  Lower middle income
#> Europe & Central Asia         2257 46.8     61  3.3             244  8.5
#> Sub-Saharan Africa              61  1.3   1464 80.0            1037 36.2
#> Latin America & Caribbean     1037 21.5     61  3.3             244  8.5
#> East Asia & Pacific            793 16.5      0  0.0             793 27.7
#> Middle East & North Africa     488 10.1    122  6.7             305 10.6
#> South Asia                       0  0.0    122  6.7             244  8.5
#> North America                  183  3.8      0  0.0               0  0.0
#>                             Upper middle income      Total
#> Europe & Central Asia                  976 26.7  3538 26.9
#> Sub-Saharan Africa                     366 10.0  2928 22.2
#> Latin America & Caribbean             1220 33.3  2562 19.4
#> East Asia & Pacific                    610 16.7  2196 16.7
#> Middle East & North Africa             366 10.0  1281  9.7
#> South Asia                             122  3.3   488  3.7
#> North America                            0  0.0   183  1.4
#> --------------------------------------------------------------------------------
#> OECD (logical): Is OECD Member Country?
#> Statistics (N = 13176)
#>                         N   Perc  Ndist
#> High income          4819  36.57      2
#> Low income           1830  13.89      1
#> Lower middle income  2867  21.76      1
#> Upper middle income  3660  27.78      2
#> 
#> Table (Freq Perc)
#>        High income   Low income  Lower middle income  Upper middle income
#> FALSE   2745  57.0   1830 100.0           2867 100.0           3538  96.7
#> TRUE    2074  43.0      0   0.0              0   0.0            122   3.3
#>              Total
#> FALSE  10980  83.3
#> TRUE    2196  16.7
#> --------------------------------------------------------------------------------
#> PCGDP (numeric): GDP per capita (constant 2010 US$)
#> Statistics (N = 9470, 28.13% NAs)
#>                         N   Perc  Ndist      Mean        SD     Min        Max
#> High income          3179  33.57   3179  30280.73  23847.05  932.04  196061.42
#> Low income           1311  13.84   1311    597.41    288.44  164.34    1864.79
#> Lower middle income  2246  23.72   2246   1574.25    858.72  144.99    4818.19
#>                      Skew   Kurt       1%       5%      10%       25%       50%
#> High income          2.17  10.34  3053.83  5395.18  7768.74  14369.61  24745.65
#> Low income           1.24   4.71   191.73   234.77   289.48    396.52    535.96
#> Lower middle income  0.91   3.72   194.43   398.88   585.24    961.12   1437.78
#>                           75%      90%       95%        99%
#> High income          38936.22    57259   75529.1  116493.28
#> Low income             745.29   985.45   1180.37    1513.83
#> Lower middle income   1987.89  2829.09   3192.75     4191.8
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> --------------------------------------------------------------------------------
#> LIFEEX (numeric): Life expectancy at birth, total (years)
#> Statistics (N = 11670, 11.43% NAs)
#>                         N   Perc  Ndist   Mean    SD    Min    Max   Skew  Kurt
#> High income          3831  32.83   3566  73.62  5.67  42.67  85.42  -1.01  5.56
#> Low income           1800  15.42   1751  49.73  9.09  26.17  74.43   0.27  2.67
#> Lower middle income  2790  23.91   2694  58.15  9.31  18.91   76.7  -0.34  2.68
#>                         1%     5%    10%    25%    50%    75%    90%    95%
#> High income          55.62  63.95  67.12   70.5  73.93  77.61  80.67  81.79
#> Low income            31.5  35.78   38.2  43.52  49.04  56.06  61.98  65.87
#> Lower middle income  36.62  42.66  45.73   51.5  58.53  65.79     70  71.72
#>                        99%
#> High income          83.22
#> Low income           71.71
#> Lower middle income  74.91
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> --------------------------------------------------------------------------------
#> GINI (numeric): Gini index (World Bank estimate)
#> Statistics (N = 1744, 86.76% NAs)
#>                        N   Perc  Ndist   Mean    SD   Min   Max  Skew  Kurt
#> High income          680  38.99    213   33.3  6.79  20.7  58.9  1.49  5.68
#> Low income           107   6.14     88  41.13  6.58  29.5  65.8  0.75  4.24
#> Lower middle income  369  21.16    219  40.05   9.3    24  63.2  0.44  2.22
#>                         1%     5%    10%    25%    50%   75%    90%    95%
#> High income          23.42   25.2  26.39  28.48  32.35  35.5  41.01  48.72
#> Low income           29.81  32.35  33.26  35.65   41.1  44.9  48.26  51.61
#> Lower middle income  24.77  26.84  28.88   32.7   38.7  46.6  54.52  56.94
#>                        99%
#> High income          56.62
#> Low income           60.99
#> Lower middle income  59.82
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> --------------------------------------------------------------------------------
#> ODA (numeric): Net official development assistance and official aid received (constant 2018 US$)
#> Statistics (N = 8608, 34.67% NAs)
#>                         N   Perc  Ndist        Mean              SD
#> High income          1575   18.3   1407  153'663194      425'918409
#> Low income           1692  19.66   1678  631'660165      941'498380
#> Lower middle income  2544  29.55   2503  692'072692  1.02452490e+09
#>                              Min             Max   Skew    Kurt            1%
#> High income          -464'709991  4.34612988e+09   5.25   36.27  -54'802401.1
#> Low income               -500000  1.04032100e+10   4.46   32.13   1'100000.02
#> Lower middle income  -605'969971  1.18790801e+10   3.79   25.24     209999.99
#>                               5%          10%         25%          50%
#> High income           -755999.99       264000  4'400000.1  21'209999.1
#> Low income           33'997999.8  71'296000.7  151'814999   332'904999
#> Lower middle income  14'721500.3  41'358000.2  100'485003   336'494995
#>                             75%             90%             95%             99%
#> High income          104'934998      375'347992      661'426996  2.31632209e+09
#> Low income           692'777496  1.47914895e+09  2.14049348e+09  4.82899863e+09
#> Lower middle income  810'707520  1.84614302e+09  2.59226945e+09  4.69573516e+09
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> --------------------------------------------------------------------------------
#> POP (numeric): Population, total
#> Statistics (N = 12919, 1.95% NAs)
#>                         N   Perc  Ndist         Mean           SD     Min
#> High income          4737  36.67   4712  12'421540.4  34'160829.5    2833
#> Low income           1792  13.87   1791  11'690380.2  13'942313.8  365047
#> Lower middle income  2790   21.6   2790  40'802037.5   137'302296   41202
#>                                 Max  Skew   Kurt         1%           5%
#> High income              328'239523   5.5  39.75    7467.08      18432.4
#> Low income               112'078730  3.22  16.57  594755.89  1'206251.55
#> Lower middle income  1.36641775e+09   6.7   52.4   58452.97    105620.75
#>                             10%          25%         50%          75%
#> High income               30517        84449    1'632114     8'336605
#> Low income           1'919035.6  3'842838.75    7'181772  13'579920.8
#> Lower middle income    224651.9     1'188469  5'914923.5  25'966431.8
#>                              90%          95%             99%
#> High income          37'508393.4  58'933084.2      209'091400
#> Low income           25'964845.3  36'596246.7     76'539171.5
#> Lower middle income    81'157844   146'949299      893'256928
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> --------------------------------------------------------------------------------

## Grouped & Weighted Desciptions
descr(wlddev, ~ income, w = ~ replace_na(POP))
#> Dataset: wlddev, 11 Variables, N = 13176, WeightSum = 313233706778
#> Grouped by: income [4]
#>                         N   Perc       WeightSum   Perc
#> High income          4819  36.57  5.88408371e+10  18.78
#> Low income           1830  13.89  2.09491614e+10   6.69
#> Lower middle income  2867  21.76  1.13837685e+11  36.34
#> Upper middle income  3660  27.78  1.19606024e+11  38.18
#> --------------------------------------------------------------------------------
#> country (character): Country Name
#> Statistics (WeightSum = 313233706778)
#>                           WeightSum   Perc  Ndist
#> High income          5.88408371e+10  18.78     79
#> Low income           2.09491614e+10   6.69     30
#> Lower middle income  1.13837685e+11  36.34     47
#> Upper middle income  1.19606024e+11  38.18     60
#> 
#> Table (WeightSum Perc)
#>                       High income     Low income  Lower middle income
#> China               0.0e+00   0.0  0.0e+00   0.0        0.0e+00   0.0
#> India               0.0e+00   0.0  0.0e+00   0.0        5.3e+10  46.4
#> United States       1.5e+10  25.9  0.0e+00   0.0        0.0e+00   0.0
#> Indonesia           0.0e+00   0.0  0.0e+00   0.0        1.1e+10   9.4
#> Brazil              0.0e+00   0.0  0.0e+00   0.0        0.0e+00   0.0
#> Russian Federation  0.0e+00   0.0  0.0e+00   0.0        0.0e+00   0.0
#> Japan               7.1e+09  12.0  0.0e+00   0.0        0.0e+00   0.0
#> Pakistan            0.0e+00   0.0  0.0e+00   0.0        6.9e+09   6.0
#> Bangladesh          0.0e+00   0.0  0.0e+00   0.0        6.2e+09   5.5
#> Nigeria             0.0e+00   0.0  0.0e+00   0.0        6.2e+09   5.4
#> Mexico              0.0e+00   0.0  0.0e+00   0.0        0.0e+00   0.0
#> Germany             4.8e+09   8.1  0.0e+00   0.0        0.0e+00   0.0
#> Vietnam             0.0e+00   0.0  0.0e+00   0.0        4.0e+09   3.5
#> Philippines         0.0e+00   0.0  0.0e+00   0.0        3.8e+09   3.3
#>                     Upper middle income          Total
#> China                     6.5e+10  54.6  6.5e+10  20.8
#> India                     0.0e+00   0.0  5.3e+10  16.9
#> United States             0.0e+00   0.0  1.5e+10   4.9
#> Indonesia                 0.0e+00   0.0  1.1e+10   3.4
#> Brazil                    8.7e+09   7.3  8.7e+09   2.8
#> Russian Federation        8.4e+09   7.0  8.4e+09   2.7
#> Japan                     0.0e+00   0.0  7.1e+09   2.3
#> Pakistan                  0.0e+00   0.0  6.9e+09   2.2
#> Bangladesh                0.0e+00   0.0  6.2e+09   2.0
#> Nigeria                   0.0e+00   0.0  6.2e+09   2.0
#> Mexico                    4.9e+09   4.1  4.9e+09   1.6
#> Germany                   0.0e+00   0.0  4.8e+09   1.5
#> Vietnam                   0.0e+00   0.0  4.0e+09   1.3
#> Philippines               0.0e+00   0.0  3.8e+09   1.2
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> 
#> Summary of Table WeightSums
#>   High income          Low income        Lower middle income
#>  Min.   :0.000e+00   Min.   :0.000e+00   Min.   :0.000e+00  
#>  1st Qu.:0.000e+00   1st Qu.:0.000e+00   1st Qu.:0.000e+00  
#>  Median :0.000e+00   Median :0.000e+00   Median :0.000e+00  
#>  Mean   :2.724e+08   Mean   :9.699e+07   Mean   :5.270e+08  
#>  3rd Qu.:1.083e+07   3rd Qu.:0.000e+00   3rd Qu.:0.000e+00  
#>  Max.   :1.523e+10   Max.   :3.280e+09   Max.   :5.284e+10  
#>  Upper middle income     Total          
#>  Min.   :0.000e+00   Min.   :5.006e+05  
#>  1st Qu.:0.000e+00   1st Qu.:3.067e+07  
#>  Median :0.000e+00   Median :2.553e+08  
#>  Mean   :5.537e+08   Mean   :1.450e+09  
#>  3rd Qu.:5.643e+06   3rd Qu.:8.257e+08  
#>  Max.   :6.527e+10   Max.   :6.527e+10  
#> --------------------------------------------------------------------------------
#> iso3c (factor): Country Code
#> Statistics (WeightSum = 313233706778)
#>                           WeightSum   Perc  Ndist
#> High income          5.88408371e+10  18.78     79
#> Low income           2.09491614e+10   6.69     30
#> Lower middle income  1.13837685e+11  36.34     47
#> Upper middle income  1.19606024e+11  38.18     60
#> 
#> Table (WeightSum Perc)
#>                   High income     Low income  Lower middle income
#> CHN             0.0e+00   0.0  0.0e+00   0.0        0.0e+00   0.0
#> IND             0.0e+00   0.0  0.0e+00   0.0        5.3e+10  46.4
#> USA             1.5e+10  25.9  0.0e+00   0.0        0.0e+00   0.0
#> IDN             0.0e+00   0.0  0.0e+00   0.0        1.1e+10   9.4
#> BRA             0.0e+00   0.0  0.0e+00   0.0        0.0e+00   0.0
#> RUS             0.0e+00   0.0  0.0e+00   0.0        0.0e+00   0.0
#> JPN             7.1e+09  12.0  0.0e+00   0.0        0.0e+00   0.0
#> PAK             0.0e+00   0.0  0.0e+00   0.0        6.9e+09   6.0
#> BGD             0.0e+00   0.0  0.0e+00   0.0        6.2e+09   5.5
#> NGA             0.0e+00   0.0  0.0e+00   0.0        6.2e+09   5.4
#> MEX             0.0e+00   0.0  0.0e+00   0.0        0.0e+00   0.0
#> DEU             4.8e+09   8.1  0.0e+00   0.0        0.0e+00   0.0
#> VNM             0.0e+00   0.0  0.0e+00   0.0        4.0e+09   3.5
#> PHL             0.0e+00   0.0  0.0e+00   0.0        3.8e+09   3.3
#>                 Upper middle income          Total
#> CHN                   6.5e+10  54.6  6.5e+10  20.8
#> IND                   0.0e+00   0.0  5.3e+10  16.9
#> USA                   0.0e+00   0.0  1.5e+10   4.9
#> IDN                   0.0e+00   0.0  1.1e+10   3.4
#> BRA                   8.7e+09   7.3  8.7e+09   2.8
#> RUS                   8.4e+09   7.0  8.4e+09   2.7
#> JPN                   0.0e+00   0.0  7.1e+09   2.3
#> PAK                   0.0e+00   0.0  6.9e+09   2.2
#> BGD                   0.0e+00   0.0  6.2e+09   2.0
#> NGA                   0.0e+00   0.0  6.2e+09   2.0
#> MEX                   4.9e+09   4.1  4.9e+09   1.6
#> DEU                   0.0e+00   0.0  4.8e+09   1.5
#> VNM                   0.0e+00   0.0  4.0e+09   1.3
#> PHL                   0.0e+00   0.0  3.8e+09   1.2
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
#> 
#> Summary of Table WeightSums
#>   High income          Low income        Lower middle income
#>  Min.   :0.000e+00   Min.   :0.000e+00   Min.   :0.000e+00  
#>  1st Qu.:0.000e+00   1st Qu.:0.000e+00   1st Qu.:0.000e+00  
#>  Median :0.000e+00   Median :0.000e+00   Median :0.000e+00  
#>  Mean   :2.724e+08   Mean   :9.699e+07   Mean   :5.270e+08  
#>  3rd Qu.:1.083e+07   3rd Qu.:0.000e+00   3rd Qu.:0.000e+00  
#>  Max.   :1.523e+10   Max.   :3.280e+09   Max.   :5.284e+10  
#>  Upper middle income     Total          
#>  Min.   :0.000e+00   Min.   :5.006e+05  
#>  1st Qu.:0.000e+00   1st Qu.:3.067e+07  
#>  Median :0.000e+00   Median :2.553e+08  
#>  Mean   :5.537e+08   Mean   :1.450e+09  
#>  3rd Qu.:5.643e+06   3rd Qu.:8.257e+08  
#>  Max.   :6.527e+10   Max.   :6.527e+10  
#> --------------------------------------------------------------------------------
#> date (Date): Date Recorded (Fictitious)
#> Statistics (N = 13176)
#>                         N   Perc  Ndist         Min         Max
#> High income          4819  36.57     61  1961-01-01  2021-01-01
#> Low income           1830  13.89     61  1961-01-01  2021-01-01
#> Lower middle income  2867  21.76     61  1961-01-01  2021-01-01
#> Upper middle income  3660  27.78     61  1961-01-01  2021-01-01
#> --------------------------------------------------------------------------------
#> year (integer): Year
#> Statistics (N = 12919, 1.95% NAs)
#>                         N  Ndist       WeightSum   Perc     Mean     SD   Min
#> High income          4737     61  5.88408371e+10  18.78  1991.75  17.15  1960
#> Low income           1792     61  2.09491614e+10   6.69  1997.27  16.31  1960
#> Lower middle income  2790     61  1.13837685e+11  36.34  1995.41  16.47  1960
#> Upper middle income  3600     61  1.19606024e+11  38.18  1993.44  16.69  1960
#>                       Max   Skew  Kurt
#> High income          2019  -0.15  1.84
#> Low income           2019  -0.56  2.23
#> Lower middle income  2019  -0.42  2.08
#> Upper middle income  2019  -0.27  1.95
#> 
#> Quantiles
#>                        1%    5%   10%   25%   50%   75%   90%   95%   99%
#> High income          1960  1963  1967  1977  1993  2007  2015  2017  2019
#> Low income           1961  1966  1972  1985  2001  2011  2016  2018  2019
#> Lower middle income  1961  1965  1970  1983  1998  2010  2016  2018  2019
#> Upper middle income  1961  1964  1969  1980  1995  2008  2015  2017  2019
#> --------------------------------------------------------------------------------
#> decade (integer): Decade
#> Statistics (N = 12919, 1.95% NAs)
#>                         N  Ndist       WeightSum   Perc     Mean     SD   Min
#> High income          4737      7  5.88408371e+10  18.78  1987.19  16.92  1960
#> Low income           1792      7  2.09491614e+10   6.69  1992.55  16.05  1960
#> Lower middle income  2790      7  1.13837685e+11  36.34  1990.76  16.25  1960
#> Upper middle income  3600      7  1.19606024e+11  38.18  1988.84  16.48  1960
#>                       Max   Skew  Kurt
#> High income          2010  -0.16  1.77
#> Low income           2010  -0.59  2.18
#> Lower middle income  2010  -0.44  2.02
#> Upper middle income  2010  -0.28  1.88
#> 
#> Quantiles
#>                        1%    5%   10%   25%   50%   75%   90%   95%   99%
#> High income          1960  1960  1960  1970  1990  2000  2010  2010  2010
#> Low income           1960  1960  1970  1980  2000  2010  2010  2010  2010
#> Lower middle income  1960  1960  1970  1980  1990  2010  2010  2010  2010
#> Upper middle income  1960  1960  1960  1980  1990  2000  2010  2010  2010
#> --------------------------------------------------------------------------------
#> region (factor): Region
#> Statistics (WeightSum = 313233706778)
#>                           WeightSum   Perc  Ndist
#> High income          5.88408371e+10  18.78      6
#> Low income           2.09491614e+10   6.69      5
#> Lower middle income  1.13837685e+11  36.34      6
#> Upper middle income  1.19606024e+11  38.18      6
#> 
#> Table (WeightSum Perc)
#>                                 High income       Low income
#> East Asia & Pacific         1.1e+10 19.3876  0.0e+00  0.0000
#> South Asia                  0.0e+00  0.0000  2.3e+09 10.7511
#> Europe & Central Asia       2.7e+10 46.3714  3.1e+08  1.4869
#> Sub-Saharan Africa          4.2e+06  0.0072  1.6e+10 78.2113
#> Latin America & Caribbean   1.5e+09  2.4920  4.3e+08  2.0514
#> North America               1.7e+10 28.6894  0.0e+00  0.0000
#> Middle East & North Africa  1.8e+09  3.0525  1.6e+09  7.4994
#>                             Lower middle income  Upper middle income
#> East Asia & Pacific             2.2e+10 19.4793      7.0e+10 58.2244
#> South Asia                      6.6e+10 57.9315      1.0e+09  0.8415
#> Europe & Central Asia           4.5e+09  3.9634      1.7e+10 14.1903
#> Sub-Saharan Africa              1.4e+10 12.6496      2.5e+09  2.1066
#> Latin America & Caribbean       1.3e+09  1.1339      2.3e+10 19.1872
#> North America                   0.0e+00  0.0000      0.0e+00  0.0000
#> Middle East & North Africa      5.5e+09  4.8424      6.5e+09  5.4500
#>                                       Total
#> East Asia & Pacific         1.0e+11 32.9538
#> South Asia                  6.9e+10 22.0942
#> Europe & Central Asia       4.9e+10 15.6692
#> Sub-Saharan Africa          3.3e+10 10.6337
#> Latin America & Caribbean   2.6e+10  8.3439
#> North America               1.7e+10  5.3893
#> Middle East & North Africa  1.5e+10  4.9159
#> --------------------------------------------------------------------------------
#> OECD (logical): Is OECD Member Country?
#> Statistics (WeightSum = 313233706778)
#>                           WeightSum   Perc  Ndist
#> High income          5.88408371e+10  18.78      2
#> Low income           2.09491614e+10   6.69      1
#> Lower middle income  1.13837685e+11  36.34      1
#> Upper middle income  1.19606024e+11  38.18      2
#> 
#> Table (WeightSum Perc)
#>          High income     Low income  Lower middle income  Upper middle income
#> FALSE  3.1e+09   5.3  2.1e+10 100.0        1.1e+11 100.0        1.1e+11  93.2
#> TRUE   5.6e+10  94.7  0.0e+00   0.0        0.0e+00   0.0        8.2e+09   6.8
#>                Total
#> FALSE  2.5e+11  79.6
#> TRUE   6.4e+10  20.4
#> --------------------------------------------------------------------------------
#> PCGDP (numeric): GDP per capita (constant 2010 US$)
#> Statistics (N = 9470, 28.13% NAs)
#>                         N  Ndist       WeightSum   Perc      Mean       SD
#> High income          3179   3179  5.55288564e+10  18.79  31284.74  13807.6
#> Low income           1311   1311  1.69031453e+10   5.72    557.14   279.41
#> Lower middle income  2246   2246  1.10267107e+11  37.32   1238.83   823.89
#> Upper middle income  2734   2734  1.12746722e+11  38.16   4145.68  3515.97
#>                         Min        Max  Skew  Kurt
#> High income          932.04  196061.42   0.2  3.21
#> Low income           164.34    1864.79  1.07  4.08
#> Lower middle income  144.99    4818.19  1.25  4.57
#> Upper middle income  132.08   20532.95  0.66  2.52
#> 
#> Quantiles
#>                           1%      5%      10%       25%       50%       75%
#> High income          3268.18  8837.9  13369.8  20951.55  31079.21  41791.15
#> Low income            179.91     205   234.62    356.02    495.11    710.27
#> Lower middle income   236.75  365.72   396.21    580.69   1039.84   1665.26
#> Upper middle income    141.8  196.78   260.78    781.64   3543.37   6803.24
#>                           90%       95%       99%
#> High income          48866.19  52164.71   60264.2
#> Low income              945.3   1128.55   1396.37
#> Lower middle income   2337.98   2885.88   3973.91
#> Upper middle income   9014.51  10737.23  12916.96
#> --------------------------------------------------------------------------------
#> LIFEEX (numeric): Life expectancy at birth, total (years)
#> Statistics (N = 11659, 11.51% NAs)
#>                         N  Ndist       WeightSum   Perc   Mean    SD    Min
#> High income          3828   3566  5.87959699e+10  18.79  75.69  4.53  42.67
#> Low income           1792   1751  2.09491614e+10    6.7  53.51  8.87  26.17
#> Lower middle income  2790   2694  1.13837685e+11  36.38  60.59  8.36  18.91
#> Upper middle income  3249   3083  1.19295269e+11  38.13  68.27  7.19  36.53
#>                        Max   Skew  Kurt
#> High income          85.42  -0.73  4.94
#> Low income           74.43  -0.01  2.47
#> Lower middle income   76.7  -0.56  2.52
#> Upper middle income  80.28  -1.42  4.95
#> 
#> Quantiles
#>                         1%     5%    10%    25%    50%    75%    90%    95%
#> High income          62.63  69.38  70.21   72.5  76.03  78.74  81.44  82.57
#> Low income           33.82  39.38  42.65  46.95     53  60.36  65.27   67.1
#> Lower middle income  41.43  45.37  47.75  54.69   62.3  67.54  69.77  71.27
#> Upper middle income  44.11  51.87   58.2  65.86   69.5  73.51  75.62  76.45
#>                        99%
#> High income          83.79
#> Low income           72.68
#> Lower middle income  74.91
#> Upper middle income  76.91
#> --------------------------------------------------------------------------------
#> GINI (numeric): Gini index (World Bank estimate)
#> Statistics (N = 1744, 86.76% NAs)
#>                        N  Ndist       WeightSum   Perc   Mean    SD   Min   Max
#> High income          680    213  2.07396836e+10  25.11  36.03  4.93  20.7  58.9
#> Low income           107     88  1.90256783e+09    2.3  39.76  5.99  29.5  65.8
#> Lower middle income  369    219  2.16883977e+10  26.26  35.16  5.52    24  63.2
#> Upper middle income  588    280  3.82704279e+10  46.33  43.88  7.52  25.2  64.8
#>                      Skew  Kurt
#> High income          0.18  3.61
#> Low income           0.58  4.23
#> Lower middle income  1.44  6.24
#> Upper middle income  0.68  2.86
#> 
#> Quantiles
#>                         1%     5%    10%    25%    50%    75%    90%    95%
#> High income          25.38  28.18  29.87   32.3  35.35  40.47   41.1   41.4
#> Low income           29.83  30.14  32.84     35  40.28  43.55  46.13   48.5
#> Lower middle income  24.95  28.63   29.8  31.77   34.4     37   41.6   46.5
#> Upper middle income  28.35   34.4   36.6   38.7     42   48.7   55.6  58.93
#>                        99%
#> High income          47.26
#> Low income           53.98
#> Lower middle income   55.5
#> Upper middle income  61.32
#> --------------------------------------------------------------------------------
#> ODA (numeric): Net official development assistance and official aid received (constant 2018 US$)
#> Statistics (N = 8597, 34.75% NAs)
#>                         N  Ndist       WeightSum   Perc            Mean
#> High income          1572   1407  5.60343429e+09   2.42      469'519277
#> Low income           1684   1678  2.05403995e+10   8.87  1.27976069e+09
#> Lower middle income  2544   2503  1.11067918e+11  47.99  2.23495473e+09
#> Upper middle income  2797   2700  9.42398516e+10  40.72  1.02122309e+09
#>                                  SD          Min             Max  Skew   Kurt
#> High income              823'186883  -464'709991  4.34612988e+09  2.18    8.3
#> Low income           1.36723776e+09      -500000  1.04032100e+10  2.17   9.67
#> Lower middle income  1.71188944e+09  -605'969971  1.18790801e+10  1.48   6.42
#> Upper middle income  1.31969536e+09  -997'679993  2.56715605e+10  2.58  38.51
#> 
#> Quantiles
#>                                1%           5%          10%          25%
#> High income           -211'221010  -111'039383   -51'563267  12'609999.7
#> Low income            18'581027.3   102'699417   170'565775   340'775627
#> Lower middle income  -1'186156.61   169'613235   351'778004   955'472807
#> Upper middle income   -951'313387  -674'066685  -464'822389   127'570913
#>                                 50%             75%             90%
#> High income             73'609429.8      636'421870  1.67398341e+09
#> Low income               792'634263  1.73644025e+09  3.15899287e+09
#> Lower middle income  2.02836117e+09  2.90852841e+09  4.33204816e+09
#> Upper middle income      653'751768  1.89074430e+09  2.85101616e+09
#>                                 95%             99%
#> High income          1.97758747e+09  4.03283599e+09
#> Low income           4.28987656e+09  6.46379868e+09
#> Lower middle income  5.37706240e+09  8.56557908e+09
#> Upper middle income  3.48410000e+09  3.84146040e+09
#> --------------------------------------------------------------------------------

## Passing Arguments down to qsu.default: for Panel Data Statistics
descr(iris, pid = iris$Species)
#> Dataset: iris, 5 Variables, N = 150
#> --------------------------------------------------------------------------------
#> Sepal.Length (numeric): 
#> Statistics
#>          N/T  Mean    SD   Min   Max   Skew  Kurt
#> Overall  150  5.84  0.83   4.3   7.9   0.31  2.43
#> Between    3  5.84   0.8  5.01  6.59  -0.21   1.5
#> Within    50  5.84  0.51  4.16  7.16   0.12  3.26
#> 
#> Quantiles
#>    1%   5%  10%  25%  50%  75%  90%   95%  99%
#>   4.4  4.6  4.8  5.1  5.8  6.4  6.9  7.25  7.7
#> --------------------------------------------------------------------------------
#> Sepal.Width (numeric): 
#> Statistics
#>          N/T  Mean    SD   Min   Max  Skew  Kurt
#> Overall  150  3.06  0.44     2   4.4  0.32  3.18
#> Between    3  3.06  0.34  2.77  3.43  0.43   1.5
#> Within    50  3.06  0.34  1.93  4.03  0.03  3.51
#> 
#> Quantiles
#>    1%    5%  10%  25%  50%  75%   90%  95%   99%
#>   2.2  2.34  2.5  2.8    3  3.3  3.61  3.8  4.15
#> --------------------------------------------------------------------------------
#> Petal.Length (numeric): 
#> Statistics
#>          N/T  Mean    SD   Min   Max   Skew  Kurt
#> Overall  150  3.76  1.77     1   6.9  -0.27   1.6
#> Between    3  3.76  2.09  1.46  5.55  -0.42   1.5
#> Within    50  3.76  0.43   2.5  5.11   0.12  3.89
#> 
#> Quantiles
#>     1%   5%  10%  25%   50%  75%  90%  95%  99%
#>   1.15  1.3  1.4  1.6  4.35  5.1  5.8  6.1  6.7
#> --------------------------------------------------------------------------------
#> Petal.Width (numeric): 
#> Statistics
#>          N/T  Mean    SD   Min   Max   Skew  Kurt
#> Overall  150   1.2  0.76   0.1   2.5   -0.1  1.66
#> Between    3   1.2   0.9  0.25  2.03  -0.25   1.5
#> Within    50   1.2   0.2  0.57  1.67  -0.05  3.36
#> 
#> Quantiles
#>    1%   5%  10%  25%  50%  75%  90%  95%  99%
#>   0.1  0.2  0.2  0.3  1.3  1.8  2.2  2.3  2.5
#> --------------------------------------------------------------------------------
#> Species (factor): 
#> Statistics
#>     N  Ndist
#>   150      3
#> 
#> Table
#>             Freq   Perc
#> setosa        50  33.33
#> versicolor    50  33.33
#> virginica     50  33.33
#> --------------------------------------------------------------------------------
descr(wlddev, pid = wlddev$iso3c)
#> Dataset: wlddev, 13 Variables, N = 13176
#> --------------------------------------------------------------------------------
#> country (character): Country Name
#> Statistics
#>       N  Ndist
#>   13176    216
#> 
#> Table
#>                       Freq   Perc
#> Afghanistan             61   0.46
#> Albania                 61   0.46
#> Algeria                 61   0.46
#> American Samoa          61   0.46
#> Andorra                 61   0.46
#> Angola                  61   0.46
#> Antigua and Barbuda     61   0.46
#> Argentina               61   0.46
#> Armenia                 61   0.46
#> Aruba                   61   0.46
#> Australia               61   0.46
#> Austria                 61   0.46
#> Azerbaijan              61   0.46
#> Bahamas, The            61   0.46
#> ... 202 Others       12322  93.52
#> 
#> Summary of Table Frequencies
#>    Min. 1st Qu.  Median    Mean 3rd Qu.    Max. 
#>      61      61      61      61      61      61 
#> --------------------------------------------------------------------------------
#> iso3c (factor): Country Code
#> Statistics
#>       N  Ndist
#>   13176    216
#> 
#> Table
#>                  Freq   Perc
#> ABW                61   0.46
#> AFG                61   0.46
#> AGO                61   0.46
#> ALB                61   0.46
#> AND                61   0.46
#> ARE                61   0.46
#> ARG                61   0.46
#> ARM                61   0.46
#> ASM                61   0.46
#> ATG                61   0.46
#> AUS                61   0.46
#> AUT                61   0.46
#> AZE                61   0.46
#> BDI                61   0.46
#> ... 202 Others  12322  93.52
#> 
#> Summary of Table Frequencies
#>    Min. 1st Qu.  Median    Mean 3rd Qu.    Max. 
#>      61      61      61      61      61      61 
#> --------------------------------------------------------------------------------
#> date (Date): Date Recorded (Fictitious)
#> Statistics
#>          N       Ndist         Min         Max  
#>      13176          61  1961-01-01  2021-01-01  
#> --------------------------------------------------------------------------------
#> year (integer): Year
#> Statistics
#>            N/T  Mean     SD   Min   Max  Skew  Kurt
#> Overall  13176  1990  17.61  1960  2020    -0   1.8
#> Between    216  1990      0  1990  1990     -     -
#> Within      61  1990  17.61  1960  2020    -0   1.8
#> 
#> Quantiles
#>     1%    5%   10%   25%   50%   75%   90%   95%   99%
#>   1960  1963  1966  1975  1990  2005  2014  2017  2020
#> --------------------------------------------------------------------------------
#> decade (integer): Decade
#> Statistics
#>            N/T     Mean     SD      Min      Max  Skew  Kurt
#> Overall  13176  1985.57  17.51     1960     2020  0.03  1.79
#> Between    216  1985.57      0  1985.57  1985.57     -     -
#> Within      61  1985.57  17.51     1960     2020  0.03  1.79
#> 
#> Quantiles
#>     1%    5%   10%   25%   50%   75%   90%   95%   99%
#>   1960  1960  1960  1970  1990  2000  2010  2010  2020
#> --------------------------------------------------------------------------------
#> region (factor): Region
#> Statistics
#>       N  Ndist
#>   13176      7
#> 
#> Table
#>                             Freq   Perc
#> Europe & Central Asia       3538  26.85
#> Sub-Saharan Africa          2928  22.22
#> Latin America & Caribbean   2562  19.44
#> East Asia & Pacific         2196  16.67
#> Middle East & North Africa  1281   9.72
#> South Asia                   488   3.70
#> North America                183   1.39
#> --------------------------------------------------------------------------------
#> income (factor): Income Level
#> Statistics
#>       N  Ndist
#>   13176      4
#> 
#> Table
#>                      Freq   Perc
#> High income          4819  36.57
#> Upper middle income  3660  27.78
#> Lower middle income  2867  21.76
#> Low income           1830  13.89
#> --------------------------------------------------------------------------------
#> OECD (logical): Is OECD Member Country?
#> Statistics
#>       N  Ndist
#>   13176      2
#> 
#> Table
#>         Freq   Perc
#> FALSE  10980  83.33
#> TRUE    2196  16.67
#> --------------------------------------------------------------------------------
#> PCGDP (numeric): GDP per capita (constant 2010 US$)
#> Statistics (28.13% NAs)
#>            N/T      Mean        SD        Min        Max  Skew   Kurt
#> Overall   9470  12048.78  19077.64     132.08  196061.42  3.13  17.12
#> Between    206  12962.61   20189.9     253.19  141200.38  3.13  16.23
#> Within   45.97  12048.78   6723.68  -33504.87   76767.53  0.66   17.2
#> 
#> Quantiles
#>       1%      5%     10%      25%      50%       75%       90%       95%
#>   227.71  399.62  555.55  1303.19  3767.16  14787.03  35646.02  48507.84
#>        99%
#>   92340.28
#> --------------------------------------------------------------------------------
#> LIFEEX (numeric): Life expectancy at birth, total (years)
#> Statistics (11.43% NAs)
#>            N/T   Mean     SD    Min    Max   Skew  Kurt
#> Overall  11670   64.3  11.48  18.91  85.42  -0.67  2.67
#> Between    207  64.95   9.89  40.97  85.42   -0.5  2.17
#> Within   56.38   64.3   6.08  32.91  84.42  -0.26   3.7
#> 
#> Quantiles
#>      1%     5%    10%    25%    50%    75%    90%    95%    99%
#>   35.83  42.77  46.83  56.36  67.44  72.95  77.08  79.34  82.36
#> --------------------------------------------------------------------------------
#> GINI (numeric): Gini index (World Bank estimate)
#> Statistics (86.76% NAs)
#>            N/T   Mean    SD    Min    Max  Skew  Kurt
#> Overall   1744  38.53   9.2   20.7   65.8   0.6  2.53
#> Between    167  39.42  8.14  24.87  61.71  0.58  2.83
#> Within   10.44  38.53  2.93  25.39  55.36  0.33  5.34
#> 
#> Quantiles
#>     1%    5%   10%   25%   50%  75%   90%    95%   99%
#>   24.6  26.3  27.6  31.5  36.4   45  52.6  55.98  60.5
#> --------------------------------------------------------------------------------
#> ODA (numeric): Net official development assistance and official aid received (constant 2018 US$)
#> Statistics (34.67% NAs)
#>            N/T        Mean          SD              Min             Max  Skew
#> Overall   8608  454'720131  868'712654      -997'679993  2.56715605e+10  6.98
#> Between    178  439'168412  569'049959        468717.92  3.62337432e+09  2.36
#> Within   48.36  454'720131  650'709624  -2.44379420e+09  2.45610972e+10   9.6
#>            Kurt
#> Overall  114.89
#> Between    9.95
#> Within   263.37
#> 
#> Quantiles
#>             1%           5%          10%          25%         50%         75%
#>   -12'593999.7  1'363500.01  8'347000.31  44'887499.8  165'970001  495'042503
#>              90%             95%             99%
#>   1.18400697e+09  1.93281696e+09  3.73380782e+09
#> --------------------------------------------------------------------------------
#> POP (numeric): Population, total
#> Statistics (1.95% NAs)
#>            N/T         Mean           SD          Min             Max   Skew
#> Overall  12919  24'245971.6   102'120674         2833  1.39771500e+09   9.75
#> Between    216    24'178573  98'616506.7      8343.33  1.08786967e+09      9
#> Within   59.81  24'245971.6  26'803077.4  -405'793067      510'077008  -0.41
#>            Kurt
#> Overall  108.91
#> Between   90.02
#> Within   149.24
#> 
#> Quantiles
#>        1%       5%      10%     25%       50%        75%          90%
#>   8698.84  31083.3  62268.4  443791  4'072517  12'816178  46'637331.4
#>           95%         99%
#>   81'177252.5  308'862641
#> --------------------------------------------------------------------------------
```
