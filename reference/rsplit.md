# Fast (Recursive) Splitting

`rsplit` (recursively) splits a vector, matrix or data frame into
subsets according to combinations of (multiple) vectors / factors and
returns a (nested) list. If `flatten = TRUE`, the list is flattened
yielding the same result as
[`split`](https://rdrr.io/pkg/data.table/man/split.html). `rsplit` is
implemented as a wrapper around
[`gsplit`](https://fastverse.org/collapse/reference/GRP.md), and
significantly faster than
[`split`](https://rdrr.io/pkg/data.table/man/split.html).

## Usage

``` r
rsplit(x, ...)

# Default S3 method
rsplit(x, fl, drop = TRUE, flatten = FALSE, use.names = TRUE, ...)

# S3 method for class 'matrix'
rsplit(x, fl, drop = TRUE, flatten = FALSE, use.names = TRUE,
       drop.dim = FALSE, ...)

# S3 method for class 'data.frame'
rsplit(x, by, drop = TRUE, flatten = FALSE, cols = NULL,
       keep.by = FALSE, simplify = TRUE, use.names = TRUE, ...)
```

## Arguments

- x:

  a vector, matrix, data.frame or list like object.

- fl:

  a [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object, or
  a (list of) vector(s) / factor(s) (internally converted to a
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md) object(s))
  used to split `x`.

- by:

  *data.frame method*: Same as `fl`, but also allows one- or two-sided
  formulas i.e. `~ group1` or `var1 + var2 ~ group1 + group2`. See
  Examples.

- drop:

  logical. `TRUE` removes unused levels or combinations of levels from
  factors before splitting; `FALSE` retains those combinations yielding
  empty list elements in the output.

- flatten:

  logical. If `fl` is a list of vectors / factors, `TRUE` calls
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md) on the list,
  creating a single grouping used for splitting; `FALSE` yields
  recursive splitting.

- use.names:

  logical. `TRUE` returns a named list (like
  [`split`](https://rdrr.io/pkg/data.table/man/split.html)); `FALSE`
  returns a plain list.

- drop.dim:

  logical. `TRUE` returns atomic vectors for matrix-splits consisting of
  one row.

- cols:

  *data.frame method*: Select columns to split using a function, column
  names, indices or a logical vector. *Note*: `cols` is ignored if a
  two-sided formula is passed to `by`.

- keep.by:

  logical. If a formula is passed to `by`, then `TRUE` preserves the
  splitting (right-hand-side) variables in the data frame.

- simplify:

  *data.frame method*: Logical. `TRUE` calls `rsplit.default` if a
  single column is split e.g. `rsplit(data, col1 ~ group1)` becomes the
  same as `rsplit(data$col1, data$group1)`.

- ...:

  further arguments passed to
  [`GRP`](https://fastverse.org/collapse/reference/GRP.md). Sensible
  choices would be `sort = FALSE`, `decreasing = TRUE` or
  `na.last = FALSE`. Note that these options only apply if `fl` is not
  already a (list of) factor(s).

## Value

a (nested) list containing the subsets of `x`.

## See also

[`gsplit`](https://fastverse.org/collapse/reference/GRP.md),
[`rapply2d`](https://fastverse.org/collapse/reference/rapply2d.md),
[`unlist2d`](https://fastverse.org/collapse/reference/unlist2d.md),
[List
Processing](https://fastverse.org/collapse/reference/list-processing.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
rsplit(mtcars$mpg, mtcars$cyl)
#> $`4`
#>  [1] 22.8 24.4 22.8 32.4 30.4 33.9 21.5 27.3 26.0 30.4 21.4
#> 
#> $`6`
#> [1] 21.0 21.0 21.4 18.1 19.2 17.8 19.7
#> 
#> $`8`
#>  [1] 18.7 14.3 16.4 17.3 15.2 10.4 10.4 14.7 15.5 15.2 13.3 19.2 15.8 15.0
#> 
rsplit(mtcars, mtcars$cyl)
#> $`4`
#>                 mpg cyl  disp hp drat    wt  qsec vs am gear carb
#> Datsun 710     22.8   4 108.0 93 3.85 2.320 18.61  1  1    4    1
#> Merc 240D      24.4   4 146.7 62 3.69 3.190 20.00  1  0    4    2
#> Merc 230       22.8   4 140.8 95 3.92 3.150 22.90  1  0    4    2
#> Fiat 128       32.4   4  78.7 66 4.08 2.200 19.47  1  1    4    1
#> Honda Civic    30.4   4  75.7 52 4.93 1.615 18.52  1  1    4    2
#> Toyota Corolla 33.9   4  71.1 65 4.22 1.835 19.90  1  1    4    1
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
#> 
#> $`6`
#>                 mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4      21.0   6 160.0 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag  21.0   6 160.0 110 3.90 2.875 17.02  0  1    4    4
#> Hornet 4 Drive 21.4   6 258.0 110 3.08 3.215 19.44  1  0    3    1
#> Valiant        18.1   6 225.0 105 2.76 3.460 20.22  1  0    3    1
#> Merc 280       19.2   6 167.6 123 3.92 3.440 18.30  1  0    4    4
#> Merc 280C      17.8   6 167.6 123 3.92 3.440 18.90  1  0    4    4
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
#> 
#> $`8`
#>                     mpg cyl  disp  hp drat   wt  qsec vs am gear carb
#> Hornet Sportabout  18.7   8 360.0 175 3.15 3.44 17.02  0  0    3    2
#> Duster 360         14.3   8 360.0 245 3.21 3.57 15.84  0  0    3    4
#> Merc 450SE         16.4   8 275.8 180 3.07 4.07 17.40  0  0    3    3
#> Merc 450SL         17.3   8 275.8 180 3.07 3.73 17.60  0  0    3    3
#> Merc 450SLC        15.2   8 275.8 180 3.07 3.78 18.00  0  0    3    3
#> Cadillac Fleetwood 10.4   8 472.0 205 2.93 5.25 17.98  0  0    3    4
#>  [ reached 'max' / getOption("max.print") -- omitted 8 rows ]
#> 

rsplit(mtcars, mtcars[.c(cyl, vs, am)])
#> $`4`
#> $`4`$`0`
#> $`4`$`0`$`1`
#>               mpg cyl  disp hp drat   wt qsec vs am gear carb
#> Porsche 914-2  26   4 120.3 91 4.43 2.14 16.7  0  1    5    2
#> 
#> 
#> $`4`$`1`
#> $`4`$`1`$`0`
#>                mpg cyl  disp hp drat    wt  qsec vs am gear carb
#> Merc 240D     24.4   4 146.7 62 3.69 3.190 20.00  1  0    4    2
#> Merc 230      22.8   4 140.8 95 3.92 3.150 22.90  1  0    4    2
#> Toyota Corona 21.5   4 120.1 97 3.70 2.465 20.01  1  0    3    1
#> 
#> $`4`$`1`$`1`
#>                 mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Datsun 710     22.8   4 108.0  93 3.85 2.320 18.61  1  1    4    1
#> Fiat 128       32.4   4  78.7  66 4.08 2.200 19.47  1  1    4    1
#> Honda Civic    30.4   4  75.7  52 4.93 1.615 18.52  1  1    4    2
#> Toyota Corolla 33.9   4  71.1  65 4.22 1.835 19.90  1  1    4    1
#> Fiat X1-9      27.3   4  79.0  66 4.08 1.935 18.90  1  1    4    1
#> Lotus Europa   30.4   4  95.1 113 3.77 1.513 16.90  1  1    5    2
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
#> 
#> 
#> 
#> $`6`
#> $`6`$`0`
#> $`6`$`0`$`1`
#>                mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4     21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> Ferrari Dino  19.7   6  145 175 3.62 2.770 15.50  0  1    5    6
#> 
#> 
#> $`6`$`1`
#> $`6`$`1`$`0`
#>                 mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Hornet 4 Drive 21.4   6 258.0 110 3.08 3.215 19.44  1  0    3    1
#> Valiant        18.1   6 225.0 105 2.76 3.460 20.22  1  0    3    1
#> Merc 280       19.2   6 167.6 123 3.92 3.440 18.30  1  0    4    4
#> Merc 280C      17.8   6 167.6 123 3.92 3.440 18.90  1  0    4    4
#> 
#> 
#> 
#> $`8`
#> $`8`$`0`
#> $`8`$`0`$`0`
#>                     mpg cyl  disp  hp drat   wt  qsec vs am gear carb
#> Hornet Sportabout  18.7   8 360.0 175 3.15 3.44 17.02  0  0    3    2
#> Duster 360         14.3   8 360.0 245 3.21 3.57 15.84  0  0    3    4
#> Merc 450SE         16.4   8 275.8 180 3.07 4.07 17.40  0  0    3    3
#> Merc 450SL         17.3   8 275.8 180 3.07 3.73 17.60  0  0    3    3
#> Merc 450SLC        15.2   8 275.8 180 3.07 3.78 18.00  0  0    3    3
#> Cadillac Fleetwood 10.4   8 472.0 205 2.93 5.25 17.98  0  0    3    4
#>  [ reached 'max' / getOption("max.print") -- omitted 6 rows ]
#> 
#> $`8`$`0`$`1`
#>                 mpg cyl disp  hp drat   wt qsec vs am gear carb
#> Ford Pantera L 15.8   8  351 264 4.22 3.17 14.5  0  1    5    4
#> Maserati Bora  15.0   8  301 335 3.54 3.57 14.6  0  1    5    8
#> 
#> 
#> 
rsplit(mtcars, ~ cyl + vs + am, keep.by = TRUE)  # Same thing
#> $`4`
#> $`4`$`0`
#> $`4`$`0`$`1`
#>               mpg cyl  disp hp drat   wt qsec vs am gear carb
#> Porsche 914-2  26   4 120.3 91 4.43 2.14 16.7  0  1    5    2
#> 
#> 
#> $`4`$`1`
#> $`4`$`1`$`0`
#>                mpg cyl  disp hp drat    wt  qsec vs am gear carb
#> Merc 240D     24.4   4 146.7 62 3.69 3.190 20.00  1  0    4    2
#> Merc 230      22.8   4 140.8 95 3.92 3.150 22.90  1  0    4    2
#> Toyota Corona 21.5   4 120.1 97 3.70 2.465 20.01  1  0    3    1
#> 
#> $`4`$`1`$`1`
#>                 mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Datsun 710     22.8   4 108.0  93 3.85 2.320 18.61  1  1    4    1
#> Fiat 128       32.4   4  78.7  66 4.08 2.200 19.47  1  1    4    1
#> Honda Civic    30.4   4  75.7  52 4.93 1.615 18.52  1  1    4    2
#> Toyota Corolla 33.9   4  71.1  65 4.22 1.835 19.90  1  1    4    1
#> Fiat X1-9      27.3   4  79.0  66 4.08 1.935 18.90  1  1    4    1
#> Lotus Europa   30.4   4  95.1 113 3.77 1.513 16.90  1  1    5    2
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
#> 
#> 
#> 
#> $`6`
#> $`6`$`0`
#> $`6`$`0`$`1`
#>                mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4     21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> Ferrari Dino  19.7   6  145 175 3.62 2.770 15.50  0  1    5    6
#> 
#> 
#> $`6`$`1`
#> $`6`$`1`$`0`
#>                 mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Hornet 4 Drive 21.4   6 258.0 110 3.08 3.215 19.44  1  0    3    1
#> Valiant        18.1   6 225.0 105 2.76 3.460 20.22  1  0    3    1
#> Merc 280       19.2   6 167.6 123 3.92 3.440 18.30  1  0    4    4
#> Merc 280C      17.8   6 167.6 123 3.92 3.440 18.90  1  0    4    4
#> 
#> 
#> 
#> $`8`
#> $`8`$`0`
#> $`8`$`0`$`0`
#>                     mpg cyl  disp  hp drat   wt  qsec vs am gear carb
#> Hornet Sportabout  18.7   8 360.0 175 3.15 3.44 17.02  0  0    3    2
#> Duster 360         14.3   8 360.0 245 3.21 3.57 15.84  0  0    3    4
#> Merc 450SE         16.4   8 275.8 180 3.07 4.07 17.40  0  0    3    3
#> Merc 450SL         17.3   8 275.8 180 3.07 3.73 17.60  0  0    3    3
#> Merc 450SLC        15.2   8 275.8 180 3.07 3.78 18.00  0  0    3    3
#> Cadillac Fleetwood 10.4   8 472.0 205 2.93 5.25 17.98  0  0    3    4
#>  [ reached 'max' / getOption("max.print") -- omitted 6 rows ]
#> 
#> $`8`$`0`$`1`
#>                 mpg cyl disp  hp drat   wt qsec vs am gear carb
#> Ford Pantera L 15.8   8  351 264 4.22 3.17 14.5  0  1    5    4
#> Maserati Bora  15.0   8  301 335 3.54 3.57 14.6  0  1    5    8
#> 
#> 
#> 
rsplit(mtcars, ~ cyl + vs + am)
#> $`4`
#> $`4`$`0`
#> $`4`$`0`$`1`
#>               mpg  disp hp drat   wt qsec gear carb
#> Porsche 914-2  26 120.3 91 4.43 2.14 16.7    5    2
#> 
#> 
#> $`4`$`1`
#> $`4`$`1`$`0`
#>                mpg  disp hp drat    wt  qsec gear carb
#> Merc 240D     24.4 146.7 62 3.69 3.190 20.00    4    2
#> Merc 230      22.8 140.8 95 3.92 3.150 22.90    4    2
#> Toyota Corona 21.5 120.1 97 3.70 2.465 20.01    3    1
#> 
#> $`4`$`1`$`1`
#>                 mpg  disp  hp drat    wt  qsec gear carb
#> Datsun 710     22.8 108.0  93 3.85 2.320 18.61    4    1
#> Fiat 128       32.4  78.7  66 4.08 2.200 19.47    4    1
#> Honda Civic    30.4  75.7  52 4.93 1.615 18.52    4    2
#> Toyota Corolla 33.9  71.1  65 4.22 1.835 19.90    4    1
#> Fiat X1-9      27.3  79.0  66 4.08 1.935 18.90    4    1
#> Lotus Europa   30.4  95.1 113 3.77 1.513 16.90    5    2
#> Volvo 142E     21.4 121.0 109 4.11 2.780 18.60    4    2
#> 
#> 
#> 
#> $`6`
#> $`6`$`0`
#> $`6`$`0`$`1`
#>                mpg disp  hp drat    wt  qsec gear carb
#> Mazda RX4     21.0  160 110 3.90 2.620 16.46    4    4
#> Mazda RX4 Wag 21.0  160 110 3.90 2.875 17.02    4    4
#> Ferrari Dino  19.7  145 175 3.62 2.770 15.50    5    6
#> 
#> 
#> $`6`$`1`
#> $`6`$`1`$`0`
#>                 mpg  disp  hp drat    wt  qsec gear carb
#> Hornet 4 Drive 21.4 258.0 110 3.08 3.215 19.44    3    1
#> Valiant        18.1 225.0 105 2.76 3.460 20.22    3    1
#> Merc 280       19.2 167.6 123 3.92 3.440 18.30    4    4
#> Merc 280C      17.8 167.6 123 3.92 3.440 18.90    4    4
#> 
#> 
#> 
#> $`8`
#> $`8`$`0`
#> $`8`$`0`$`0`
#>                      mpg  disp  hp drat    wt  qsec gear carb
#> Hornet Sportabout   18.7 360.0 175 3.15 3.440 17.02    3    2
#> Duster 360          14.3 360.0 245 3.21 3.570 15.84    3    4
#> Merc 450SE          16.4 275.8 180 3.07 4.070 17.40    3    3
#> Merc 450SL          17.3 275.8 180 3.07 3.730 17.60    3    3
#> Merc 450SLC         15.2 275.8 180 3.07 3.780 18.00    3    3
#> Cadillac Fleetwood  10.4 472.0 205 2.93 5.250 17.98    3    4
#> Lincoln Continental 10.4 460.0 215 3.00 5.424 17.82    3    4
#> Chrysler Imperial   14.7 440.0 230 3.23 5.345 17.42    3    4
#>  [ reached 'max' / getOption("max.print") -- omitted 4 rows ]
#> 
#> $`8`$`0`$`1`
#>                 mpg disp  hp drat   wt qsec gear carb
#> Ford Pantera L 15.8  351 264 4.22 3.17 14.5    5    4
#> Maserati Bora  15.0  301 335 3.54 3.57 14.6    5    8
#> 
#> 
#> 

rsplit(mtcars, ~ cyl + vs + am, flatten = TRUE)
#> $`4.0.1`
#>               mpg  disp hp drat   wt qsec gear carb
#> Porsche 914-2  26 120.3 91 4.43 2.14 16.7    5    2
#> 
#> $`4.1.0`
#>                mpg  disp hp drat    wt  qsec gear carb
#> Merc 240D     24.4 146.7 62 3.69 3.190 20.00    4    2
#> Merc 230      22.8 140.8 95 3.92 3.150 22.90    4    2
#> Toyota Corona 21.5 120.1 97 3.70 2.465 20.01    3    1
#> 
#> $`4.1.1`
#>                 mpg  disp  hp drat    wt  qsec gear carb
#> Datsun 710     22.8 108.0  93 3.85 2.320 18.61    4    1
#> Fiat 128       32.4  78.7  66 4.08 2.200 19.47    4    1
#> Honda Civic    30.4  75.7  52 4.93 1.615 18.52    4    2
#> Toyota Corolla 33.9  71.1  65 4.22 1.835 19.90    4    1
#> Fiat X1-9      27.3  79.0  66 4.08 1.935 18.90    4    1
#> Lotus Europa   30.4  95.1 113 3.77 1.513 16.90    5    2
#> Volvo 142E     21.4 121.0 109 4.11 2.780 18.60    4    2
#> 
#> $`6.0.1`
#>                mpg disp  hp drat    wt  qsec gear carb
#> Mazda RX4     21.0  160 110 3.90 2.620 16.46    4    4
#> Mazda RX4 Wag 21.0  160 110 3.90 2.875 17.02    4    4
#> Ferrari Dino  19.7  145 175 3.62 2.770 15.50    5    6
#> 
#> $`6.1.0`
#>                 mpg  disp  hp drat    wt  qsec gear carb
#> Hornet 4 Drive 21.4 258.0 110 3.08 3.215 19.44    3    1
#> Valiant        18.1 225.0 105 2.76 3.460 20.22    3    1
#> Merc 280       19.2 167.6 123 3.92 3.440 18.30    4    4
#> Merc 280C      17.8 167.6 123 3.92 3.440 18.90    4    4
#> 
#> $`8.0.0`
#>                      mpg  disp  hp drat    wt  qsec gear carb
#> Hornet Sportabout   18.7 360.0 175 3.15 3.440 17.02    3    2
#> Duster 360          14.3 360.0 245 3.21 3.570 15.84    3    4
#> Merc 450SE          16.4 275.8 180 3.07 4.070 17.40    3    3
#> Merc 450SL          17.3 275.8 180 3.07 3.730 17.60    3    3
#> Merc 450SLC         15.2 275.8 180 3.07 3.780 18.00    3    3
#> Cadillac Fleetwood  10.4 472.0 205 2.93 5.250 17.98    3    4
#> Lincoln Continental 10.4 460.0 215 3.00 5.424 17.82    3    4
#> Chrysler Imperial   14.7 440.0 230 3.23 5.345 17.42    3    4
#>  [ reached 'max' / getOption("max.print") -- omitted 4 rows ]
#> 
#> $`8.0.1`
#>                 mpg disp  hp drat   wt qsec gear carb
#> Ford Pantera L 15.8  351 264 4.22 3.17 14.5    5    4
#> Maserati Bora  15.0  301 335 3.54 3.57 14.6    5    8
#> 

rsplit(mtcars, mpg ~ cyl)
#> $`4`
#>  [1] 22.8 24.4 22.8 32.4 30.4 33.9 21.5 27.3 26.0 30.4 21.4
#> 
#> $`6`
#> [1] 21.0 21.0 21.4 18.1 19.2 17.8 19.7
#> 
#> $`8`
#>  [1] 18.7 14.3 16.4 17.3 15.2 10.4 10.4 14.7 15.5 15.2 13.3 19.2 15.8 15.0
#> 
rsplit(mtcars, mpg ~ cyl, simplify = FALSE)
#> $`4`
#>                 mpg
#> Datsun 710     22.8
#> Merc 240D      24.4
#> Merc 230       22.8
#> Fiat 128       32.4
#> Honda Civic    30.4
#> Toyota Corolla 33.9
#> Toyota Corona  21.5
#> Fiat X1-9      27.3
#> Porsche 914-2  26.0
#> Lotus Europa   30.4
#> Volvo 142E     21.4
#> 
#> $`6`
#>                 mpg
#> Mazda RX4      21.0
#> Mazda RX4 Wag  21.0
#> Hornet 4 Drive 21.4
#> Valiant        18.1
#> Merc 280       19.2
#> Merc 280C      17.8
#> Ferrari Dino   19.7
#> 
#> $`8`
#>                      mpg
#> Hornet Sportabout   18.7
#> Duster 360          14.3
#> Merc 450SE          16.4
#> Merc 450SL          17.3
#> Merc 450SLC         15.2
#> Cadillac Fleetwood  10.4
#> Lincoln Continental 10.4
#> Chrysler Imperial   14.7
#> Dodge Challenger    15.5
#> AMC Javelin         15.2
#> Camaro Z28          13.3
#> Pontiac Firebird    19.2
#> Ford Pantera L      15.8
#> Maserati Bora       15.0
#> 
rsplit(mtcars, mpg + hp ~ cyl + vs + am)
#> $`4`
#> $`4`$`0`
#> $`4`$`0`$`1`
#>               mpg hp
#> Porsche 914-2  26 91
#> 
#> 
#> $`4`$`1`
#> $`4`$`1`$`0`
#>                mpg hp
#> Merc 240D     24.4 62
#> Merc 230      22.8 95
#> Toyota Corona 21.5 97
#> 
#> $`4`$`1`$`1`
#>                 mpg  hp
#> Datsun 710     22.8  93
#> Fiat 128       32.4  66
#> Honda Civic    30.4  52
#> Toyota Corolla 33.9  65
#> Fiat X1-9      27.3  66
#> Lotus Europa   30.4 113
#> Volvo 142E     21.4 109
#> 
#> 
#> 
#> $`6`
#> $`6`$`0`
#> $`6`$`0`$`1`
#>                mpg  hp
#> Mazda RX4     21.0 110
#> Mazda RX4 Wag 21.0 110
#> Ferrari Dino  19.7 175
#> 
#> 
#> $`6`$`1`
#> $`6`$`1`$`0`
#>                 mpg  hp
#> Hornet 4 Drive 21.4 110
#> Valiant        18.1 105
#> Merc 280       19.2 123
#> Merc 280C      17.8 123
#> 
#> 
#> 
#> $`8`
#> $`8`$`0`
#> $`8`$`0`$`0`
#>                      mpg  hp
#> Hornet Sportabout   18.7 175
#> Duster 360          14.3 245
#> Merc 450SE          16.4 180
#> Merc 450SL          17.3 180
#> Merc 450SLC         15.2 180
#> Cadillac Fleetwood  10.4 205
#> Lincoln Continental 10.4 215
#> Chrysler Imperial   14.7 230
#> Dodge Challenger    15.5 150
#> AMC Javelin         15.2 150
#> Camaro Z28          13.3 245
#> Pontiac Firebird    19.2 175
#> 
#> $`8`$`0`$`1`
#>                 mpg  hp
#> Ford Pantera L 15.8 264
#> Maserati Bora  15.0 335
#> 
#> 
#> 
rsplit(mtcars, mpg + hp ~ cyl + vs + am, keep.by = TRUE)
#> $`4`
#> $`4`$`0`
#> $`4`$`0`$`1`
#>               cyl vs am mpg hp
#> Porsche 914-2   4  0  1  26 91
#> 
#> 
#> $`4`$`1`
#> $`4`$`1`$`0`
#>               cyl vs am  mpg hp
#> Merc 240D       4  1  0 24.4 62
#> Merc 230        4  1  0 22.8 95
#> Toyota Corona   4  1  0 21.5 97
#> 
#> $`4`$`1`$`1`
#>                cyl vs am  mpg  hp
#> Datsun 710       4  1  1 22.8  93
#> Fiat 128         4  1  1 32.4  66
#> Honda Civic      4  1  1 30.4  52
#> Toyota Corolla   4  1  1 33.9  65
#> Fiat X1-9        4  1  1 27.3  66
#> Lotus Europa     4  1  1 30.4 113
#> Volvo 142E       4  1  1 21.4 109
#> 
#> 
#> 
#> $`6`
#> $`6`$`0`
#> $`6`$`0`$`1`
#>               cyl vs am  mpg  hp
#> Mazda RX4       6  0  1 21.0 110
#> Mazda RX4 Wag   6  0  1 21.0 110
#> Ferrari Dino    6  0  1 19.7 175
#> 
#> 
#> $`6`$`1`
#> $`6`$`1`$`0`
#>                cyl vs am  mpg  hp
#> Hornet 4 Drive   6  1  0 21.4 110
#> Valiant          6  1  0 18.1 105
#> Merc 280         6  1  0 19.2 123
#> Merc 280C        6  1  0 17.8 123
#> 
#> 
#> 
#> $`8`
#> $`8`$`0`
#> $`8`$`0`$`0`
#>                     cyl vs am  mpg  hp
#> Hornet Sportabout     8  0  0 18.7 175
#> Duster 360            8  0  0 14.3 245
#> Merc 450SE            8  0  0 16.4 180
#> Merc 450SL            8  0  0 17.3 180
#> Merc 450SLC           8  0  0 15.2 180
#> Cadillac Fleetwood    8  0  0 10.4 205
#> Lincoln Continental   8  0  0 10.4 215
#> Chrysler Imperial     8  0  0 14.7 230
#> Dodge Challenger      8  0  0 15.5 150
#> AMC Javelin           8  0  0 15.2 150
#> Camaro Z28            8  0  0 13.3 245
#> Pontiac Firebird      8  0  0 19.2 175
#> 
#> $`8`$`0`$`1`
#>                cyl vs am  mpg  hp
#> Ford Pantera L   8  0  1 15.8 264
#> Maserati Bora    8  0  1 15.0 335
#> 
#> 
#> 

# Split this sectoral data, first by Variable (Emloyment and Value Added), then by Country
GGDCspl <- rsplit(GGDC10S, ~ Variable + Country, cols = 6:16)
str(GGDCspl)
#> List of 2
#>  $ EMP:List of 42
#>   ..$ ARG:'data.frame':  62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] 1800 1835 1731 2030 1889 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] 32.7 34.4 35.6 33.8 33.3 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] 1603 1641 1690 1578 1722 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] 39.3 42.4 49.2 52.2 57.7 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] 314 353 311 292 330 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] 890 880 932 904 914 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] 425 428 462 455 469 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] 204 204 219 210 214 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] 825 818 881 870 894 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] 411 411 442 435 445 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] 6544 6647 6752 6859 6967 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ BOL:'data.frame':  61 obs. of  11 variables:
#>   .. ..$ AGR : num [1:61] 1052 1019 987 976 946 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:61] 46.3 49.5 50.8 54.6 46.9 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:61] 117 117 115 122 141 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:61] 1.45 1.39 1.38 1.46 1.4 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:61] 26 37.8 48.2 34.3 33.5 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:61] 60.8 69.4 71.1 64.4 66.2 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:61] 23.1 22.9 27.7 28.8 32.5 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:61] 4.31 4.94 5.08 4.62 4.76 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:61] 119 113 114 121 121 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:61] 1450 1435 1421 1407 1393 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ BRA:'data.frame':  62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] 12637 12886 13140 13400 13665 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] 96.8 98.2 99.6 101 102.4 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] 2254 2326 2399 2475 2554 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] 174 178 182 186 190 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] 709 731 754 777 801 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] 1332 1394 1458 1525 1596 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] 587 613 640 669 699 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] 461 489 518 549 583 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] 690 732 777 824 875 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] 694 737 782 830 881 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] 19635 20183 20749 21337 21945 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ BWA:'data.frame':  52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] NA NA NA NA 152 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] NA NA NA NA 1.94 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] NA NA NA NA 2.42 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] NA NA NA NA 0.12 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] NA NA NA NA 2.7 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] NA NA NA NA 2.47 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] NA NA NA NA 2.31 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] NA NA NA NA 1.21 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] NA NA NA NA 4.51 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] NA NA NA NA 4.07 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] NA NA NA NA 174 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ CHL:'data.frame':  63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] 679 696 749 702 630 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] 108 130 113 119 103 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] 418 379 348 351 558 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] 21.2 24.2 19.4 19.6 16.8 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] 97.6 97.9 97.1 86.3 84.4 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] 225 222 241 256 218 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] 93.4 101.6 108.2 119.4 119.6 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] 50.5 53.9 51 52.8 46.8 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] 480 491 492 537 491 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] 2172 2196 2219 2243 2267 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ CHN:'data.frame':  62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA 173170 177470 181510 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA 1610 1804 1979 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA 11653 13054 14325 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA 212 238 261 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA 1834 2055 2255 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA 4881 4936 4669 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA 3211 3247 3071 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA 1347 1362 1288 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA 6870 6946 6570 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA 2501 2529 2392 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA 207290 213640 218320 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ COL:'data.frame':  61 obs. of  11 variables:
#>   .. ..$ AGR : num [1:61] 2094 2097 2145 2095 2074 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:61] 56.6 61.6 58.4 58.7 57.5 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:61] 421 422 425 443 459 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:61] 10.6 10.6 10.4 10.7 10.7 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:61] 120 114 117 142 180 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:61] 180 191 201 226 253 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:61] 117 136 144 149 156 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:61] 170 173 175 190 206 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:61] 541 576 576 610 604 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:61] 3710 3781 3852 3925 4000 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ CRI:'data.frame':  62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] 153 161 165 158 160 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] 0.795 0.843 0.834 0.825 0.909 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] 29.1 31 30.8 30.6 33.9 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] 1.59 1.58 1.6 1.87 1.94 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] 11.4 10.1 11 14.8 15.4 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] 20.9 21 21.2 23.2 22.3 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] 9.27 8.81 9.08 9.64 10.17 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] 4.24 4.16 4.44 5.12 5.17 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] 25.8 24.9 25.1 29.3 30.4 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] 13.1 12.7 13.1 15.1 15.4 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] 269 276 282 289 296 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ DEW:'data.frame':  61 obs. of  11 variables:
#>   .. ..$ AGR : num [1:61] 4837 4658 4520 4394 4275 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:61] 620 640 675 691 685 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:61] 6262 6638 6791 7036 7327 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:61] 154 158 167 169 173 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:61] 1489 1588 1673 1788 1922 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:61] 2745 2943 3120 3331 3510 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:61] 1005 1029 1062 1086 1120 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:61] 587 647 718 776 832 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:61] 2149 2175 2257 2326 2403 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:61] 531 530 547 566 587 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:61] 20379 21007 21527 22164 22832 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ DNK:'data.frame':  64 obs. of  11 variables:
#>   .. ..$ AGR : num [1:64] 516 509 502 495 484 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:64] 30.9 29.5 28.1 26.7 25.3 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:64] 476 487 514 522 503 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:64] 10.8 10.8 11.1 11.3 11.4 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:64] 128 133 138 135 140 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:64] 345 345 360 367 374 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:64] 128 130 131 132 126 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:64] 83.4 82.5 80.2 78.4 80.3 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:64] 155 157 159 176 182 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:64] 91.2 92.1 92.9 94.1 92.8 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:64] 1963 1975 2016 2038 2020 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ EGY:'data.frame':  65 obs. of  11 variables:
#>   .. ..$ AGR : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ ESP:'data.frame':  64 obs. of  11 variables:
#>   .. ..$ AGR : num [1:64] NA NA 4339 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:64] NA NA 111 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:64] NA NA 1532 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:64] NA NA 42.9 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:64] NA NA 523 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:64] NA NA 1104 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:64] NA NA 383 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:64] NA NA 224 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:64] NA NA 592 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:64] NA NA 957 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:64] NA NA 9808 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ ETH:'data.frame':  52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] NA 8960 9140 9323 9510 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] NA 0.401 0.535 0.535 0.669 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] NA 119 125 129 147 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] NA 1.44 1.74 2.2 2.52 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] NA 16.5 18.1 18.7 19.5 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] NA 75.4 85.3 94.7 111.2 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] NA 14 15.4 16.6 19.2 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] NA 13.8 13.7 13.6 13.4 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] NA 75.1 80.7 88.6 97.7 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] NA 39.2 50.5 54 59.3 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] NA 9315 9531 9741 9981 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ FRA:'data.frame':  64 obs. of  11 variables:
#>   .. ..$ AGR : num [1:64] NA NA 5004 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:64] NA NA 212 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:64] NA NA 4637 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:64] NA NA 111 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:64] NA NA 1494 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:64] NA NA 2459 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:64] NA NA 946 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:64] NA NA 1177 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:64] NA NA 3234 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:64] NA NA 367 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:64] NA NA 19643 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ GBR:'data.frame':  64 obs. of  11 variables:
#>   .. ..$ AGR : num [1:64] 1372 1368 1352 1317 1289 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:64] 1651 1647 1606 1610 1647 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:64] 7255 7388 7573 7747 7632 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:64] 269 281 296 304 312 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:64] 1669 1655 1654 1670 1661 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:64] 3321 3418 3452 3507 3529 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:64] 3673 3669 3679 3601 3615 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:64] 547 561 555 561 568 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:64] 4006 3881 3906 4045 4127 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:64] 647 626 621 610 621 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:64] 24410 24494 24694 24970 25001 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ GHA:'data.frame':  52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] 1555 1576 1599 1621 1644 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] 47.4 46.5 44.7 45.8 38.8 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] 280 281 277 290 262 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] 13.95 12.86 11.6 11.15 8.88 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] 87.2 89.1 89.1 95 87 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] 365 378 383 414 407 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] 66.7 69.7 71.2 77.5 72.6 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] 7.03 7.47 7.76 8.59 8.18 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] 96.7 105.7 112.8 128.5 125.7 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] 44.2 46 46.8 50.8 47.4 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] 2562 2613 2642 2742 2701 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ HKG:'data.frame':  62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ IDN:'data.frame':  63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ IND:'data.frame':  61 obs. of  11 variables:
#>   .. ..$ AGR : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ ITA:'data.frame':  64 obs. of  11 variables:
#>   .. ..$ AGR : num [1:64] NA NA NA 9517 9284 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:64] NA NA NA 39.9 41.9 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:64] NA NA NA 4025 4035 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:64] NA NA NA 96.7 99.6 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:64] NA NA NA 1108 1264 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:64] NA NA NA 1818 1913 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:64] NA NA NA 607 619 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:64] NA NA NA 364 368 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:64] NA NA NA 1791 1838 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:64] NA NA NA 773 787 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:64] NA NA NA 20139 20249 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ JPN:'data.frame':  63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] NA NA NA 17082 16570 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] NA NA NA 663 630 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] NA NA NA 6836 7074 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] NA NA NA 199 194 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] NA NA NA 1954 2038 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] NA NA NA 5952 6496 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] NA NA NA 1879 1830 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] NA NA NA 1509 1562 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] NA NA NA 3839 3974 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] NA NA NA 818 847 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] NA NA NA 40732 41216 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ KEN:'data.frame':  52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ KOR:'data.frame':  61 obs. of  11 variables:
#>   .. ..$ AGR : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ MEX:'data.frame':  63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] 4808 4781 4725 4748 4826 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] 96.8 94.7 101.6 100.1 94.9 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] 969 1002 1036 1060 1064 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] 24.9 25.3 25.9 27 26.6 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] 224 234 249 263 266 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] 682 708 727 751 737 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] 210 205 233 230 236 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] 86 89.1 94.8 93.6 101 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] 688 723 750 761 772 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] 421 443 459 465 472 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] 8210 8305 8401 8498 8596 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ MOR:'data.frame':  65 obs. of  11 variables:
#>   .. ..$ AGR : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:65] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ MUS:'data.frame':  52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] NA NA 67.2 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] NA NA 0.152 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] NA NA 26 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] NA NA 2.16 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] NA NA 18.8 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] NA NA 17.1 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] NA NA 11.2 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] NA NA 0.781 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] NA NA 15.8 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] NA NA 17.4 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] NA NA 177 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ MWI:'data.frame':  52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ MYS:'data.frame':  62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ NGA:'data.frame':  52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] 13009 13182 13359 13537 13718 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] 42 42.4 35.6 26.8 24.6 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] 567 664 797 969 1079 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] 21 22.9 22.4 23 23.2 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] 252 240 212 202 199 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] 2155 2189 2227 2601 2893 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] 294 327 341 354 402 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] 42 42.4 42.8 42.1 45.7 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] 95 125 171 227 330 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] 163 217 263 314 418 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] 16640 17053 17470 18296 19133 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ NLD:'data.frame':  64 obs. of  11 variables:
#>   .. ..$ AGR : num [1:64] NA 637 629 619 610 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:64] NA 61.5 NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:64] NA 1034 NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:64] NA 31.2 NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:64] NA 356 NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:64] NA 623 NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:64] NA 293 NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:64] NA 169 NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:64] NA 708 NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:64] NA 375 NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:64] NA 4288 629 619 610 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ PER:'data.frame':  62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ PHL:'data.frame':  63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ SEN:'data.frame':  51 obs. of  11 variables:
#>   .. ..$ AGR : num [1:51] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:51] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:51] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:51] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:51] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:51] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:51] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:51] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:51] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:51] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:51] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ SGP:'data.frame':  62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ SWE:'data.frame':  64 obs. of  11 variables:
#>   .. ..$ AGR : num [1:64] NA NA 735 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:64] NA NA 20.1 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:64] NA NA 971 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:64] NA NA 18.9 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:64] NA NA 273 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:64] NA NA 351 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:64] NA NA 263 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:64] NA NA 63.4 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:64] NA NA 451 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:64] NA NA 148 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:64] NA NA 3294 NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ THA:'data.frame':  62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ TWN:'data.frame':  63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ TZA:'data.frame':  52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] 4021 4119 4221 4324 4430 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] 5.98 5.48 4.91 3.84 4.06 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] 46.9 49.4 52.8 56.2 60.3 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] 1.23 4.3 4.54 4.75 4.56 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] 7.47 6.52 8.2 8.94 11.63 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] 42.4 53.7 54.8 56.8 62 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] 10.2 27.7 29.1 31.1 33 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] 4.05 5.13 5.27 5.5 6.05 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] 153 206 179 157 140 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] 91.3 123.3 107 93.7 83.6 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] 4383 4601 4666 4742 4835 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ USA:'data.frame':  64 obs. of  11 variables:
#>   .. ..$ AGR : num [1:64] NA NA NA 5703 5375 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:64] NA NA NA 980 995 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:64] NA NA NA 15641 16868 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:64] NA NA NA 463 473 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:64] NA NA NA 3676 3927 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:64] NA NA NA 12579 13168 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:64] NA NA NA 4898 5160 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:64] NA NA NA 4238 4456 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:64] NA NA NA 11826 13900 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:64] NA NA NA 2534 2604 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:64] NA NA NA 62539 66926 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ VEN:'data.frame':  62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] 695 729 733 727 678 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] 47 49.5 49.3 45.6 47 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] 164 160 174 194 219 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] 6.07 6.85 7.56 8.96 9.78 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] 97.1 113.2 119.5 106.7 121.9 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] 144 163 166 181 209 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] 54.6 55.2 54 65.6 72.2 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] 50.5 55.8 55.4 58.8 66.1 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] 309 287 313 339 360 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] 1567 1619 1672 1727 1783 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ ZAF:'data.frame':  52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] 3375 3302 3231 3161 3091 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] 615 641 629 635 648 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] 644 668 697 731 795 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] 28.3 30.2 30.9 33.5 35 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] 276 269 272 283 320 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] 751 774 797 821 846 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] 205 216 227 242 253 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] 119 125 132 138 146 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] 323 337 355 354 362 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] 586 607 622 639 654 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] 6922 6969 6992 7038 7149 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ ZMB:'data.frame':  52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>  $ VA :List of 43
#>   ..$ ARG     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] 5.89e-07 9.17e-07 9.96e-07 1.48e-06 1.40e-06 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] 0 0 0 0 0 0 0 0 0 0 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] 3.53e-06 4.77e-06 5.35e-06 5.37e-06 6.56e-06 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] 0 0 0 0 0 0 0 0 0 0 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] 5.59e-07 6.66e-07 7.02e-07 6.78e-07 8.21e-07 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] 2.99e-06 4.43e-06 4.74e-06 4.97e-06 5.56e-06 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] 8.81e-07 1.27e-06 1.50e-06 1.58e-06 1.76e-06 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] 2.51e-07 3.12e-07 3.72e-07 4.37e-07 4.96e-07 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] 1.21e-06 1.68e-06 1.94e-06 2.19e-06 2.55e-06 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] 2.92e-07 3.95e-07 5.05e-07 5.69e-07 6.64e-07 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] 1.03e-05 1.44e-05 1.61e-05 1.73e-05 1.98e-05 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ BOL     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ BRA     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ BWA     :'data.frame': 52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] NA NA NA NA 16.3 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] NA NA NA NA 3.49 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] NA NA NA NA 0.737 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] NA NA NA NA 0.104 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] NA NA NA NA 0.66 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] NA NA NA NA 6.24 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] NA NA NA NA 1.66 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] NA NA NA NA 1.12 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] NA NA NA NA 4.82 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] NA NA NA NA 2.34 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] NA NA NA NA 37.5 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ CHL     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] 0.0149 0.0176 0.0251 0.0296 0.0449 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] 0.0189 0.0269 0.0317 0.0424 0.0631 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] 0.0129 0.0143 0.0189 0.0259 0.0752 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] 0.00131 0.00185 0.00211 0.00287 0.00438 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] 0.0114 0.0132 0.0174 0.0196 0.0322 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] 0.0198 0.0247 0.0347 0.0485 0.0734 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] 0.0137 0.0164 0.0247 0.0343 0.0578 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] 0.0279 0.0347 0.0452 0.0611 0.086 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] 0.0322 0.0385 0.0517 0.0723 0.1135 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] 0.153 0.188 0.251 0.337 0.551 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ CHN     :'data.frame': 63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] NA NA 34598 38139 39552 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] NA NA 1366 1855 2040 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] NA NA 11010 14947 16437 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] NA NA 681 924 1017 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] NA NA 2200 2900 2700 3100 5600 4600 6900 7700 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] NA NA 8470 12183 12690 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] NA NA 3080 3717 4036 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] NA NA 2247 2157 2157 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] NA NA 3049 4015 3710 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] NA NA 781 1028 950 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] NA NA 67482 81866 85288 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ COL     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] 3305 3632 3859 4290 5003 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] 223 286 350 370 394 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] 1818 1995 2209 2819 3206 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] 50.3 56.3 62.3 82.7 83.9 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] 355 380 412 489 618 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] 1600 1585 1820 2111 2490 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] 589 714 783 856 988 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] 754 869 923 960 1027 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] 1365 1539 1749 1896 2158 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] 10059 11057 12167 13873 15968 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ CRI     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] 371 398 439 475 495 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] 1.73 1.86 2.03 2.2 2.34 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] 306 330 361 391 415 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] 17.4 19 20.7 22.3 23.7 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] 117 125 137 148 158 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] 320 345 375 404 426 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] 108 120 133 145 153 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] 123 135 146 158 168 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] 106 117 131 145 164 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] 50 55.1 61.8 68.5 77.3 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] 1520 1646 1806 1960 2082 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ DEW     :'data.frame': 61 obs. of  11 variables:
#>   .. ..$ AGR : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:61] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ DNK     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ EGY     :'data.frame': 64 obs. of  11 variables:
#>   .. ..$ AGR : num [1:64] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:64] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:64] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:64] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:64] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:64] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:64] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:64] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:64] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:64] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:64] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ ESP     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ ETH     :'data.frame': 53 obs. of  11 variables:
#>   .. ..$ AGR : num [1:53] NA 4496 4514 4669 5129 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:53] NA 11.9 15.8 15.8 19.8 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:53] NA 110 121 127 150 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:53] NA 31.6 31.6 37.9 44.2 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:53] NA 284 313 323 339 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:53] NA 334 358 389 487 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:53] NA 62.4 66.8 72.1 85.3 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:53] NA 19 20.9 23.7 26.6 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:53] NA 91.8 113.5 117.6 129 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:53] NA 106 111 120 143 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:53] NA 5547 5665 5896 6552 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ FRA     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ GBR     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ GHA     :'data.frame': 52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] 0.0358 0.0382 0.0405 0.0452 0.0508 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] 0.0051 0.00546 0.00579 0.00645 0.00724 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] 0.0174 0.0187 0.0198 0.022 0.0248 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] 0.00078 0.000834 0.000884 0.000985 0.001107 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] 0.016 0.0171 0.0181 0.0202 0.0227 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] 0.00906 0.00968 0.01027 0.01144 0.01286 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] 0.0195 0.0209 0.0221 0.0247 0.0277 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] 0.00651 0.00589 0.00634 0.00725 0.00792 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] 0.0185 0.0198 0.021 0.0234 0.0263 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] 0.00144 0.00154 0.00163 0.00182 0.00204 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] 0.13 0.138 0.146 0.163 0.183 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ HKG     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ IDN     :'data.frame': 63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ IND     :'data.frame': 63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] 52242 53942 52545 57895 49249 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] 733 817 841 853 889 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] 10957 12158 11446 12902 13381 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] 252 309 320 343 378 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] 2581 2949 2734 2744 3052 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] 6540 6951 7063 7552 7685 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] 3185 3570 3529 3702 3874 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] 7177 7723 8156 8797 9423 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] 5008 5253 5384 5742 6100 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] 2999 3116 3232 3326 3354 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] 91673 96788 95250 103857 97384 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ ITA     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ JPN     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA 1136846 1256178 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA 170793 180088 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA 1974868 2073194 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA 141430 163909 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA 292007 301608 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA 967553 1105979 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA 478903 556146 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA 434033 495936 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA 882683 1008780 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA 161783 184894 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA 6640899 7326712 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ KEN     :'data.frame': 52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] 4910 4714 5613 6043 6100 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] 59.7 43.4 43.4 48.8 83 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] 818 863 875 928 1083 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] 299 299 352 384 414 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] 447 441 385 277 294 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] 549 561 570 604 669 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] 626 650 687 758 804 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] 284 308 316 335 388 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] 841 912 950 973 1138 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] 374 340 343 372 406 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] 9208 9133 10134 10723 11378 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ KOR     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA 22697 26809 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA 724 888 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA 4012 7199 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA 204 297 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA 852 1441 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA 2442 4452 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA 2447 3581 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA 8701 11218 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA 5647 10541 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA 47726 66426 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ MEX     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] 10.9 13.1 13.9 14.6 18.1 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] 3.2 4.18 4.61 3.96 4.75 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] 9.24 11.9 13.18 13.48 16.19 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] 0.282 0.358 0.429 0.48 0.567 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] 1.34 2.03 2.63 1.95 2.57 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] 13.1 18.3 20.4 19.2 23.8 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] 4.25 5.22 6.13 6.34 6.86 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] 3.07 3.6 4.46 4.71 5.77 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] 2.31 2.72 3.15 3.47 4.22 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] 0.653 0.769 0.891 0.984 1.195 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] 48.3 62.2 69.8 69.2 84 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ MOR     :'data.frame': 63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ MUS     :'data.frame': 53 obs. of  11 variables:
#>   .. ..$ AGR : num [1:53] 106 182 178 293 177 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:53] 22 22 22 22 22 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:53] 68.5 118.5 119.5 178.2 120.4 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:53] 29.9 22.2 22.6 21.7 22.2 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:53] 34.9 40.7 49.5 61.1 61.1 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:53] 96.4 106.5 106.5 113.7 125.2 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:53] 102 112 116 133 117 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:53] 32.6 32.6 35.1 37.6 37.6 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:53] 64.2 68.9 73.7 78.4 83.2 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:53] 11.2 11.7 12.5 13.1 13.8 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:53] 567 717 735 952 780 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ MWI     :'data.frame': 52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] 81.1 84.9 88.3 94.6 87.7 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] 0.585 0.63 0.652 0.922 0.854 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] 8.73 9.4 9.73 13.76 12.75 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] 1.33 1.99 1.66 1.99 1.99 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] 7.93 7.13 7.13 7.53 10.7 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] 22.2 21.8 21.8 23.4 23.4 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] 12.1 13.2 14.2 14.2 13.2 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] 0.732 0.619 0.628 0.86 1.204 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] 17 18.7 21.3 23 50.2 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] 2.94 3.1 3.25 3.33 17.57 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] 155 161 169 183 220 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ MYS     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ NGA     :'data.frame': 52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] 1903 1955 2156 2247 2250 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] 25.3 41.5 52.6 53.4 71.3 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] 357 405 483 571 573 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] 63.8 77.1 77.1 97.1 114.4 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] 145 163 173 181 210 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] 507 523 559 619 680 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] 97.4 116 123.4 130.4 152.2 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] 48.5 55.3 57.8 65 75.1 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] 67.2 72.6 77.6 81.3 94.9 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] 51.6 56.7 64.4 71.1 81.2 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] 3266 3466 3824 4116 4303 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ NGA(alt):'data.frame': 4 obs. of  11 variables:
#>   .. ..$ AGR : num [1:4] 12988809 14421929 15918632 17625143
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:4] 8454554 11140408 11382588 11631349
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:4] 3578642 4085393 4744699 5476303
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:4] 388270 569933 754282 961717
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:4] 1570973 1819803 2142754 2502582
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:4] 9156043 10608005 12170934 13732584
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:4] 6655717 7508984 8573386 9773442
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:4] 7095640 8179975 9848171 11650290
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:4] 3385190 3862789 4422719 5056768
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:4] 930958 1061360 1228371 1411950
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:4] 54204795 63258579 71186535 79822128
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ NLD     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ PER     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] 7.8 9.59 9.85 10.17 10.85 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] 2.45 3.35 3.55 3.74 4.72 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] 2.04 2.32 2.67 3.03 3.72 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] 0.281 0.344 0.389 0.407 0.441 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] 0.43 0.527 0.596 0.623 0.676 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] 1.76 2.15 2.5 2.69 2.82 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] 1.64 2.01 2.27 2.38 2.58 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] -7.26 -8.51 -13.76 -13.82 -14.94 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] 1.37 1.72 2.28 2.58 2.71 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] 10.5 13.5 10.3 11.8 13.6 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ PHL     :'data.frame': 63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ SEN     :'data.frame': 51 obs. of  11 variables:
#>   .. ..$ AGR : num [1:51] 47013 49204 52369 47237 51412 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:51] 2033 2222 1843 1811 1958 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:51] 14586 15805 16212 18428 19915 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:51] 4658 5092 4224 4151 4486 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:51] 2515 2601 3086 3024 3174 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:51] 37244 41224 44208 47604 48727 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:51] 9903 10068 10524 10689 11561 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:51] 3157 2672 2710 3400 3342 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:51] 29462 29462 29462 35993 36363 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:51] 3627 3627 3627 4431 4476 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:51] 154198 161976 168266 176768 185413 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ SGP     :'data.frame': 63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ SWE     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ THA     :'data.frame': 62 obs. of  11 variables:
#>   .. ..$ AGR : num [1:62] NA 12192 11161 12087 11062 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:62] NA 334 349 328 340 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:62] NA 3845 4358 4923 5008 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:62] NA 42.2 47.7 58.1 81.4 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:62] NA 541 784 887 898 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:62] NA 6065 6760 6955 7364 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:62] NA 810 1068 1481 1618 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:62] NA 52.3 57 62.9 99.3 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:62] NA 1765 2972 3094 3599 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:62] NA 529 592 688 698 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:62] NA 26176 28150 30564 30767 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ TWN     :'data.frame': 63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] NA 3959 5529 7867 7027 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] NA 191 391 448 543 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] NA 1767 2148 2804 3848 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] NA 141 149 203 224 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] NA 449 620 870 1233 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] NA 1667 3035 4176 4264 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] NA 517 701 863 950 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] NA 720 926 1225 1360 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] NA 1567 1947 2377 3045 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] NA 355 442 520 590 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] NA 11334 15886 21354 23083 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ TZA     :'data.frame': 52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] 2696 2727 2969 3332 3394 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] 334 353 327 282 385 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] 1503 1912 2104 2131 2322 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] 98.5 115 123.2 131.4 147.8 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] 380 480 504 513 604 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] 1881 1992 2142 2291 2571 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] 700 692 756 756 805 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] 510 540 581 622 698 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] 1702 1978 2224 2239 2423 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] 458 497 516 575 621 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] 10263 11286 12246 12873 13970 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ USA     :'data.frame': 65 obs. of  11 variables:
#>   .. ..$ AGR : num [1:65] 19219 22268 18045 19217 22150 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:65] 5688 7922 6849 7901 8676 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:65] 59475 67616 65369 76046 89611 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:65] 4072 5160 5160 5904 6994 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:65] 7362 8341 9495 10481 12045 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:65] 52321 56739 56224 60339 66545 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:65] 18304 20651 20313 22658 25554 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:65] 35003 38985 42001 45917 50686 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:65] 38109 37691 39314 39649 48780 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:65] 4534 4915 5062 5421 5915 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:65] 244086 270289 267831 293534 336958 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ VEN     :'data.frame': 63 obs. of  11 variables:
#>   .. ..$ AGR : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:63] NA NA NA NA NA NA NA NA NA NA ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ ZAF     :'data.frame': 52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] 559 608 622 678 646 687 774 950 863 907 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] 618 638 674 702 757 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] 1001 1088 1166 1336 1513 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] 120 131 141 153 165 177 196 222 246 271 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] 149 144 154 180 234 291 315 337 366 414 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] 694 715 783 883 973 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] 495 504 548 610 675 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] 260 284 292 314 374 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] 446 477 511 564 618 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] 378 401 421 446 479 513 557 598 647 699 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] 4720 4990 5312 5866 6434 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   ..$ ZMB     :'data.frame': 52 obs. of  11 variables:
#>   .. ..$ AGR : num [1:52] 54.7 58.8 56.2 62.2 64.4 ...
#>   .. .. ..- attr(*, "label")= chr "Agriculture "
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MIN : num [1:52] 410 371 359 368 421 ...
#>   .. .. ..- attr(*, "label")= chr "Mining"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ MAN : num [1:52] 6.79 8.51 8.59 9.81 11.53 ...
#>   .. .. ..- attr(*, "label")= chr "Manufacturing"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ PU  : num [1:52] 26.4 24.4 25.1 27 22.5 ...
#>   .. .. ..- attr(*, "label")= chr "Utilities"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ CON : num [1:52] 30.6 26.4 23.7 24.9 30 ...
#>   .. .. ..- attr(*, "label")= chr "Construction"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ WRT : num [1:52] 48.4 48.1 51.4 51.7 75.4 ...
#>   .. .. ..- attr(*, "label")= chr "Trade, restaurants and hotels"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ TRA : num [1:52] 58.9 55.5 55.5 56.1 58.4 ...
#>   .. .. ..- attr(*, "label")= chr "Transport, storage and communication"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ FIRE: num [1:52] 5.46 5.21 5.59 5.94 5.58 ...
#>   .. .. ..- attr(*, "label")= chr "Finance, insurance, real estate and business services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ GOV : num [1:52] 47.5 52.6 57.8 60.3 68 ...
#>   .. .. ..- attr(*, "label")= chr "Government services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ OTH : num [1:52] 5.09 5.18 5.27 5.75 5.87 ...
#>   .. .. ..- attr(*, "label")= chr "Community, social and personal services"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"
#>   .. ..$ SUM : num [1:52] 693 656 648 672 763 ...
#>   .. .. ..- attr(*, "label")= chr "Summation of sector GDP"
#>   .. .. ..- attr(*, "format.stata")= chr "%10.0g"

