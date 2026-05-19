# Fast Renaming and Relabelling Objects

`frename` returns a renamed shallow-copy, `setrename` renames objects by
reference. These functions also work with objects other than data frames
that have a 'names' attribute. `relabel` and `setrelabel` do that same
for labels attached to data frame columns.

## Usage

``` r
frename(.x, ..., cols = NULL, .nse = TRUE)
rnm(.x, ..., cols = NULL, .nse = TRUE)     # Shorthand for frename()

setrename(.x, ..., cols = NULL, .nse = TRUE)

relabel(.x, ..., cols = NULL, attrn = "label")

setrelabel(.x, ..., cols = NULL, attrn = "label")
```

## Arguments

- .x:

  for `(f/set)rename`: an R object with a `"names"` attribute. For
  `(set)relabel`: a named list.

- ...:

  either tagged vector expressions of the form `name = newname` /
  `name = newlabel` (`frename` also supports `newname = name`), a
  (named) vector of names/labels, or a single function (+ optional
  arguments to the function) applied to all names/labels (of
  columns/elements selected in `cols`).

- cols:

  If `...` is a function, select a subset of columns/elements to
  rename/relabel using names, indices, a logical vector or a function
  applied to the columns if `.x` is a list (e.g. `is.numeric`).

- .nse:

  logical. `TRUE` allows non-standard evaluation of tagged vector
  expressions, allowing you to supply new names without quotes. Set to
  `FALSE` for programming or passing vectors of names.

- attrn:

  character. Name of attribute to store labels or retrieve labels from.

## Value

`.x` renamed / relabelled. `setrename` and `setrelabel` return `.x`
invisibly.

## Note

Note that both `relabel` and `setrelabel` modify `.x` by reference. This
is because labels are attached to columns themselves, making it
impossible to avoid permanent modification by taking a shallow copy of
the encompassing list / data.frame. On the other hand `frename` makes a
shallow copy whereas `setrename` also modifies by reference.

## See also

