# Recipes — patterns from production code

These are idioms distilled from research and package code that uses collapse as
its backend (economics panels, trade/IO tables, spatial infrastructure data,
transport networks, dynamic factor models). They show the shapes collapse code
takes when it is written well.

## Table of contents
- [Script header](#script-header)
- [Shares, indices, and normalizations without a join](#shares-indices-and-normalizations-without-a-join)
- [Picking the best record per group](#picking-the-best-record-per-group)
  - [Spatial deduplication across sources](#spatial-deduplication-across-sources)
- [Panel preparation](#panel-preparation)
- [Ordering categories for plots and tables](#ordering-categories-for-plots-and-tables)
- [Long ↔ wide for figures and LaTeX tables](#long--wide-for-figures-and-latex-tables)
- [Matrices, arrays, and input-output tables](#matrices-arrays-and-input-output-tables)
- [Input-output tables: aggregating a square table](#input-output-tables-aggregating-a-square-table)
- [Input-output tables: coefficients, Leontief, and regionalization](#input-output-tables-coefficients-leontief-and-regionalization)
- [Input-output tables: long ↔ block matrices](#input-output-tables-long--block-matrices)
- [Input-output tables: assembling and balancing (IPF/RAS)](#input-output-tables-assembling-and-balancing-ipfras)
- [Aggregating to a grid or region with zero-fill](#aggregating-to-a-grid-or-region-with-zero-fill)
- [Building a graph from coordinates](#building-a-graph-from-coordinates)
- [Working with `sf`](#working-with-sf)
- [Working with `data.table`](#working-with-datatable)
- [Working with the tidyverse](#working-with-the-tidyverse)

## Script header

Every analysis script in these projects starts the same way:

```r
library(fastverse)                                       # collapse + data.table + kit + magrittr
fastverse_extend(qs2, sf, units, ggplot2, xtable, install = TRUE)
set_collapse(mask = c("manip", "helper", "special"), nthreads = 4, sort = FALSE)
source("code/helpers.R")
fastverse_conflicts()                                    # print what is masked
```

`sort` is a deliberate choice, not a default to copy. `sort = FALSE` is the
speedup — unsorted (hash-based) grouping is faster and preserves first-appearance
order — and it is right when you sort the final table anyway or need a block
structure preserved (see the IO recipes). `sort = TRUE` is right when downstream
code indexes results by group order, or when you want reproducible sorted output
from every aggregation without thinking about it; several of these projects set
it explicitly for that reason. Add `remove = "old"` to drop deprecated names. In
an Rmd or a package, drop the masking.

Once masked, these names are the collapse functions with collapse's arguments —
`unique(x, sort = TRUE)` is `funique()`, not `base::unique()`. Always call
`fastverse_conflicts()` so the swap is on the record, and read the header before
editing an existing script. See gotcha 4b.

## Shares, indices, and normalizations without a join

`TRA` removes the "compute a group aggregate, join it back, divide" pattern
entirely. This is the single biggest style difference from dplyr code.

```r
# share of the group total
fmutate(dat, share = fsum(value, list(region, year), TRA = "/"))

# percent of the group total
fmutate(dat, pct = fsum(value, region, TRA = "%"))

# deviation from the group mean, then from the grand mean
fmutate(dat, dev = fwithin(v, id), dev2 = fmean(v, id, TRA = "-+"))

# columns of a matrix as proportions, in place
fsum(m, TRA = "/", set = TRUE)

# add a Total column across numeric columns (psum is kit::psum, attached by fastverse)
(\(d) add_vars(d, Total = psum(num_vars(d))))(TBL) |> colorder(id, Total)
```

Balassa's Revealed Comparative Advantage — two different groupings in one
expression, computed by reference:

```r
settransform(exports,
  RCA = fsum(v, list(c, y), TRA = "/") %/=%
        fsum(fsum(v, y, TRA = "/"), list(s, y), TRA = "fill", set = TRUE))
```

Aggregated shares within a two-level grouping:

```r
leontief |>
  subset(source_country != using_country & using_country %in% africa) |>
  group_by(sector = using_industry, from_africa = source_country %in% africa) |>
  select(I2E = fvax) |> fsum() |>
  mutate(I2E_AFR = fsum(I2E, sector, TRA = "/"))
```

## Picking the best record per group

`fmode` with weights and `TRA = "fill"` deduplicates in one line: keep the records
coming from the importance-weighted most frequent source, per location and type.

```r
fsubset(data, source == fmode(source, list(location, type), importance, "fill"))
```

This is efficient because nothing intermediate is materialized — no grouped
aggregate, no join back, no filter on a helper column. All the work is relegated
to `fmode()`, which calls the internal grouping algorithm. Whenever you catch
yourself about to aggregate-then-join in order to filter, look for the version
where the grouping goes inside the statistical function instead.

### Spatial deduplication across sources

The same one-liner scales to deduplicating points of interest merged from a dozen
providers, where "same location" means "within ~10 m" rather than "identical key".
Two collapse tricks carry it.

**Snap coordinates to a grid with `TRA(x, tol, "-%%")`.** The `"-%%"` operation is
"subtract the modulus", i.e. `x - x %% tol` — it rounds down to a multiple of
`tol`. With no `g`, `TRA()` is just a fast sweep against a scalar, so this is grid
binning without `cut()`, factors, or a spatial index:

```r
dpm <- 1 / (40075017 / 360)             # degrees per metre
tol <- 10 * dpm                         # 10 m cell
points <- fmutate(points, lon_cos = lon * cos(lat * pi / 180))   # metric-ish longitude

dup_id <- finteraction(main_cat,
                       TRA(points$lon_cos, tol, "-%%"),
                       TRA(points$lat,     tol, "-%%"), factor = FALSE)
```

`finteraction(..., factor = FALSE)` returns a `qG` (integer) rather than a factor —
cheaper, and all the statistical functions accept it. Including `main_cat` in the
interaction means only same-category points compete.

**Nudge the grid and repeat.** A fixed grid splits pairs that straddle a cell
boundary, so shift it by 1 m at a time over a 10×10 offset grid and re-run the
dedup each pass:

```r
grid <- expand.grid(lon_nudge = (0:9) * dpm, lat_nudge = (0:9) * dpm)

for (i in seq_row(grid)) {
  points <- points |>
    fmutate(dup_id = finteraction(main_cat,
              TRA(lon_cos + grid$lon_nudge[i], tol, "-%%"),
              TRA(lat     + grid$lat_nudge[i], tol, "-%%"), factor = FALSE)) |>
    fsubset(source == fmode(source, dup_id, weight, "fill"))
}
```

On 400 points representing 200 real facilities seen by two sources with a few
metres of jitter: the 100-pass loop collapses to exactly 200 rows, while a single
unshifted pass leaves 287 — the boundary pairs it cannot see. Each pass is cheap
because the frame keeps shrinking.

**The weight is a tie-break, not an override.** Scaling it to ~1e-5 means `fmode`
still decides on frequency first and only uses the weight to break ties, with
preferred sources pushed to the top of their bracket:

```r
v <- pmax(replace_na(as.numeric(points$value), 0), 0)
points <- fmutate(points,
  weight = 1 + log(data.table::fifelse(source %in% important_sources,
                                       fmax(v) + 1, v + 1)) / 1e5)
```

Exempt whatever should never be deduplicated (authoritative registries), dedup the
rest, and `rowbind()` them back:

```r
exempt <- fsubset(points, source %in% exempt_sources | is.na(main_cat))
dedup  <- fsubset(points, source %!in% exempt_sources & !is.na(main_cat))
# ... loop over dedup ...
out <- rowbind(exempt, get_vars(dedup, names(exempt)))
```

The same shape works for "keep the row where a variable attains its group max",
which avoids a join or a slice:

```r
# latest observation within the last 3 years, per country-sector
exports_latest <- subset(exports, y > 12 & y == fmax(y, list(c, s), "fill"), -y)

# grouped top-n rows
fslice(dat, id, n = 3, how = "max", order.by = score)
fsubset(dat, data.table::rowid(id1, id2) <= n)      # grouped slice_head
```

## Panel preparation

```r
# 1. transform in place
settransform(PDATA,
  damage_pct = damage / gdp * 100,
  oil        = log(oil_prod + 1),
  affected   = log(naffect + 1))

# 2. difference every double column by country, respecting the time variable
PR_diff <- PDATA |>
  ftransformv(is.double, fdiff, g = iso3c, t = year, apply = FALSE) |>
  na_omit(cols = c("t", "growth")) |>
  get_vars(varying(PDATA) & fnobs(PDATA) > 350L)     # drop constant / thin columns

# 3. inspect
namlab(PR_diff, N = TRUE, Nd = TRUE, class = TRUE)
qsu(PR_diff, labels = TRUE)

# 4. winsorize before imputation / modelling
X <- get_vars(PR_diff, predictors) |> replace_outliers(5)      # |z| > 5 -> NA
cwt <- rowSums(!is.na(X)); X <- ss(X, cwt > 30)
```

`apply = FALSE` in `ftransformv` is essential here — it hands the whole numeric block to
`fdiff.data.frame` in one vectorized call instead of column by column.

For within/between decompositions and multi-way fixed effects:

```r
qsu(dat, pid = ~ iso3c, cols = 9:12)          # overall / between / within stats
HDW(dat, ~ firm + year)                       # two-way demeaned
fFtest(y, exc = dat$region, X = controls)     # test excluding a large factor
```

## Ordering categories for plots and tables

ggplot alphabetizes character axes and destroys a ranking. The fix is two lines:

```r
DATA |>
  roworder(-value) |>                       # sort
  fmutate(x = qF(x, sort = FALSE))          # factor in CURRENT row order
```

`qF(x, sort = FALSE)` means "make a factor, keep first-appearance order". Then:

```r
EM_BS_AGG_SEC <- EM_BS |>
  subset(order(vax + fva - davax, decreasing = TRUE)) |>
  transform(ndavax = vax - davax, davax = NULL,
            sector_code = qF(sector_code, sort = FALSE)) |>
  colorder(sector_code, sector, ndavax) |>
  rename(toupper, cols = is.numeric)
```

Relabel factor levels in place for nicer legends (`set_attr` is `magrittr`'s,
attached by fastverse; collapse's own equivalents are `setattrib`/`setAttrib`):

```r
fmutate(d, variable = set_attr(variable, "levels", c("Intra-African", "Africa-ROW")))
```

## Long ↔ wide for figures and LaTeX tables

```r
# wide -> long for fill/colour aesthetics; new columns are `variable` and `value`
WIDE |> pivot("country")
WIDE |> pivot(ids = c("country", "label"), values = .c(E2R, I2E))
WIDE |> pivot(1:2) |> ggplot(aes(x = sector_code, y = value, fill = variable)) + ...

# long -> wide matrix for heatmaps / maps
LONG |> pivot(ids = "sector_code", values = "NDAVAX", names = "REC", how = "w")
qM(DF, 1)                                     # first column -> rownames, then pheatmap/image
```

Composite "value (share%)" cells for a paper table:

```r
DATA |>
  fmutate(res = paste0(round(value, 2), " (", signif(share * 100, 2), "%)")) |>
  pivot("sector_code", "res", "rec_o", how = "w") |>
  xtable::xtable() |> print(include.rownames = FALSE, booktabs = TRUE)
```

Format every numeric column as "level (share%)":

```r
TBL |>
  ftransformv(is.numeric, \(x) paste0(signif(round(x, 2), 4), " (",
                               signif(proportions(x) * 100, 2), "%)")) |>
  xtable::xtable(digits = 1) |> print(include.rownames = FALSE, booktabs = TRUE)
```

## Matrices, arrays, and input-output tables

collapse is at home in matrix code because `qM`, `mctl`/`mrtl`, `rsplit`,
`rapply2d`, `unlist2d`, and the `%r*%`/`%c*%` operators cover the round trip.

```r
# unpack several returns at once
.c(rows, cols) %=% lapply(dimnames(icio), \(x) qDT(tstrsplit(x, "_")))
c("T", "N")   %=% dim(X)

# index lookups without logical vectors
tax_row  <- rows$V1 %==% "TLS"
sele_col <- c(cols$V1 %==% country, ncol(icio))

# nested list of matrices -> one long table -> back again
NIOT |> unlist2d(c("year", "country"), "sector") |> fwrite("NIOT.csv")
long |> rsplit(~ year + country) |> rapply2d(qM, 1)      # inverse

# sweep vectors across rows / columns
X_std   <- (x_sm %r*% sd_vec) %r+% mean_vec
resid   <- get_vars(X, names(fit)) %c-% fit
setop(x_sm %r*% Wx, "+", Mx, rowwise = TRUE)             # same, by reference
cov(Y, X_log) %r/% fvar(X_log)                           # regression coefficients
```

`rsplit(dat, lon + lat + area ~ main_cat)` splits selected columns into a named
list; `get_elem(results, "r.squared") |> unlist()` harvests one component from a
list of fitted models; `t_list()` swaps two levels of nesting.

## Input-output tables: aggregating a square table

An IO table is a matrix whose row and column labels encode the same
country × sector (or region × sector) structure, e.g. `"USA_01T02"`. Aggregating
it to coarser sectors means aggregating **both** dimensions with the same
grouping. `fsum()` on a matrix aggregates rows, so you transpose in between:

```r
Z_agg <- Z |> fsum(g_rows) |> t() |> fsum(g_cols) |> t()
```

`g_rows` and `g_cols` differ only because the column side usually carries extra
final-demand items the row side lacks. Build both from the dimnames:

```r
nace <- c("A", "B-E", "F", "G-I", "J", "K", "L", "M-N", "O-Q", "R-U")
sec_map <- ...            # named list: aggregate sector -> vector of ICIO sectors
fd_map  <- list(HFCE = c("HFCE", "DPABR"), NPISH = "NPISH", GGFC = "GGFC",
                GFCF = "GFCF", INVNT = "INVNT")

make_grp <- function(labels, map) {
  detail <- substr(labels, 5, 100)                       # sector part
  agg    <- kit::vswitch(detail, unlist(map, use.names = FALSE),
                         rep(names(map), lengths(map)), default = detail)
  paste0(substr(labels, 1, 4), agg) |> GRP(sort = FALSE)  # country part + new sector
}
g_rows <- make_grp(rownames(Z), sec_map)
g_cols <- make_grp(colnames(Z), c(sec_map, fd_map))

IO_data <- lapply(IO_data, \(x) x |> fsum(g_rows) |> t() |> fsum(g_cols) |> t())
```

**`sort = FALSE` is not optional here.** IO tables are block-structured, and
sorted grouping would reorder countries and sectors alphabetically, silently
scrambling the block layout that every downstream index expression relies on.
First-appearance order keeps the table's own ordering.

`kit::vswitch()`/`nswitch()` (attached by fastverse) is the fast many-to-one
recode you want for the sector mapping — much faster than nested `ifelse` or long
`recode_char` chains.

### The one-pass alternative

Because a matrix is just a vector in column-major order, you can do the same
aggregation in a single pass by grouping on the *interaction* of the row and
column labels — useful when you want the cells rather than a matrix, and it is
how you compute shares within blocks:

```r
g  <- qF(substr(rownames(Z), 1, 3), sort = FALSE)                  # country per row
gp <- qF(as.vector(outer(g, g, finteraction, sort = FALSE)), sort = FALSE)

all.equal(as.vector(Z |> fsum(g) |> t() |> fsum(g) |> t()),
          as.vector(fsum.default(Z, gp, use.g.names = FALSE)))     # TRUE
```

Note `fsum.default` called explicitly: it forces the vector method, so the matrix
is treated as one long vector and `gp` (length `nrow*ncol`) groups the cells. The
equivalence holds **only with `sort = FALSE` throughout** — sorted interaction
levels run in a different order than column-major cells, and the two results stop
lining up.

The payoff is block-wise shares in one call, which is how a trade matrix becomes
a distribution key:

```r
P_trade <- fsum.default(trade, gp, TRA = "/") |> setv(NaN, 1e-7)
```

## Input-output tables: coefficients, Leontief, and regionalization

`%r/%` divides each **column** by an element of a vector (it applies the vector
to each row), which is exactly the technical-coefficient definition
`a_ij = z_ij / x_j`:

```r
x <- colSums(Z) + va                       # gross output
A <- Z %r/% x                              # technical coefficients
L <- solve(diag(nrow(A)) - A)              # Leontief inverse (base R)
mult <- colSums(L)                         # output multipliers
```

When you only need each column as a share of its own total — the shape used for
distribution keys rather than for Leontief algebra — `fsum(Z, TRA = "/")` does it
directly, and `set = TRUE` does it in place across a whole list of tables:

```r
coeff <- fsum(Z, TRA = "/")                            # columns sum to 1
EU_data |> fsum(gp, TRA = "/", set = TRUE) |> replace_na(0, set = TRUE)
```

**Coerce to double first.** `TRA` preserves the storage type, so `"/"` and `"%"`
on integer data give integer division — and IO cells read from CSV are routinely
integer. Do it once when the table is loaded: `Z <- Z + 0` or
`storage.mode(Z) <- "double"`. See gotcha 30b.

Regionalizing a national table means distributing national sector vectors across
regions using regional shares. `%r*%` is the whole operation — a
`region × sector` share matrix times a length-`sector` national vector:

```r
rgva_frac <- fsum(rgva_mat, TRA = "/") |> setv(NaN, 1e-7)   # region shares, per sector
rva       <- rgva_frac %r*% nio["VA",     nace]             # VA by region x sector
rtotal    <- rgva_frac %r*% nio["OUTPUT", nace]
rgfcf     <- rgfcf_frac %r*% nio[nace, "GFCF"]
```

Simple location quotients and export splits follow with ordinary matrix algebra;
keep `setv(x, NaN, 1e-7)` after every division so empty regions or sectors do not
poison downstream matrix operations with `NaN`.

## Input-output tables: long ↔ block matrices

Bilateral trade data arrives long (`origin, destination, sector_o, sector_d,
value`) and the model wants one matrix per sector pair. `rsplit()` builds the
nesting, `rapply2d()` applies `psmat()` at the leaves, and `unlist2d()` inverts
the whole thing:

```r
blocks <- trade_long |>
  rsplit(~ nace_o + nace_d) |>                       # nested list: sector_o -> sector_d
  rapply2d(psmat, value ~ nuts2_o, ~ nuts2_d) |>     # each leaf -> origin x destination
  rapply2d(\(x) setv(x, NA, 1e-7))                   # no NAs in a matrix you will invert

blocks$A$A                                           # sector A -> sector A, regions x regions
all_identical(dimnames(blocks$A$A), list(regions, regions))

long_again <- blocks |> unlist2d(c("nace_o", "nace_d"), "nuts2_o", DT = TRUE)
```

Make the origin/destination columns factors with the full region set *before*
`psmat()` — that guarantees every block has identical dimnames even where a pair
never trades, which is what lets you index blocks positionally afterwards:

```r
trade_long <- trade_long |>
  fmutate(across(c(nuts2_o, nuts2_d), factor, levels = regions),
          across(c(nace_o,  nace_d),  factor, levels = nace))
```

`psmat(data, ~ from, ~ to, array = FALSE)` returns a *list* of matrices, one per
value column — handy when several final-demand items share one long table.

Pulling one component out of a deep result list and writing it flat:

```r
SRIO |>
  get_elem("IO", keep.tree = TRUE) |>
  unlist2d(c("year", "iso3c", "region", "variable"), "sector") |>
  fwrite("SRIO.csv")
```

`keep.tree = TRUE` preserves the nesting levels above the extracted element, so
the `idcols` of `unlist2d()` line up with them one for one.

## Input-output tables: assembling and balancing (IPF/RAS)

Build the empty MRIO shell from the label grid, then fill blocks:

```r
dn <- as.vector(t(outer(regions, nace, paste, sep = "_")))
MRIO <- matrix(0, length(dn), length(dn), dimnames = alloc(dn, 2))

rn <- substr(rownames(MRIO), 1, 4)          # region part of each label
for (i in regions) for (j in regions)
  MRIO[rn %==% i, rn %==% j] <- if (i == j) SRIO[[i]] else ipf_2d(seed, row_target, col_target)
```

`alloc(dn, 2)` builds the two-element dimnames list without duplicating the
character vector, and `%==%` returns positions directly, so the block indexing
never materializes a logical mask.

Two-dimensional iterative proportional fitting (RAS balancing) is a natural fit
for the by-reference operators — the whole loop allocates nothing after the
initial copy:

```r
ipf_2d <- function(seed, row_target, col_target, tol = 1e-5, max_iter = 30L, set = FALSE) {
  m <- nrow(seed); n <- ncol(seed)
  X <- if (set) seed else seed + 0                       # +0 forces a double copy
  iter <- 0L
  repeat {
    iter <- iter + 1L
    rs <- .rowSums(X, m, n); rf <- row_target / rs
    X %*=% setv(rf, is.finite(rf), 0, invert = TRUE, vind1 = TRUE)   # scales ROWS
    cs <- .colSums(X, m, n); cf <- col_target / cs
    setv(cf, is.finite(cf), 0, invert = TRUE, vind1 = TRUE)
    setop(X, "*", cf, rowwise = TRUE)                                # scales COLUMNS
    if (max(max(abs(row_target - rs)), max(abs(col_target - cs))) < tol ||
        iter > max_iter) break
  }
  X
}
```

Three things carry the weight here:

- `X %*=% v` with `length(v) == nrow(X)` recycles down columns, i.e. scales
  **rows**; `setop(X, "*", v, rowwise = TRUE)` with `length(v) == ncol(X)` scales
  **columns**. That pair replaces `sweep()` and makes no copies.
- `setv(rf, is.finite(rf), 0, invert = TRUE, vind1 = TRUE)` zeroes the non-finite
  scaling factors in place — `vind1 = TRUE` says the second argument holds
  positions, `invert = TRUE` flips the selection to the *non*-finite ones. This is
  the collapse form of `rf[!is.finite(rf)] <- 0` without allocating the mask.
- `seed + 0` is a deliberate copy so the caller's seed survives; `set = TRUE` opts
  into modifying it.

Sanity checks after assembly, all cheap:

```r
if (any(sapply(MRIO_final, anyNA))) stop("Missing values in final MRIOs!")
replace_outliers(MRIO_final, 0, 0, "min", set = TRUE)      # floor at zero, in place
descr(mapply(\(x, z) z["TOTAL", ] / (colSums(x) + colSums(z[c("TAXSUB","VA"), ])),
             MRIO_final, VA_final))                        # column balance, should be ~1
```

`replace_outliers(x, 0, 0, "min", set = TRUE)` floors a whole list of matrices at
zero by reference — the usual last step after IPF leaves tiny negatives.

## Aggregating to a grid or region with zero-fill

The canonical spatial aggregation, keeping empty cells:

```r
cnt  <- fcount(pts, cell, main_cat)                        # counts by cell x category
wide <- pivot(cnt, ids = "cell", names = "main_cat", values = "N",
              how = "w", sort = "ids")
out  <- join(meta, wide, on = "cell", how = "left")        # meta has ALL cells
replace_na(out, 0, cols = grep("^pt_", names(out)), set = TRUE)
```

Combining two aggregated layers, converting `units` columns, zero-filling:

```r
out <- join(points_hex, lines_hex, on = "cell", how = "full", sort = TRUE)
num <- names(out)[vapply(out, \(x) is.numeric(x) || inherits(x, "units"), TRUE)]
settransformv(out, num, \(x) replace_na(as.numeric(x), 0))
```

## Building a graph from coordinates

From `flownet` — turning `sf` LINESTRINGs into a from/to edge table. Note
`group()` to hash coordinate pairs into node ids, and `fmatch()` on **lists** to
match on two columns at once:

```r
coords <- st_coordinates(lines) |> qDF()
g  <- GRP(list(edge = coords$L1), return.order = FALSE)
XY <- fselect(coords, X, Y)

from_xy <- XY |> ffirst(g, na.rm = FALSE, use.g.names = FALSE) |> add_stub("F")
to_xy   <- XY |> flast(g,  na.rm = FALSE, use.g.names = FALSE) |> add_stub("T")

graph <- from_xy |>
  add_vars(to_xy) |>
  add_vars(g$groups, pos = "front") |>
  fmutate(from = unattrib(group(FX, FY)),
          to   = from[fmatch(list(TX, TY), list(FX, FY))]) |>
  colorder(edge, from, FX, FY, to, TX, TY)

if (anyNA(graph$to)) {
  miss <- whichNA(graph$to)
  setv(graph$to, miss,
       group(ss(graph, miss, c("TX","TY"), check = FALSE)) %+=% fmax(graph$from),
       vind1 = TRUE)
}
```

`unattrib()` strips the `qG` class so the ids are plain integers; `%+=%` offsets
the new ids in place.

## Working with `sf`

collapse supports `sf` by never destroying the geometry column: column-subsetting
functions add it back automatically, `funique`/`roworder` skip it when grouping,
and `descr`/`qsu` omit it from statistics. It performs **no geometric
operations** — after `fsubset()` the bounding box attribute is stale.

```r
net |> fsubset(highway %in% keep, osm_id, highway)      # geometry retained
net |> fgroup_by(country) |> fsummarise(len = fsum(length))
qDT(sf::st_drop_geometry(hex))                          # drop it when you don't need it
fselect(graph_df, -FX, -FY, -TX, -TY) |> add_vars(list(geometry = geoms)) |>
  sf::st_sf(sf_column_name = "geometry", crs = 4326)
```

`units` columns are preserved by `.units` methods; convert with `as.numeric()`
before writing to disk or joining to plain numerics.

## Working with `data.table`

All collapse **data manipulation** functions return a properly over-allocated
data.table, so `:=` works afterwards. Statistical functions' `.data.frame`
methods do not over-allocate (it costs 2–3 µs), so:

```r
res  <- DT |> fgroup_by(id) |> fsummarise(mu = fmean(a))   # valid DT, `:=` fine
res2 <- DT |> fgroup_by(id) |> fmean()                     # `:=` warns
res2 <- DT |> fgroup_by(id) |> fmean() |> qDT()            # fix
```

Mixing is idiomatic and encouraged: use `data.table` for `:=` by reference,
`nafill`, `setorder`, `rowid`, `tstrsplit`, `fread`/`fwrite`, `%chin%`, `%ilike%`,
and collapse for the statistics and reshaping.

```r
pts[, cell := dggridR::dgGEO_to_SEQNUM(spec, lon, lat)$seqnum]
pts <- fsubset(pts, cell %in% valid_cells)
out[, (col) := data.table::nafill(get(col), fill = 0L)]
```

## Working with the tidyverse

collapse's f-prefixed namespace does not conflict, so you can drop individual
functions into a dplyr pipeline for speed:

```r
data |> group_by(g) |> summarise(across(a:c, fmean))   # dplyr grouping, collapse stats
data |> dplyr::group_by(g) |> fmean()                  # collapse reads dplyr groupings
```

collapse converts a `dplyr` grouping to a `GRP` object in C, so all `.grouped_df`
methods work on either. Going the other way, `fgroup_by()` output continues to
print and behave as a tibble/data.table.

Deliberate differences to expect: no tidyselect helpers, no lambdas, no `.by=`
argument (use internal grouping instead), and `fsubset()` refuses grouped input.
