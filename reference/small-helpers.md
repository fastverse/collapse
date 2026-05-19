# Small (Helper) Functions

Convenience functions in the *collapse* package that help to deal with
object attributes such as variable names and labels, object checking,
metaprogramming, and that improve the workflow.

## Usage

``` r
.c(...)                       # Non-standard concatenation i.e. .c(a, b) == c("a", "b")
nam %=% values                # Multiple-assignment e.g. .c(x, y) %=% c(1, 2),
massign(nam, values,          # can also assign to different environment.
        envir = parent.frame())
vlabels(X, attrn = "label",   # Get labels of variables in X, in attr(X[[i]], attrn)
        use.names = TRUE)
vlabels(X, attrn = "label") <- value    # Set labels of variables in X (by reference)
setLabels(X, value = NULL,    # Set labels of variables in X (by reference) and return X
          attrn = "label", cols = NULL)
vclasses(X, use.names = TRUE) # Get classes of variables in X
namlab(X, class = FALSE,      # Return data frame of names and labels,
  attrn = "label", N = FALSE, # and (optionally) classes, number of observations
  Ndistinct = FALSE)          # and number of non-missing distinct values
add_stub(X, stub, pre = TRUE, # Add a stub (i.e. prefix or postfix) to column names
         cols = NULL)
rm_stub(X, stub, pre = TRUE,  # Remove stub from column names, also supports general
        regex = FALSE,        # regex matching and removing of characters
        cols = NULL, ...)
all_identical(...)            # Check exact equality of multiple objects or list-elements
all_obj_equal(...)            # Check near equality of multiple objects or list-elements
all_funs(expr)                # Find all functions called in an R language expression
setRownames(object,           # Set rownames of object and return object
    nm = if(is.atomic(object)) seq_row(object) else NULL)
setColnames(object, nm)       # Set colnames of object and return object
setDimnames(object, dn,       # Set dimension names of object and return object
            which = NULL)
unattrib(object)              # Remove all attributes from object
setAttrib(object, a)          # Replace all attributes with list of attributes 'a'
setattrib(object, a)          # Same thing by reference, returning object invisibly
copyAttrib(to, from)          # Copy all attributes from object 'from' to object 'to'
copyMostAttrib(to, from)      # Copy most attributes from object 'from' to object 'to'
is_categorical(x)             # The opposite of is.numeric
is_date(x)                    # Check if object is of class "Date", "POSIXlt" or "POSIXct"
```

## Arguments

- X:

  a matrix or data frame (some functions also support vectors and arrays
  although that is less common).

- x:

  a (atomic) vector.

- expr:

  an expression of type "language" e.g. `quote(x / sum(x))`.

- object, to, from:

  a suitable R object.

- a:

  a suitable list of attributes.

- attrn:

  character. Name of attribute to store labels or retrieve labels from.

- N, Ndistinct:

  logical. Options to display the number of observations or number of
  distinct non-missing values.

- value:

  for `whichv` and `alloc`: a single value of any vector type. For
  `vlabels<-` and `setLabels`: a matching character vector or list of
  variable labels.

- use.names:

  logical. Preserve names if `X` is a list.

- cols:

  integer. (optional) indices of columns to apply the operation to. Note
  that for these small functions this needs to be integer, whereas for
  other functions in the package this argument is more flexible.

- class:

  logical. Also show the classes of variables in X in a column?

- stub:

  a single character stub, i.e. "log.", which by default will be
  pre-applied to all variables or column names in X.

- pre:

  logical. `FALSE` will post-apply `stub`.

