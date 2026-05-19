# Fast Row/Column Arithmetic for Matrix-Like Objects

Fast operators to perform row- or column-wise replacing and sweeping
operations of vectors on matrices, data frames, lists. See also
[`setop`](https://fastverse.org/collapse/reference/efficient-programming.md)
for math by reference and
[`setTRA`](https://fastverse.org/collapse/reference/TRA.md) for sweeping
by reference.

## Usage

``` r
## Perform the operation with v and each row of X

X %rr% v    # Replace rows of X with v
X %r+% v    # Add v to each row of X
X %r-% v    # Subtract v from each row of X
X %r*% v    # Multiply each row of X with v
X %r/% v    # Divide each row of X by v

## Perform a column-wise operation between V and X

X %cr% V    # Replace columns of X with V
X %c+% V    # Add V to columns of X
X %c-% V    # Subtract V from columns of X
X %c*% V    # Multiply columns of X with V
X %c/% V    # Divide columns of X by V
```

## Arguments

- X:

  a vector, matrix, data frame or list like object (with rows (r)
  columns (c) matching `v` / `V`).

- v:

  for row operations: an atomic vector of matching `NCOL(X)`. If `X` is
  a data frame, `v` can also be a list of scalar atomic elements. It is
  also possible to sweep lists of vectors `v` out of lists of matrices
  or data frames `X`.

- V:

  for column operations: a suitable scalar, vector, or matrix / data
  frame matching `NROW(X)`. `X` can also be a list of vectors / matrices
  in which case `V` can be a scalar / vector / matrix or matching list
  of scalars / vectors / matrices.

## Details

With a matrix or data frame `X`, the default behavior of R when calling
`X op v` (such as multiplication `X * v`) is to perform the operation of
`v` with each column of `X`. The equivalent operation is performed by
`X %cop% V`, with the difference that it computes significantly faster
if `X`/`V` is a data frame / list. A more complex but frequently
required task is to perform an operation with `v` on each row of `X`.
This is provided based on efficient C++ code by the `%rop%` set of
functions, e.g. `X %r*% v` efficiently multiplies `v` to each row of
`X`.

## Value

`X` where the operation with `v` / `V` was performed on each row or
column. All attributes of `X` are preserved.

## Note

*Computations and Output:* These functions are all quite simple, they
only work with `X` on the LHS i.e. `v %op% X` will likely fail. The row
operations are simple wrappers around
[`TRA`](https://fastverse.org/collapse/reference/TRA.md) which provides
more operations including grouped replacing and sweeping (where `v`
would be a matrix or data frame with less rows than `X` being mapped to
the rows of `X` by grouping vectors). One consequence is that just like
[`TRA`](https://fastverse.org/collapse/reference/TRA.md), row-wise
mathematical operations (+, -, \*, /) always yield numeric output, even
if both `X` and `v` may be integer. This is different for column-
operations which depend on base R and may also preserve integer data.

*Rules of Arithmetic:* Since these operators are defined as simple infix
functions, the normal rules of arithmetic are not respected. So
`a %c+% b %c*% c` evaluates as `(a %c+% b) %c*% c`. As with all chained
infix operations, they are just evaluated sequentially from left to
right.

*Performance Notes:* The function
[`setop`](https://fastverse.org/collapse/reference/efficient-programming.md)
and a related set of `%op=%` operators as well as the
[`setTRA`](https://fastverse.org/collapse/reference/TRA.md) function can
be used to perform these operations by reference, and are faster if
copies of the output are not required!! Furthermore, for Fast
Statistical Functions, using `fmedian(X, TRA = "-")` will be a tiny bit
faster than `X %r-% fmedian(X)`. Also use `fwithin(X)` for fast
centering using the mean, and `fscale(X)` for fast scaling and centering
or mean-preserving scaling.

## See also

[`setop`](https://fastverse.org/collapse/reference/efficient-programming.md),
[`TRA`](https://fastverse.org/collapse/reference/TRA.md),
[`dapply`](https://fastverse.org/collapse/reference/dapply.md),
[Efficient
Programming](https://fastverse.org/collapse/reference/efficient-programming.md),
[Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Using data frame's / lists
v <- mtcars$cyl
mtcars %cr% v
#>                   mpg cyl disp hp drat wt qsec vs am gear carb
#> Mazda RX4           6   6    6  6    6  6    6  6  6    6    6
#> Mazda RX4 Wag       6   6    6  6    6  6    6  6  6    6    6
#> Datsun 710          4   4    4  4    4  4    4  4  4    4    4
#> Hornet 4 Drive      6   6    6  6    6  6    6  6  6    6    6
#> Hornet Sportabout   8   8    8  8    8  8    8  8  8    8    8
#> Valiant             6   6    6  6    6  6    6  6  6    6    6
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
mtcars %c-% v
#>                    mpg cyl disp  hp  drat     wt  qsec vs am gear carb
#> Mazda RX4         15.0   0  154 104 -2.10 -3.380 10.46 -6 -5   -2   -2
#> Mazda RX4 Wag     15.0   0  154 104 -2.10 -3.125 11.02 -6 -5   -2   -2
#> Datsun 710        18.8   0  104  89 -0.15 -1.680 14.61 -3 -3    0   -3
#> Hornet 4 Drive    15.4   0  252 104 -2.92 -2.785 13.44 -5 -6   -3   -5
#> Hornet Sportabout 10.7   0  352 167 -4.85 -4.560  9.02 -8 -8   -5   -6
#> Valiant           12.1   0  219  99 -3.24 -2.540 14.22 -5 -6   -3   -5
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
mtcars %r-% seq_col(mtcars)
#>                    mpg cyl disp  hp  drat     wt  qsec vs am gear carb
#> Mazda RX4         20.0   4  157 106 -1.10 -3.380  9.46 -8 -8   -6   -7
#> Mazda RX4 Wag     20.0   4  157 106 -1.10 -3.125 10.02 -8 -8   -6   -7
#> Datsun 710        21.8   2  105  89 -1.15 -3.680 11.61 -7 -8   -6  -10
#> Hornet 4 Drive    20.4   4  255 106 -1.92 -2.785 12.44 -7 -9   -7  -10
#> Hornet Sportabout 17.7   6  357 171 -1.85 -2.560 10.02 -8 -9   -7   -9
#> Valiant           17.1   4  222 101 -2.24 -2.540 13.22 -7 -9   -7  -10
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
mtcars %r-% lapply(mtcars, quantile, 0.28)
#>                     mpg cyl    disp    hp    drat     wt    qsec vs am gear
#> Mazda RX4         5.296   2  25.536  7.56  0.7724 -0.102 -0.5216  0  1    1
#> Mazda RX4 Wag     5.296   2  25.536  7.56  0.7724  0.153  0.0384  0  1    1
#> Datsun 710        7.096   0 -26.464 -9.44  0.7224 -0.402  1.6284  1  1    1
#> Hornet 4 Drive    5.696   2 123.536  7.56 -0.0476  0.493  2.4584  1  0    0
#> Hornet Sportabout 2.996   4 225.536 72.56  0.0224  0.718  0.0384  0  0    0
#> Valiant           2.396   2  90.536  2.56 -0.3676  0.738  3.2384  1  0    0
#>                   carb
#> Mazda RX4            2
#> Mazda RX4 Wag        2
#> Datsun 710          -1
#> Hornet 4 Drive      -1
#> Hornet Sportabout    0
#> Valiant             -1
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]

mtcars %c*% 5       # Significantly faster than mtcars * 5
#>                     mpg cyl disp  hp  drat     wt   qsec vs am gear carb
#> Mazda RX4         105.0  30  800 550 19.50 13.100  82.30  0  5   20   20
#> Mazda RX4 Wag     105.0  30  800 550 19.50 14.375  85.10  0  5   20   20
#> Datsun 710        114.0  20  540 465 19.25 11.600  93.05  5  5   20    5
#> Hornet 4 Drive    107.0  30 1290 550 15.40 16.075  97.20  5  0   15    5
#> Hornet Sportabout  93.5  40 1800 875 15.75 17.200  85.10  0  0   15   10
#> Valiant            90.5  30 1125 525 13.80 17.300 101.10  5  0   15    5
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
mtcars %c*% mtcars  # Significantly faster than mtcars * mtcars
#>                      mpg cyl   disp    hp    drat        wt     qsec vs am gear
#> Mazda RX4         441.00  36  25600 12100 15.2100  6.864400 270.9316  0  1   16
#> Mazda RX4 Wag     441.00  36  25600 12100 15.2100  8.265625 289.6804  0  1   16
#> Datsun 710        519.84  16  11664  8649 14.8225  5.382400 346.3321  1  1   16
#> Hornet 4 Drive    457.96  36  66564 12100  9.4864 10.336225 377.9136  1  0    9
#> Hornet Sportabout 349.69  64 129600 30625  9.9225 11.833600 289.6804  0  0    9
#> Valiant           327.61  36  50625 11025  7.6176 11.971600 408.8484  1  0    9
#>                   carb
#> Mazda RX4           16
#> Mazda RX4 Wag       16
#> Datsun 710           1
#> Hornet 4 Drive       1
#> Hornet Sportabout    4
#> Valiant              1
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]

## Using matrices
X <- qM(mtcars)
X %cr% v
#>                     mpg cyl disp hp drat wt qsec vs am gear carb
#> Mazda RX4             6   6    6  6    6  6    6  6  6    6    6
#> Mazda RX4 Wag         6   6    6  6    6  6    6  6  6    6    6
#> Datsun 710            4   4    4  4    4  4    4  4  4    4    4
#> Hornet 4 Drive        6   6    6  6    6  6    6  6  6    6    6
#> Hornet Sportabout     8   8    8  8    8  8    8  8  8    8    8
#> Valiant               6   6    6  6    6  6    6  6  6    6    6
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
X %c-% v
#>                      mpg cyl  disp  hp  drat     wt  qsec vs am gear carb
#> Mazda RX4           15.0   0 154.0 104 -2.10 -3.380 10.46 -6 -5   -2   -2
#> Mazda RX4 Wag       15.0   0 154.0 104 -2.10 -3.125 11.02 -6 -5   -2   -2
#> Datsun 710          18.8   0 104.0  89 -0.15 -1.680 14.61 -3 -3    0   -3
#> Hornet 4 Drive      15.4   0 252.0 104 -2.92 -2.785 13.44 -5 -6   -3   -5
#> Hornet Sportabout   10.7   0 352.0 167 -4.85 -4.560  9.02 -8 -8   -5   -6
#> Valiant             12.1   0 219.0  99 -3.24 -2.540 14.22 -5 -6   -3   -5
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
X %r-% dapply(X, quantile, 0.28)
#>                        mpg cyl    disp     hp    drat     wt    qsec vs am gear
#> Mazda RX4            5.296   2  25.536   7.56  0.7724 -0.102 -0.5216  0  1    1
#> Mazda RX4 Wag        5.296   2  25.536   7.56  0.7724  0.153  0.0384  0  1    1
#> Datsun 710           7.096   0 -26.464  -9.44  0.7224 -0.402  1.6284  1  1    1
#> Hornet 4 Drive       5.696   2 123.536   7.56 -0.0476  0.493  2.4584  1  0    0
#> Hornet Sportabout    2.996   4 225.536  72.56  0.0224  0.718  0.0384  0  0    0
#> Valiant              2.396   2  90.536   2.56 -0.3676  0.738  3.2384  1  0    0
#>                     carb
#> Mazda RX4              2
#> Mazda RX4 Wag          2
#> Datsun 710            -1
#> Hornet 4 Drive        -1
#> Hornet Sportabout      0
#> Valiant               -1
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]

## Chained Operations
library(magrittr) # Needed here to evaluate infix operators in sequence
mtcars %>% fwithin() %r-% rnorm(11) %c*% 5 %>%
    tfm(mpg = fsum(mpg)) %>% qsu()
#>        N       Mean        SD        Min        Max
#> mpg   32  -245.2386         0  -245.2386  -245.2386
#> cyl   32    -2.1457    8.9296   -13.0832     6.9168
#> disp  32    -0.6105  619.6935  -798.7199  1205.7801
#> hp    32     5.6901  342.8143  -467.7474   947.2526
#> drat  32     2.7901    2.6734    -1.3927     9.4573
#> wt    32    -5.2627    4.8923   -13.7839     5.7711
#> qsec  32    -3.3884    8.9347   -20.1322    21.8678
#> vs    32    -0.1925    2.5201      -2.38       2.62
#> am    32     1.7819     2.495    -0.2493     4.7507
#> gear  32    -3.9142     3.689    -7.3517     2.6483
#> carb  32    -4.0221     8.076   -13.0846    21.9154
```
