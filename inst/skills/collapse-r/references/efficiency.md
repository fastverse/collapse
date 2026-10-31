# Writing efficient collapse code

collapse is fast by default, but the difference between naive and deliberate
collapse code is routinely 2–5x on the same result. The levers are few and they
compose. This file is about analysis and pipeline code; for package internals see
`references/programming.md`, which goes further down the stack.

Timings below were measured on collapse 2.1.7 (macOS, OpenMP enabled) with
5M rows / 500k groups unless stated. They are illustrative, not promises —
the ranking is stable, the ratios are not.

## Table of contents
- [The mindset](#the-mindset)
- [The five levers](#the-five-levers)
- [1. Sequence operations so less data moves](#1-sequence-operations-so-less-data-moves)
- [2. Use the minimal grouping the job needs](#2-use-the-minimal-grouping-the-job-needs)
- [3. Unsorted grouping (`sort = FALSE`)](#3-unsorted-grouping-sort--false)
- [4. Skip the missing-value check (`na.rm = FALSE`)](#4-skip-the-missing-value-check-narm--false)
- [5. Multithreading (`nthreads`)](#5-multithreading-nthreads)
- [Materialize only what you need](#materialize-only-what-you-need)
- [Computing by reference](#computing-by-reference)
  - [The iterative-fitting pattern](#the-iterative-fitting-pattern)
  - [The trap: no copy means no backup](#the-trap-no-copy-means-no-backup)
- [Reuse an ordering across order statistics](#reuse-an-ordering-across-order-statistics)
- [Vectorized grouped regression](#vectorized-grouped-regression)
- [Setting the options globally](#setting-the-options-globally)
- [A worked before/after](#a-worked-beforeafter)

## The mindset

collapse usually offers several ways to reach the same outcome. That is confusing
at first, but the options are distinguishable **by efficiency** — so the habit to
build is thinking about computer resources and crafting the minimal solution to
the problem, rather than reaching for the first thing that works.

The canonical illustration is a deduplication problem: multiple spatial datasets
of points of interest, keep the richest source for each location and POI type.
With comparable confidence, location and type indicators in hand, the whole thing
is one line:

```r
fsubset(data, source == fmode(source, list(location, type), confidence, "fill"))
```

Keep POIs from the confidence-weighted most frequent source, by location and type.
No intermediate dataset is materialized and every computation is relegated to
`fmode()`, which calls a highly optimized internal grouping algorithm. The
alternative — group, aggregate, join back, filter — computes the same answer
while building three objects nobody wanted.

That is the pattern to look for: **can the grouping go inside the statistical
function, so nothing intermediate is built?**

## The five levers

| Lever | What to write | Measured here |
|---|---|---|
| Sequence operations | subset **and** select in one call, before grouping | 1.65x |
| Minimal grouping | `f<stat>(x, g, TRA=)` instead of `fgroup_by \|> fmutate` | 1.1–1.6x |
| Unsorted grouping | `sort = FALSE`, or `group(g)` | 1.25–1.56x |
| No `NA` check | `na.rm = FALSE` when the data has none | 1.04x on clean doubles; more with many groups/types |
| Multithreading | `nthreads = n` | 4–7x ungrouped; ~1.6x across data-frame columns; ~1x for one grouped vector |

Stacked on one grouped sum, the tuned form ran **1.75x** faster than the default:

```r
fsum(x, g)                                        # 42 ms
fsum(x, group(g), use = FALSE, na.rm = FALSE)     # 24 ms
```

## 1. Sequence operations so less data moves

The cheapest computation is the one that never touches the columns you don't
need. Subset and select in the **same** call, and do it before grouping:

```r
# 84 ms — subsets a 6-column frame, then narrows
fsubset(d, a > 0) |> fselect(id, a) |> fgroup_by(id) |> fmean()

# 51 ms — carries two columns through the whole pipeline
fsubset(d, a > 0, id, a) |> fgroup_by(id) |> fmean()
```

`fsubset()`'s `...` exist precisely for this. The same applies to `collap()`
(`cols =`) and the operators (`cols =`).

collapse enforces part of this for you: `fsubset()` **refuses** grouped data and
`roworder()` messages about it, because filtering or reordering a grouped frame
means rebuilding both the data and the grouping object. Subset, then sort, then
group — in that order.

Also prefer index-returning comparisons over logical masks on large data:

```r
fsubset(d, x == 1)      # allocates an n-length logical vector
fsubset(d, x %==% 1)    # returns positions directly
fsubset(d, id %iin% keep)   # instead of %in%
```

## 2. Use the minimal grouping the job needs

**Do not reach for `fgroup_by()` unless you need it.** It builds a grouping
object and attaches it to the frame; for a single grouped statistic the
statistical function's own `g` argument is cheaper and reads better. collapse
deliberately has no `.by =` argument — internal grouping *is* the `.by`.

```r
d |> fgroup_by(g1) |> fmutate(m = fmedian(v)) |> fungroup()   # 150 ms
fmutate(d, m = fmedian(v, g1, TRA = "fill"))                  # 137 ms
fmutate(d, m = fmedian(v, g1, TRA = "fill", set = TRUE))      #  93 ms
```

Named shortcuts for the common cases: `fbetween`/`B` (group mean, expanded),
`fwithin`/`W` (group-centred), `fscale`/`STD`.

`fgroup_by()` earns its place when several *different* functions share one
grouping. Even then, for mutate-style work pass `return.groups = FALSE` so the
unique grouping columns are never materialized:

```r
d |> fgroup_by(cyl, vs, am, return.groups = FALSE) |>
  fmutate(med = fmedian(mpg), dev = fwithin(mpg), z = fscale(mpg), .keep = "used")
```

For a repeated grouping across many operations, build a `GRP` object once and
pass it everywhere — see `references/programming.md` for choosing between
`group()`, `qF`/`qG`, and `GRP`.

## 3. Unsorted grouping (`sort = FALSE`)

Sorted grouping uses `radixorder`; unsorted uses hashing, which is faster and
preserves first-appearance order:

```r
fsum(x, g)                                          # 50 ms  sorted (default)
fsum(x, qF(g, sort = FALSE))                        # 40 ms
fsum(x, qF(g, sort = FALSE, na.exclude = FALSE))    # 40 ms  + skips the NA check
fsum(x, group(g), use = FALSE)                      # 32 ms  fastest
```

The cost is that results come out in first-appearance order rather than sorted.
That is usually irrelevant (you sort the final table anyway) and occasionally
matters a lot — see the IO-table recipes, where `sort = FALSE` is *required* to
keep a block structure intact.

Set it per call, or globally with `set_collapse(sort = FALSE)`. Statistical
functions have **no** `sort` argument, so when output order must be sorted
regardless of the global option, pass a sorted grouping explicitly:

```r
fmean(x, qF(g, sort = TRUE))
```

## 4. Skip the missing-value check (`na.rm = FALSE`)

`na.rm = TRUE` is collapse's default (the opposite of base R). If the data has no
missing values, `FALSE` skips the check:

```r
fsum(x, group(g), use = FALSE, na.rm = TRUE)    # 29 ms
fsum(x, group(g), use = FALSE, na.rm = FALSE)   # 28 ms
```

On clean doubles the gain is small — the check is cheap. It grows with more
groups, with integer and character data, and with the order statistics. Treat it
as free money when you *know* the data is complete (`set_collapse(na.rm = FALSE)`
once at the top), not as something to micro-tune per call. The related trick is
`na.exclude = FALSE` in `qF()`/`qG()`, which adds an `"na.included"` class so the
statistical functions skip their own check on the grouping vector.

**Be careful in code that generates `NA`s.** If a step produces missing values and
a later one is expected to skip them, a global `na.rm = FALSE` silently changes
the answer.

## 5. Multithreading (`nthreads`)

`nthreads` is an argument on the statistical functions and a global option. What
it parallelizes depends on the shape of the problem — this is the part that is
usually misunderstood:

```r
fsum(x,  nthreads = 1)   # 4 ms      ungrouped vector, 20M obs
fsum(x,  nthreads = 4)   # 1 ms      -> ~4x
fmean(x, nthreads = 4)   #           -> ~7x vs 1 thread

fmean(d, g, nthreads = 1)   # 11 ms  data.frame, 8 columns
fmean(d, g, nthreads = 4)   #  7 ms  -> ~1.6x, threads run across columns

fmean(x, g, nthreads = 4)   #        ~1x for a single grouped vector
```

So: big wins on ungrouped vector reductions, useful gains across the columns of a
data frame, and little for one grouped vector. Set it once globally rather than
per call:

```r
set_collapse(nthreads = 4)                    # session-wide
fmean(d, g, nthreads = 4)                     # or per call
```

Only `fsum`, `fmean`, `fmode`, `fndistinct` and some others accept `nthreads`;
`pivot()` takes one too (minor gains — its grouping stays serial). If collapse
was built without OpenMP the argument is silently ignored.

## Materialize only what you need

Every optional output has a cost. The `GRP` object is the clearest case:

```r
fsum(x, GRP(g))                                                    # 53 ms
fsum(x, GRP(g, sort = FALSE, return.groups = FALSE), use = FALSE)  # 28 ms
```

Nearly 2x, purely from not building things the result does not use:

- `use.g.names = FALSE` (abbreviate `use = FALSE`) — skip constructing group names
  as row/vector names. Real savings with many groups.
- `return.groups = FALSE` — do not materialize the unique grouping columns.
- `return.order = FALSE` — do not keep the ordering vector.
- `na.exclude = FALSE` in `qF()`/`qG()` — skip the missing-value check.
- `drop = TRUE` (the default) on matrix/data.frame methods — return a plain vector
  when there are no groups.

The same instinct applies to intermediates. Use by-reference operations where the
copy is pure waste:

```r
res <- x + y + z            # two copies
res <- x + y; res %+=% z    # one

fmedian(d$x, d$g, TRA = "fill", set = TRUE)   # transform in place, no copy
settransform(d, z = log(v))                    # modify the frame in place
fsum(m, TRA = "/", set = TRUE)                 # matrix columns -> proportions, in place
```

Keep top-level analysis code copy-on-modify where you still want to inspect
intermediates; use `set = TRUE` in loops, in package internals, and on data big
enough that the copy hurts.

## Computing by reference

The `set = TRUE` / `set*` family is not just a micro-optimization — it is what
makes iterative numerical work (balancing, fitting, rescaling) feasible on large
objects, because the loop allocates nothing after the initial object.

The inventory:

| Facility | Does |
|---|---|
| `f<stat>(x, g, TRA = ..., set = TRUE)` | grouped transform in place |
| `setTRA(x, STATS, FUN, g)` | same, standalone |
| `setv(X, v, R, invert, vind1, xlist)` | replace values/indices in place |
| `setop(X, op, V, rowwise)` and `%+=%` `%-=%` `%*=%` `%/=%` | arithmetic in place |
| `settransform()` / `settransformv()` | add/modify columns in place |
| `setrename()`, `setColnames()`, `setattrib()`, `setrelabel()` | metadata in place |
| `replace_na(set = TRUE)`, `replace_inf(set = TRUE)`, `replace_outliers(set = TRUE)`, `na_locf(set = TRUE)`, `na_insert(set = TRUE)` | cleaning in place |

**They all return the object, so they chain.** Every one returns its (mutated)
argument — invisibly for `set = TRUE` — which means a whole sequence of in-place
operations composes in a pipe while touching one allocation:

```r
REIOT_data_FD |> fsum(g_FD, TRA = "/", set = TRUE) |> setv(NaN, 1e-7) %*=% TIVA_FD
REIOT_data    |> fsum(g_IO, TRA = "/", set = TRUE) |> setv(NaN, 1e-7) %*=% TIVA_IO
```

Read left to right: divide every cell by its block total (grouped, in place),
replace the `NaN`s from empty blocks, then multiply by the target table — three
passes over the same memory, no intermediate object at any point. Both `|>` and
`%>%` work here.

**It applies elementwise to lists.** `REIOT_data` above is a *list of matrices*
(one per year). The statistical functions treat each element as a column, so a
grouping vector of length `nrow * ncol` addresses the cells of every matrix, and
`%*=%` pairs the list against another list of the same shape:

```r
L <- list(a = matrix(1:4 + 0, 2, 2), b = matrix(5:8 + 0, 2, 2))
fsum(L, g, TRA = "/", set = TRUE)     # each matrix scaled by its group sums
L %*=% list(a = c(2, 3), b = c(1, 1)) # per element; a length-nrow vector scales rows
```

### The iterative-fitting pattern

Allocate once outside the loop, mutate inside it. Nothing in this loop body
allocates except the small factor vectors:

```r
while (n < 15L && i < 200L) {
  # 1. rescale to match the target aggregates, in place
  REIOT_data_FD |> fsum(g_FD, TRA = "/", set = TRUE) |> setv(NaN, 1e-7) %*=% TIVA_FD
  REIOT_data    |> fsum(g_IO, TRA = "/", set = TRUE) |> setv(NaN, 1e-7) %*=% TIVA_IO

  # 2. bounded adjustment factors, applied in place
  FD_factors <- lapply(REIOT, with, pmin(1.1, pmax(0.9, (colSums(IO) + colSums(VA) -
                                                         rowSums(IO)) / rowSums(FD))))
  REIOT_data_FD %*=% FD_factors

  # 3. IPF to restore the column margins — return value discarded, the mutation is the point
  Map(function(x, z) ipf_2d(x, rowSums(x), z, tol = 1e-3, set = TRUE, check = FALSE),
      REIOT_data, cSums)
}
```

`Map()` here is called purely for effect: `ipf_2d(..., set = TRUE)` writes into the
matrices `REIOT_data` already holds, so the list it returns is thrown away. That
reads oddly if you expect functional style, and it is the whole reason the loop is
cheap. (`ipf_2d()` itself is in `references/recipes.md`; note its `X <- if (set)
seed else seed + 0` — the `+ 0` is the deliberate copy when `set = FALSE`.)

Clamping the factors (`pmin`/`pmax`) is what keeps this stable — an unbounded
ratio on a near-zero denominator sends a cell to infinity and the next pass
propagates it everywhere.

### The trap: no copy means no backup

Assignment does **not** copy, so a "backup" taken before an in-place operation is
mutated too:

```r
L_backup <- L                       # NOT a copy
fsum(L, g, TRA = "/", set = TRUE)
L_backup$a[1]                       # already changed
```

Use `data.table::copy()` (or `L2 <- lapply(L, `+`, 0)`) if you genuinely need the
original afterwards. This is the main reason to keep top-level analysis code
copy-on-modify: reach for `set = TRUE` inside loops, package internals, and on
objects big enough that the copy hurts — not in exploratory code where you still
want to re-run a line and get the same answer.

## Reuse an ordering across order statistics

Quantiles, medians and `fnth()` are dominated by the **sort**, not the selection.
`fnth`/`fmedian`/`fquantile` all take an `o` argument — a pre-computed ordering —
so if you want several order statistics on the same variable, sort once:

```r
wlddev |> fsubset(is.finite(POP)) |> fgroup_by(region) |>
  fmutate(o = radixorder(GRPid(), LIFEEX)) |>
  fsummarise(min    = fmin(LIFEEX),
             Q1     = fnth(LIFEEX, 0.25, POP, o = o, ties = "q8"),
             mean   = fmean(LIFEEX, POP),
             median = fmedian(LIFEEX, POP, o = o),
             Q3     = fnth(LIFEEX, 0.75, POP, o = o, ties = "q8"),
             max    = fmax(LIFEEX))
```

Three weighted order statistics on 2M rows / 2000 groups: **54 ms** sharing one
ordering vs **242 ms** letting each function sort for itself — 4.5x, identical
results.

The key is `radixorder(GRPid(), LIFEEX)`: `GRPid()` returns the group id inside a
grouped operation, so ordering by *(group, value)* produces one vector that is
correctly ordered **within** every group. Computing it in a grouped `fmutate()`
puts it alongside the data at full length, which is what `o =` expects.

`ties = "q8"` selects quantile type 8; see `?fnth` for the full set. Non-order
statistics in the same call (`fmin`, `fmean`, `fmax`) simply ignore `o`.

## Vectorized grouped regression

`fsum(y, g, w = x)` is an inner product Σ y·x by group. That turns an OLS slope
into two sums, with no model matrix and no split:

```r
wlddev |> collap(POP ~ region + year, FUN = fsum) |>
  fmutate(POP = POP / 1e6) |>
  fsubset(is.finite(POP)) |>                        # see the warning below
  fgroup_by(region) |>
  fmutate(dmy = fmean(year, TRA = "-")) |>          # demean the regressor by group
  fsummarise(beta  = fsum(POP, dmy) %/=% fsum(dmy, dmy),
             POP20 = flast(POP)) |>
  fmutate(POP21 = POP20 + beta, POP22 = POP21 + beta,
          POP23 = POP22 + beta, POP24 = POP23 + beta)
```

`sum(y·(x - x̄)) / sum((x - x̄)²)` is exactly the OLS slope, and `%/=%` divides in
place rather than allocating. `flast()` grabs the endpoint, and the trailing
`fmutate()` iterates the linear forecast forward — all without leaving the frame.
Verified to match `lm()` per group to ~1e-13.

> **Align the sample first.** The numerator `fsum(POP, dmy)` skips rows where
> `POP` is `NA` (`na.rm = TRUE`), but `fmean(year, TRA = "-")` and the denominator
> `fsum(dmy, dmy)` use *every* row. If `y` has missing values the two sums run
> over different samples and the slope is biased — on this data, 19.64 instead of
> 22.56 for South Asia, purely from one `NA` year. `fsubset(is.finite(POP))`
> before the demeaning fixes it. The same applies to the `greg()` function in
> `references/programming.md`.

For the general case (many regressors, standard errors) use `flm()`; this idiom
is for when you want one slope per group and nothing else.

## Setting the options globally

```r
set_collapse(sort = FALSE, nthreads = 4, na.rm = FALSE)
get_collapse()                       # read them all back
oldopts <- set_collapse(nthreads = 2)  # returns the OLD values, invisibly
set_collapse(oldopts)                  # restore
```

**Do not add this to a user's code unless they asked, or you are writing a new
script's header, or the project already sets it.** These are session-wide and
several change *results*, not just speed. See the rule in `SKILL.md`.

### The complete option list

`set_collapse()` accepts exactly these (stored in the internal `.op`
environment, which is why they are cheap to read):

| Option | Default | Effect |
|---|---|---|
| `na.rm` | `TRUE` | skip missing values in statistical algorithms. Does **not** affect non-statistical `na.rm` arguments such as `pivot()`'s |
| `sort` | `TRUE` | sorted grouping; also `qF()` and `qtab()`. Excludes `sort` arguments where grouping isn't the objective (`funique`, `pivot`) |
| `nthreads` | `1` | OpenMP threads in the functions that support it |
| `stable.algo` | `TRUE` | `fvar`/`fsd`/`qsu`: `FALSE` uses a fast one-pass SD, risking catastrophic cancellation |
| `stub` | `TRUE` | whether the operators (`W`, `L`, `STD`, …) prefix result column names |
| `verbose` | `1` | diagnostic messages; currently `join()` and `roworder()` |
| `digits` | `2` | printing precision in `descr()`, `pwcor()`, … |
| `mask` | `NULL` | export unprefixed copies — see the namespace section in `SKILL.md` |
| `remove` | `NULL` | un-export functions; keywords `"shorthand"`, `"infix"`, `"operator"`, `"old"` |

Both `mask` and `remove` are reversible (`set_collapse(mask = NULL)`) and both
silently detach/re-attach collapse, putting it at the top of the search path.

### Options set with `options()`, not `set_collapse()`

These affect startup or global behaviour and are **not** in `.op`:

- `collapse_unused_arg_action` — what a generic does when handed an unknown
  argument: `"warning"` (default), `"error"`, `"message"`, `"none"`. The default
  means `fmean(x, bla = 1)` warns rather than failing; `"error"` is worth setting
  while debugging a pipeline that silently ignores a typo'd argument.
- `collapse_export_F` — exports the lead operator `F`, which is **not exported by
  default** since v1.9.0 because of clashes with `base::F`. Without it, use
  `collapse:::F` or `flag(x, -1)`.
- `collapse_nthreads`, `collapse_na_rm`, `collapse_sort`, `collapse_stable_algo`,
  `collapse_verbose`, `collapse_digits`, `collapse_mask`, `collapse_remove` —
  initialize `.op` differently **before the package loads**. Once loaded they do
  nothing; use `set_collapse()`. The place for these is an `.Rprofile` or a
  project `.fastverse` config file, which is also the right answer when a user
  wants a setting to apply across a whole project rather than one script.
- `options("collapse_remove")` genuinely removes functions from the namespace and
  is **not** reversible by `set_collapse(remove = NULL)` — only by reloading.

## A worked before/after

The canonical example from the *collapse for tidyverse users* vignette. Start
with permissible-but-wasteful code:

```r
mtcars |> fgroup_by(cyl) |> fsubset(mpg > 13) |> roworder(mpg)   # collapse refuses this
```

Filtering and reordering *grouped* data means touching both the frame and the
grouping object. collapse blocks the subset and messages on the reorder.

Now the same aggregation, tightened step by step:

```r
# 1. subset the whole frame, compute on a few columns
mtcars |>
  fsubset(mpg > 11) |>
  fgroup_by(cyl, vs, am) |>
  fsummarise(across(c(mpg, carb, hp), fmean), qsec_wt = fmean(qsec, wt))

# 2. select the columns you need during the subset
mtcars |>
  fsubset(mpg > 11, cyl, vs, am, mpg, carb, hp, qsec, wt) |>
  fgroup_by(cyl, vs, am) |>
  fsummarise(across(c(mpg, carb, hp), fmean), qsec_wt = fmean(qsec, wt))

# 3. drop the weighted mean -> every column gets the same statistic,
#    so `across`/`fsummarise` are unnecessary
mtcars |>
  fsubset(mpg > 11, cyl, vs, am, mpg, carb, hp) |>
  fgroup_by(cyl, vs, am) |>
  fmean()

# 4. turn on the options
mtcars |>
  fsubset(mpg > 11, cyl, vs, am, mpg, carb, hp) |>
  fgroup_by(cyl, vs, am, sort = FALSE) |>
  fmean(nthreads = 3, na.rm = FALSE)
```

And for a mutate rather than an aggregation, skip the grouping object entirely:

```r
fmutate(dat, mpg_median = fmedian(mpg, list(cyl, vs, am), TRA = "fill"))
```

The RCA index from the same vignette shows how far this goes — two different
groupings, one expression, no intermediates, all by reference:

```r
settransform(exports,
  RCA = fsum(v, list(c, y), TRA = "/") %/=%
        fsum(fsum(v, y, TRA = "/"), list(s, y), TRA = "fill", set = TRUE))
```

**Measure before you tune.** These levers are real but the ratios depend on data
size, group count, column types, and whether the build has OpenMP. Reach for the
defaults that are free (sequencing, minimal grouping, not materializing) always;
reach for `sort`/`na.rm`/`nthreads` when the job is big enough to care.
