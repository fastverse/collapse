# Fast Statistical Functions and data transformations

## Table of contents
- [The 14 Fast Statistical Functions](#the-14-fast-statistical-functions)
- [The `g` argument: what to pass](#the-g-argument-what-to-pass)
- [Weights (`w`)](#weights-w)
- [The `TRA` argument](#the-tra-argument)
- [`TRA()` and `setTRA()` standalone](#tra-and-settra-standalone)
- [Named transformation functions](#named-transformation-functions)
- [`fhdwithin` / `fhdbetween` — multi-factor centering and partialling out](#fhdwithin--fhdbetween)
- [Apply-style: `dapply` and `BY`](#apply-style-dapply-and-by)
- [Row/column arithmetic operators](#rowcolumn-arithmetic-operators)
- [Other statistical helpers](#other-statistical-helpers)

## The 14 Fast Statistical Functions

`fsum fprod fmean fmedian fmode fvar fsd fmin fmax fnth ffirst flast fnobs fndistinct`

They share one signature (the macro `.FAST_STAT_FUN` holds the list):

```r
f<stat>(x, g = NULL, w = NULL, TRA = NULL, na.rm = TRUE,
        use.g.names = TRUE, drop = TRUE, nthreads = 1, ...)
```

`x` may be an atomic vector, a matrix, a data.frame/data.table/tibble/sf, or a
`grouped_df`. The `grouped_df` method takes no `g` (it uses the attached grouping)
and adds `keep.group_vars = TRUE`, `keep.w = TRUE`.

Function-specific extras:

| Function | Extra arguments |
|---|---|
| `fnth` | `n = 0.5` (proportion) or an integer element; `ties = "mean"/"min"/"max"`; `o`/`check.o` for a pre-computed order |
| `fmedian` | `ties` — wrapper around `fnth(n = 0.5)` |
| `fmode` | `ties = "first"/"min"/"max"/"last"`; weighted mode picks the highest-weight value |
| `fvar`, `fsd` | `stable.algo = TRUE` (Welford). `FALSE` is a fast one-pass formula, risks catastrophic cancellation |
| `ffirst`, `flast` | `na.rm = TRUE` skips leading/trailing NAs |
| `fndistinct` | counts distinct values; `nthreads` |
| `fnobs` | counts non-missing |
| `fsum`, `fprod`, `fmean` | `nthreads` (OpenMP); `fsum(x, w=)` is a weighted/inner-product sum |

`use.g.names = TRUE` puts group names on the result (row names / vector names).
Abbreviate as `use = FALSE` when you don't need them — it saves real time on many
groups. `drop = TRUE` (matrix/data.frame) returns a plain named vector when
`g = NULL`.

```r
fmean(mtcars$mpg)                 # scalar
fmean(EuStockMarkets)             # named vector, one per column
fmean(mtcars)                     # named vector, one per column
fmean(mtcars, mtcars$cyl)         # data.frame: 3 rows x 11 columns
fmean(mtcars$mpg, mtcars$cyl, mtcars$wt)   # weighted group means
```

**`fsum` with weights is an inner product**, which is the trick behind fast
grouped regressions and covariances: `fsum(y, g, w = dmx)` = Σ y·dmx by group.

## The `g` argument: what to pass

Anything that identifies groups. In increasing order of setup cost and
information content:

| Passed | Behaviour |
|---|---|
| atomic vector | internally converted (respects `set_collapse(sort=)`) |
| list / data.frame of vectors | internally converted to a `GRP` object |
| factor | used directly, cheapest for simple functions |
| `qG` object | used directly, no names |
| `GRP` object | richest; required for best performance with `fmedian`, `fnth`, `fmode`, `fndistinct`, and `BY`/`gsplit` |

For repeated grouped work, build the grouping once: `g <- GRP(dat, c("id","year"))`.
See `references/programming.md` for how to choose. Note that a bare vector or list
in `g` is grouped according to the global `sort` option — pass
`qF(v, sort = TRUE)` if the output order must be sorted regardless.

## Weights (`w`)

`w` is a numeric vector of the same length as (rows of) `x`. Supported by `fsum`,
`fprod`, `fmean`, `fmedian`, `fnth`, `fmode`, `fvar`, `fsd`, and by `fscale`,
`fbetween`, `fwithin`, `fhdwithin`, `qsu`, `pwcor`, `collap`, `fquantile`.

```r
fmean(wlddev$PCGDP, wlddev$region, wlddev$POP)            # population-weighted mean
fsummarise(wlddev, across(PCGDP:GINI, fmean, w = POP))    # in a summarise
wlddev |> fgroup_by(region) |> fselect(PCGDP:GINI, POP) |> fmean(POP, keep.w = FALSE)
```

In the `grouped_df` method the first unnamed argument is `w`, and `keep.w`
controls whether the aggregated weight column is returned (`fsum` of the weights).

## The `TRA` argument

Sweeps the computed statistic back onto the original data, at original length.

| Int | String | Meaning |
|---|---|---|
| 0 | `"na"`, `"replace_na"` | replace only missing values in `x` with the statistic |
| 1 | `"fill"`, `"replace_fill"` | replace all values **including NA** |
| 2 | `"replace"` | replace values but **keep NAs as NA** |
| 3 | `"-"` | subtract (centre) |
| 4 | `"-+"` | subtract group statistic, add the overall (frequency-weighted) statistic |
| 5 | `"/"` | divide (proportion of group total) |
| 6 | `"%"` | percentage (divide × 100) |
| 7 | `"+"` | add |
| 8 | `"*"` | multiply |
| 9 | `"%%"` | modulus |
| 10 | `"-%%"` | subtract modulus (make divisible) |

```r
fsum(dat$v, list(dat$country, dat$year), TRA = "/")   # share of country-year total
fsum(m, TRA = "/")                                    # columns of a matrix -> proportions
fmean(x, g, TRA = "-+")                               # centre, preserving the grand mean
fmedian(iris$Sepal.Length, iris$Species, TRA = "fill", set = TRUE)   # in place, no copy
```

`set = TRUE` performs the transformation **by reference** and returns invisibly.
It works anywhere, including inside `fmutate`, with or without groups.

`"fill"` vs `"replace"` is a real distinction: `"fill"` also overwrites `NA`,
`"replace"` leaves `NA` as `NA`.

## `TRA()` and `setTRA()` standalone

```r
TRA(x, STATS, FUN = "-", g = NULL, set = FALSE)
setTRA(x, STATS, FUN = "-", g = NULL)     # invisible(TRA(..., set = TRUE))
```

Without `g` this is a much faster `sweep()`: `STATS` is a vector of length
`NCOL(x)` (or a 1-row matrix/list). With `g`, `STATS` must have one row per group,
in group order — exactly what `f<stat>(x, g)` returns. `fmean(x, g, TRA = "-")` is
shorthand for `TRA(x, fmean(x, g), "-", g)`.

## Named transformation functions

These are the common `TRA` cases with better names and extra options. Each has an
**operator** alias that additionally adds a column-name prefix and takes `by =`
/ `cols =` on data frames.

| Function | Operator | Does |
|---|---|---|
| `fbetween(x, g, w, fill = FALSE)` | `B` | group averages, expanded (`fill = TRUE` also fills NAs) |
| `fwithin(x, g, w, mean = 0, theta = 1)` | `W` | group-centre. `mean = "overall.mean"` re-adds the grand mean; `theta` gives quasi-demeaning for random effects |
| `fscale(x, g, w, mean = 0, sd = 1)` | `STD` | (grouped, weighted) standardize. `mean = FALSE` scales without centering; `mean = "overall.mean"` / `sd = "within.sd"` for panel scaling |
| `fhdbetween` / `fhdwithin` | `HDB` / `HDW` | higher-dimensional between/within — see below |
| `fcumsum(x, g, o, na.rm, fill)` | — | grouped, ordered cumulative sum |

```r
dat |> fmutate(dev  = fwithin(v, id),            # == fmean(v, id, TRA = "-")
               avg  = fbetween(v, id),           # == fmean(v, id, TRA = "fill")
               z    = fscale(v, id))
W(wlddev, ~ iso3c, cols = 9:12)                  # operator form, prefixes W.PCGDP etc.
STD(get_vars(wlddev, 9:12), wlddev$iso3c)
```

The operator versions honour `set_collapse(stub = FALSE)` to suppress prefixes.

## `fhdwithin` / `fhdbetween`

Centre on **several** factors at once (alternating projections / Method of
Alternating Projections), or partial out arbitrary regressors:

```r
fhdwithin(y, list(firm, year))          # two-way fixed effects residual
HDW(dat, ~ firm + year)                 # formula interface, prefixed names
fhdwithin(y, ~ x1 + x2 + factor(id))    # residuals from a regression
fhdbetween(y, fl)                       # the fitted part instead
```

Arguments: `fl` (factors, list, or formula), `w`, `fill`, `variable.wise`
(data.frame: use each column's own complete cases), `lm.method`. With more than
one factor collapse uses `fixest::demean` internals if available.

`flm(y, X, w, add.icpt, method)` fits a bare linear model very fast (6 solvers:
`"lm"`, `"solve"`, `"qr"`, `"arma"`, `"chol"`, `"eigen"`); `fFtest(y, exc, X, w)`
tests exclusion restrictions on models with large factors.

## Apply-style: `dapply` and `BY`

Use these when no Fast Statistical Function fits.

```r
dapply(X, FUN, ..., MARGIN = 2, parallel = FALSE, return = "same", drop = TRUE)
```
`dapply` applies a function **column-wise (or row-wise) preserving the object**:
attributes, class, labels all survive. It is the collapse answer to
`lapply(df, f) |> as.data.frame()` and to `apply()` on matrices.

```r
dapply(GGDC10S, log)                    # keeps the data.frame + variable labels
dapply(qM(mtcars), quantile, 0.28)      # row of quantiles, per column
GGDC10S |> ftransformv(6:16, `*`, 100/SUM)     # ftransformv is often what you actually want
```

```r
BY(x, g, FUN, ..., use.g.names = TRUE, sort = TRUE, expand.wide = FALSE)
```
`BY` is split-apply-combine over groups for arbitrary functions (internally
`gsplit()` + `lapply` + `unlist`, with attribute preservation). Use it only when
the function is not vectorizable; `f<stat>(x, g)` is orders of magnitude faster.

## Row/column arithmetic operators

Fast, copy-light sweeps for matrices and data frames:

```r
X %r+% v   X %r-% v   X %r*% v   X %r/% v   X %rr% v    # v is applied to each ROW
X %c+% V   X %c-% V   X %c*% V   X %c/% V   X %cr% V    # V is applied column-wise
```

`%r*%` multiplies every row of `X` element-wise by the vector `v` (length
`ncol(X)`); `%c*%` multiplies each column by `V` (length `nrow(X)`, or another
frame/matrix of the same shape).

**Use these whenever the left side is a data frame.** Base `/`, `*`, `+`, `-` on
a data frame dispatch through `Ops.data.frame`, which is slow; the collapse
operators go straight to C:

```r
get_vars(d, cols) / d$total * 100      # slow — Ops.data.frame
get_vars(d, cols) %c/% d$total * 100   # fast
settransformv(d, cols, `/`, total/100) # faster still — by reference, no copy
```
`mtcars %c*% 5` is much faster than `mtcars * 5` for the same reason. On plain
matrices ordinary arithmetic is already fine; this is a data-frame rule.

These do **not** obey operator precedence — they evaluate left to right, so
parenthesize in chains, and with `magrittr` pipes chain them deliberately:

```r
X_res <- get_vars(X, nam) %c-% fit                 # residuals
resid  <- (Res$x_sm %r*% Wx) %r+% Mx         # un-standardize a matrix
setop(Res$x_sm %r*% Wx, "+", Mx, rowwise = TRUE)   # same, by reference
```

## Other statistical helpers

- `fquantile(x, probs, w, o, na.rm, type = 7, names = TRUE)` — fast (weighted)
  quantiles; `.quantile` is a bare-bones version. `frange(x)` = `c(min, max)` in
  one pass.
- `fdist(x, y, method, nthreads)` — fast euclidean distance (vector-to-matrix or
  full distance matrix).
- `fcumsum(x, g, o, na.rm, fill)` — grouped/ordered cumulative sums.
- `pwcor` / `pwcov` / `pwnobs` — pairwise correlations/covariances with `N` and
  p-values, weighted, with a compact print method (`print(x, show = "lower.tri")`).
- `psacf` / `pspacf` / `psccf` — panel-data auto/partial/cross-correlations.

See `references/summaries-and-lists.md` for `qsu`, `descr`, `qtab`, `varying`.
