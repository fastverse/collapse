# Fast Hash-Based Grouping

`group()` scans the rows of a data frame (or atomic vector / list of
atomic vectors), assigning to each unique row an integer id - starting
with 1 and proceeding in first-appearance order of the rows. The
function is written in C and optimized for R's data structures. It is
the workhorse behind functions like
[`GRP`](https://fastverse.org/collapse/reference/GRP.md) /
[`fgroup_by`](https://fastverse.org/collapse/reference/GRP.md),
[`collap`](https://fastverse.org/collapse/reference/collap.md),
[`qF`](https://fastverse.org/collapse/reference/qF.md),
[`qG`](https://fastverse.org/collapse/reference/qF.md),
[`finteraction`](https://fastverse.org/collapse/reference/qF.md) and
[`funique`](https://fastverse.org/collapse/reference/funique.md), when
called with argument `sort = FALSE`.

## Usage

``` r
group(..., starts = FALSE, group.sizes = FALSE)

groupv(x, starts = FALSE, group.sizes = FALSE)
```

## Arguments

- ...:

  comma separated atomic vectors to group. Also supports a single list
  of vectors for backward compatibility.

- x:

  an atomic vector or data frame / list of equal-length atomic vectors.

- starts:

  logical. If `TRUE`, an additional attribute `"starts"` is attached
  giving a vector of group starts (= index of first-occurrence of unique
  rows).

- group.sizes:

  logical. If `TRUE`, an additional attribute `"group.sizes"` is
  attached giving the size of each group.

## Details

A data frame is grouped on a column-by-column basis, starting from the
leftmost column. For each new column the grouping vector obtained after
the previous column is also fed back into the hash function so that
unique values are determined on a running basis. The algorithm
terminates as soon as the number of unique rows reaches the size of the
data frame. Missing values are also grouped just like any other values.
Invoking arguments `starts` and/or `group.sizes` requires an additional
pass through the final grouping vector.

## Value

An object is of class 'qG' see
[`qG`](https://fastverse.org/collapse/reference/qF.md).

## Author

The Hash Function and inspiration was taken from the excellent *kit*
package by Morgan Jacob, the algorithm was developed by Sebastian
Krantz.

## See also

[`radixorder`](https://fastverse.org/collapse/reference/radixorder.md),
[`GRPid`](https://fastverse.org/collapse/reference/GRP.md), [Fast
Grouping and
Ordering](https://fastverse.org/collapse/reference/fast-grouping-ordering.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
# Let's replicate what funique does
g <- groupv(wlddev, starts = TRUE)
if(attr(g, "N.groups") == fnrow(wlddev)) wlddev else
   ss(wlddev, attr(g, "starts"))
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
#>  [ reached 'max' / getOption("max.print") -- omitted 13171 rows ]
```
