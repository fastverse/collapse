# Find and Extract / Subset List Elements

A suite of functions to subset or extract from (potentially complex)
lists and list-like structures. Subsetting may occur according to
certain data types, using identifier functions, element names or regular
expressions to search the list for certain objects.

- `atomic_elem` and `list_elem` are non-recursive functions to extract
  and replace the atomic and sub-list elements at the top-level of the
  list tree.

- `reg_elem` is the recursive equivalent of `atomic_elem` and returns
  the 'regular' part of the list - with atomic elements in the final
  nodes. `irreg_elem` returns all the non-regular elements (i.e. call
  and terms objects, formulas, etc...). See Examples.

- `get_elem` returns the part of the list responding to either an
  identifier function, regular expression, exact element names or
  indices applied to all final objects. `has_elem` checks for the
  existence of an element and returns `TRUE` if a match is found. See
  Examples.

## Usage

``` r
## Non-recursive (top-level) subsetting and replacing
atomic_elem(l, return = "sublist", keep.class = FALSE)
atomic_elem(l) <- value
list_elem(l, return = "sublist", keep.class = FALSE)
list_elem(l) <- value

## Recursive separation of regular (atomic) and irregular (non-atomic) parts
reg_elem(l, recursive = TRUE, keep.tree = FALSE, keep.class = FALSE)
irreg_elem(l, recursive = TRUE, keep.tree = FALSE, keep.class = FALSE)

## Extract elements / subset list tree
get_elem(l, elem, recursive = TRUE, DF.as.list = FALSE, keep.tree = FALSE,
         keep.class = FALSE, regex = FALSE, invert = FALSE, ...)

## Check for the existence of elements
has_elem(l, elem, recursive = TRUE, DF.as.list = FALSE, regex = FALSE, ...)
```

## Arguments

- l:

  a list.

- value:

  a list of the same length as the extracted subset of `l`.

- elem:

  a function returning `TRUE` or `FALSE` when applied to elements of
  `l`, or a character vector of element names or regular expressions (if
  `regex = TRUE`). `get_elem` also supports a vector or indices which
  will be used to subset all final objects.

- return:

  an integer or string specifying what the selector function should
  return. The options are:

  |        |     |                 |     |                          |
  |--------|-----|-----------------|-----|--------------------------|
  | *Int.* |     | *String*        |     | *Description*            |
  | 1      |     | "sublist"       |     | subset of list (default) |
  | 2      |     | "names"         |     | column names             |
  | 3      |     | "indices"       |     | column indices           |
  | 4      |     | "named_indices" |     | named column indices     |
  | 5      |     | "logical"       |     | logical selection vector |
  | 6      |     | "named_logical" |     | named logical vector     |

  *Note*: replacement functions only replace data, names are replaced
  together with the data.

- recursive:

  logical. Should the list search be recursive (i.e. go though all the
  elements), or just at the top-level?

- DF.as.list:

  logical. `TRUE` treats data frames like (sub-)lists; `FALSE` like
  atomic elements.

- keep.tree:

  logical. `TRUE` always returns the entire list tree leading up to all
  matched results, while `FALSE` drops the top-level part of the tree if
  possible.

- keep.class:

  logical. For list-based objects: should the class be retained? This
  only works if these objects have a `[` method that retains the class.

- regex:

  logical. Should regular expression search be used on the list names,
  or only exact matches?

- invert:

  logical. Invert search i.e. exclude matched elements from the list?

- ...:

  further arguments to `grep` (if `regex = TRUE`).

## Details

For a lack of better terminology, *collapse* defines 'regular' R objects
as objects that are either atomic or a list. `reg_elem` with
`recursive = TRUE` extracts the subset of the list tree leading up to
atomic elements in the final nodes. This part of the list tree is
unlistable - calling `is_unlistable(reg_elem(l))` will be `TRUE` for all
lists `l`. Conversely, all elements left behind by `reg_elem` will be
picked up be `irreg_elem`. Thus `is_unlistable(irreg_elem(l))` is always
`FALSE` for lists with irregular elements (otherwise `irreg_elem`
returns an empty list).  

