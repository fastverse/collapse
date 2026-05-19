# Fast (Grouped, Weighted) Scaling and Centering of Matrix-like Objects

`fscale` is a generic function to efficiently standardize (scale and
center) data. `STD` is a wrapper around `fscale` representing the
'standardization operator', with more options than `fscale` when applied
to matrices and data frames. Standardization can be simple or groupwise,
ordinary or weighted. Arbitrary target means and standard deviations can
be set, with special options for grouped scaling and centering. It is
also possible to scale data without centering i.e. perform
mean-preserving scaling.

## Usage

``` r
fscale(x, ...)
   STD(x, ...)

# Default S3 method
fscale(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], mean = 0, sd = 1, ...)
# Default S3 method
STD(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], mean = 0, sd = 1, ...)

# S3 method for class 'matrix'
fscale(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], mean = 0, sd = 1, ...)
# S3 method for class 'matrix'
STD(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], mean = 0, sd = 1,
    stub = .op[["stub"]], ...)

# S3 method for class 'data.frame'
fscale(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], mean = 0, sd = 1, ...)
# S3 method for class 'data.frame'
STD(x, by = NULL, w = NULL, cols = is.numeric, na.rm = .op[["na.rm"]],
    mean = 0, sd = 1, stub = .op[["stub"]], keep.by = TRUE, keep.w = TRUE, ...)

# Methods for indexed data / compatibility with plm:

# S3 method for class 'pseries'
fscale(x, effect = 1L, w = NULL, na.rm = .op[["na.rm"]], mean = 0, sd = 1, ...)
# S3 method for class 'pseries'
STD(x, effect = 1L, w = NULL, na.rm = .op[["na.rm"]], mean = 0, sd = 1, ...)

# S3 method for class 'pdata.frame'
fscale(x, effect = 1L, w = NULL, na.rm = .op[["na.rm"]], mean = 0, sd = 1, ...)
# S3 method for class 'pdata.frame'
STD(x, effect = 1L, w = NULL, cols = is.numeric, na.rm = .op[["na.rm"]],
    mean = 0, sd = 1, stub = .op[["stub"]], keep.ids = TRUE, keep.w = TRUE, ...)

# Methods for grouped data frame / compatibility with dplyr:

# S3 method for class 'grouped_df'
fscale(x, w = NULL, na.rm = .op[["na.rm"]], mean = 0, sd = 1,
       keep.group_vars = TRUE, keep.w = TRUE, ...)
# S3 method for class 'grouped_df'
STD(x, w = NULL, na.rm = .op[["na.rm"]], mean = 0, sd = 1,
    stub = .op[["stub"]], keep.group_vars = TRUE, keep.w = TRUE, ...)
```

## Arguments

- x:

  a numeric vector, matrix, data frame, 'indexed_series' ('pseries'),
  'indexed_frame' ('pdata.frame') or grouped data frame ('grouped_df').

