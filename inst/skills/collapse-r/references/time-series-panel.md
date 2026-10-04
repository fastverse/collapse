# Time series and panel data

collapse treats time series and panels as ordinary data with an optional time
index. There is no requirement to convert to `ts`/`xts`/`pdata.frame` — but the
`t` argument is what makes results correct on irregular data, and it is the thing
most often forgotten.

## Table of contents
- [Lags, differences, growth rates](#lags-differences-growth-rates)
- [Why `t` matters](#why-t-matters)
- [The transformation operators](#the-transformation-operators)
- [Indexed frames and series](#indexed-frames-and-series)
- [Panel arrays, ACF, and helpers](#panel-arrays-acf-and-helpers)
- [Interop: `plm`, `xts`/`zoo`, `ts`](#interop-plm-xtszoo-ts)

## Lags, differences, growth rates

```r
flag(x, n = 1, g = NULL, t = NULL, fill = NA, stubs = TRUE)
fdiff(x, n = 1, diff = 1, g = NULL, t = NULL, fill = NA, log = FALSE, rho = 1, stubs)
fgrowth(x, n = 1, diff = 1, g = NULL, t = NULL, fill = NA, logdiff = FALSE,
        scale = 100, power = 1, stubs)
fcumsum(x, g = NULL, o = NULL, na.rm = TRUE, fill = FALSE, check.o = TRUE)
```

All are fully vectorized across columns, groups, **and** multiple lags/differences
at once. Negative `n` gives leads.

```r
flag(x, 1, g = id, t = year)             # panel lag
flag(dat, -1:3, g = id, t = year)        # leads and lags in one call -> wide result
fdiff(x, 1, 1, g = id, t = year)         # first difference
fdiff(x, 1, 2, g = id, t = year)         # second difference
fdiff(x, 1, 1, log = TRUE)               # log-difference
fgrowth(RCA, g = list(c, s), t = y)      # percentage growth on an irregular panel
fgrowth(x, logdiff = TRUE)               # log-difference growth rate
fcumsum(x, g = id, o = year)             # ordered cumulative sum within panel
fdiff(x, rho = 0.9)                      # quasi-difference x - rho*L(x)
```

`stubs = TRUE` prefixes the result columns (`L1.x`, `D1.x`, `G1.x`). For a single
lag/diff on a vector it defaults to `TRUE` for `flag`/`fdiff`/`fgrowth` defaults
but to `length(n) > 1` for matrices/data.frames.

## Why `t` matters

Without `t`, these functions assume rows within each group are **consecutive and
in order**. That is fine for a balanced, sorted panel and wrong otherwise:

```r
# unbalanced/irregular panel: sectors not observed every year
exports |> fmutate(RCA_growth = fgrowth(RCA, g = list(c, s), t = y))   # correct
exports |> fmutate(RCA_growth = fgrowth(RCA, g = list(c, s)))          # silently wrong
```

With `t` supplied, collapse computes an integer time-id and looks up the actual
previous period, inserting `NA` where it is missing. It also errors on duplicate
`(g, t)` pairs — a useful data check in itself.

How `t` is interpreted:

- plain **numeric or factor** `t` → coerced with `as.integer()`, so integer steps
  are the time unit. This is right for calendar years.
- **numeric time classes** (`Date`, `POSIXct`, `yearmon`, `yearqtr`, …) → passed
  through `timeid()`, which takes the greatest common divisor of the differences
  and builds an integer index. So use a proper class for monthly/quarterly data
  (`zoo::yearmon`, `zoo::yearqtr`) rather than a decimal year.
- character or list `t` → ordered grouping is applied; avoid this.

Helpers:

```r
timeid(x, factor = FALSE, ordered = factor, extra = FALSE)   # time vector -> integer id
seqid(x, o = NULL, del = 1, start = 1, na.skip = FALSE, skip.seq = FALSE)  # run-length grouping
is_irregular(x, any_id = TRUE)
```

`seqid()` turns an integer sequence into a group id that increments whenever the
sequence breaks — the way to identify consecutive spells (`del` sets the expected
increment).

## The transformation operators

`.OPERATOR_FUN` holds: `L`, `F`, `D`, `Dlog`, `G`, `B`, `W`, `HDB`, `HDW`, `STD`,
`TRA`. They wrap the `f*` functions and differ in three ways: they take
`by =` / `t =` / `cols =` on data frames (including formulas), they **prefix
result column names**, and they keep id columns (`keep.ids = TRUE`).

**`F` (lead) is not exported.** It was withdrawn in v1.9.0 because it clashes
with `base::F`, so a bare `F(x)` gives you `FALSE`, not a lead. Use `flag(x, -1)`,
or `collapse:::F`, or set `options(collapse_export_F = TRUE)` before loading the
package. Every other operator in the list is exported normally.

```r
L(wlddev, 1:2, ~ iso3c, ~ year, cols = 9:12)      # L1.PCGDP, L2.PCGDP, ...
D(dat, 1, 1, ~ id, ~ year)
G(dat, by = ~ id, t = ~ year)
W(wlddev, ~ iso3c, cols = 9:12, keep.by = FALSE)  # within-transform
STD(get_vars(wlddev, 9:12), wlddev$iso3c)               # grouped standardize
HDW(dat, ~ firm + year)                           # two-way demeaning
```

Set `set_collapse(stub = FALSE)` to suppress the prefixes. On indexed data
(below) the `by`/`t` arguments are unnecessary — the index is used automatically.

## Indexed frames and series

```r
findex_by(.X, ..., single = "auto", interact.ids = TRUE)
findex(x)
unindex(x)
reindex(x, index = findex(x), single = "auto")
is_irregular(x)
to_plm(x, row.names = FALSE)
```

`findex_by(dat, id, year)` attaches an `index_df` and makes the frame an
`indexed_frame` (which inherits `pdata.frame`, so `plm` works too). Every column
becomes an `indexed_series` carrying an external pointer to the index — so the
index is stored once, and time-aware computation works inside data-masking
environments and model calls:

```r
pdat <- findex_by(wlddev, iso3c, year)
pdat |> fmutate(g_PCGDP = fgrowth(PCGDP))     # no g=, no t= needed
with(pdat, flag(PCGDP))                       # index found via the pointer
lm(PCGDP ~ L(LIFEEX), data = pdat)            # works inside estimation commands
settransform(pdat, dev = W(PCGDP))
```

Indexed objects handle irregularity natively (a time factor with unused levels
marks the gaps) and have internal methods for `fsubset`, `funique`, `roworder(v)`,
`na_omit`, `[`, `$`, `[[` that subset the index alongside the data.
`drop.index.levels = "id"` (default) controls which index levels are dropped when
subsetting.

Use `unindex()` before handing data to functions that don't understand the class,
and `to_plm()` for a genuine `pdata.frame`.

## Panel arrays, ACF, and helpers

```r
psmat(x, g, t = NULL, transpose = FALSE, fill = NULL)      # panel series -> N x T matrix
psmat(dat, by, t, cols, array = TRUE)                      # data.frame -> 3D array
plot(psmat_object, legend = TRUE)
psacf(x, g, t, lag.max, type, plot = TRUE, gscale = TRUE)  # panel ACF
pspacf(...)   psccf(x, y, ...)                             # partial / cross
```

`psmat()` is the fastest way to reshape a long panel into a matrix for matrix
algebra or plotting; `gscale = TRUE` in the ACF functions standardizes within
groups first (the correct panel ACF).

## Interop: `plm`, `xts`/`zoo`, `ts`

- **plm**: all relevant functions have `.pseries`/`.pdata.frame` methods, so
  `fmean`, `fwithin`, `flag`, `qsu`, … work directly on `plm` objects.
  `fhdwithin` is the fast within-transform for multi-way fixed effects.
- **xts/zoo**: handled by `.zoo` methods that dispatch to `.matrix` or `.default`.
  They do **not** use the index automatically — pass it yourself, and with the
  right class:
  ```r
  flag(xts_daily,   1:3, t = index(xts_daily))
  flag(xts_monthly, 1:3, t = zoo::as.yearmon(index(xts_monthly)))
  ```
- **ts**: aggregating a `ts` breaks the class, so collapse drops `ts` attributes
  when the number of rows changes. Same-length transformations preserve them.