If `keep.tree = TRUE`, `reg_elem`, `irreg_elem` and `get_elem` always
return the entire list tree, but cut off all of the branches not leading
to the desired result. If `keep.tree = FALSE`, top-level parts of the
tree are omitted as far as possible. For example in a nested list with
three levels and one data-matrix in one of the final branches,
`get_elem(l, is.matrix, keep.tree = TRUE)` will return a list (`lres`)
of depth 3, from which the matrix can be accessed as
`lres[[1]][[1]][[1]]`. This however does not make much sense.
`get_elem(l, is.matrix, keep.tree = FALSE)` will therefore figgure out
that it can drop the entire tree and return just the matrix.
`keep.tree = FALSE` makes additional optimizations if matching elements
are at far-apart corners in a nested structure, by only preserving the
hierarchy if elements are above each other on the same branch. Thus for
a list `l <- list(list(2,list("a",1)),list(1,list("b",2)))` calling
`get_elem(l, is.character)` will just return `list("a","b")`.

## See also

[List
Processing](https://fastverse.org/collapse/reference/list-processing.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
m <- qM(mtcars)
get_elem(list(list(list(m))), is.matrix)
#>                      mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4           21.0   6 160.0 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag       21.0   6 160.0 110 3.90 2.875 17.02  0  1    4    4
#> Datsun 710          22.8   4 108.0  93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive      21.4   6 258.0 110 3.08 3.215 19.44  1  0    3    1
#> Hornet Sportabout   18.7   8 360.0 175 3.15 3.440 17.02  0  0    3    2
#> Valiant             18.1   6 225.0 105 2.76 3.460 20.22  1  0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
get_elem(list(list(list(m))), is.matrix, keep.tree = TRUE)
#> [[1]]
#> [[1]][[1]]
#> [[1]][[1]][[1]]
#>                      mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4           21.0   6 160.0 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag       21.0   6 160.0 110 3.90 2.875 17.02  0  1    4    4
#> Datsun 710          22.8   4 108.0  93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive      21.4   6 258.0 110 3.08 3.215 19.44  1  0    3    1
#> Hornet Sportabout   18.7   8 360.0 175 3.15 3.440 17.02  0  0    3    2
#> Valiant             18.1   6 225.0 105 2.76 3.460 20.22  1  0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
#> 
#> 
#> 

l <- list(list(2,list("a",1)),list(1,list("b",2)))
has_elem(l, is.logical)
#> [1] FALSE
has_elem(l, is.numeric)
#> [1] TRUE
get_elem(l, is.character)
#> [[1]]
#> [1] "a"
#> 
#> [[2]]
#> [1] "b"
#> 
get_elem(l, is.character, keep.tree = TRUE)
#> [[1]]
#> [[1]][[1]]
#> [[1]][[1]][[1]]
#> [1] "a"
#> 
#> 
#> 
#> [[2]]
#> [[2]][[1]]
#> [[2]][[1]][[1]]
#> [1] "b"
#> 
#> 
#> 

l <- lm(mpg ~ cyl + vs, data = mtcars)
str(reg_elem(l))
#> List of 9
#>  $ coefficients : Named num [1:3] 39.625 -3.091 -0.939
#>   ..- attr(*, "names")= chr [1:3] "(Intercept)" "cyl" "vs"
#>  $ residuals    : Named num [1:32] -0.081 -0.081 -3.523 1.258 3.8 ...
#>   ..- attr(*, "names")= chr [1:32] "Mazda RX4" "Mazda RX4 Wag" "Datsun 710" "Hornet 4 Drive" ...
#>  $ effects      : Named num [1:32] -113.65 -28.6 1.54 2.39 3.75 ...
#>   ..- attr(*, "names")= chr [1:32] "(Intercept)" "cyl" "vs" "" ...
#>  $ rank         : int 3
#>  $ fitted.values: Named num [1:32] 21.1 21.1 26.3 20.1 14.9 ...
#>   ..- attr(*, "names")= chr [1:32] "Mazda RX4" "Mazda RX4 Wag" "Datsun 710" "Hornet 4 Drive" ...
#>  $ assign       : int [1:3] 0 1 2
#>  $ qr           :List of 5
#>   ..$ qr   : num [1:32, 1:3] -5.657 0.177 0.177 0.177 0.177 ...
#>   .. ..- attr(*, "dimnames")=List of 2
#>   .. .. ..$ : chr [1:32] "Mazda RX4" "Mazda RX4 Wag" "Datsun 710" "Hornet 4 Drive" ...
#>   .. .. ..$ : chr [1:3] "(Intercept)" "cyl" "vs"
#>   .. ..- attr(*, "assign")= int [1:3] 0 1 2
#>   ..$ qraux: num [1:3] 1.18 1.02 1.13
#>   ..$ pivot: int [1:3] 1 2 3
#>   ..$ tol  : num 1e-07
#>   ..$ rank : int 3
#>  $ df.residual  : int 29
#>  $ model        :'data.frame':   32 obs. of  3 variables:
#>   ..$ mpg: num [1:32] 21 21 22.8 21.4 18.7 18.1 14.3 24.4 22.8 19.2 ...
#>   ..$ cyl: num [1:32] 6 6 4 6 8 6 8 4 4 6 ...
#>   ..$ vs : num [1:32] 0 0 1 1 0 1 0 1 1 1 ...
#>   ..- attr(*, "terms")=Classes 'terms', 'formula'  language mpg ~ cyl + vs
#>   .. .. ..- attr(*, "variables")= language list(mpg, cyl, vs)
#>   .. .. ..- attr(*, "factors")= int [1:3, 1:2] 0 1 0 0 0 1
#>   .. .. .. ..- attr(*, "dimnames")=List of 2
#>   .. .. .. .. ..$ : chr [1:3] "mpg" "cyl" "vs"
#>   .. .. .. .. ..$ : chr [1:2] "cyl" "vs"
#>   .. .. ..- attr(*, "term.labels")= chr [1:2] "cyl" "vs"
#>   .. .. ..- attr(*, "order")= int [1:2] 1 1
#>   .. .. ..- attr(*, "intercept")= int 1
#>   .. .. ..- attr(*, "response")= int 1
#>   .. .. ..- attr(*, ".Environment")=<environment: 0x1618570b0> 
#>   .. .. ..- attr(*, "predvars")= language list(mpg, cyl, vs)
#>   .. .. ..- attr(*, "dataClasses")= Named chr [1:3] "numeric" "numeric" "numeric"
#>   .. .. .. ..- attr(*, "names")= chr [1:3] "mpg" "cyl" "vs"
str(irreg_elem(l))
#> List of 2
#>  $ call : language lm(formula = mpg ~ cyl + vs, data = mtcars)
#>  $ terms:Classes 'terms', 'formula'  language mpg ~ cyl + vs
#>   .. ..- attr(*, "variables")= language list(mpg, cyl, vs)
#>   .. ..- attr(*, "factors")= int [1:3, 1:2] 0 1 0 0 0 1
#>   .. .. ..- attr(*, "dimnames")=List of 2
#>   .. .. .. ..$ : chr [1:3] "mpg" "cyl" "vs"
#>   .. .. .. ..$ : chr [1:2] "cyl" "vs"
#>   .. ..- attr(*, "term.labels")= chr [1:2] "cyl" "vs"
#>   .. ..- attr(*, "order")= int [1:2] 1 1
#>   .. ..- attr(*, "intercept")= int 1
#>   .. ..- attr(*, "response")= int 1
#>   .. ..- attr(*, ".Environment")=<environment: 0x1618570b0> 
#>   .. ..- attr(*, "predvars")= language list(mpg, cyl, vs)
#>   .. ..- attr(*, "dataClasses")= Named chr [1:3] "numeric" "numeric" "numeric"
#>   .. .. ..- attr(*, "names")= chr [1:3] "mpg" "cyl" "vs"
get_elem(l, is.matrix)
#>                     (Intercept)          cyl          vs
#> Mazda RX4            -5.6568542 -35.00178567 -2.47487373
#> Mazda RX4 Wag         0.1767767   9.94359090 -2.27533496
#> Datsun 710            0.1767767   0.21715832 -1.64251357
#> Hornet 4 Drive        0.1767767   0.01602374  0.36419832
#> Hornet Sportabout     0.1767767  -0.18511084 -0.01520019
#> Valiant               0.1767767   0.01602374  0.36419832
#> Duster 360            0.1767767  -0.18511084 -0.01520019
#> Merc 240D             0.1767767   0.21715832  0.13477385
#> Merc 230              0.1767767   0.21715832  0.13477385
#> Merc 280              0.1767767   0.01602374  0.36419832
#> Merc 280C             0.1767767   0.01602374  0.36419832
#> Merc 450SE            0.1767767  -0.18511084 -0.01520019
#> Merc 450SL            0.1767767  -0.18511084 -0.01520019
#> Merc 450SLC           0.1767767  -0.18511084 -0.01520019
#> Cadillac Fleetwood    0.1767767  -0.18511084 -0.01520019
#> Lincoln Continental   0.1767767  -0.18511084 -0.01520019
#> Chrysler Imperial     0.1767767  -0.18511084 -0.01520019
#> Fiat 128              0.1767767   0.21715832  0.13477385
#> Honda Civic           0.1767767   0.21715832  0.13477385
#> Toyota Corolla        0.1767767   0.21715832  0.13477385
#> Toyota Corona         0.1767767   0.21715832  0.13477385
#> Dodge Challenger      0.1767767  -0.18511084 -0.01520019
#> AMC Javelin           0.1767767  -0.18511084 -0.01520019
#>  [ reached 'max' / getOption("max.print") -- omitted 9 rows ]
#> attr(,"assign")
#> [1] 0 1 2
get_elem(l, "residuals")
#>           Mazda RX4       Mazda RX4 Wag          Datsun 710      Hornet 4 Drive 
#>          -0.0809747          -0.0809747          -3.5232427           1.2581068 
#>   Hornet Sportabout             Valiant          Duster 360           Merc 240D 
#>           3.8003749          -2.0418932          -0.5996251          -1.9232427 
#>            Merc 230            Merc 280           Merc 280C          Merc 450SE 
#>          -3.5232427          -0.9418932          -2.3418932           1.5003749 
#>          Merc 450SL         Merc 450SLC  Cadillac Fleetwood Lincoln Continental 
#>           2.4003749           0.3003749          -4.4996251          -4.4996251 
#>   Chrysler Imperial            Fiat 128         Honda Civic      Toyota Corolla 
#>          -0.1996251           6.0767573           4.0767573           7.5767573 
#>       Toyota Corona    Dodge Challenger         AMC Javelin          Camaro Z28 
#>          -4.8232427           0.6003749           0.3003749          -1.5996251 
#>    Pontiac Firebird           Fiat X1-9       Porsche 914-2        Lotus Europa 
#>           4.3003749           0.9767573          -1.2623243           4.0767573 
#>      Ford Pantera L        Ferrari Dino       Maserati Bora          Volvo 142E 
#>           0.9003749          -1.3809747           0.1003749          -4.9232427 
get_elem(l, "fit", regex = TRUE)
#>           Mazda RX4       Mazda RX4 Wag          Datsun 710      Hornet 4 Drive 
#>            21.08097            21.08097            26.32324            20.14189 
#>   Hornet Sportabout             Valiant          Duster 360           Merc 240D 
#>            14.89963            20.14189            14.89963            26.32324 
#>            Merc 230            Merc 280           Merc 280C          Merc 450SE 
#>            26.32324            20.14189            20.14189            14.89963 
#>          Merc 450SL         Merc 450SLC  Cadillac Fleetwood Lincoln Continental 
#>            14.89963            14.89963            14.89963            14.89963 
#>   Chrysler Imperial            Fiat 128         Honda Civic      Toyota Corolla 
#>            14.89963            26.32324            26.32324            26.32324 
#>       Toyota Corona    Dodge Challenger         AMC Javelin          Camaro Z28 
#>            26.32324            14.89963            14.89963            14.89963 
#>    Pontiac Firebird           Fiat X1-9       Porsche 914-2        Lotus Europa 
#>            14.89963            26.32324            27.26232            26.32324 
#>      Ford Pantera L        Ferrari Dino       Maserati Bora          Volvo 142E 
#>            14.89963            21.08097            14.89963            26.32324 
has_elem(l, "tol")
#> [1] TRUE
get_elem(l, "tol")
#> [1] 1e-07
```
