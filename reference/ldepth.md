# Determine the Depth / Level of Nesting of a List

`ldepth` provides the depth of a list or list-like structure.

## Usage

``` r
ldepth(l, DF.as.list = FALSE)
```

## Arguments

- l:

  a list.

- DF.as.list:

  logical. `TRUE` treats data frames like (sub-)lists; `FALSE` like
  atomic elements.

## Details

The depth or level or nesting of a list or list-like structure (e.g. a
model object) is found by recursing down to the bottom of the list and
adding an integer count of 1 for each level passed. For example the
depth of a data frame is 1. If a data frame has list-columns, the depth
is 2. However for reasons of efficiency, if `l` is not a data frame and
`DF.as.list = FALSE`, data frames found inside `l` will not be checked
for list column's but assumed to have a depth of 1.

## Value

A single integer indicating the depth of the list.

## See also

[`is_unlistable`](https://fastverse.org/collapse/reference/is_unlistable.md),
[`has_elem`](https://fastverse.org/collapse/reference/extract_list.md),
[List
Processing](https://fastverse.org/collapse/reference/list-processing.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
l <- list(1, 2)
ldepth(l)
#> [1] 1
l <- list(1, 2, mtcars)
ldepth(l)
#> [1] 1
ldepth(l, DF.as.list = FALSE)
#> [1] 1
l <- list(1, 2, list(4, 5, list(6, mtcars)))
ldepth(l)
#> [1] 3
ldepth(l, DF.as.list = FALSE)
#> [1] 3
```
