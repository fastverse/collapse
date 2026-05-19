# Small Functions to Make R Programming More Efficient

A small set of functions to address some common inefficiencies in R,
such as the creation of logical vectors to compare quantities,
unnecessary copies of objects in elementary mathematical or
sub-assignment operations, obtaining information about objects (esp.
data frames), or dealing with missing values.

## Usage

``` r
anyv(x, value)              # Faster than any(x == value). See also kit::panyv()
allv(x, value)              # Faster than all(x == value). See also kit::pallv()
allNA(x)                    # Faster than all(is.na(x)). See also kit::pallNA()
whichv(x, value,            # Faster than which(x == value)
       invert = FALSE)      # or which(x != value). See also Note (3)
whichNA(x, invert = FALSE)  # Faster than which((!)is.na(x))
x %==% value
x %!=% value
alloc(value, n,             # Fast rep_len(value, n) or replicate(n, value).
      simplify = TRUE)      # simplify only works if length(value) == 1. See Details.
copyv(X, v, R, ..., invert  # Fast replace(X, v, R), replace(X, X (!/=)= v, R) or
    = FALSE, vind1 = FALSE, # replace(X, (!)v, R[(!)v]). See Details and Note (4).
    xlist = FALSE)          # For multi-replacement see also kit::vswitch()
setv(X, v, R, ..., invert   # Same for X[v] <- r, X[x (!/=)= v] <- r or
    = FALSE, vind1 = FALSE, # x[(!)v] <- r[(!)v]. Modifies X by reference, fastest.
    xlist = FALSE)          # X/R/V can also be lists/DFs. See Details and Examples.
setop(X, op, V, ...,        # Faster than X <- X +\-\*\/ V (modifies by reference)
      rowwise = FALSE)      # optionally can also add v to rows of a matrix or list
X %+=% V
X %-=% V
X %*=% V
X %/=% V
na_rm(x)                    # Fast: if(anyNA(x)) x[!is.na(x)] else x, last
na_locf(x, set = FALSE)     # obs. carried forward and first obs. carried back.
na_focb(x, set = FALSE)     # (by reference). These also support lists (NULL/empty)
na_omit(X, cols = NULL,     # Faster na.omit for matrices and data frames,
        na.attr = FALSE,    # can use selected columns to check, attach indices,
        prop = 0, ...)      # and remove cases with a proportion of values missing
na_insert(X, prop = 0.1,    # Insert missing values at random (by reference)
    value = NA, set = FALSE)
missing_cases(X, cols=NULL, # The opposite of complete.cases(), faster for DF's.
  prop = 0, count = FALSE)  # See also kit::panyNA(), kit::pallNA(), kit::pcountNA()
vlengths(X, use.names=TRUE) # Faster lengths() and nchar() (in C, no method dispatch)
vtypes(X, use.names = TRUE) # Get data storage types (faster vapply(X, typeof, ...))
vgcd(x)                     # Greatest common divisor of positive integers or doubles
fnlevels(x)                 # Faster version of nlevels(x) (for factors)
fnrow(X)                    # Faster nrow for data frames (not faster for matrices)
fncol(X)                    # Faster ncol for data frames (not faster for matrices)
fdim(X)                     # Faster dim for data frames (not faster for matrices)
seq_row(X)                  # Fast integer sequences along rows of X
seq_col(X)                  # Fast integer sequences along columns of X
vec(X)                      # Vectorization (stacking) of matrix or data frame/list
cinv(x)                     # Choleski (fast) inverse of symmetric PD matrix, e.g. X'X
```

## Arguments

- X, V, R:

  a vector, matrix or data frame.

- x, v:

  a (atomic) vector or matrix (`na_rm`/`locf`/`focb` also support
  lists).

- value:

  a single value of any (atomic) vector type. For `whichv` it can also
  be a `length(x)` vector.

- invert:

  logical. `TRUE` considers elements `x != value`.

- set:

  logical. `TRUE` transforms `x` by reference.

