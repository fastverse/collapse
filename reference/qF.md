# Fast Factor Generation, Interactions and Vector Grouping

`qF`, shorthand for 'quick-factor' implements very fast factor
generation from atomic vectors using either radix ordering or index
hashing followed by sorting.

`qG`, shorthand for 'quick-group', generates a kind of factor-light
without the levels attribute but instead an attribute providing the
number of levels. Optionally the levels / groups can be attached, but
without converting them to character (which can have large performance
implications). Objects have a class 'qG'.

`finteraction` generates a factor or 'qG' object by interacting multiple
vectors or factors. In that process missing values are always replaced
with a level and unused levels/combinations are always dropped.

*collapse* internally makes optimal use of factors and 'qG' objects when
passed as grouping vectors to statistical functions (`g/by`, or `t`
arguments) i.e. typically no further grouping or ordering is performed
and objects are used directly by statistical C/C++ code.

## Usage

``` r
qF(x, ordered = FALSE, na.exclude = TRUE, sort = .op[["sort"]], drop = FALSE,
   keep.attr = TRUE, method = "auto")

qG(x, ordered = FALSE, na.exclude = TRUE, sort = .op[["sort"]],
   return.groups = FALSE, method = "auto")

is_qG(x)

as_factor_qG(x, ordered = FALSE, na.exclude = TRUE)

finteraction(..., factor = TRUE, ordered = FALSE, sort = factor && .op[["sort"]],
             method = "auto", sep = ".")
itn(...) # Shorthand for finteraction
```

## Arguments

- x:

  a atomic vector, factor or quick-group.

- ordered:

  logical. Adds a class 'ordered'.

- na.exclude:

  logical. `TRUE` preserves missing values (i.e. no level is generated
  for `NA`). `FALSE` attaches an additional class `"na.included"` which
  is used to skip missing value checks performed before sending objects
  to C/C++. See Details.

