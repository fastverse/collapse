# Fast Matching

Fast matching of elements/rows in `x` to elements/rows in `table`.

This is a much faster replacement for
[`match`](https://rdrr.io/r/base/match.html) that works with atomic
vectors and data frames / lists of equal-length vectors. It is the
workhorse function of
[`join`](https://fastverse.org/collapse/reference/join.md).

## Usage

``` r
fmatch(x, table, nomatch = NA_integer_,
       count = FALSE, overid = 1L)

# Check match: throws an informative error for non-matched elements
# Default message reflects frequent internal use to check data frame columns
ckmatch(x, table, e = "Unknown columns:", ...)

# Infix operators based on fmatch():
x %!in% table
x %iin% table
x %!iin% table
# Use set_collapse(mask = "%in%") to replace %in% with
# a much faster version based on fmatch()
```

## Arguments

- x:

  a vector, list or data frame whose elements are matched against
  `table`. If a list/data frame, matches are found by comparing rows,
  unlike [`match`](https://rdrr.io/r/base/match.html) which compares
  columns.

- table:

  a vector, list or data frame to match against.

- nomatch:

  integer. Value to be returned in the case when no match is found.
  Default is `NA_integer_`.

- count:

  logical. Counts number of (unique) matches and attaches 4 attributes:

  - `"N.nomatch"`: The number of elements in `x` not matched
    `= sum(result == nomatch)`.

  - `"N.groups"`: The size of the table ` = NROW(table)`.

  - `"N.distinct"`: The number of unique matches
    ` = fndistinct(result[result != nomatch])`.

  - `"class"`: The
    [`"qG"`](https://fastverse.org/collapse/reference/qF.md) class:
    needed for optimized computations on the results object (e.g.
    `funique(result)`, which is needed for a full join).

  *Note* that computing these attributes requires an extra pass through
  the matching vector. Also note that these attributes contain no
  general information about whether either `x` or `table` are unique,
  except for two special cases when N.groups = N.distinct (table is
  unique) or length(result) = N.distinct (x is unique). Otherwise use
  [`any_duplicated`](https://fastverse.org/collapse/reference/funique.md)
  to check x/table.

- overid:

  integer. If `x/table` are lists/data frames, `fmatch` compares the
  rows incrementally, starting with the first two columns, and matching
  further columns as necessary (see Details). Overidentification
  corresponds to the case when a subset of the columns uniquely identify
  the data. In this case this argument controls the behavior:

  - `0`: Early termination: stop matching additional columns. Most
    efficient.

  - `1`: Continue matching columns and issue a warning that the data is
    overidentified.

  - `2`: Continue matching columns without warning.

- e:

  the error message thrown by `ckmatch` for non-matched elements. The
  message is followed by the comma-separated non-matched elements.

- ...:

  further arguments to `fmatch`.

## Value

Integer vector containing the positions of first matches of `x` in
`table`. `nomatch` is returned for elements of `x` that have no match in
`table`. If `count = TRUE`, the result has additional attributes and a
class [`"qG"`](https://fastverse.org/collapse/reference/qF.md).

## Details

With data frames / lists, `fmatch` compares the rows but moves through
the data on a column-by-column basis (like a vectorized hash join
algorithm). With two or more columns, the first two columns are hashed
simultaneously for speed. Further columns can be added to this match. It
is likely that the first 2, 3, 4 etc. columns of a data frame fully
identify the data. After each column `fmatch()` internally checks
whether the `table` rows that are still eligible for matching
(eliminating `nomatch` rows from earlier columns) are unique. If this is
the case and `overid = 0`, `fmatch()` terminates early without
considering further columns. This is efficient but may give
undesirable/wrong results if considering further columns would turn some
additional elements of the result vector into `nomatch` values.

## See also

[`join`](https://fastverse.org/collapse/reference/join.md),
[`funique`](https://fastverse.org/collapse/reference/funique.md),
[`group`](https://fastverse.org/collapse/reference/group.md), [Fast
Grouping and
Ordering](https://fastverse.org/collapse/reference/fast-grouping-ordering.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
x <- c("b", "c", "a", "e", "f", "ff")
fmatch(x, letters)
#> [1]  2  3  1  5  6 NA
fmatch(x, letters, nomatch = 0)
#> [1] 2 3 1 5 6 0
fmatch(x, letters, count = TRUE)
#> [1]  2  3  1  5  6 NA
#> attr(,"N.nomatch")
#> [1] 1
#> attr(,"N.groups")
#> [1] 26
#> attr(,"N.distinct")
#> [1] 5
#> attr(,"class")
#> [1] "qG"

# Table 1
df1 <- data.frame(
  id1 = c(1, 1, 2, 3),
  id2 = c("a", "b", "b", "c"),
  name = c("John", "Bob", "Jane", "Carl")
)
head(df1)
#>   id1 id2 name
#> 1   1   a John
#> 2   1   b  Bob
#> 3   2   b Jane
#> 4   3   c Carl
# Table 2
df2 <- data.frame(
  id1 = c(1, 2, 3, 3),
  id2 = c("a", "b", "c", "e"),
  name = c("John", "Janne", "Carl", "Lynne")
)
head(df2)
#>   id1 id2  name
#> 1   1   a  John
#> 2   2   b Janne
#> 3   3   c  Carl
#> 4   3   e Lynne

# This gives an overidentification warning: columns 1:2 identify the data
if(FALSE) fmatch(df1, df2)
# This just runs through without warning
fmatch(df1, df2, overid = 2)
#> [1]  1 NA NA  3
# This terminates computation after first 2 columns
fmatch(df1, df2, overid = 0)
#> [1]  1 NA  2  3
fmatch(df1[1:2], df2[1:2])  # Same thing!
#> [1]  1 NA  2  3
# -> note that here we get an additional match based on the unique ids,
# which we didn't get before because "Jane" != "Janne"
```
