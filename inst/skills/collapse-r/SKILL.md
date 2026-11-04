---
name: collapse-r
description: >-
  Expert guidance for the collapse R package (v2.x) — fast, class-agnostic data
  transformation and statistical computing in R, and the core of the fastverse.
  Covers the Fast Statistical Functions (fmean/fsum/fmedian/fmode/fnth/fndistinct
  … with their g/w/TRA arguments), grouping objects (GRP, qF, qG, group,
  fgroup_by, gsplit), the dplyr-like verbs (fsubset/fselect/fmutate/fsummarise/
  across/roworder/frename/fslice/fcount), join(), pivot(), collap(), panel and
  time-series operations (flag/L, fdiff/D, fgrowth/G, fcumsum, findex_by,
  psmat), summaries (qsu, descr, qtab, pwcor, varying, namlab/vlabels), list
  processing (rsplit, unlist2d, get_elem, rapply2d, t_list), memory-efficient
  programming helpers (setv, whichv, %==%, %+=%, %iin%, alloc, copyv, setop,
  ss), quick conversions (qDF/qDT/qM/qTBL/mctl/mrtl), and namespace masking via
  set_collapse().
  Use this skill WHENEVER the user is manipulating, aggregating, reshaping,
  joining, summarising, or transforming data in R and collapse or the fastverse
  is available or already used in the project; whenever they mention collapse,
  fastverse, set_collapse, any f-prefixed verb, TRA, GRP, fgroup_by, qDT/qM,
  collap, pivot, or the L/D/G/W/B/STD/HDW operators; whenever they need
  grouped/weighted statistics, panel or within/between transformations, or
  fast joins/reshapes on large data; and whenever they are writing an R package
  that uses collapse as a computational backend. Prefer this over ad-hoc
  dplyr/base R so code matches the established fastverse idiom — also when the
  user just says "aggregate/reshape/join/clean this in R" without naming a
  package.
---

# collapse — fast data transformation and statistical computing in R

collapse is a C/C++ package that makes statistical operations **vectorized across
columns and across groups at once**. That single idea is what separates it from
dplyr/data.table: instead of splitting data by group and applying a function, you
pass the grouping to the function. `fmean(x, g)` computes all group means in one C
pass. Everything else — the verbs, the joins, the reshapes — exists to feed those
functions efficiently.

It is also **class-agnostic**: the same code works on vectors, matrices,
data.frames, data.tables, tibbles, grouped_df, sf, pseries/pdata.frame, xts/zoo,
and units objects, preserving their attributes. You never need to convert.

Current version is 2.1.7. Check with `packageVersion("collapse")`; `join()`,
`pivot()`, and `set_collapse()` require ≥ 2.0.0, `fslice()`/`groupv()` ≥ 2.1.0,
and `GRP(drop = FALSE)`/`fcount(drop=)` ≥ 2.1.7.

## Decide the namespace before writing a line

This is the highest-leverage decision and the most common source of broken code.
By default collapse exports **f-prefixed** names only: `fsubset`, `fselect`,
`fmutate`, `fsummarise`, `fgroup_by`, `frename`, `ftransform`. Bare `mutate`,
`subset`, `select`, `group_by`, `summarise`, `n()` are **not available** from a
plain `library(collapse)`.

```r
# --- Analysis / research scripts: mask, so the code reads like dplyr ---
library(fastverse)                                    # or library(collapse)
set_collapse(mask = c("manip", "helper", "special"),  # the standard trio
             nthreads = 4, sort = FALSE)
fastverse_conflicts()                                 # report what got masked
```

- `"manip"` → `subset`, `select`, `mutate`, `summarise`, `transform`, `group_by`,
  `rename`, `slice`, `compute`, `index_by`, `ungroup`, …
- `"helper"` → `unique`, `duplicated`, `count`, `quantile`, `range`, `dist`,
  `droplevels`, `interaction`, `match`, `nrow`, `ncol`, …
