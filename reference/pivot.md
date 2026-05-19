# Fast and Easy Data Reshaping

`pivot()` is *collapse*'s data reshaping command. It combines longer-,
wider-, and recast-pivoting functionality in a single parsimonious API.
Notably, it can also accommodate variable labels.

## Usage

``` r
pivot(data,               # Summary of Documentation:
      ids = NULL,         # identifier cols to preserve
      values = NULL,      # cols containing the data
      names = NULL,       # name(s) of new col(s) | col(s) containing names
      labels = NULL,      # name of new labels col | col(s) containing labels
      how = "longer",     # method: "longer"/"l", "wider"/"w" or "recast"/"r"
      na.rm = FALSE,      # remove rows missing 'values' in reshaped data
      factor = c("names", "labels"), # create new id col(s) as factor variable(s)?
      check.dups = FALSE, # detect duplicate 'ids'+'names' combinations

      # Only apply if how = "wider" or "recast"
      FUN = "last",       # aggregation function (internal or external)
      FUN.args = NULL,    # list of arguments passed to aggregation function
      nthreads = .op[["nthreads"]], # minor gains as grouping remains serial
      fill = NULL,        # value to insert for unbalanced data (default NA/NULL)
      drop = TRUE,        # drop unused levels (=columns) if 'names' is factor
      sort = FALSE,       # "ids": sort 'ids' and/or "names": alphabetic casting

      # Only applies if how = "wider" with multiple long columns ('values')
      transpose = FALSE   # "columns": applies t_list() before flattening, and/or
)                         # "names": sets names nami_colj. default: colj_nami
```

## Arguments

- data:

  data frame-like object (list of equal-length columns).

