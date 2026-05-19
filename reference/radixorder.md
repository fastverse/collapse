# Fast Radix-Based Ordering

A slight modification of
[`order(..., method = "radix")`](https://rdrr.io/pkg/data.table/man/setorder.html)
that is more programmer friendly and, importantly, provides features for
ordered grouping of data (similar to `data.table:::forderv` from which
it descended).

## Usage

``` r
radixorder(..., na.last = TRUE, decreasing = FALSE, starts = FALSE,
           group.sizes = FALSE, sort = TRUE)

radixorderv(x, na.last = TRUE, decreasing = FALSE, starts = FALSE,
            group.sizes = FALSE, sort = TRUE)
```

## Arguments

- ...:

  comma-separated atomic vectors to order.

- x:

  an atomic vector or list of atomic vectors such as a data frame.

- na.last:

  logical. for controlling the treatment of `NA`'s. If `TRUE`, missing
  values in the data are put last; if `FALSE`, they are put first; if
  NA, they are removed.

- decreasing:

  logical. Should the sort order be increasing or decreasing? Can be a
  vector of length equal to the number of arguments in `...` / `x`.

- starts:

  logical. `TRUE` returns an attribute 'starts' containing the first
  element of each new group i.e. the row denoting the start of each new
  group if the data were sorted using the computed ordering vector. See
  Examples.

- group.sizes:

  logical. `TRUE` returns an attribute 'group.sizes' containing sizes of
  each group in the same order as groups are encountered if the data
  were sorted using the computed ordering vector. See Examples.

- sort:

  logical. This argument only affects character vectors / columns
  passed. If `FALSE`, these are not ordered but simply grouped in the
  order of first appearance of unique elements. This provides a slight
  performance gain if only grouping but not alphabetic ordering is
  required. See also
  [`group`](https://fastverse.org/collapse/reference/group.md).

## Value

An integer ordering vector with attributes: Unless `na.last = NA` an
attribute `"sorted"` indicating whether the input data was already
sorted is attached. If `starts = TRUE`, `"starts"` giving a vector of
group starts in the ordered data, and if `group.sizes = TRUE`,
`"group.sizes"` giving the vector of group sizes are attached. In either
case an attribute `"maxgrpn"` providing the size of the largest group is
also attached.

## Author

The C code was taken - with slight modifications - from [base R source
code](https://github.com/wch/r-source/blob/79298c499218846d14500255efd622b5021c10ec/src/main/radixsort.c),
and is originally due to *data.table* authors Matt Dowle and Arun
Srinivasan.

## See also

[Fast Grouping and
Ordering](https://fastverse.org/collapse/reference/fast-grouping-ordering.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
radixorder(mtcars$mpg)
#>  [1] 15 16 24  7 17 31 14 23 22 29 12 13 11  6  5 10 25 30  1  2  4 32 21  3  9
#> [26]  8 27 26 19 28 18 20
#> attr(,"sorted")
#> [1] FALSE
head(mtcars[radixorder(mtcars$mpg), ])
#>                      mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Cadillac Fleetwood  10.4   8  472 205 2.93 5.250 17.98  0  0    3    4
#> Lincoln Continental 10.4   8  460 215 3.00 5.424 17.82  0  0    3    4
#> Camaro Z28          13.3   8  350 245 3.73 3.840 15.41  0  0    3    4
#> Duster 360          14.3   8  360 245 3.21 3.570 15.84  0  0    3    4
#> Chrysler Imperial   14.7   8  440 230 3.23 5.345 17.42  0  0    3    4
#> Maserati Bora       15.0   8  301 335 3.54 3.570 14.60  0  1    5    8
radixorder(mtcars$cyl, mtcars$vs)
#>  [1] 27  3  8  9 18 19 20 21 26 28 32  1  2 30  4  6 10 11  5  7 12 13 14 15 16
#> [26] 17 22 23 24 25 29 31
#> attr(,"sorted")
#> [1] FALSE

o <- radixorder(mtcars$cyl, mtcars$vs, starts = TRUE)
st <- attr(o, "starts")
head(mtcars[o, ])
#>                mpg cyl  disp hp drat    wt  qsec vs am gear carb
#> Porsche 914-2 26.0   4 120.3 91 4.43 2.140 16.70  0  1    5    2
#> Datsun 710    22.8   4 108.0 93 3.85 2.320 18.61  1  1    4    1
#> Merc 240D     24.4   4 146.7 62 3.69 3.190 20.00  1  0    4    2
#> Merc 230      22.8   4 140.8 95 3.92 3.150 22.90  1  0    4    2
#> Fiat 128      32.4   4  78.7 66 4.08 2.200 19.47  1  1    4    1
#> Honda Civic   30.4   4  75.7 52 4.93 1.615 18.52  1  1    4    2
mtcars[o[st], c("cyl", "vs")]  # Unique groups
#>                   cyl vs
#> Porsche 914-2       4  0
#> Datsun 710          4  1
#> Mazda RX4           6  0
#> Hornet 4 Drive      6  1
#> Hornet Sportabout   8  0

# Note that if attr(o, "sorted") == TRUE, then all(o[st] == st)
radixorder(rep(1:3, each = 3), starts = TRUE)
#> [1] 1 2 3 4 5 6 7 8 9
#> attr(,"starts")
#> [1] 1 4 7
#> attr(,"maxgrpn")
#> [1] 3
#> attr(,"sorted")
#> [1] TRUE

# Group sizes
radixorder(mtcars$cyl, mtcars$vs, group.sizes = TRUE)
#>  [1] 27  3  8  9 18 19 20 21 26 28 32  1  2 30  4  6 10 11  5  7 12 13 14 15 16
#> [26] 17 22 23 24 25 29 31
#> attr(,"group.sizes")
#> [1]  1 10  3  4 14
#> attr(,"maxgrpn")
#> [1] 14
#> attr(,"sorted")
#> [1] FALSE

# Both
radixorder(mtcars$cyl, mtcars$vs, starts = TRUE, group.sizes = TRUE)
#>  [1] 27  3  8  9 18 19 20 21 26 28 32  1  2 30  4  6 10 11  5  7 12 13 14 15 16
#> [26] 17 22 23 24 25 29 31
#> attr(,"starts")
#> [1]  1  2 12 15 19
#> attr(,"group.sizes")
#> [1]  1 10  3  4 14
#> attr(,"maxgrpn")
#> [1] 14
#> attr(,"sorted")
#> [1] FALSE
```