- `"special"` → `n()` (= `GRPN`), `table()` (= `qtab`), and a fast `%in%`.
- `"all"` additionally masks the statistical and transformation functions —
  `sum`, `mean`, `median`, `min`, `max`, `sd`, `var`, `mode`, `first`, `last`,
  `scale`, `diff`, `cumsum`, `between`, `within`, … (59 names in total).
  Powerful, but it silently turns every `sum()`/`mean()` inside
  `fmutate`/`fsummarise` into a vectorized grouped call, and shadows base
  `scale`/`diff`/`within` project-wide. Only use it deliberately, and reach for
  `base::sum()` when you want the split-apply behaviour back.

```r
# --- Package code: NEVER call set_collapse(mask=) — it leaks into user sessions ---
#' @importFrom collapse fsubset fmutate fsum GRP get_vars add_vars
```

If you are editing an existing project, **read the top of a nearby script and
mirror its choice**. Writing `mutate()` into a script that never called
`set_collapse(mask = "manip")` produces a dplyr call (or an error), silently
losing all the performance the code was written for.

### Do not call `set_collapse()` on the user's behalf

`set_collapse()` is **global and session-wide**: it changes defaults for every
later call, in the user's code and in every other package that uses collapse.
Options like `sort` and `na.rm` change *results*, not just speed, and `mask`
rewrites the exported namespace and moves collapse to the top of the search path.
None of that is yours to decide silently.

So: write code that works under whatever settings are already in force. Add a
`set_collapse()` call only when

- the user asked for it ("make this faster", "set up masking", "use 4 threads"), or
- you are writing a **new standalone script** and are setting its header — in
  which case say so in one line, and prefer mirroring a sibling script's header
  over inventing one, or
- the project already sets it and you are matching that convention.

Never add it to package code (see `references/programming.md`), never inside a
function without `on.exit()` restoring the old value, and do not sprinkle
`nthreads`/`na.rm`/`sort` into individual calls as a reflex — reach for them when
the job is large enough to care. The complete option list is in
`references/efficiency.md`.

## House style: full names, piped, one verb per line

Two rules, both about the next person to read the code.

**1. Write the full function names.** collapse ships terse aliases — `slt`, `sbt`,
`gby`, `smr`, `mtt`, `tfm`, `tfmv`, `settfm`, `settfmv`, `gv`, `gvr`, `nv`, `av`,
`rnm`, `iby`, `ix`, `itn`. Do not write them. Use `fselect`, `fsubset`,
`fgroup_by`, `fsummarise`, `fmutate`, `ftransform`, `ftransformv`,
`settransform`, `settransformv`, `get_vars`, `get_vars(..., regex = TRUE)`,
`num_vars`, `add_vars`, `frename`, `findex_by`, `findex`, `finteraction`.

**2. Pipe; do not nest.** Every collapse verb takes the data as its first
argument, so chains read top-to-bottom with one verb per line. Nesting them
inside-out is how collapse code gets its undeserved reputation for being cryptic.

```r
smr(sbt(d, x > 0, id, v), m = fmean(v))     # don't — aliases, and inside-out

d |>                                         # do
  fsubset(x > 0, id, v) |>
  fsummarise(m = fmean(v))
```

The two together are what make a pipeline skimmable — each line is one verb, and
each verb says what it does:

```r
exports |>
  fsubset(year > 2010, country, sector, value) |>
  fgroup_by(country, sector) |>
  fsummarise(value = fsum(value)) |>
  roworder(-value)
```

Use the native `|>`. Reach for magrittr's `%>%` only when you actually need the
`.` placeholder (an argument other than the first, or referring to the data twice
in one step) — and when a step needs `.` twice, an intermediate variable is
usually clearer than a `%>% {...}` block.

The aliases are identical in behaviour, so none of this changes results —
`set_collapse(remove = "shorthand")` deletes them from the namespace entirely if
you want the rule enforced rather than remembered. You still need to
**recognize** them when reading existing code; the alias table is in
`references/data-manipulation.md`.

