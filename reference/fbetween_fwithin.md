# Fast Between (Averaging) and (Quasi-)Within (Centering) Transformations

`fbetween` and `fwithin` are S3 generics to efficiently obtain
between-transformed (averaged) or (quasi-)within-transformed (demeaned)
data. These operations can be performed groupwise and/or weighted. `B`
and `W` are wrappers around `fbetween` and `fwithin` representing the
'between-operator' and the 'within-operator'.

(`B` / `W` provide more flexibility than `fbetween` / `fwithin` when
applied to data frames (i.e. column subsetting, formula input,
auto-renaming and id-variable-preservation capabilities...), but are
otherwise identical.)

## Usage

``` r
fbetween(x, ...)
 fwithin(x, ...)
       B(x, ...)
       W(x, ...)

# Default S3 method
fbetween(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, ...)
# Default S3 method
fwithin(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], mean = 0, theta = 1, ...)
# Default S3 method
B(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, ...)
# Default S3 method
W(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], mean = 0, theta = 1, ...)

# S3 method for class 'matrix'
fbetween(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, ...)
# S3 method for class 'matrix'
fwithin(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], mean = 0, theta = 1, ...)
# S3 method for class 'matrix'
B(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, stub = .op[["stub"]], ...)
# S3 method for class 'matrix'
W(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], mean = 0, theta = 1,
  stub = .op[["stub"]], ...)

# S3 method for class 'data.frame'
fbetween(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, ...)
# S3 method for class 'data.frame'
fwithin(x, g = NULL, w = NULL, na.rm = .op[["na.rm"]], mean = 0, theta = 1, ...)
# S3 method for class 'data.frame'
B(x, by = NULL, w = NULL, cols = is.numeric, na.rm = .op[["na.rm"]],
  fill = FALSE, stub = .op[["stub"]], keep.by = TRUE, keep.w = TRUE, ...)
# S3 method for class 'data.frame'
W(x, by = NULL, w = NULL, cols = is.numeric, na.rm = .op[["na.rm"]],
  mean = 0, theta = 1, stub = .op[["stub"]], keep.by = TRUE, keep.w = TRUE, ...)

# Methods for indexed data / compatibility with plm:

# S3 method for class 'pseries'
fbetween(x, effect = 1L, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, ...)
# S3 method for class 'pseries'
fwithin(x, effect = 1L, w = NULL, na.rm = .op[["na.rm"]], mean = 0, theta = 1, ...)
# S3 method for class 'pseries'
B(x, effect = 1L, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, ...)
# S3 method for class 'pseries'
W(x, effect = 1L, w = NULL, na.rm = .op[["na.rm"]], mean = 0, theta = 1, ...)

# S3 method for class 'pdata.frame'
fbetween(x, effect = 1L, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, ...)
# S3 method for class 'pdata.frame'
fwithin(x, effect = 1L, w = NULL, na.rm = .op[["na.rm"]], mean = 0, theta = 1, ...)
# S3 method for class 'pdata.frame'
B(x, effect = 1L, w = NULL, cols = is.numeric, na.rm = .op[["na.rm"]],
  fill = FALSE, stub = .op[["stub"]], keep.ids = TRUE, keep.w = TRUE, ...)
# S3 method for class 'pdata.frame'
W(x, effect = 1L, w = NULL, cols = is.numeric, na.rm = .op[["na.rm"]],
  mean = 0, theta = 1, stub = .op[["stub"]], keep.ids = TRUE, keep.w = TRUE, ...)

# Methods for grouped data frame / compatibility with dplyr:

# S3 method for class 'grouped_df'
fbetween(x, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE,
         keep.group_vars = TRUE, keep.w = TRUE, ...)
# S3 method for class 'grouped_df'
fwithin(x, w = NULL, na.rm = .op[["na.rm"]], mean = 0, theta = 1,
        keep.group_vars = TRUE, keep.w = TRUE, ...)
# S3 method for class 'grouped_df'
B(x, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE,
  stub = .op[["stub"]], keep.group_vars = TRUE, keep.w = TRUE, ...)
# S3 method for class 'grouped_df'
W(x, w = NULL, na.rm = .op[["na.rm"]], mean = 0, theta = 1,
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

  *B and W data.frame method*: Same as g, but also allows one- or
  two-sided formulas i.e. `~ group1` or `var1 + var2 ~ group1 + group2`.
  See Examples.

- w:

  a numeric vector of (non-negative) weights. `B`/`W` data frame and
  `pdata.frame` methods also allow a one-sided formula i.e.
  `~ weightcol`. The `grouped_df` (*dplyr*) method supports
  lazy-evaluation. See Examples.

- cols:

  *B/W (p)data.frame methods*: Select columns to scale using a function,
  column names, indices or a logical vector. Default: All numeric
  columns. *Note*: `cols` is ignored if a two-sided formula is passed to
  `by`.

- na.rm:

  logical. Skip missing values in `x` and `w` when computing averages.
  If `na.rm = FALSE` and a `NA` or `NaN` is encountered, the average for
  that group will be `NA`, and all data points belonging to that group
  in the output vector will also be `NA`.

- effect:

  *plm* methods: Select which panel identifier should be used as
  grouping variable. 1L takes the first variable in the
  [index](https://fastverse.org/collapse/reference/indexing.md), 2L the
  second etc. Index variables can also be called by name using a
  character string. If more than one variable is supplied, the
  corresponding index-factors are interacted.

- stub:

  character. A prefix/stub to add to the names of all transformed
  columns. `TRUE` (default) uses `"W."/"B."`, `FALSE` will not rename
  columns.

- fill:

  *option to `fbetween`/`B`*: Logical. `TRUE` will overwrite missing
  values in `x` with the respective average. By default missing values
  in `x` are preserved.

- mean:

  *option to `fwithin`/`W`*: The mean to center on, default is 0, but a
  different mean can be supplied and will be added to the data after the
  centering is performed. A special option when performing grouped
  centering is `mean = "overall.mean"`. In that case the overall mean of
  the data will be added after subtracting out group means.

- theta:

  *option to `fwithin`/`W`*: Double. An optional scalar parameter for
  quasi-demeaning i.e. `x - theta * xi.`. This is useful for variance
  components ('random-effects') estimators. see Details.

- keep.by, keep.ids, keep.group_vars:

  *B and W data.frame, pdata.frame and grouped_df methods*: Logical.
  Retain grouping / panel-identifier columns in the output. For data
  frames this only works if grouping variables were passed in a formula.

- keep.w:

  *B and W data.frame, pdata.frame and grouped_df methods*: Logical.
  Retain column containing the weights in the output. Only works if `w`
  is passed as formula / lazy-expression.

- ...:

  arguments to be passed to or from other methods.

## Details

Without groups, `fbetween`/`B` replaces all data points in `x` with
their mean or weighted mean (if `w` is supplied). Similarly `fwithin/W`
subtracts the (weighted) mean from all data points i.e. centers the data
on the mean.  

With groups supplied to `g`, the replacement / centering performed by
`fbetween/B` \| `fwithin/W` becomes groupwise. In terms of panel data
notation: If `x` is a vector in such a panel dataset, `xit` denotes a
single data-point belonging to group `i` in time-period `t` (`t` need
not be a time-period). Then `xi.` denotes `x`, averaged over `t`.
`fbetween`/`B` now returns `xi.` and `fwithin`/`W` returns `x - xi.`.
Thus for any data `x` and any grouping vector `g`:
`B(x,g) + W(x,g) = xi. + x - xi. = x`. In terms of variance,
`fbetween/B` only retains the variance between group averages, while
`fwithin`/`W`, by subtracting out group means, only retains the variance
within those groups.  

The data replacement performed by `fbetween`/`B` can keep (default) or
overwrite missing values (option `fill = TRUE`) in `x`. `fwithin/W` can
center data simply (default), or add back a mean after centering (option
`mean = value`), or add the overall mean in groupwise computations
(option `mean = "overall.mean"`). Let `x..` denote the overall mean of
`x`, then `fwithin`/`W` with `mean = "overall.mean"` returns
`x - xi. + x..` instead of `x - xi.`. This is useful to get rid of
group-differences but preserve the overall level of the data. In
regression analysis, centering with `mean = "overall.mean"` will only
change the constant term. See Examples.

If `theta != 1`, `fwithin`/`W` performs quasi-demeaning
`x - theta * xi.`. If `mean = "overall.mean"`,
`x - theta * xi. + theta * x..` is returned, so that the mean of the
partially demeaned data is still equal to the overall data mean `x..`. A
numeric value passed to `mean` will simply be added back to the
quasi-demeaned data i.e. `x - theta * xi. + mean`.

Now in the case of a linear panel model \\y\_{it} = \beta_0 + \beta_1
X\_{it} + u\_{it}\\ with \\u\_{it} = \alpha_i + \epsilon\_{it}\\. If
\\\alpha_i \neq \alpha = const.\\ (there exists individual
heterogeneity), then pooled OLS is at least inefficient and inference on
\\\beta_1\\ is invalid. If \\E\[\alpha_i\|X\_{it}\] = 0\\ (mean
independence of individual heterogeneity \\\alpha_i\\), the variance
components or 'random-effects' estimator provides an asymptotically
efficient FGLS solution by estimating a transformed model
\\y\_{it}-\theta y\_{i.} = \beta_0 + \beta_1 (X\_{it} - \theta
X\_{i.}) + (u\_{it} - \theta u\_{i.}\\), where \\\theta = 1 -
\frac{\sigma\_\alpha}{\sqrt(\sigma^2\_\alpha + T \sigma^2\_\epsilon)}\\.
An estimate of \\\theta\\ can be obtained from the an estimate of
\\\hat{u}\_{it}\\ (the residuals from the pooled model). If
\\E\[\alpha_i\|X\_{it}\] \neq 0\\, pooled OLS is biased and
inconsistent, and taking \\\theta = 1\\ gives an unbiased and consistent
fixed-effects estimator of \\\beta_1\\. See Examples.

## Value

`fbetween`/`B` returns `x` with every element replaced by its
(groupwise) mean (`xi.`). Missing values are preserved if `fill = FALSE`
(the default). `fwithin/W` returns `x` where every element was
subtracted its (groupwise) mean (`x - theta * xi. + mean` or, if
`mean = "overall.mean"`, `x - theta * xi. + theta * x..`). See Details.

## References

Mundlak, Yair. 1978. On the Pooling of Time Series and Cross Section
Data. *Econometrica* 46 (1): 69-85.

## See also

[`fhdbetween/HDB and fhdwithin/HDW`](https://fastverse.org/collapse/reference/fhdbetween_fhdwithin.md),
[`fscale/STD`](https://fastverse.org/collapse/reference/fscale.md),
[`TRA`](https://fastverse.org/collapse/reference/TRA.md), [Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Simple centering and averaging
head(fbetween(mtcars))
#>                        mpg    cyl     disp       hp     drat      wt     qsec
#> Mazda RX4         20.09062 6.1875 230.7219 146.6875 3.596562 3.21725 17.84875
#> Mazda RX4 Wag     20.09062 6.1875 230.7219 146.6875 3.596562 3.21725 17.84875
#> Datsun 710        20.09062 6.1875 230.7219 146.6875 3.596562 3.21725 17.84875
#> Hornet 4 Drive    20.09062 6.1875 230.7219 146.6875 3.596562 3.21725 17.84875
#> Hornet Sportabout 20.09062 6.1875 230.7219 146.6875 3.596562 3.21725 17.84875
#> Valiant           20.09062 6.1875 230.7219 146.6875 3.596562 3.21725 17.84875
#>                       vs      am   gear   carb
#> Mazda RX4         0.4375 0.40625 3.6875 2.8125
#> Mazda RX4 Wag     0.4375 0.40625 3.6875 2.8125
#> Datsun 710        0.4375 0.40625 3.6875 2.8125
#> Hornet 4 Drive    0.4375 0.40625 3.6875 2.8125
#> Hornet Sportabout 0.4375 0.40625 3.6875 2.8125
#> Valiant           0.4375 0.40625 3.6875 2.8125
head(B(mtcars))
#>                      B.mpg  B.cyl   B.disp     B.hp   B.drat    B.wt   B.qsec
#> Mazda RX4         20.09062 6.1875 230.7219 146.6875 3.596562 3.21725 17.84875
#> Mazda RX4 Wag     20.09062 6.1875 230.7219 146.6875 3.596562 3.21725 17.84875
#> Datsun 710        20.09062 6.1875 230.7219 146.6875 3.596562 3.21725 17.84875
#> Hornet 4 Drive    20.09062 6.1875 230.7219 146.6875 3.596562 3.21725 17.84875
#> Hornet Sportabout 20.09062 6.1875 230.7219 146.6875 3.596562 3.21725 17.84875
#> Valiant           20.09062 6.1875 230.7219 146.6875 3.596562 3.21725 17.84875
#>                     B.vs    B.am B.gear B.carb
#> Mazda RX4         0.4375 0.40625 3.6875 2.8125
#> Mazda RX4 Wag     0.4375 0.40625 3.6875 2.8125
#> Datsun 710        0.4375 0.40625 3.6875 2.8125
#> Hornet 4 Drive    0.4375 0.40625 3.6875 2.8125
#> Hornet Sportabout 0.4375 0.40625 3.6875 2.8125
#> Valiant           0.4375 0.40625 3.6875 2.8125
head(fwithin(mtcars))
#>                         mpg     cyl        disp       hp       drat       wt
#> Mazda RX4          0.909375 -0.1875  -70.721875 -36.6875  0.3034375 -0.59725
#> Mazda RX4 Wag      0.909375 -0.1875  -70.721875 -36.6875  0.3034375 -0.34225
#> Datsun 710         2.709375 -2.1875 -122.721875 -53.6875  0.2534375 -0.89725
#> Hornet 4 Drive     1.309375 -0.1875   27.278125 -36.6875 -0.5165625 -0.00225
#> Hornet Sportabout -1.390625  1.8125  129.278125  28.3125 -0.4465625  0.22275
#> Valiant           -1.990625 -0.1875   -5.721875 -41.6875 -0.8365625  0.24275
#>                       qsec      vs       am    gear    carb
#> Mazda RX4         -1.38875 -0.4375  0.59375  0.3125  1.1875
#> Mazda RX4 Wag     -0.82875 -0.4375  0.59375  0.3125  1.1875
#> Datsun 710         0.76125  0.5625  0.59375  0.3125 -1.8125
#> Hornet 4 Drive     1.59125  0.5625 -0.40625 -0.6875 -1.8125
#> Hornet Sportabout -0.82875 -0.4375 -0.40625 -0.6875 -0.8125
#> Valiant            2.37125  0.5625 -0.40625 -0.6875 -1.8125
head(W(mtcars))
#>                       W.mpg   W.cyl      W.disp     W.hp     W.drat     W.wt
#> Mazda RX4          0.909375 -0.1875  -70.721875 -36.6875  0.3034375 -0.59725
#> Mazda RX4 Wag      0.909375 -0.1875  -70.721875 -36.6875  0.3034375 -0.34225
#> Datsun 710         2.709375 -2.1875 -122.721875 -53.6875  0.2534375 -0.89725
#> Hornet 4 Drive     1.309375 -0.1875   27.278125 -36.6875 -0.5165625 -0.00225
#> Hornet Sportabout -1.390625  1.8125  129.278125  28.3125 -0.4465625  0.22275
#> Valiant           -1.990625 -0.1875   -5.721875 -41.6875 -0.8365625  0.24275
#>                     W.qsec    W.vs     W.am  W.gear  W.carb
#> Mazda RX4         -1.38875 -0.4375  0.59375  0.3125  1.1875
#> Mazda RX4 Wag     -0.82875 -0.4375  0.59375  0.3125  1.1875
#> Datsun 710         0.76125  0.5625  0.59375  0.3125 -1.8125
#> Hornet 4 Drive     1.59125  0.5625 -0.40625 -0.6875 -1.8125
#> Hornet Sportabout -0.82875 -0.4375 -0.40625 -0.6875 -0.8125
#> Valiant            2.37125  0.5625 -0.40625 -0.6875 -1.8125
all.equal(fbetween(mtcars) + fwithin(mtcars), mtcars)
#> [1] TRUE

## Groupwise centering and averaging
head(fbetween(mtcars, mtcars$cyl))
#>                        mpg cyl     disp        hp     drat       wt     qsec
#> Mazda RX4         19.74286   6 183.3143 122.28571 3.585714 3.117143 17.97714
#> Mazda RX4 Wag     19.74286   6 183.3143 122.28571 3.585714 3.117143 17.97714
#> Datsun 710        26.66364   4 105.1364  82.63636 4.070909 2.285727 19.13727
#> Hornet 4 Drive    19.74286   6 183.3143 122.28571 3.585714 3.117143 17.97714
#> Hornet Sportabout 15.10000   8 353.1000 209.21429 3.229286 3.999214 16.77214
#> Valiant           19.74286   6 183.3143 122.28571 3.585714 3.117143 17.97714
#>                          vs        am     gear     carb
#> Mazda RX4         0.5714286 0.4285714 3.857143 3.428571
#> Mazda RX4 Wag     0.5714286 0.4285714 3.857143 3.428571
#> Datsun 710        0.9090909 0.7272727 4.090909 1.545455
#> Hornet 4 Drive    0.5714286 0.4285714 3.857143 3.428571
#> Hornet Sportabout 0.0000000 0.1428571 3.285714 3.500000
#> Valiant           0.5714286 0.4285714 3.857143 3.428571
head(fwithin(mtcars, mtcars$cyl))
#>                         mpg cyl       disp        hp        drat          wt
#> Mazda RX4          1.257143   0 -23.314286 -12.28571  0.31428571 -0.49714286
#> Mazda RX4 Wag      1.257143   0 -23.314286 -12.28571  0.31428571 -0.24214286
#> Datsun 710        -3.863636   0   2.863636  10.36364 -0.22090909  0.03427273
#> Hornet 4 Drive     1.657143   0  74.685714 -12.28571 -0.50571429  0.09785714
#> Hornet Sportabout  3.600000   0   6.900000 -34.21429 -0.07928571 -0.55921429
#> Valiant           -1.642857   0  41.685714 -17.28571 -0.82571429  0.34285714
#>                         qsec          vs         am        gear       carb
#> Mazda RX4         -1.5171429 -0.57142857  0.5714286  0.14285714  0.5714286
#> Mazda RX4 Wag     -0.9571429 -0.57142857  0.5714286  0.14285714  0.5714286
#> Datsun 710        -0.5272727  0.09090909  0.2727273 -0.09090909 -0.5454545
#> Hornet 4 Drive     1.4628571  0.42857143 -0.4285714 -0.85714286 -2.4285714
#> Hornet Sportabout  0.2478571  0.00000000 -0.1428571 -0.28571429 -1.5000000
#> Valiant            2.2428571  0.42857143 -0.4285714 -0.85714286 -2.4285714
all.equal(fbetween(mtcars, mtcars$cyl) + fwithin(mtcars, mtcars$cyl), mtcars)
#> [1] TRUE

head(W(wlddev, ~ iso3c, cols = 9:13))    # Center the 5 series in this dataset by country
#>   iso3c W.PCGDP  W.LIFEEX W.GINI       W.ODA    W.POP
#> 1   AFG      NA -16.75117     NA -1370778502 -9365285
#> 2   AFG      NA -16.23517     NA -1255468497 -9192848
#> 3   AFG      NA -15.72617     NA -1374708502 -9010817
#> 4   AFG      NA -15.22617     NA -1249828497 -8819053
#> 5   AFG      NA -14.73417     NA -1191628485 -8617477
#> 6   AFG      NA -14.24917     NA -1145708502 -8405938
head(cbind(get_vars(wlddev,"iso3c"),     # Same thing done manually using fwithin..
      add_stub(fwithin(get_vars(wlddev,9:13), wlddev$iso3c), "W.")))
#>   iso3c W.PCGDP  W.LIFEEX W.GINI       W.ODA    W.POP
#> 1   AFG      NA -16.75117     NA -1370778502 -9365285
#> 2   AFG      NA -16.23517     NA -1255468497 -9192848
#> 3   AFG      NA -15.72617     NA -1374708502 -9010817
#> 4   AFG      NA -15.22617     NA -1249828497 -8819053
#> 5   AFG      NA -14.73417     NA -1191628485 -8617477
#> 6   AFG      NA -14.24917     NA -1145708502 -8405938

## Using B() and W() for fixed-effects regressions:

# Several ways of running the same regression with cyl-fixed effects
lm(W(mpg,cyl) ~ W(carb,cyl), data = mtcars)                     # Centering each individually
#> 
#> Call:
#> lm(formula = W(mpg, cyl) ~ W(carb, cyl), data = mtcars)
#> 
#> Coefficients:
#>  (Intercept)  W(carb, cyl)  
#>   -2.822e-16    -4.655e-01  
#> 
lm(mpg ~ carb, data = W(mtcars, ~ cyl, stub = FALSE))           # Centering the entire data
#> 
#> Call:
#> lm(formula = mpg ~ carb, data = W(mtcars, ~cyl, stub = FALSE))
#> 
#> Coefficients:
#> (Intercept)         carb  
#>  -2.822e-16   -4.655e-01  
#> 
lm(mpg ~ carb, data = W(mtcars, ~ cyl, stub = FALSE,            # Here only the intercept changes
                        mean = "overall.mean"))
#> 
#> Call:
#> lm(formula = mpg ~ carb, data = W(mtcars, ~cyl, stub = FALSE, 
#>     mean = "overall.mean"))
#> 
#> Coefficients:
#> (Intercept)         carb  
#>     21.3999      -0.4655  
#> 
lm(mpg ~ carb + B(carb,cyl), data = mtcars)                     # Procedure suggested by
#> 
#> Call:
#> lm(formula = mpg ~ carb + B(carb, cyl), data = mtcars)
#> 
#> Coefficients:
#>  (Intercept)          carb  B(carb, cyl)  
#>      34.8297       -0.4655       -4.7750  
#> 
# ..Mundlak (1978) - partialling out group averages amounts to the same as demeaning the data
plm::plm(mpg ~ carb, mtcars, index = "cyl", model = "within")   # "Proof"..
#> 
#> Model Formula: mpg ~ carb
#> <environment: 0x1417bd120>
#> 
#> Coefficients:
#>     carb 
#> -0.46551 
#> 

# This takes the interaction of cyl, vs and am as fixed effects
lm(W(mpg) ~ W(carb), data = iby(mtcars, id = finteraction(cyl, vs, am)))
#> 
#> Call:
#> lm(formula = W(mpg) ~ W(carb), data = iby(mtcars, id = finteraction(cyl, 
#>     vs, am)))
#> 
#> Coefficients:
#> (Intercept)      W(carb)  
#>  -1.306e-15   -9.413e-01  
#> 
lm(mpg ~ carb, data = W(mtcars, ~ cyl + vs + am, stub = FALSE))
#> 
#> Call:
#> lm(formula = mpg ~ carb, data = W(mtcars, ~cyl + vs + am, stub = FALSE))
#> 
#> Coefficients:
#> (Intercept)         carb  
#>  -1.306e-15   -9.413e-01  
#> 
lm(mpg ~ carb + B(carb,list(cyl,vs,am)), data = mtcars)
#> 
#> Call:
#> lm(formula = mpg ~ carb + B(carb, list(cyl, vs, am)), data = mtcars)
#> 
#> Coefficients:
#>                (Intercept)                        carb  
#>                    27.8168                     -0.9413  
#> B(carb, list(cyl, vs, am))  
#>                    -1.8057  
#> 

# Now with cyl fixed effects weighted by hp:
lm(W(mpg,cyl,hp) ~ W(carb,cyl,hp), data = mtcars)
#> 
#> Call:
#> lm(formula = W(mpg, cyl, hp) ~ W(carb, cyl, hp), data = mtcars)
#> 
#> Coefficients:
#>      (Intercept)  W(carb, cyl, hp)  
#>           0.1747           -0.4469  
#> 
lm(mpg ~ carb, data = W(mtcars, ~ cyl, ~ hp, stub = FALSE))
#> 
#> Call:
#> lm(formula = mpg ~ carb, data = W(mtcars, ~cyl, ~hp, stub = FALSE))
#> 
#> Coefficients:
#> (Intercept)         carb  
#>      0.1747      -0.4469  
#> 
lm(mpg ~ carb + B(carb,cyl,hp), data = mtcars)       # WRONG ! Gives a different coefficient!!
#> 
#> Call:
#> lm(formula = mpg ~ carb + B(carb, cyl, hp), data = mtcars)
#> 
#> Coefficients:
#>      (Intercept)              carb  B(carb, cyl, hp)  
#>          34.1833           -0.4383           -4.2638  
#> 

## Manual variance components (random-effects) estimation
res <- HDW(mtcars, mpg ~ carb)[[1]]  # Get residuals from pooled OLS
sig2_u <- fvar(res)
sig2_e <- fvar(fwithin(res, mtcars$cyl))
T <- length(res) / fndistinct(mtcars$cyl)
sig2_alpha <- sig2_u - sig2_e
theta <- 1 - sqrt(sig2_alpha) / sqrt(sig2_alpha + T * sig2_e)
lm(mpg ~ carb, data = W(mtcars, ~ cyl, theta = theta, mean = "overall.mean", stub = FALSE))
#> 
#> Call:
#> lm(formula = mpg ~ carb, data = W(mtcars, ~cyl, theta = theta, 
#>     mean = "overall.mean", stub = FALSE))
#> 
#> Coefficients:
#> (Intercept)         carb  
#>     21.8727      -0.6336  
#> 

# A slightly different method to obtain theta...
plm::plm(mpg ~ carb, mtcars, index = "cyl", model = "random")
#> 
#> Model Formula: mpg ~ carb
#> <environment: 0x1417bd120>
#> 
#> Coefficients:
#> (Intercept)        carb 
#>    22.40631    -0.68522 
#> 
```
