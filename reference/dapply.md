# Data Apply

`dapply` efficiently applies functions to columns or rows of matrix-like
objects and by default returns an object of the same type and with the
same attributes (unless the result is scalar and `drop = TRUE`).
Alternatively it is possible to return the result in a plain matrix or
data.frame. A simple parallelism is also available.

## Usage

``` r
dapply(X, FUN, ..., MARGIN = 2, parallel = FALSE, mc.cores = 1L,
       return = c("same", "matrix", "data.frame"), drop = TRUE)
```

## Arguments

- X:

  a matrix, data frame or alike object.

- FUN:

  a function, can be scalar- or vector-valued.

- ...:

  further arguments to `FUN`.

- MARGIN:

  integer. The margin which `FUN` will be applied over. Default `2`
  indicates columns while `1` indicates rows. See also Details.

- parallel:

  logical. `TRUE` implements simple parallel execution by internally
  calling `mclapply` instead of
  [`lapply`](https://rdrr.io/r/base/lapply.html).

- mc.cores:

  integer. Argument to `mclapply` indicating the number of cores to use
  for parallel execution. Can use `detectCores()` to select all
  available cores.

- return:

  an integer or string indicating the type of object to return. The
  default `1 - "same"` returns the same object type (i.e. class and
  other attributes are retained, just the names for the dimensions are
  adjusted). `2 - "matrix"` always returns the output as matrix and
  `3 - "data.frame"` always returns a data frame.

- drop:

  logical. If the result has only one row or one column, `drop = TRUE`
  will drop dimensions and return a (named) atomic vector.

## Details

`dapply` is an efficient command to apply functions to rows or columns
of data without loosing information (attributes) about the data or
changing the classes or format of the data. It is principally an
efficient wrapper around [`lapply`](https://rdrr.io/r/base/lapply.html)
and works as follows:

- Save the attributes of `X`.

- If `MARGIN = 2` (columns), convert matrices to plain lists of columns
  using
  [`mctl`](https://fastverse.org/collapse/reference/quick-conversion.md)
  and remove all attributes from data frames.

- If `MARGIN = 1` (rows), convert matrices to plain lists of rows using
  [`mrtl`](https://fastverse.org/collapse/reference/quick-conversion.md).
  For data frames remove all attributes, efficiently convert to matrix
  using `do.call(cbind, X)` and also convert to list of rows using
  [`mrtl`](https://fastverse.org/collapse/reference/quick-conversion.md).

- Call [`lapply`](https://rdrr.io/r/base/lapply.html) or `mclapply` on
  these plain lists (which is faster than calling `lapply` on an object
  with attributes).

- depending on the requested output type, use
  [`matrix`](https://rdrr.io/r/base/matrix.html),
  [`unlist`](https://rdrr.io/r/base/unlist.html) or
  [`do.call(cbind, ...)`](https://rdrr.io/r/base/do.call.html) to
  convert the result back to a matrix or list of columns.

- modify the relevant attributes accordingly and efficiently attach to
  the object again (no further checks).

The performance gain from working with plain lists makes `dapply` not
much slower than calling `lapply` itself on a data frame. Because of the
conversions involved, row-operations require some memory, but are still
faster than [`apply`](https://rdrr.io/r/base/apply.html).

## Value

`X` where `FUN` was applied to every row or column.

## See also

[`BY`](https://fastverse.org/collapse/reference/BY.md),
[`collap`](https://fastverse.org/collapse/reference/collap.md), [Fast
Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
head(dapply(mtcars, log))                      # Take natural log of each variable
#>                        mpg      cyl     disp       hp     drat        wt
#> Mazda RX4         3.044522 1.791759 5.075174 4.700480 1.360977 0.9631743
#> Mazda RX4 Wag     3.044522 1.791759 5.075174 4.700480 1.360977 1.0560527
#> Datsun 710        3.126761 1.386294 4.682131 4.532599 1.348073 0.8415672
#> Hornet 4 Drive    3.063391 1.791759 5.552960 4.700480 1.124930 1.1678274
#> Hornet Sportabout 2.928524 2.079442 5.886104 5.164786 1.147402 1.2354715
#> Valiant           2.895912 1.791759 5.416100 4.653960 1.015231 1.2412686
#>                       qsec   vs   am     gear      carb
#> Mazda RX4         2.800933 -Inf    0 1.386294 1.3862944
#> Mazda RX4 Wag     2.834389 -Inf    0 1.386294 1.3862944
#> Datsun 710        2.923699    0    0 1.386294 0.0000000
#> Hornet 4 Drive    2.967333    0 -Inf 1.098612 0.0000000
#> Hornet Sportabout 2.834389 -Inf -Inf 1.098612 0.6931472
#> Valiant           3.006672    0 -Inf 1.098612 0.0000000
head(dapply(mtcars, log, return = "matrix"))   # Return as matrix
#>                        mpg      cyl     disp       hp     drat        wt
#> Mazda RX4         3.044522 1.791759 5.075174 4.700480 1.360977 0.9631743
#> Mazda RX4 Wag     3.044522 1.791759 5.075174 4.700480 1.360977 1.0560527
#> Datsun 710        3.126761 1.386294 4.682131 4.532599 1.348073 0.8415672
#> Hornet 4 Drive    3.063391 1.791759 5.552960 4.700480 1.124930 1.1678274
#> Hornet Sportabout 2.928524 2.079442 5.886104 5.164786 1.147402 1.2354715
#> Valiant           2.895912 1.791759 5.416100 4.653960 1.015231 1.2412686
#>                       qsec   vs   am     gear      carb
#> Mazda RX4         2.800933 -Inf    0 1.386294 1.3862944
#> Mazda RX4 Wag     2.834389 -Inf    0 1.386294 1.3862944
#> Datsun 710        2.923699    0    0 1.386294 0.0000000
#> Hornet 4 Drive    2.967333    0 -Inf 1.098612 0.0000000
#> Hornet Sportabout 2.834389 -Inf -Inf 1.098612 0.6931472
#> Valiant           3.006672    0 -Inf 1.098612 0.0000000
m <- as.matrix(mtcars)
head(dapply(m, log))                           # Same thing
#>                        mpg      cyl     disp       hp     drat        wt
#> Mazda RX4         3.044522 1.791759 5.075174 4.700480 1.360977 0.9631743
#> Mazda RX4 Wag     3.044522 1.791759 5.075174 4.700480 1.360977 1.0560527
#> Datsun 710        3.126761 1.386294 4.682131 4.532599 1.348073 0.8415672
#> Hornet 4 Drive    3.063391 1.791759 5.552960 4.700480 1.124930 1.1678274
#> Hornet Sportabout 2.928524 2.079442 5.886104 5.164786 1.147402 1.2354715
#> Valiant           2.895912 1.791759 5.416100 4.653960 1.015231 1.2412686
#>                       qsec   vs   am     gear      carb
#> Mazda RX4         2.800933 -Inf    0 1.386294 1.3862944
#> Mazda RX4 Wag     2.834389 -Inf    0 1.386294 1.3862944
#> Datsun 710        2.923699    0    0 1.386294 0.0000000
#> Hornet 4 Drive    2.967333    0 -Inf 1.098612 0.0000000
#> Hornet Sportabout 2.834389 -Inf -Inf 1.098612 0.6931472
#> Valiant           3.006672    0 -Inf 1.098612 0.0000000
head(dapply(m, log, return = "data.frame"))    # Return data frame from matrix
#>                        mpg      cyl     disp       hp     drat        wt
#> Mazda RX4         3.044522 1.791759 5.075174 4.700480 1.360977 0.9631743
#> Mazda RX4 Wag     3.044522 1.791759 5.075174 4.700480 1.360977 1.0560527
#> Datsun 710        3.126761 1.386294 4.682131 4.532599 1.348073 0.8415672
#> Hornet 4 Drive    3.063391 1.791759 5.552960 4.700480 1.124930 1.1678274
#> Hornet Sportabout 2.928524 2.079442 5.886104 5.164786 1.147402 1.2354715
#> Valiant           2.895912 1.791759 5.416100 4.653960 1.015231 1.2412686
#>                       qsec   vs   am     gear      carb
#> Mazda RX4         2.800933 -Inf    0 1.386294 1.3862944
#> Mazda RX4 Wag     2.834389 -Inf    0 1.386294 1.3862944
#> Datsun 710        2.923699    0    0 1.386294 0.0000000
#> Hornet 4 Drive    2.967333    0 -Inf 1.098612 0.0000000
#> Hornet Sportabout 2.834389 -Inf -Inf 1.098612 0.6931472
#> Valiant           3.006672    0 -Inf 1.098612 0.0000000
dapply(mtcars, sum); dapply(m, sum)            # Computing sum of each column, return as vector
#>      mpg      cyl     disp       hp     drat       wt     qsec       vs 
#>  642.900  198.000 7383.100 4694.000  115.090  102.952  571.160   14.000 
#>       am     gear     carb 
#>   13.000  118.000   90.000 
#>      mpg      cyl     disp       hp     drat       wt     qsec       vs 
#>  642.900  198.000 7383.100 4694.000  115.090  102.952  571.160   14.000 
#>       am     gear     carb 
#>   13.000  118.000   90.000 
dapply(mtcars, sum, drop = FALSE)              # This returns a data frame of 1 row
#>     mpg cyl   disp   hp   drat      wt   qsec vs am gear carb
#> 1 642.9 198 7383.1 4694 115.09 102.952 571.16 14 13  118   90
dapply(mtcars, sum, MARGIN = 1)                # Compute row-sum of each column, return as vector
#>           Mazda RX4       Mazda RX4 Wag          Datsun 710      Hornet 4 Drive 
#>             328.980             329.795             259.580             426.135 
#>   Hornet Sportabout             Valiant          Duster 360           Merc 240D 
#>             590.310             385.540             656.920             270.980 
#>            Merc 230            Merc 280           Merc 280C          Merc 450SE 
#>             299.570             350.460             349.660             510.740 
#>          Merc 450SL         Merc 450SLC  Cadillac Fleetwood Lincoln Continental 
#>             511.500             509.850             728.560             726.644 
#>   Chrysler Imperial            Fiat 128         Honda Civic      Toyota Corolla 
#>             725.695             213.850             195.165             206.955 
#>       Toyota Corona    Dodge Challenger         AMC Javelin          Camaro Z28 
#>             273.775             519.650             506.085             646.280 
#>    Pontiac Firebird           Fiat X1-9       Porsche 914-2        Lotus Europa 
#>             631.175             208.215             272.570             273.683 
#>      Ford Pantera L        Ferrari Dino       Maserati Bora          Volvo 142E 
#>             670.690             379.590             694.710             288.890 
dapply(m, sum, MARGIN = 1)                     # Same thing for matrices, faster t. apply(m, 1, sum)
#>           Mazda RX4       Mazda RX4 Wag          Datsun 710      Hornet 4 Drive 
#>             328.980             329.795             259.580             426.135 
#>   Hornet Sportabout             Valiant          Duster 360           Merc 240D 
#>             590.310             385.540             656.920             270.980 
#>            Merc 230            Merc 280           Merc 280C          Merc 450SE 
#>             299.570             350.460             349.660             510.740 
#>          Merc 450SL         Merc 450SLC  Cadillac Fleetwood Lincoln Continental 
#>             511.500             509.850             728.560             726.644 
#>   Chrysler Imperial            Fiat 128         Honda Civic      Toyota Corolla 
#>             725.695             213.850             195.165             206.955 
#>       Toyota Corona    Dodge Challenger         AMC Javelin          Camaro Z28 
#>             273.775             519.650             506.085             646.280 
#>    Pontiac Firebird           Fiat X1-9       Porsche 914-2        Lotus Europa 
#>             631.175             208.215             272.570             273.683 
#>      Ford Pantera L        Ferrari Dino       Maserati Bora          Volvo 142E 
#>             670.690             379.590             694.710             288.890 
head(dapply(m, sum, MARGIN = 1, drop = FALSE)) # Gives matrix with one column
#>                       sum
#> Mazda RX4         328.980
#> Mazda RX4 Wag     329.795
#> Datsun 710        259.580
#> Hornet 4 Drive    426.135
#> Hornet Sportabout 590.310
#> Valiant           385.540
head(dapply(m, quantile, MARGIN = 1))          # Compute row-quantiles
#>                   0%    25%   50%    75% 100%
#> Mazda RX4          0 3.2600 4.000 18.730  160
#> Mazda RX4 Wag      0 3.3875 4.000 19.010  160
#> Datsun 710         1 1.6600 4.000 20.705  108
#> Hornet 4 Drive     0 2.0000 3.215 20.420  258
#> Hornet Sportabout  0 2.5000 3.440 17.860  360
#> Valiant            0 1.8800 3.460 19.160  225
dapply(m, quantile)                            # Column-quantiles
#>         mpg cyl    disp    hp  drat      wt    qsec vs am gear carb
#> 0%   10.400   4  71.100  52.0 2.760 1.51300 14.5000  0  0    3    1
#> 25%  15.425   4 120.825  96.5 3.080 2.58125 16.8925  0  0    3    2
#> 50%  19.200   6 196.300 123.0 3.695 3.32500 17.7100  0  0    4    2
#> 75%  22.800   8 326.000 180.0 3.920 3.61000 18.9000  1  1    4    4
#> 100% 33.900   8 472.000 335.0 4.930 5.42400 22.9000  1  1    5    8
head(dapply(mtcars, quantile, MARGIN = 1))     # Same for data frames, output is also a data.frame
#>                   0%    25%   50%    75% 100%
#> Mazda RX4          0 3.2600 4.000 18.730  160
#> Mazda RX4 Wag      0 3.3875 4.000 19.010  160
#> Datsun 710         1 1.6600 4.000 20.705  108
#> Hornet 4 Drive     0 2.0000 3.215 20.420  258
#> Hornet Sportabout  0 2.5000 3.440 17.860  360
#> Valiant            0 1.8800 3.460 19.160  225
dapply(mtcars, quantile)
#>         mpg cyl    disp    hp  drat      wt    qsec vs am gear carb
#> 0%   10.400   4  71.100  52.0 2.760 1.51300 14.5000  0  0    3    1
#> 25%  15.425   4 120.825  96.5 3.080 2.58125 16.8925  0  0    3    2
#> 50%  19.200   6 196.300 123.0 3.695 3.32500 17.7100  0  0    4    2
#> 75%  22.800   8 326.000 180.0 3.920 3.61000 18.9000  1  1    4    4
#> 100% 33.900   8 472.000 335.0 4.930 5.42400 22.9000  1  1    5    8

# With classed objects, we have to be a bit careful
if (FALSE) { # \dontrun{
dapply(EuStockMarkets, quantile)  # This gives an error because the tsp attribute is misspecified
} # }
dapply(EuStockMarkets, quantile, return = "matrix")    # These both work fine..
#>           DAX      SMI     CAC     FTSE
#> 0%   1402.340 1587.400 1611.00 2281.000
#> 25%  1744.102 2165.625 1875.15 2843.150
#> 50%  2140.565 2796.350 1992.30 3246.600
#> 75%  2722.367 3812.425 2274.35 3993.575
#> 100% 6186.090 8412.000 4388.50 6179.000
dapply(EuStockMarkets, quantile, return = "data.frame")
#>           DAX      SMI     CAC     FTSE
#> 0%   1402.340 1587.400 1611.00 2281.000
#> 25%  1744.102 2165.625 1875.15 2843.150
#> 50%  2140.565 2796.350 1992.30 3246.600
#> 75%  2722.367 3812.425 2274.35 3993.575
#> 100% 6186.090 8412.000 4388.50 6179.000
 
# Similarly for grouped tibbles and other data frame based classes
library(dplyr)
gmtcars <- group_by(mtcars,cyl,vs,am)
head(dapply(gmtcars, log))               # Still gives a grouped tibble back
#> # A tibble: 6 × 11
#> # Groups:   cyl, vs, am [4]
#>     mpg   cyl  disp    hp  drat    wt  qsec    vs    am  gear  carb
#>   <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl>
#> 1  3.04  1.79  5.08  4.70  1.36 0.963  2.80  -Inf     0  1.39 1.39 
#> 2  3.04  1.79  5.08  4.70  1.36 1.06   2.83  -Inf     0  1.39 1.39 
#> 3  3.13  1.39  4.68  4.53  1.35 0.842  2.92     0     0  1.39 0    
#> 4  3.06  1.79  5.55  4.70  1.12 1.17   2.97     0  -Inf  1.10 0    
#> 5  2.93  2.08  5.89  5.16  1.15 1.24   2.83  -Inf  -Inf  1.10 0.693
#> 6  2.90  1.79  5.42  4.65  1.02 1.24   3.01     0  -Inf  1.10 0    
dapply(gmtcars, quantile, MARGIN = 1)    # Here it makes sense to keep the groups attribute
#> # A tibble: 32 × 5
#> # Groups:   cyl, vs, am [7]
#>     `0%` `25%` `50%` `75%` `100%`
#>  * <dbl> <dbl> <dbl> <dbl>  <dbl>
#>  1     0  3.26  4     18.7   160 
#>  2     0  3.39  4     19.0   160 
#>  3     1  1.66  4     20.7   108 
#>  4     0  2     3.22  20.4   258 
#>  5     0  2.5   3.44  17.9   360 
#>  6     0  1.88  3.46  19.2   225 
#>  7     0  3.10  4     15.1   360 
#>  8     0  2.60  4     22.2   147.
#>  9     0  2.58  4     22.8   141.
#> 10     0  3.68  4     18.8   168.
#> # ℹ 22 more rows
dapply(gmtcars, quantile)                # This does not make much sense, ...
#> # A tibble: 5 × 11
#> # Groups:   cyl, vs, am [7]
#>     mpg   cyl  disp    hp  drat    wt  qsec    vs    am  gear  carb
#> * <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl>
#> 1  10.4     4  71.1  52    2.76  1.51  14.5     0     0     3     1
#> 2  15.4     4 121.   96.5  3.08  2.58  16.9     0     0     3     2
#> 3  19.2     6 196.  123    3.70  3.32  17.7     0     0     4     2
#> 4  22.8     8 326   180    3.92  3.61  18.9     1     1     4     4
#> 5  33.9     8 472   335    4.93  5.42  22.9     1     1     5     8
dapply(gmtcars, quantile,                # better convert to plain data.frame:
       return = "data.frame")
#>         mpg cyl    disp    hp  drat      wt    qsec vs am gear carb
#> 0%   10.400   4  71.100  52.0 2.760 1.51300 14.5000  0  0    3    1
#> 25%  15.425   4 120.825  96.5 3.080 2.58125 16.8925  0  0    3    2
#> 50%  19.200   6 196.300 123.0 3.695 3.32500 17.7100  0  0    4    2
#> 75%  22.800   8 326.000 180.0 3.920 3.61000 18.9000  1  1    4    4
#> 100% 33.900   8 472.000 335.0 4.930 5.42400 22.9000  1  1    5    8
```
