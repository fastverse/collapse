# Fast (Weighted) Sample Quantiles and Range

A faster alternative to
[`quantile`](https://rdrr.io/r/stats/quantile.html) (written fully in
C), that supports sampling weights, and can also quickly compute
quantiles from an ordering vector (e.g. `order(x)`). `frange` provides a
fast alternative to [`range`](https://rdrr.io/r/base/range.html).

## Usage

``` r
fquantile(x, probs = c(0, 0.25, 0.5, 0.75, 1), w = NULL,
          o = if(length(x) > 1e5L && length(probs) > log(length(x)))
              radixorder(x) else NULL,
          na.rm = .op[["na.rm"]], type = 7L, names = TRUE,
          check.o = is.null(attr(o, "sorted")))

# Programmers version: no names, intelligent defaults, or checks
.quantile(x, probs = c(0, 0.25, 0.5, 0.75, 1), w = NULL, o = NULL,
          na.rm = TRUE, type = 7L, names = FALSE, check.o = FALSE)

# Fast range (min and max)
frange(x, na.rm = .op[["na.rm"]], finite = FALSE)
.range(x, na.rm = TRUE, finite = FALSE)
```

## Arguments

- x:

  a numeric or integer vector.

- probs:

  numeric vector of probabilities with values in \[0,1\].

- w:

  a numeric vector of strictly positive sampling weights. Missing
  weights are only supported if `x` is also missing.

- o:

  integer. An vector giving the ordering of the elements in `x`, such
  that `identical(x[o], sort(x))`. If available this considerably speeds
  up the estimation.

- na.rm:

  logical. Remove missing values, default `TRUE`.

- finite:

  logical. Omit all non-finite values.

- type:

  integer. Quantile types 4-9. See
  [`quantile`](https://rdrr.io/r/stats/quantile.html). Further details
  are provided in [Hyndman and Fan
  (1996)](https://www.tandfonline.com/doi/abs/10.1080/00031305.1996.10473566)
  who recommended type 8. The default method is type 7.

- names:

  logical. Generates names of the form
  `paste0(round(probs * 100, 1), "%")` (in C). Set to `FALSE` for
  speedup.

- check.o:

  logical. If `o` is supplied, `TRUE` runs through `o` once and checks
  that it is valid, i.e. that each element is in `[1, length(x)]`. Set
  to `FALSE` for significant speedup if `o` is known to be valid.

## Details

`fquantile` is implemented using a quickselect algorithm in C, inspired
by *data.table*'s `gmedian`. The algorithm is applied incrementally to
different sections of the array to find individual quantiles. If many
quantile probabilities are requested, sorting the whole array with the
fast
[`radixorder`](https://fastverse.org/collapse/reference/radixorder.md)
algorithm is more efficient. The default threshold for this
(`length(x) > 1e5L && length(probs) > log(length(x))`) is conservative,
given that quickselect is generally more efficient on longitudinal data
with similar values repeated by groups. With random data, my
investigations yield that a threshold of
`length(probs) > log10(length(x))` would be more appropriate.

`frange` is considerably more efficient than
[`range`](https://rdrr.io/r/base/range.html), requiring only one pass
through the data instead of two. For probabilities 0 and 1, `fquantile`
internally calls `frange`.

Following [Hyndman and Fan
(1996)](https://www.tandfonline.com/doi/abs/10.1080/00031305.1996.10473566),
the quantile type-\\i\\ quantile function of the sample \\X\\ can be
written as a weighted average of two order statistics:

\$\$\hat{Q}\_{X,i}(p) = (1 - \gamma) X\_{(j)} + \gamma X\_{(j + 1)}\$\$

where \\j = \lfloor pn + m \rfloor,\\ m \in \mathbb{R}\\ and \\\gamma =
pn + m - j,\\ 0 \le \gamma \le 1\\, with \\m\\ differing by quantile
type (\\i\\). For example, the default type 7 quantile estimator uses
\\m = 1 - p\\, see [`quantile`](https://rdrr.io/r/stats/quantile.html).

For weighted data with normalized weights \\w = \\w_1, ..., w_n\\\\,
where \\w_k \> 0\\ and \\\sum_k w_k = 1\\, let \\\\w\_{(1)}, ...,
w\_{(n)}\\\\ be the weights for each order statistic and \\W\_{(k)} =
\operatorname{Weight}\[X_j \le X\_{(k)}\] = \sum\_{j=1}^k w\_{(j)}\\ the
cumulative weight for each order statistic.

We can then first find the largest value \\l\\ such that the cumulative
normalized weight \\W\_{(l)} \leq p\\, and replace \\pn\\ with \\l +
(p - W\_{(l)})/w\_{(l+1)}\\, where \\w\_{(l+1)}\\ is the weight of the
next observation. This gives:

\$\$j = \lfloor l + \frac{p - W\_{(l)}}{w\_{(l+1)}} + m \rfloor\$\$
\$\$\gamma = l + \frac{p - W\_{(l)}}{w\_{(l+1)}} + m - j\$\$

For a more detailed exposition [see these excellent
notes](https://htmlpreview.github.io/?https://github.com/mjskay/uncertainty-examples/blob/master/weighted-quantiles.html)
by Matthew Kay. See also the R implementation of weighted quantiles type
7 in the Examples below.

## Note

The new weighted quantile algorithm from v2.1.0 does not skip zero
weights anymore as this is technically very difficult (it is not clear
if \\j\\ hits a zero weight element whether one should move forward or
backward to find an alternative). Thus, all non-missing elements are
considered and weights should be strictly positive.

## Value

A vector of quantiles. If `names = TRUE`, `fquantile` generates names as
`paste0(round(probs * 100, 1), "%")` (in C).

## Author

Sebastian Krantz based on
[notes](https://htmlpreview.github.io/?https://github.com/mjskay/uncertainty-examples/blob/master/weighted-quantiles.html)
by Matthew Kay.

## References

Hyndman, R. J. and Fan, Y. (1996) Sample quantiles in statistical
packages, *American Statistician* 50, 361–365. doi:10.2307/2684934.

Wicklin, R. (2017) Sample quantiles: A comparison of 9 definitions; SAS
Blog.
https://blogs.sas.com/content/iml/2017/05/24/definitions-sample-quantiles.html

Wikipedia:
https://en.wikipedia.org/wiki/Quantile#Estimating_quantiles_from_a_sample

Weighted Quantiles by Matthew Kay:
https://htmlpreview.github.io/?https://github.com/mjskay/uncertainty-examples/blob/master/weighted-quantiles.html

## See also

[`fnth`](https://fastverse.org/collapse/reference/fnth_fmedian.md),
[Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Basic range and quantiles
frange(mtcars$mpg)
#> [1] 10.4 33.9
fquantile(mtcars$mpg)
#>     0%    25%    50%    75%   100% 
#> 10.400 15.425 19.200 22.800 33.900 

## Checking computational equivalence to stats::quantile()
w = alloc(abs(rnorm(1)), 32)
o = radixorder(mtcars$mpg)
for (i in 5:9) print(all_obj_equal(fquantile(mtcars$mpg, type = i),
                                   fquantile(mtcars$mpg, type = i, w = w),
                                   fquantile(mtcars$mpg, type = i, o = o),
                                   fquantile(mtcars$mpg, type = i, w = w, o = o),
                                    quantile(mtcars$mpg, type = i)))
#> [1] TRUE
#> [1] TRUE
#> [1] TRUE
#> [1] TRUE
#> [1] TRUE

## Demonstaration: weighted quantiles type 7 in R
wquantile7R <- function(x, w, probs = c(0.25, 0.5, 0.75), na.rm = TRUE, names = TRUE) {
  if(na.rm && anyNA(x)) {             # Removing missing values (only in x)
    cc = whichNA(x, invert = TRUE)    # The C code first calls radixorder(x), which places
    x = x[cc]; w = w[cc]              # missing values last, so removing = early termination
  }
  o = radixorder(x)                   # Ordering
  wo = proportions(w[o])
  Wo = cumsum(wo)                     # Cumulative sum
  res = sapply(probs, function(p) {
    l = which.max(Wo > p) - 1L        # Lower order statistic
    s = l + (p - Wo[l])/wo[l+1L] + 1 - p
    j = floor(s)
    gamma = s - j
    (1 - gamma) * x[o[j]] + gamma * x[o[j+1L]]  # Weighted quantile
  })
  if(names) names(res) = paste0(as.integer(probs * 100), "%")
  res
} # Note: doesn't work for min and max.

wquantile7R(mtcars$mpg, mtcars$wt)
#>      25%      50%      75% 
#> 15.07936 17.89174 21.40000 

all.equal(wquantile7R(mtcars$mpg, mtcars$wt),
          fquantile(mtcars$mpg, c(0.25, 0.5, 0.75), mtcars$wt))
#> [1] TRUE

## Efficient grouped quantile estimation: use .quantile for less call overhead
BY(mtcars$mpg, mtcars$cyl, .quantile, names = TRUE, expand.wide = TRUE)
#>     0%   25%  50%   75% 100%
#> 4 21.4 22.80 26.0 30.40 33.9
#> 6 17.8 18.65 19.7 21.00 21.4
#> 8 10.4 14.40 15.2 16.25 19.2
BY(mtcars, mtcars$cyl, .quantile, names = TRUE)
#>         mpg cyl   disp    hp  drat     wt  qsec vs  am gear carb
#> 4.0%   21.4   4  71.10  52.0 3.690 1.5130 16.70  0 0.0    3    1
#> 4.25%  22.8   4  78.85  65.5 3.810 1.8850 18.56  1 0.5    4    1
#> 4.50%  26.0   4 108.00  91.0 4.080 2.2000 18.90  1 1.0    4    2
#> 4.75%  30.4   4 120.65  96.0 4.165 2.6225 19.95  1 1.0    4    2
#> 4.100% 33.9   4 146.70 113.0 4.930 3.1900 22.90  1 1.0    5    2
#> 6.0%   17.8   6 145.00 105.0 2.760 2.6200 15.50  0 0.0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 9 rows ]
mtcars |> fgroup_by(cyl) |> BY(.quantile)
#>   cyl  mpg   disp    hp  drat     wt  qsec vs  am gear carb
#> 1   4 21.4  71.10  52.0 3.690 1.5130 16.70  0 0.0    3    1
#> 2   4 22.8  78.85  65.5 3.810 1.8850 18.56  1 0.5    4    1
#> 3   4 26.0 108.00  91.0 4.080 2.2000 18.90  1 1.0    4    2
#> 4   4 30.4 120.65  96.0 4.165 2.6225 19.95  1 1.0    4    2
#> 5   4 33.9 146.70 113.0 4.930 3.1900 22.90  1 1.0    5    2
#> 6   6 17.8 145.00 105.0 2.760 2.6200 15.50  0 0.0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 9 rows ]

## With weights
BY(mtcars$mpg, mtcars$cyl, .quantile, w = mtcars$wt, names = TRUE, expand.wide = TRUE)
#>     0%      25%      50%      75% 100%
#> 4 21.4 22.80000 24.53116 29.75889 33.9
#> 6 17.8 18.46561 19.55289 21.00000 21.4
#> 8 10.4 13.91543 15.15267 16.11036 19.2
BY(mtcars, mtcars$cyl, .quantile, w = mtcars$wt, names = TRUE)
#>             mpg cyl     disp        hp     drat       wt     qsec vs am gear
#> 4.0%   21.40000   4  71.1000  52.00000 3.690000 1.513000 16.70000  0  0    3
#> 4.25%  22.80000   4  80.2647  65.55695 3.783351 2.023886 18.60116  1  0    4
#> 4.50%  24.53116   4 119.7122  91.67897 3.996622 2.330844 19.25457  1  1    4
#> 4.75%  29.75889   4 126.2910  95.88316 4.109992 2.878872 20.00040  1  1    4
#> 4.100% 33.90000   4 146.7000 113.00000 4.930000 3.190000 22.90000  1  1    5
#> 6.0%   17.80000   6 145.0000 105.00000 2.760000 2.620000 15.50000  0  0    3
#>        carb
#> 4.0%      1
#> 4.25%     1
#> 4.50%     2
#> 4.75%     2
#> 4.100%    2
#> 6.0%      1
#>  [ reached 'max' / getOption("max.print") -- omitted 9 rows ]
mtcars |> fgroup_by(cyl) |> fselect(-wt) |> BY(.quantile, w = mtcars$wt)
#>   cyl      mpg     disp        hp     drat     qsec vs am     gear     carb
#> 1   4 21.40000  71.1000  52.00000 3.690000 16.70000  0  0 3.000000 1.000000
#> 2   4 22.80000  80.2647  65.55695 3.783351 18.60116  1  0 4.000000 1.000000
#> 3   4 24.53116 119.7122  91.67897 3.996622 19.25457  1  1 4.000000 2.000000
#> 4   4 29.75889 126.2910  95.88316 4.109992 20.00040  1  1 4.000000 2.000000
#> 5   4 33.90000 146.7000 113.00000 4.930000 22.90000  1  1 5.000000 2.000000
#> 6   6 17.80000 145.0000 105.00000 2.760000 15.50000  0  0 3.000000 1.000000
#> 7   6 18.46561 160.0000 110.00000 3.280086 16.89266  0  0 3.397399 2.192197
#>  [ reached 'max' / getOption("max.print") -- omitted 8 rows ]
mtcars |> fgroup_by(cyl) |> fsummarise(across(-wt, .quantile, w = wt))
#>   cyl      mpg     disp        hp     drat     qsec vs am     gear     carb
#> 1   4 21.40000  71.1000  52.00000 3.690000 16.70000  0  0 3.000000 1.000000
#> 2   4 22.80000  80.2647  65.55695 3.783351 18.60116  1  0 4.000000 1.000000
#> 3   4 24.53116 119.7122  91.67897 3.996622 19.25457  1  1 4.000000 2.000000
#> 4   4 29.75889 126.2910  95.88316 4.109992 20.00040  1  1 4.000000 2.000000
#> 5   4 33.90000 146.7000 113.00000 4.930000 22.90000  1  1 5.000000 2.000000
#> 6   6 17.80000 145.0000 105.00000 2.760000 15.50000  0  0 3.000000 1.000000
#> 7   6 18.46561 160.0000 110.00000 3.280086 16.89266  0  0 3.397399 2.192197
#>  [ reached 'max' / getOption("max.print") -- omitted 8 rows ]
```
