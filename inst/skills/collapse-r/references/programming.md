# Programming with collapse

This file covers grouping objects, memory-efficient primitives, and the rules for
using collapse inside an R package. The guiding principle from the package's own
"Developing with collapse" vignette: **be minimalistic in computations, think
about memory, and internally favour primitive R objects.**

## Table of contents
- [Choosing the right grouping object](#choosing-the-right-grouping-object)
- [The `GRP` object](#the-grp-object)
- [`gsplit` and `greorder`](#gsplit-and-greorder)
- [Memory-efficient primitives](#memory-efficient-primitives)
- [In-place arithmetic](#in-place-arithmetic)
- [Favour lists over data frames internally](#favour-lists-over-data-frames-internally)
- [Global options in package code](#global-options-in-package-code)
- [A worked example: grouped regression](#a-worked-example-grouped-regression)

## Choosing the right grouping object

Cheapest to richest. Pick the least you need.

| Object | Created by | Good for |
|---|---|---|
| nothing | `fsum(x, g)` with a raw vector | one-off grouping — the function converts internally |
| factor | `qF(g, sort = FALSE, na.exclude = FALSE)` | repeated use with simple functions (`fsum`, `fmean`, `fmin`, …) |
| `qG` | `qG(g, sort = FALSE, na.exclude = FALSE)` | same, without group names |
| integer id | `group(g1, g2)` / `groupv(list)` | fastest hashing for plain vectors; first-appearance order |
| `GRP` | `GRP(X, by, sort, return.groups, return.order)` | repeated use with `fmedian`, `fnth`, `fmode`, `fndistinct`, `BY`, `gsplit`; when you need the unique groups back |

```r
fsum(x, g)                                    # one-off aggregation (sorted)
fsum(x, g, TRA = "fill")                      # one-off expansion (uses unsorted grouping: fast)
fsum(x, qF(g, sort = FALSE, na.exclude = FALSE))
fsum(x, group(g), use = FALSE)                # fastest simple grouped sum
fmedian(x, GRP(g), use = FALSE)               # complex functions want a GRP object
```

`na.exclude = FALSE` adds an `"na.included"` class that tells the statistical
functions to skip the missing-value check — a real saving on large vectors.
`use.g.names = FALSE` (abbreviate `use = FALSE`) skips constructing group names.
`na.rm = FALSE` is the fastest path when the data has no `NA`s.

Why not always `group()`? Because `qF`/`qG` are smarter about inputs that are
*already* factors or `qG` objects, whereas `group()` re-hashes everything.

## The `GRP` object

```r
GRP(X, by = NULL, sort = TRUE, decreasing = FALSE, na.last = TRUE,
    return.groups = TRUE, return.order = sort, method = "auto", drop = TRUE, call = TRUE)
```

Returns a list with `N.groups`, `group.id` (length of data), `group.sizes`,
`groups` (a data frame of unique values, or `NULL`), `group.starts`, `ordered`,
`order`, `call`. Accessors: `GRPN(g)`, `GRPid(g)`, `GRPnames(g)`, `is_GRP(g)`,
`length(g)`, `as_factor_GRP(g)`.

```r
g <- GRP(mtcars, c("cyl","vs","am"))
g$N.groups; g$group.sizes; head(g$group.id)
```

Set `sort = FALSE` when order doesn't matter, `return.groups = FALSE` when you
don't need the unique values, and `return.order = FALSE` to skip storing the
ordering vector. `drop = FALSE` (v2.1.7+, with at least one factor) keeps
unobserved factor-level combinations, with `group.sizes` of 0.

`fsummarise` and friends are convenience wrappers over exactly this. In package
code, go one level down — it is faster and safer:

```r
# mtcars |> fgroup_by(cyl, vs, am) |> fsummarise(mpg = fsum(mpg), across(c(carb,hp), fmean))
g <- GRP(mtcars, c("cyl","vs","am"))
add_vars(g$groups,
         get_vars(mtcars, "mpg")             |> fsum(g,  use = FALSE),
         get_vars(mtcars, c("carb","hp"))    |> fmean(g, use = FALSE))
```

If you do use `fgroup_by()` in a package, use it with NSE (`fgroup_by(cyl, vs)`)
or use `group_by_vars(X, ind)` for character input — `fgroup_by(ind)` would grab a
column literally named `ind` if one exists.

### Keep the function small

The whole shape of a grouped-summary backend is a dozen lines. Resist padding it
with defensive scaffolding:

```r
group_summary <- function(data, by, cols, w = NULL, ...) {

  g <- GRP.default(data, by, ...)
  x <- get_vars(data, cols)

  mean_x   <- fmean(x,   g, w, use.g.names = FALSE)
  median_x <- fmedian(x, g, w, use.g.names = FALSE)

  names(mean_x)   <- paste0(cols, "_mean")
  names(median_x) <- paste0(cols, "_median")

  add_vars(g$groups, list(n = g$group.sizes), mean_x, median_x)
}
```

One `GRP` built once and reused; both statistics vectorized across all `cols` in
a single call; group sizes taken from the object rather than recounted; the
result assembled onto `g$groups`, which carries the input's class.

**Let errors reach the user.** If a weight column has `NA`s and `fmedian()`
refuses them (see below), that is information the caller needs — do not silently
zero-fill the weights to make the call succeed, because it changes the statistic
without saying so. Validate what you can state cheaply and clearly (`ckmatch()`
on column names gives a good error for free), and let everything else surface.
A package function that quietly repairs its input is harder to trust than one
that stops.

## `gsplit` and `greorder`

```r
gsplit(x, g, use.g.names = FALSE, ...)        # fast split by a GRP object
greorder(x, g, ...)                            # inverse: identical(greorder(unlist(gsplit(x, g)), g), x)
```

This pair is how you apply a non-vectorizable function by group and put the
results back in original row order — the engine behind `BY()` and the fallback
path in `fmutate`:

```r
res <- unlist(lapply(gsplit(x, g), some_function), FALSE, FALSE) |> greorder(g)
```

## Memory-efficient primitives

R code is slow mostly because it is not vectorized and because it makes copies.
These functions attack the second problem. The recurring theme: **indices beat
logical vectors, and in-place beats allocate**.

```r
whichv(x, value, invert = FALSE)      x %==% value      x %!=% value
whichNA(x, invert = FALSE)
anyv(x, value)   allv(x, value)   allNA(x)
setv(X, v, R, invert = FALSE, vind1 = FALSE, xlist = FALSE)     # by reference
copyv(X, v, R, ...)                                             # same, returns a copy
alloc(value, n, simplify = TRUE)                                # fast rep_len/replicate
na_rm(x)   na_insert(X, prop, value, set)   missing_cases(X, cols, prop, count)
vlengths(X)   vtypes(X)   vgcd(x)   vec(X)   cinv(x)
```

```r
x[x == 0] <- NA          ->   setv(x, 0, NA)
x[is.na(x)] <- y[is.na(x)]  ->   setv(x, NA, y)
which(x == 0)            ->   whichv(x, 0)   or   x %==% 0
any(x == 0)              ->   anyv(x, 0)
x[!is.na(x)]             ->   na_rm(x)
rep(0, n)                ->   alloc(0, n)
x %in% tab               ->   x %iin% tab      # returns indices, feeds fsubset directly
```

Logical subsetting is slower than index subsetting, so `fsubset(d, x %==% 1)`
beats `fsubset(d, x == 1)` on large data. `ss(d, i, j, check = FALSE)` skips index
validation in known-good internal code.

`setv()` is versatile: `v` can be a value to search for, a logical vector, or
indices (`vind1 = TRUE` declares that `v` holds indices when ambiguity is
possible); `R` can be a scalar or an aligned vector; `invert = TRUE` flips the
query; `xlist = TRUE` treats a list `X` as a list of vectors to modify.

```r
setv(graph$to, miss, group(ss(graph, miss, c("TX","TY"))) %+=% fmax(graph$from), vind1 = TRUE)
setv(cbind(n, area, RWI, IWI), NA, -2)
```

## In-place arithmetic

```r
setop(X, op, V, rowwise = FALSE)
X %+=% V     X %-=% V     X %*=% V     X %/=%V
```

`res <- x + y + z` makes two copies; `res <- x + y; res %+=% z` makes one. `setop`
works on vectors, matrices, and data frames, and `rowwise = TRUE` sweeps a vector
across matrix/data-frame **rows**:

```r
m <- qM(mtcars); setop(m, "*", seq_col(m), rowwise = TRUE)
leontief$fvax %+=% new_values
leontief$fvax %/=% length(years)
x_res <- get_vars(diff, nam) %c/% get_vars(perc_diff, nam) %*=% 100
```

Every `%op%` binds *tighter* than `*`, `/`, `+`, so `a %+=% b * 2` parses as
`(a %+=% b) * 2` and silently mutates `a` by `b`. Parenthesize whenever an infix
operator meets ordinary arithmetic.

Related by-reference facilities: `set = TRUE` in all Fast Statistical Functions
(via `TRA`), `setTRA()`, `settransform()`, `setrename()`, `setColnames()`,
`setattrib()`, `na_locf(set = TRUE)`, `replace_na(set = TRUE)`.

## Favour lists over data frames internally

Everything you do to a `data.frame` is more expensive than the same thing on a
plain list: `names(l)` vs `names(df)`, `l[[i]]` vs `df[[i]]`, `length(l)` vs
`ncol(df)`. collapse itself never uses data frames internally — it unclasses them.

```r
f <- function(data) {
  ax <- attributes(data)
  attributes(data) <- NULL        # now a plain list
  # ... manipulate with [[, lapply, etc. ...
  ax$names <- new_names
  setattrib(data, ax)             # restore
}
```
Or simply `class(X) <- NULL` at the top and rebuild at the end:
```r
attr(l, "row.names") <- .set_row_names(length(l[[1L]]))
class(l) <- "data.frame"
```

If you only use collapse functions this is moot — `join()`, `pivot()`,
`fsubset()` etc. already handle their input as lists. It matters when you mix
collapse with base `[`/`$` semantics. Otherwise use `.subset()`, `.subset2()`,
`get_vars()`, and `attr()` rather than `[`, `[[`, `names<-`.

`qDF()`, `qDT()`, `qTBL()` construct data frames far faster than `as.data.frame`
and friends.

## Global options in package code

```r
set_collapse(na.rm, sort, nthreads, stable.algo, stub, verbose, digits, mask, remove)
get_collapse(opts = NULL)
```

Rules:

1. **Never set `mask` or `remove` inside a package.** They mutate the exported
   namespace and push collapse to the top of the user's search path.
2. If you set other options, restore them:
   ```r
   fast_function <- function(x, ...) {
     oldopts <- set_collapse(nthreads = 4, na.rm = FALSE)
     on.exit(set_collapse(oldopts))
     ...
   }
   ```
3. Remember that **user options affect your package**. `na.rm`, `sort`, and
   `nthreads` propagate into your code unless you set the arguments explicitly.
   Usually that is desirable. But if your code *generates* `NA`s and relies on
   them being skipped, set `na.rm = TRUE` explicitly; and if output ordering
   matters, pass a sorted grouping rather than relying on the default:
   ```r
   fmean(x, qF(g, sort = TRUE))     # robust to set_collapse(sort = FALSE)
   ```
4. Import explicitly (`importFrom(collapse, fmean)` or `collapse::fmean()`), so
   the user's masking choices cannot affect your code.

Useful macros for programmatic checks: `.FAST_FUN`, `.FAST_STAT_FUN`,
`.OPERATOR_FUN`, `.COLLAPSE_ALL`, `.COLLAPSE_TOPICS`. `all_funs(expr)` lists the
functions called in an expression — this is how `fsummarise` decides whether to
vectorize:

```r
any(all_funs(substitute(fmean(mpg) + min(mpg))) %in% .FAST_STAT_FUN)   # TRUE
```

Validate user-supplied column names with `ckmatch(cols, names(data))`, which
errors listing exactly what was not found.

## A worked example: grouped regression

The idiom that shows what "minimalistic" buys you — a univariate grouped OLS
slope in three lines, with exactly one full copy of `x`:

```r
greg <- function(y, x, g) {
  g   <- group(g)
  dmx <- fmean(x, g, TRA = "-", na.rm = FALSE)
  (fsum(y,   g, dmx, use = FALSE, na.rm = FALSE) %/=%
   fsum(dmx, g, dmx, use = FALSE, na.rm = FALSE))
}
```

`sum(y*(x-mean(x))) / sum((x-mean(x))^2)` per group, using `w =` to form the
products inside the C loop and `%/=%` to divide without another allocation. On
10M rows with 1M groups this runs at roughly the cost of the grouping itself.

`na.rm = FALSE` here is not only a speed setting — it is what keeps the result
correct. Under `na.rm = TRUE`, a missing `y` is skipped in the numerator while
still contributing to `dmx` and the denominator, so the two sums run over
different samples and the slope is biased. Drop incomplete rows before calling
this. `references/efficiency.md` has the data-frame version of the idiom, a
worked example of the bias, and the forecast pattern built on top of it.
