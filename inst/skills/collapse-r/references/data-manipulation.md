# Data manipulation verbs

Every verb here works identically on data.frame, data.table, tibble, sf, and
indexed/grouped frames, and returns the input class.

**Write the full names, not the shorthands.** collapse ships terse aliases, and
you will meet them in existing code, but they should not go into code you write —
they are unreadable to anyone who hasn't memorized the table, and the long names
cost almost nothing. `set_collapse(remove = "shorthand")` deletes them from the
namespace if you want the rule enforced.

| Alias | Write instead | | Alias | Write instead |
|---|---|---|---|---|
| `sbt` | `fsubset` | | `gv` | `get_vars` |
| `slt` | `fselect` | | `gvr` | `get_vars(..., regex = TRUE)` |
| `mtt` | `fmutate` | | `nv` | `num_vars` |
| `smr` | `fsummarise` | | `av` | `add_vars` |
| `gby` | `fgroup_by` | | `rnm` | `frename` |
| `tfm` / `tfmv` | `ftransform` / `ftransformv` | | `iby` | `findex_by` |
| `settfm` / `settfmv` | `settransform` / `settransformv` | | `ix` | `findex` |
| | | | `itn` | `finteraction` |

`ss()` and `.c()` look like shorthands but are not — `ss()` is the programmer's
`[` (distinct from `fsubset`) and `.c()` is non-standard concatenation. Use both
freely.