Two names that look like shorthands but are not, and should be used freely:
`ss()` (the programmer's `[`, not an alias for `fsubset`) and `.c()`
(non-standard concatenation).

**3. Write the direct form, not the defensive one.** The most common way to
write bad collapse code is not getting it wrong — it is reaching past the simple
call for something more elaborate:

| Reflex | Direct form |
|---|---|
| `get_vars(d, c("a", "b"))` | `fselect(d, a, b)` — quote names only when they come from a variable |
| `fsummarise(across(cols, fmean))` | `fgroup_by(g) \|> get_vars(cols) \|> fmean()` when every column gets the same statistic |
| `value / fsum(value, g, TRA = "fill")` | `fsum(value, g, TRA = "/")` — one pass, no extra copy |
| `join(x, y, on = "id", validate = "1:1")` | `join(x, y, on = "id")` — validation costs a pass; add it when the check is wanted, not by reflex |
| `get_vars(d, cols) / d$total` | `get_vars(d, cols) %c/% d$total`, or ``settransformv(d, cols, `/`, total)`` |
| `rowSums(!is.na(get_vars(d, cols))) > 0` | `!missing_cases(d, cols, prop = 1)` |
| zero-filling `NA` weights so `fmedian()` stops erroring | let it error, or drop the rows deliberately |

The point is not brevity for its own sake — each left-hand form allocates
something the right-hand form doesn't, and hides a decision the reader would want
to see.

## The core mechanic: `g`, `w`, `TRA`

Every *Fast Statistical Function* — `fsum fprod fmean fmedian fmode fvar fsd fmin
fmax fnth ffirst flast fnobs fndistinct` — has the same signature:

```r
f<stat>(x, g = NULL, w = NULL, TRA = NULL, na.rm = TRUE, use.g.names = TRUE, nthreads = 1)
```

- `x` — vector, matrix, data.frame, or grouped_df. One function, all shapes.
- `g` — grouping: a vector, list of vectors, factor, `qG`, or `GRP` object.
- `w` — weights. `fmean(x, w = pop)` is a weighted mean; `fmedian`, `fmode`,
  `fnth`, `fvar`, `fsd` are all weighted too.
- `TRA` — instead of returning one value per group, sweep the statistic back onto
  the data at full length. This is the feature people miss most.

```r
fmean(mtcars$mpg, mtcars$cyl)                 # aggregate:  3 group means
fmean(mtcars$mpg, mtcars$cyl, TRA = "fill")   # expand:     32 values, group mean
fmean(mtcars$mpg, mtcars$cyl, TRA = "-")      # centre by group mean
fsum(x, list(country, year), TRA = "/")       # share of group total — no join needed
```

`TRA` operations: `"replace_na"`/`"na"`, `"fill"`, `"replace"`, `"-"`, `"-+"`,
`"/"`, `"%"`, `"+"`, `"*"`, `"%%"`, `"-%%"`. `"fill"` replaces missing values too,
`"replace"` keeps them missing. Add `set = TRUE` to modify **by reference**, no
copy: `fmedian(df$x, df$g, TRA = "fill", set = TRUE)`.

Named shortcuts for the common cases: `fbetween`/`B` (= `TRA = "fill"` of the
mean), `fwithin`/`W` (= `TRA = "-"`), `fscale`/`STD` (grouped standardization),
`fhdwithin`/`HDW` (centre on multiple factors / partial out a regression).

## Eager vectorization inside `fmutate`/`fsummarise`

collapse reads the expression you type. If it contains a Fast Statistical
Function, the whole expression is executed **once** with the grouping passed
down; otherwise it falls back to split-apply-combine via `gsplit()`.

```r
mtcars |> fgroup_by(cyl) |> fsummarise(x = fmean(mpg) + fmin(qsec))  # fmean(mpg,g) + fmin(qsec,g)
mtcars |> fgroup_by(cyl) |> fsummarise(x = fmean(mpg) +  min(qsec))  # grouped mean + OVERALL min (!)
mtcars |> fgroup_by(cyl) |> fsummarise(x =  mean(mpg) +  min(qsec))  # split-apply-combine, per group
```

This is intentional and lets you vectorize complex expressions, but it means
**mixing prefixed and unprefixed statistics in one expression is almost always a
bug**. Only expressions you type out are inspected — a statistic hidden inside
`myfun <- function(x) fmean(x) + fmin(x)` is not vectorized.

Prefer to avoid `fgroup_by` for mutate-style work entirely — internal grouping is
faster and reads fine:

```r
dat |> fmutate(med = fmedian(mpg, list(cyl, vs, am), TRA = "fill"))   # best
dat |> fgroup_by(cyl, vs, am, return.groups = FALSE) |> fmutate(...)  # if reusing the grouping
```

## Which function do I need?

| Task | Function |
|---|---|
| Filter rows (+ select columns in the same call) | `fsubset(d, cond, col1, col2)` |
| Filter by position, programmatically | `ss(d, i, j, check = FALSE)` |
| Select / drop columns | `fselect(d, a, b, -c)`; by name/index/predicate `get_vars`, by regex `get_vars(d, re, regex = TRUE)`, by type `num_vars`, `cat_vars`, `char_vars` |
| Add / modify columns | `fmutate` (sequential, `across`), `ftransform` (parallel), `settransform` (by reference) |
| Keep only computed columns | `fcompute(d, ...)` ; `fmutate(..., .keep = "used")` |
| Apply a function to many columns | `across()` inside `fmutate`/`fsummarise`; `ftransformv(d, cols, FUN, ..., apply = FALSE)`; or `dapply` |
| Aggregate | `fgroup_by()` then `fsummarise()`, or `f<stat>(d, g)`, or `collap()` for multi-type |
| Weighted statistics | the `w =` argument: `fmean`, `fmedian`, `fnth`, `fmode`, `fsd`, `fvar`, `fquantile`, `collap`, `qsu`, `pwcor` |
| Centre / demean / standardize by group | `fwithin`/`W`, `fbetween`/`B`, `fscale`/`STD`; multi-factor `fhdwithin`/`HDW` |
| Share / percentage of a group total | `f<stat>(x, g, TRA = "/")` — no join, no `fgroup_by` |
| Sort rows / columns | `roworder(d, -x)`, `roworderv`; `colorder`, `colorderv` |
| Join | `join(x, y, on, how)` |
| Reshape | `pivot(d, ids, values, names, how = "longer"/"wider"/"recast")` |
| Bind rows | `rowbind(...)`; columns: `add_vars(x, y)` |
| Unique / duplicates / counts | `funique`, `fduplicated`, `fnunique`, `any_duplicated`, `fcount` |
| Rename | `frename(d, new = old)` or `frename(d, toupper)`; by reference `setrename` |
| Convert | `qDF`, `qDT`, `qTBL`, `qM`, `qF`, `mctl`, `mrtl` |
| Lags / differences / growth | `flag`/`L`, `fdiff`/`D`, `Dlog`, `fgrowth`/`G`, `fcumsum` |
| Panel structure | `findex_by`, `findex`, `unindex`, `is_irregular`, `psmat` |
| Describe data | `descr`, `qsu`, `qtab`, `namlab`, `vlabels`, `varying`, `pwcor` |
| Quantiles, range, distances | `fquantile`, `fnth`, `frange`, `fdist` (reuse one `radixorder` via `o =` for several) |
| Linear models / fixed effects | `flm`, `fFtest`; absorb factors with `fhdwithin`/`HDW` |
| Build a reusable grouping | `GRP(d, by)`, `qF`, `qG`, `group`; split/recombine `gsplit`/`greorder`, `BY` |
| Nested lists | `rsplit`, `unlist2d`, `get_elem`, `rapply2d`, `t_list`, `atomic_elem` |
| Replace values fast | `setv`, `copyv`, `replace_na`, `replace_inf`, `replace_outliers`, `recode_char`, `recode_num` |
| Avoid logical vectors | `whichv`, `whichNA`, `%==%`, `%!=%`, `%iin%`, `%!iin%`, `anyv`, `allv`, `allNA` |
| In-place arithmetic | `%+=%`, `%-=%`, `%*=%`, `%/=%`, `setop`; row/col-wise `%r*%`, `%c-%`, … |
| Make existing code faster | `references/efficiency.md` — sequencing, minimal grouping, `sort`/`na.rm`/`nthreads`, by-reference |

## Gotchas that bite immediately

1. **`na.rm = TRUE` is the default** everywhere — the opposite of base R. Set
   `set_collapse(na.rm = FALSE)` if the data has no missing values (it is also
   faster).
2. **`join()` takes only the first match by default** (`multiple = FALSE`), does
   not sort (`sort = FALSE` preserves `x` order), and does not validate
   (`validate = "m:m"`). For a dplyr-`left_join`-like cartesian result you must
   pass `multiple = TRUE`. Use `validate = "1:1"` / `require = list(x = 0.9)` to
   assert what you expect.
3. **`fsubset`'s `...` are columns to keep**, not extra conditions:
   `fsubset(d, x > 1 & y < 2, id, x, y)`. Combine conditions with `&`.
4. **No tidyselect.** No `starts_with()`, `where()`, `c_across()`, no `~ .x`
   lambdas. Use `get_vars(d, "^pt_", regex = TRUE)` for regex, `across(is.numeric, fmean)` for
   predicates, and plain function names or `\(x) ...` for anonymous functions.
5. **`across()` is not a real function** — it is an expression parsed by
   `fmutate`/`fsummarise`. `collapse::across()` does not work.
6. **`fsubset()` is not allowed on grouped data** and `roworder()` messages about
   it — this is deliberate: subset and sort *before* grouping.
7. **`qDF`/`qDT`/`qM` drop non-essential attributes** by default. Pass
   `keep.attr = TRUE` if you need them.
8. **`set_collapse(sort = FALSE)` changes result ordering globally**, including
   inside packages that use collapse. Great for speed, but pass a sorted factor
   (`qF(g, sort = TRUE)`) where output order matters.
9. **Infix operators evaluate strictly left to right** — `a %+=% b * 2` is not
   what you think. Parenthesize.
10. **Operators `L`/`D`/`G`/`W`/`B`/`STD`/`HDW` add name prefixes** to
    matrix/data.frame columns (`L1.x`, `W.x`). Their `f*` counterparts
    (`flag`, `fdiff`, `fwithin`, …) do not, unless `stubs = TRUE`.

`references/gotchas.md` has the full list, including panel/time handling and
attribute-preservation surprises.

## Reference files

Read the one that matches the task rather than guessing at arguments:

- **`references/statistical-functions.md`** — the Fast Statistical Functions in
  detail, the full `TRA` table, weights, `fscale`/`fbetween`/`fwithin`/`fhdwithin`,
  `dapply`/`BY`, `flm`/`fFtest`, `fquantile`/`frange`/`fdist`.
- **`references/data-manipulation.md`** — every verb with its real signature:
  subsetting, selection, mutate/transform/compute, `across`, summarise, ordering,
  renaming, unique/count, quick conversions.
- **`references/join-pivot-aggregate.md`** — `join()` semantics and diagnostics,
  `pivot()` in all three directions (including labels), `collap()` for multi-type
  and weighted aggregation, `rowbind()`.
- **`references/time-series-panel.md`** — `flag`/`fdiff`/`fgrowth`/`fcumsum`, why
  passing `t` matters on irregular panels, indexed frames, `psmat`/`psacf`,
  `seqid`/`timeid`, `xts`/`plm` interop.
- **`references/summaries-and-lists.md`** — `qsu`, `descr`, `qtab`, `pwcor`,
  `varying`, labels (`vlabels`/`namlab`/`setrelabel`), and the list-processing
  toolkit (`rsplit`, `unlist2d`, `get_elem`, `rapply2d`, `t_list`).
- **`references/efficiency.md`** — writing fast collapse code: sequencing,
  minimal grouping, `sort`/`na.rm`/`nthreads`, controlling what gets
  materialized, **computing by reference** (chained `set = TRUE` ops, iterative
  fitting loops, the aliasing trap), reusing one ordering across several order
  statistics, and vectorized grouped regression. Measured timings throughout.
- **`references/programming.md`** — `GRP`/`qF`/`qG`/`group`/`gsplit`/`greorder`,
  choosing the minimal grouping object, memory-efficient programming (`setv`,
  `whichv`, `alloc`, `copyv`, `setop`, `%+=%`), and rules for using collapse
  inside an R package.
- **`references/recipes.md`** — worked patterns lifted from production research
  code: shares and indices via `TRA`, deduplication by weighted mode (including
  **spatial dedup** by grid-snapping), panel prep, wide↔long for plotting and
  LaTeX tables, **input-output tables** (bi-directional aggregation, Leontief
  coefficients, block matrices, IPF/RAS balancing), grid aggregation, graph
  building, `sf` and `data.table` interop.
- **`references/gotchas.md`** — the complete pitfall list with the correct code
  next to the broken code.

A companion skill, **`fastverse-r`**, covers the rest of the stack that
`library(fastverse)` attaches — **data.table**, **kit** and **magrittr** — and
the division of labour between them and collapse. Consult it when the task
needs something collapse does not have (rolling windows, rolling/non-equi
joins, conditional `:=` updates, row-wise statistics across columns, `topn`,
vectorized switches, `fread`/`fwrite`), or when deciding whether a given step
belongs in collapse or in a data.table `[i, j, by]`.

## Writing efficient code

collapse usually offers several routes to the same answer, and they differ mainly
in **efficiency**. The habit worth building is to think about what the machine has
to do and craft the minimal solution — not to reach for the first thing that
works. Full treatment with measured timings in `references/efficiency.md`.

**Sequence operations so less data moves.** Subset and select in the *same* call,
before grouping: `fsubset(d, cond, only, the, needed, cols) |> fgroup_by(g) |> fmean()`
(~1.65x faster than filtering wide and selecting later). collapse enforces part of
this — `fsubset()` refuses grouped data and `roworder()` messages about it.

**Use the minimal grouping the job needs.** Don't reach for `fgroup_by()` unless
several different functions share one grouping; there is no `.by =` argument
because internal grouping *is* the `.by`:

```r
fmutate(d, med = fmedian(v, list(cyl, vs, am), TRA = "fill"))   # best
d |> fgroup_by(g, return.groups = FALSE) |> fmutate(...)        # if reusing the grouping
```

**Turn on the three options once, at the top.** Unsorted grouping and no
missing-value check are faster; `nthreads` parallelizes ungrouped reductions
(~4–7x) and work across data-frame columns (~1.6x), though little for a single
grouped vector:

```r
set_collapse(sort = FALSE, nthreads = 4, na.rm = FALSE)
```
`sort = FALSE` changes result *order* to first-appearance; `na.rm = FALSE` is only
safe when the data has no `NA`s. Statistical functions have no `sort` argument —
pass `qF(g, sort = TRUE)` where sorted output is required regardless.

**Materialize only what you need.** `use.g.names = FALSE`, `return.groups = FALSE`,
`return.order = FALSE`, `na.exclude = FALSE` — a lean `GRP` object was ~1.9x
faster than a full one here. Same instinct for intermediates: `settransform`,
`setv`, `%+=%`, `set = TRUE` where the copy is pure waste.

**Prefer indices to logical vectors.** `whichv(x, 0)` / `x %==% 0` returns
positions; `setv(x, 0, NA)` replaces without allocating a mask at all.

**Let the C code do the work.** If you are writing `lapply` over groups,
`sapply(split(...))`, or a join purely to attach a group aggregate, there is
almost certainly a `g`/`TRA`/`w`/`o` argument that removes it. Two idioms worth
knowing by heart, both in `references/efficiency.md`: reusing one `radixorder()`
across several order statistics (~4.5x), and `fsum(y, g, w = x)` as an inner
product for grouped regression slopes.

For anything not covered here, the package's own documentation is unusually
complete: `help("collapse-documentation")` is a hierarchical index of every
function, and `?fsummarise`, `?TRA`, `?join`, `?pivot`, `?GRP` all have extensive
worked examples. The JSS article (doi:10.18637/jss.v116.i01) is the concise
reference.
