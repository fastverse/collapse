# Fast Select, Replace or Add Data Frame Columns

Efficiently select and replace (or add) a subset of columns from (to) a
data frame. This can be done by data type, or using expressions, column
names, indices, logical vectors, selector functions or regular
expressions matching column names.

## Usage

``` r
## Select and replace variables, analgous to dplyr::select but significantly faster
fselect(.x, ..., return = "data")
fselect(x, ...) <- value
slt(.x, ..., return = "data")   # Shorthand for fselect
slt(x, ...) <- value            # Shorthand for fselect<-

## Select and replace columns by names, indices, logical vectors,
## regular expressions or using functions to identify columns

get_vars(x, vars, return = "data", regex = FALSE, rename = FALSE, ...)
      gv(x, vars, return = "data", ...)   # Shorthand for get_vars
     gvr(x, vars, return = "data", ...)   # Shorthand for get_vars(..., regex = TRUE)

get_vars(x, vars, regex = FALSE, ...) <- value
gv(x, vars, ...) <- value           # Shorthand for get_vars<-
gvr(x, vars, ...) <- value           # Shorthand for get_vars<-(..., regex = TRUE)

## Add columns at any position within a data.frame

add_vars(x, ..., pos = "end")
add_vars(x, pos = "end") <- value
      av(x, ..., pos = "end")             # Shorthand for add_vars
av(x, pos = "end") <- value         # Shorthand for add_vars<-

## Select and replace columns by data type

num_vars(x, return = "data")
num_vars(x) <- value
      nv(x, return = "data")       # Shorthand for num_vars
nv(x) <- value               # Shorthand for num_vars<-
cat_vars(x, return = "data")       # Categorical variables, see is_categorical
cat_vars(x) <- value
char_vars(x, return = "data")
char_vars(x) <- value
fact_vars(x, return = "data")
fact_vars(x) <- value
logi_vars(x, return = "data")
logi_vars(x) <- value
date_vars(x, return = "data")      # See is_date
date_vars(x) <- value
```

## Arguments

- x, .x:

  a data frame or list.

- value:

  a data frame or list of columns whose dimensions exactly match those
  of the extracted subset of `x`. If only 1 variable is in the subset of
  `x`, `value` can also be an atomic vector or matrix, provided that
  `NROW(value) == nrow(x)`.

- vars:

  a vector of column names, indices (can be negative), a suitable
  logical vector, or a vector of regular expressions matching column
  names (if `regex = TRUE`). It is also possible to pass a function
  returning `TRUE` or `FALSE` when applied to the columns of `x`.

- return:

  an integer or string specifying what the selector function should
  return. The options are:

  |        |     |                 |     |                                |
  |--------|-----|-----------------|-----|--------------------------------|
  | *Int.* |     | *String*        |     | *Description*                  |
  | 1      |     | "data"          |     | subset of data frame (default) |
  | 2      |     | "names"         |     | column names                   |
  | 3      |     | "indices"       |     | column indices                 |
  | 4      |     | "named_indices" |     | named column indices           |
  | 5      |     | "logical"       |     | logical selection vector       |
  | 6      |     | "named_logical" |     | named logical vector           |

  *Note*: replacement functions only replace data, however column names
  are replaced together with the data (if available).

