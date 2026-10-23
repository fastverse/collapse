# Gotchas

Each entry pairs the broken code with the fix and the reason. Most of these come
from collapse being *deliberately* different from dplyr/base R, not from bugs.

## Namespace

**1. `mutate`/`subset`/`select`/`summarise`/`n()` do not exist by default.**

```r
library(collapse)
dat |> mutate(x = 1)          # calls dplyr's, or errors
dat |> fmutate(x = 1)         # correct
set_collapse(mask = "manip")  # or enable the unprefixed names first
```
Read the top of the file you are editing and match it. Writing masked names into
an unmasked script silently loses all the performance the code exists for.

**2. `set_collapse(mask = ...)` must never appear in package code.** It mutates
the exported namespace and moves collapse to the top of the user's search path.
Use `importFrom(collapse, ...)` or `collapse::`.

**3. Masking by *keyword* silently changes results; masking a single function
does not.** Only the keywords `"all"`, `"fast-fun"`, `"fast-stat-fun"`,
`"fast-trfm-fun"` flip collapse's internal vectorization flags, so an expression
containing a masked statistic is executed *once* with the grouping passed down:

```r
d <- qDF(list(g = c(1,1,2,2), v = c(1,3,5,7)))
f <- \() d |> fgroup_by(g) |> fsummarise(mu = sum(v) / length(v))

f()                                        # 2 6  — per-group mean (no masking)
set_collapse(mask = "sum");            f() # 2 6  — individual mask: NOT vectorized
set_collapse(mask = "fast-stat-fun");  f() # 1 3  — grouped sum / OVERALL length
```

Same code, different numbers, no warning. It is deliberate — you are meant to be
either all in (keyword) or all out — but it means `mask = "all"` quietly rewrites
the meaning of every `sum()`/`mean()` inside `fmutate`/`fsummarise` in the
project. Write `base::sum()` where you want the split-apply behaviour back.

**4. `across()` is not a function.** `collapse::across(...)` fails — it is an
expression parsed by `fmutate`/`fsummarise`.

**4b. Masking swaps the *function*, not just the name.** A masked `unique()`,
`table()`, `quantile()`, `range()`, `match()` is the collapse function with
collapse's arguments and defaults. Code that looks portable is not:

```r
set_collapse(mask = "helper")
unique(c(3, 1, 3, 2), sort = TRUE)    # 1 2 3   — funique sorts
base::unique(c(3, 1, 3, 2), sort = TRUE)  # 3 1 2 — `sort` lands in `...` and is IGNORED
```

Base silently swallows the extra argument rather than erroring, so the same line
gives different answers depending on whether masking is on. Two consequences:

- Read the script header before editing. A file that never called
  `set_collapse(mask = ...)` gets base semantics for every one of these names.
- Documentation still lives under the prefixed name — `?funique`, not `?unique`.

`fastverse::fastverse_conflicts()` reports exactly what is masked, which is why
the standard script header calls it. Masked names are ordinary functions, so they
also work where a function is expected: `lapply(list_of_dfs, select, id, code)`.

## Defaults that differ from other packages

**5. `na.rm = TRUE` everywhere.** The opposite of base R's `sum`/`mean`. If your
data has no `NA`s, `set_collapse(na.rm = FALSE)` is both more base-R-like and
faster.

**6. `join()` keeps only the first match** (`multiple = FALSE`), unlike
`dplyr::left_join`.

```r
join(x, y, on = "id")                    # first match only
join(x, y, on = "id", multiple = TRUE)   # dplyr-equivalent cartesian result
```
It also does not sort (`sort = FALSE` keeps `x` order) and does not validate
(`validate = "m:m"`). Use `validate = "1:1"` or `require = list(x = 0.9)` to
assert your expectation, and read the `verbose = 1` summary it prints.

**7. `pivot(..., how = "wider")` silently aggregates duplicates** with
`FUN = "last"`. Pass `check.dups = TRUE` to be warned instead.

**8. `pivot()` renames only via a named list.**

```r
pivot(d, "country", names = "term")                    # SELECTS a column called term
pivot(d, "country", names = list(variable = "term"))   # RENAMES the names column
```

**9. `funique()` returns the input unchanged (no copy) when everything is
already unique** — do not rely on getting a fresh object back.

