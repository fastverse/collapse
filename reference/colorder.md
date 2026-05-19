# Fast Reordering of Data Frame Columns

Efficiently reorder columns in a data frame. To do this fully by
reference see also
[`data.table::setcolorder`](https://rdrr.io/pkg/data.table/man/setcolorder.html).

## Usage

``` r
colorder(.X, ..., pos = "front")

colorderv(X, neworder = radixorder(names(X)),
          pos = "front", regex = FALSE, ...)
```

## Arguments

- .X, X:

  a data frame or list.

- ...:

  for `colorder`: Column names of `.X` in the new order (can also use
  sequences i.e. `col1:coln, newname = colk, ...`). For `colorderv`:
  Further arguments to [`grep`](https://rdrr.io/r/base/grep.html) if
  `regex = TRUE`.

- neworder:

  a vector of column names, positive indices, a suitable logical vector,
  a function such as `is.numeric`, or a vector of regular expressions
  matching column names (if `regex = TRUE`).

- pos:

  integer or character. Different options regarding column arrangement
  if `...length() < ncol(.X)` (or `length(neworder) < ncol(X)`).

  |  |  |  |  |  |
  |----|----|----|----|----|
  | *Int.* |  | *String* |  | *Description* |
  | 1 |  | "front" |  | move specified columns to the front (the default). |
  | 2 |  | "end" |  | move specified columns to the end. |
  | 3 |  | "exchange" |  | just exchange the positions of selected columns, other columns remain in the same position. |
  | 4 |  | "after" |  | place all further selected columns behind the first selected column. |

- regex:

  logical. `TRUE` will do regular expression search on the column names
  of `X` using a (vector of) regular expression(s) passed to `neworder`.
  Matching is done using [`grep`](https://rdrr.io/r/base/grep.html).
  *Note* that multiple regular expressions will be matched in the order
  they are passed, and
  [`funique`](https://fastverse.org/collapse/reference/funique.md) will
  be applied to the resulting set of indices.

## Value

`.X/X` with columns reordered (no deep copies).

## See also

[`roworder`](https://fastverse.org/collapse/reference/roworder.md),
[Data Frame
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
head(colorder(mtcars, vs, cyl:hp, am))
#>                   vs cyl disp  hp am  mpg drat    wt  qsec gear carb
#> Mazda RX4          0   6  160 110  1 21.0 3.90 2.620 16.46    4    4
#> Mazda RX4 Wag      0   6  160 110  1 21.0 3.90 2.875 17.02    4    4
#> Datsun 710         1   4  108  93  1 22.8 3.85 2.320 18.61    4    1
#> Hornet 4 Drive     1   6  258 110  0 21.4 3.08 3.215 19.44    3    1
#> Hornet Sportabout  0   8  360 175  0 18.7 3.15 3.440 17.02    3    2
#> Valiant            1   6  225 105  0 18.1 2.76 3.460 20.22    3    1
head(colorder(mtcars, vs, cyl:hp, am, pos = "end"))
#>                    mpg drat    wt  qsec gear carb vs cyl disp  hp am
#> Mazda RX4         21.0 3.90 2.620 16.46    4    4  0   6  160 110  1
#> Mazda RX4 Wag     21.0 3.90 2.875 17.02    4    4  0   6  160 110  1
#> Datsun 710        22.8 3.85 2.320 18.61    4    1  1   4  108  93  1
#> Hornet 4 Drive    21.4 3.08 3.215 19.44    3    1  1   6  258 110  0
#> Hornet Sportabout 18.7 3.15 3.440 17.02    3    2  0   8  360 175  0
#> Valiant           18.1 2.76 3.460 20.22    3    1  1   6  225 105  0
head(colorder(mtcars, vs, cyl:hp, am, pos = "after"))
#>                    mpg drat    wt  qsec vs cyl disp  hp am gear carb
#> Mazda RX4         21.0 3.90 2.620 16.46  0   6  160 110  1    4    4
#> Mazda RX4 Wag     21.0 3.90 2.875 17.02  0   6  160 110  1    4    4
#> Datsun 710        22.8 3.85 2.320 18.61  1   4  108  93  1    4    1
#> Hornet 4 Drive    21.4 3.08 3.215 19.44  1   6  258 110  0    3    1
#> Hornet Sportabout 18.7 3.15 3.440 17.02  0   8  360 175  0    3    2
#> Valiant           18.1 2.76 3.460 20.22  1   6  225 105  0    3    1
head(colorder(mtcars, vs, cyl, pos = "exchange"))
#>                    mpg vs disp  hp drat    wt  qsec cyl am gear carb
#> Mazda RX4         21.0  0  160 110 3.90 2.620 16.46   6  1    4    4
#> Mazda RX4 Wag     21.0  0  160 110 3.90 2.875 17.02   6  1    4    4
#> Datsun 710        22.8  1  108  93 3.85 2.320 18.61   4  1    4    1
#> Hornet 4 Drive    21.4  1  258 110 3.08 3.215 19.44   6  0    3    1
#> Hornet Sportabout 18.7  0  360 175 3.15 3.440 17.02   8  0    3    2
#> Valiant           18.1  1  225 105 2.76 3.460 20.22   6  0    3    1
head(colorder(mtcars, vs, cyl:hp, new = am))    # renaming
#>                   vs cyl disp  hp new  mpg drat    wt  qsec gear carb
#> Mazda RX4          0   6  160 110   1 21.0 3.90 2.620 16.46    4    4
#> Mazda RX4 Wag      0   6  160 110   1 21.0 3.90 2.875 17.02    4    4
#> Datsun 710         1   4  108  93   1 22.8 3.85 2.320 18.61    4    1
#> Hornet 4 Drive     1   6  258 110   0 21.4 3.08 3.215 19.44    3    1
#> Hornet Sportabout  0   8  360 175   0 18.7 3.15 3.440 17.02    3    2
#> Valiant            1   6  225 105   0 18.1 2.76 3.460 20.22    3    1

## Same in standard evaluation
head(colorderv(mtcars, c(8, 2:4, 9)))
#>                   vs cyl disp  hp am  mpg drat    wt  qsec gear carb
#> Mazda RX4          0   6  160 110  1 21.0 3.90 2.620 16.46    4    4
#> Mazda RX4 Wag      0   6  160 110  1 21.0 3.90 2.875 17.02    4    4
#> Datsun 710         1   4  108  93  1 22.8 3.85 2.320 18.61    4    1
#> Hornet 4 Drive     1   6  258 110  0 21.4 3.08 3.215 19.44    3    1
#> Hornet Sportabout  0   8  360 175  0 18.7 3.15 3.440 17.02    3    2
#> Valiant            1   6  225 105  0 18.1 2.76 3.460 20.22    3    1
head(colorderv(mtcars, c(8, 2:4, 9), pos = "end"))
#>                    mpg drat    wt  qsec gear carb vs cyl disp  hp am
#> Mazda RX4         21.0 3.90 2.620 16.46    4    4  0   6  160 110  1
#> Mazda RX4 Wag     21.0 3.90 2.875 17.02    4    4  0   6  160 110  1
#> Datsun 710        22.8 3.85 2.320 18.61    4    1  1   4  108  93  1
#> Hornet 4 Drive    21.4 3.08 3.215 19.44    3    1  1   6  258 110  0
#> Hornet Sportabout 18.7 3.15 3.440 17.02    3    2  0   8  360 175  0
#> Valiant           18.1 2.76 3.460 20.22    3    1  1   6  225 105  0
head(colorderv(mtcars, c(8, 2:4, 9), pos = "after"))
#>                    mpg drat    wt  qsec vs cyl disp  hp am gear carb
#> Mazda RX4         21.0 3.90 2.620 16.46  0   6  160 110  1    4    4
#> Mazda RX4 Wag     21.0 3.90 2.875 17.02  0   6  160 110  1    4    4
#> Datsun 710        22.8 3.85 2.320 18.61  1   4  108  93  1    4    1
#> Hornet 4 Drive    21.4 3.08 3.215 19.44  1   6  258 110  0    3    1
#> Hornet Sportabout 18.7 3.15 3.440 17.02  0   8  360 175  0    3    2
#> Valiant           18.1 2.76 3.460 20.22  1   6  225 105  0    3    1
head(colorderv(mtcars, c(8, 2), pos = "exchange"))
#>                    mpg vs disp  hp drat    wt  qsec cyl am gear carb
#> Mazda RX4         21.0  0  160 110 3.90 2.620 16.46   6  1    4    4
#> Mazda RX4 Wag     21.0  0  160 110 3.90 2.875 17.02   6  1    4    4
#> Datsun 710        22.8  1  108  93 3.85 2.320 18.61   4  1    4    1
#> Hornet 4 Drive    21.4  1  258 110 3.08 3.215 19.44   6  0    3    1
#> Hornet Sportabout 18.7  0  360 175 3.15 3.440 17.02   8  0    3    2
#> Valiant           18.1  1  225 105 2.76 3.460 20.22   6  0    3    1
```