[Data Frame
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Using tagged expressions
head(frename(iris, Sepal.Length = SL, Sepal.Width = SW,
                   Petal.Length = PL, Petal.Width = PW))
#>    SL  SW  PL  PW Species
#> 1 5.1 3.5 1.4 0.2  setosa
#> 2 4.9 3.0 1.4 0.2  setosa
#> 3 4.7 3.2 1.3 0.2  setosa
#> 4 4.6 3.1 1.5 0.2  setosa
#> 5 5.0 3.6 1.4 0.2  setosa
#> 6 5.4 3.9 1.7 0.4  setosa
head(frename(iris, Sepal.Length = "S L", Sepal.Width = "S W",
                   Petal.Length = "P L", Petal.Width = "P W"))
#>   S L S W P L P W Species
#> 1 5.1 3.5 1.4 0.2  setosa
#> 2 4.9 3.0 1.4 0.2  setosa
#> 3 4.7 3.2 1.3 0.2  setosa
#> 4 4.6 3.1 1.5 0.2  setosa
#> 5 5.0 3.6 1.4 0.2  setosa
#> 6 5.4 3.9 1.7 0.4  setosa

## Since v2.0.0 this is also supported
head(frename(iris, SL = Sepal.Length, SW = Sepal.Width,
                   PL = Petal.Length, PW = Petal.Width))
#>    SL  SW  PL  PW Species
#> 1 5.1 3.5 1.4 0.2  setosa
#> 2 4.9 3.0 1.4 0.2  setosa
#> 3 4.7 3.2 1.3 0.2  setosa
#> 4 4.6 3.1 1.5 0.2  setosa
#> 5 5.0 3.6 1.4 0.2  setosa
#> 6 5.4 3.9 1.7 0.4  setosa

## Using a function
head(frename(iris, tolower))
#>   sepal.length sepal.width petal.length petal.width species
#> 1          5.1         3.5          1.4         0.2  setosa
#> 2          4.9         3.0          1.4         0.2  setosa
#> 3          4.7         3.2          1.3         0.2  setosa
#> 4          4.6         3.1          1.5         0.2  setosa
#> 5          5.0         3.6          1.4         0.2  setosa
#> 6          5.4         3.9          1.7         0.4  setosa
head(frename(iris, tolower, cols = 1:2))
#>   sepal.length sepal.width Petal.Length Petal.Width Species
#> 1          5.1         3.5          1.4         0.2  setosa
#> 2          4.9         3.0          1.4         0.2  setosa
#> 3          4.7         3.2          1.3         0.2  setosa
#> 4          4.6         3.1          1.5         0.2  setosa
#> 5          5.0         3.6          1.4         0.2  setosa
#> 6          5.4         3.9          1.7         0.4  setosa
head(frename(iris, tolower, cols = is.numeric))
#>   sepal.length sepal.width petal.length petal.width Species
#> 1          5.1         3.5          1.4         0.2  setosa
#> 2          4.9         3.0          1.4         0.2  setosa
#> 3          4.7         3.2          1.3         0.2  setosa
#> 4          4.6         3.1          1.5         0.2  setosa
#> 5          5.0         3.6          1.4         0.2  setosa
#> 6          5.4         3.9          1.7         0.4  setosa
head(frename(iris, paste, "new", sep = "_", cols = 1:2))
#>   Sepal.Length_new Sepal.Width_new Petal.Length Petal.Width Species
#> 1              5.1             3.5          1.4         0.2  setosa
#> 2              4.9             3.0          1.4         0.2  setosa
#> 3              4.7             3.2          1.3         0.2  setosa
#> 4              4.6             3.1          1.5         0.2  setosa
#> 5              5.0             3.6          1.4         0.2  setosa
#> 6              5.4             3.9          1.7         0.4  setosa

## Using vectors of names and programming
newname = "sepal_length"
head(frename(iris, Sepal.Length = newname, .nse = FALSE))
#>   sepal_length Sepal.Width Petal.Length Petal.Width Species
#> 1          5.1         3.5          1.4         0.2  setosa
#> 2          4.9         3.0          1.4         0.2  setosa
#> 3          4.7         3.2          1.3         0.2  setosa
#> 4          4.6         3.1          1.5         0.2  setosa
#> 5          5.0         3.6          1.4         0.2  setosa
#> 6          5.4         3.9          1.7         0.4  setosa
newnames = c("sepal_length", "sepal_width")
head(frename(iris, newnames, cols = 1:2))
#>   sepal_length sepal_width Petal.Length Petal.Width Species
#> 1          5.1         3.5          1.4         0.2  setosa
#> 2          4.9         3.0          1.4         0.2  setosa
#> 3          4.7         3.2          1.3         0.2  setosa
#> 4          4.6         3.1          1.5         0.2  setosa
#> 5          5.0         3.6          1.4         0.2  setosa
#> 6          5.4         3.9          1.7         0.4  setosa
newnames = c(Sepal.Length = "sepal_length", Sepal.Width = "sepal_width")
head(frename(iris, newnames, .nse = FALSE))
#>   sepal_length sepal_width Petal.Length Petal.Width Species
#> 1          5.1         3.5          1.4         0.2  setosa
#> 2          4.9         3.0          1.4         0.2  setosa
#> 3          4.7         3.2          1.3         0.2  setosa
#> 4          4.6         3.1          1.5         0.2  setosa
#> 5          5.0         3.6          1.4         0.2  setosa
#> 6          5.4         3.9          1.7         0.4  setosa
# Since v2.0.0, this works as well
newnames = c(sepal_length = "Sepal.Length", sepal_width = "Sepal.Width")
head(frename(iris, newnames, .nse = FALSE))
#>   sepal_length sepal_width Petal.Length Petal.Width Species
#> 1          5.1         3.5          1.4         0.2  setosa
#> 2          4.9         3.0          1.4         0.2  setosa
#> 3          4.7         3.2          1.3         0.2  setosa
#> 4          4.6         3.1          1.5         0.2  setosa
#> 5          5.0         3.6          1.4         0.2  setosa
#> 6          5.4         3.9          1.7         0.4  setosa

## Renaming by reference
# setrename(iris, tolower)
# head(iris)
# rm(iris)
# etc...

## Relabelling (by reference)
# namlab(relabel(wlddev, PCGDP = "GDP per Capita", LIFEEX = "Life Expectancy"))
# namlab(relabel(wlddev, toupper))

```
