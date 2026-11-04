# Summary statistics, labels, and list processing

## Table of contents
- [`descr()` — describe a data frame](#descr--describe-a-data-frame)
- [`qsu()` — fast (grouped, panel) summary statistics](#qsu--fast-grouped-panel-summary-statistics)
- [`qtab()` — fast tabulation](#qtab--fast-tabulation)
- [`pwcor` / `pwcov` / `pwnobs`](#pwcor--pwcov--pwnobs)
- [`varying()`](#varying)
- [Variable labels and names](#variable-labels-and-names)
- [List processing](#list-processing)
- [Small helpers worth knowing](#small-helpers-worth-knowing)

## `descr()` — describe a data frame

```r
descr(X, by = NULL, w = NULL, cols = NULL, Ndistinct = TRUE, higher = TRUE,
      table = TRUE, sort.table = "freq",
      Qprobs = c(.01,.05,.1,.25,.5,.75,.9,.95,.99), Qtype = 7, label.attr = "label",
      stepwise = FALSE)
```

The fastest thorough look at a dataset: per column it gives class, N, missing,
distinct, mean/sd/min/max, skew/kurtosis, a quantile grid, and a frequency table
for categorical/low-cardinality columns. It respects variable labels.

```r
descr(wlddev)
descr(wlddev, by = ~ region)            # grouped
descr(dat, cols = is.numeric, table = FALSE, higher = FALSE)
print(descr(dat), n = 20, compact = TRUE)
as.data.frame(descr(dat))               # tidy it for export
```

## `qsu()` — fast (grouped, panel) summary statistics

```r
qsu(x, by = NULL, pid = NULL, w = NULL, cols = NULL, higher = FALSE,
    array = TRUE, labels = FALSE, stable.algo = TRUE)
```

`qsu` is `summary()` done properly for panels. `pid` (panel id) triggers a
**between/within decomposition**: overall, between-group, and within-group N,
mean, SD, min, max.

```r
qsu(wlddev, labels = TRUE)                        # N, Mean, SD, Min, Max per column
qsu(wlddev, by = ~ region)                        # by group
qsu(wlddev, pid = ~ iso3c, cols = 9:12)           # overall / between / within
qsu(GGDC10S, by = ~ Variable, pid = ~ Country, cols = 6:16, higher = TRUE)
```

With `by` and/or `pid` the result is a 3D/4D array; `aperm()` it into a readable
layout, or `as.data.frame(qsu_obj, gid = "Group")`. `higher = TRUE` adds skewness
and kurtosis.

## `qtab()` — fast tabulation

```r
qtab(..., w = NULL, wFUN = NULL, wFUN.args = NULL, dnn = "auto",
     sort = TRUE, na.exclude = TRUE, drop = FALSE, method = "auto")
```

A drop-in `table()` replacement (masked as `table()` by `set_collapse(mask = "special")`)
that is much faster and supports weights and arbitrary aggregation of the weight:

There is no `data =` argument — pass the vectors, or wrap in `with()`:

```r
qtab(dat$region, dat$income)
with(dat, qtab(region, income))                  # dimnames taken from the expressions
qtab(dat$region, w = dat$POP)                    # sum of population per region
qtab(dat$region, w = dat$PCGDP, wFUN = fmean)    # mean GDP per region
qtab(dat$sector, na.exclude = FALSE)
qtab(dat$region, dnn = "Region")
```

## `pwcor` / `pwcov` / `pwnobs`

```r
pwcor(X, ..., w = NULL, N = FALSE, P = FALSE, array = TRUE, use = "pairwise.complete.obs")
pwcov(...)     pwnobs(X)
```

Pairwise (weighted) correlations with optional pairwise observation counts and
p-values, and a compact print method:

```r
pwcor(num_vars(dat))
pwcor(dat, N = TRUE, P = TRUE)
print(pwcor(dat, N = TRUE, P = TRUE), show = "lower.tri")
pwcor(X, Y)                                  # cross-correlation between two sets
```

## `varying()`

```r
varying(x, by = NULL, cols = NULL, any_group = TRUE, use.g.names = TRUE, drop = TRUE)
```

Checks whether variables vary — overall, or within groups/panel ids. Essential for
panel model specification and for pruning constant columns:

```r
varying(dat)                                   # TRUE/FALSE per column
varying(dat, ~ iso3c)                          # does it vary within country?
varying(dat, ~ iso3c, any_group = FALSE)       # per-group matrix
PR_diff |> get_vars(varying(PR_diff) & fnobs(PR_diff) > 350L)   # keep useful columns
```

## Variable labels and names

collapse preserves `"label"` attributes through most operations, which makes
labelled data (haven, WDI, Stata imports) pleasant to work with.

```r
vlabels(X, attrn = "label", use.names = TRUE)
vlabels(X) <- value                       # set by reference
setLabels(X, value, attrn = "label", cols = NULL)
vclasses(X)     vtypes(X)     vlengths(X)
namlab(X, class = FALSE, N = FALSE, Ndistinct = FALSE)
relabel(.x, ...)    setrelabel(.x, ...)   # rename labels like frename renames names
```

```r
namlab(dat)                                # names + labels side by side
namlab(dat, N = TRUE, Nd = TRUE, class = TRUE)
vlabels(X)["acled_fatalities"] <- "ACLED Fatalities"
names(X) <- sub("current US\\$", "% of GDP", vlabels(X))   # promote labels to names
setLabels(dat, strsplit(vlabels(dat), " \\(| -"))
```

## List processing

collapse has a complete toolkit for nested lists — the natural representation of
model outputs, multi-year matrices, and API results.

```r
rsplit(x, by, drop = TRUE, flatten = FALSE, cols = NULL, keep.by = FALSE, simplify = TRUE)
unlist2d(l, idcols = ".id", row.names = FALSE, recursive = TRUE, id.factor = FALSE, DT = FALSE)
t_list(l)                                   # transpose a nested list
rapply2d(l, FUN, ..., classes = "data.frame")   # rapply that treats data frames as leaves
get_elem(l, elem, recursive = TRUE, DF.as.list = FALSE, keep.tree = FALSE, regex = FALSE, invert = FALSE)
has_elem(l, elem, ...)
atomic_elem(l)   list_elem(l)   reg_elem(l)   irreg_elem(l)
ldepth(l)        is_unlistable(l)
```

**`rsplit`** splits recursively — a formula with several RHS variables gives a
nested list, and the LHS selects columns:

```r
rsplit(dat, ~ region)                            # list of data frames
rsplit(dat, ~ region + year)                     # NESTED list (region -> year)
rsplit(dat, lon + lat + area ~ main_cat)         # only these columns, split by main_cat
rsplit(dat, NUTS2 ~ ISO2)                        # single column -> list of vectors
rsplit(mat, f)                                   # matrices split too
```

**`unlist2d`** is the inverse and the generalized recursive row-bind: it turns any
nested list of vectors / matrices / data frames into one data frame, with id
columns recording the nesting path.

```r
NIOT |> unlist2d(c("year", "country"), "sector") |> fwrite("NIOT.csv")
grids_stats |> get_elem("variable.importance") |> unlist2d("Size", DT = TRUE)
lapply(mats, \(x) cbind(N = nrow(x), cor(x))) |> unlist2d("group", "variable", DT = TRUE)
```
`idcols` names the id column(s) (one per nesting level), the second positional
argument `row.names` names a column made from row names — that is how matrix row
labels survive. `DT = TRUE` returns a data.table.

`rsplit()` + `rapply2d()` + `unlist2d()` round-trip: to rebuild a nested list of
matrices from a long table, `dat |> rsplit(~ year + country) |> rapply2d(qM, 1)`.

**`get_elem`** pulls elements out of arbitrarily deep lists by name or regex —
the way to harvest one component from a list of fitted models:

```r
results |> get_elem("r.squared") |> unlist()
results |> get_elem("enet_cv_ensemble", DF.as.list = TRUE)
fit |> get_elem("coefficients", regex = TRUE)
```

**`t_list`** transposes a list of lists (swap the two levels) — useful for
reorganizing simulation output before binding.

`atomic_elem(l)` / `list_elem(l)` split a list into its atomic and non-atomic
top-level parts (`reg_elem`/`irreg_elem` do it recursively) — handy for
separating a model object's data from its metadata.

## Small helpers worth knowing

```r
.c(a, b, c)                 # c("a","b","c") — non-standard concatenation
.c(x, y) %=% list(1, 2)     # multiple assignment; also massign(nam, values, envir)
c("T","N") %=% dim(X)       # the idiom used everywhere in numerical code
all_identical(...)          all_obj_equal(...)     all_funs(expr)
setColnames(x, nm)   setRownames(x, nm)   setDimnames(x, dn, which)
unattrib(x)                 # strip all attributes (fast; often needed before c()/hashing)
setAttrib(x, a)             setattrib(x, a)     # replace attribute list (by reference)
copyAttrib(to, from)        copyMostAttrib(to, from)   # copyMostAttrib skips dim/dimnames/names
is_categorical(x)           is_date(x)
fnrow(X)  fncol(X)  fdim(X)  seq_row(X)  seq_col(X)  fnlevels(f)
vec(X)                      # matrix/list -> single vector
```

`.c()` is used constantly to write long variable lists without quotes.
`%=%` unpacks a list or function result into several names at once.