**10. `qDF`/`qDT`/`qM`/`qTBL` drop non-essential attributes.** `qM(EuStockMarkets)`
is a plain matrix, unlike `as.matrix()`. Pass `keep.attr = TRUE` to preserve.

## Grouping and vectorization

**11. Mixing prefixed and unprefixed statistics in one expression.**

```r
fgroup_by(d, g) |> fsummarise(x = fmean(a) + min(b))    # grouped mean + OVERALL min
fgroup_by(d, g) |> fsummarise(x = fmean(a) + fmin(b))   # both grouped (vectorized)
fgroup_by(d, g) |> fsummarise(x =  mean(a) +  min(b))   # both grouped (split-apply-combine)
```
Any Fast Statistical Function in the expression switches the *whole* expression to
vectorized execution. Pick one style per expression.

**12. Vectorization only sees what you type.** A statistic inside a helper
function (`f <- \(x) fmean(x) + fmin(x)`) is not vectorized — it is applied per
group.

**13. `fsubset()` is not allowed on grouped data**, and `roworder()` messages
about it. Subset and sort before grouping.

**14. `set_collapse(sort = FALSE)` changes result ordering globally** — including
inside other packages that use collapse. When output order matters, pass a sorted
grouping explicitly:

```r
fmean(x, qF(g, sort = TRUE))     # robust regardless of the global option
```
Statistical functions have no `sort` argument, so this is the only way.

**15. `fgroup_by(ind)` with a character variable grabs a column named `ind`.**
Use `group_by_vars(X, ind)` for standard evaluation in programs.

**16. Factors are efficient inputs for simple functions but not for
`fmedian`/`fnth`/`fmode`/`fndistinct`/`BY`/`gsplit`** — those want a `GRP` object
when the grouping is reused.

**16b. Weighted order statistics reject `NA` weights.** `fmedian`, `fnth`, and
`fquantile` error with *"Missing weights in order statistics are currently only
supported if x is also missing"* when a weight is `NA` but the value is not.
`fmean`, `fmode`, `fsd`, `fvar` accept them silently.

```r
x <- c(1, 2, 3, 4); w <- c(1, NA, 1, 1); g <- c(1, 1, 2, 2)
fmean(x, g, w)      # 1  3.5
fmedian(x, g, w)    # Error: Missing weights in order statistics ...
```

This bites when one function computes a weighted mean *and* a weighted median
over real data whose weight column has gaps (population, employment, GDP). The
fix is a decision, not a workaround: either drop those rows, or state that
missing weights count as zero and set them so. Doing the latter silently inside a
package function changes the statistic without telling the caller.

## Selection and syntax

**17. No tidyselect.** `starts_with()`, `ends_with()`, `where()`, `all_of()`,
`c_across()` do not exist. Use `get_vars(d, "^prefix", regex = TRUE)` for regex, bare predicates
(`across(is.numeric, fmean)`, `get_vars(d, is.numeric)`), or explicit ranges
(`a:f`).

**18. No purrr lambdas.** `~ .x + 1` is not supported; write `\(x) x + 1`.

**19. `fselect` negation uses `-`, not `!`:** `fselect(d, -a, -b)` or
`fselect(d, -(a:c))`.

**20. `fsubset`'s `...` are columns to keep, not further conditions.**

```r
fsubset(d, x > 1, y < 2)          # WRONG: `y < 2` is read as a column expression
fsubset(d, x > 1 & y < 2, id, x)  # right: conditions with &, then columns
```

**21. `across()` cannot use `where()` but *can* compute on grouping columns** —
unlike dplyr. Watch out when using `.cols = NULL` (all columns) on grouped data.

## Time series and panels

**22. Forgetting `t` on an irregular panel gives silently wrong results.**
`flag`/`fdiff`/`fgrowth` assume consecutive, ordered rows within groups when `t`
is absent.

```r
fgrowth(v, g = list(c, s))             # assumes no gaps — wrong on unbalanced data
fgrowth(v, g = list(c, s), t = year)   # correct; also errors on duplicate (g,t)
```

**23. Numeric `t` is coerced with `as.integer()`.** A decimal year for monthly
data collapses to the same integer. Use a proper class (`zoo::yearmon`,
`zoo::yearqtr`, `Date`) — those go through `timeid()` and get the right step.

