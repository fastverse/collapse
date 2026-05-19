# Fast Transform and Compute Columns on a Data Frame

`ftransform` is a much faster version of
[`transform`](https://rdrr.io/pkg/data.table/man/transform.data.table.html)
for data frames. It returns the data frame with new columns computed
and/or existing columns modified or deleted. `settransform` does all of
that by reference. `fcompute` computes and returns new columns. These
functions evaluate all arguments simultaneously, allow list-input
(nested pipelines) and disregard grouped data.

Catering to the *tidyverse* user, v1.7.0 introduced `fmutate`, providing
familiar functionality i.e. arguments are evaluated sequentially,
computation on grouped data is done by groups, and functions can be
applied to multiple columns using
[`across`](https://fastverse.org/collapse/reference/across.md). See also
the Details.

## Usage

``` r
# dplyr-style mutate (sequential evaluation + across() feature)
fmutate(.data, ..., .keep = "all", .cols = NULL)
mtt(.data, ..., .keep = "all", .cols = NULL) # Shorthand for fmutate

# Modify and return data frame
ftransform(.data, ...)
ftransformv(.data, vars, FUN, ..., apply = TRUE)
tfm(.data, ...)               # Shorthand for ftransform
tfmv(.data, vars, FUN, ..., apply = TRUE)

# Modify data frame by reference
settransform(.data, ...)
settransformv(.data, ...)     # Same arguments as ftransformv
settfm(.data, ...)            # Shorthand for settransform
settfmv(.data, ...)

# Replace/add modified columns in/to a data frame
ftransform(.data) <- value
tfm(.data) <- value           # Shorthand for ftransform<-

# Compute columns, returned as a new data frame
fcompute(.data, ..., keep = NULL)
fcomputev(.data, vars, FUN, ..., apply = TRUE, keep = NULL)
```

## Arguments

- .data:

  a data frame or named list of columns.

- ...:

  further arguments of the form `column = value`. The `value` can be a
  combination of other columns, a scalar value, or `NULL`, which deletes
  `column`. Alternatively it is also possible to place a single list
  here, which will be treated like a list of `column = value` arguments.
  For `ftransformv` and `fcomputev`, `...` can be used to pass further
  arguments to `FUN`. The ellipsis (`...`) is always evaluated within
  the data frame (`.data`) environment. See Examples. `fmutate`
  additionally supports
  [`across`](https://fastverse.org/collapse/reference/across.md)
  statements, and evaluates tagged vector expressions sequentially. With
  grouped execution, `dots` can also contain arbitrary expressions that
  result in a list of data-length columns. See Examples.

- vars:

  variables to be transformed by applying `FUN` to them: select using
  names, indices, a logical vector or a selector function (e.g.
  `is.numeric`). Since v1.7 `vars` is evaluated within the `.data`
  environment, permitting expressions on columns e.g.
  `c(col1, col3:coln)`.

- FUN:

  a single function yielding a result of length `NROW(.data)` or 1. See
  also `apply`.

- apply:

  logical. `TRUE` (default) will apply `FUN` to each column selected in
  `vars`; `FALSE` will apply `FUN` to the subsetted data frame i.e.
  `FUN(get_vars(.data, vars), ...)`. The latter is useful for *collapse*
  functions with data frame or grouped / panel data frame methods,
  yielding performance gains and enabling grouped transformations. See
  Examples.

- value:

  a named list of replacements, it will be treated like an evaluated
  list of `column = value` arguments.

- keep:

  select columns to preserve using column names, indices or a function
  (e.g. `is.numeric`). By default computed columns are added after the
  preserved ones, unless they are assigned the same name in which case
  the preserved columns will be replaced in order.

- .keep:

  either one of `"all", "used", "unused"` or `"none"` (see
  [`mutate`](https://dplyr.tidyverse.org/reference/mutate.html)), or
  columns names/indices/function as `keep`. *Note* that this does not
  work well with
  [`across()`](https://fastverse.org/collapse/reference/across.md) or
  other expressions supported since v1.9.0. The only sensible option you
  have there is to supply a character vector of all columns in the final
  dataset that you want to keep.

- .cols:

  for expressions involving `.data`, `.cols` can be used to subset
  columns, e.g.
  `mtcars |> gby(cyl) |> mtt(broom::augment(lm(mpg ~., .data)), .cols = 1:7)`.
  Can pass column names, indices, a logical vector or a selector
  function (e.g. `is.numericr`).

## Details

The `...` arguments to `ftransform` are tagged vector expressions, which
are evaluated in the data frame `.data`. The tags are matched against
`names(.data)`, and for those that match, the values replace the
corresponding variable in `.data`, whereas the others are appended to
`.data`. It is also possible to delete columns by assigning `NULL` to
them, i.e. `ftransform(data, colk = NULL)` removes `colk` from the data.
*Note* that `names(.data)` and the names of the `...` arguments are
checked for uniqueness beforehand, yielding an error if this is not the
case.

Since *collapse* v1.3.0, is is also possible to pass a single named list
to `...`, i.e. `ftransform(data, newdata)`. This list will be treated
like a list of tagged vector expressions. *Note* the different behavior:
`ftransform(data, list(newcol = col1))` is the same as
`ftransform(data, newcol = col1)`, whereas
`ftransform(data, newcol = as.list(col1))` creates a list column.
Something like `ftransform(data, as.list(col1))` gives an error because
the list is not named. See Examples.

The function `ftransformv` added in v1.3.2 provides a fast replacement
for the functions
[`dplyr::mutate_at`](https://dplyr.tidyverse.org/reference/mutate_all.html)
and
[`dplyr::mutate_if`](https://dplyr.tidyverse.org/reference/mutate_all.html)
(without the grouping feature) facilitating mutations of groups of
columns
([`dplyr::mutate_all`](https://dplyr.tidyverse.org/reference/mutate_all.html)
is already accounted for by
[`dapply`](https://fastverse.org/collapse/reference/dapply.md)). See
Examples.

The function `settransform` does all of that by reference, but uses
base-R's copy-on modify semantics, which is equivalent to replacing the
data with `<-` (thus it is still memory efficient but the data will have
a different memory address afterwards).

The function `fcompute(v)` works just like `ftransform(v)`, but returns
only the changed / computed columns without modifying or appending the
data in `.data`. See Examples.

The function `fmutate` added in v1.7.0, provides functionality familiar
from *dplyr* 1.0.0 and higher. It evaluates tagged vector expressions
sequentially and does operations by groups on a grouped frame (thus it
is slower than `ftransform` if you have many tagged expressions or a
grouped data frame). Note however that *collapse* does not depend on
*rlang*, so things like lambda expressions are not available. *Note
also* that `fmutate` operates differently on grouped data whether you
use `.FAST_FUN` or base R functions / functions from other packages.
With `.FAST_FUN` (including `.OPERATOR_FUN`, excluding `fhdbetween` /
`fhdwithin` / `HDW` / `HDB`), `fmutate` performs an efficient vectorized
execution, i.e. the grouping object from the grouped data frame is
passed to the `g` argument of these functions, and for `.FAST_STAT_FUN`
also `TRA = "replace_fill"` is set (if not overwritten by the user),
yielding internal grouped computation by these functions without the
need for splitting the data by groups. For base R and other functions,
`fmutate` performs classical split-apply combine computing i.e. the
relevant columns of the data are selected and split into groups, the
expression is evaluated for each group, and the result is recombined and
suitably expanded to match the original data frame. **Note** that it is
not possible to mix vectorized and standard execution in the same
expression!! Vectorized execution is performed if **any** `.FAST_FUN` or
`.OPERATOR_FUN` is part of the expression, thus a code like
`mtcars |> gby(cyl) |> fmutate(new = fmin(mpg) / min(mpg))` will be
expanded to something like
`mtcars |> gby(cyl) |> ftransform(new = fmin(mpg, g = GRP(.), TRA = "replace_fill") / min(mpg))`
and then executed, i.e. `fmin(mpg)` will be executed in a vectorized
way, and `min(mpg)` will not be executed by groups at all.

## Note

`ftransform` ignores grouped data. This is on purpose as it allows
non-grouped transformation inside a pipeline on grouped data, and
affords greater flexibility and performance in programming with the
`.FAST_FUN`. In particular, you can run a nested pipeline inside
`ftransform`, and decide which expressions should be grouped, and you
can use the ad-hoc grouping functionality of the `.FAST_FUN`, allowing
operations where different groupings are applied simultaneously in an
expression. See Examples or the answer provided
[here](https://stackoverflow.com/questions/67349744/using-ftransform-along-with-fgroup-by-from-collapse-r-package).

`fmutate` on the other hand supports grouped operations just like
[`dplyr::mutate`](https://dplyr.tidyverse.org/reference/mutate.html),
but works in two different ways depending on whether you use `.FAST_FUN`
in an expression or other functions. See the Examples.

## Value

The modified data frame `.data`, or, for `fcompute`, a new data frame
with the columns computed on `.data`. All attributes of `.data` are
preserved.

## See also

[`across`](https://fastverse.org/collapse/reference/across.md),
[`fsummarise`](https://fastverse.org/collapse/reference/fsummarise.md),
[Data Frame
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r

## fmutate() examples ---------------------------------------------------------------

# Please note that expressions are vectorized whenever they contain 'ANY' fast function
mtcars |>
  fgroup_by(cyl, vs, am) |>
  fmutate(mean_mpg = fmean(mpg),                     # Vectorized
          mean_mpg_base = mean(mpg),                 # Non-vectorized
          mpg_cumpr = fcumsum(mpg) / fsum(mpg),      # Vectorized
          mpg_cumpr_base = cumsum(mpg) / sum(mpg),   # Non-vectorized
          mpg_cumpr_mixed = fcumsum(mpg) / sum(mpg)) # Vectorized: division by overall sum
#>                 mpg cyl disp  hp drat    wt  qsec vs am gear carb mean_mpg
#> Mazda RX4      21.0   6  160 110 3.90 2.620 16.46  0  1    4    4 20.56667
#> Mazda RX4 Wag  21.0   6  160 110 3.90 2.875 17.02  0  1    4    4 20.56667
#> Datsun 710     22.8   4  108  93 3.85 2.320 18.61  1  1    4    1 28.37143
#> Hornet 4 Drive 21.4   6  258 110 3.08 3.215 19.44  1  0    3    1 19.12500
#>                mean_mpg_base mpg_cumpr mpg_cumpr_base mpg_cumpr_mixed
#> Mazda RX4           20.56667 0.3403566      0.3403566      0.03266449
#> Mazda RX4 Wag       20.56667 0.6807131      0.6807131      0.06532898
#> Datsun 710          28.37143 0.1148036      0.1148036      0.03546430
#> Hornet 4 Drive      19.12500 0.2797386      0.2797386      0.03328667
#>  [ reached 'max' / getOption("max.print") -- omitted 28 rows ]
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 

# Using across: here fmean() gets vectorized across both groups and columns (requiring a single
# call to fmean.data.frame which goes to C), whereas weighted.mean needs to be called many times.
mtcars |> fgroup_by(cyl, vs, am) |>
  fmutate(across(disp:qsec, list(mu = fmean, mu2 = weighted.mean), w = wt, .names = "flip"))
#>                mpg cyl disp  hp drat    wt  qsec vs am gear carb  mu_disp
#> Mazda RX4     21.0   6  160 110 3.90 2.620 16.46  0  1    4    4 154.9728
#> Mazda RX4 Wag 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4 154.9728
#> Datsun 710    22.8   4  108  93 3.85 2.320 18.61  1  1    4    1  92.2352
#>               mu2_disp     mu_hp    mu2_hp  mu_drat mu2_drat    mu_wt   mu2_wt
#> Mazda RX4     154.9728 131.78463 131.78463 3.806158 3.806158 2.758975 2.758975
#> Mazda RX4 Wag 154.9728 131.78463 131.78463 3.806158 3.806158 2.758975 2.758975
#> Datsun 710     92.2352  82.11819  82.11819 4.130037 4.130037 2.110131 2.110131
#>                mu_qsec mu2_qsec
#> Mazda RX4     16.33306 16.33306
#> Mazda RX4 Wag 16.33306 16.33306
#> Datsun 710    18.75509 18.75509
#>  [ reached 'max' / getOption("max.print") -- omitted 29 rows ]
#> 
#> Grouped by:  cyl, vs, am  [7 | 5 (3.8) 1-12] 

# Can do more complex things...
mtcars |> fgroup_by(cyl) |>
  fmutate(res = resid(lm(mpg ~ carb + hp, weights = wt)))
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb       res
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46  0  1    4    4  1.125008
#> Mazda RX4 Wag     21.0   6  160 110 3.90 2.875 17.02  0  1    4    4  1.125008
#> Datsun 710        22.8   4  108  93 3.85 2.320 18.61  1  1    4    1 -2.716748
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44  1  0    3    1  1.870410
#> Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2  2.229621
#>  [ reached 'max' / getOption("max.print") -- omitted 27 rows ]
#> 
#> Grouped by:  cyl  [3 | 11 (3.5) 7-14] 

# Since v1.9.0: supports arbitrary expressions returning suitable lists
if (FALSE)  
mtcars |> fgroup_by(cyl) |>
  fmutate(broom::augment(lm(mpg ~ carb + hp, weights = wt)))

# Same thing using across() (supported before 1.9.0)
modelfun <- function(data) broom::augment(lm(mpg ~ carb + hp, data, weights = wt))
mtcars |> fgroup_by(cyl) |>
  fmutate(across(c(mpg, carb, hp, wt), modelfun, .apply = FALSE))
#> Error in loadNamespace(x): there is no package called ‘broom’
 # \dontrun{}


## ftransform() / fcompute() examples: ----------------------------------------------

## ftransform modifies and returns a data.frame
head(ftransform(airquality, Ozone = -Ozone))
#>   Ozone Solar.R Wind Temp Month Day
#> 1   -41     190  7.4   67     5   1
#> 2   -36     118  8.0   72     5   2
#> 3   -12     149 12.6   74     5   3
#> 4   -18     313 11.5   62     5   4
#> 5    NA      NA 14.3   56     5   5
#> 6   -28      NA 14.9   66     5   6
head(ftransform(airquality, new = -Ozone, Temp = (Temp-32)/1.8))
#>   Ozone Solar.R Wind     Temp Month Day new
#> 1    41     190  7.4 19.44444     5   1 -41
#> 2    36     118  8.0 22.22222     5   2 -36
#> 3    12     149 12.6 23.33333     5   3 -12
#> 4    18     313 11.5 16.66667     5   4 -18
#> 5    NA      NA 14.3 13.33333     5   5  NA
#> 6    28      NA 14.9 18.88889     5   6 -28
head(ftransform(airquality, new = -Ozone, new2 = 1, Temp = NULL))  # Deleting Temp
#>   Ozone Solar.R Wind Month Day new new2
#> 1    41     190  7.4     5   1 -41    1
#> 2    36     118  8.0     5   2 -36    1
#> 3    12     149 12.6     5   3 -12    1
#> 4    18     313 11.5     5   4 -18    1
#> 5    NA      NA 14.3     5   5  NA    1
#> 6    28      NA 14.9     5   6 -28    1
head(ftransform(airquality, Ozone = NULL, Temp = NULL))            # Deleting columns
#>   Solar.R Wind Month Day
#> 1     190  7.4     5   1
#> 2     118  8.0     5   2
#> 3     149 12.6     5   3
#> 4     313 11.5     5   4
#> 5      NA 14.3     5   5
#> 6      NA 14.9     5   6

# With collapse's grouped and weighted functions, complex operations are done on the fly
head(ftransform(airquality, # Grouped operations by month:
                Ozone_Month_median = fmedian(Ozone, Month, TRA = "fill"),
                Ozone_Month_sd = fsd(Ozone, Month, TRA = "replace"),
                Ozone_Month_centered = fwithin(Ozone, Month)))
#>   Ozone Solar.R Wind Temp Month Day Ozone_Month_median Ozone_Month_sd
#> 1    41     190  7.4   67     5   1                 18       22.22445
#> 2    36     118  8.0   72     5   2                 18       22.22445
#> 3    12     149 12.6   74     5   3                 18       22.22445
#> 4    18     313 11.5   62     5   4                 18       22.22445
#> 5    NA      NA 14.3   56     5   5                 18             NA
#> 6    28      NA 14.9   66     5   6                 18       22.22445
#>   Ozone_Month_centered
#> 1            17.384615
#> 2            12.384615
#> 3           -11.615385
#> 4            -5.615385
#> 5                   NA
#> 6             4.384615

# Grouping by month and above/below average temperature in each month
head(ftransform(airquality, Ozone_Month_high_median =
                  fmedian(Ozone, list(Month, Temp > fbetween(Temp, Month)), TRA = "fill")))
#>   Ozone Solar.R Wind Temp Month Day Ozone_Month_high_median
#> 1    41     190  7.4   67     5   1                      28
#> 2    36     118  8.0   72     5   2                      28
#> 3    12     149 12.6   74     5   3                      28
#> 4    18     313 11.5   62     5   4                      14
#> 5    NA      NA 14.3   56     5   5                      14
#> 6    28      NA 14.9   66     5   6                      28

## ftransformv can be used to modify multiple columns using a function
head(ftransformv(airquality, 1:3, log))
#>      Ozone  Solar.R     Wind Temp Month Day
#> 1 3.713572 5.247024 2.001480   67     5   1
#> 2 3.583519 4.770685 2.079442   72     5   2
#> 3 2.484907 5.003946 2.533697   74     5   3
#> 4 2.890372 5.746203 2.442347   62     5   4
#> 5       NA       NA 2.660260   56     5   5
#> 6 3.332205       NA 2.701361   66     5   6
head(`[<-`(airquality, 1:3, value = lapply(airquality[1:3], log))) # Same thing in base R
#>      Ozone  Solar.R     Wind Temp Month Day
#> 1 3.713572 5.247024 2.001480   67     5   1
#> 2 3.583519 4.770685 2.079442   72     5   2
#> 3 2.484907 5.003946 2.533697   74     5   3
#> 4 2.890372 5.746203 2.442347   62     5   4
#> 5       NA       NA 2.660260   56     5   5
#> 6 3.332205       NA 2.701361   66     5   6

head(ftransformv(airquality, 1:3, log, apply = FALSE))
#>      Ozone  Solar.R     Wind Temp Month Day
#> 1 3.713572 5.247024 2.001480   67     5   1
#> 2 3.583519 4.770685 2.079442   72     5   2
#> 3 2.484907 5.003946 2.533697   74     5   3
#> 4 2.890372 5.746203 2.442347   62     5   4
#> 5       NA       NA 2.660260   56     5   5
#> 6 3.332205       NA 2.701361   66     5   6
head(`[<-`(airquality, 1:3, value = log(airquality[1:3])))         # Same thing in base R
#>      Ozone  Solar.R     Wind Temp Month Day
#> 1 3.713572 5.247024 2.001480   67     5   1
#> 2 3.583519 4.770685 2.079442   72     5   2
#> 3 2.484907 5.003946 2.533697   74     5   3
#> 4 2.890372 5.746203 2.442347   62     5   4
#> 5       NA       NA 2.660260   56     5   5
#> 6 3.332205       NA 2.701361   66     5   6

# Using apply = FALSE yields meaningful performance gains with collapse functions
# This calls fwithin.default, and repeates the grouping by month 3 times:
head(ftransformv(airquality, 1:3, fwithin, Month))
#>        Ozone    Solar.R       Wind Temp Month Day
#> 1  17.384615   8.703704 -4.2225806   67     5   1
#> 2  12.384615 -63.296296 -3.6225806   72     5   2
#> 3 -11.615385 -32.296296  0.9774194   74     5   3
#> 4  -5.615385 131.703704 -0.1225806   62     5   4
#> 5         NA         NA  2.6774194   56     5   5
#> 6   4.384615         NA  3.2774194   66     5   6

# This calls fwithin.data.frame, and only groups one time -> 5x faster!
head(ftransformv(airquality, 1:3, fwithin, Month, apply = FALSE))
#>        Ozone    Solar.R       Wind Temp Month Day
#> 1  17.384615   8.703704 -4.2225806   67     5   1
#> 2  12.384615 -63.296296 -3.6225806   72     5   2
#> 3 -11.615385 -32.296296  0.9774194   74     5   3
#> 4  -5.615385 131.703704 -0.1225806   62     5   4
#> 5         NA         NA  2.6774194   56     5   5
#> 6   4.384615         NA  3.2774194   66     5   6

# This also works for grouped and panel data frames (calling fwithin.grouped_df)
airquality |> fgroup_by(Month) |>
  ftransformv(1:3, fwithin, apply = FALSE) |> head()
#>        Ozone    Solar.R       Wind Temp Month Day
#> 1  17.384615   8.703704 -4.2225806   67     5   1
#> 2  12.384615 -63.296296 -3.6225806   72     5   2
#> 3 -11.615385 -32.296296  0.9774194   74     5   3
#> 4  -5.615385 131.703704 -0.1225806   62     5   4
#> 5         NA         NA  2.6774194   56     5   5
#> 6   4.384615         NA  3.2774194   66     5   6

# But this gives the WRONG result (calling fwithin.default). Need option apply = FALSE!!
airquality |> fgroup_by(Month) |>
  ftransformv(1:3, fwithin) |> head()
#>       Ozone    Solar.R      Wind Temp Month Day
#> 1  -1.12931   4.068493 -2.557516   67     5   1
#> 2  -6.12931 -67.931507 -1.957516   72     5   2
#> 3 -30.12931 -36.931507  2.642484   74     5   3
#> 4 -24.12931 127.068493  1.542484   62     5   4
#> 5        NA         NA  4.342484   56     5   5
#> 6 -14.12931         NA  4.942484   66     5   6

# For grouped modification of single columns in a grouped dataset, we can use GRP():
library(magrittr)
airquality |> fgroup_by(Month) %>%
  ftransform(W_Ozone = fwithin(Ozone, GRP(.)),                 # Grouped centering
             sd_Ozone_m = fsd(Ozone, GRP(.), TRA = "replace"), # In-Month standard deviation
             sd_Ozone = fsd(Ozone, TRA = "replace"),           # Overall standard deviation
             sd_Ozone2 = fsd(Ozone, TRA = "fill"),             # Same, overwriting NA's
             sd_Ozone3 = fsd(Ozone)) |> head()                 # Same thing (calling alloc())
#>   Ozone Solar.R Wind Temp Month Day    W_Ozone sd_Ozone_m sd_Ozone sd_Ozone2
#> 1    41     190  7.4   67     5   1  17.384615   22.22445 32.98788  32.98788
#> 2    36     118  8.0   72     5   2  12.384615   22.22445 32.98788  32.98788
#> 3    12     149 12.6   74     5   3 -11.615385   22.22445 32.98788  32.98788
#> 4    18     313 11.5   62     5   4  -5.615385   22.22445 32.98788  32.98788
#> 5    NA      NA 14.3   56     5   5         NA         NA       NA  32.98788
#> 6    28      NA 14.9   66     5   6   4.384615   22.22445 32.98788  32.98788
#>   sd_Ozone3
#> 1  32.98788
#> 2  32.98788
#> 3  32.98788
#> 4  32.98788
#> 5  32.98788
#> 6  32.98788

## For more complex mutations we can use ftransform with compound pipes
airquality |> fgroup_by(Month) %>%
  ftransform(get_vars(., 1:3) |> fwithin() |> flag(0:2)) |> head()
#>        Ozone    Solar.R       Wind Temp Month Day   L1.Ozone  L2.Ozone
#> 1  17.384615   8.703704 -4.2225806   67     5   1         NA        NA
#> 2  12.384615 -63.296296 -3.6225806   72     5   2  17.384615        NA
#> 3 -11.615385 -32.296296  0.9774194   74     5   3  12.384615  17.38462
#> 4  -5.615385 131.703704 -0.1225806   62     5   4 -11.615385  12.38462
#> 5         NA         NA  2.6774194   56     5   5  -5.615385 -11.61538
#>   L1.Solar.R L2.Solar.R    L1.Wind    L2.Wind
#> 1         NA         NA         NA         NA
#> 2   8.703704         NA -4.2225806         NA
#> 3 -63.296296   8.703704 -3.6225806 -4.2225806
#> 4 -32.296296 -63.296296  0.9774194 -3.6225806
#> 5 131.703704 -32.296296 -0.1225806  0.9774194
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]

airquality %>% ftransform(STD(., cols = 1:3) |> replace_na(0)) |> head()
#>   Ozone Solar.R Wind Temp Month Day   STD.Ozone STD.Solar.R   STD.Wind
#> 1    41     190  7.4   67     5   1 -0.03423409  0.04517615 -0.7259482
#> 2    36     118  8.0   72     5   2 -0.18580489 -0.75430487 -0.5556388
#> 3    12     149 12.6   74     5   3 -0.91334473 -0.41008388  0.7500660
#> 4    18     313 11.5   62     5   4 -0.73145977  1.41095624  0.4378323
#> 5    NA      NA 14.3   56     5   5  0.00000000  0.00000000  1.2326091
#> 6    28      NA 14.9   66     5   6 -0.42831817  0.00000000  1.4029185

# The list argument feature also allows flexible operations creating multiple new columns
airquality |> # The variance of Wind and Ozone, by month, weighted by temperature:
  ftransform(fvar(list(Wind_var = Wind, Ozone_var = Ozone), Month, Temp, "replace")) |> head()
#>   Ozone Solar.R Wind Temp Month Day Wind_var Ozone_var
#> 1    41     190  7.4   67     5   1 12.08975  533.2819
#> 2    36     118  8.0   72     5   2 12.08975  533.2819
#> 3    12     149 12.6   74     5   3 12.08975  533.2819
#> 4    18     313 11.5   62     5   4 12.08975  533.2819
#> 5    NA      NA 14.3   56     5   5 12.08975        NA
#> 6    28      NA 14.9   66     5   6 12.08975  533.2819

# Same as above using a grouped data frame (a bit more complex)
airquality |> fgroup_by(Month) %>%
  ftransform(fselect(., Wind, Ozone) |> fvar(Temp, "replace") |> add_stub("_var", FALSE)) |>
  fungroup() |> head()
#>   Ozone Solar.R Wind Temp Month Day Wind_var Ozone_var
#> 1    41     190  7.4   67     5   1 12.08975  533.2819
#> 2    36     118  8.0   72     5   2 12.08975  533.2819
#> 3    12     149 12.6   74     5   3 12.08975  533.2819
#> 4    18     313 11.5   62     5   4 12.08975  533.2819
#> 5    NA      NA 14.3   56     5   5 12.08975        NA
#> 6    28      NA 14.9   66     5   6 12.08975  533.2819

# This performs 2 different multi-column grouped operations (need c() to make it one list)
ftransform(airquality, c(fmedian(list(Wind_Day_median = Wind,
                                      Ozone_Day_median = Ozone), Day, TRA = "replace"),
                         fsd(list(Wind_Month_sd = Wind,
                                  Ozone_Month_sd = Ozone), Month, TRA = "replace"))) |> head()
#>   Ozone Solar.R Wind Temp Month Day Wind_Day_median Ozone_Day_median
#> 1    41     190  7.4   67     5   1             6.9             68.5
#> 2    36     118  8.0   72     5   2             9.2             42.5
#> 3    12     149 12.6   74     5   3             9.2             24.0
#> 4    18     313 11.5   62     5   4             9.2             78.0
#> 5    NA      NA 14.3   56     5   5             7.4               NA
#> 6    28      NA 14.9   66     5   6            14.3             36.0
#>   Wind_Month_sd Ozone_Month_sd
#> 1       3.53145       22.22445
#> 2       3.53145       22.22445
#> 3       3.53145       22.22445
#> 4       3.53145       22.22445
#> 5       3.53145             NA
#> 6       3.53145       22.22445

## settransform(v) works like ftransform(v) but modifies a data frame in the global environment..
settransform(airquality, Ratio = Ozone / Temp, Ozone = NULL, Temp = NULL)
head(airquality)
#>   Solar.R Wind Month Day     Ratio
#> 1     190  7.4     5   1 0.6119403
#> 2     118  8.0     5   2 0.5000000
#> 3     149 12.6     5   3 0.1621622
#> 4     313 11.5     5   4 0.2903226
#> 5      NA 14.3     5   5        NA
#> 6      NA 14.9     5   6 0.4242424
rm(airquality)

# Grouped and weighted centering
settransformv(airquality, 1:3, fwithin, Month, Temp, apply = FALSE)
head(airquality)
#>       Ozone    Solar.R        Wind Temp Month Day
#> 1  16.22536   3.571669 -4.08917323   67     5   1
#> 2  11.22536 -68.428331 -3.48917323   72     5   2
#> 3 -12.77464 -37.428331  1.11082677   74     5   3
#> 4  -6.77464 126.571669  0.01082677   62     5   4
#> 5        NA         NA  2.81082677   56     5   5
#> 6   3.22536         NA  3.41082677   66     5   6
rm(airquality)

# Suitably lagged first-differences
settransform(airquality, get_vars(airquality, 1:3) |> fdiff() |> flag(0:2))
head(airquality)
#>   Ozone Solar.R Wind Temp Month Day L1.Ozone L2.Ozone L1.Solar.R L2.Solar.R
#> 1    NA      NA   NA   67     5   1       NA       NA         NA         NA
#> 2    -5     -72  0.6   72     5   2       NA       NA         NA         NA
#> 3   -24      31  4.6   74     5   3       -5       NA        -72         NA
#> 4     6     164 -1.1   62     5   4      -24       -5         31        -72
#> 5    NA      NA  2.8   56     5   5        6      -24        164         31
#>   L1.Wind L2.Wind
#> 1      NA      NA
#> 2      NA      NA
#> 3     0.6      NA
#> 4     4.6     0.6
#> 5    -1.1     4.6
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
rm(airquality)

# Same as above using magrittr::`%<>%`
airquality %<>% ftransform(get_vars(., 1:3) |> fdiff() |> flag(0:2))
head(airquality)
#>   Ozone Solar.R Wind Temp Month Day L1.Ozone L2.Ozone L1.Solar.R L2.Solar.R
#> 1    NA      NA   NA   67     5   1       NA       NA         NA         NA
#> 2    -5     -72  0.6   72     5   2       NA       NA         NA         NA
#> 3   -24      31  4.6   74     5   3       -5       NA        -72         NA
#> 4     6     164 -1.1   62     5   4      -24       -5         31        -72
#> 5    NA      NA  2.8   56     5   5        6      -24        164         31
#>   L1.Wind L2.Wind
#> 1      NA      NA
#> 2      NA      NA
#> 3     0.6      NA
#> 4     4.6     0.6
#> 5    -1.1     4.6
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
rm(airquality)

# It is also possible to achieve the same thing via a replacement method (if needed)
ftransform(airquality) <- get_vars(airquality, 1:3) |> fdiff() |> flag(0:2)
head(airquality)
#>   Ozone Solar.R Wind Temp Month Day L1.Ozone L2.Ozone L1.Solar.R L2.Solar.R
#> 1    NA      NA   NA   67     5   1       NA       NA         NA         NA
#> 2    -5     -72  0.6   72     5   2       NA       NA         NA         NA
#> 3   -24      31  4.6   74     5   3       -5       NA        -72         NA
#> 4     6     164 -1.1   62     5   4      -24       -5         31        -72
#> 5    NA      NA  2.8   56     5   5        6      -24        164         31
#>   L1.Wind L2.Wind
#> 1      NA      NA
#> 2      NA      NA
#> 3     0.6      NA
#> 4     4.6     0.6
#> 5    -1.1     4.6
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
rm(airquality)

## fcompute only returns the modified / computed columns
head(fcompute(airquality, Ozone = -Ozone))
#>   Ozone
#> 1   -41
#> 2   -36
#> 3   -12
#> 4   -18
#> 5    NA
#> 6   -28
head(fcompute(airquality, new = -Ozone, Temp = (Temp-32)/1.8))
#>   new     Temp
#> 1 -41 19.44444
#> 2 -36 22.22222
#> 3 -12 23.33333
#> 4 -18 16.66667
#> 5  NA 13.33333
#> 6 -28 18.88889
head(fcompute(airquality, new = -Ozone, new2 = 1))
#>   new new2
#> 1 -41    1
#> 2 -36    1
#> 3 -12    1
#> 4 -18    1
#> 5  NA    1
#> 6 -28    1

# Can preserve existing columns, computed ones are added to the right if names are different
head(fcompute(airquality, new = -Ozone, new2 = 1, keep = 1:3))
#>   Ozone Solar.R Wind new new2
#> 1    41     190  7.4 -41    1
#> 2    36     118  8.0 -36    1
#> 3    12     149 12.6 -12    1
#> 4    18     313 11.5 -18    1
#> 5    NA      NA 14.3  NA    1
#> 6    28      NA 14.9 -28    1

# If given same name as preserved columns, preserved columns are replaced in order...
head(fcompute(airquality, Ozone = -Ozone, new = 1, keep = 1:3))
#>   Ozone Solar.R Wind new
#> 1   -41     190  7.4   1
#> 2   -36     118  8.0   1
#> 3   -12     149 12.6   1
#> 4   -18     313 11.5   1
#> 5    NA      NA 14.3   1
#> 6   -28      NA 14.9   1

# Same holds for fcomputev
head(fcomputev(iris, is.numeric, log)) # Same as:
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1     1.629241    1.252763    0.3364722  -1.6094379
#> 2     1.589235    1.098612    0.3364722  -1.6094379
#> 3     1.547563    1.163151    0.2623643  -1.6094379
#> 4     1.526056    1.131402    0.4054651  -1.6094379
#> 5     1.609438    1.280934    0.3364722  -1.6094379
#> 6     1.686399    1.360977    0.5306283  -0.9162907
iris |> get_vars(is.numeric) |> dapply(log) |> head()
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1     1.629241    1.252763    0.3364722  -1.6094379
#> 2     1.589235    1.098612    0.3364722  -1.6094379
#> 3     1.547563    1.163151    0.2623643  -1.6094379
#> 4     1.526056    1.131402    0.4054651  -1.6094379
#> 5     1.609438    1.280934    0.3364722  -1.6094379
#> 6     1.686399    1.360977    0.5306283  -0.9162907

head(fcomputev(iris, is.numeric, log, keep = "Species"))   # Adds in front
#>   Species Sepal.Length Sepal.Width Petal.Length Petal.Width
#> 1  setosa     1.629241    1.252763    0.3364722  -1.6094379
#> 2  setosa     1.589235    1.098612    0.3364722  -1.6094379
#> 3  setosa     1.547563    1.163151    0.2623643  -1.6094379
#> 4  setosa     1.526056    1.131402    0.4054651  -1.6094379
#> 5  setosa     1.609438    1.280934    0.3364722  -1.6094379
#> 6  setosa     1.686399    1.360977    0.5306283  -0.9162907
head(fcomputev(iris, is.numeric, log, keep = names(iris))) # Preserve order
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width Species
#> 1     1.629241    1.252763    0.3364722  -1.6094379  setosa
#> 2     1.589235    1.098612    0.3364722  -1.6094379  setosa
#> 3     1.547563    1.163151    0.2623643  -1.6094379  setosa
#> 4     1.526056    1.131402    0.4054651  -1.6094379  setosa
#> 5     1.609438    1.280934    0.3364722  -1.6094379  setosa
#> 6     1.686399    1.360977    0.5306283  -0.9162907  setosa

# Keep a subset of the data, add standardized columns
head(fcomputev(iris, 3:4, STD, apply = FALSE, keep = names(iris)[3:5]))
#>   Petal.Length Petal.Width Species STD.Petal.Length STD.Petal.Width
#> 1          1.4         0.2  setosa        -1.335752       -1.311052
#> 2          1.4         0.2  setosa        -1.335752       -1.311052
#> 3          1.3         0.2  setosa        -1.392399       -1.311052
#> 4          1.5         0.2  setosa        -1.279104       -1.311052
#> 5          1.4         0.2  setosa        -1.335752       -1.311052
#> 6          1.7         0.4  setosa        -1.165809       -1.048667
```