- simplify:

  logical. If `value` is a length-1 vector, `alloc()` with
  `simplify = TRUE` returns a length-n vector of the same type. If
  `simplify = FALSE`, the result is always a list.

- vind1:

  logical. If `length(v) == 1L`, setting `vind1 = TRUE` will interpret
  `v` as an index, rather than a value to search and replace.

- xlist:

  logical. If `X` is a list, the default is to treat it like a data
  frame and replace rows. Setting `xlist = TRUE` will treat `X` and its
  replacement `R` like 1-dimensional list vectors.

- op:

  an integer or character string indicating the operation to perform.

  |        |     |          |     |                 |
  |--------|-----|----------|-----|-----------------|
  | *Int.* |     | *String* |     | *Description*   |
  | 1      |     | `"+"`    |     | add `V`         |
  | 2      |     | `"-"`    |     | subtract `V`    |
  | 3      |     | `"*"`    |     | multiply by `V` |
  | 4      |     | `"/"`    |     | divide by `V`   |

- rowwise:

  logical. `TRUE` performs the operation between `V` and each row of
  `X`.

- cols:

  select columns to check for missing values using column names,
  indices, a logical vector or a function (e.g. `is.numeric`). The
  default is to check all columns, which could be inefficient.

- n:

  integer. The length of the vector to allocate with `value`.

- na.attr:

  logical. `TRUE` adds an attribute containing the removed cases. For
  compatibility reasons this is exactly the same format as `na.omit`
  i.e. the attribute is called "na.action" and of class "omit".

- prop:

  double. For `na_insert`: the proportion of observations to be randomly
  replaced with `NA`. For `missing_cases` and `na_omit`: the proportion
  of values missing for the case to be considered missing (within `cols`
  if specified). For matrices this is implemented in R as
  `rowSums(is.na(X)) >= max(as.integer(prop * ncol(X)), 1L)`. The C code
  for data frames works equivalently, and skips list- and raw-columns
  (`ncol(X)` is adjusted downwards).

- count:

  logical. `TRUE` returns the row-wise missing value count (within
  `cols`). This ignores `prop`.

- use.names:

  logical. Preserve names if `X` is a list.

