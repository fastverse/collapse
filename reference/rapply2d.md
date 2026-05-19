# Recursively Apply a Function to a List of Data Objects

`rapply2d` is a recursive version of `lapply` with three differences to
[`rapply`](https://rdrr.io/r/base/rapply.html):

1.  data frames (or other list-based objects specified in `classes`) are
    considered as atomic, not as (sub-)lists

2.  `FUN` is applied to all 'atomic' objects in the nested list

3.  the result is not simplified / unlisted.

## Usage

``` r
rapply2d(l, FUN, ..., classes = "data.frame")
```

## Arguments

- l:

  a list.

- FUN:

  a function that can be applied to all 'atomic' elements in l.

- ...:

  additional elements passed to FUN.

- classes:

  character. Classes of list-based objects inside `l` that should be
  considered as atomic.

## Value

A list of the same structure as `l`, where `FUN` was applied to all
atomic elements and list-based objects of a class included in `classes`.

## Note

The main reason `rapply2d` exists is to have a recursive function that
out-of-the-box applies a function to a nested list of data frames.

For most other purposes [`rapply`](https://rdrr.io/r/base/rapply.html),
or by extension the excellent
[rrapply](https://cran.r-project.org/package=rrapply) function /
package, provide more advanced functionality and greater performance.

## See also

[`rsplit`](https://fastverse.org/collapse/reference/rsplit.md),
[`unlist2d`](https://fastverse.org/collapse/reference/unlist2d.md),
[List
Processing](https://fastverse.org/collapse/reference/list-processing.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
l <- list(mtcars, list(mtcars, as.matrix(mtcars)))
rapply2d(l, fmean)
#> [[1]]
#>        mpg        cyl       disp         hp       drat         wt       qsec 
#>  20.090625   6.187500 230.721875 146.687500   3.596563   3.217250  17.848750 
#>         vs         am       gear       carb 
#>   0.437500   0.406250   3.687500   2.812500 
#> 
#> [[2]]
#> [[2]][[1]]
#>        mpg        cyl       disp         hp       drat         wt       qsec 
#>  20.090625   6.187500 230.721875 146.687500   3.596563   3.217250  17.848750 
#>         vs         am       gear       carb 
#>   0.437500   0.406250   3.687500   2.812500 
#> 
#> [[2]][[2]]
#>        mpg        cyl       disp         hp       drat         wt       qsec 
#>  20.090625   6.187500 230.721875 146.687500   3.596563   3.217250  17.848750 
#>         vs         am       gear       carb 
#>   0.437500   0.406250   3.687500   2.812500 
#> 
#> 
unlist2d(rapply2d(l, fmean))
#>   .id.1 .id.2      mpg    cyl     disp       hp     drat      wt     qsec
#> 1     1    NA 20.09063 6.1875 230.7219 146.6875 3.596563 3.21725 17.84875
#> 2     2     1 20.09063 6.1875 230.7219 146.6875 3.596563 3.21725 17.84875
#> 3     2     2 20.09063 6.1875 230.7219 146.6875 3.596563 3.21725 17.84875
#>       vs      am   gear   carb
#> 1 0.4375 0.40625 3.6875 2.8125
#> 2 0.4375 0.40625 3.6875 2.8125
#> 3 0.4375 0.40625 3.6875 2.8125
```