## Table of contents
- [Subsetting rows](#subsetting-rows)
- [Selecting columns](#selecting-columns)
- [Adding and modifying columns](#adding-and-modifying-columns)
- [`across()`](#across)
- [Summarising](#summarising)
- [Ordering rows and columns](#ordering-rows-and-columns)
- [Renaming and relabelling](#renaming-and-relabelling)
- [Unique values, duplicates, counts](#unique-values-duplicates-counts)
- [Missing values](#missing-values)
- [Recoding and replacing](#recoding-and-replacing)
- [Quick conversions](#quick-conversions)

## Subsetting rows

```r
fsubset(.x, subset, ...)
ss(x, i, j, check = TRUE)     # programmer's `[` with drop = FALSE
fslice(.x, ..., n = 1, how = "first", order.by = NULL, na.rm = TRUE, sort = FALSE, with.ties = FALSE)
fslicev(x, n, how, ...)       # standard-evaluation version
```

The `...` of `fsubset` are **columns to keep** (with optional renaming), not extra
conditions. Combine conditions with `&`/`|`.

```r
fsubset(airquality, Temp > 90, Ozone, Temp)
fsubset(airquality, Temp > 90, OZ = Ozone, Temp)      # rename while subsetting
fsubset(airquality, Day == 1, -Temp)                  # drop a column
fsubset(airquality, Day == 1, Ozone:Wind, Month)      # ranges
fsubset(airquality, Day %==% 1, -Temp)                # %==% returns indices: faster on big data
```

`ss()` is the fast `[`: `ss(d, 1:10, 2:3)`. Use `check = FALSE` inside loops and
package code when the indices are known-good.

Grouped subsetting is deliberately **not supported** — subset first, then group.
For a grouped "top n rows", use `fslice`, or
`fsubset(d, data.table::rowid(id1, id2) <= n)`.

```r
fslice(mtcars, cyl, n = 2, how = "min", order.by = mpg)   # 2 lowest-mpg rows per cyl
```

## Selecting columns

Two families, and the choice between them is not about speed — it is about
whether the column names are **literal or held in a variable**:

```r
fselect(d, iso3c, region, year, POP)          # names typed out  -> fselect
vars <- c("iso3c", "region", "year", "POP")
get_vars(d, vars)                             # names in a variable -> get_vars
```

Writing `get_vars(d, c("iso3c", "region", "year", "POP"))` when you could have
typed the names is noise: you pay quoting and a `c()` for nothing, and it reads
as if the selection were dynamic when it isn't. `get_vars` earns its place when
the selection is computed — a variable, indices, a regex, or a predicate.

`fselect` is non-standard evaluation (interactive, dplyr-like); `get_vars` and
friends take names, indices, logicals, regex, or predicate functions
(programmatic, and marginally faster).

```r
fselect(.x, ..., return = "data")
fselect(x, ...) <- value                     # replacement / deletion

get_vars(x, vars, return = "data", regex = FALSE, rename = FALSE, ...)
get_vars(x, vars) <- value

add_vars(x, ..., pos = "end")                # cbind that keeps class
add_vars(x, pos = "front") <- value

num_vars(x)   cat_vars(x)   char_vars(x)   fact_vars(x)   logi_vars(x)   date_vars(x)
```

```r
fselect(wlddev, Country = country, Year = year, ODA)   # select + rename
fselect(wlddev, -country, -year)                       # negative selection with `-`, not `!`
fselect(wlddev, country, PCGDP:ODA)                    # ranges
fselect(wlddev, PCGDP:ODA) <- NULL                     # efficient deletion

get_vars(wlddev, 9:12)                                 # by index
get_vars(wlddev, c("PCGDP","LIFEEX"))                  # by name
get_vars(wlddev, "^pt_", regex = TRUE)                 # by regex — the tidyselect replacement
get_vars(wlddev, is.numeric)                           # by predicate function
get_vars(d, c("_VA$", "_EMP$"), regex = TRUE)          # several regexes, applied in sequence
get_vars(d, "sector", regex = TRUE, invert = TRUE)     # everything EXCEPT the matches
```

`return =` controls what comes back: `"data"` (default), `"names"`, `"indices"`,
`"named_indices"`, `"logical"`, `"named_logical"`. Very useful for programming:
`fselect(d, a:f, return = "indices")`.

`num_vars()` uses a C-level definition equivalent to
`typeof(x) %in% c("integer","double") && !inherits(x, c("factor","Date","POSIXct","yearmon","yearqtr"))`.
It does not dispatch on `is.numeric` methods — use `get_vars(d, is.numeric)` if a
class defines its own.

## Adding and modifying columns

```r
fmutate(.data, ..., .keep = "all", .cols = NULL)   # sequential, supports across()
ftransform(.data, ...)                             # parallel evaluation, no across()
ftransformv(.data, vars, FUN, ..., apply = TRUE)   # apply FUN to selected columns
settransform(.data, ...)                           # modifies in place, assigns back
settransformv(.data, vars, FUN, ...)               # same, by reference
ftransform(.data) <- value                         # replace/add a list of columns
fcompute(.data, ..., keep = NULL)                  # return ONLY the computed columns
fcomputev(.data, vars, FUN, ..., keep = NULL)
```

- `fmutate` evaluates expressions **sequentially** (later ones see earlier ones)
  and supports `across()` and grouped data. `.keep = "used"/"unused"/"none"/"all"`.
- `ftransform` evaluates in the data environment in one go — leaner, no `across`.
  Setting a column to `NULL` deletes it.
- `settransform` modifies the object **in the calling environment** — no
  copy, no reassignment needed. This is the workhorse in scripts.

```r
dat |> fmutate(logv = log(v), share = fsum(v, list(c, y), TRA = "/"))
settransform(exports, RCA = fsum(v, list(c, y), TRA = "/") %/=%
                      fsum(fsum(v, y, TRA = "/"), list(s, y), TRA = "fill", set = TRUE))
ftransformv(GGDC10S, 6:16, `*`, 100/SUM)                 # scale many columns by another
ftransformv(dat, is.numeric, replace_outliers, 5)        # predicate selection
ftransformv(dat, is.character, qF)                       # bulk type conversion
ftransform(dat, unwanted = NULL)                         # delete
```

`apply = TRUE` (default in `*v` functions) applies `FUN` column by column;
`apply = FALSE` passes the whole selected block to `FUN` at once — needed for
functions with a data.frame method (`fmedian`, `fscale`, `flag`, …), and much
faster:

```r
ftransformv(dat, c(mpg, disp, qsec), fmedian, list(cyl, vs, am), TRA = "fill", apply = FALSE)
```

`FUN` can be a bare operator, and the `...` are evaluated **inside the data**, so
rescaling a block of columns by another column is one call — no selection,
division, and re-assignment:

```r
get_vars(va, sect) <- get_vars(va, sect) / va$SUM * 100   # don't
settransformv(va, sect, `/`, SUM/100)                     # do — by reference, no copy
```

## `across()`

```r
across(.cols = NULL, .fns, ..., .names = NULL, .apply = "auto", .transpose = "auto")
```

`across()` is not a function — it is an expression recognized inside `fmutate` and
`fsummarise` (so `collapse::across()` fails). It supports column names, ranges,
indices, logical vectors, and **predicate functions used bare**:
`across(is.numeric, fmean)` — there is no `where()`. There are no purrr lambdas;
use a named function or `\(x) ...`.

```r
dat |> fgroup_by(cyl, vs) |> fsummarise(across(c(mpg, hp), fmean),
                           across(disp, list(md = fmedian, sd = fsd)))
dat |> fmutate(across(c(mpg, disp), fmedian, list(cyl, vs), TRA = "fill"))
fsummarise(d, across(PCGDP:GINI, fmean, w = POP))            # extra args go through ...
fsummarise(d, across(a:c, fmean, .names = "flip"))           # fmean_a instead of a_fmean
fsummarise(d, across(a:c, list(fmean, fsd), .names = \(c, f) paste0(f, "_", c)))
```

`.apply = FALSE` hands all selected columns to the function at once — required
for multivariate functions (`pwcor`, `lm`) and for operators that rename
(`W`, `STD`, `L`), usually together with `.names = FALSE`.

## Summarising

```r
fsummarise(.data, ..., keep.group_vars = TRUE, .cols = NULL)   # fsummarize also works
fgroup_by(.X, ..., sort, decreasing, na.last, return.groups = TRUE, return.order, .drop = TRUE)
group_by_vars(X, by)          # standard-evaluation version — use this in packages
fgroup_vars(X, return = "data")
fungroup(X)
```

**When every column gets the same statistic, do not write `fsummarise(across(...))`.**
Select the columns on the grouped frame and call the function on it — shorter,
more legible, and slightly faster, because `across()` has nothing to dispatch on:

```r
va |> fgroup_by(Country) |> fsummarise(across(sect, fmean))   # don't
va |> fgroup_by(Country) |> get_vars(sect) |> fmean()         # do
```

`fselect`/`get_vars` keep the grouping attached, so the statistic still runs by
group. Reserve `fsummarise` for the cases it is actually for: different
expressions per output column, or several functions at once.

```r
mtcars |> fgroup_by(cyl, vs, am) |> fselect(mpg, carb, hp) |> fmean()
mtcars |> fgroup_by(cyl, vs, am) |> fsummarise(mpg = fsum(mpg), across(c(carb, hp), fmean))
mtcars |> fgroup_by(cyl) |> fsummarise(n = GRPN(), mu = fmean(mpg))   # n() with mask="special"
```

`fgroup_by(..., return.groups = FALSE)` skips materializing the unique group
values; use it when you only mutate. `.drop = FALSE` (v2.1.7+, needs at least one
factor) retains unobserved factor-level combinations.

A `GRP_df` prints the data plus a grouping summary and continues to behave as its
original class. collapse also reads `dplyr::group_by()` groupings.

## Ordering rows and columns

```r
roworder(X, ..., na.last = TRUE, verbose)          # NSE: roworder(d, -x, y)
roworderv(X, cols = NULL, neworder = NULL, decreasing = FALSE, na.last = TRUE,
          pos = "front")                           # SE / explicit permutation
colorder(X, ..., pos = "front")                    # pos: "front"/"end"/"exchange"/"after"/"before"
colorderv(X, neworder, pos = "front")
```

`colorder()` also **renames** while reordering, which is a compact way to put a
frame into its final shape in one call:

```r
colorder(d, source, year, country = cntry, sect)   # moves those to the front, renames cntry
```

```r
roworder(dat, -value)                              # descending
roworder(dat, country, -year)
roworderv(dat, c("country","year"), decreasing = c(FALSE, TRUE))
roworderv(dat, neworder = idx)                     # explicit row permutation
colorder(dat, id, Total, pos = "after")
```

`fsubset(d, order(x, decreasing = TRUE))` also appears in real code and is fine
for small data; `roworder` uses radix sort and is faster.

Row binding: `rowbind(..., idcol = NULL, row.names = FALSE, use.names = TRUE, fill = FALSE, id.factor)`
— accepts loose objects or a single list, fills missing columns with `fill = TRUE`,
and is a drop-in `data.table::rbindlist`.

## Renaming and relabelling

```r
frename(.x, ..., cols = NULL)
setrename(.x, ...)                 # by reference
frename(d, newname = oldname)
frename(d, toupper)                # apply a function to all names
frename(d, tolower, cols = is.numeric)
frename(d, gsub, pattern = "_x$", replacement = "")
add_stub(x, stub, pre = TRUE)      # prefix/suffix all names
rm_stub(x, stub, pre = TRUE)
setColnames / setRownames / setDimnames / setLabels / setrelabel
```

## Unique values, duplicates, counts

```r
funique(x, cols = NULL, sort = FALSE, method = "auto")   # returns x unchanged (no copy) if all unique
fnunique(x)              fduplicated(x, all = FALSE)            any_duplicated(x)
fcount(x, ..., w = NULL, name = "N", add = FALSE, sort = FALSE, decreasing = FALSE, drop = TRUE)
fcountv(x, cols = NULL, ...)                             # SE version
fmatch(x, table, nomatch = NA_integer_, count = FALSE, overid = 1)
%iin% / %!iin%           # like %in%/%!in% but return INDICES
ckmatch(x, table, e = "Unknown columns:")                # match with an informative error
```

```r
fcount(dat, region, year)                  # counts
fcount(dat, region, add = TRUE)            # append the count column to dat
fcount(dat, region, w = pop, name = "POP") # weighted
letters %iin% c("a","b")                   # 1:2 — use directly for subsetting
fsubset(wlddev, iso3c %iin% c("USA","DEU"))
```

`ckmatch()` is the idiomatic way to validate user-supplied column names in
package code: it errors listing exactly which entries were not found.

## Missing values

```r
na_omit(X, cols = NULL, na.attr = FALSE, prop = 0, ...)   # much faster than na.omit
missing_cases(X, cols, prop = 0, count = FALSE)
na_rm(x)                                                  # x[!is.na(x)] for vectors, drops NULL list elements
whichNA(x, invert = FALSE)
allNA(x)   anyNA(x)
na_insert(x, prop = 0.1, value = NA, set = FALSE)
na_locf(x, set = FALSE)   na_focb(x, set = FALSE)         # last/first observation carried forward/back
replace_na(X, value = 0, cols = NULL, set = FALSE)        # alias replace_NA
```

`na_omit(d, cols = is.numeric)` drops rows missing on numeric columns only.
`na_locf`/`na_focb` are **not** vectorized across groups — inside a grouped
`fmutate` they are split-applied; add `set = TRUE` to avoid intermediates.

`missing_cases()` replaces the `rowSums(is.na(...))` family, which materializes a
full logical matrix just to count:

```r
rowSums(!is.na(get_vars(d, cols))) > 0   # don't — allocates an n x k logical matrix
!missing_cases(d, cols, prop = 1)        # do — "not all of cols are missing"
missing_cases(d, cols)                   # any of cols missing
missing_cases(d, cols, prop = 0.5)       # at least half of cols missing
missing_cases(d, cols, count = TRUE)     # how many are missing per row
```

## Recoding and replacing

```r
recode_num(X, ..., default = NULL, missing = NULL, set = FALSE)
recode_char(X, ..., default, missing, regex = FALSE, ignore.case = FALSE, fixed = FALSE, set = FALSE)
replace_inf(X, value = NA, replace.nan = FALSE, set = FALSE)   # alias replace_Inf
replace_outliers(X, limits, value = NA, single.limit = "SDs", ignore.groups = FALSE, set = FALSE)
pad(X, i, value = NA, method = "auto")
```

```r
recode_char(x, ABC = "abc", DEF = "def", default = NA)
recode_char(x, `road|street` = "road", regex = TRUE)
replace_outliers(X, 5)                              # |z| > 5 -> NA (single.limit = "SDs")
replace_outliers(X, c(2, 100))                      # outside [2,100] -> NA
replace_outliers(X, 1, NA, "min")                   # below 1 -> NA
```

For pure value replacement, `setv()`/`copyv()` (see `references/programming.md`)
are faster than any of these, and `kit::vswitch()`/`nswitch()` beat long
`recode_*` chains.

## Quick conversions

```r
qDF(X, row.names.col = FALSE, keep.attr = FALSE, class = "data.frame")
qDT(X, row.names.col = FALSE, keep.attr = FALSE, class = c("data.table","data.frame"))
qTBL(X, ...)
qM(X, row.names.col = FALSE, keep.attr = FALSE, class = NULL)     # to matrix
qF(x, ordered = FALSE, na.exclude = TRUE, sort = TRUE, drop = FALSE, keep.attr = TRUE)
qG(x, ordered = FALSE, na.exclude = TRUE, sort = TRUE, return.groups = FALSE)
mctl(m, names = FALSE, return = "list")     # matrix -> list of COLUMNS
mrtl(m, names = FALSE, return = "list")     # matrix -> list of ROWS
as_numeric_factor / as_integer_factor / as_character_factor
```

These are **strict** by default: `keep.attr = FALSE` drops everything
non-essential to the target class, so `qM(EuStockMarkets)` gives a plain matrix
(unlike `as.matrix`). Pass `keep.attr = TRUE` to preserve.

```r
qM(DF, 1)                        # first column becomes rownames — feeds heatmaps/maps
qDF(mtcars, "car")               # rownames saved into a column
qDT(sf::st_drop_geometry(x))
qF(x, sort = FALSE)              # factor in FIRST-APPEARANCE order — locks a plot ordering
```

`qF(x, sort = FALSE)` after `roworder()` is the standard way to freeze a category
ordering for ggplot so it isn't alphabetized.
