# Split-Apply-Combine Computing

`BY` is an S3 generic that efficiently applies functions over vectors or
matrix- and data frame columns by groups. Similar to
[`dapply`](https://fastverse.org/collapse/reference/dapply.md) it seeks
to retain the structure and attributes of the data, but can also output
to various standard formats. A simple parallelism is also available.

## Usage

``` r
BY(x, ...)

# Default S3 method
BY(x, g, FUN, ..., use.g.names = TRUE, sort = .op[["sort"]], reorder = TRUE,
   expand.wide = FALSE, parallel = FALSE, mc.cores = 1L,
   return = c("same", "vector", "list"))

# S3 method for class 'matrix'
BY(x, g, FUN, ..., use.g.names = TRUE, sort = .op[["sort"]], reorder = TRUE,
   expand.wide = FALSE, parallel = FALSE, mc.cores = 1L,
   return = c("same", "matrix", "data.frame", "list"))

# S3 method for class 'data.frame'
BY(x, g, FUN, ..., use.g.names = TRUE, sort = .op[["sort"]], reorder = TRUE,
   expand.wide = FALSE, parallel = FALSE, mc.cores = 1L,
   return = c("same", "matrix", "data.frame", "list"))

# S3 method for class 'grouped_df'
BY(x, FUN, ..., reorder = TRUE, keep.group_vars = TRUE, use.g.names = FALSE)
```

## Arguments

- x:

  a vector, matrix, data frame or alike object.

- g:

  a [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object, or
  a factor / atomic vector / list of atomic vectors (internally
  converted to a
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object) used
  to group `x`.

- FUN:

  a function, can be scalar- or vector-valued. For vector valued
  functions see also `reorder` and `expand.wide`.

- ...:

  further arguments to `FUN`, or to `BY.data.frame` for the 'grouped_df'
  method. Since v1.9.0 data length arguments are also split by groups.

- use.g.names:

  logical. Make group-names and add to the result as names (default
  method) or row-names (matrix and data frame methods). For
  vector-valued functions (row-)names are only generated if the function
  itself creates names for the statistics e.g.
  [`quantile()`](https://rdrr.io/r/stats/quantile.html) adds names,
  [`range()`](https://rdrr.io/r/base/range.html) or
  [`log()`](https://rdrr.io/r/base/Log.html) don't. No row-names are
  generated on *data.table*'s.

- sort:

  logical. Sort the groups? Internally passed to
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md), and only
  effective if `g` is not already a factor or
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object.

- reorder:

  logical. If a vector-valued function is passed that preserves the data
  length, `TRUE` will reorder the result such that the elements/rows
  match the original data. `FALSE` just combines the data in order of
  the groups (i.e. all elements of the first group in first-appearance
  order followed by all elements in the second group etc..). *Note* that
  if `reorder = FALSE`, grouping variables, names or rownames are only
  retained if the grouping is on sorted data, see
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md).

- expand.wide:

  logical. If `FUN` is a vector-valued function returning a vector of
  fixed length \> 1 (such as the
  [`quantile`](https://rdrr.io/r/stats/quantile.html) function),
  `expand.wide` can be used to return the result in a wider format
  (instead of stacking the resulting vectors of fixed length above each
  other in each output column).

- parallel:

  logical. `TRUE` implements simple parallel execution by internally
  calling `mclapply` instead of
  [`lapply`](https://rdrr.io/r/base/lapply.html). Parallelism is across
  columns, except for the default method.

- mc.cores:

  integer. Argument to `mclapply` indicating the number of cores to use
  for parallel execution. Can use `detectCores()` to select all
  available cores.

- return:

  an integer or string indicating the type of object to return. The
  default `1 - "same"` returns the same object type (i.e. class and
  other attributes are retained if the underlying data type is the same,
  just the names for the dimensions are adjusted). `2 - "matrix"` always
  returns the output as matrix, `3 - "data.frame"` always returns a data
  frame and `4 - "list"` returns the raw (uncombined) output. *Note*:
  `4 - "list"` works together with `expand.wide` to return a list of
  matrices.

- keep.group_vars:

  *grouped_df method:* Logical. `FALSE` removes grouping variables after
  computation. See also the Note.

## Details

`BY` is a re-implementation of the Split-Apply-Combine computing
paradigm. It is faster than
[`tapply`](https://rdrr.io/r/base/tapply.html),
[`by`](https://rdrr.io/r/base/by.html),
[`aggregate`](https://rdrr.io/r/stats/aggregate.html) and *(d)plyr*, and
preserves data attributes just like
[`dapply`](https://fastverse.org/collapse/reference/dapply.md).

It is principally a wrapper around `lapply(gsplit(x, g), FUN, ...)`,
that uses [`gsplit`](https://fastverse.org/collapse/reference/GRP.md)
for optimized splitting and also strongly optimizes on the internal code
compared to *base* R functions. For more details look at the
documentation for
[`dapply`](https://fastverse.org/collapse/reference/dapply.md) which
works very similar (apart from the splitting performed in `BY`). The
function is intended for simple cases involving flexible computation of
statistics across groups using a single function e.g.
`iris |> gby(Species) |> BY(IQR)` is simpler than
`iris |> gby(Species) |> smr(acr(.fns = IQR))` etc..

## Value

`X` where `FUN` was applied to every column split by `g`.

## See also

[`dapply`](https://fastverse.org/collapse/reference/dapply.md),
[`collap`](https://fastverse.org/collapse/reference/collap.md), [Fast
Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
v <- iris$Sepal.Length   # A numeric vector
g <- GRP(iris$Species)   # A grouping

## default vector method
BY(v, g, sum)                                # Sum by species
#>     setosa versicolor  virginica 
#>      250.3      296.8      329.4 
head(BY(v, g, scale))                        # Scale by species (please use fscale instead)
#> [1]  0.26667447 -0.30071802 -0.86811050 -1.15180675 -0.01702177  1.11776320
BY(v, g, fquantile)                          # Species quantiles: by default stacked
#>       setosa.0%      setosa.25%      setosa.50%      setosa.75%     setosa.100% 
#>           4.300           4.800           5.000           5.200           5.800 
#>   versicolor.0%  versicolor.25%  versicolor.50%  versicolor.75% versicolor.100% 
#>           4.900           5.600           5.900           6.300           7.000 
#>    virginica.0%   virginica.25%   virginica.50%   virginica.75%  virginica.100% 
#>           4.900           6.225           6.500           6.900           7.900 
BY(v, g, fquantile, expand.wide = TRUE)      # Wide format
#>             0%   25% 50% 75% 100%
#> setosa     4.3 4.800 5.0 5.2  5.8
#> versicolor 4.9 5.600 5.9 6.3  7.0
#> virginica  4.9 6.225 6.5 6.9  7.9

## matrix method
m <- qM(num_vars(iris))
BY(m, g, sum)                          # Also return as matrix
#>            Sepal.Length Sepal.Width Petal.Length Petal.Width
#> setosa            250.3       171.4         73.1        12.3
#> versicolor        296.8       138.5        213.0        66.3
#> virginica         329.4       148.7        277.6       101.3
BY(m, g, sum, return = "data.frame")   # Return as data.frame.. also works for computations below
#>            Sepal.Length Sepal.Width Petal.Length Petal.Width
#> setosa            250.3       171.4         73.1        12.3
#> versicolor        296.8       138.5        213.0        66.3
#> virginica         329.4       148.7        277.6       101.3
head(BY(m, g, scale))
#>      Sepal.Length Sepal.Width Petal.Length Petal.Width
#> [1,]   0.26667447   0.1899414   -0.3570112  -0.4364923
#> [2,]  -0.30071802  -1.1290958   -0.3570112  -0.4364923
#> [3,]  -0.86811050  -0.6014810   -0.9328358  -0.4364923
#> [4,]  -1.15180675  -0.8652884    0.2188133  -0.4364923
#> [5,]  -0.01702177   0.4537488   -0.3570112  -0.4364923
#> [6,]   1.11776320   1.2451711    1.3704625   1.4613004
BY(m, g, fquantile)
#>                 Sepal.Length Sepal.Width Petal.Length Petal.Width
#> setosa.0%              4.300       2.300        1.000         0.1
#> setosa.25%             4.800       3.200        1.400         0.2
#> setosa.50%             5.000       3.400        1.500         0.2
#> setosa.75%             5.200       3.675        1.575         0.3
#> setosa.100%            5.800       4.400        1.900         0.6
#> versicolor.0%          4.900       2.000        3.000         1.0
#> versicolor.25%         5.600       2.525        4.000         1.2
#> versicolor.50%         5.900       2.800        4.350         1.3
#> versicolor.75%         6.300       3.000        4.600         1.5
#> versicolor.100%        7.000       3.400        5.100         1.8
#> virginica.0%           4.900       2.200        4.500         1.4
#> virginica.25%          6.225       2.800        5.100         1.8
#> virginica.50%          6.500       3.000        5.550         2.0
#> virginica.75%          6.900       3.175        5.875         2.3
#> virginica.100%         7.900       3.800        6.900         2.5
BY(m, g, fquantile, expand.wide = TRUE)
#>            Sepal.Length.0% Sepal.Length.25% Sepal.Length.50% Sepal.Length.75%
#> setosa                 4.3            4.800              5.0              5.2
#> versicolor             4.9            5.600              5.9              6.3
#> virginica              4.9            6.225              6.5              6.9
#>            Sepal.Length.100% Sepal.Width.0% Sepal.Width.25% Sepal.Width.50%
#> setosa                   5.8            2.3           3.200             3.4
#> versicolor               7.0            2.0           2.525             2.8
#> virginica                7.9            2.2           2.800             3.0
#>            Sepal.Width.75% Sepal.Width.100% Petal.Length.0% Petal.Length.25%
#> setosa               3.675              4.4             1.0              1.4
#> versicolor           3.000              3.4             3.0              4.0
#> virginica            3.175              3.8             4.5              5.1
#>            Petal.Length.50% Petal.Length.75% Petal.Length.100% Petal.Width.0%
#> setosa                 1.50            1.575               1.9            0.1
#> versicolor             4.35            4.600               5.1            1.0
#> virginica              5.55            5.875               6.9            1.4
#>            Petal.Width.25% Petal.Width.50% Petal.Width.75% Petal.Width.100%
#> setosa                 0.2             0.2             0.3              0.6
#> versicolor             1.2             1.3             1.5              1.8
#> virginica              1.8             2.0             2.3              2.5
ml <- BY(m, g, fquantile, expand.wide = TRUE, # Return as list of matrices
         return = "list")
ml
#> $Sepal.Length
#>             0%   25% 50% 75% 100%
#> setosa     4.3 4.800 5.0 5.2  5.8
#> versicolor 4.9 5.600 5.9 6.3  7.0
#> virginica  4.9 6.225 6.5 6.9  7.9
#> 
#> $Sepal.Width
#>             0%   25% 50%   75% 100%
#> setosa     2.3 3.200 3.4 3.675  4.4
#> versicolor 2.0 2.525 2.8 3.000  3.4
#> virginica  2.2 2.800 3.0 3.175  3.8
#> 
#> $Petal.Length
#>             0% 25%  50%   75% 100%
#> setosa     1.0 1.4 1.50 1.575  1.9
#> versicolor 3.0 4.0 4.35 4.600  5.1
#> virginica  4.5 5.1 5.55 5.875  6.9
#> 
#> $Petal.Width
#>             0% 25% 50% 75% 100%
#> setosa     0.1 0.2 0.2 0.3  0.6
#> versicolor 1.0 1.2 1.3 1.5  1.8
#> virginica  1.4 1.8 2.0 2.3  2.5
#> 
# Unlisting to Data Frame
unlist2d(ml, idcols = "Variable", row.names = "Species")
#>        Variable    Species  0%   25%  50%   75% 100%
#> 1  Sepal.Length     setosa 4.3 4.800 5.00 5.200  5.8
#> 2  Sepal.Length versicolor 4.9 5.600 5.90 6.300  7.0
#> 3  Sepal.Length  virginica 4.9 6.225 6.50 6.900  7.9
#> 4   Sepal.Width     setosa 2.3 3.200 3.40 3.675  4.4
#> 5   Sepal.Width versicolor 2.0 2.525 2.80 3.000  3.4
#> 6   Sepal.Width  virginica 2.2 2.800 3.00 3.175  3.8
#> 7  Petal.Length     setosa 1.0 1.400 1.50 1.575  1.9
#> 8  Petal.Length versicolor 3.0 4.000 4.35 4.600  5.1
#> 9  Petal.Length  virginica 4.5 5.100 5.55 5.875  6.9
#> 10  Petal.Width     setosa 0.1 0.200 0.20 0.300  0.6
#>  [ reached 'max' / getOption("max.print") -- omitted 2 rows ]

## data.frame method
BY(num_vars(iris), g, sum)             # Also returns a data.fram
#>            Sepal.Length Sepal.Width Petal.Length Petal.Width
#> setosa            250.3       171.4         73.1        12.3
#> versicolor        296.8       138.5        213.0        66.3
#> virginica         329.4       148.7        277.6       101.3
BY(num_vars(iris), g, sum, return = 2) # Return as matrix.. also works for computations below
#>            Sepal.Length Sepal.Width Petal.Length Petal.Width
#> setosa            250.3       171.4         73.1        12.3
#> versicolor        296.8       138.5        213.0        66.3
#> virginica         329.4       148.7        277.6       101.3
head(BY(num_vars(iris), g, scale))
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1   0.26667447   0.1899414   -0.3570112  -0.4364923
#> 2  -0.30071802  -1.1290958   -0.3570112  -0.4364923
#> 3  -0.86811050  -0.6014810   -0.9328358  -0.4364923
#> 4  -1.15180675  -0.8652884    0.2188133  -0.4364923
#> 5  -0.01702177   0.4537488   -0.3570112  -0.4364923
#> 6   1.11776320   1.2451711    1.3704625   1.4613004
BY(num_vars(iris), g, fquantile)
#>                 Sepal.Length Sepal.Width Petal.Length Petal.Width
#> setosa.0%              4.300       2.300        1.000         0.1
#> setosa.25%             4.800       3.200        1.400         0.2
#> setosa.50%             5.000       3.400        1.500         0.2
#> setosa.75%             5.200       3.675        1.575         0.3
#> setosa.100%            5.800       4.400        1.900         0.6
#> versicolor.0%          4.900       2.000        3.000         1.0
#> versicolor.25%         5.600       2.525        4.000         1.2
#> versicolor.50%         5.900       2.800        4.350         1.3
#> versicolor.75%         6.300       3.000        4.600         1.5
#> versicolor.100%        7.000       3.400        5.100         1.8
#> virginica.0%           4.900       2.200        4.500         1.4
#> virginica.25%          6.225       2.800        5.100         1.8
#> virginica.50%          6.500       3.000        5.550         2.0
#> virginica.75%          6.900       3.175        5.875         2.3
#> virginica.100%         7.900       3.800        6.900         2.5
BY(num_vars(iris), g, fquantile, expand.wide = TRUE)
#>            Sepal.Length.0% Sepal.Length.25% Sepal.Length.50% Sepal.Length.75%
#> setosa                 4.3            4.800              5.0              5.2
#> versicolor             4.9            5.600              5.9              6.3
#> virginica              4.9            6.225              6.5              6.9
#>            Sepal.Length.100% Sepal.Width.0% Sepal.Width.25% Sepal.Width.50%
#> setosa                   5.8            2.3           3.200             3.4
#> versicolor               7.0            2.0           2.525             2.8
#> virginica                7.9            2.2           2.800             3.0
#>            Sepal.Width.75% Sepal.Width.100% Petal.Length.0% Petal.Length.25%
#> setosa               3.675              4.4             1.0              1.4
#> versicolor           3.000              3.4             3.0              4.0
#> virginica            3.175              3.8             4.5              5.1
#>            Petal.Length.50% Petal.Length.75% Petal.Length.100% Petal.Width.0%
#> setosa                 1.50            1.575               1.9            0.1
#> versicolor             4.35            4.600               5.1            1.0
#> virginica              5.55            5.875               6.9            1.4
#>            Petal.Width.25% Petal.Width.50% Petal.Width.75% Petal.Width.100%
#> setosa                 0.2             0.2             0.3              0.6
#> versicolor             1.2             1.3             1.5              1.8
#> virginica              1.8             2.0             2.3              2.5
BY(num_vars(iris), g, fquantile,       # Return as list of matrices
   expand.wide = TRUE, return = "list")
#> $Sepal.Length
#>             0%   25% 50% 75% 100%
#> setosa     4.3 4.800 5.0 5.2  5.8
#> versicolor 4.9 5.600 5.9 6.3  7.0
#> virginica  4.9 6.225 6.5 6.9  7.9
#> 
#> $Sepal.Width
#>             0%   25% 50%   75% 100%
#> setosa     2.3 3.200 3.4 3.675  4.4
#> versicolor 2.0 2.525 2.8 3.000  3.4
#> virginica  2.2 2.800 3.0 3.175  3.8
#> 
#> $Petal.Length
#>             0% 25%  50%   75% 100%
#> setosa     1.0 1.4 1.50 1.575  1.9
#> versicolor 3.0 4.0 4.35 4.600  5.1
#> virginica  4.5 5.1 5.55 5.875  6.9
#> 
#> $Petal.Width
#>             0% 25% 50% 75% 100%
#> setosa     0.1 0.2 0.2 0.3  0.6
#> versicolor 1.0 1.2 1.3 1.5  1.8
#> virginica  1.4 1.8 2.0 2.3  2.5
#> 

## grouped data frame method
giris <- fgroup_by(iris, Species)
giris |> BY(sum)                      # Compute sum
#>      Species Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1     setosa        250.3       171.4         73.1        12.3
#> 2 versicolor        296.8       138.5        213.0        66.3
#> 3  virginica        329.4       148.7        277.6       101.3
giris |> BY(sum, use.g.names = TRUE,  # Use row.names and
             keep.group_vars = FALSE)  # remove 'Species' and groups attribute
#>            Sepal.Length Sepal.Width Petal.Length Petal.Width
#> setosa            250.3       171.4         73.1        12.3
#> versicolor        296.8       138.5        213.0        66.3
#> virginica         329.4       148.7        277.6       101.3
giris |> BY(sum, return = "matrix")   # Return matrix
#>      Sepal.Length Sepal.Width Petal.Length Petal.Width
#> [1,]        250.3       171.4         73.1        12.3
#> [2,]        296.8       138.5        213.0        66.3
#> [3,]        329.4       148.7        277.6       101.3
giris |> BY(sum, return = "matrix",   # Matrix with row.names
             use.g.names = TRUE)
#>            Sepal.Length Sepal.Width Petal.Length Petal.Width
#> setosa            250.3       171.4         73.1        12.3
#> versicolor        296.8       138.5        213.0        66.3
#> virginica         329.4       148.7        277.6       101.3
giris |> BY(.quantile)                # Compute quantiles (output is stacked)
#>       Species Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1      setosa        4.300       2.300        1.000         0.1
#> 2      setosa        4.800       3.200        1.400         0.2
#> 3      setosa        5.000       3.400        1.500         0.2
#> 4      setosa        5.200       3.675        1.575         0.3
#> 5      setosa        5.800       4.400        1.900         0.6
#> 6  versicolor        4.900       2.000        3.000         1.0
#> 7  versicolor        5.600       2.525        4.000         1.2
#> 8  versicolor        5.900       2.800        4.350         1.3
#> 9  versicolor        6.300       3.000        4.600         1.5
#> 10 versicolor        7.000       3.400        5.100         1.8
#> 11  virginica        4.900       2.200        4.500         1.4
#> 12  virginica        6.225       2.800        5.100         1.8
#> 13  virginica        6.500       3.000        5.550         2.0
#> 14  virginica        6.900       3.175        5.875         2.3
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
giris |> BY(.quantile, names = TRUE,  # Wide output
               expand.wide = TRUE)
#>      Species Sepal.Length.0% Sepal.Length.25% Sepal.Length.50% Sepal.Length.75%
#> 1     setosa             4.3            4.800              5.0              5.2
#> 2 versicolor             4.9            5.600              5.9              6.3
#> 3  virginica             4.9            6.225              6.5              6.9
#>   Sepal.Length.100% Sepal.Width.0% Sepal.Width.25% Sepal.Width.50%
#> 1               5.8            2.3           3.200             3.4
#> 2               7.0            2.0           2.525             2.8
#> 3               7.9            2.2           2.800             3.0
#>   Sepal.Width.75% Sepal.Width.100% Petal.Length.0% Petal.Length.25%
#> 1           3.675              4.4             1.0              1.4
#> 2           3.000              3.4             3.0              4.0
#> 3           3.175              3.8             4.5              5.1
#>   Petal.Length.50% Petal.Length.75% Petal.Length.100% Petal.Width.0%
#> 1             1.50            1.575               1.9            0.1
#> 2             4.35            4.600               5.1            1.0
#> 3             5.55            5.875               6.9            1.4
#>   Petal.Width.25% Petal.Width.50% Petal.Width.75% Petal.Width.100%
#> 1             0.2             0.2             0.3              0.6
#> 2             1.2             1.3             1.5              1.8
#> 3             1.8             2.0             2.3              2.5
```
