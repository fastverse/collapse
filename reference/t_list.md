# Efficient List Transpose

`t_list` turns a list of lists inside-out. The performance is quite
efficient regardless of the size of the list.

## Usage

``` r
t_list(l)
```

## Arguments

- l:

  a list of lists. Elements inside the sublists can be heterogeneous,
  including further lists.

## Value

`l` transposed such that the second layer of the list becomes the top
layer and the top layer the second layer. See Examples.

## Note

To transpose a data frame / list of atomic vectors see
[`data.table::transpose()`](https://rdrr.io/pkg/data.table/man/transpose.html).

## See also

[`rsplit`](https://fastverse.org/collapse/reference/rsplit.md), [List
Processing](https://fastverse.org/collapse/reference/list-processing.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
# Homogenous list of lists
l <- list(a = list(c = 1, d = 2), b = list(c = 3, d = 4))
str(l)
#> List of 2
#>  $ a:List of 2
#>   ..$ c: num 1
#>   ..$ d: num 2
#>  $ b:List of 2
#>   ..$ c: num 3
#>   ..$ d: num 4
str(t_list(l))
#> List of 2
#>  $ c:List of 2
#>   ..$ a: num 1
#>   ..$ b: num 3
#>  $ d:List of 2
#>   ..$ a: num 2
#>   ..$ b: num 4

# Heterogenous case
l2 <- list(a = list(c = 1, d = letters), b = list(c = 3:10, d = list(4, e = 5)))
attr(l2, "bla") <- "abc"  # Attributes other than names are preserved
str(l2)
#> List of 2
#>  $ a:List of 2
#>   ..$ c: num 1
#>   ..$ d: chr [1:26] "a" "b" "c" "d" ...
#>  $ b:List of 2
#>   ..$ c: int [1:8] 3 4 5 6 7 8 9 10
#>   ..$ d:List of 2
#>   .. ..$  : num 4
#>   .. ..$ e: num 5
#>  - attr(*, "bla")= chr "abc"
str(t_list(l2))
#> List of 2
#>  $ c:List of 2
#>   ..$ a: num 1
#>   ..$ b: int [1:8] 3 4 5 6 7 8 9 10
#>  $ d:List of 2
#>   ..$ a: chr [1:26] "a" "b" "c" "d" ...
#>   ..$ b:List of 2
#>   .. ..$  : num 4
#>   .. ..$ e: num 5
#>  - attr(*, "bla")= chr "abc"

rm(l, l2)
```
