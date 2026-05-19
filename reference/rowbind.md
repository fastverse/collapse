# Row-Bind Lists / Data Frame-Like Objects

*collapse*'s version of
[`data.table::rbindlist`](https://rdrr.io/pkg/data.table/man/rbindlist.html)
and `rbind.data.frame`. The core code is copied from *data.table*, which
deserves all credit for the implementation. `rowbind` only binds
lists/data.frame's. For a more flexible recursive version see
[`unlist2d`](https://fastverse.org/collapse/reference/unlist2d.md). To
combine lists column-wise see
[`add_vars`](https://fastverse.org/collapse/reference/select_replace_vars.md)
or
[`ftransform`](https://fastverse.org/collapse/reference/ftransform.md)
(with replacement).

## Usage

``` r
rowbind(..., idcol = NULL, row.names = FALSE,
        use.names = TRUE, fill = FALSE, id.factor = "auto",
        return = c("as.first", "data.frame", "data.table", "tibble", "list"))
```

## Arguments

- ...:

  a single list of list-like objects (data.frames) or comma separated
  objects (internally assembled using `list(...)`). Names can be
  supplied if `!is.null(idcol)`.

- idcol:

  character. The name of an id-column to be generated identifying the
  source of rows in the final object. Using `idcol = TRUE` will set the
  name to `".id"`. If the input list has names, these will form the
  content of the id column, otherwise integers are used. To save memory,
  it is advised to keep `id.factor = TRUE`.

- row.names:

  `TRUE` extracts row names from all the objects in `l` and adds them to
  the output in a column named `"row.names"`. Alternatively, a column
  name i.e. `row.names = "variable"` can be supplied.

- use.names:

  logical. `TRUE` binds by matching column name, `FALSE` by position.

- fill:

  logical. `TRUE` fills missing columns with NAs. When `TRUE`,
  `use.names` is set to `TRUE`.

- id.factor:

  if `TRUE` and `!isFALSE(idcols)`, create id column as factor instead
  of character or integer vector. It is also possible to specify
  `"ordered"` to generate an ordered factor id. `"auto"` uses `TRUE` if
  `!is.null(names(l))` where `l` is the input list (because factors are
  much more memory efficient than character vectors).

- return:

  an integer or string specifying what to return. `1 - "as.first"`
  preserves the attributes of the first element of the list,
  `2/3/4 - "data.frame"/"data.table"/"tibble"` coerces to specific
  objects, and `5 - "list"` returns a (named) list.

## Value

a long list or data frame-like object formed by combining the rows /
elements of the input objects. The `return` argument controls the exact
format of the output.

## See also

[`unlist2d`](https://fastverse.org/collapse/reference/unlist2d.md),
[`add_vars`](https://fastverse.org/collapse/reference/select_replace_vars.md),
[`ftransform`](https://fastverse.org/collapse/reference/ftransform.md),
[Data Frame
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
# These are the same
rowbind(mtcars, mtcars)
#>    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> 1 21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> 2 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> 3 22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> 4 21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> 5 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#> 6 18.1   6  225 105 2.76 3.460 20.22  1  0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 58 rows ]
rowbind(list(mtcars, mtcars))
#>    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> 1 21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> 2 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> 3 22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> 4 21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> 5 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#> 6 18.1   6  225 105 2.76 3.460 20.22  1  0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 58 rows ]

# With id column
rowbind(mtcars, mtcars, idcol = "id")
#>   id  mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> 1  1 21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> 2  1 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> 3  1 22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> 4  1 21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> 5  1 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#>  [ reached 'max' / getOption("max.print") -- omitted 59 rows ]
rowbind(a = mtcars, b = mtcars, idcol = "id")
#>   id  mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> 1  a 21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> 2  a 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> 3  a 22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> 4  a 21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> 5  a 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#>  [ reached 'max' / getOption("max.print") -- omitted 59 rows ]

# With saving row-names
rowbind(mtcars, mtcars, row.names = "cars")
#>                cars  mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> 1         Mazda RX4 21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> 2     Mazda RX4 Wag 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> 3        Datsun 710 22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> 4    Hornet 4 Drive 21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> 5 Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#>  [ reached 'max' / getOption("max.print") -- omitted 59 rows ]
rowbind(a = mtcars, b = mtcars, idcol = "id", row.names = "cars")
#>   id              cars  mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> 1  a         Mazda RX4 21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> 2  a     Mazda RX4 Wag 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> 3  a        Datsun 710 22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> 4  a    Hornet 4 Drive 21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> 5  a Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#>  [ reached 'max' / getOption("max.print") -- omitted 59 rows ]

# Filling up columns
rowbind(mtcars, mtcars[2:8], fill = TRUE)
#>    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> 1 21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> 2 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> 3 22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> 4 21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> 5 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#> 6 18.1   6  225 105 2.76 3.460 20.22  1  0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 58 rows ]
```