- ...:

  for `na_omit`: further arguments passed to `[` for vectors and
  matrices. With indexed data it is also possible to specify the
  `drop.index.levels` argument, see
  [indexing](https://fastverse.org/collapse/reference/indexing.md). For
  `copyv`, `setv` and `setop`, the argument is unused, and serves as a
  placeholder for possible future arguments.

## Details

`alloc` is a fusion of [`rep_len`](https://rdrr.io/r/base/rep.html) and
[`replicate`](https://rdrr.io/r/base/lapply.html) that is faster in both
cases. If `value` is a length one vector and `simplify = TRUE`, the
functionality is as `rep_len(value, n)` i.e. the output is a length `n`
vector with `value`. Otherwise, it is equivalent to
`replicate(n, value, simplify = FALSE)`, i.e., the output is a
length-`n` list of the objects. For efficiency reasons the object is not
copied (only the pointer to the object is replicated).

`copyv` and `setv` are designed to optimize operations that require
replacing data in objects in the broadest sense. The only difference
between them is that `copyv` first deep-copies `X` before doing
replacements whereas `setv` modifies `X` in place and returns the result
invisibly. There are 3 ways these functions can be used:

1.  To replace a single value, `setv(X, v, R)` is an efficient
    alternative to `X[X == v] <- R`, and `copyv(X, v, R)` is more
    efficient than `replace(X, X == v, R)`. This can be inverted using
    `setv(X, v, R, invert = TRUE)`, equivalent to `X[X != v] <- R`.

2.  To do standard replacement with integer or logical indices i.e.
    `X[v] <- R` is more efficient using `setv(X, v, R)`, and, if `v` is
    logical, `setv(X, v, R, invert = TRUE)` is efficient for
    `X[!v] <- R`. To distinguish this from use case (1) when
    `length(v) == 1`, the argument `vind1 = TRUE` can be set to ensure
    that `v` is always interpreted as an index.

3.  To copy values from objects of equal size i.e. `setv(X, v, R)` is
    faster than `X[v] <- R[v]`, and `setv(X, v, R, invert = TRUE)` is
    faster than `X[!v] <- R[!v]`.

Both `X` and `R` can be atomic or data frames / lists. If `X` is a list,
the default behavior is to interpret it like a data frame, and apply
`setv/copyv` to each element/column of `X`. If `R` is also a list, this
is done using [`mapply`](https://rdrr.io/r/base/mapply.html). Thus
`setv/copyv` can also be used to replace elements or rows in data
frames, or copy rows from equally sized frames. Note that for replacing
subsets in data frames
[`set`](https://rdrr.io/pkg/data.table/man/assign.html) from
`data.table` provides a more convenient interface (and there is also
[`copy`](https://rdrr.io/pkg/data.table/man/copy.html) if you just want
to deep-copy an object without any modifications to it).

If `X` should not be interpreted like a data frame, setting
`xlist = TRUE` will interpret it like a 1D list-vector analogous to
atomic vectors, except that use case (1) is not permitted i.e. no value
comparisons on list elements.

## Note

1.  None of these functions (apart from `alloc`) currently support
    complex vectors.

2.  `setop` and the operators `%+=%`, `%-=%`, `%*=%` and `%/=%` also
    work with integer data, but do not perform any integer related
    checks. R's integers are bounded between +-2,147,483,647 and
    `NA_integer_` is stored as the value -2,147,483,648. Thus
    computations resulting in values exceeding +-2,147,483,647 will
    result in integer overflows, and `NA_integer_` should not occur on
    either side of a `setop` call. These are programmers functions and
    meant to provide the most efficient math possible to responsible
    users.

3.  It is possible to compare factors by the levels (e.g.
    `iris$Species %==% "setosa")`) or using integers
    (`iris$Species %==% 1L`). The latter is slightly more efficient.
    Nothing special is implemented for other objects apart from basic
    types, e.g. for dates (which are stored as doubles) you need to
    generate a date object i.e.
    `wlddev$date %==% as.Date("2019-01-01")`. Using
    `wlddev$date %==% "2019-01-01"` will give `integer(0)`.

4.  `setv/copyv` only allow positive integer indices being passed to
    `v`, and, for efficiency reasons, they only check the first and the
    last index. Thus if there are indices in the middle that fall
    outside of the data range it will terminate R.

## See also

[Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md),
[Small (Helper)
Functions](https://fastverse.org/collapse/reference/small-helpers.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
oldopts <- options(max.print = 70)
## Which value
whichNA(wlddev$PCGDP)                # Same as which(is.na(wlddev$PCGDP))
#>  [1]   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19
#> [20]  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38
#> [39]  39  40  41  42  61  62  63  64  65  66  67  68  69  70  71  72  73  74  75
#> [58]  76  77  78  79  80  81 122 183 184 185 186 187 188
#>  [ reached 'max' / getOption("max.print") -- omitted 3636 entries ]
whichNA(wlddev$PCGDP, invert = TRUE) # Same as which(!is.na(wlddev$PCGDP))
#>  [1]  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  82
#> [20]  83  84  85  86  87  88  89  90  91  92  93  94  95  96  97  98  99 100 101
#> [39] 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120
#> [58] 121 123 124 125 126 127 128 129 130 131 132 133 134
#>  [ reached 'max' / getOption("max.print") -- omitted 9400 entries ]
whichv(wlddev$country, "Chad")       # Same as which(wlddev$county == "Chad")
#>  [1] 2319 2320 2321 2322 2323 2324 2325 2326 2327 2328 2329 2330 2331 2332 2333
#> [16] 2334 2335 2336 2337 2338 2339 2340 2341 2342 2343 2344 2345 2346 2347 2348
#> [31] 2349 2350 2351 2352 2353 2354 2355 2356 2357 2358 2359 2360 2361 2362 2363
#> [46] 2364 2365 2366 2367 2368 2369 2370 2371 2372 2373 2374 2375 2376 2377 2378
#> [61] 2379
wlddev$country %==% "Chad"           # Same thing
#>  [1] 2319 2320 2321 2322 2323 2324 2325 2326 2327 2328 2329 2330 2331 2332 2333
#> [16] 2334 2335 2336 2337 2338 2339 2340 2341 2342 2343 2344 2345 2346 2347 2348
#> [31] 2349 2350 2351 2352 2353 2354 2355 2356 2357 2358 2359 2360 2361 2362 2363
#> [46] 2364 2365 2366 2367 2368 2369 2370 2371 2372 2373 2374 2375 2376 2377 2378
#> [61] 2379
whichv(wlddev$country, "Chad", TRUE) # Same as which(wlddev$county != "Chad")
#>  [1]  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
#> [26] 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50
#> [51] 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70
#>  [ reached 'max' / getOption("max.print") -- omitted 13045 entries ]
wlddev$country %!=% "Chad"           # Same thing
#>  [1]  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
#> [26] 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50
#> [51] 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70
#>  [ reached 'max' / getOption("max.print") -- omitted 13045 entries ]
lvec <- wlddev$country == "Chad"     # If we already have a logical vector...
whichv(lvec, FALSE)                  # is fastver than which(!lvec)
#>  [1]  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
#> [26] 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50
#> [51] 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70
#>  [ reached 'max' / getOption("max.print") -- omitted 13045 entries ]
rm(lvec)

# Using the %==% operator can yield tangible performance gains
fsubset(wlddev, iso3c %==% "DEU") # 3x faster than:
#>   country iso3c       date year decade                region      income OECD
#> 1 Germany   DEU 1961-01-01 1960   1960 Europe & Central Asia High income TRUE
#> 2 Germany   DEU 1962-01-01 1961   1960 Europe & Central Asia High income TRUE
#> 3 Germany   DEU 1963-01-01 1962   1960 Europe & Central Asia High income TRUE
#> 4 Germany   DEU 1964-01-01 1963   1960 Europe & Central Asia High income TRUE
#> 5 Germany   DEU 1965-01-01 1964   1960 Europe & Central Asia High income TRUE
#>   PCGDP   LIFEEX GINI ODA      POP
#> 1    NA 69.31002   NA  NA 72814900
#> 2    NA 69.50800   NA  NA 73377632
#> 3    NA 69.69154   NA  NA 74025784
#> 4    NA 69.85961   NA  NA 74714353
#> 5    NA 70.01371   NA  NA 75318337
#>  [ reached 'max' / getOption("max.print") -- omitted 56 rows ]
fsubset(wlddev, iso3c == "DEU")
#>   country iso3c       date year decade                region      income OECD
#> 1 Germany   DEU 1961-01-01 1960   1960 Europe & Central Asia High income TRUE
#> 2 Germany   DEU 1962-01-01 1961   1960 Europe & Central Asia High income TRUE
#> 3 Germany   DEU 1963-01-01 1962   1960 Europe & Central Asia High income TRUE
#> 4 Germany   DEU 1964-01-01 1963   1960 Europe & Central Asia High income TRUE
#> 5 Germany   DEU 1965-01-01 1964   1960 Europe & Central Asia High income TRUE
#>   PCGDP   LIFEEX GINI ODA      POP
#> 1    NA 69.31002   NA  NA 72814900
#> 2    NA 69.50800   NA  NA 73377632
#> 3    NA 69.69154   NA  NA 74025784
#> 4    NA 69.85961   NA  NA 74714353
#> 5    NA 70.01371   NA  NA 75318337
#>  [ reached 'max' / getOption("max.print") -- omitted 56 rows ]

# With multiple categories we can use %iin%
fsubset(wlddev, iso3c %iin% c("DEU", "ITA", "FRA"))
#>   country iso3c       date year decade                region      income OECD
#> 1  France   FRA 1961-01-01 1960   1960 Europe & Central Asia High income TRUE
#> 2  France   FRA 1962-01-01 1961   1960 Europe & Central Asia High income TRUE
#> 3  France   FRA 1963-01-01 1962   1960 Europe & Central Asia High income TRUE
#> 4  France   FRA 1964-01-01 1963   1960 Europe & Central Asia High income TRUE
#> 5  France   FRA 1965-01-01 1964   1960 Europe & Central Asia High income TRUE
#>      PCGDP   LIFEEX GINI ODA      POP
#> 1 12743.93 69.86829   NA  NA 46621669
#> 2 13203.32 70.11707   NA  NA 47240543
#> 3 13911.26 70.31463   NA  NA 47904877
#> 4 14572.28 70.51463   NA  NA 48582611
#> 5 15337.08 70.66341   NA  NA 49230595
#>  [ reached 'max' / getOption("max.print") -- omitted 178 rows ]

## Math by reference: permissible types of operations
x <- alloc(1.0, 1e5) # Vector
x %+=% 1
x %+=% 1:1e5
xm <- matrix(alloc(1.0, 1e5), ncol = 100) # Matrix
xm %+=% 1
xm %+=% 1:1e3
setop(xm, "+", 1:100, rowwise = TRUE)
xm %+=% xm
xm %+=% 1:1e5
xd <- qDF(replicate(100, alloc(1.0, 1e3), simplify = FALSE)) # Data Frame
xd %+=% 1
xd %+=% 1:1e3
setop(xd, "+", 1:100, rowwise = TRUE)
xd %+=% xd
rm(x, xm, xd)

## setv() and copyv()
x <- rnorm(100)
y <- sample.int(10, 100, replace = TRUE)
setv(y, 5, 0)            # Faster than y[y == 5] <- 0
setv(y, 4, x)            # Faster than y[y == 4] <- x[y == 4]
#> Warning: Type of R (double) is larger than X (integer) and thus coerced. This incurs loss of information, such as digits of real numbers being truncated upon coercion to integer. To avoid this, make sure X has a larger type than R: character > double > integer > logical.
setv(y, 20:30, y[40:50]) # Faster than y[20:30] <- y[40:50]
setv(y, 20:30, x)        # Faster than y[20:30] <- x[20:30]
#> Warning: Type of R (double) is larger than X (integer) and thus coerced. This incurs loss of information, such as digits of real numbers being truncated upon coercion to integer. To avoid this, make sure X has a larger type than R: character > double > integer > logical.
rm(x, y)

# Working with data frames, here returning copies of the frame
copyv(mtcars, 20:30, ss(mtcars, 10:20))
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag     21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> Datsun 710        22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#> Valiant           18.1   6  225 105 2.76 3.460 20.22  1  0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
copyv(mtcars, 20:30, fscale(mtcars))
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag     21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> Datsun 710        22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#> Valiant           18.1   6  225 105 2.76 3.460 20.22  1  0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
ftransform(mtcars, new = copyv(cyl, 4, vs))
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb new
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46  0  1    4    4   6
#> Mazda RX4 Wag     21.0   6  160 110 3.90 2.875 17.02  0  1    4    4   6
#> Datsun 710        22.8   4  108  93 3.85 2.320 18.61  1  1    4    1   1
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44  1  0    3    1   6
#> Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2   8
#>  [ reached 'max' / getOption("max.print") -- omitted 27 rows ]
# Column-wise:
copyv(mtcars, 2:3, fscale(mtcars), xlist = TRUE)
#>                    mpg        cyl        disp  hp drat    wt  qsec vs am gear
#> Mazda RX4         21.0 -0.1049878 -0.57061982 110 3.90 2.620 16.46  0  1    4
#> Mazda RX4 Wag     21.0 -0.1049878 -0.57061982 110 3.90 2.875 17.02  0  1    4
#> Datsun 710        22.8 -1.2248578 -0.99018209  93 3.85 2.320 18.61  1  1    4
#> Hornet 4 Drive    21.4 -0.1049878  0.22009369 110 3.08 3.215 19.44  1  0    3
#> Hornet Sportabout 18.7  1.0148821  1.04308123 175 3.15 3.440 17.02  0  0    3
#> Valiant           18.1 -0.1049878 -0.04616698 105 2.76 3.460 20.22  1  0    3
#>                   carb
#> Mazda RX4            4
#> Mazda RX4 Wag        4
#> Datsun 710           1
#> Hornet 4 Drive       1
#> Hornet Sportabout    2
#> Valiant              1
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
copyv(mtcars, 2:3, mtcars[4:5], xlist = TRUE)
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0 110 3.90 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag     21.0 110 3.90 110 3.90 2.875 17.02  0  1    4    4
#> Datsun 710        22.8  93 3.85  93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive    21.4 110 3.08 110 3.08 3.215 19.44  1  0    3    1
#> Hornet Sportabout 18.7 175 3.15 175 3.15 3.440 17.02  0  0    3    2
#> Valiant           18.1 105 2.76 105 2.76 3.460 20.22  1  0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]

## Missing values
mtc_na <- na_insert(mtcars, 0.15)    # Set 15% of values missing at random
fnobs(mtc_na)                        # See observation count
#>  mpg  cyl disp   hp drat   wt qsec   vs   am gear carb 
#>   28   28   28   28   28   28   28   28   28   28   28 
missing_cases(mtc_na)                # Fast equivalent to !complete.cases(mtc_na)
#>  [1]  TRUE  TRUE  TRUE FALSE  TRUE FALSE  TRUE  TRUE  TRUE  TRUE  TRUE  TRUE
#> [13]  TRUE FALSE  TRUE FALSE  TRUE FALSE  TRUE  TRUE  TRUE FALSE  TRUE  TRUE
#> [25] FALSE FALSE  TRUE FALSE  TRUE  TRUE  TRUE FALSE
missing_cases(mtc_na, cols = 3:4)    # Missing cases on certain columns?
#>  [1] FALSE FALSE  TRUE FALSE  TRUE FALSE FALSE FALSE  TRUE  TRUE FALSE FALSE
#> [13] FALSE FALSE  TRUE FALSE FALSE FALSE FALSE  TRUE FALSE FALSE FALSE  TRUE
#> [25] FALSE FALSE  TRUE FALSE FALSE FALSE FALSE FALSE
missing_cases(mtc_na, count = TRUE)  # Missing case count
#>  [1] 3 2 1 0 1 0 2 2 6 3 1 2 1 0 1 0 2 0 1 4 2 0 2 1 0 0 2 0 2 1 2 0
missing_cases(mtc_na, prop = 0.8)    # Cases with 80% or more missing
#>  [1] FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE
#> [13] FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE
#> [25] FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE
missing_cases(mtc_na, cols = 3:4, prop = 1)     # Cases mssing columns 3 and 4
#>  [1] FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE
#> [13] FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE
#> [25] FALSE FALSE FALSE FALSE FALSE FALSE FALSE FALSE
missing_cases(mtc_na, cols = 3:4, count = TRUE) # Missing case count on columns 3 and 4
#>  [1] 0 0 1 0 1 0 0 0 1 1 0 0 0 0 1 0 0 0 0 1 0 0 0 1 0 0 1 0 0 0 0 0

na_omit(mtc_na)                      # 12x faster than na.omit(mtc_na)
#>                      mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Hornet 4 Drive      21.4   6 258.0 110 3.08 3.215 19.44  1  0    3    1
#> Valiant             18.1   6 225.0 105 2.76 3.460 20.22  1  0    3    1
#> Merc 450SLC         15.2   8 275.8 180 3.07 3.780 18.00  0  0    3    3
#> Lincoln Continental 10.4   8 460.0 215 3.00 5.424 17.82  0  0    3    4
#> Fiat 128            32.4   4  78.7  66 4.08 2.200 19.47  1  1    4    1
#> Dodge Challenger    15.5   8 318.0 150 2.76 3.520 16.87  0  0    3    2
#>  [ reached 'max' / getOption("max.print") -- omitted 4 rows ]
na_omit(mtc_na, prop = 0.8)          # Only remove cases missing 80% or more
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46 NA  1   NA   NA
#> Mazda RX4 Wag     21.0  NA  160 110 3.90 2.875 17.02  0 NA    4    4
#> Datsun 710        22.8   4   NA  93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> Hornet Sportabout 18.7   8   NA 175 3.15 3.440 17.02  0  0    3    2
#> Valiant           18.1   6  225 105 2.76 3.460 20.22  1  0    3    1
#>  [ reached 'max' / getOption("max.print") -- omitted 26 rows ]
na_omit(mtc_na, na.attr = TRUE)      # Adds attribute with removed cases, like na.omit
#>                      mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Hornet 4 Drive      21.4   6 258.0 110 3.08 3.215 19.44  1  0    3    1
#> Valiant             18.1   6 225.0 105 2.76 3.460 20.22  1  0    3    1
#> Merc 450SLC         15.2   8 275.8 180 3.07 3.780 18.00  0  0    3    3
#> Lincoln Continental 10.4   8 460.0 215 3.00 5.424 17.82  0  0    3    4
#> Fiat 128            32.4   4  78.7  66 4.08 2.200 19.47  1  1    4    1
#> Dodge Challenger    15.5   8 318.0 150 2.76 3.520 16.87  0  0    3    2
#>  [ reached 'max' / getOption("max.print") -- omitted 4 rows ]
na_omit(mtc_na, cols = .c(vs, am))   # Removes only cases missing vs or am
#>                    mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Datsun 710        22.8   4    NA  93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive    21.4   6 258.0 110 3.08 3.215 19.44  1  0    3    1
#> Hornet Sportabout 18.7   8    NA 175 3.15 3.440 17.02  0  0    3    2
#> Valiant           18.1   6 225.0 105 2.76 3.460 20.22  1  0    3    1
#> Duster 360        14.3   8 360.0 245   NA 3.570 15.84  0  0   NA    4
#> Merc 280            NA   6 167.6  NA 3.92 3.440 18.30  1  0   NA    4
#>  [ reached 'max' / getOption("max.print") -- omitted 18 rows ]
na_omit(qM(mtc_na))                  # Also works for matrices
#>                      mpg cyl  disp  hp drat    wt  qsec vs am gear carb
#> Hornet 4 Drive      21.4   6 258.0 110 3.08 3.215 19.44  1  0    3    1
#> Valiant             18.1   6 225.0 105 2.76 3.460 20.22  1  0    3    1
#> Merc 450SLC         15.2   8 275.8 180 3.07 3.780 18.00  0  0    3    3
#> Lincoln Continental 10.4   8 460.0 215 3.00 5.424 17.82  0  0    3    4
#> Fiat 128            32.4   4  78.7  66 4.08 2.200 19.47  1  1    4    1
#> Dodge Challenger    15.5   8 318.0 150 2.76 3.520 16.87  0  0    3    2
#>  [ reached 'max' / getOption("max.print") -- omitted 4 rows ]
na_omit(mtc_na$vs, na.attr = TRUE)   # Also works with vectors
#>  [1] 0 1 1 0 1 0 1 1 0 0 0 0 0 0 1 1 1 0 0 0 0 1 0 1 0 0 0 1
#> attr(,"na.action")
#> [1]  1  8  9 20
#> attr(,"class")
#> [1] "omit"
na_rm(mtc_na$vs)                     # For vectors na_rm is faster ...
#>  [1] 0 1 1 0 1 0 1 1 0 0 0 0 0 0 1 1 1 0 0 0 0 1 0 1 0 0 0 1
rm(mtc_na)

## Efficient vectorization
head(vec(EuStockMarkets)) # Atomic objects: no copy at all
#> [1] 1628.75 1613.63 1606.51 1621.04 1618.16 1610.61
head(vec(mtcars))         # Lists: directly in C
#> [1] 21.0 21.0 22.8 21.4 18.7 18.1

options(oldopts)
```