- regex:

  logical. Match pattern anywhere in names using a regular expression
  and remove it with [`gsub`](https://rdrr.io/r/base/grep.html).

- nm:

  a suitable vector of row- or column-names.

- dn:

  a suitable vector or list of names for dimension(s).

- which:

  integer. If `NULL`, `dn` has to be a list fully specifying the
  dimension names of the object. Alternatively, a vector or list of
  names for dimensions `which` can be supplied. See Examples.

- nam:

  character. A vector of object names.

- values:

  a matching atomic vector or list of objects.

- envir:

  the environment to assign into.

- ...:

  for `.c`: Comma-separated expressions. For
  `all_identical / all_obj_equal`: Either multiple comma-separated
  objects or a single list of objects in which all elements will be
  checked for exact / numeric equality. For `rm_stub`: further arguments
  passed to [`gsub`](https://rdrr.io/r/base/grep.html).

## Details

`all_funs` is the opposite of
[`all.vars`](https://rdrr.io/r/base/allnames.html), to return the
functions called rather than the variables in an expression. See
Examples.

`copyAttrib` and `copyMostAttrib` take a shallow copy of the attribute
list, i.e. they don't duplicate in memory the attributes themselves.
They also, along with `setAttrib`, take a shallow copy of lists passed
to the `to` argument, so that lists are not modified by reference.
Atomic `to` arguments are however modified by reference. The function
`setattrib`, added in v1.8.9, modifies the `object` by reference i.e. no
shallow copies are taken.

`copyMostAttrib` copies all attributes except for `"names"`, `"dim"` and
`"dimnames"` (like the corresponding C-API function), and further only
copies the `"row.names"` attribute of data frames if known to be valid.
Thus it is a suitable choice if objects should be of the same type but
are not of equal dimensions.

## See also

[Efficient
Programming](https://fastverse.org/collapse/reference/efficient-programming.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## Non-standard concatenation
.c(a, b, "c d", e == f)
#> [1] "a"      "b"      "c d"    "e == f"

## Multiple assignment
.c(a, b) %=% list(1, 2)
.c(T, N) %=% dim(EuStockMarkets)
names(iris) %=% iris
list2env(iris)          # Same thing
#> <environment: 0x138f132c8>
rm(list = c("a", "b", "T", "N", names(iris)))

## Variable labels
namlab(wlddev)
#>    Variable
#> 1   country
#> 2     iso3c
#> 3      date
#> 4      year
#> 5    decade
#> 6    region
#> 7    income
#> 8      OECD
#> 9     PCGDP
#> 10   LIFEEX
#> 11     GINI
#> 12      ODA
#> 13      POP
#>                                                                                Label
#> 1                                                                       Country Name
#> 2                                                                       Country Code
#> 3                                                         Date Recorded (Fictitious)
#> 4                                                                               Year
#> 5                                                                             Decade
#> 6                                                                             Region
#> 7                                                                       Income Level
#> 8                                                            Is OECD Member Country?
#> 9                                                 GDP per capita (constant 2010 US$)
#> 10                                           Life expectancy at birth, total (years)
#> 11                                                  Gini index (World Bank estimate)
#> 12 Net official development assistance and official aid received (constant 2018 US$)
#> 13                                                                 Population, total
namlab(wlddev, class = TRUE, N = TRUE, Ndistinct = TRUE)
#>    Variable     Class     N Ndist
#> 1   country character 13176   216
#> 2     iso3c    factor 13176   216
#> 3      date      Date 13176    61
#> 4      year   integer 13176    61
#> 5    decade   integer 13176     7
#> 6    region    factor 13176     7
#> 7    income    factor 13176     4
#> 8      OECD   logical 13176     2
#> 9     PCGDP   numeric  9470  9470
#> 10   LIFEEX   numeric 11670 10548
#> 11     GINI   numeric  1744   368
#> 12      ODA   numeric  8608  7832
#> 13      POP   numeric 12919 12877
#>                                                                                Label
#> 1                                                                       Country Name
#> 2                                                                       Country Code
#> 3                                                         Date Recorded (Fictitious)
#> 4                                                                               Year
#> 5                                                                             Decade
#> 6                                                                             Region
#> 7                                                                       Income Level
#> 8                                                            Is OECD Member Country?
#> 9                                                 GDP per capita (constant 2010 US$)
#> 10                                           Life expectancy at birth, total (years)
#> 11                                                  Gini index (World Bank estimate)
#> 12 Net official development assistance and official aid received (constant 2018 US$)
#> 13                                                                 Population, total
vlabels(wlddev)
#>                                                                             country 
#>                                                                      "Country Name" 
#>                                                                               iso3c 
#>                                                                      "Country Code" 
#>                                                                                date 
#>                                                        "Date Recorded (Fictitious)" 
#>                                                                                year 
#>                                                                              "Year" 
#>                                                                              decade 
#>                                                                            "Decade" 
#>                                                                              region 
#>                                                                            "Region" 
#>                                                                              income 
#>                                                                      "Income Level" 
#>                                                                                OECD 
#>                                                           "Is OECD Member Country?" 
#>                                                                               PCGDP 
#>                                                "GDP per capita (constant 2010 US$)" 
#>                                                                              LIFEEX 
#>                                           "Life expectancy at birth, total (years)" 
#>                                                                                GINI 
#>                                                  "Gini index (World Bank estimate)" 
#>                                                                                 ODA 
#> "Net official development assistance and official aid received (constant 2018 US$)" 
#>                                                                                 POP 
#>                                                                 "Population, total" 
vlabels(wlddev) <- vlabels(wlddev)

## Stub-renaming
log_mtc <- add_stub(log(mtcars), "log.")
head(log_mtc)
#>                    log.mpg  log.cyl log.disp   log.hp log.drat    log.wt
#> Mazda RX4         3.044522 1.791759 5.075174 4.700480 1.360977 0.9631743
#> Mazda RX4 Wag     3.044522 1.791759 5.075174 4.700480 1.360977 1.0560527
#> Datsun 710        3.126761 1.386294 4.682131 4.532599 1.348073 0.8415672
#> Hornet 4 Drive    3.063391 1.791759 5.552960 4.700480 1.124930 1.1678274
#> Hornet Sportabout 2.928524 2.079442 5.886104 5.164786 1.147402 1.2354715
#> Valiant           2.895912 1.791759 5.416100 4.653960 1.015231 1.2412686
#>                   log.qsec log.vs log.am log.gear  log.carb
#> Mazda RX4         2.800933   -Inf      0 1.386294 1.3862944
#> Mazda RX4 Wag     2.834389   -Inf      0 1.386294 1.3862944
#> Datsun 710        2.923699      0      0 1.386294 0.0000000
#> Hornet 4 Drive    2.967333      0   -Inf 1.098612 0.0000000
#> Hornet Sportabout 2.834389   -Inf   -Inf 1.098612 0.6931472
#> Valiant           3.006672      0   -Inf 1.098612 0.0000000
head(rm_stub(log_mtc, "log."))
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
rm(log_mtc)

## Setting dimension names of an object
head(setRownames(mtcars))
#>    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> 1 21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> 2 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> 3 22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> 4 21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> 5 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#> 6 18.1   6  225 105 2.76 3.460 20.22  1  0    3    1
ar <- array(1:9, c(3,3,3))
setRownames(ar)
#> , , 1
#> 
#>   [,1] [,2] [,3]
#> 1    1    4    7
#> 2    2    5    8
#> 3    3    6    9
#> 
#> , , 2
#> 
#>   [,1] [,2] [,3]
#> 1    1    4    7
#> 2    2    5    8
#> 3    3    6    9
#> 
#> , , 3
#> 
#>   [,1] [,2] [,3]
#> 1    1    4    7
#> 2    2    5    8
#> 3    3    6    9
#> 
setColnames(ar, c("a","b","c"))
#> , , 1
#> 
#>      a b c
#> [1,] 1 4 7
#> [2,] 2 5 8
#> [3,] 3 6 9
#> 
#> , , 2
#> 
#>      a b c
#> [1,] 1 4 7
#> [2,] 2 5 8
#> [3,] 3 6 9
#> 
#> , , 3
#> 
#>      a b c
#> [1,] 1 4 7
#> [2,] 2 5 8
#> [3,] 3 6 9
#> 
setDimnames(ar, c("a","b","c"), which = 3)
#> , , a
#> 
#>      [,1] [,2] [,3]
#> [1,]    1    4    7
#> [2,]    2    5    8
#> [3,]    3    6    9
#> 
#> , , b
#> 
#>      [,1] [,2] [,3]
#> [1,]    1    4    7
#> [2,]    2    5    8
#> [3,]    3    6    9
#> 
#> , , c
#> 
#>      [,1] [,2] [,3]
#> [1,]    1    4    7
#> [2,]    2    5    8
#> [3,]    3    6    9
#> 
setDimnames(ar, list(c("d","e","f"), c("a","b","c")), which = 2:3)
#> , , a
#> 
#>      d e f
#> [1,] 1 4 7
#> [2,] 2 5 8
#> [3,] 3 6 9
#> 
#> , , b
#> 
#>      d e f
#> [1,] 1 4 7
#> [2,] 2 5 8
#> [3,] 3 6 9
#> 
#> , , c
#> 
#>      d e f
#> [1,] 1 4 7
#> [2,] 2 5 8
#> [3,] 3 6 9
#> 
setDimnames(ar, list(c("g","h","i"), c("d","e","f"), c("a","b","c")))
#> , , a
#> 
#>   d e f
#> g 1 4 7
#> h 2 5 8
#> i 3 6 9
#> 
#> , , b
#> 
#>   d e f
#> g 1 4 7
#> h 2 5 8
#> i 3 6 9
#> 
#> , , c
#> 
#>   d e f
#> g 1 4 7
#> h 2 5 8
#> i 3 6 9
#> 

## Checking exact equality of multiple objects
all_identical(iris, iris, iris, iris)
#> [1] TRUE
l <- replicate(100, fmean(num_vars(iris), iris$Species), simplify = FALSE)
all_identical(l)
#> [1] TRUE
rm(l)

## Function names from expressions
ex = quote(sum(x) + mean(y) / z)
all.names(ex)
#> [1] "+"    "sum"  "x"    "/"    "mean" "y"    "z"   
all.vars(ex)
#> [1] "x" "y" "z"
all_funs(ex)
#> [1] "+"    "sum"  "/"    "mean"
rm(ex)
```
