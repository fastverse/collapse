# Unlistable Lists

A (nested) list with atomic objects in all final nodes of the list-tree
is unlistable - checked with `is_unlistable`.

## Usage

``` r
is_unlistable(l, DF.as.list = FALSE)
```

## Arguments

- l:

  a list.

- DF.as.list:

  logical. `TRUE` treats data frames like (sub-)lists; `FALSE` like
  atomic elements.

## Details

`is_unlistable` with `DF.as.list = TRUE` is defined as
`all(rapply(l, is.atomic))`, whereas `DF.as.list = FALSE` yields
checking using
`all(unlist(rapply2d(l, function(x) is.atomic(x) || is.list(x)), use.names = FALSE))`,
assuming that data frames are lists composed of atomic elements. If `l`
contains data frames, the latter can be a lot faster than applying
`is.atomic` to every data frame column.

## Value

`logical(1)` - `TRUE` or `FALSE`.

## See also

[`ldepth`](https://fastverse.org/collapse/reference/ldepth.md),
[`has_elem`](https://fastverse.org/collapse/reference/extract_list.md),
[List
Processing](https://fastverse.org/collapse/reference/list-processing.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
l <- list(1, 2, list(3, 4, "b", FALSE))
is_unlistable(l)
#> [1] TRUE
l <- list(1, 2, list(3, 4, "b", FALSE, e ~ b))
is_unlistable(l)
#> [1] FALSE
```
