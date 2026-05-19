# Fast Summarise

`fsummarise` is a much faster version of
[`dplyr::summarise`](https://dplyr.tidyverse.org/reference/summarise.html),
when used together with the [Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md).

`fsummarize` and `fsummarise` are synonyms.

## Usage

``` r
fsummarise(.data, ..., keep.group_vars = TRUE, .cols = NULL)
fsummarize(.data, ..., keep.group_vars = TRUE, .cols = NULL)
smr(.data, ..., keep.group_vars = TRUE, .cols = NULL)        # Shorthand
```

## Arguments

- .data:

  a (grouped) data frame or named list of columns. Grouped data can be
  created with
  [`fgroup_by`](https://fastverse.org/collapse/reference/GRP.md) or
  [`dplyr::group_by`](https://dplyr.tidyverse.org/reference/group_by.html).

- ...:

  name-value pairs of summary functions,
  [`across`](https://fastverse.org/collapse/reference/across.md)
  statements, or arbitrary expressions resulting in a list. See
  Examples. For fast performance use the [Fast Statistical
  Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md).

- keep.group_vars:

  logical. `FALSE` removes grouping variables after computation.

- .cols:

  for expressions involving `.data`, `.cols` can be used to subset
  columns, e.g.
  `mtcars |> gby(cyl) |> smr(mctl(cor(.data), TRUE), .cols = 5:7)`. Can
  pass column names, indices, a logical vector or a selector function
  (e.g. `is.numericr`).

## Value

If `.data` is grouped by
[`fgroup_by`](https://fastverse.org/collapse/reference/GRP.md) or
[`dplyr::group_by`](https://dplyr.tidyverse.org/reference/group_by.html),
the result is a data frame of the same class and attributes with rows
reduced to the number of groups. If `.data` is not grouped, the result
is a data frame of the same class and attributes with 1 row.

## Note

Since v1.7, `fsummarise` is fully featured, allowing expressions using
functions and columns of the data as well as external scalar values
(just like
[`dplyr::summarise`](https://dplyr.tidyverse.org/reference/summarise.html)).
**NOTE** however that once a [Fast Statistical
Function](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
is used, the execution will be vectorized instead of split-apply-combine
computing over groups. Please see the first Example.

## See also

[`across`](https://fastverse.org/collapse/reference/across.md),
[`collap`](https://fastverse.org/collapse/reference/collap.md), [Data
Frame
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Since v1.7, fsummarise supports arbitrary expressions, and expressions
## containing fast statistical functions receive vectorized execution:

# (a) This is an expression using base R functions which is executed by groups
mtcars |> fgroup_by(cyl) |> fsummarise(res = mean(mpg) + min(qsec))
#>   cyl      res
#> 1   4 43.36364
#> 2   6 35.24286
#> 3   8 29.60000

# (b) Here, the use of fmean causes the whole expression to be executed
# in a vectorized way i.e. the expression is translated to something like
# fmean(mpg, g = cyl) + min(mpg) and executed, thus the result is different
# from (a), because the minimum is calculated over the entire sample
mtcars |> fgroup_by(cyl) |> fsummarise(mpg = fmean(mpg) + min(qsec))
#>   cyl      mpg
#> 1   4 41.16364
#> 2   6 34.24286
#> 3   8 29.60000

# (c) For fully vectorized execution, use fmin. This yields the same as (a)
mtcars |> fgroup_by(cyl) |> fsummarise(mpg = fmean(mpg) + fmin(qsec))
#>   cyl      mpg
#> 1   4 43.36364
#> 2   6 35.24286
#> 3   8 29.60000

# More advanced use: vectorized grouped regression slopes: mpg ~ carb
mtcars |>
  fgroup_by(cyl) |>
  fmutate(dm_carb = fwithin(carb)) |>
  fsummarise(beta = fsum(mpg, dm_carb) %/=% fsum(dm_carb^2))
#>   cyl         beta
#> 1   4 -1.680000000
#> 2   6 -0.006521739
#> 3   8 -0.647619048


# In across() statements it is fine to mix different functions, each will
# be executed on its own terms (i.e. vectorized for fmean and standard for sum)
mtcars |> fgroup_by(cyl) |> fsummarise(across(mpg:hp, list(fmean, sum)))
#>   cyl mpg_fmean mpg_sum cyl_fmean cyl_sum disp_fmean disp_sum  hp_fmean hp_sum
#> 1   4  26.66364   293.3         4      44   105.1364   1156.5  82.63636    909
#> 2   6  19.74286   138.2         6      42   183.3143   1283.2 122.28571    856
#> 3   8  15.10000   211.4         8     112   353.1000   4943.4 209.21429   2929

# Note that this still detects fmean as a fast function, the names of the list
# are irrelevant, but the function name must be typed or passed as a character vector,
# Otherwise functions will be executed by groups e.g. function(x) fmean(x) won't vectorize
mtcars |> fgroup_by(cyl) |> fsummarise(across(mpg:hp, list(mu = fmean, sum = sum)))
#>   cyl   mpg_mu mpg_sum cyl_mu cyl_sum  disp_mu disp_sum     hp_mu hp_sum
#> 1   4 26.66364   293.3      4      44 105.1364   1156.5  82.63636    909
#> 2   6 19.74286   138.2      6      42 183.3143   1283.2 122.28571    856
#> 3   8 15.10000   211.4      8     112 353.1000   4943.4 209.21429   2929

# We can force none-vectorized execution by setting .apply = TRUE
mtcars |> fgroup_by(cyl) |> fsummarise(across(mpg:hp, list(mu = fmean, sum = sum), .apply = TRUE))
#>   cyl   mpg_mu mpg_sum cyl_mu cyl_sum  disp_mu disp_sum     hp_mu hp_sum
#> 1   4 26.66364   293.3      4      44 105.1364   1156.5  82.63636    909
#> 2   6 19.74286   138.2      6      42 183.3143   1283.2 122.28571    856
#> 3   8 15.10000   211.4      8     112 353.1000   4943.4 209.21429   2929

# Another argument of across(): Order the result first by function, then by column
mtcars |> fgroup_by(cyl) |>
     fsummarise(across(mpg:hp, list(mu = fmean, sum = sum), .transpose = FALSE))
#>   cyl   mpg_mu cyl_mu  disp_mu     hp_mu mpg_sum cyl_sum disp_sum hp_sum
#> 1   4 26.66364      4 105.1364  82.63636   293.3      44   1156.5    909
#> 2   6 19.74286      6 183.3143 122.28571   138.2      42   1283.2    856
#> 3   8 15.10000      8 353.1000 209.21429   211.4     112   4943.4   2929


# Since v1.9.0, can also evaluate arbitrary expressions
mtcars |> fgroup_by(cyl, vs, am) |>
   fsummarise(mctl(cor(cbind(mpg, wt, carb)), names = TRUE))
#>    cyl vs am        mpg         wt       carb
#> 1    4  0  1         NA         NA         NA
#> 2    4  0  1         NA         NA         NA
#> 3    4  0  1         NA         NA         NA
#> 4    4  1  0  1.0000000  0.8606981  0.8346751
#> 5    4  1  0  0.8606981  1.0000000  0.9987950
#> 6    4  1  0  0.8346751  0.9987950  1.0000000
#> 7    4  1  1  1.0000000 -0.7185019 -0.1909932
#> 8    4  1  1 -0.7185019  1.0000000 -0.1253054
#> 9    4  1  1 -0.1909932 -0.1253054  1.0000000
#> 10   6  0  1  1.0000000 -0.1013606 -1.0000000
#> 11   6  0  1 -0.1013606  1.0000000  0.1013606
#>  [ reached 'max' / getOption("max.print") -- omitted 10 rows ]

# This can also be achieved using across():
corfun <- function(x) mctl(cor(x), names = TRUE)
mtcars |> fgroup_by(cyl, vs, am) |>
   fsummarise(across(c(mpg, wt, carb), corfun, .apply = FALSE))
#>    cyl vs am        mpg         wt       carb
#> 1    4  0  1         NA         NA         NA
#> 2    4  0  1         NA         NA         NA
#> 3    4  0  1         NA         NA         NA
#> 4    4  1  0  1.0000000  0.8606981  0.8346751
#> 5    4  1  0  0.8606981  1.0000000  0.9987950
#> 6    4  1  0  0.8346751  0.9987950  1.0000000
#> 7    4  1  1  1.0000000 -0.7185019 -0.1909932
#> 8    4  1  1 -0.7185019  1.0000000 -0.1253054
#> 9    4  1  1 -0.1909932 -0.1253054  1.0000000
#> 10   6  0  1  1.0000000 -0.1013606 -1.0000000
#> 11   6  0  1 -0.1013606  1.0000000  0.1013606
#>  [ reached 'max' / getOption("max.print") -- omitted 10 rows ]

#----------------------------------------------------------------------------
# Examples that also work for pre 1.7 versions

# Simple use
fsummarise(mtcars, mean_mpg = fmean(mpg),
                   sd_mpg = fsd(mpg))
#>   mean_mpg   sd_mpg
#> 1 20.09063 6.026948

# Using base functions (not a big difference without groups)
fsummarise(mtcars, mean_mpg = mean(mpg),
                   sd_mpg = sd(mpg))
#>   mean_mpg   sd_mpg
#> 1 20.09062 6.026948

# Grouped use
mtcars |> fgroup_by(cyl) |>
  fsummarise(mean_mpg = fmean(mpg),
             sd_mpg = fsd(mpg))
#>   cyl mean_mpg   sd_mpg
#> 1   4 26.66364 4.509828
#> 2   6 19.74286 1.453567
#> 3   8 15.10000 2.560048

# This is still efficient but quite a bit slower on large data (many groups)
mtcars |> fgroup_by(cyl) |>
  fsummarise(mean_mpg = mean(mpg),
             sd_mpg = sd(mpg))
#>   cyl mean_mpg   sd_mpg
#> 1   4 26.66364 4.509828
#> 2   6 19.74286 1.453567
#> 3   8 15.10000 2.560048

# Weighted aggregation
mtcars |> fgroup_by(cyl) |>
  fsummarise(w_mean_mpg = fmean(mpg, wt),
             w_sd_mpg = fsd(mpg, wt))
#>   cyl w_mean_mpg w_sd_mpg
#> 1   4   25.93504 4.275234
#> 2   6   19.64578 1.397297
#> 3   8   14.80643 2.638850

 
## Can also group with dplyr::group_by, but at a conversion cost, see ?GRP
library(dplyr)
mtcars |> group_by(cyl) |>
  fsummarise(mean_mpg = fmean(mpg),
             sd_mpg = fsd(mpg))
#> # A tibble: 3 × 3
#>     cyl mean_mpg sd_mpg
#>   <dbl>    <dbl>  <dbl>
#> 1     4     26.7   4.51
#> 2     6     19.7   1.45
#> 3     8     15.1   2.56

# Again less efficient...
mtcars |> group_by(cyl) |>
  fsummarise(mean_mpg = mean(mpg),
             sd_mpg = sd(mpg))
#> # A tibble: 3 × 3
#>     cyl mean_mpg sd_mpg
#>   <dbl>    <dbl>  <dbl>
#> 1     4     26.7   4.51
#> 2     6     19.7   1.45
#> 3     8     15.1   2.56

```