- sort:

  logical. `TRUE` sorts the levels in ascending order (like
  [`factor`](https://rdrr.io/pkg/data.table/man/fctr.html)); `FALSE`
  provides the levels in order of first appearance, which can be
  significantly faster. Note that if a factor is passed as input, only
  `sort = FALSE` takes effect and unused levels will be dropped (as
  factors usually have sorted levels and checking sortedness can be
  expensive).

- drop:

  logical. If `x` is a factor, `TRUE` efficiently drops unused factor
  levels beforehand using
  [`fdroplevels`](https://fastverse.org/collapse/reference/fdroplevels.md).

- keep.attr:

  logical. If `TRUE` and `x` has additional attributes apart from
  'levels' and 'class', these are preserved in the conversion to factor.

- method:

  an integer or character string specifying the method of computation:

  |  |  |  |  |  |
  |----|----|----|----|----|
  | *Int.* |  | *String* |  | *Description* |
  | 1 |  | "auto" |  | automatic selection: `if(is.double(x) && sort) "radix" else if(sort && length(x) < 1e5) "rcpp_hash" else "hash"`. |
  | 2 |  | "radix" |  | use radix ordering to generate factors. Supports `sort = FALSE` only for character vectors. See Details. |
  | 3 |  | "hash" |  | use hashing to generate factors. Since v1.8.3 this is a fast hybrid implementation using [`group`](https://fastverse.org/collapse/reference/group.md) and radix ordering applied to the unique elements. See Details. |
  | 4 |  | "rcpp_hash" |  | the previous "hash" algorithm prior to v1.8.3: uses `Rcpp::sugar::sort_unique` and `Rcpp::sugar::match`. Only supports `sort = TRUE`. |

  Note that for `finteraction`, `method = "hash"` is always unsorted and
  `method = "rcpp_hash"` is not available.

- return.groups:

  logical. `TRUE` returns the unique elements / groups / levels of `x`
  in an attribute called `"groups"`. Unlike `qF`, they are not converted
  to character.

- factor:

  logical. `TRUE` returns an factor, `FALSE` returns a 'qG' object.

- sep:

  character. The separator passed to
  [`paste`](https://rdrr.io/r/base/paste.html) when creating factor
  levels from multiple grouping variables.

- ...:

  multiple atomic vectors or factors, or a single list of equal-length
  vectors or factors. See Details.

## Details

Whenever a vector is passed to a [Fast Statistical
Function](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
such as `fmean(mtcars, mtcars$cyl)`, is is grouped using `qF`, or `qG`
if `use.g.names = FALSE`.

`qF` is a combination of `as.factor` and `factor`. Applying it to a
vector i.e. `qF(x)` gives the same result as `as.factor(x)`.
`qF(x, ordered = TRUE)` generates an ordered factor (same as
`factor(x, ordered = TRUE)`), and `qF(x, na.exclude = FALSE)` generates
a level for missing values (same as `factor(x, exclude = NULL)`). An
important addition is that `qF(x, na.exclude = FALSE)` also adds a class
'na.included'. This prevents *collapse* functions from checking missing
values in the factor, and is thus computationally more efficient.
Therefore factors used in grouped operations should preferably be
generated using `qF(x, na.exclude = FALSE)`. Setting `sort = FALSE`
gathers the levels in first-appearance order (unless `method = "radix"`
and `x` is numeric, in which case the levels are always sorted). This
often gives a noticeable speed improvement.

There are 3 internal methods of computation: radix ordering, hashing,
and Rcpp sugar hashing. Radix ordering is done by combining the
functions
[`radixorder`](https://fastverse.org/collapse/reference/radixorder.md)
and [`groupid`](https://fastverse.org/collapse/reference/groupid.md). It
is generally faster than hashing for large numeric data and pre-sorted
data (although there are exceptions). Hashing uses
[`group`](https://fastverse.org/collapse/reference/group.md), followed
by
[`radixorder`](https://fastverse.org/collapse/reference/radixorder.md)
on the unique elements if `sort = TRUE`. It is generally fastest for
character data. Rcpp hashing uses `Rcpp::sugar::sort_unique` and
`Rcpp::sugar::match`. This is often less efficient than the former on
large data, but the sorting properties (relying on `std::sort`) may be
superior in borderline cases where
[`radixorder`](https://fastverse.org/collapse/reference/radixorder.md)
fails to deliver exact lexicographic ordering of factor levels.

Regarding speed: In general `qF` is around 5x faster than `as.factor` on
character data and about 30x faster on numeric data. Automatic method
dispatch typically does a good job delivering optimal performance.

`qG` is in the first place a programmers function. It generates a
factor-'light' class 'qG' consisting of only an integer grouping vector
and an attribute providing the number of groups. It is slightly faster
and more memory efficient than
[`GRP`](https://fastverse.org/collapse/reference/GRP.md) for grouping
atomic vectors, and also convenient as it can be stored in a data frame
column, which are the main reasons for its existence.

`finteraction` is simply a wrapper around
`as_factor_GRP(GRP.default(X))`, where X is replaced by the arguments in
'...' combined in a list (so its not really an interaction function but
just a multivariate grouping converted to factor, see
[`GRP`](https://fastverse.org/collapse/reference/GRP.md) for
computational details). In general: All vectors, factors, or lists of
vectors / factors passed can be interacted. Interactions always create a
level for missing values and always drop unused levels.

## Value

`qF` returns an (ordered) factor. `qG` returns an object of class 'qG':
an integer grouping vector with an attribute `"N.groups"` indicating the
number of groups, and, if `return.groups = TRUE`, an attribute
`"groups"` containing the vector of unique groups / elements in `x`
corresponding to the integer-id. `finteraction` can return either.

## Note

An efficient alternative for character vectors with multithreading
support is provided by
[`kit::charToFact`](https://fastverse.org/kit/reference/charToFact.html).

`qG(x, sort = FALSE, na.exclude = FALSE, method = "hash")` internally
calls [`group(x)`](https://fastverse.org/collapse/reference/group.md)
which can also be used directly and also supports multivariate
groupings.

Neither `qF` nor `qG` reorder groups / factor levels. An exception was
added in v1.7, when calling `qF(f, sort = FALSE)` on a factor `f`, the
levels are recast in first appearance order. These objects can however
be converted into one another using `qF/qG` or the direct method
`as_factor_qG` (called inside `qF`). It is also possible to add a class
'ordered' (`ordered = TRUE`) and to create am extra level / integer for
missing values (`na.exclude = FALSE`) if factors or 'qG' objects are
passed to `qF` or `qG`.

## See also

[`group`](https://fastverse.org/collapse/reference/group.md),
[`groupid`](https://fastverse.org/collapse/reference/groupid.md),
[`GRP`](https://fastverse.org/collapse/reference/GRP.md), [Fast Grouping
and
Ordering](https://fastverse.org/collapse/reference/fast-grouping-ordering.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
cylF <- qF(mtcars$cyl)     # Factor from atomic vector
cylG <- qG(mtcars$cyl)     # Quick-group from atomic vector
cylG                       # See the simple structure of this object
#>  [1] 2 2 1 2 3 2 3 1 1 2 2 3 3 3 3 3 3 1 1 1 1 3 3 3 3 1 1 1 3 2 3 1
#> attr(,"N.groups")
#> [1] 3
#> attr(,"class")
#> [1] "qG"

cf  <- qF(wlddev$country)  # Bigger data
cf2 <- qF(wlddev$country, na.exclude = FALSE)  # With na.included class
dat <- num_vars(wlddev)
 
# cf2 is faster in grouped operations because no missing value check is performed
library(microbenchmark)
microbenchmark(fmax(dat, cf), fmax(dat, cf2))
#> Unit: microseconds
#>            expr    min      lq      mean median       uq     max neval
#>   fmax(dat, cf) 97.170 98.2155 101.98258 99.712 101.9465 137.309   100
#>  fmax(dat, cf2) 90.528 92.2090  96.41847 93.849  96.2680 152.397   100

finteraction(mtcars$cyl, mtcars$vs)  # Interacting two variables (can be factors)
#>  [1] 6.0 6.0 4.1 6.1 8.0 6.1 8.0 4.1 4.1 6.1 6.1 8.0 8.0 8.0 8.0 8.0 8.0 4.1 4.1
#> [20] 4.1 4.1 8.0 8.0 8.0 8.0 4.1 4.0 4.1 8.0 6.0 8.0 4.1
#> Levels: 4.0 4.1 6.0 6.1 8.0
head(finteraction(mtcars))           # A more crude example..
#> [1] 21.6.160.110.3.9.2.62.16.46.0.1.4.4    
#> [2] 21.6.160.110.3.9.2.875.17.02.0.1.4.4   
#> [3] 22.8.4.108.93.3.85.2.32.18.61.1.1.4.1  
#> [4] 21.4.6.258.110.3.08.3.215.19.44.1.0.3.1
#> [5] 18.7.8.360.175.3.15.3.44.17.02.0.0.3.2 
#> [6] 18.1.6.225.105.2.76.3.46.20.22.1.0.3.1 
#> 32 Levels: 10.4.8.460.215.3.5.424.17.82.0.0.3.4 ...

finteraction(mtcars$cyl, mtcars$vs, factor = FALSE) # Returns 'qG', by default unsorted
#>  [1] 1 1 2 3 4 3 4 2 2 3 3 4 4 4 4 4 4 2 2 2 2 4 4 4 4 2 5 2 4 1 4 2
#> attr(,"N.groups")
#> [1] 5
#> attr(,"class")
#> [1] "qG"          "na.included"
group(mtcars$cyl, mtcars$vs) # Same thing
#>  [1] 1 1 2 3 4 3 4 2 2 3 3 4 4 4 4 4 4 2 2 2 2 4 4 4 4 2 5 2 4 1 4 2
#> attr(,"N.groups")
#> [1] 5
#> attr(,"class")
#> [1] "qG"          "na.included"
```
