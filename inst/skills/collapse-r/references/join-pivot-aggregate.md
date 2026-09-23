# Joining, reshaping, and multi-type aggregation

## Table of contents
- [`join()`](#join)
- [`pivot()`](#pivot)
- [`collap()`](#collap)
- [`rowbind()` and column binding](#rowbind-and-column-binding)

## `join()`

```r
join(x, y, on = NULL, how = "left", suffix = NULL, validate = "m:m",
     multiple = FALSE, sort = FALSE, keep.col.order = TRUE, drop.dup.cols = FALSE,
     verbose = 1, require = NULL, column = NULL, attr = NULL, ...)
```

A single hash-based join for all types. `how`: `"left"` (default), `"right"`,
`"inner"`, `"full"`, `"semi"`, `"anti"` — the first letter suffices.

**The two defaults that differ from dplyr and will surprise you:**

- `multiple = FALSE` — when a row of `x` matches several rows of `y`, only the
  **first** match is used. dplyr's `left_join` returns all of them. Pass
  `multiple = TRUE` for the full cartesian product.
- `sort = FALSE` — row order of `x` is preserved. `sort = TRUE` switches to a
  completely different sort-merge algorithm and returns rows sorted by the join
  keys.

```r
join(x, y, on = "id")                                    # left join, first match
join(x, y, on = c("x_id" = "y_id"))                      # differently named keys
join(meta, wide, on = "cell", how = "full", sort = TRUE)
join(x, y, on = c("iso3c", "year"), how = "inner", multiple = TRUE)
```

`verbose = 1` (the default) prints a compact summary: the table names, the join
columns, how many records from each side were used, and the average join order
(`<4:1.5>` = each `x` key matches 4 records on the left and 1.5 on the right).
Read it — it catches key mistakes immediately. Set `verbose = 0` in package code
or loops (it is also marginally faster).

Guardrails, for when you actually need them:

```r
join(x, y, on = "id", validate = "1:1")                        # error if not 1:1
join(x, y, require = list(x = 0.9, fail = "warning"))          # >=90% of x must match
join(x, y, column = TRUE)                                      # adds a ".join" factor column
join(x, y, column = list("merge", c("x","y","both")))
join(x, y, suffix = "_new")                                    # rename duplicate y columns
join(x, y, drop.dup.cols = "y")                                # or just drop them
```

`validate` only checks — it never changes the result, and failures raise an error
*before* the join runs. **But it costs an extra pass**, so do not add it by
reflex. Reach for it when the user asks for the check, when the join keys are
genuinely uncertain, or when a silent many-to-one would corrupt downstream
numbers. A plain `join(x, y, on = "id")` against a lookup you already know has
one row per key needs no validation — and `verbose = 1` (the default) already
prints the match summary that would have told you.

Under the hood is `fmatch(x, table, nomatch, count, overid)`, which is also
directly useful: `%iin%` / `%!iin%` return match indices for subsetting, and
`fmatch(list(TX, TY), list(FX, FY))` matches on multiple columns at once.

## `pivot()`

```r
pivot(data, ids = NULL, values = NULL, names = NULL, labels = NULL,
      how = "longer", na.rm = FALSE, factor = c("names","labels"), check.dups = FALSE,
      # wider / recast only:
      FUN = "last", FUN.args = NULL, nthreads = 1, fill = NULL, drop = TRUE,
      sort = FALSE, transpose = FALSE)
```

One function for melt, cast, and recast. `how`: `"longer"`/`"l"` (default),
`"wider"`/`"w"`, `"recast"`/`"r"`.

### Longer (melt)

```r
pivot(mtcars)                                    # melt everything
pivot(iris, "Species")                           # ids kept, the rest melted
pivot(iris, values = 1:4)                        # same thing, stated from the other side
pivot(wlddev, c("iso3c","year"), c("PCGDP","LIFEEX"), na.rm = TRUE)
```

New columns are named `variable` and `value` by default. **To rename them you
must pass a named list** — a bare string *selects* rather than renames:

```r
pivot(GGDC10S, 1:5, names = list("Sectorcode", "Value"))          # positional: name, value
pivot(GGDC10S, 1:5, names = list(variable = "Sectorcode"))        # rename just the names col
```

`labels = TRUE` carries variable labels into an extra column (and
`labels = list(name, new_labels)` assigns new ones). `pivot(d, ids, labels = "label")`
followed by `pivot(how = "w", labels = "label")` is a perfect round trip that
preserves labels; without `labels` you lose them.

### Wider (cast)

```r
pivot(long, ids = "cell", names = "main_cat", values = "N", how = "w", sort = "ids")
pivot(exports, ids = "c", values = "RCA", names = "s", how = "wider",
      FUN = "mean", sort = TRUE)                    # aggregate duplicates
pivot(long, "sector_code", "res", "rec_o", how = "w")
```

If `names`/`values` are not supplied, `pivot` looks for columns literally called
`variable` and `value`. When `ids` + `names` do not uniquely identify rows, values
are silently aggregated with `FUN` (default `"last"`) — pass `check.dups = TRUE`
to get a warning instead of a silent surprise.

`FUN` accepts fast internal strings (`"first"`, `"last"`, `"count"`, `"sum"`,
`"mean"`, `"min"`, `"max"`) or any R function (`fmedian`, `fsum`, …). Internal
strings are much faster. `fill` sets the value for unbalanced combinations;
`sort = "ids"` / `"names"` / `TRUE` control ordering; `drop = FALSE` retains
unused factor levels as (empty) columns.

### Recast

```r
pivot(d, ids = c("id","time"), names = list(from = "var", to = "newvar"),
      values = "value", how = "recast", FUN = fsum)
```

`pivot()` does not melt to multiple value columns in one call. When you need
that, melt each group separately and bind them back with `add_vars()`. Name the
intermediates rather than reaching for a `%>% {...}` block — the data appears
twice, so there is no clean pipeline:

```r
va  <- pivot(wide, ids = 1:2, values = get_vars(wide, "_VA$",  regex = TRUE, return = "names"),
             names = list(variable = "Sectorcode", value = "VA"))
emp <- pivot(wide, ids = 1:2, values = get_vars(wide, "_EMP$", regex = TRUE, return = "names"),
             names = list(value = "EMP"))
long <- add_vars(va, EMP = emp$EMP)
```

## `collap()`

The multi-type aggregator: applies one function to numeric columns and another to
categorical ones, in a single call, with optional weights.

```r
collap(X, by, FUN = fmean, catFUN = fmode, cols = NULL, w = NULL, wFUN = fsum,
       custom = NULL, ..., keep.by = TRUE, keep.w = TRUE, keep.col.order = TRUE,
       sort = TRUE, decreasing = FALSE, na.last = TRUE, method = "auto", drop = TRUE,
       parallel = FALSE, mc.cores = 2, return = "wide", give.names = "auto")

collapv(X, by, ...)   # `by`/`w` as column names or indices — for programming
collapg(X, ...)       # for grouped_df input, NSE
```

```r
collap(iris, ~ Species)                                 # mean of numerics, mode of factors
collap(wlddev, ~ country + decade, fmedian, ffirst)     # different FUN / catFUN
collap(wlddev, PCGDP + LIFEEX ~ country + decade, fsum) # formula selects columns
collap(wlddev, ~ region + year, w = ~ POP)              # weighted mean + weighted mode
collap(dat, ~ g, list(fmean, fmedian), return = "long") # several functions
collapv(iris, 5, custom = list(fmean = 1:2, fmedian = 3:4))   # map functions to columns
collapv(wlddev, c("country","decade"), fsum)            # names/indices instead of a formula
collap(wlddev, g)                                       # pre-computed GRP object: fastest
```

- `by` accepts a formula (`~ g1 + g2`, or `y1 + y2 ~ g` to also select columns), a
  data.frame/list of grouping vectors, or a `GRP` object.
- `custom = list(fmean = c("a","b"), fsum = "c")` gives per-column control and
  overrides `FUN`/`catFUN`. Names of the result get function suffixes when
  `give.names = "auto"` and more than one function is used.
- `return = "wide"` (default), `"long"`, `"long_dupl"`, `"list"`.
- `w` is aggregated with `wFUN` (default `fsum`) and kept unless `keep.w = FALSE`.
- `cols` can be indices, names, or a predicate (`is.numeric`, `is_categorical`).

`collap()` decides numeric vs categorical with the C-level `is_numeric_C`
definition (see `num_vars` in `data-manipulation.md`), not `is.numeric` methods.

**When to use what:** `collap()` when numeric and categorical columns need
different treatment or when weights apply to both; `fgroup_by |> f<stat>()` when
one statistic covers everything (faster and simpler); `fsummarise` when each
output column is a different expression.

## `rowbind()` and column binding

```r
rowbind(..., idcol = NULL, row.names = FALSE, use.names = TRUE, fill = FALSE,
        id.factor = "auto", return = "as.first")
```

Takes loose objects or a single list of them. `fill = TRUE` fills absent columns
with `NA`; `idcol` records which input each row came from. Column binding that
preserves class and is far cheaper than `cbind.data.frame`:

```r
add_vars(x, y, z)                    # class-preserving cbind
add_vars(x, pos = "front") <- list(new = v)
add_vars(graph) <- list(.length = st_length(lines))
```

For nested lists of tables, `unlist2d()` is the generalized recursive row-bind —
see `references/summaries-and-lists.md`.