**24. `xts`/`zoo` methods do not use the index automatically.** Pass it yourself:
`flag(x, 1:3, t = zoo::as.yearmon(index(x)))`.

**25. Operators `L`/`D`/`G`/`W`/`B`/`STD`/`HDW` prefix result column names**
(`L1.x`, `W.x`) while `flag`/`fdiff`/`fwithin` do not (unless `stubs = TRUE`).
`set_collapse(stub = FALSE)` turns the prefixes off globally.

## Arithmetic and by-reference operations

**26. Infix `%op%` binds tighter than `*` and `/`, and these operators mutate.**

```r
a <- 1
a %+=% 2 * 3        # parsed as (a %+=% 2) * 3  ->  a is now 3, and 9 is discarded
a %+=% (2 * 3)      # what you meant
```
R gives every `%op%` higher precedence than arithmetic, so a chain like
`X %r*% w %r+% m` is fine (left to right) but any mix with `*`/`/`/`+` needs
parentheses. The same applies to `%r*%`, `%c-%`, `%==%`, and friends.

**27. `set = TRUE` / `settransform` / `setv` modify the original object, and
assignment is not a copy.** A "backup" taken beforehand is mutated too:

```r
L_backup <- L                        # NOT a copy
fsum(L, g, TRA = "/", set = TRUE)
L_backup$a[1]                        # already changed
```

Use `data.table::copy()` when you need the original afterwards. Convenient in
loops and package internals; surprising if the caller still holds a reference.
`data.table` semantics apply — a shallow copy shares columns. See
`references/efficiency.md` for the by-reference toolkit and when to reach for it.

**28. `settransform(d, ...)` assigns back to the *calling environment*.** It does
not return the data; `d <- settransform(d, ...)` is wrong.

## Attribute handling

**29. Aggregating a classed matrix or a `ts` drops attributes.** collapse
preserves attributes unless doing so is likely to produce something wrong. For a
matrix with a class (`mts`, `xts`), only dimnames survive aggregation; for `ts`
vectors, attributes are dropped when length changes.

**30. `TRA = "fill"`/`"replace"` may change the data type.** The result takes the
type of `STATS`; attribute preservation then follows a documented precedence
(`STATS` attributes win if `is.object(STATS)`). Relevant when replacing a factor
with counts.

**30b. `TRA = "/"` on *integer* input does integer division.** This is a narrow
type note, not a reason to avoid `TRA = "/"` — that remains the correct and
fastest way to compute a share, and on double data (which is the normal case)
there is nothing to think about:

```r
fmutate(d, share = fsum(value, list(country, sector), TRA = "/"))   # do this
fmutate(d, share = value / fsum(value, list(country, sector), TRA = "fill"))
                                       # don't — same answer, one extra full copy
```

The caveat applies only when `x` is integer *and* the statistic is integer, since
`TRA` preserves storage type:

```r
x <- c(7L, 2L, 9L, 3L); g <- c(1L, 1L, 2L, 2L)
fsum(x, g, TRA = "/")               # 0 0 0 0    — integer division
fsum(x + 0, g, TRA = "/")           # 0.778 0.222 0.750 0.250
```

So: if you know the column is double, just write `TRA = "/"`. If you are handling
data of unknown type — counts, or IO/trade cells read straight from CSV — coerce
once when the data is loaded (`x + 0`, `storage.mode(m) <- "double"`,
`ftransformv(d, is.integer, as.double)`), not at each call site.

**31. `num_vars()` uses a C-level type check, not `is.numeric` methods.** A class
that defines `is.numeric.foo <- function(x) FALSE` is still returned by
`num_vars()`. Use `get_vars(d, is.numeric)` when the method matters. `collap()`
uses the same C definition to split numeric from categorical columns.

**32. Classes with parallel-length attributes break.** `lubridate::interval` has
a `"starts"` attribute the same length as the data; collapse preserves it without
subsetting it, producing corrupt objects. Convert such columns first.

**33. `sf` bounding boxes are not recomputed.** After `fsubset()` the geometry's
bbox is stale — collapse does no geometric operations.

## data.table

**34. `:=` after a collapse *statistical* function warns.** Data manipulation
functions return over-allocated data.tables; the `.data.frame` methods of
statistical functions do not.

```r
DT |> fgroup_by(id) |> fmean() |> qDT()    # add qDT() to silence the warning
```
