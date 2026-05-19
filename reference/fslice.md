# Fast Slicing of Matrix-Like Objects

A fast function to extract rows from a matrix or data frame-like object
(by groups).

## Usage

``` r
fslice(x, ..., n = 1, how = "first", order.by = NULL,
       na.rm = .op[["na.rm"]], sort = FALSE, with.ties = FALSE)

fslicev(x, cols = NULL, n = 1, how = "first", order.by = NULL,
        na.rm = .op[["na.rm"]], sort = FALSE, with.ties = FALSE, ...)
```

## Arguments

- x:

  a matrix, data frame or list-like object, including 'grouped_df'.

- ...:

  for `fslice`: names or sequences of columns to group by - passed to
  [`fselect`](https://fastverse.org/collapse/reference/select_replace_vars.md).
  If `x` is a matrix: atomic vectors to group `x`. Can be empty to
  operate on (un)grouped data. For `fslicev`: further arguments passed
  to [`GRP`](https://fastverse.org/collapse/reference/GRP.md) (such as
  `decreasing`, `na.last`, `method`).

- cols:

  select columns to group by, using column names, indices, a logical
  vector or a selector function (e.g. `is_categorical`). It can also be
  a list of vectors, or, if `x` is a matrix, a single vector.

- n:

  integer or proportion (if \< 1). Number of rows to select from each
  group. If a proportion is provided, it is converted to the equivalent
  number of rows using `max(1, round(n * nrow(x)))` or
  `max(1, round(n * nrow(x) / N.groups))` for grouped data.

- how:

  character. Method to select rows. One of:

  - `"first"`: select first `n` rows

  - `"last"`: select last `n` rows

  - `"min"`: select `n` rows with minimum values of `order.by`

  - `"max"`: select `n` rows with maximum values of `order.by`

- order.by:

  vector or column name to order by when `how` is `"min"` or `"max"`.
  Must be same length as rows in `x`. In `fslice` it must not be quoted.

- na.rm:

  logical. If `TRUE`, missing values in `order.by` are removed before
  selecting rows.

- sort:

  logical. If `TRUE`, sort selected rows on the grouping columns.
  `FALSE` uses first-appearance order (including grouping columns if
  `how` is `"first"` or `"last"`) - fastest.

- with.ties:

  logical. If `TRUE` and `how` is `"min"` or `"max"`, returns all rows
  with the extreme value. Currently only supported for `n = 1` and
  `sort = FALSE`.

## Value

A subset of `x` containing the selected rows.

## See also

[`fsubset`](https://fastverse.org/collapse/reference/fsubset.md),
[`fcount`](https://fastverse.org/collapse/reference/fcount.md), [Data
Frame
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
# Basic usage
fslice(mtcars, n = 3)                    # First 3 rows
#>                mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4     21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> Datsun 710    22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
fslice(mtcars, n = 3, how = "last")      # Last 3 rows
#>                mpg cyl disp  hp drat   wt qsec vs am gear carb
#> Ferrari Dino  19.7   6  145 175 3.62 2.77 15.5  0  1    5    6
#> Maserati Bora 15.0   8  301 335 3.54 3.57 14.6  0  1    5    8
#> Volvo 142E    21.4   4  121 109 4.11 2.78 18.6  1  1    4    2
fslice(mtcars, n = 0.1)                  # First 10% of rows
#>                mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4     21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> Datsun 710    22.8   4  108  93 3.85 2.320 18.61  1  1    4    1

# Using order.by
fslice(mtcars, n = 3, how = "min", order.by = mpg)  # 3 cars with lowest mpg
#>                      mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Cadillac Fleetwood  10.4   8  472 205 2.93 5.250 17.98  0  0    3    4
#> Lincoln Continental 10.4   8  460 215 3.00 5.424 17.82  0  0    3    4
#> Camaro Z28          13.3   8  350 245 3.73 3.840 15.41  0  0    3    4
fslice(mtcars, n = 3, how = "max", order.by = mpg)  # 3 cars with highest mpg
#>                 mpg cyl disp hp drat    wt  qsec vs am gear carb
#> Toyota Corolla 33.9   4 71.1 65 4.22 1.835 19.90  1  1    4    1
#> Fiat 128       32.4   4 78.7 66 4.08 2.200 19.47  1  1    4    1
#> Honda Civic    30.4   4 75.7 52 4.93 1.615 18.52  1  1    4    2

# With grouping
mtcars |> fslice(cyl, n = 2)                        # First 2 cars per cylinder
#>                    mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6 160.0 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag     21.0   6 160.0 110 3.90 2.875 17.02  0  1    4    4
#> Datsun 710        22.8   4 108.0  93 3.85 2.320 18.61  1  1    4    1
#> Hornet Sportabout 18.7   8 360.0 175 3.15 3.440 17.02  0  0    3    2
#> Duster 360        14.3   8 360.0 245 3.21 3.570 15.84  0  0    3    4
#> Merc 240D         24.4   4 146.7  62 3.69 3.190 20.00  1  0    4    2
mtcars |> fslice(cyl, n = 2, sort = TRUE)           # with sorting (slightly less efficient)
#>                    mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Datsun 710        22.8   4 108.0  93 3.85 2.320 18.61  1  1    4    1
#> Merc 240D         24.4   4 146.7  62 3.69 3.190 20.00  1  0    4    2
#> Mazda RX4         21.0   6 160.0 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag     21.0   6 160.0 110 3.90 2.875 17.02  0  1    4    4
#> Hornet Sportabout 18.7   8 360.0 175 3.15 3.440 17.02  0  0    3    2
#> Duster 360        14.3   8 360.0 245 3.21 3.570 15.84  0  0    3    4
mtcars |> fslice(cyl, n = 2, how = "min", order.by = mpg)  # 2 lowest mpg cars per cylinder
#>                      mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Merc 280C           17.8   6 167.6 123 3.92 3.440 18.90  1  0    4    4
#> Valiant             18.1   6 225.0 105 2.76 3.460 20.22  1  0    3    1
#> Volvo 142E          21.4   4 121.0 109 4.11 2.780 18.60  1  1    4    2
#> Toyota Corona       21.5   4 120.1  97 3.70 2.465 20.01  1  0    3    1
#> Cadillac Fleetwood  10.4   8 472.0 205 2.93 5.250 17.98  0  0    3    4
#> Lincoln Continental 10.4   8 460.0 215 3.00 5.424 17.82  0  0    3    4

# Using with.ties
mtcars |> fslice(cyl, n = 1, how = "min", order.by = mpg, with.ties = TRUE)
#>                      mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Merc 280C           17.8   6 167.6 123 3.92 3.440 18.90  1  0    4    4
#> Cadillac Fleetwood  10.4   8 472.0 205 2.93 5.250 17.98  0  0    3    4
#> Lincoln Continental 10.4   8 460.0 215 3.00 5.424 17.82  0  0    3    4
#> Volvo 142E          21.4   4 121.0 109 4.11 2.780 18.60  1  1    4    2

# With grouped data
mtcars |>
  fgroup_by(cyl) |>
  fslice(n = 2, how = "max", order.by = mpg)        # 2 highest mpg cars per cylinder
#>                    mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Toyota Corolla    33.9   4  71.1  65 4.22 1.835 19.90  1  1    4    1
#> Fiat 128          32.4   4  78.7  66 4.08 2.200 19.47  1  1    4    1
#> Hornet 4 Drive    21.4   6 258.0 110 3.08 3.215 19.44  1  0    3    1
#> Mazda RX4         21.0   6 160.0 110 3.90 2.620 16.46  0  1    4    4
#> Pontiac Firebird  19.2   8 400.0 175 3.08 3.845 17.05  0  0    3    2
#> Hornet Sportabout 18.7   8 360.0 175 3.15 3.440 17.02  0  0    3    2
```
