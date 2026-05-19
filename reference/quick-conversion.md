# Quick Data Conversion

Fast, flexible and precise conversion of common data objects, without
method dispatch and extensive checks:

- `qDF`, `qDT` and `qTBL` convert vectors, matrices, higher-dimensional
  arrays and suitable lists to data frame, *data.table* and *tibble*,
  respectively.

- `qM` converts vectors, higher-dimensional arrays, data frames and
  suitable lists to matrix.

- `mctl` and `mrtl` column- or row-wise convert a matrix to list, data
  frame or *data.table*. They are used internally by `qDF/qDT/qTBL`,
  [`dapply`](https://fastverse.org/collapse/reference/dapply.md),
  [`BY`](https://fastverse.org/collapse/reference/BY.md), etc...

- [`qF`](https://fastverse.org/collapse/reference/qF.md) converts atomic
  vectors to factor (documented on a separate page).

- `as_numeric_factor`, `as_integer_factor`, and `as_character_factor`
  convert factors, or all factor columns in a data frame / list, to
  character or numeric (by converting the levels).

## Usage

``` r
# Converting between matrices, data frames / tables / tibbles

 qDF(X, row.names.col = FALSE, keep.attr = FALSE, class = "data.frame")
 qDT(X, row.names.col = FALSE, keep.attr = FALSE, class = c("data.table", "data.frame"))
qTBL(X, row.names.col = FALSE, keep.attr = FALSE, class = c("tbl_df","tbl","data.frame"))
  qM(X, row.names.col = NULL , keep.attr = FALSE, class = NULL, sep = ".")

# Programmer functions: matrix rows or columns to list / DF / DT - fully in C++

mctl(X, names = FALSE, return = "list")
mrtl(X, names = FALSE, return = "list")

# Converting factors or factor columns

  as_numeric_factor(X, keep.attr = TRUE)
  as_integer_factor(X, keep.attr = TRUE)
as_character_factor(X, keep.attr = TRUE)
```

## Arguments

- X:

  a vector, factor, matrix, higher-dimensional array, data frame or
  list. `mctl` and `mrtl` only accept matrices, `as_numeric_factor`,
  `as_integer_factor` and `as_character_factor` only accept factors,
  data frames or lists.

- row.names.col:

  can be used to add an column saving names or row.names when converting
  objects to data frame using `qDF/qDT/qTBL`. `TRUE` will add a column
  `"row.names"`, or you can supply a name e.g.
  `row.names.col = "variable"`. If `X` is a named atomic vector, a
  length 2 vector of names can be supplied, e.g.,
  `qDF(fmean(mtcars), c("car", "mean"))`. With `qM`, the argument has
  the opposite meaning, and can be used to select one or more columns in
  a data frame/list which will be used to create the rownames of the
  matrix e.g. `qM(iris, row.names.col = "Species")`. In this case the
  column(s) can be specified using names, indices, a logical vector or a
  selector function. See Examples.

- keep.attr:

  logical. `FALSE` (default) yields a *hard* / *thorough* object
  conversion: All unnecessary attributes are removed from the object
  yielding a plain matrix / data.frame / *data.table*. `FALSE` yields a
  *soft* / *minimal* object conversion: Only the attributes 'names',
  'row.names', 'dim', 'dimnames' and 'levels' are modified in the
  conversion. Other attributes are preserved. See also `class`.

- class:

  if a vector of classes is passed here, the converted object will be
  assigned these classes. If `NULL` is passed, the default classes are
  assigned: `qM` assigns no class, `qDF` a class `"data.frame"`, and
  `qDT` a class `c("data.table", "data.frame")`. If `keep.attr = TRUE`
  and `class = NULL` and the object already inherits the default
  classes, further inherited classes are preserved. See Details and the
  Example.

- sep:

  character. Separator used for interacting multiple variables selected
  through `row.names.col`.

- names:

  logical. Should the list be named using row/column names from the
  matrix?

- return:

  an integer or string specifying what to return. The options are:

  |        |     |              |     |                              |
  |--------|-----|--------------|-----|------------------------------|
  | *Int.* |     | *String*     |     | *Description*                |
  | 1      |     | "list"       |     | returns a plain list         |
  | 2      |     | "data.frame" |     | returns a plain data.frame   |
  | 3      |     | "data.table" |     | returns a plain *data.table* |

## Details

Object conversions using these functions are maximally efficient and
involve 3 consecutive steps: (1) Converting the storage mode /
dimensions / data of the object, (2) converting / modifying the
attributes and (3) modifying the class of the object:

\(1\) is determined by the choice of function and the optional
`row.names.col` argument. Higher-dimensional arrays are converted by
expanding the second dimension (adding columns, same as
`as.matrix, as.data.frame, as.data.table`).

\(2\) is determined by the `keep.attr` argument: `keep.attr = TRUE`
seeks to preserve the attributes of the object. Its effect is like
copying `attributes(converted) <- attributes(original)`, and then
modifying the `"dim", "dimnames", "names", "row.names"` and `"levels"`
attributes as necessitated by the conversion task. `keep.attr = FALSE`
only converts / assigns / removes these attributes and drops all others.

\(3\) is determined by the `class` argument: Setting `class = "myclass"`
will yield a converted object of class `"myclass"`, with any other /
prior classes being removed by this replacement. Setting `class = NULL`
does NOT mean that a class `NULL` is assigned (which would remove the
class attribute), but rather that the default classes are assigned: `qM`
assigns no class, `qDF` a class `"data.frame"`, and `qDT` a class
`c("data.table", "data.frame")`. At this point there is an interaction
with `keep.attr`: If `keep.attr = TRUE` and `class = NULL` and the
object converted already inherits the respective default classes, then
any other inherited classes will also be preserved (with
`qM(x, keep.attr = TRUE, class = NULL)` any class will be preserved if
`is.matrix(x)` evaluates to `TRUE`.)

The default `keep.attr = FALSE` ensures *hard* conversions so that all
unnecessary attributes are dropped. Furthermore in `qDF/qDT/qTBL` the
default classes were explicitly assigned. This is to ensure that the
default methods apply, even if the user chooses to preserve further
attributes. For `qM` a more lenient default setup was chosen to enable
the full preservation of time series matrices with `keep.attr = TRUE`.
If the user wants to keep attributes attached to a matrix but make sure
that all default methods work properly, either one of
`qM(x, keep.attr = TRUE, class = "matrix")` or
`unclass(qM(x, keep.attr = TRUE))` should be employed.

## Value

`qDF` - returns a data.frame  
`qDT` - returns a *data.table*  
`qTBL` - returns a *tibble*  
`qM` - returns a matrix  
`mctl`, `mrtl` - return a list, data frame or *data.table*  
`qF` - returns a factor  
`as_numeric_factor` - returns X with factors converted to numeric
(double) variables  
`as_integer_factor` - returns X with factors converted to integer
variables  
`as_character_factor` - returns X with factors converted to character
variables

## See also

[`qF`](https://fastverse.org/collapse/reference/qF.md), [Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Basic Examples
mtcarsM <- qM(mtcars)                   # Matrix from data.frame
mtcarsDT <- qDT(mtcarsM)                # data.table from matrix columns
mtcarsTBL <- qTBL(mtcarsM)              # tibble from matrix columns
head(mrtl(mtcarsM, TRUE, "data.frame")) # data.frame from matrix rows, etc..
#>     Mazda RX4 Mazda RX4 Wag Datsun 710 Hornet 4 Drive Hornet Sportabout Valiant
#> mpg        21            21       22.8           21.4              18.7    18.1
#> cyl         6             6        4.0            6.0               8.0     6.0
#>     Duster 360 Merc 240D Merc 230 Merc 280 Merc 280C Merc 450SE Merc 450SL
#> mpg       14.3      24.4     22.8     19.2      17.8       16.4       17.3
#> cyl        8.0       4.0      4.0      6.0       6.0        8.0        8.0
#>     Merc 450SLC Cadillac Fleetwood Lincoln Continental Chrysler Imperial
#> mpg        15.2               10.4                10.4              14.7
#> cyl         8.0                8.0                 8.0               8.0
#>     Fiat 128 Honda Civic Toyota Corolla Toyota Corona Dodge Challenger
#> mpg     32.4        30.4           33.9          21.5             15.5
#> cyl      4.0         4.0            4.0           4.0              8.0
#>     AMC Javelin Camaro Z28 Pontiac Firebird Fiat X1-9 Porsche 914-2
#> mpg        15.2       13.3             19.2      27.3            26
#> cyl         8.0        8.0              8.0       4.0             4
#>     Lotus Europa Ford Pantera L Ferrari Dino Maserati Bora Volvo 142E
#> mpg         30.4           15.8         19.7            15       21.4
#> cyl          4.0            8.0          6.0             8        4.0
#>  [ reached 'max' / getOption("max.print") -- omitted 4 rows ]
head(qDF(mtcarsM, "cars"))              # Adding a row.names column when converting from matrix
#>                cars  mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> 1         Mazda RX4 21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> 2     Mazda RX4 Wag 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> 3        Datsun 710 22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> 4    Hornet 4 Drive 21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> 5 Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
head(qDT(mtcars, "cars"))               # Saving row.names when converting data frame to data.table
#>                 cars   mpg   cyl  disp    hp  drat    wt  qsec    vs    am
#>               <char> <num> <num> <num> <num> <num> <num> <num> <num> <num>
#> 1:         Mazda RX4  21.0     6   160   110  3.90 2.620 16.46     0     1
#> 2:     Mazda RX4 Wag  21.0     6   160   110  3.90 2.875 17.02     0     1
#> 3:        Datsun 710  22.8     4   108    93  3.85 2.320 18.61     1     1
#> 4:    Hornet 4 Drive  21.4     6   258   110  3.08 3.215 19.44     1     0
#>     gear  carb
#>    <num> <num>
#> 1:     4     4
#> 2:     4     4
#> 3:     4     1
#> 4:     3     1
#>  [ reached 'max' / getOption("max.print") -- omitted 2 rows ]
head(qM(iris, "Species"))               # Examples converting data to matrix, saving information
#>        Sepal.Length Sepal.Width Petal.Length Petal.Width
#> setosa          5.1         3.5          1.4         0.2
#> setosa          4.9         3.0          1.4         0.2
#> setosa          4.7         3.2          1.3         0.2
#> setosa          4.6         3.1          1.5         0.2
#> setosa          5.0         3.6          1.4         0.2
#> setosa          5.4         3.9          1.7         0.4
head(qM(GGDC10S, is.character))         # as rownames
#>                               Year      AGR      MIN       MAN        PU
#> BWA.SSA.Sub-saharan Africa.VA 1960       NA       NA        NA        NA
#> BWA.SSA.Sub-saharan Africa.VA 1961       NA       NA        NA        NA
#> BWA.SSA.Sub-saharan Africa.VA 1962       NA       NA        NA        NA
#> BWA.SSA.Sub-saharan Africa.VA 1963       NA       NA        NA        NA
#> BWA.SSA.Sub-saharan Africa.VA 1964 16.30154 3.494075 0.7365696 0.1043936
#>                                     CON      WRT      TRA     FIRE      GOV
#> BWA.SSA.Sub-saharan Africa.VA        NA       NA       NA       NA       NA
#> BWA.SSA.Sub-saharan Africa.VA        NA       NA       NA       NA       NA
#> BWA.SSA.Sub-saharan Africa.VA        NA       NA       NA       NA       NA
#> BWA.SSA.Sub-saharan Africa.VA        NA       NA       NA       NA       NA
#> BWA.SSA.Sub-saharan Africa.VA 0.6600454 6.243732 1.658928 1.119194 4.822485
#>                                    OTH      SUM
#> BWA.SSA.Sub-saharan Africa.VA       NA       NA
#> BWA.SSA.Sub-saharan Africa.VA       NA       NA
#> BWA.SSA.Sub-saharan Africa.VA       NA       NA
#> BWA.SSA.Sub-saharan Africa.VA       NA       NA
#> BWA.SSA.Sub-saharan Africa.VA 2.341328 37.48229
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ]
head(qM(gv(GGDC10S, -(2:3)), 1:3, sep = "-")) # plm-style rownames
#>                  AGR      MIN       MAN        PU       CON      WRT      TRA
#> BWA-VA-1960       NA       NA        NA        NA        NA       NA       NA
#> BWA-VA-1961       NA       NA        NA        NA        NA       NA       NA
#> BWA-VA-1962       NA       NA        NA        NA        NA       NA       NA
#> BWA-VA-1963       NA       NA        NA        NA        NA       NA       NA
#> BWA-VA-1964 16.30154 3.494075 0.7365696 0.1043936 0.6600454 6.243732 1.658928
#> BWA-VA-1965 15.72700 2.495768 1.0181992 0.1350976 1.3462312 7.064825 1.939007
#>                 FIRE      GOV      OTH      SUM
#> BWA-VA-1960       NA       NA       NA       NA
#> BWA-VA-1961       NA       NA       NA       NA
#> BWA-VA-1962       NA       NA       NA       NA
#> BWA-VA-1963       NA       NA       NA       NA
#> BWA-VA-1964 1.119194 4.822485 2.341328 37.48229
#> BWA-VA-1965 1.246789 5.695848 2.678338 39.34710

qDF(fmean(mtcars), c("cars", "mean"))   # Data frame from named vector, with names
#>    cars       mean
#> 1   mpg  20.090625
#> 2   cyl   6.187500
#> 3  disp 230.721875
#> 4    hp 146.687500
#> 5  drat   3.596563
#> 6    wt   3.217250
#> 7  qsec  17.848750
#> 8    vs   0.437500
#> 9    am   0.406250
#> 10 gear   3.687500
#> 11 carb   2.812500

# mrtl() and mctl() are very useful for iteration over matrices
# Think of a coordninates matrix e.g. from sf::st_coordinates()
coord <- matrix(rnorm(10), ncol = 2, dimnames = list(NULL, c("X", "Y")))
# Then we can
for (d in mrtl(coord)) {
  cat("lon =", d[1], ", lat =", d[2], fill = TRUE)
  # do something complicated ...
}
#> lon = -1.34008 , lat = 0.4111546
#> lon = -0.9054664 , lat = 0.5640929
#> lon = -0.2121424 , lat = 0.06356228
#> lon = -0.778162 , lat = 0.5173632
#> lon = -0.1411845 , lat = 0.4395096
rm(coord)

## Factors
cylF <- qF(mtcars$cyl)                  # Factor from atomic vector
cylF
#>  [1] 6 6 4 6 8 6 8 4 4 6 6 8 8 8 8 8 8 4 4 4 4 8 8 8 8 4 4 4 8 6 8 4
#> Levels: 4 6 8

# Factor to numeric conversions
identical(mtcars,  as_numeric_factor(dapply(mtcars, qF)))
#> [1] TRUE
```
