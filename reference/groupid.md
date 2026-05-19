# Generate Run-Length Type Group-Id

`groupid` is an enhanced version of
[`data.table::rleid`](https://rdrr.io/pkg/data.table/man/rleid.html) for
atomic vectors. It generates a run-length type group-id where
consecutive identical values are assigned the same integer. It is a
generalization as it can be applied to unordered vectors, generate group
id's starting from an arbitrary value, and skip missing values.

## Usage

``` r
groupid(x, o = NULL, start = 1L, na.skip = FALSE, check.o = TRUE)
```

## Arguments

- x:

  an atomic vector of any type. Attributes are not considered.

- o:

  an (optional) integer ordering vector specifying the order by which to
  pass through `x`.

- start:

  integer. The starting value of the resulting group-id. Default is
  starting from 1.

&nbsp;

- na.skip:

  logical. Skip missing values i.e. if `TRUE` something like
  `groupid(c("a", NA, "a"))` gives `c(1, NA, 1)` whereas `FALSE` gives
  `c(1, 2, 3)`.

- check.o:

  logical. Programmers option: `FALSE` prevents checking that each
  element of `o` is in the range `[1, length(x)]`, it only checks the
  length of `o`. This gives some extra speed, but will terminate R if
  any element of `o` is too large or too small.

## Value

An integer vector of class 'qG'. See
[`qG`](https://fastverse.org/collapse/reference/qF.md).

## See also

[`seqid`](https://fastverse.org/collapse/reference/seqid.md),
[`timeid`](https://fastverse.org/collapse/reference/timeid.md),
[`qG`](https://fastverse.org/collapse/reference/qF.md), [Fast Grouping
and
Ordering](https://fastverse.org/collapse/reference/fast-grouping-ordering.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
groupid(airquality$Month)
#>  [1] 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2
#> [39] 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 3
#>  [ reached 'max' / getOption("max.print") -- omitted 83 entries ]
#> attr(,"N.groups")
#> [1] 5
#> attr(,"class")
#> [1] "qG"          "na.included"
groupid(airquality$Month, start = 0)
#>  [1] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1
#> [39] 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2
#>  [ reached 'max' / getOption("max.print") -- omitted 83 entries ]
#> attr(,"N.groups")
#> [1] 5
groupid(wlddev$country)[1:100]
#>  [1] 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
#> [39] 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2
#>  [ reached 'max' / getOption("max.print") -- omitted 30 entries ]

## Same thing since country is alphabetically ordered: (groupid is faster..)
all.equal(groupid(wlddev$country), qG(wlddev$country, na.exclude = FALSE))
#> [1] TRUE

## When data is unordered, group-id can be generated through an ordering..
uo <- order(rnorm(fnrow(airquality)))
monthuo <- airquality$Month[uo]
o <- order(monthuo)
groupid(monthuo, o)
#>  [1] 1 3 4 5 2 1 1 4 5 5 3 2 2 2 4 5 1 2 4 5 2 1 3 3 3 2 4 1 5 1 1 2 2 4 1 2 1 1
#> [39] 1 4 5 1 2 5 1 4 2 5 1 2 4 4 3 3 5 3 4 5 4 4 2 3 5 3 5 3 5 5 3 2
#>  [ reached 'max' / getOption("max.print") -- omitted 83 entries ]
#> attr(,"N.groups")
#> [1] 5
#> attr(,"class")
#> [1] "qG"          "na.included"
identical(groupid(monthuo, o)[o], unattrib(groupid(airquality$Month)))
#> [1] TRUE
```