- g:

  a factor, [`GRP`](https://fastverse.org/collapse/reference/GRP.md)
  object, or atomic vector / list of vectors (internally grouped with
  [`group`](https://fastverse.org/collapse/reference/group.md)) used to
  group `x`.

- by:

  *STD data.frame method*: Same as `g`, but also allows one- or
  two-sided formulas i.e. `~ group1` or `var1 + var2 ~ group1 + group2`.
  See Examples.

- cols:

  *STD (p)data.frame method*: Select columns to scale using a function,
  column names, indices or a logical vector. Default: All numeric
  columns. *Note*: `cols` is ignored if a two-sided formula is passed to
  `by`.

- w:

  a numeric vector of (non-negative) weights. `STD` data frame and
  `pdata.frame` methods also allow a one-sided formula i.e.
  `~ weightcol`. The `grouped_df` (*dplyr*) method supports
  lazy-evaluation. See Examples.

- na.rm:

  logical. Skip missing values in `x` or `w` when computing means and
  sd's.

- effect:

  *plm* methods: Select which panel identifier should be used as
  group-id. 1L takes the first variable in the
  [index](https://fastverse.org/collapse/reference/indexing.md), 2L the
  second etc.. Index variables can also be called by name using a
  character string. More than one variable can be supplied.

- stub:

  character. A prefix/stub to add to the names of all transformed
  columns. `TRUE` (default) uses `"STD."`, `FALSE` will not rename
  columns.

- mean:

  the mean to center on (default is 0). If `mean = FALSE`, no centering
  will be performed. In that case the scaling is mean-preserving. A
  numeric value different from 0 (i.e. `mean = 5`) will be added to the
  data after subtracting out the mean(s), such that the data will have a
  mean of 5. A special option when performing grouped scaling and
  centering is `mean = "overall.mean"`. In that case the overall mean of
  the data will be added after subtracting out group means.

- sd:

  the standard deviation to scale the data to (default is 1). A numeric
  value different from 0 (i.e. `sd = 3`) will scale the data to have a
  standard deviation of 3. A special option when performing grouped
  scaling is `sd = "within.sd"`. In that case the within standard
  deviation (= the standard deviation of the group-centered series) will
  be calculated and applied to each group. The results is that the
  variance of the data within each group is harmonized without forcing a
  certain variance (such as 1).

- keep.by, keep.ids, keep.group_vars:

  *data.frame, pdata.frame and grouped_df methods*: Logical. Retain
  grouping / panel-identifier columns in the output. For
  `STD.data.frame` this only works if grouping variables were passed in
  a formula.

- keep.w:

  *data.frame, pdata.frame and grouped_df methods*: Logical. Retain
  column containing the weights in the output. Only works if `w` is
  passed as formula / lazy-expression.

- ...:

  arguments to be passed to or from other methods.

## Details

If `g = NULL`, `fscale` by default (column-wise) subtracts the mean or
weighted mean (if `w` is supplied) from all data points in `x`, and then
divides this difference by the standard deviation or frequency-weighted
standard deviation. The result is that all columns in `x` will have a
(weighted) mean 0 and (weighted) standard deviation 1. Alternatively,
data can be scaled to have a mean of `mean` and a standard deviation of
`sd`. If `mean = FALSE` the data is only scaled (not centered) such that
the mean of the data is preserved.  

Means and standard deviations are computed using Welford's numerically
stable online algorithm.

With groups supplied to `g`, this standardizing becomes groupwise, so
that in each group (in each column) the data points will have mean
`mean` and standard deviation `sd`. Naturally if `mean = FALSE` then
each group is just scaled and the mean is preserved. For centering
without scaling see
[`fwithin`](https://fastverse.org/collapse/reference/fbetween_fwithin.md).

If `na.rm = FALSE` and a `NA` or `NaN` is encountered, the mean and sd
for that group will be `NA`, and all data points belonging to that group
will also be `NA` in the output.

If `na.rm = TRUE`, means and sd's are computed (column-wise) on the
available data points, and also the weight vector can have missing
values. In that case, the weighted mean an sd are computed on
(column-wise) `complete.cases(x, w)`, and `x` is scaled using these
statistics. *Note* that `fscale` will not insert a missing value in `x`
if the weight for that value is missing, rather, that value will be
scaled using a weighted mean and standard-deviated computed without
itself! (The intention here is that a few (randomly) missing weights
shouldn't break the computation when `na.rm = TRUE`, but it is not meant
for weight vectors with many missing values. If you don't like this
behavior, you should prepare your data using `x[is.na(w), ] <- NA`, or
impute your weight vector for non-missing `x`).

Special options for grouped scaling are `mean = "overall.mean"` and
`sd = "within.sd"`. The former group-centers vectors on the overall mean
of the data (see
[`fwithin`](https://fastverse.org/collapse/reference/fbetween_fwithin.md)
for more details) and the latter scales the data in each group to have
the within-group standard deviation (= the standard deviation of the
group-centered data). Thus scaling a grouped vector with options
`mean = "overall.mean"` and `sd = "within.sd"` amounts to removing all
differences in the mean and standard deviations between these groups. In
weighted computations, `mean = "overall.mean"` will subtract weighted
group-means from the data and add the overall weighted mean of the data,
whereas `sd = "within.sd"` will compute the weighted within- standard
deviation and apply it to each group.

## Value

`x` standardized (mean = mean, standard deviation = sd), grouped by
`g/by`, weighted with `w`. See Details.

## Note

For centering without scaling see
[`fwithin/W`](https://fastverse.org/collapse/reference/fbetween_fwithin.md).
For simple not mean-preserving scaling use
[`fsd(..., TRA = "/")`](https://fastverse.org/collapse/reference/fvar_fsd.md).
To sweep pre-computed means and scale-factors out of data see
[`TRA`](https://fastverse.org/collapse/reference/TRA.md).

## See also

[`fwithin`](https://fastverse.org/collapse/reference/fbetween_fwithin.md),
[`fsd`](https://fastverse.org/collapse/reference/fvar_fsd.md),
[`TRA`](https://fastverse.org/collapse/reference/TRA.md), [Fast
Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Simple Scaling & Centering / Standardizing
head(fscale(mtcars))               # Doesn't rename columns
#>                          mpg        cyl        disp         hp       drat
#> Mazda RX4          0.1508848 -0.1049878 -0.57061982 -0.5350928  0.5675137
#> Mazda RX4 Wag      0.1508848 -0.1049878 -0.57061982 -0.5350928  0.5675137
#> Datsun 710         0.4495434 -1.2248578 -0.99018209 -0.7830405  0.4739996
#> Hornet 4 Drive     0.2172534 -0.1049878  0.22009369 -0.5350928 -0.9661175
#> Hornet Sportabout -0.2307345  1.0148821  1.04308123  0.4129422 -0.8351978
#> Valiant           -0.3302874 -0.1049878 -0.04616698 -0.6080186 -1.5646078
#>                             wt       qsec         vs         am       gear
#> Mazda RX4         -0.610399567 -0.7771651 -0.8680278  1.1899014  0.4235542
#> Mazda RX4 Wag     -0.349785269 -0.4637808 -0.8680278  1.1899014  0.4235542
#> Datsun 710        -0.917004624  0.4260068  1.1160357  1.1899014  0.4235542
#> Hornet 4 Drive    -0.002299538  0.8904872  1.1160357 -0.8141431 -0.9318192
#> Hornet Sportabout  0.227654255 -0.4637808 -0.8680278 -0.8141431 -0.9318192
#> Valiant            0.248094592  1.3269868  1.1160357 -0.8141431 -0.9318192
#>                         carb
#> Mazda RX4          0.7352031
#> Mazda RX4 Wag      0.7352031
#> Datsun 710        -1.1221521
#> Hornet 4 Drive    -1.1221521
#> Hornet Sportabout -0.5030337
#> Valiant           -1.1221521
head(STD(mtcars))                  # By default adds a prefix
#>                      STD.mpg    STD.cyl    STD.disp     STD.hp   STD.drat
#> Mazda RX4          0.1508848 -0.1049878 -0.57061982 -0.5350928  0.5675137
#> Mazda RX4 Wag      0.1508848 -0.1049878 -0.57061982 -0.5350928  0.5675137
#> Datsun 710         0.4495434 -1.2248578 -0.99018209 -0.7830405  0.4739996
#> Hornet 4 Drive     0.2172534 -0.1049878  0.22009369 -0.5350928 -0.9661175
#> Hornet Sportabout -0.2307345  1.0148821  1.04308123  0.4129422 -0.8351978
#> Valiant           -0.3302874 -0.1049878 -0.04616698 -0.6080186 -1.5646078
#>                         STD.wt   STD.qsec     STD.vs     STD.am   STD.gear
#> Mazda RX4         -0.610399567 -0.7771651 -0.8680278  1.1899014  0.4235542
#> Mazda RX4 Wag     -0.349785269 -0.4637808 -0.8680278  1.1899014  0.4235542
#> Datsun 710        -0.917004624  0.4260068  1.1160357  1.1899014  0.4235542
#> Hornet 4 Drive    -0.002299538  0.8904872  1.1160357 -0.8141431 -0.9318192
#> Hornet Sportabout  0.227654255 -0.4637808 -0.8680278 -0.8141431 -0.9318192
#> Valiant            0.248094592  1.3269868  1.1160357 -0.8141431 -0.9318192
#>                     STD.carb
#> Mazda RX4          0.7352031
#> Mazda RX4 Wag      0.7352031
#> Datsun 710        -1.1221521
#> Hornet 4 Drive    -1.1221521
#> Hornet Sportabout -0.5030337
#> Valiant           -1.1221521
qsu(STD(mtcars))                   # See that is works
#>            N  Mean  SD      Min     Max
#> STD.mpg   32     0   1  -1.6079  2.2913
#> STD.cyl   32     0   1  -1.2249  1.0149
#> STD.disp  32    -0   1  -1.2879  1.9468
#> STD.hp    32     0   1   -1.381  2.7466
#> STD.drat  32    -0   1  -1.5646  2.4939
#> STD.wt    32    -0   1  -1.7418  2.2553
#> STD.qsec  32    -0   1   -1.874  2.8268
#> STD.vs    32     0   1   -0.868   1.116
#> STD.am    32    -0   1  -0.8141  1.1899
#> STD.gear  32    -0   1  -0.9318  1.7789
#> STD.carb  32    -0   1  -1.1222  3.2117
qsu(STD(mtcars, mean = 5, sd = 3)) # Assigning a mean of 5 and a standard deviation of 3
#>            N  Mean  SD      Min      Max
#> STD.mpg   32     5   3   0.1764  11.8738
#> STD.cyl   32     5   3   1.3254   8.0446
#> STD.disp  32     5   3   1.1363  10.8403
#> STD.hp    32     5   3   0.8569  13.2397
#> STD.drat  32     5   3   0.3062  12.4817
#> STD.wt    32     5   3  -0.2253   11.766
#> STD.qsec  32     5   3   -0.622  13.4803
#> STD.vs    32     5   3   2.3959   8.3481
#> STD.am    32     5   3   2.5576   8.5697
#> STD.gear  32     5   3   2.2045  10.3368
#> STD.carb  32     5   3   1.6335   14.635
qsu(STD(mtcars, mean = FALSE))     # No centering: Scaling is mean-preserving
#>            N      Mean  SD       Min       Max
#> STD.mpg   32   20.0906   1   18.4827   22.3819
#> STD.cyl   32    6.1875   1    4.9626    7.2024
#> STD.disp  32  230.7219   1   229.434  232.6686
#> STD.hp    32  146.6875   1  145.3065  149.4341
#> STD.drat  32    3.5966   1     2.032    6.0905
#> STD.wt    32    3.2172   1    1.4755    5.4726
#> STD.qsec  32   17.8487   1   15.9747   20.6755
#> STD.vs    32    0.4375   1   -0.4305    1.5535
#> STD.am    32    0.4062   1   -0.4079    1.5962
#> STD.gear  32    3.6875   1    2.7557    5.4664
#> STD.carb  32    2.8125   1    1.6903    6.0242

## Panel Data
head(fscale(get_vars(wlddev,9:12), wlddev$iso3c))   # Standardizing 4 series within each country
#>   PCGDP    LIFEEX GINI        ODA
#> 1    NA -1.653181   NA -0.6498451
#> 2    NA -1.602256   NA -0.5951801
#> 3    NA -1.552023   NA -0.6517082
#> 4    NA -1.502678   NA -0.5925063
#> 5    NA -1.454122   NA -0.5649154
#> 6    NA -1.406257   NA -0.5431461
head(STD(wlddev, ~iso3c, cols = 9:12))              # Same thing using STD, id's added
#>   iso3c STD.PCGDP STD.LIFEEX STD.GINI    STD.ODA
#> 1   AFG        NA  -1.653181       NA -0.6498451
#> 2   AFG        NA  -1.602256       NA -0.5951801
#> 3   AFG        NA  -1.552023       NA -0.6517082
#> 4   AFG        NA  -1.502678       NA -0.5925063
#> 5   AFG        NA  -1.454122       NA -0.5649154
#> 6   AFG        NA  -1.406257       NA -0.5431461
pwcor(fscale(get_vars(wlddev,9:12), wlddev$iso3c))  # Correlaing panel series after standardizing
#>        PCGDP LIFEEX  GINI   ODA
#> PCGDP     1     .62  -.20   .09
#> LIFEEX   .62     1   -.12   .32
#> GINI    -.20   -.12    1   -.09
#> ODA      .09    .32  -.09    1 

fmean(get_vars(wlddev, 9:12))                       # This calculates the overall means
#>        PCGDP       LIFEEX         GINI          ODA 
#> 1.204878e+04 6.429630e+01 3.853412e+01 4.547201e+08 
fsd(fwithin(get_vars(wlddev, 9:12), wlddev$iso3c))  # This calculates the within standard deviations
#>        PCGDP       LIFEEX         GINI          ODA 
#> 6.723681e+03 6.084205e+00 2.927700e+00 6.507096e+08 
head(qsu(fscale(get_vars(wlddev, 9:12),             # This group-centers on the overall mean and
    wlddev$iso3c,                                   # group-scales to the within standard deviation
    mean = "overall.mean", sd = "within.sd"),       # -> data harmonized in the first 2 moments
    by = wlddev$iso3c))
#> , , PCGDP
#> 
#>       N       Mean         SD          Min         Max
#> ABW  32  12048.778  6723.6808  -13032.4333  19779.7812
#> AFG  18  12048.778  6723.6808    2023.3142   18822.252
#> AGO  40  12048.778  6723.6808   -1027.1433  22877.0816
#> ALB  40  12048.778  6723.6808    3212.4503  25460.2799
#> AND  50  12048.778  6723.6808    -111.8716  24171.0081
#> ARE  45  12048.778  6723.6808    3206.3558  26904.4271
#> 
#> , , LIFEEX
#> 
#>       N     Mean      SD      Min      Max
#> ABW  60  64.2963  6.0842  49.8607  72.6147
#> AFG  60  64.2963  6.0842   54.238  73.6849
#> AGO  60  64.2963  6.0842  55.6648  77.7463
#> ALB  60  64.2963  6.0842  50.8542  74.1559
#> AND   0        0       -        0        0
#> ARE  60  64.2963  6.0842  49.6668  71.3434
#> 
#> , , GINI
#> 
#>      N     Mean      SD      Min      Max
#> ABW  0        0       -        0        0
#> AFG  0        0       -        0        0
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 1 slice ] 

## Indexed data
wldi <- findex_by(wlddev, iso3c, year)
head(STD(wldi))                                  # Standardizing all numeric variables by country
#>   iso3c year STD.decade STD.PCGDP STD.LIFEEX STD.GINI    STD.ODA    STD.POP
#> 1   AFG 1960  -1.448413        NA  -1.653181       NA -0.6498451 -1.0754727
#> 2   AFG 1961  -1.448413        NA  -1.602256       NA -0.5951801 -1.0556707
#> 3   AFG 1962  -1.448413        NA  -1.552023       NA -0.6517082 -1.0347669
#> 4   AFG 1963  -1.448413        NA  -1.502678       NA -0.5925063 -1.0127455
#> 5   AFG 1964  -1.448413        NA  -1.454122       NA -0.5649154 -0.9895973
#> 6   AFG 1965  -1.448413        NA  -1.406257       NA -0.5431461 -0.9653050
#> 
#> Indexed by:  iso3c [1] | year [6 (61)] 
head(STD(wldi, effect = 2L))                     # Standardizing all numeric variables by year
#>   iso3c year STD.decade STD.PCGDP STD.LIFEEX STD.GINI      STD.ODA     STD.POP
#> 1   AFG 1960        NaN        NA  -1.755632       NA -0.185893459 -0.08682756
#> 2   AFG 1961        NaN        NA  -1.759208       NA -0.059419593 -0.08697252
#> 3   AFG 1962        NaN        NA  -1.769154       NA -0.282990408 -0.08693992
#> 4   AFG 1963        NaN        NA  -1.772255       NA -0.071937146 -0.08682708
#> 5   AFG 1964        NaN        NA  -1.774865       NA -0.010951326 -0.08661029
#> 6   AFG 1965        NaN        NA  -1.790502       NA  0.006003786 -0.08627312
#> 
#> Indexed by:  iso3c [1] | year [6 (61)] 

## Weighted Standardizing
weights = abs(rnorm(nrow(wlddev)))
head(fscale(get_vars(wlddev,9:12), wlddev$iso3c, weights))
#>   PCGDP    LIFEEX GINI        ODA
#> 1    NA -1.467564   NA -0.4994722
#> 2    NA -1.414317   NA -0.4338014
#> 3    NA -1.361793   NA -0.5017104
#> 4    NA -1.310197   NA -0.4305893
#> 5    NA -1.259427   NA -0.3974435
#> 6    NA -1.209379   NA -0.3712913
head(STD(wlddev, ~iso3c, weights, 9:12))
#>   iso3c STD.PCGDP STD.LIFEEX STD.GINI    STD.ODA
#> 1   AFG        NA  -1.467564       NA -0.4994722
#> 2   AFG        NA  -1.414317       NA -0.4338014
#> 3   AFG        NA  -1.361793       NA -0.5017104
#> 4   AFG        NA  -1.310197       NA -0.4305893
#> 5   AFG        NA  -1.259427       NA -0.3974435
#> 6   AFG        NA  -1.209379       NA -0.3712913

# Grouped data
wlddev |> fgroup_by(iso3c) |> fselect(PCGDP,LIFEEX) |> STD()
#>    STD.PCGDP  STD.LIFEEX
#> 1         NA -1.65318076
#> 2         NA -1.60225647
#> 3         NA -1.55202301
#> 4         NA -1.50267777
#> 5         NA -1.45412205
#> 6         NA -1.40625717
#> 7         NA -1.35868835
#> 8         NA -1.31092216
#> 9         NA -1.26266251
#> 10        NA -1.21361334
#> 11        NA -1.16337988
#> 12        NA -1.11196214
#> 13        NA -1.05955749
#> 14        NA -1.00606725
#> 15        NA -0.95129403
#> 16        NA -0.89504046
#> 17        NA -0.83710914
#> 18        NA -0.77740140
#> 19        NA -0.71581853
#> 20        NA -0.65255793
#> 21        NA -0.58752090
#> 22        NA -0.52051007
#> 23        NA -0.45201887
#> 24        NA -0.38224470
#> 25        NA -0.31158231
#> 26        NA -0.24042647
#> 27        NA -0.16887587
#> 28        NA -0.09732527
#> 29        NA -0.02636681
#> 30        NA  0.04370344
#> 31        NA  0.11189856
#> 32        NA  0.17782381
#> 33        NA  0.24118310
#> 34        NA  0.30187774
#> 35        NA  0.35971037
#>  [ reached 'max' / getOption("max.print") -- omitted 13141 rows ]
#> 
#> Grouped by:  iso3c  [216 | 61 (0)] 
wlddev |> fgroup_by(iso3c) |> fselect(PCGDP,LIFEEX) |> STD(weights) # weighted standardizing
#>    STD.PCGDP   STD.LIFEEX
#> 1         NA -1.467563659
#> 2         NA -1.414317039
#> 3         NA -1.361792758
#> 4         NA -1.310197196
#> 5         NA -1.259427164
#> 6         NA -1.209379469
#> 7         NA -1.159641348
#> 8         NA -1.109696844
#> 9         NA -1.059236385
#> 10        NA -1.007950397
#> 11        NA -0.955426116
#> 12        NA -0.901663540
#> 13        NA -0.846869054
#> 14        NA -0.790939466
#> 15        NA -0.733668392
#> 16        NA -0.674849452
#> 17        NA -0.614276263
#> 18        NA -0.551845634
#> 19        NA -0.487454373
#> 20        NA -0.421308863
#> 21        NA -0.353305913
#> 22        NA -0.283239140
#> 23        NA -0.211624501
#> 24        NA -0.138668377
#> 25        NA -0.064783533
#> 26        NA  0.009617267
#> 27        NA  0.084430831
#> 28        NA  0.159244395
#> 29        NA  0.233438812
#> 30        NA  0.306704510
#> 31        NA  0.378009576
#> 32        NA  0.446941246
#> 33        NA  0.513189947
#> 34        NA  0.576652487
#> 35        NA  0.637122485
#>  [ reached 'max' / getOption("max.print") -- omitted 13141 rows ]
#> 
#> Grouped by:  iso3c  [216 | 61 (0)] 
wlddev |> fgroup_by(iso3c) |> fselect(PCGDP,LIFEEX,POP) |> STD(POP) # weighting by POP ->
#>         POP STD.PCGDP STD.LIFEEX
#> 1   8996973        NA -2.1727695
#> 2   9169410        NA -2.1194893
#> 3   9351441        NA -2.0669319
#> 4   9543205        NA -2.0153038
#> 5   9744781        NA -1.9645018
#> 6   9956320        NA -1.9144225
#> 7  10174836        NA -1.8646530
#> 8  10399926        NA -1.8146770
#> 9  10637063        NA -1.7641848
#> 10 10893776        NA -1.7128664
#> 11 11173642        NA -1.6603090
#> 12 11475445        NA -1.6065126
#> 13 11791215        NA -1.5516835
#> 14 12108963        NA -1.4957187
#> 15 12412950        NA -1.4384115
#> 16 12689160        NA -1.3795555
#> 17 12943093        NA -1.3189441
#> 18 13171306        NA -1.2564741
#> 19 13341198        NA -1.1920422
#> 20 13411056        NA -1.1258550
#> 21 13356511        NA -1.0578092
#> 22 13171673        NA -0.9876983
#> 23 12882528        NA -0.9160385
#>  [ reached 'max' / getOption("max.print") -- omitted 13153 rows ]
#> 
#> Grouped by:  iso3c  [216 | 61 (0)] 
# ..keeps the weight column unless keep.w = FALSE
```
