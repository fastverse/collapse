# Recode and Replace Values in Matrix-Like Objects

A small suite of functions to efficiently perform common recoding and
replacing tasks in matrix-like objects.

## Usage

``` r
recode_num(X, ..., default = NULL, missing = NULL, set = FALSE)

recode_char(X, ..., default = NULL, missing = NULL, regex = FALSE,
            ignore.case = FALSE, fixed = FALSE, set = FALSE)

replace_na(X, value = 0, cols = NULL, set = FALSE, type = "const")

replace_inf(X, value = NA, replace.nan = FALSE, set = FALSE)

replace_outliers(X, limits, value = NA,
                 single.limit = c("sd", "mad", "min", "max"),
                 ignore.groups = FALSE, set = FALSE)
```

## Arguments

- X:

  a vector, matrix, array, data frame or list of atomic objects.
  `replace_outliers` has internal methods for
  [grouped](https://fastverse.org/collapse/reference/GRP.md) and
  [indexed](https://fastverse.org/collapse/reference/indexing.md) data.

- ...:

  comma-separated recode arguments of the form:
  `` value = replacement, `2` = 0, Secondary = "SEC" `` etc.
  `recode_char` with `regex = TRUE` also supports regular expressions
  i.e. `` `^S|D$` = "STD" `` etc.

- default:

  optional argument to specify a scalar value to replace non-matched
  elements with.

- missing:

  optional argument to specify a scalar value to replace missing
  elements with. *Note* that to increase efficiency this is done before
  the rest of the recoding i.e. the recoding is performed on data where
  missing values are filled!

- set:

  logical. `TRUE` does replacements by reference (i.e. in-place
  modification of the data) and returns the result invisibly.

- type:

  character. One of `"const"`, `"locf"` (last non-missing observation
  carried forward) or `"focb"` (first non-missing observation carried
  back). The latter two ignore `value`.

- regex:

  logical. If `TRUE`, all recode-argument names are (sequentially)
  passed to [`grepl`](https://rdrr.io/r/base/grep.html) as a pattern to
  search `X`. All matches are replaced. *Note* that `NA`'s are also
  matched as strings by `grepl`.

- value:

  a single (scalar) value to replace matching elements with. In
  `replace_outliers` setting `value = "clip"` will replace outliers with
  the corresponding threshold values. See Examples.

- cols:

  select columns to replace missing values in using a function, column
  names, indices or a logical vector.

- replace.nan:

  logical. `TRUE` replaces `NaN/Inf/-Inf`. `FALSE` (default) replaces
  only `Inf/-Inf`.

- limits:

  either a vector of two-numeric values `c(minval, maxval)` constituting
  a two-sided outlier threshold, or a single numeric value:

- single.limit:

  character, controls the behavior if `length(limits) == 1`:

  - `"sd"/"mad":` `limits` will be interpreted as a (two-sided) outlier
    threshold in terms of (column) standard deviations/median absolute
    deviations. For the standard deviation this is equivalent to
    `X[abs(fscale(X)) > limits] <- value`. Since `fscale` is S3 generic
    with methods for 'grouped_df', 'pseries' and 'pdata.frame', the
    standardizing will be grouped if such objects are passed (i.e. the
    outlier threshold is then measured in within-group standard
    deviations) unless `ignore.groups = TRUE`. The same holds for median
    absolute deviations.

  - `"min"/"max":` `limits` will be interpreted as a (one-sided)
    minimum/maximum threshold. The underlying code is equivalent to
    `X[X </> limits] <- value`.

- ignore.groups:

  logical. If `length(limits) == 1` and
  `single.limit %in% c("sd", "mad")` and `X` is a 'grouped_df',
  'pseries' or 'pdata.frame', `TRUE` will ignore the grouped nature of
  the data and calculate outlier thresholds on the entire dataset rather
  than within each group.

- ignore.case, fixed:

  logical. Passed to [`grepl`](https://rdrr.io/r/base/grep.html) and
  only applicable if `regex = TRUE`.

## Details

- `recode_num` and `recode_char` can be used to efficiently recode
  multiple numeric or character values, respectively. The syntax is
  inspired by
  [`dplyr::recode`](https://dplyr.tidyverse.org/reference/recode.html),
  but the functionality is enhanced in the following respects: (1) when
  passed a data frame / list, all appropriately typed columns will be
  recoded. (2) They preserve the attributes of the data object and of
  columns in a data frame / list, and (3) `recode_char` also supports
  regular expression matching using
  [`grepl`](https://rdrr.io/r/base/grep.html).

- `replace_na` efficiently replaces `NA/NaN` with a value (default is
  `0`). data can be multi-typed, in which case appropriate columns can
  be selected through the `cols` argument. For numeric data a more
  versatile alternative is provided by
  [`data.table::nafill`](https://rdrr.io/pkg/data.table/man/nafill.html)
  and
  [`data.table::setnafill`](https://rdrr.io/pkg/data.table/man/nafill.html).

- `replace_inf` replaces `Inf/-Inf` (or optionally `NaN/Inf/-Inf`) with
  a value (default is `NA`). It skips non-numeric columns in a data
  frame.

- `replace_outliers` replaces values falling outside a 1- or 2-sided
  numeric threshold or outside a certain number of standard deviations
  or median absolute deviation with a value (default is `NA`). It skips
  non-numeric columns in a data frame.

## Note

These functions are not generic and do not offer support for factors or
date(-time) objects. see
[`dplyr::recode_factor`](https://dplyr.tidyverse.org/reference/recode.html),
*forcats* and other appropriate packages for dealing with these classes.

Simple replacing tasks on a vector can also effectively be handled by,
[`setv`](https://fastverse.org/collapse/reference/efficient-programming.md)
/
[`copyv`](https://fastverse.org/collapse/reference/efficient-programming.md).
Fast vectorized switches are offered by package *kit* (functions `iif`,
`nif`, `vswitch`, `nswitch`) as well as
[`data.table::fcase`](https://rdrr.io/pkg/data.table/man/fcase.html) and
[`data.table::fifelse`](https://rdrr.io/pkg/data.table/man/fifelse.html).
Using switches is more efficient than `recode_*`, as `recode_*` creates
an internal copy of the object to enable cross-replacing.

Function [`TRA`](https://fastverse.org/collapse/reference/TRA.md), and
the associated `TRA` ('transform') argument to [Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
also has option `"replace_na"`, to replace missing values with a
statistic computed on the non-missing observations, e.g.
`fmedian(airquality, TRA = "replace_na")` does median imputation.

## See also

[`pad`](https://fastverse.org/collapse/reference/pad.md), [Efficient
Programming](https://fastverse.org/collapse/reference/efficient-programming.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
recode_char(c("a","b","c"), a = "b", b = "c")
#> [1] "b" "c" "c"
recode_char(month.name, ber = NA, regex = TRUE)
#>  [1] "January"  "February" "March"    "April"    "May"      "June"    
#>  [7] "July"     "August"   NA         NA         NA         NA        
mtcr <- recode_num(mtcars, `0` = 2, `4` = Inf, `1` = NaN)
replace_inf(mtcr)
#>                    mpg cyl disp  hp drat    wt  qsec  vs  am gear carb
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46   2 NaN   NA   NA
#> Mazda RX4 Wag     21.0   6  160 110 3.90 2.875 17.02   2 NaN   NA   NA
#> Datsun 710        22.8  NA  108  93 3.85 2.320 18.61 NaN NaN   NA  NaN
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44 NaN   2    3  NaN
#> Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02   2   2    3    2
#> Valiant           18.1   6  225 105 2.76 3.460 20.22 NaN   2    3  NaN
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
replace_inf(mtcr, replace.nan = TRUE)
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46  2 NA   NA   NA
#> Mazda RX4 Wag     21.0   6  160 110 3.90 2.875 17.02  2 NA   NA   NA
#> Datsun 710        22.8  NA  108  93 3.85 2.320 18.61 NA NA   NA   NA
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44 NA  2    3   NA
#> Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  2  2    3    2
#> Valiant           18.1   6  225 105 2.76 3.460 20.22 NA  2    3   NA
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
replace_outliers(mtcars, c(2, 100))                 # Replace all values below 2 and above 100 w. NA
#>                    mpg cyl disp hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6   NA NA 3.90 2.620 16.46 NA NA    4    4
#> Mazda RX4 Wag     21.0   6   NA NA 3.90 2.875 17.02 NA NA    4    4
#> Datsun 710        22.8   4   NA 93 3.85 2.320 18.61 NA NA    4   NA
#> Hornet 4 Drive    21.4   6   NA NA 3.08 3.215 19.44 NA NA    3   NA
#> Hornet Sportabout 18.7   8   NA NA 3.15 3.440 17.02 NA NA    3    2
#> Valiant           18.1   6   NA NA 2.76 3.460 20.22 NA NA    3   NA
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
replace_outliers(mtcars, c(2, 100), value = "clip") # Clipping outliers to the thresholds
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6  100 100 3.90 2.620 16.46  2  2    4    4
#> Mazda RX4 Wag     21.0   6  100 100 3.90 2.875 17.02  2  2    4    4
#> Datsun 710        22.8   4  100  93 3.85 2.320 18.61  2  2    4    2
#> Hornet 4 Drive    21.4   6  100 100 3.08 3.215 19.44  2  2    3    2
#> Hornet Sportabout 18.7   8  100 100 3.15 3.440 17.02  2  2    3    2
#> Valiant           18.1   6  100 100 2.76 3.460 20.22  2  2    3    2
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
replace_outliers(mtcars, 2, single.limit = "min")   # Replace all value smaller than 2 with NA
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46 NA NA    4    4
#> Mazda RX4 Wag     21.0   6  160 110 3.90 2.875 17.02 NA NA    4    4
#> Datsun 710        22.8   4  108  93 3.85 2.320 18.61 NA NA    4   NA
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44 NA NA    3   NA
#> Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02 NA NA    3    2
#> Valiant           18.1   6  225 105 2.76 3.460 20.22 NA NA    3   NA
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
replace_outliers(mtcars, 100, single.limit = "max") # Replace all value larger than 100 with NA
#>                    mpg cyl disp hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6   NA NA 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag     21.0   6   NA NA 3.90 2.875 17.02  0  1    4    4
#> Datsun 710        22.8   4   NA 93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive    21.4   6   NA NA 3.08 3.215 19.44  1  0    3    1
#> Hornet Sportabout 18.7   8   NA NA 3.15 3.440 17.02  0  0    3    2
#> Valiant           18.1   6   NA NA 2.76 3.460 20.22  1  0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
replace_outliers(mtcars, 2)                         # Replace all values above or below 2 column-
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag     21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> Datsun 710        22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#> Valiant           18.1   6  225 105 2.76 3.460 20.22  1  0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
                                                    # standard-deviations from the column-mean w. NA
replace_outliers(fgroup_by(iris, Species), 2)       # Passing a grouped_df, pseries or pdata.frame
#>    Sepal.Length Sepal.Width Petal.Length Petal.Width Species
#> 1           5.1         3.5          1.4         0.2  setosa
#> 2           4.9         3.0          1.4         0.2  setosa
#> 3           4.7         3.2          1.3         0.2  setosa
#> 4           4.6         3.1          1.5         0.2  setosa
#> 5           5.0         3.6          1.4         0.2  setosa
#> 6           5.4         3.9          1.7         0.4  setosa
#> 7           4.6         3.4          1.4         0.3  setosa
#> 8           5.0         3.4          1.5         0.2  setosa
#> 9           4.4         2.9          1.4         0.2  setosa
#> 10          4.9         3.1          1.5         0.1  setosa
#> 11          5.4         3.7          1.5         0.2  setosa
#> 12          4.8         3.4          1.6         0.2  setosa
#> 13          4.8         3.0          1.4         0.1  setosa
#> 14           NA         3.0           NA         0.1  setosa
#>  [ reached 'max' / getOption("max.print") -- omitted 136 rows ]
#> 
#> Grouped by:  Species  [3 | 50 (0)] 
                                                    # allows to remove outliers according to
                                                    # in-group standard-deviation. see ?fscale
```