# The nested list can be reassembled using unlist2d()
head(unlist2d(GGDCspl, idcols = .c(Variable, Country)))
#>   Variable Country      AGR      MIN      MAN       PU      CON      WRT
#> 1      EMP     ARG 1799.565 32.71936 1603.249 39.26323 314.1059 889.9666
#> 2      EMP     ARG 1835.181 34.37387 1640.927 42.43318 353.4173 879.7295
#> 3      EMP     ARG 1730.611 35.55357 1690.117 49.16048 311.3910 932.2140
#> 4      EMP     ARG 2029.762 33.83809 1578.124 52.20825 291.6067 903.6310
#> 5      EMP     ARG 1889.316 33.34185 1721.997 57.66520 330.1971 913.8400
#>        TRA     FIRE      GOV      OTH      SUM
#> 1 425.3517 203.8384 824.9212 410.8922 6543.872
#> 2 427.9257 204.0642 817.7359 411.4769 6647.265
#> 3 461.5084 218.9973 881.0104 441.7270 6752.291
#> 4 455.3023 209.7324 870.0849 434.6854 6858.976
#> 5 468.6230 213.7772 893.7040 444.8849 6967.347
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
rm(GGDCspl)

# Another example with mtcars (not as clean because of row.names)
nl <- rsplit(mtcars, mpg + hp ~ cyl + vs + am)
str(nl)
#> List of 3
#>  $ 4:List of 2
#>   ..$ 0:List of 1
#>   .. ..$ 1:'data.frame': 1 obs. of  2 variables:
#>   .. .. ..$ mpg: num 26
#>   .. .. ..$ hp : num 91
#>   ..$ 1:List of 2
#>   .. ..$ 0:'data.frame': 3 obs. of  2 variables:
#>   .. .. ..$ mpg: num [1:3] 24.4 22.8 21.5
#>   .. .. ..$ hp : num [1:3] 62 95 97
#>   .. ..$ 1:'data.frame': 7 obs. of  2 variables:
#>   .. .. ..$ mpg: num [1:7] 22.8 32.4 30.4 33.9 27.3 30.4 21.4
#>   .. .. ..$ hp : num [1:7] 93 66 52 65 66 113 109
#>  $ 6:List of 2
#>   ..$ 0:List of 1
#>   .. ..$ 1:'data.frame': 3 obs. of  2 variables:
#>   .. .. ..$ mpg: num [1:3] 21 21 19.7
#>   .. .. ..$ hp : num [1:3] 110 110 175
#>   ..$ 1:List of 1
#>   .. ..$ 0:'data.frame': 4 obs. of  2 variables:
#>   .. .. ..$ mpg: num [1:4] 21.4 18.1 19.2 17.8
#>   .. .. ..$ hp : num [1:4] 110 105 123 123
#>  $ 8:List of 1
#>   ..$ 0:List of 2
#>   .. ..$ 0:'data.frame': 12 obs. of  2 variables:
#>   .. .. ..$ mpg: num [1:12] 18.7 14.3 16.4 17.3 15.2 10.4 10.4 14.7 15.5 15.2 ...
#>   .. .. ..$ hp : num [1:12] 175 245 180 180 180 205 215 230 150 150 ...
#>   .. ..$ 1:'data.frame': 2 obs. of  2 variables:
#>   .. .. ..$ mpg: num [1:2] 15.8 15
#>   .. .. ..$ hp : num [1:2] 264 335
unlist2d(nl, idcols = .c(cyl, vs, am), row.names = "car")
#>    cyl vs am            car  mpg  hp
#> 1    4  0  1  Porsche 914-2 26.0  91
#> 2    4  1  0      Merc 240D 24.4  62
#> 3    4  1  0       Merc 230 22.8  95
#> 4    4  1  0  Toyota Corona 21.5  97
#> 5    4  1  1     Datsun 710 22.8  93
#> 6    4  1  1       Fiat 128 32.4  66
#> 7    4  1  1    Honda Civic 30.4  52
#> 8    4  1  1 Toyota Corolla 33.9  65
#> 9    4  1  1      Fiat X1-9 27.3  66
#> 10   4  1  1   Lotus Europa 30.4 113
#> 11   4  1  1     Volvo 142E 21.4 109
#>  [ reached 'max' / getOption("max.print") -- omitted 21 rows ]
rm(nl)
```
