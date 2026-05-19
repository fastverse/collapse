# Fast Unique Elements / Rows

`funique` is an efficient alternative to
[`unique`](https://rdrr.io/pkg/data.table/man/duplicated.html) (or
`unique.data.table, kit::funique, dplyr::distinct`).

`fnunique` is an alternative to `NROW(unique(x))` (or
`data.table::uniqueN, kit::uniqLen, dplyr::n_distinct`).

`fduplicated` is an alternative to
[`duplicated`](https://rdrr.io/pkg/data.table/man/duplicated.html) (or
`duplicated.data.table`,
[`kit::fduplicated`](https://fastverse.org/kit/reference/funique.html)).

The *collapse* versions are versatile and highly competitive.

`any_duplicated(x)` is faster than `any(fduplicated(x))`. *Note* that
for atomic vectors,
[`anyDuplicated`](https://rdrr.io/pkg/data.table/man/duplicated.html) is
currently more efficient if there are duplicates at the beginning of the
vector.

## Usage

``` r
funique(x, ...)

# Default S3 method
funique(x, sort = FALSE, method = "auto", ...)

# S3 method for class 'data.frame'
funique(x, cols = NULL, sort = FALSE, method = "auto", ...)

# S3 method for class 'sf'
funique(x, cols = NULL, sort = FALSE, method = "auto", ...)

# Methods for indexed data / compatibility with plm:

# S3 method for class 'pseries'
funique(x, sort = FALSE, method = "auto", drop.index.levels = "id", ...)

# S3 method for class 'pdata.frame'
funique(x, cols = NULL, sort = FALSE, method = "auto", drop.index.levels = "id", ...)


fnunique(x)                  # Fast NROW(unique(x)), for vectors and lists
fduplicated(x, all = FALSE)  # Fast duplicated(x), for vectors and lists
any_duplicated(x)            # Simple logical TRUE|FALSE duplicates check
```

## Arguments

- x:

  a atomic vector or data frame / list of equal-length columns.

- sort:

  logical. `TRUE` orders the unique elements / rows. `FALSE` returns
  unique values in order of first occurrence.

- method:

  an integer or character string specifying the method of computation:

  |  |  |  |  |  |
  |----|----|----|----|----|
  | *Int.* |  | *String* |  | *Description* |
  | 1 |  | "auto" |  | automatic selection: hash if `sort = FALSE` else radix. |
  | 2 |  | "radix" |  | use radix ordering to determine unique values. Supports `sort = FALSE` but only for character data. |
  | 3 |  | "hash" |  | use index hashing to determine unique values. Supports `sort = TRUE` but only for atomic vectors (default method). |

- cols:

  compute unique rows according to a subset of columns. Columns can be
  selected using column names, indices, a logical vector or a selector
  function (e.g. `is.character`). *Note:* All columns are returned.

- ...:

  arguments passed to
  [`radixorder`](https://fastverse.org/collapse/reference/radixorder.md),
  e.g. `decreasing` or `na.last`. Only applicable if `method = "radix"`.

- drop.index.levels:

  character. Either `"id"`, `"time"`, `"all"` or `"none"`. See
  [indexing](https://fastverse.org/collapse/reference/indexing.md).

- all:

  logical. `TRUE` returns all duplicated values, including the first
  occurrence.

## Details

If all values/rows are already unique, then `x` is returned. Otherwise a
copy of `x` with duplicate rows removed is returned. See
[`group`](https://fastverse.org/collapse/reference/group.md) for some
additional computational details.

The *sf* method simply ignores the geometry column when determining
unique values.

Methods for indexed data also subset the index accordingly.

`any_duplicated` is currently simply implemented as
`fnunique(x) < NROW(x)`, which means it does not have facilities to
terminate early, and users are advised to use
[`anyDuplicated`](https://rdrr.io/pkg/data.table/man/duplicated.html)
with atomic vectors if chances are high that there are duplicates at the
beginning of the vector. With no duplicate values or data frames,
`any_duplicated` is considerably faster than
[`anyDuplicated`](https://rdrr.io/pkg/data.table/man/duplicated.html).

## Note

These functions treat lists like data frames, unlike
[`unique`](https://rdrr.io/pkg/data.table/man/duplicated.html) which has
a list method to determine uniqueness of (non-atomic/heterogeneous)
elements in a list.

No matrix method is provided. Please use the alternatives provided in
package *kit* with matrices.

## Value

`funique` returns `x` with duplicate elements/rows removed, `fnunique`
returns an integer giving the number of unique values/rows,
`fduplicated` gives a logical vector with `TRUE` indicating duplicated
elements/rows.

## See also

[`fndistinct`](https://fastverse.org/collapse/reference/fndistinct.md),
[`group`](https://fastverse.org/collapse/reference/group.md), [Fast
Grouping and
Ordering](https://fastverse.org/collapse/reference/fast-grouping-ordering.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md).

## Examples

``` r
funique(mtcars$cyl)
#> [1] 6 4 8
funique(gv(mtcars, c(2,8,9)))
#>                   cyl vs am
#> Mazda RX4           6  0  1
#> Datsun 710          4  1  1
#> Hornet 4 Drive      6  1  0
#> Hornet Sportabout   8  0  0
#> Merc 240D           4  1  0
#> Porsche 914-2       4  0  1
#> Ford Pantera L      8  0  1
funique(mtcars, cols = c(2,8,9))
#>                    mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6 160.0 110 3.90 2.620 16.46  0  1    4    4
#> Datsun 710        22.8   4 108.0  93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive    21.4   6 258.0 110 3.08 3.215 19.44  1  0    3    1
#> Hornet Sportabout 18.7   8 360.0 175 3.15 3.440 17.02  0  0    3    2
#> Merc 240D         24.4   4 146.7  62 3.69 3.190 20.00  1  0    4    2
#> Porsche 914-2     26.0   4 120.3  91 4.43 2.140 16.70  0  1    5    2
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
fnunique(gv(mtcars, c(2,8,9)))
#> [1] 7
fduplicated(gv(mtcars, c(2,8,9)))
#>  [1] FALSE  TRUE FALSE FALSE FALSE  TRUE  TRUE FALSE  TRUE  TRUE  TRUE  TRUE
#> [13]  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE
#> [25]  TRUE  TRUE FALSE  TRUE FALSE  TRUE  TRUE  TRUE
fduplicated(gv(mtcars, c(2,8,9)), all = TRUE)
#>  [1]  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE
#> [13]  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE
#> [25]  TRUE  TRUE FALSE  TRUE  TRUE  TRUE  TRUE  TRUE
any_duplicated(gv(mtcars, c(2,8,9)))
#> [1] TRUE
any_duplicated(mtcars)
#> [1] FALSE
```
