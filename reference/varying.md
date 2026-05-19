# Fast Check of Variation in Data

`varying` is a generic function that (column-wise) checks for variation
in the values of `x`, (optionally) within the groups `g` (e.g. a
panel-identifier).

## Usage

``` r
varying(x, ...)

# Default S3 method
varying(x, g = NULL, any_group = TRUE, use.g.names = TRUE, ...)

# S3 method for class 'matrix'
varying(x, g = NULL, any_group = TRUE, use.g.names = TRUE, drop = TRUE, ...)

# S3 method for class 'data.frame'
varying(x, by = NULL, cols = NULL, any_group = TRUE, use.g.names = TRUE, drop = TRUE, ...)

# Methods for indexed data / compatibility with plm:

# S3 method for class 'pseries'
varying(x, effect = 1L, any_group = TRUE, use.g.names = TRUE, ...)

# S3 method for class 'pdata.frame'
varying(x, effect = 1L, cols = NULL, any_group = TRUE, use.g.names = TRUE,
        drop = TRUE, ...)

# Methods for grouped data frame / compatibility with dplyr:

# S3 method for class 'grouped_df'
varying(x, any_group = TRUE, use.g.names = FALSE, drop = TRUE,
        keep.group_vars = TRUE, ...)

# Methods for grouped data frame / compatibility with sf:

# S3 method for class 'sf'
varying(x, by = NULL, cols = NULL, any_group = TRUE, use.g.names = TRUE, drop = TRUE, ...)
```

## Arguments

- x:

  a vector, matrix, data frame, 'indexed_series' ('pseries'),
  'indexed_frame' ('pdata.frame') or grouped data frame ('grouped_df').
  Data must not be numeric.

- g:

  a factor, `GRP` object, atomic vector (internally converted to factor)
  or a list of vectors / factors (internally converted to a `GRP`
  object) used to group `x`.

- by:

  same as `g`, but also allows one- or two-sided formulas i.e.
  `~ group1 + group2` or `var1 + var2 ~ group1 + group2`. See Examples

- any_group:

  logical. If `!is.null(g)`, `FALSE` will check and report variation in
  all groups, whereas the default `TRUE` only checks if there is
  variation within any group. See Examples.

- cols:

  select columns using column names, indices or a function (e.g.
  `is.numeric`). Two-sided formulas passed to `by` overwrite `cols`.

- use.g.names:

  logical. Make group-names and add to the result as names (default
  method) or row-names (matrix and data frame methods). No row-names are
  generated for *data.table*'s.

- drop:

  *matrix and data.frame methods:* Logical. `TRUE` drops dimensions and
  returns an atomic vector if the result is 1-dimensional.

- effect:

  *plm* methods: Select the panel identifier by which variation in the
  data should be examined. 1L takes the first variable in the
  [index](https://fastverse.org/collapse/reference/indexing.md), 2L the
  second etc.. Index variables can also be called by name. More than one
  index variable can be supplied, which will be interacted.

- keep.group_vars:

  *grouped_df method:* Logical. `FALSE` removes grouping variables after
  computation.

- ...:

  arguments to be passed to or from other methods.

## Details

Without groups passed to `g`, `varying` simply checks if there is any
variation in the columns of `x` and returns `TRUE` for each column where
this is the case and `FALSE` otherwise. A set of data points is defined
as varying if it contains at least 2 distinct non-missing values (such
that a non-0 standard deviation can be computed on numeric data).
`varying` checks for variation in both numeric and non-numeric data.

If groups are supplied to `g` (or alternatively a *grouped_df* to `x`),
`varying` can operate in one of 2 modes:

- If `any_group = TRUE` (the default), `varying` checks each column for
  variation in any of the groups defined by `g`, and returns `TRUE` if
  such within-variation was detected and `FALSE` otherwise. Thus only
  one logical value is returned for each column and the computation on
  each column is terminated as soon as any variation within any group
  was found.

- If `any_group = FALSE`, `varying` runs through the entire data
  checking each group for variation and returns, for each column in `x`,
  a logical vector reporting the variation check for all groups. If a
  group contains only missing values, a `NA` is returned for that group.

The *sf* method simply ignores the geometry column.

## Value

A logical vector or (if `!is.null(g)` and `any_group = FALSE`), a matrix
or data frame of logical vectors indicating whether the data vary (over
the dimension supplied by `g`).

## See also

[Summary
Statistics](https://fastverse.org/collapse/reference/summary-statistics.md),
[Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Checks overall variation in all columns
varying(wlddev)
#> country   iso3c    date    year  decade  region  income    OECD   PCGDP  LIFEEX 
#>    TRUE    TRUE    TRUE    TRUE    TRUE    TRUE    TRUE    TRUE    TRUE    TRUE 
#>    GINI     ODA     POP 
#>    TRUE    TRUE    TRUE 

## Checks whether data are time-variant i.e. vary within country
varying(wlddev, ~ country)
#>  iso3c   date   year decade region income   OECD  PCGDP LIFEEX   GINI    ODA 
#>  FALSE   TRUE   TRUE   TRUE  FALSE  FALSE  FALSE   TRUE   TRUE   TRUE   TRUE 
#>    POP 
#>   TRUE 

## Same as above but done for each country individually, countries without data are coded NA
head(varying(wlddev, ~ country, any_group = FALSE))
#>                iso3c date year decade region income  OECD PCGDP LIFEEX GINI
#> Afghanistan    FALSE TRUE TRUE   TRUE  FALSE  FALSE FALSE  TRUE   TRUE   NA
#> Albania        FALSE TRUE TRUE   TRUE  FALSE  FALSE FALSE  TRUE   TRUE TRUE
#> Algeria        FALSE TRUE TRUE   TRUE  FALSE  FALSE FALSE  TRUE   TRUE TRUE
#> American Samoa FALSE TRUE TRUE   TRUE  FALSE  FALSE FALSE  TRUE     NA   NA
#> Andorra        FALSE TRUE TRUE   TRUE  FALSE  FALSE FALSE  TRUE     NA   NA
#>                 ODA  POP
#> Afghanistan    TRUE TRUE
#> Albania        TRUE TRUE
#> Algeria        TRUE TRUE
#> American Samoa   NA TRUE
#> Andorra          NA TRUE
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
```