- regex:

  logical. `TRUE` will do regular expression search on the column names
  of `x` using a (vector of) regular expression(s) passed to `vars`.
  Matching is done using [`grep`](https://rdrr.io/r/base/grep.html).

- rename:

  logical. If `vars` is a named vector of column names or indices,
  `rename = TRUE` will use the (non missing) names to rename columns.

- pos:

  the position where columns are added in the data frame. `"end"`
  (default) will append the data frame at the end (right) side. "front"
  will add columns in front (left). Alternatively one can pass a vector
  of positions (matching `length(value)` if value is a list). In that
  case the other columns will be shifted around the new ones while
  maintaining their order.

- ...:

  for `fselect`: column names and expressions e.g.
  `fselect(mtcars, newname = mpg, hp, carb:vs)`. for `get_vars`: further
  arguments passed to [`grep`](https://rdrr.io/r/base/grep.html), if
  `regex = TRUE`. For `add_vars`: multiple lists/data frames or vectors
  (which should be given names e.g. `name = vector`). A single argument
  passed may also be an (unnamed) vector or matrix.

## Details

`get_vars(<-)` is around 2x faster than `` `[.data.frame` `` and 8x
faster than `` `[<-.data.frame` ``, so the common operation
`data[cols] <- someFUN(data[cols])` can be made 10x more efficient
(abstracting from computations performed by `someFUN`) using
`get_vars(data, cols) <- someFUN(get_vars(data, cols))` or the shorthand
`gv(data, cols) <- someFUN(gv(data, cols))`.

Similarly type-wise operations like `data[sapply(data, is.numeric)]` or
`data[sapply(data, is.numeric)] <- value` are facilitated and more
efficient using `num_vars(data)` and `num_vars(data) <- value` or the
shortcuts `nv` and `nv<-` etc.

`fselect` provides an efficient alternative to
[`dplyr::select`](https://dplyr.tidyverse.org/reference/select.html),
allowing the selection of variables based on expressions evaluated
within the data frame, see Examples. It is about 100x faster than
[`dplyr::select`](https://dplyr.tidyverse.org/reference/select.html) but
also more simple as it does not provide special methods (except for 'sf'
and 'data.table' which are handled internally) .

Finally, `add_vars(data1, data2, data3, ...)` is a lot faster than
`cbind(data1, data2, data3, ...)`, and preserves the attributes of
`data1` (i.e. it is like adding columns to `data1`). The replacement
function `add_vars(data) <- someFUN(get_vars(data, cols))` efficiently
appends `data` with computed columns. The `pos` argument allows adding
columns at positions other than the end (right) of the data frame, see
Examples. *Note* that `add_vars` does not check duplicated column names
or `NULL` columns, and does not evaluate expressions in a data
environment, or replicate length 1 inputs like
[`cbind`](https://rdrr.io/pkg/data.table/man/cbindlist.html). All of
this is provided by
[`ftransform`](https://fastverse.org/collapse/reference/ftransform.md).

All functions introduced here perform their operations
class-independent. They all basically work like this: (1) save the
attributes of `x`, (2) unclass `x`, (3) subset, replace or append `x` as
a list, (4) modify the "names" component of the attributes of `x`
accordingly and (5) efficiently attach the attributes again to the
result from step (3). Thus they can freely be applied to data.table's,
grouped tibbles, panel data frames and other classes and will return an
object of exactly the same class and the same attributes.

## Note

In many cases functions here only check the length of the first column,
which is one of the reasons why they are so fast. When lists of
unequal-length columns are offered as replacements this yields a
malformed data frame (which will also print a warning in the console
i.e. you will notice that).

## See also

[`fsubset`](https://fastverse.org/collapse/reference/fsubset.md),
[`ftransform`](https://fastverse.org/collapse/reference/ftransform.md),
[`rowbind`](https://fastverse.org/collapse/reference/rowbind.md), [Data
Frame
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Wold Development Data
head(fselect(wlddev, Country = country, Year = year, ODA)) # Fast dplyr-like selecting
#>       Country Year       ODA
#> 1 Afghanistan 1960 116769997
#> 2 Afghanistan 1961 232080002
#> 3 Afghanistan 1962 112839996
#> 4 Afghanistan 1963 237720001
#> 5 Afghanistan 1964 295920013
#> 6 Afghanistan 1965 341839996
head(fselect(wlddev, -country, -year, -PCGDP))
#>   iso3c       date decade     region     income  OECD LIFEEX GINI       ODA
#> 1   AFG 1961-01-01   1960 South Asia Low income FALSE 32.446   NA 116769997
#> 2   AFG 1962-01-01   1960 South Asia Low income FALSE 32.962   NA 232080002
#> 3   AFG 1963-01-01   1960 South Asia Low income FALSE 33.471   NA 112839996
#> 4   AFG 1964-01-01   1960 South Asia Low income FALSE 33.971   NA 237720001
#> 5   AFG 1965-01-01   1960 South Asia Low income FALSE 34.463   NA 295920013
#> 6   AFG 1966-01-01   1960 South Asia Low income FALSE 34.948   NA 341839996
#>       POP
#> 1 8996973
#> 2 9169410
#> 3 9351441
#> 4 9543205
#> 5 9744781
#> 6 9956320
head(fselect(wlddev, country, year, PCGDP:ODA))
#>       country year PCGDP LIFEEX GINI       ODA
#> 1 Afghanistan 1960    NA 32.446   NA 116769997
#> 2 Afghanistan 1961    NA 32.962   NA 232080002
#> 3 Afghanistan 1962    NA 33.471   NA 112839996
#> 4 Afghanistan 1963    NA 33.971   NA 237720001
#> 5 Afghanistan 1964    NA 34.463   NA 295920013
#> 6 Afghanistan 1965    NA 34.948   NA 341839996
head(fselect(wlddev, -(PCGDP:ODA)))
#>       country iso3c       date year decade     region     income  OECD     POP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE 8996973
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE 9169410
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE 9351441
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE 9543205
#> 5 Afghanistan   AFG 1965-01-01 1964   1960 South Asia Low income FALSE 9744781
#> 6 Afghanistan   AFG 1966-01-01 1965   1960 South Asia Low income FALSE 9956320
fselect(wlddev, country, year, PCGDP:ODA) <- NULL          # Efficient deleting
head(wlddev)
#>   iso3c       date decade     region     income  OECD     POP
#> 1   AFG 1961-01-01   1960 South Asia Low income FALSE 8996973
#> 2   AFG 1962-01-01   1960 South Asia Low income FALSE 9169410
#> 3   AFG 1963-01-01   1960 South Asia Low income FALSE 9351441
#> 4   AFG 1964-01-01   1960 South Asia Low income FALSE 9543205
#> 5   AFG 1965-01-01   1960 South Asia Low income FALSE 9744781
#> 6   AFG 1966-01-01   1960 South Asia Low income FALSE 9956320
rm(wlddev)

head(num_vars(wlddev))                                     # Select numeric variables
#>   year decade PCGDP LIFEEX GINI       ODA     POP
#> 1 1960   1960    NA 32.446   NA 116769997 8996973
#> 2 1961   1960    NA 32.962   NA 232080002 9169410
#> 3 1962   1960    NA 33.471   NA 112839996 9351441
#> 4 1963   1960    NA 33.971   NA 237720001 9543205
#> 5 1964   1960    NA 34.463   NA 295920013 9744781
#> 6 1965   1960    NA 34.948   NA 341839996 9956320
head(cat_vars(wlddev))                                     # Select categorical (non-numeric) vars
#>       country iso3c       date     region     income  OECD
#> 1 Afghanistan   AFG 1961-01-01 South Asia Low income FALSE
#> 2 Afghanistan   AFG 1962-01-01 South Asia Low income FALSE
#> 3 Afghanistan   AFG 1963-01-01 South Asia Low income FALSE
#> 4 Afghanistan   AFG 1964-01-01 South Asia Low income FALSE
#> 5 Afghanistan   AFG 1965-01-01 South Asia Low income FALSE
#> 6 Afghanistan   AFG 1966-01-01 South Asia Low income FALSE
head(get_vars(wlddev, is_categorical))                     # Same thing
#>       country iso3c       date     region     income  OECD
#> 1 Afghanistan   AFG 1961-01-01 South Asia Low income FALSE
#> 2 Afghanistan   AFG 1962-01-01 South Asia Low income FALSE
#> 3 Afghanistan   AFG 1963-01-01 South Asia Low income FALSE
#> 4 Afghanistan   AFG 1964-01-01 South Asia Low income FALSE
#> 5 Afghanistan   AFG 1965-01-01 South Asia Low income FALSE
#> 6 Afghanistan   AFG 1966-01-01 South Asia Low income FALSE

num_vars(wlddev) <- num_vars(wlddev)                       # Replace Numeric Variables by themselves
get_vars(wlddev,is.numeric) <- get_vars(wlddev,is.numeric) # Same thing

head(get_vars(wlddev, 9:12))                               # Select columns 9 through 12, 2x faster
#>   PCGDP LIFEEX GINI       ODA
#> 1    NA 32.446   NA 116769997
#> 2    NA 32.962   NA 232080002
#> 3    NA 33.471   NA 112839996
#> 4    NA 33.971   NA 237720001
#> 5    NA 34.463   NA 295920013
#> 6    NA 34.948   NA 341839996
head(get_vars(wlddev, -(9:12)))                            # All except columns 9 through 12
#>       country iso3c       date year decade     region     income  OECD     POP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE 8996973
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE 9169410
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE 9351441
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE 9543205
#> 5 Afghanistan   AFG 1965-01-01 1964   1960 South Asia Low income FALSE 9744781
#> 6 Afghanistan   AFG 1966-01-01 1965   1960 South Asia Low income FALSE 9956320
head(get_vars(wlddev, c("PCGDP","LIFEEX","GINI","ODA")))   # Select using column names
#>   PCGDP LIFEEX GINI       ODA
#> 1    NA 32.446   NA 116769997
#> 2    NA 32.962   NA 232080002
#> 3    NA 33.471   NA 112839996
#> 4    NA 33.971   NA 237720001
#> 5    NA 34.463   NA 295920013
#> 6    NA 34.948   NA 341839996
head(get_vars(wlddev, "[[:upper:]]", regex = TRUE))        # Same thing: match upper-case var. names
#>    OECD PCGDP LIFEEX GINI       ODA     POP
#> 1 FALSE    NA 32.446   NA 116769997 8996973
#> 2 FALSE    NA 32.962   NA 232080002 9169410
#> 3 FALSE    NA 33.471   NA 112839996 9351441
#> 4 FALSE    NA 33.971   NA 237720001 9543205
#> 5 FALSE    NA 34.463   NA 295920013 9744781
#> 6 FALSE    NA 34.948   NA 341839996 9956320
head(gvr(wlddev, "[[:upper:]]"))                           # Same thing
#>    OECD PCGDP LIFEEX GINI       ODA     POP
#> 1 FALSE    NA 32.446   NA 116769997 8996973
#> 2 FALSE    NA 32.962   NA 232080002 9169410
#> 3 FALSE    NA 33.471   NA 112839996 9351441
#> 4 FALSE    NA 33.971   NA 237720001 9543205
#> 5 FALSE    NA 34.463   NA 295920013 9744781
#> 6 FALSE    NA 34.948   NA 341839996 9956320

get_vars(wlddev, 9:12) <- get_vars(wlddev, 9:12)           # 9x faster wlddev[9:12] <- wlddev[9:12]
add_vars(wlddev) <- STD(gv(wlddev,9:12), wlddev$iso3c)     # Add Standardized columns 9 through 12
head(wlddev)                                               # gv and av are shortcuts
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE    NA
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE    NA
#>   LIFEEX GINI       ODA     POP STD.PCGDP STD.LIFEEX STD.GINI    STD.ODA
#> 1 32.446   NA 116769997 8996973        NA  -1.653181       NA -0.6498451
#> 2 32.962   NA 232080002 9169410        NA  -1.602256       NA -0.5951801
#> 3 33.471   NA 112839996 9351441        NA  -1.552023       NA -0.6517082
#> 4 33.971   NA 237720001 9543205        NA  -1.502678       NA -0.5925063
#>  [ reached 'max' / getOption("max.print") -- omitted 2 rows ]

get_vars(wlddev, 14:17) <- NULL                            # Efficient Deleting added columns again
av(wlddev, "front") <- STD(gv(wlddev,9:12), wlddev$iso3c)  # Again adding in Front
head(wlddev)
#>   STD.PCGDP STD.LIFEEX STD.GINI    STD.ODA     country iso3c       date year
#> 1        NA  -1.653181       NA -0.6498451 Afghanistan   AFG 1961-01-01 1960
#> 2        NA  -1.602256       NA -0.5951801 Afghanistan   AFG 1962-01-01 1961
#> 3        NA  -1.552023       NA -0.6517082 Afghanistan   AFG 1963-01-01 1962
#> 4        NA  -1.502678       NA -0.5925063 Afghanistan   AFG 1964-01-01 1963
#>   decade     region     income  OECD PCGDP LIFEEX GINI       ODA     POP
#> 1   1960 South Asia Low income FALSE    NA 32.446   NA 116769997 8996973
#> 2   1960 South Asia Low income FALSE    NA 32.962   NA 232080002 9169410
#> 3   1960 South Asia Low income FALSE    NA 33.471   NA 112839996 9351441
#> 4   1960 South Asia Low income FALSE    NA 33.971   NA 237720001 9543205
#>  [ reached 'max' / getOption("max.print") -- omitted 2 rows ]
get_vars(wlddev, 1:4) <- NULL                              # Deleting
av(wlddev,c(10,12,14,16)) <- W(wlddev,~iso3c, cols = 9:12, # Adding next to original variables
                               keep.by = FALSE)
head(wlddev)
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE    NA
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE    NA
#>   W.PCGDP LIFEEX  W.LIFEEX GINI W.GINI       ODA       W.ODA     POP
#> 1      NA 32.446 -16.75117   NA     NA 116769997 -1370778502 8996973
#> 2      NA 32.962 -16.23517   NA     NA 232080002 -1255468497 9169410
#> 3      NA 33.471 -15.72617   NA     NA 112839996 -1374708502 9351441
#> 4      NA 33.971 -15.22617   NA     NA 237720001 -1249828497 9543205
#>  [ reached 'max' / getOption("max.print") -- omitted 2 rows ]
get_vars(wlddev, c(10,12,14,16)) <- NULL                   # Deleting

head(add_vars(wlddev, new = STD(wlddev$PCGDP)))                  # Can also add columns like this
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE    NA
#> 4 Afghanistan   AFG 1964-01-01 1963   1960 South Asia Low income FALSE    NA
#> 5 Afghanistan   AFG 1965-01-01 1964   1960 South Asia Low income FALSE    NA
#>   LIFEEX GINI       ODA     POP new
#> 1 32.446   NA 116769997 8996973  NA
#> 2 32.962   NA 232080002 9169410  NA
#> 3 33.471   NA 112839996 9351441  NA
#> 4 33.971   NA 237720001 9543205  NA
#> 5 34.463   NA 295920013 9744781  NA
#>  [ reached 'max' / getOption("max.print") -- omitted 1 rows ]
head(add_vars(wlddev, STD(nv(wlddev)), new = W(wlddev$PCGDP)))   # etc...
#>       country iso3c       date year decade     region     income  OECD PCGDP
#> 1 Afghanistan   AFG 1961-01-01 1960   1960 South Asia Low income FALSE    NA
#> 2 Afghanistan   AFG 1962-01-01 1961   1960 South Asia Low income FALSE    NA
#> 3 Afghanistan   AFG 1963-01-01 1962   1960 South Asia Low income FALSE    NA
#>   LIFEEX GINI       ODA     POP  STD.year STD.decade STD.PCGDP STD.LIFEEX
#> 1 32.446   NA 116769997 8996973 -1.703821  -1.460378        NA  -2.775283
#> 2 32.962   NA 232080002 9169410 -1.647027  -1.460378        NA  -2.730321
#> 3 33.471   NA 112839996 9351441 -1.590233  -1.460378        NA  -2.685969
#>   STD.GINI    STD.ODA    STD.POP new
#> 1       NA -0.3890241 -0.1493233  NA
#> 2       NA -0.2562874 -0.1476348  NA
#> 3       NA -0.3935480 -0.1458523  NA
#>  [ reached 'max' / getOption("max.print") -- omitted 3 rows ]

head(add_vars(mtcars, mtcars, mpg = mtcars$mpg, mtcars), 2)      # add_vars does not check names!
#>               mpg cyl disp  hp drat    wt  qsec vs am gear carb mpg cyl disp
#> Mazda RX4      21   6  160 110  3.9 2.620 16.46  0  1    4    4  21   6  160
#> Mazda RX4 Wag  21   6  160 110  3.9 2.875 17.02  0  1    4    4  21   6  160
#>                hp drat    wt  qsec vs am gear carb mpg mpg cyl disp  hp drat
#> Mazda RX4     110  3.9 2.620 16.46  0  1    4    4  21  21   6  160 110  3.9
#> Mazda RX4 Wag 110  3.9 2.875 17.02  0  1    4    4  21  21   6  160 110  3.9
#>                  wt  qsec vs am gear carb
#> Mazda RX4     2.620 16.46  0  1    4    4
#> Mazda RX4 Wag 2.875 17.02  0  1    4    4
```