- ids:

  identifier columns to keep. Specified using column names, indices, a
  logical vector or an identifier function e.g.
  [`is_categorical`](https://fastverse.org/collapse/reference/small-helpers.md).

- values:

  columns containing the data to be reshaped. Specified like `ids`.

- names:

  names of columns to generate, or retrieve variable names from:

  |  |  |  |
  |----|----|----|
  | ` how ` |  | *Description* |
  |  |  |  |
  | `"longer"` |  | list of names for the variable and value column in the long format, respectively. If `NULL`, `list("variable", "value")` will be chosen. Alternatively, a named list length 1 or 2 can be provided using "variable"/"value" as keys e.g. `list(value = "data_col")`. |
  |  |  |  |
  | ` "wider"` |  | column(s) containing names of the new variables. Specified using a vector of column names, indices, a logical vector or selector function e.g. `is.character`. Multiple columns will be combined using [`finteraction`](https://fastverse.org/collapse/reference/qF.md) with `"_"` as separator. |
  |  |  |  |
  | ` "recast"` |  | (named) list with the following elements: \[\[1\]\]/\[\["from"\]\] - column(s) containing names of the new variables, specified as in `"wider"`; \[\[2\]\]/\[\["to"\]\] - name of the variable to generate containing old column names. If `NULL`, `list("variable", "variable")` will be chosen. |

- labels:

  names of columns to generate, or retrieve variable labels from:

  |  |  |  |
  |----|----|----|
  | ` how ` |  | *Description* |
  |  |  |  |
  | `"longer"` |  | A string specifying the name of the column to store labels - retrieved from the data using `vlabels(values)`. `TRUE` will create a column named `"label"`. Alternatively, a (named) list with two elements: \[\[1\]\]/\[\["name"\]\] - the name of the labels column; \[\[2\]\]/\[\["new"\]\] - a (named) character vector of new labels for the 'variable', 'label' and 'value' columns in the long-format frame. See Examples. |
  |  |  |  |
  | ` "wider"` |  | column(s) containing labels of the new variables. Specified using a vector of column names, indices, a logical vector or selector function e.g. `is.character`. Multiple columns will be combined using [`finteraction`](https://fastverse.org/collapse/reference/qF.md) with `" - "` as separator. |
  |  |  |  |
  | ` "recast"` |  | (named) list with the following elements: \[\[1\]\]/\[\["from"\]\] - column(s) containing labels for the new variables, specified as in `"wider"`; \[\[2\]\]/\[\["to"\]\] - name of the variable to generate containing old labels; \[\[3\]\]/\[\["new"\]\] - a (named) character vector of new labels for the generated 'variable' and 'label' columns. If \[\[1\]\]/\[\["from"\]\] is not supplied, this can also include labels for new variables. Omitting one of the elements via a named list or setting it to `NULL` in a list of 3 will omit the corresponding operation i.e. either not saving existing labels or not assigning new ones. |

- how:

  character. The pivoting method: one of `"longer"`, `"wider"` or
  `"recast"`. These can be abbreviated by the first letter i.e.
  `"l"/"w"/"r"`.

- na.rm:

  logical. `TRUE` will remove missing values such that in the reshaped
  data there is no row missing all data columns - selected through
  'values'. For wide/recast pivots using internal `FUN`'s
  `"first"/"last"/"count"`, this also toggles skipping of missing
  values.

- factor:

  character. Whether to generate new 'names' and/or 'labels' columns as
  factor variables. This is generally recommended as factors are more
  memory efficient than character vectors and also faster in subsequent
  filtering and grouping. Internally, this argument is evaluated as
  `factor <- c("names", "labels") %in% factor`, so passing anything
  other than `"names"` and/or `"labels"` will disable it.

- check.dups:

  logical. `TRUE` checks for duplicate 'ids'+'names' combinations, and,
  if 'labels' are specified, also for duplicate 'names'+'labels'
  combinations. The default `FALSE` implies that the algorithm just runs
  through the data, leading effectively to the `FUN` option to be
  executed (default last value). See Details.

- FUN:

  function to aggregate values. At present, only a single function is
  allowed. [Fast Statistical
  Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
  receive vectorized execution. For maximum efficiency, a small set of
  internal functions is provided: `"first"`, `"last"`, `"count"`,
  `"sum"`, `"mean"`, `"min"`, or `"max"`. In options
  `"first"/"last"/"count"` setting `na.rm = TRUE` skips missing values.
  In options `"sum"/"mean"/"min"/"max"` missing values are always
  skipped (see Details why). The `fill` argument is ignored in
  `"count"/"sum"/"mean"/"min"/"max"` (`"count"/"sum"` force `fill = 0`
  else `NA` is used).

- FUN.args:

  (optional) list of arguments passed to `FUN` (if using an external
  function). Data-length arguments such as weight vectors are supported.

- nthreads:

  integer. if `how = "wider"|"recast"`: number of threads to use with
  OpenMP (default `get_collapse("nthreads")`, initialized to 1). Only
  the distribution of values to columns with `how = "wider"|"recast"` is
  multithreaded here. Since grouping id columns on a long data frame is
  expensive and serial, the gains are minor. With `how = "long"`,
  multithreading does not make much sense as the most expensive
  operation is allocating the long results vectors. The rest is a couple
  of `memset()`'s in C to copy the values.

- fill:

  if `how = "wider"|"recast"`: value to insert for 'ids'-'names'
  combinations not present in the long format. `NULL` uses `NA` for
  atomic vectors and `NULL` for lists.

- drop:

  logical. if `how = "wider"|"recast"` and 'names' is a single factor
  variable: `TRUE` will check for and drop unused levels in that factor,
  avoiding the generation of empty columns.

- sort:

  if `how = "wider"|"recast"`: specifying `"ids"` applies ordered
  grouping on the id-columns, returning data sorted by ids. Specifying
  `"names"` sorts the names before casting (unless 'names' is a factor),
  yielding columns cast in alphabetic order. Both options can be passed
  as a character vector, or, alternatively, `TRUE` can be used to enable
  both.

- transpose:

  if `how = "wider"|"recast"` and multiple columns are selected through
  'values': specifying `"columns"` applies
  [`t_list`](https://fastverse.org/collapse/reference/t_list.md) to the
  result before flattening, resulting in a different column order.
  Specifying `"names"` generates names of the form nami_colj, instead of
  colj_nami. Both options can be passed as a character vector, or,
  alternatively, `TRUE` can be used to enable both.

## Details

Pivot wider essentially works as follows: compute `g_rows = group(ids)`
and also `g_cols = group(names)` (using
[`group`](https://fastverse.org/collapse/reference/group.md) if
`sort = FALSE`). `g_rows` gives the row-numbers of the wider data frame
and `g_cols` the column numbers.

Then, a C function generates a wide data frame and runs through each
long column ('values'), assigning each value to the corresponding row
and column in the wide frame. In this process `FUN` is always applied.
The default, `"last"`, does nothing at all, i.e., if there are
duplicates, some values are overwritten. `"first"` works similarly just
that the C-loop is executed the other way around. The other hard-coded
options count, sum, average, or compare observations on the fly. Missing
values are internally skipped for statistical functions as there is no
way to distinguish an incoming `NA` from an initial `NA` - apart from
counting occurrences using an internal structure of the same size as the
result data frame which is costly and thus not implemented.

When passing an R-function to `FUN`, the data is grouped using
`g_full = group(g_rows, g_cols)`, aggregated by groups, and expanded
again to full length using
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) before entering
the reshaping algorithm. Thus, this is significantly more expensive than
the optimized internal functions. With [Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
the aggregation is vectorized across groups, other functions are applied
using [`BY`](https://fastverse.org/collapse/reference/BY.md) - by far
the slowest option.

If `check.dups = TRUE`, a check of the form
`fnunique(list(g_rows, g_cols)) < fnrow(data)` is run, and an
informative warning is issued if duplicates are found.

Recast pivoting works similarly. In long pivots `FUN` is ignored and the
check simply amounts to `fnunique(ids) < fnrow(data)`.

## Value

A reshaped data frame with the same class and attributes (except for
'names'/'row-names') as the input frame.

## Note

Leaving either 'ids' or 'values' empty will assign all other columns
(except for `"variable"` if `how = "wider"|"recast"`) to the
non-specified argument. It is also possible to leave both empty, e.g.
for complete melting if `how = "longer"` or data transposition if
`how = "recast"` (similar to
[`data.table::transpose`](https://rdrr.io/pkg/data.table/man/transpose.html)
but supporting multiple names columns and variable labels). See
Examples.

`pivot` currently does not support concurrently melting/pivoting longer
to multiple columns. See
[`data.table::melt`](https://rdrr.io/pkg/data.table/man/melt.data.table.html)
or `pivot_longer` from *tidyr* or *tidytable* for an efficient
alternative with this feature. It is also possible to achieve this with
just a little bit of programming. An example is provided below.

## See also

[`collap`](https://fastverse.org/collapse/reference/collap.md),
[`vec`](https://fastverse.org/collapse/reference/efficient-programming.md),
[`rowbind`](https://fastverse.org/collapse/reference/rowbind.md),
[`unlist2d`](https://fastverse.org/collapse/reference/unlist2d.md),
[Data Frame
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
# -------------------------------- PIVOT LONGER ---------------------------------
# Simple Melting (Reshaping Long)
pivot(mtcars) |> head()
#>   variable value
#> 1      mpg  21.0
#> 2      mpg  21.0
#> 3      mpg  22.8
#> 4      mpg  21.4
#> 5      mpg  18.7
#> 6      mpg  18.1
pivot(iris, "Species") |> head()
#>   Species     variable value
#> 1  setosa Sepal.Length   5.1
#> 2  setosa Sepal.Length   4.9
#> 3  setosa Sepal.Length   4.7
#> 4  setosa Sepal.Length   4.6
#> 5  setosa Sepal.Length   5.0
#> 6  setosa Sepal.Length   5.4
pivot(iris, values = 1:4) |> head() # Same thing
#>   Species     variable value
#> 1  setosa Sepal.Length   5.1
#> 2  setosa Sepal.Length   4.9
#> 3  setosa Sepal.Length   4.7
#> 4  setosa Sepal.Length   4.6
#> 5  setosa Sepal.Length   5.0
#> 6  setosa Sepal.Length   5.4

# Using collapse's datasets
head(wlddev)
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE    NA
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE    NA
#> 5 Afghanistan   AFG 1965-01-01 1964   1960 South Asia Low income FALSE    NA
#>   LIFEEX GINI       ODA     POP
#> 1 32.446   NA 116769997 8996973
#> 2 32.962   NA 232080002 9169410
#> 3 33.471   NA 112839996 9351441
#> 4 33.971   NA 237720001 9543205
#> 5 34.463   NA 295920013 9744781
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
pivot(wlddev, 1:8, na.rm = TRUE) |> head()
#>       country iso3c       date year decade     region     income  OECD variable
#> 1 Afghanistan   AFG 2003-01-01 2002   2000 South Asia Low income FALSE    PCGDP
#> 2 Afghanistan   AFG 2004-01-01 2003   2000 South Asia Low income FALSE    PCGDP
#> 3 Afghanistan   AFG 2005-01-01 2004   2000 South Asia Low income FALSE    PCGDP
#> 4 Afghanistan   AFG 2006-01-01 2005   2000 South Asia Low income FALSE    PCGDP
#> 5 Afghanistan   AFG 2007-01-01 2006   2000 South Asia Low income FALSE    PCGDP
#> 6 Afghanistan   AFG 2008-01-01 2007   2000 South Asia Low income FALSE    PCGDP
#>      value
#> 1 330.3036
#> 2 343.0809
#> 3 333.2167
#> 4 357.2347
#> 5 365.2845
#> 6 405.5490
pivot(wlddev, c("iso3c", "year"), c("PCGDP", "LIFEEX"), na.rm = TRUE) |> head()
#>   iso3c year variable    value
#> 1   AFG 2002    PCGDP 330.3036
#> 2   AFG 2003    PCGDP 343.0809
#> 3   AFG 2004    PCGDP 333.2167
#> 4   AFG 2005    PCGDP 357.2347
#> 5   AFG 2006    PCGDP 365.2845
#> 6   AFG 2007    PCGDP 405.5490
head(GGDC10S)
#>   Country Regioncode             Region Variable Year AGR MIN MAN PU CON WRT
#> 1     BWA        SSA Sub-saharan Africa       VA 1960  NA  NA  NA NA  NA  NA
#> 2     BWA        SSA Sub-saharan Africa       VA 1961  NA  NA  NA NA  NA  NA
#> 3     BWA        SSA Sub-saharan Africa       VA 1962  NA  NA  NA NA  NA  NA
#> 4     BWA        SSA Sub-saharan Africa       VA 1963  NA  NA  NA NA  NA  NA
#>   TRA FIRE GOV OTH SUM
#> 1  NA   NA  NA  NA  NA
#> 2  NA   NA  NA  NA  NA
#> 3  NA   NA  NA  NA  NA
#> 4  NA   NA  NA  NA  NA
#>  [ reached 'max' / getOption("max.print") -- omitted 2 rows ]
pivot(GGDC10S, 1:5, names = list("Sectorcode", "Value"), na.rm = TRUE) |> head()
#>   Country Regioncode             Region Variable Year Sectorcode    Value
#> 1     BWA        SSA Sub-saharan Africa       VA 1964        AGR 16.30154
#> 2     BWA        SSA Sub-saharan Africa       VA 1965        AGR 15.72700
#> 3     BWA        SSA Sub-saharan Africa       VA 1966        AGR 17.68066
#> 4     BWA        SSA Sub-saharan Africa       VA 1967        AGR 19.14591
#> 5     BWA        SSA Sub-saharan Africa       VA 1968        AGR 21.09957
#> 6     BWA        SSA Sub-saharan Africa       VA 1969        AGR 21.86221
# Can also set by name: variable and/or value. Note that 'value' here remains lowercase
pivot(GGDC10S, 1:5, names = list(variable = "Sectorcode"), na.rm = TRUE) |> head()
#>   Country Regioncode             Region Variable Year Sectorcode    value
#> 1     BWA        SSA Sub-saharan Africa       VA 1964        AGR 16.30154
#> 2     BWA        SSA Sub-saharan Africa       VA 1965        AGR 15.72700
#> 3     BWA        SSA Sub-saharan Africa       VA 1966        AGR 17.68066
#> 4     BWA        SSA Sub-saharan Africa       VA 1967        AGR 19.14591
#> 5     BWA        SSA Sub-saharan Africa       VA 1968        AGR 21.09957
#> 6     BWA        SSA Sub-saharan Africa       VA 1969        AGR 21.86221

# Melting including saving labels
pivot(GGDC10S, 1:5, na.rm = TRUE, labels = TRUE) |> head()
#>   Country Regioncode             Region Variable Year variable        label
#> 1     BWA        SSA Sub-saharan Africa       VA 1964      AGR Agriculture 
#> 2     BWA        SSA Sub-saharan Africa       VA 1965      AGR Agriculture 
#> 3     BWA        SSA Sub-saharan Africa       VA 1966      AGR Agriculture 
#> 4     BWA        SSA Sub-saharan Africa       VA 1967      AGR Agriculture 
#> 5     BWA        SSA Sub-saharan Africa       VA 1968      AGR Agriculture 
#> 6     BWA        SSA Sub-saharan Africa       VA 1969      AGR Agriculture 
#>      value
#> 1 16.30154
#> 2 15.72700
#> 3 17.68066
#> 4 19.14591
#> 5 21.09957
#> 6 21.86221
pivot(GGDC10S, 1:5, na.rm = TRUE, labels = "description") |> head()
#>   Country Regioncode             Region Variable Year variable  description
#> 1     BWA        SSA Sub-saharan Africa       VA 1964      AGR Agriculture 
#> 2     BWA        SSA Sub-saharan Africa       VA 1965      AGR Agriculture 
#> 3     BWA        SSA Sub-saharan Africa       VA 1966      AGR Agriculture 
#> 4     BWA        SSA Sub-saharan Africa       VA 1967      AGR Agriculture 
#> 5     BWA        SSA Sub-saharan Africa       VA 1968      AGR Agriculture 
#> 6     BWA        SSA Sub-saharan Africa       VA 1969      AGR Agriculture 
#>      value
#> 1 16.30154
#> 2 15.72700
#> 3 17.68066
#> 4 19.14591
#> 5 21.09957
#> 6 21.86221

# Also assigning new labels
pivot(GGDC10S, 1:5, na.rm = TRUE, labels = list("description",
            c("Sector Code", "Sector Description", "Value"))) |> namlab()
#>      Variable              Label
#> 1     Country            Country
#> 2  Regioncode        Region code
#> 3      Region             Region
#> 4    Variable           Variable
#> 5        Year               Year
#> 6    variable        Sector Code
#> 7 description Sector Description
#> 8       value              Value

# Can leave out value column by providing named vector of labels
pivot(GGDC10S, 1:5, na.rm = TRUE, labels = list("description",
          c(variable = "Sector Code", description = "Sector Description"))) |> namlab()
#>      Variable              Label
#> 1     Country            Country
#> 2  Regioncode        Region code
#> 3      Region             Region
#> 4    Variable           Variable
#> 5        Year               Year
#> 6    variable        Sector Code
#> 7 description Sector Description
#> 8       value               <NA>

# Now here is a nice example that is explicit and respects the dataset naming conventions
pivot(GGDC10S, ids = 1:5, na.rm = TRUE,
      names = list(variable = "Sectorcode",
                   value = "Value"),
      labels = list(name = "Sector",
                    new = c(Sectorcode = "GGDC10S Sector Code",
                            Sector = "Long Sector Description",
                            Value = "Employment or Value Added"))) |>
  namlab(N = TRUE, Nd = TRUE, class = TRUE)
#>     Variable     Class     N Ndist                     Label
#> 1    Country character 46942    43                   Country
#> 2 Regioncode character 46942     6               Region code
#> 3     Region character 46942     6                    Region
#> 4   Variable character 46942     2                  Variable
#> 5       Year   numeric 46942    67                      Year
#> 6 Sectorcode    factor 46942    11       GGDC10S Sector Code
#> 7     Sector    factor 46942    11   Long Sector Description
#> 8      Value   numeric 46942 46478 Employment or Value Added

# Note that pivot() currently does not support melting to multiple columns
# But you can tackle the issue with a bit of programming:
wide <- pivot(GGDC10S, c("Country", "Year"), c("AGR", "MAN", "SUM"), "Variable",
              how = "wider", na.rm = TRUE)
head(wide)
#>   Country Year   AGR_VA  AGR_EMP    MAN_VA  MAN_EMP   SUM_VA  SUM_EMP
#> 1     BWA 1964 16.30154 152.1179 0.7365696 2.420000 37.48229 173.8829
#> 2     BWA 1965 15.72700 153.2971 1.0181992 2.330406 39.34710 178.7637
#> 3     BWA 1966 17.68066 153.8867 0.8038415 1.281642 43.14677 179.7183
#> 4     BWA 1967 19.14591 155.0659 0.9378151 1.041623 41.39519 178.9181
#> 5     BWA 1968 21.09957 156.2451 0.7502521 1.069332 41.14259 181.9292
#> 6     BWA 1969 21.86221 157.4243 2.1396077 2.124402 51.22160 188.0569
library(magrittr)
wide %>% {av(pivot(., 1:2, grep("_VA", names(.))), pivot(gvr(., "_EMP")))} |> head()
#>   Country Year variable    value variable    value
#> 1     BWA 1964   AGR_VA 16.30154  AGR_EMP 152.1179
#> 2     BWA 1965   AGR_VA 15.72700  AGR_EMP 153.2971
#> 3     BWA 1966   AGR_VA 17.68066  AGR_EMP 153.8867
#> 4     BWA 1967   AGR_VA 19.14591  AGR_EMP 155.0659
#> 5     BWA 1968   AGR_VA 21.09957  AGR_EMP 156.2451
#> 6     BWA 1969   AGR_VA 21.86221  AGR_EMP 157.4243
wide %>% {av(av(gv(., 1:2), rm_stub(gvr(., "_VA"), "_VA", pre = FALSE)) |>
                   pivot(1:2, names = list("Sectorcode", "VA"), labels = "Sector"),
             EMP = vec(gvr(., "_EMP")))} |> head()
#>   Country Year Sectorcode       Sector       VA      EMP
#> 1     BWA 1964        AGR Agriculture  16.30154 152.1179
#> 2     BWA 1965        AGR Agriculture  15.72700 153.2971
#> 3     BWA 1966        AGR Agriculture  17.68066 153.8867
#> 4     BWA 1967        AGR Agriculture  19.14591 155.0659
#> 5     BWA 1968        AGR Agriculture  21.09957 156.2451
#> 6     BWA 1969        AGR Agriculture  21.86221 157.4243
rm(wide)

# -------------------------------- PIVOT WIDER ---------------------------------
iris_long <- pivot(iris, "Species") # Getting a long frame
head(iris_long)
#>   Species     variable value
#> 1  setosa Sepal.Length   5.1
#> 2  setosa Sepal.Length   4.9
#> 3  setosa Sepal.Length   4.7
#> 4  setosa Sepal.Length   4.6
#> 5  setosa Sepal.Length   5.0
#> 6  setosa Sepal.Length   5.4
# If 'names'/'values' not supplied, searches for 'variable' and 'value' columns
pivot(iris_long, how = "wider")
#>      Species Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1     setosa          5.0         3.3          1.4         0.2
#> 2 versicolor          5.7         2.8          4.1         1.3
#> 3  virginica          5.9         3.0          5.1         1.8
# But here the records are not identified by 'Species': thus aggregation with last value:
pivot(iris_long, how = "wider", check = TRUE) # issues a warning
#> Warning: duplicates detected: there are 12 unique combinations of id- and name-columns, but the data has 600 rows. This means you have on average 50 duplicates per id-name-combination. If how = 'wider', pivot() will take the last of those duplicates in first-appearance-order. Consider aggregating your data e.g. using collap() before applying pivot().
#>      Species Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1     setosa          5.0         3.3          1.4         0.2
#> 2 versicolor          5.7         2.8          4.1         1.3
#> 3  virginica          5.9         3.0          5.1         1.8
rm(iris_long)

# This works better, these two are inverse operations
wlddev |> pivot(1:8) |> pivot(how = "w") |> head()
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE    NA
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE    NA
#> 5 Afghanistan   AFG 1965-01-01 1964   1960 South Asia Low income FALSE    NA
#>   LIFEEX GINI       ODA     POP
#> 1 32.446   NA 116769997 8996973
#> 2 32.962   NA 232080002 9169410
#> 3 33.471   NA 112839996 9351441
#> 4 33.971   NA 237720001 9543205
#> 5 34.463   NA 295920013 9744781
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
# ...but not perfect, we loose labels
namlab(wlddev)
#>    Variable
#> 1   country
#> 2     iso3c
#> 3      date
#> 4      year
#> 5    decade
#> 6    region
#> 7    income
#> 8      OECD
#> 9     PCGDP
#> 10   LIFEEX
#> 11     GINI
#> 12      ODA
#> 13      POP
#>                                                                                Label
#> 1                                                                       Country Name
#> 2                                                                       Country Code
#> 3                                                         Date Recorded (Fictitious)
#> 4                                                                               Year
#> 5                                                                             Decade
#> 6                                                                             Region
#> 7                                                                       Income Level
#> 8                                                            Is OECD Member Country?
#> 9                                                 GDP per capita (constant 2010 US$)
#> 10                                           Life expectancy at birth, total (years)
#> 11                                                  Gini index (World Bank estimate)
#> 12 Net official development assistance and official aid received (constant 2018 US$)
#> 13                                                                 Population, total
wlddev |> pivot(1:8) |> pivot(how = "w") |> namlab()
#>    Variable                      Label
#> 1   country               Country Name
#> 2     iso3c               Country Code
#> 3      date Date Recorded (Fictitious)
#> 4      year                       Year
#> 5    decade                     Decade
#> 6    region                     Region
#> 7    income               Income Level
#> 8      OECD    Is OECD Member Country?
#> 9     PCGDP                       <NA>
#> 10   LIFEEX                       <NA>
#> 11     GINI                       <NA>
#> 12      ODA                       <NA>
#> 13      POP                       <NA>
# But pivot() supports labels: these are perfect inverse operations
wlddev |> pivot(1:8, labels = "label") |> print(max = 50) |> # Notice the "label" column
  pivot(how = "w", labels = "label") |> namlab()
#>       country iso3c       date year decade     region     income  OECD variable
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    PCGDP
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    PCGDP
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE    PCGDP
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE    PCGDP
#>                                label value
#> 1 GDP per capita (constant 2010 US$)    NA
#> 2 GDP per capita (constant 2010 US$)    NA
#> 3 GDP per capita (constant 2010 US$)    NA
#> 4 GDP per capita (constant 2010 US$)    NA
#>  [ reached 'max' / getOption("max.print") -- omitted 65876 rows ]
#>    Variable
#> 1   country
#> 2     iso3c
#> 3      date
#> 4      year
#> 5    decade
#> 6    region
#> 7    income
#> 8      OECD
#> 9     PCGDP
#> 10   LIFEEX
#> 11     GINI
#> 12      ODA
#> 13      POP
#>                                                                                Label
#> 1                                                                       Country Name
#> 2                                                                       Country Code
#> 3                                                         Date Recorded (Fictitious)
#> 4                                                                               Year
#> 5                                                                             Decade
#> 6                                                                             Region
#> 7                                                                       Income Level
#> 8                                                            Is OECD Member Country?
#> 9                                                 GDP per capita (constant 2010 US$)
#> 10                                           Life expectancy at birth, total (years)
#> 11                                                  Gini index (World Bank estimate)
#> 12 Net official development assistance and official aid received (constant 2018 US$)
#> 13                                                                 Population, total

# If the data does not have 'variable'/'value' cols: need to specify 'names'/'values'
# Using a single column:
pivot(GGDC10S, c("Country", "Year"), "SUM", "Variable", how = "w") |> head()
#>   Country Year       VA      EMP
#> 1     BWA 1960       NA       NA
#> 2     BWA 1961       NA       NA
#> 3     BWA 1962       NA       NA
#> 4     BWA 1963       NA       NA
#> 5     BWA 1964 37.48229 173.8829
#> 6     BWA 1965 39.34710 178.7637
SUM_wide <- pivot(GGDC10S, c("Country", "Year"), "SUM", "Variable", how = "w", na.rm = TRUE)
head(SUM_wide) # na.rm = TRUE here removes all new rows completely missing data
#>   Country Year       VA      EMP
#> 1     BWA 1964 37.48229 173.8829
#> 2     BWA 1965 39.34710 178.7637
#> 3     BWA 1966 43.14677 179.7183
#> 4     BWA 1967 41.39519 178.9181
#> 5     BWA 1968 41.14259 181.9292
#> 6     BWA 1969 51.22160 188.0569
tail(SUM_wide) # But there may still be NA's, notice the NA in the final row
#>      Country Year        VA      EMP
#> 2341     EGY 2008  844222.3 21039.90
#> 2342     EGY 2009  978684.0 21863.86
#> 2343     EGY 2010 1133629.1 22019.88
#> 2344     EGY 2011 1290896.1 22219.39
#> 2345     EGY 2012 1487175.1 22532.56
#> 2346     EGY 2013 1650962.8       NA
# We could use fill to set another value
pivot(GGDC10S, c("Country", "Year"), "SUM", "Variable", how = "w",
      na.rm = TRUE, fill = -9999) |> tail()
#>      Country Year        VA      EMP
#> 2341     EGY 2008  844222.3 21039.90
#> 2342     EGY 2009  978684.0 21863.86
#> 2343     EGY 2010 1133629.1 22019.88
#> 2344     EGY 2011 1290896.1 22219.39
#> 2345     EGY 2012 1487175.1 22532.56
#> 2346     EGY 2013 1650962.8 -9999.00
# This will keep the label of "SUM", unless we supply a column with new labels
namlab(SUM_wide)
#>   Variable                   Label
#> 1  Country                 Country
#> 2     Year                    Year
#> 3       VA Summation of sector GDP
#> 4      EMP Summation of sector GDP
# Such a column is not available here, but we could use "Variable" twice
pivot(GGDC10S, c("Country", "Year"), "SUM", "Variable", "Variable", how = "w",
      na.rm = TRUE) |> namlab()
#>   Variable   Label
#> 1  Country Country
#> 2     Year    Year
#> 3       VA      VA
#> 4      EMP     EMP
# Alternatively, can of course relabel ex-post
SUM_wide |> relabel(VA = "Value Added", EMP = "Employment") |> namlab()
#>   Variable       Label
#> 1  Country     Country
#> 2     Year        Year
#> 3       VA Value Added
#> 4      EMP  Employment
rm(SUM_wide)

# Multiple-column pivots
pivot(GGDC10S, c("Country", "Year"), c("AGR", "MAN", "SUM"), "Variable", how = "w",
      na.rm = TRUE) |> head()
#>   Country Year   AGR_VA  AGR_EMP    MAN_VA  MAN_EMP   SUM_VA  SUM_EMP
#> 1     BWA 1964 16.30154 152.1179 0.7365696 2.420000 37.48229 173.8829
#> 2     BWA 1965 15.72700 153.2971 1.0181992 2.330406 39.34710 178.7637
#> 3     BWA 1966 17.68066 153.8867 0.8038415 1.281642 43.14677 179.7183
#> 4     BWA 1967 19.14591 155.0659 0.9378151 1.041623 41.39519 178.9181
#> 5     BWA 1968 21.09957 156.2451 0.7502521 1.069332 41.14259 181.9292
#> 6     BWA 1969 21.86221 157.4243 2.1396077 2.124402 51.22160 188.0569
# Here we may prefer a transposed column order
pivot(GGDC10S, c("Country", "Year"), c("AGR", "MAN", "SUM"), "Variable", how = "w",
      na.rm = TRUE, transpose = "columns") |> head()
#>   Country Year   AGR_VA    MAN_VA   SUM_VA  AGR_EMP  MAN_EMP  SUM_EMP
#> 1     BWA 1964 16.30154 0.7365696 37.48229 152.1179 2.420000 173.8829
#> 2     BWA 1965 15.72700 1.0181992 39.34710 153.2971 2.330406 178.7637
#> 3     BWA 1966 17.68066 0.8038415 43.14677 153.8867 1.281642 179.7183
#> 4     BWA 1967 19.14591 0.9378151 41.39519 155.0659 1.041623 178.9181
#> 5     BWA 1968 21.09957 0.7502521 41.14259 156.2451 1.069332 181.9292
#> 6     BWA 1969 21.86221 2.1396077 51.22160 157.4243 2.124402 188.0569
# Can also flip the order of names (independently of columns)
pivot(GGDC10S, c("Country", "Year"), c("AGR", "MAN", "SUM"), "Variable", how = "w",
      na.rm = TRUE, transpose = "names") |> head()
#>   Country Year   VA_AGR  EMP_AGR    VA_MAN  EMP_MAN   VA_SUM  EMP_SUM
#> 1     BWA 1964 16.30154 152.1179 0.7365696 2.420000 37.48229 173.8829
#> 2     BWA 1965 15.72700 153.2971 1.0181992 2.330406 39.34710 178.7637
#> 3     BWA 1966 17.68066 153.8867 0.8038415 1.281642 43.14677 179.7183
#> 4     BWA 1967 19.14591 155.0659 0.9378151 1.041623 41.39519 178.9181
#> 5     BWA 1968 21.09957 156.2451 0.7502521 1.069332 41.14259 181.9292
#> 6     BWA 1969 21.86221 157.4243 2.1396077 2.124402 51.22160 188.0569
# Can also enable both (complete transposition)
pivot(GGDC10S, c("Country", "Year"), c("AGR", "MAN", "SUM"), "Variable", how = "w",
      na.rm = TRUE, transpose = TRUE) |> head() # or tranpose = c("columns", "names")
#>   Country Year   VA_AGR    VA_MAN   VA_SUM  EMP_AGR  EMP_MAN  EMP_SUM
#> 1     BWA 1964 16.30154 0.7365696 37.48229 152.1179 2.420000 173.8829
#> 2     BWA 1965 15.72700 1.0181992 39.34710 153.2971 2.330406 178.7637
#> 3     BWA 1966 17.68066 0.8038415 43.14677 153.8867 1.281642 179.7183
#> 4     BWA 1967 19.14591 0.9378151 41.39519 155.0659 1.041623 178.9181
#> 5     BWA 1968 21.09957 0.7502521 41.14259 156.2451 1.069332 181.9292
#> 6     BWA 1969 21.86221 2.1396077 51.22160 157.4243 2.124402 188.0569

# Finally, here is a nice, simple way to reshape the entire dataset.
pivot(GGDC10S, values = 6:16, names = "Variable", na.rm = TRUE, how = "w") |>
  namlab(N = TRUE, Nd = TRUE, class = TRUE)
#>      Variable     Class    N Ndist         Label
#> 1     Country character 2346    43       Country
#> 2  Regioncode character 2346     6   Region code
#> 3      Region character 2346     6        Region
#> 4        Year   numeric 2346    67          Year
#> 5      AGR_VA   numeric 2139  2135  Agriculture 
#> 6     AGR_EMP   numeric 2225  2219  Agriculture 
#> 7      MIN_VA   numeric 2139  2072        Mining
#> 8     MIN_EMP   numeric 2216  2153        Mining
#> 9      MAN_VA   numeric 2139  2139 Manufacturing
#> 10    MAN_EMP   numeric 2216  2214 Manufacturing
#> 11      PU_VA   numeric 2139  2097     Utilities
#> 12     PU_EMP   numeric 2215  2141     Utilities
#> 13     CON_VA   numeric 2139  2130  Construction
#> 14    CON_EMP   numeric 2216  2209  Construction
#>  [ reached 'max' / getOption("max.print") -- omitted 12 rows ]

# -------------------------------- PIVOT RECAST ---------------------------------
# Look at the data again
head(GGDC10S)
#>   Country Regioncode             Region Variable Year AGR MIN MAN PU CON WRT
#> 1     BWA        SSA Sub-saharan Africa       VA 1960  NA  NA  NA NA  NA  NA
#> 2     BWA        SSA Sub-saharan Africa       VA 1961  NA  NA  NA NA  NA  NA
#> 3     BWA        SSA Sub-saharan Africa       VA 1962  NA  NA  NA NA  NA  NA
#> 4     BWA        SSA Sub-saharan Africa       VA 1963  NA  NA  NA NA  NA  NA
#>   TRA FIRE GOV OTH SUM
#> 1  NA   NA  NA  NA  NA
#> 2  NA   NA  NA  NA  NA
#> 3  NA   NA  NA  NA  NA
#> 4  NA   NA  NA  NA  NA
#>  [ reached 'max' / getOption("max.print") -- omitted 2 rows ]
# Let's stack the sectors and instead create variable columns
pivot(GGDC10S, .c(Country, Regioncode, Region, Year),
      names = list("Variable", "Sectorcode"), how = "r") |> head()
#>   Country Regioncode             Region Year Sectorcode       VA      EMP
#> 1     BWA        SSA Sub-saharan Africa 1960        AGR       NA       NA
#> 2     BWA        SSA Sub-saharan Africa 1961        AGR       NA       NA
#> 3     BWA        SSA Sub-saharan Africa 1962        AGR       NA       NA
#> 4     BWA        SSA Sub-saharan Africa 1963        AGR       NA       NA
#> 5     BWA        SSA Sub-saharan Africa 1964        AGR 16.30154 152.1179
#> 6     BWA        SSA Sub-saharan Africa 1965        AGR 15.72700 153.2971
# Same thing (a bit easier)
pivot(GGDC10S, values = 6:16, names = list("Variable", "Sectorcode"), how = "r") |> head()
#>   Country Regioncode             Region Year Sectorcode       VA      EMP
#> 1     BWA        SSA Sub-saharan Africa 1960        AGR       NA       NA
#> 2     BWA        SSA Sub-saharan Africa 1961        AGR       NA       NA
#> 3     BWA        SSA Sub-saharan Africa 1962        AGR       NA       NA
#> 4     BWA        SSA Sub-saharan Africa 1963        AGR       NA       NA
#> 5     BWA        SSA Sub-saharan Africa 1964        AGR 16.30154 152.1179
#> 6     BWA        SSA Sub-saharan Africa 1965        AGR 15.72700 153.2971
# Removing missing values
pivot(GGDC10S, values = 6:16, names = list("Variable", "Sectorcode"), how = "r",
      na.rm = TRUE) |> head()
#>   Country Regioncode             Region Year Sectorcode       VA      EMP
#> 1     BWA        SSA Sub-saharan Africa 1960        AGR       NA       NA
#> 2     BWA        SSA Sub-saharan Africa 1961        AGR       NA       NA
#> 3     BWA        SSA Sub-saharan Africa 1962        AGR       NA       NA
#> 4     BWA        SSA Sub-saharan Africa 1963        AGR       NA       NA
#> 5     BWA        SSA Sub-saharan Africa 1964        AGR 16.30154 152.1179
#> 6     BWA        SSA Sub-saharan Africa 1965        AGR 15.72700 153.2971
# Saving Labels
pivot(GGDC10S, values = 6:16, names = list("Variable", "Sectorcode"),
      labels = list(to = "Sector"), how = "r", na.rm = TRUE) |> head()
#>   Country Regioncode             Region Year Sectorcode       Sector       VA
#> 1     BWA        SSA Sub-saharan Africa 1960        AGR Agriculture        NA
#> 2     BWA        SSA Sub-saharan Africa 1961        AGR Agriculture        NA
#> 3     BWA        SSA Sub-saharan Africa 1962        AGR Agriculture        NA
#> 4     BWA        SSA Sub-saharan Africa 1963        AGR Agriculture        NA
#> 5     BWA        SSA Sub-saharan Africa 1964        AGR Agriculture  16.30154
#> 6     BWA        SSA Sub-saharan Africa 1965        AGR Agriculture  15.72700
#>        EMP
#> 1       NA
#> 2       NA
#> 3       NA
#> 4       NA
#> 5 152.1179
#> 6 153.2971

# Supplying new labels for generated columns: as complete as it gets
pivot(GGDC10S, values = 6:16, names = list("Variable", "Sectorcode"),
      labels = list(to = "Sector",
                    new = c(Sectorcode = "GGDC10S Sector Code",
                            Sector = "Long Sector Description",
                            VA = "Value Added",
                            EMP = "Employment")), how = "r", na.rm = TRUE) |>
  namlab(N = TRUE, Nd = TRUE, class = TRUE)
#>     Variable     Class     N Ndist                   Label
#> 1    Country character 27852    43                 Country
#> 2 Regioncode character 27852     6             Region code
#> 3     Region character 27852     6                  Region
#> 4       Year   numeric 27852    67                    Year
#> 5 Sectorcode    factor 27852    11     GGDC10S Sector Code
#> 6     Sector    factor 27852    11 Long Sector Description
#> 7         VA   numeric 23092 22915             Value Added
#> 8        EMP   numeric 23850 23610              Employment

# Now another (slightly unconventional) use case here is data transposition
# Let's get the data for Botswana
BWA <- GGDC10S |> fsubset(Country == "BWA", Variable, Year, AGR:SUM)
head(BWA)
#>   Variable Year      AGR      MIN       MAN        PU       CON      WRT
#> 1       VA 1960       NA       NA        NA        NA        NA       NA
#> 2       VA 1961       NA       NA        NA        NA        NA       NA
#> 3       VA 1962       NA       NA        NA        NA        NA       NA
#> 4       VA 1963       NA       NA        NA        NA        NA       NA
#> 5       VA 1964 16.30154 3.494075 0.7365696 0.1043936 0.6600454 6.243732
#>        TRA     FIRE      GOV      OTH      SUM
#> 1       NA       NA       NA       NA       NA
#> 2       NA       NA       NA       NA       NA
#> 3       NA       NA       NA       NA       NA
#> 4       NA       NA       NA       NA       NA
#> 5 1.658928 1.119194 4.822485 2.341328 37.48229
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
# By supplying no ids or values, we are simply requesting a transpose operation
pivot(BWA, names = list(from = c("Variable", "Year"), to = "Sectorcode"), how = "r")
#>      Sectorcode VA_1960 VA_1961 VA_1962 VA_1963 VA_1964 VA_1965 VA_1966 VA_1967
#>      VA_1968 VA_1969 VA_1970 VA_1971 VA_1972 VA_1973 VA_1974 VA_1975 VA_1976
#>      VA_1977 VA_1978 VA_1979 VA_1980 VA_1981 VA_1982 VA_1983 VA_1984 VA_1985
#>      VA_1986 VA_1987 VA_1988 VA_1989 VA_1990 VA_1991 VA_1992 VA_1993 VA_1994
#>      VA_1995 VA_1996 VA_1997 VA_1998 VA_1999 VA_2000 VA_2001 VA_2002 VA_2003
#>      VA_2004 VA_2005 VA_2006 VA_2007 VA_2008 VA_2009 VA_2010 VA_2011 EMP_1960
#>      EMP_1961 EMP_1962 EMP_1963 EMP_1964 EMP_1965 EMP_1966 EMP_1967 EMP_1968
#>      EMP_1969 EMP_1970 EMP_1971 EMP_1972 EMP_1973 EMP_1974 EMP_1975 EMP_1976
#>  [ reached 'max' / getOption("max.print") -- omitted 35 columns ]
#>  [ reached 'max' / getOption("max.print") -- omitted 11 rows ]
# Same with labels
pivot(BWA, names = list(from = c("Variable", "Year"), to = "Sectorcode"),
      labels = list(to = "Sector"), how = "r")
#>      Sectorcode Sector VA_1960 VA_1961 VA_1962 VA_1963 VA_1964 VA_1965 VA_1966
#>      VA_1967 VA_1968 VA_1969 VA_1970 VA_1971 VA_1972 VA_1973 VA_1974 VA_1975
#>      VA_1976 VA_1977 VA_1978 VA_1979 VA_1980 VA_1981 VA_1982 VA_1983 VA_1984
#>      VA_1985 VA_1986 VA_1987 VA_1988 VA_1989 VA_1990 VA_1991 VA_1992 VA_1993
#>      VA_1994 VA_1995 VA_1996 VA_1997 VA_1998 VA_1999 VA_2000 VA_2001 VA_2002
#>      VA_2003 VA_2004 VA_2005 VA_2006 VA_2007 VA_2008 VA_2009 VA_2010 VA_2011
#>      EMP_1960 EMP_1961 EMP_1962 EMP_1963 EMP_1964 EMP_1965 EMP_1966 EMP_1967
#>      EMP_1968 EMP_1969 EMP_1970 EMP_1971 EMP_1972 EMP_1973 EMP_1974 EMP_1975
#>  [ reached 'max' / getOption("max.print") -- omitted 36 columns ]
#>  [ reached 'max' / getOption("max.print") -- omitted 11 rows ]
# For simple cases, data.table::transpose() will be more efficient, but with multiple
# columns to generate names and/or variable labels to be saved/assigned, pivot() is handy
rm(BWA)
```
