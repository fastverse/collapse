# Fast Removal of Unused Factor Levels

A substantially faster replacement for
[`droplevels`](https://rdrr.io/r/base/droplevels.html).

## Usage

``` r
fdroplevels(x, ...)

# S3 method for class 'factor'
fdroplevels(x, ...)

# S3 method for class 'data.frame'
fdroplevels(x, ...)
```

## Arguments

- x:

  a factor, or data frame / list containing one or more factors.

- ...:

  not used.

## Details

[`droplevels`](https://rdrr.io/r/base/droplevels.html) passes a factor
from which levels are to be dropped to
[`factor`](https://rdrr.io/r/base/factor.html), which first calls
[`unique`](https://rdrr.io/r/base/unique.html) and then
[`match`](https://rdrr.io/r/base/match.html) to drop unused levels. Both
functions internally use a hash table, which is highly inefficient.
`fdroplevels` does not require mapping values at all, but uses a super
fast boolean vector method to determine which levels are unused and
remove those levels. In addition, if no unused levels are found, `x` is
simply returned. Any missing values found in `x` are efficiently skipped
in the process of checking and replacing levels. All other attributes of
`x` are preserved.

## Value

`x` with unused factor levels removed.

## Note

If `x` is malformed e.g. has too few levels, this function can cause a
segmentation fault terminating the R session, thus only use with
ordinary / proper factors.

## See also

[`qF`](https://fastverse.org/collapse/reference/qF.md),
[`funique`](https://fastverse.org/collapse/reference/funique.md), [Fast
Grouping and
Ordering](https://fastverse.org/collapse/reference/fast-grouping-ordering.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
f <- iris$Species[1:100]
fdroplevels(f)
#>  [1] setosa     setosa     setosa     setosa     setosa     setosa    
#>  [7] setosa     setosa     setosa     setosa     setosa     setosa    
#> [13] setosa     setosa     setosa     setosa     setosa     setosa    
#> [19] setosa     setosa     setosa     setosa     setosa     setosa    
#> [25] setosa     setosa     setosa     setosa     setosa     setosa    
#> [31] setosa     setosa     setosa     setosa     setosa     setosa    
#> [37] setosa     setosa     setosa     setosa     setosa     setosa    
#> [43] setosa     setosa     setosa     setosa     setosa     setosa    
#> [49] setosa     setosa     versicolor versicolor versicolor versicolor
#> [55] versicolor versicolor versicolor versicolor versicolor versicolor
#> [61] versicolor versicolor versicolor versicolor versicolor versicolor
#> [67] versicolor versicolor versicolor versicolor
#>  [ reached 'max' / getOption("max.print") -- omitted 30 entries ]
#> Levels: setosa versicolor
identical(fdroplevels(f), droplevels(f))
#> [1] TRUE

fNA <- na_insert(f)
fdroplevels(fNA)
#>  [1] setosa     setosa     setosa     setosa     setosa     setosa    
#>  [7] <NA>       setosa     setosa     <NA>       <NA>       setosa    
#> [13] setosa     setosa     setosa     setosa     setosa     setosa    
#> [19] setosa     setosa     setosa     setosa     setosa     setosa    
#> [25] setosa     <NA>       setosa     setosa     setosa     setosa    
#> [31] setosa     setosa     setosa     <NA>       setosa     setosa    
#> [37] setosa     setosa     setosa     setosa     setosa     setosa    
#> [43] setosa     setosa     setosa     setosa     setosa     setosa    
#> [49] setosa     <NA>       versicolor versicolor versicolor versicolor
#> [55] versicolor versicolor versicolor <NA>       versicolor versicolor
#> [61] versicolor versicolor versicolor versicolor versicolor versicolor
#> [67] <NA>       versicolor versicolor versicolor
#>  [ reached 'max' / getOption("max.print") -- omitted 30 entries ]
#> Levels: setosa versicolor
identical(fdroplevels(fNA), droplevels(fNA))
#> [1] TRUE

identical(fdroplevels(ss(iris, 1:100)), droplevels(ss(iris, 1:100)))
#> [1] TRUE
```
