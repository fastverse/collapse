# Higher-Dimensional Centering and Linear Prediction

`fhdbetween` is a generalization of `fbetween` to efficiently predict
with multiple factors and linear models (i.e. predict with
vectors/factors, matrices, or data frames/lists where the latter may
contain multiple factor variables). Similarly, `fhdwithin` is a
generalization of `fwithin` to center on multiple factors and
partial-out linear models.

The corresponding operators `HDB` and `HDW` additionally allow to
predict / partial out full [`lm()`](https://rdrr.io/r/stats/lm.html)
formulas with interactions between variables.

## Usage

``` r
fhdbetween(x, ...)
 fhdwithin(x, ...)
       HDB(x, ...)
       HDW(x, ...)

# Default S3 method
fhdbetween(x, fl, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, lm.method = "qr", ...)
# Default S3 method
fhdwithin(x, fl, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, lm.method = "qr", ...)
# Default S3 method
HDB(x, fl, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, lm.method = "qr", ...)
# Default S3 method
HDW(x, fl, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, lm.method = "qr", ...)

# S3 method for class 'matrix'
fhdbetween(x, fl, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, lm.method = "qr", ...)
# S3 method for class 'matrix'
fhdwithin(x, fl, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, lm.method = "qr", ...)
# S3 method for class 'matrix'
HDB(x, fl, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, stub = .op[["stub"]],
    lm.method = "qr", ...)
# S3 method for class 'matrix'
HDW(x, fl, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE, stub = .op[["stub"]],
    lm.method = "qr", ...)

# S3 method for class 'data.frame'
fhdbetween(x, fl, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE,
           variable.wise = FALSE, lm.method = "qr", ...)
# S3 method for class 'data.frame'
fhdwithin(x, fl, w = NULL, na.rm = .op[["na.rm"]], fill = FALSE,
          variable.wise = FALSE, lm.method = "qr", ...)
# S3 method for class 'data.frame'
HDB(x, fl, w = NULL, cols = is.numeric, na.rm = .op[["na.rm"]], fill = FALSE,
    variable.wise = FALSE, stub = .op[["stub"]], lm.method = "qr", ...)
# S3 method for class 'data.frame'
HDW(x, fl, w = NULL, cols = is.numeric, na.rm = .op[["na.rm"]], fill = FALSE,
    variable.wise = FALSE, stub = .op[["stub"]], lm.method = "qr", ...)

# Methods for indexed data / compatibility with plm:

# S3 method for class 'pseries'
fhdbetween(x, effect = "all", w = NULL, na.rm = .op[["na.rm"]], fill = TRUE, ...)
# S3 method for class 'pseries'
fhdwithin(x, effect = "all", w = NULL, na.rm = .op[["na.rm"]], fill = TRUE, ...)
# S3 method for class 'pseries'
HDB(x, effect = "all", w = NULL, na.rm = .op[["na.rm"]], fill = TRUE, ...)
# S3 method for class 'pseries'
HDW(x, effect = "all", w = NULL, na.rm = .op[["na.rm"]], fill = TRUE, ...)

# S3 method for class 'pdata.frame'
fhdbetween(x, effect = "all", w = NULL, na.rm = .op[["na.rm"]], fill = TRUE,
           variable.wise = TRUE, ...)
# S3 method for class 'pdata.frame'
fhdwithin(x, effect = "all", w = NULL, na.rm = .op[["na.rm"]], fill = TRUE,
          variable.wise = TRUE, ...)
# S3 method for class 'pdata.frame'
HDB(x, effect = "all", w = NULL, cols = is.numeric, na.rm = .op[["na.rm"]],
    fill = TRUE, variable.wise = TRUE, stub = .op[["stub"]], ...)
# S3 method for class 'pdata.frame'
HDW(x, effect = "all", w = NULL, cols = is.numeric, na.rm = .op[["na.rm"]],
    fill = TRUE, variable.wise = TRUE, stub = .op[["stub"]], ...)
```

## Arguments

- x:

  a numeric vector, matrix, data frame, 'indexed_series' ('pseries') or
  'indexed_frame' ('pdata.frame').

- fl:

  a numeric vector, factor, matrix, data frame or list (which may or may
  not contain factors). In the `HDW/HDB` data frame method `fl` can also
  be a one-or two sided [`lm()`](https://rdrr.io/r/stats/lm.html)
  formula with variables contained in `x`. Interactions `(:)` and full
  interactions `(*)` are supported. See Examples and the Note.

- w:

  a vector of (non-negative) weights.

- cols:

  *data.frame methods*: Select columns to center (partial-out) or
  predict using column names, indices, a logical vector or a function.
  Unless specified otherwise all numeric columns are selected. If
  `NULL`, all columns are selected.

- na.rm:

  remove missing values from both `x` and `fl`. by default rows with
  missing values in `x` or `fl` are removed. In that case an attribute
  "na.rm" is attached containing the rows removed.

- fill:

  If `na.rm = TRUE`, `fill = TRUE` will not remove rows with missing
  values in `x` or `fl`, but fill them with `NA`'s.

- variable.wise:

  *(p)data.frame methods*: Setting `variable.wise = TRUE` will process
  each column individually i.e. use all non-missing cases in each column
  and in `fl` (`fl` is only checked for missing values if
  `na.rm = TRUE`). This is a lot less efficient but uses all data
  available in each column.

- effect:

  *plm* methods: Select which panel identifiers should be used for
  centering. 1L takes the first variable in the
  [index](https://fastverse.org/collapse/reference/indexing.md), 2L the
  second etc.. Index variables can also be called by name using a
  character vector. The keyword `"all"` uses all identifiers.

- stub:

  character. A prefix/stub to add to the names of all transformed
  columns. `TRUE` (default) uses `"HDW."/"HDB."`, `FALSE` will not
  rename columns.

- lm.method:

  character. The linear fitting method. Supported are `"chol"` and
  `"qr"`. See [`flm`](https://fastverse.org/collapse/reference/flm.md).

- ...:

  further arguments passed to
  [`fixest::demean`](https://lrberge.github.io/fixest/reference/demean.html)
  (other than `notes` and `im_confident`) and
  [`chol`](https://rdrr.io/r/base/chol.html) /
  [`qr`](https://rdrr.io/r/base/qr.html). Possible choices are `tol` to
  set a uniform numerical tolerance for the entire fitting process, or
  `nthreads` and `iter` to govern the higher-order centering process.

## Details

`fhdbetween/HDB` and `fhdwithin/HDW` are powerful functions for
high-dimensional linear prediction problems involving large factors and
datasets, but can just as well handle ordinary regression problems. They
are implemented as efficient wrappers around
[`fbetween / fwithin`](https://fastverse.org/collapse/reference/fbetween_fwithin.md),
[`flm`](https://fastverse.org/collapse/reference/flm.md) and some C++
code from the `fixest` package that is imported for higher-order
centering tasks (thus `fixest` needs to be installed for problems
involving more than one factor).

Intended areas of use are to efficiently obtain residuals and predicted
values from data, and to prepare data for complex linear models
involving multiple levels of fixed effects. Such models can now be
fitted using `(g)lm()` on data prepared with `fhdwithin / HDW` (relying
on bootstrapped SE's for inference, or implementing the appropriate
corrections). See Examples.

If `fl` is a vector or matrix, the result are identical to `lm` i.e.
`fhdbetween / HDB` returns `fitted(lm(x ~ fl))` and `fhdwithin / HDW`
`residuals(lm(x ~ fl))`. If `fl` is a list containing factors, all
variables in `x` and non-factor variables in `fl` are centered on these
factors using either
[`fbetween / fwithin`](https://fastverse.org/collapse/reference/fbetween_fwithin.md)
for a single factor or `fixest` C++ code for multiple factors.
Afterwards the centered data is regressed on the centered predictors. If
`fl` is just a list of factors, `fhdwithin/HDW` returns the centered
data and `fhdbetween/HDB` the corresponding means. Take as a most
general example a list `fl = list(fct1, fct2, ..., var1, var2, ...)`
where `fcti` are factors and `vari` are continuous variables. The output
of `fhdwithin/HDW | fhdbetween/HDB` will then be identical to calling
`resid | fitted` on `lm(x ~ fct1 + fct2 + ... + var1 + var2 + ...)`. The
computations performed by `fhdwithin/HDW` and `fhdbetween/HDB` are
however much faster and more memory efficient than `lm` because factors
are not passed to
[`model.matrix`](https://rdrr.io/r/stats/model.matrix.html) and expanded
to matrices of dummies but projected out beforehand.

The formula interface to the data.frame method (only supported by the
operators `HDW | HDB`) provides ease of use and allows for additional
modeling complexity. For example it is possible to project out formulas
like
`HDW(data, ~ fct1*var1 + fct2:fct3 + var2:fct2:fct3 + var2:var3 + poly(var5,3)*fct5)`
containing simple `(:)` or full `(*)` interactions of factors with
continuous variables or polynomials of continuous variables, and two-or
three-way interactions of factors and continuous variables. If the
formula is one-sided as in the example above (the space left of `(~)` is
left empty), the formula is applied to all variables selected through
`cols`. The specification provided in `cols` (default: all numeric
variables not used in the formula) can be overridden by supplying one-or
more dependent variables. For example
`HDW(data, var1 + var2 ~ fct1 + fct2)` will return a data.frame with
`var1` and `var2` centered on `fct1` and `fct2`.

The special methods for 'indexed_series'
([`plm::pseries`](https://rdrr.io/pkg/plm/man/pseries.html)) and
'indexed_frame's
([`plm::pdata.frame`](https://rdrr.io/pkg/plm/man/pdata.frame.html))
center a panel series or variables in a panel data frame on all
panel-identifiers. By default in these methods `fill = TRUE` and
`variable.wise = TRUE`, so missing values are kept. This change in the
default arguments was done to ensure a coherent framework of functions
and operators applied to *plm* panel data classes.

## Note

### On the differences between `fhdwithin/HDW`... and `fwithin/W`...:

- `fhdwithin/HDW` can center data on multiple factors and also partial
  out continuous variables and factor-continuous interactions while
  `fwithin/W` only centers on one factor or the interaction of a set of
  factors, and does that very efficiently.

- `HDW(data, ~ qF(group1) + qF(group2))` simultaneously centers numeric
  variables in data on `group1` and `group2`, while
  `W(data, ~ group1 + group2)` centers data on the interaction of
  `group1` and `group2`. The equivalent operation in `HDW` would be:
  `HDW(data, ~ qF(group1):qF(group2))`.

- `W` always does computations on the variable-wise complete
  observations (in both matrices and data frames), whereas by default
  `HDW` removes all cases missing in either `x` or `fl`. In short,
  `W(data, ~ group1 + group2)` is actually equivalent to
  `HDW(data, ~ qF(group1):qF(group2), variable.wise = TRUE)`.
  `HDW(data, ~ qF(group1):qF(group2))` would remove any missing cases.

- `fbetween/B` and `fwithin/W` have options to fill missing cases using
  group-averages and to add the overall mean back to group-demeaned
  data. These options are not available in `fhdbetween/HDB` and
  `fhdwithin/HDW`. Since `HDB` and `HDW` by default remove missing
  cases, they also don't have options to keep grouping-columns as in `B`
  and `W`.

## Value

`HDB` returns fitted values of regressing `x` on `fl`. `HDW` returns
residuals. See Details and Examples.

## See also

[`fbetween, fwithin`](https://fastverse.org/collapse/reference/fbetween_fwithin.md),
[`fscale`](https://fastverse.org/collapse/reference/fscale.md),
[`TRA`](https://fastverse.org/collapse/reference/TRA.md),
[`flm`](https://fastverse.org/collapse/reference/flm.md),
[`fFtest`](https://fastverse.org/collapse/reference/fFtest.md), [Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
HDW(mtcars$mpg, mtcars$carb)                   # Simple regression problems
#>  [1]  3.3505410  3.3505410 -1.0166151 -2.4166151 -3.0608964 -5.7166151
#>  [7] -3.3494590  2.6391036  1.0391036  1.5505410  0.1505410 -3.3051777
#> [13] -2.4051777 -4.5051777 -7.2494590 -7.2494590 -2.9494590  8.5833849
#> [19]  8.6391036 10.0833849 -2.3166151 -6.2608964 -6.5608964 -4.3494590
#> [25] -2.5608964  3.4833849  4.2391036  8.6391036 -1.8494590  6.1619784
#> [31]  5.5734158 -0.3608964
HDW(mtcars$mpg, mtcars[-1])
#>  [1] -1.599505761 -1.111886079 -3.450644085  0.162595453  1.006565971
#>  [6] -2.283039036 -0.086256253  1.903988115 -1.619089898  0.500970058
#> [11] -1.391654392  2.227837890  1.700426404 -0.542224699 -1.634013415
#> [16] -0.536437711  4.206370638  4.627094192  0.503261089  4.387630904
#> [21] -2.143103442 -1.443053221 -2.532181498 -0.006021976  2.508321011
#> [26] -0.993468693 -0.152953961  2.763727417 -3.070040803  0.006171846
#> [31]  1.058881618 -2.968267683
HDW(mtcars$mpg, qM(mtcars[-1]))
#>  [1] -1.599505761 -1.111886079 -3.450644085  0.162595453  1.006565971
#>  [6] -2.283039036 -0.086256253  1.903988115 -1.619089898  0.500970058
#> [11] -1.391654392  2.227837890  1.700426404 -0.542224699 -1.634013415
#> [16] -0.536437711  4.206370638  4.627094192  0.503261089  4.387630904
#> [21] -2.143103442 -1.443053221 -2.532181498 -0.006021976  2.508321011
#> [26] -0.993468693 -0.152953961  2.763727417 -3.070040803  0.006171846
#> [31]  1.058881618 -2.968267683
head(HDW(qM(mtcars[3:4]), mtcars[1:2]))
#>                     HDW.disp     HDW.hp
#> Mazda RX4         -56.791929 -29.668202
#> Mazda RX4 Wag     -56.791929 -29.668202
#> Datsun 710         -7.001038   6.283636
#> Hornet 4 Drive     43.577448 -28.558294
#> Hornet Sportabout  38.455459 -19.007424
#> Valiant            -8.969914 -42.715033
head(HDW(iris[1:2], iris[3:4]))                # Partialling columns 3 and 4 out of columns 1 and 2
#>   HDW.Sepal.Length HDW.Sepal.Width
#> 1       0.21483967       0.2001352
#> 2       0.01483967      -0.2998648
#> 3      -0.13098262      -0.1255786
#> 4      -0.33933805      -0.1741510
#> 5       0.11483967       0.3001352
#> 6       0.41621663       0.6044681
head(HDW(iris[1:2], iris[3:5]))                # Adding the Species factor -> fixed effect
#>   HDW.Sepal.Length HDW.Sepal.Width
#> 1       0.14989286       0.1102684
#> 2      -0.05010714      -0.3897316
#> 3      -0.15951256      -0.1742640
#> 4      -0.44070173      -0.3051992
#> 5       0.04989286       0.2102684
#> 6       0.17930818       0.3391766

head(HDW(wlddev, PCGDP + LIFEEX ~ iso3c + qF(year))) # Partialling out 2 fixed effects
#>   HDW.PCGDP HDW.LIFEEX
#> 1 1578.6211 -1.3980224
#> 2 1412.8849 -1.1838196
#> 3  917.2033 -1.0547978
#> 4  627.8605 -0.8296048
#> 5  168.0458 -0.6683027
#> 6 -234.9535 -0.4708428
head(HDW(wlddev, PCGDP + LIFEEX ~ iso3c + qF(year), variable.wise = TRUE)) # Variable-wise
#>   HDW.PCGDP HDW.LIFEEX
#> 1        NA  -6.706423
#> 2        NA  -6.688440
#> 3        NA  -6.562210
#> 4        NA  -6.472079
#> 5        NA  -6.445378
#> 6        NA  -6.367659
head(HDW(wlddev, PCGDP + LIFEEX ~ iso3c + qF(year) + ODA)) # Adding ODA as a continuous regressor
#>   HDW.PCGDP HDW.LIFEEX
#> 1 -324.3991 -1.1765307
#> 2 -439.5404 -0.9751559
#> 3 -598.9266 -0.7835446
#> 4  100.2175 -0.6186010
#> 5  -70.7664 -0.4966332
#> 6  330.3561 -0.2257800
head(HDW(wlddev, PCGDP + LIFEEX ~ iso3c:qF(decade) + qF(year) + ODA)) # Country-decade and year FE's
#>    HDW.PCGDP  HDW.LIFEEX
#> 1  411.79228 -0.55122290
#> 2  231.95880 -0.36367639
#> 3  -73.18195 -0.20459213
#> 4   43.93176 -0.05394933
#> 5 -136.49858  0.06637048
#> 6 -151.30884  0.24440305

head(HDW(wlddev, PCGDP + LIFEEX ~ iso3c*year))          # Country specific time trends
#>    HDW.PCGDP    HDW.LIFEEX
#> 1  -3.035801 -0.1540994153
#> 2  -7.963841 -0.1544275886
#> 3 -35.533424 -0.1407557620
#> 4 -29.220766 -0.1100839354
#> 5 -38.876368 -0.0614121087
#> 6 -16.317261  0.0002597179
head(HDW(wlddev, PCGDP + LIFEEX ~ iso3c*poly(year, 3))) # Country specific cubic trends
#>    HDW.PCGDP   HDW.LIFEEX
#> 1   8.885334  0.023614035
#> 2  13.685446  0.006724458
#> 3 -10.597857 -0.011405315
#> 4  -6.279492 -0.023928535
#> 5 -22.048660 -0.025998452
#> 6  -8.561088 -0.018768318

# More complex examples
lm(HDW.mpg ~ HDW.hp, data = HDW(mtcars, ~ factor(cyl)*carb + vs + wt:gear + wt:gear:carb))
#> 
#> Call:
#> lm(formula = HDW.mpg ~ HDW.hp, data = HDW(mtcars, ~factor(cyl) * 
#>     carb + vs + wt:gear + wt:gear:carb))
#> 
#> Coefficients:
#> (Intercept)       HDW.hp  
#>  -1.731e-15   -3.265e-02  
#> 
lm(mpg ~ hp + factor(cyl)*carb + vs + wt:gear + wt:gear:carb, data = mtcars)
#> 
#> Call:
#> lm(formula = mpg ~ hp + factor(cyl) * carb + vs + wt:gear + wt:gear:carb, 
#>     data = mtcars)
#> 
#> Coefficients:
#>       (Intercept)                 hp       factor(cyl)6       factor(cyl)8  
#>          42.11872           -0.02366           -3.70912           -3.80071  
#>              carb                 vs  factor(cyl)6:carb  factor(cyl)8:carb  
#>          -1.64558           -0.81529           -0.82919           -1.56964  
#>           wt:gear       carb:wt:gear  
#>          -1.52766            0.26438  
#> 

lm(HDW.mpg ~ HDW.hp, data = HDW(mtcars, ~ factor(cyl)*carb + vs + wt:gear))
#> 
#> Call:
#> lm(formula = HDW.mpg ~ HDW.hp, data = HDW(mtcars, ~factor(cyl) * 
#>     carb + vs + wt:gear))
#> 
#> Coefficients:
#> (Intercept)       HDW.hp  
#>  -1.731e-15   -3.265e-02  
#> 
lm(mpg ~ hp + factor(cyl)*carb + vs + wt:gear, data = mtcars)
#> 
#> Call:
#> lm(formula = mpg ~ hp + factor(cyl) * carb + vs + wt:gear, data = mtcars)
#> 
#> Coefficients:
#>       (Intercept)                 hp       factor(cyl)6       factor(cyl)8  
#>           36.4543            -0.0274            -6.2463            -9.4541  
#>              carb                 vs  factor(cyl)6:carb  factor(cyl)8:carb  
#>            0.2508            -0.3227             0.5897             1.0374  
#>           wt:gear  
#>           -0.8238  
#> 

lm(HDW.mpg ~ HDW.hp, data = HDW(mtcars, ~ cyl*carb + vs + wt:gear))
#> 
#> Call:
#> lm(formula = HDW.mpg ~ HDW.hp, data = HDW(mtcars, ~cyl * carb + 
#>     vs + wt:gear))
#> 
#> Coefficients:
#> (Intercept)       HDW.hp  
#>  -3.898e-17   -2.151e-02  
#> 
lm(mpg ~ hp + cyl*carb + vs + wt:gear, data = mtcars)
#> 
#> Call:
#> lm(formula = mpg ~ hp + cyl * carb + vs + wt:gear, data = mtcars)
#> 
#> Coefficients:
#> (Intercept)           hp          cyl         carb           vs     cyl:carb  
#>    48.42617     -0.02151     -2.80751     -1.72418     -1.03254      0.36051  
#>     wt:gear  
#>    -0.81296  
#> 

lm(HDW.mpg ~ HDW.hp, data = HDW(mtcars, mpg + hp ~ cyl*carb + factor(cyl)*poly(drat,2)))
#> 
#> Call:
#> lm(formula = HDW.mpg ~ HDW.hp, data = HDW(mtcars, mpg + hp ~ 
#>     cyl * carb + factor(cyl) * poly(drat, 2)))
#> 
#> Coefficients:
#> (Intercept)       HDW.hp  
#>  -1.476e-15   -4.725e-02  
#> 
lm(mpg ~ hp + cyl*carb + factor(cyl)*poly(drat,2), data = mtcars)
#> 
#> Call:
#> lm(formula = mpg ~ hp + cyl * carb + factor(cyl) * poly(drat, 
#>     2), data = mtcars)
#> 
#> Coefficients:
#>                 (Intercept)                           hp  
#>                    29.87184                     -0.06227  
#>                         cyl                         carb  
#>                    -0.32237                     -2.19559  
#>                factor(cyl)6                 factor(cyl)8  
#>                    -1.60109                           NA  
#>              poly(drat, 2)1               poly(drat, 2)2  
#>                    27.84148                     -8.41291  
#>                    cyl:carb  factor(cyl)6:poly(drat, 2)1  
#>                     0.35323                    -49.59226  
#> factor(cyl)8:poly(drat, 2)1  factor(cyl)6:poly(drat, 2)2  
#>                   -18.35266                    -18.70972  
#> factor(cyl)8:poly(drat, 2)2  
#>                    -0.56842  
#> 
```
