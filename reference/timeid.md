# Generate Integer-Id From Time/Date Sequences

`timeid` groups time vectors in a way that preserves the temporal
structure. It generate an integer id where unit steps represent the
greatest common divisor in the original sequence e.g
`c(4, 6, 10) -> c(1, 2, 4)` or `c(0.25, 0.75, 1) -> c(1, 3, 4)`.

## Usage

``` r
timeid(x, factor = FALSE, ordered = factor, extra = FALSE)
```

## Arguments

- x:

  a numeric time object such as a `Date`, `POSIXct` or other integer or
  double vector representing time.

- factor:

  logical. `TRUE` returns an (ordered) factor with levels corresponding
  to the full sequence (without irregular gaps) of time. This is useful
  for inclusion in the
  [index](https://fastverse.org/collapse/reference/indexing.md) but
  might be computationally expensive for long sequences, see Details.
  `FALSE` returns a simpler object of class
  '[`qG`](https://fastverse.org/collapse/reference/qF.md)'.

- ordered:

  logical. `TRUE` adds a class 'ordered'.

- extra:

  logical. `TRUE` attaches a set of 4 diagnostic items as attributes to
  the result:

  - `"unique_ints"`: `unique(unattrib(timeid(x)))` - the unique integer
    time steps in first-appearance order. This can be useful to check
    the size of gaps in the sequence.

  - `"sort_unique_x"`: `sort(unique(x))`.

  - `"range_x"`: `range(x)`.

  - `"step_x"`: `vgcd(sort(unique(diff(sort(unique(x))))))` - the
    greatest common divisor.

  *Note* that returning these attributes does not incur additional
  computations.

## Details

Let `range_x` and `step_x` be the like-named attributes returned when
`extra = TRUE`, then, if `factor = TRUE`, a complete sequence of levels
is generated as
`seq(range_x[1], range_x[2], by = step_x) |> copyMostAttrib(x) |> as.character()`.
If `factor = FALSE`, the number of timesteps recorded in the
`"N.groups"` attribute is computed as
`(range_x[2]-range_x[1])/step_x + 1`, which is equal to the number of
factor levels. In both cases the underlying integer id is the same and
preserves gaps in time. Large gaps (strong irregularity) can result in
many unused factor levels, the generation of which can become expensive.
Using `factor = FALSE` (the default) is thus more efficient.

## Value

A factor or '[`qG`](https://fastverse.org/collapse/reference/qF.md)'
object, optionally with additional attributes attached.

## See also

[`seqid`](https://fastverse.org/collapse/reference/seqid.md),
[Indexing](https://fastverse.org/collapse/reference/indexing.md), [Time
Series and Panel
Series](https://fastverse.org/collapse/reference/time-series-panel-series.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
oldopts <- options(max.print = 30)

# A normal use case
timeid(wlddev$decade)
#>  [1] 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 3 3
#>  [ reached 'max' / getOption("max.print") -- omitted 13146 entries ]
#> attr(,"N.groups")
#> [1] 7
#> attr(,"class")
#> [1] "qG"          "na.included"
timeid(wlddev$decade, factor = TRUE)
#>  [1] 1960 1960 1960 1960 1960 1960 1960 1960 1960 1960 1970 1970 1970 1970 1970
#> [16] 1970 1970 1970 1970 1970 1980 1980 1980 1980 1980 1980 1980 1980 1980 1980
#>  [ reached 'max' / getOption("max.print") -- omitted 13146 entries ]
#> Levels: 1960 < 1970 < 1980 < 1990 < 2000 < 2010 < 2020
timeid(wlddev$decade, extra = TRUE)
#>  [1] 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 3 3
#>  [ reached 'max' / getOption("max.print") -- omitted 13146 entries ]
#> attr(,"N.groups")
#> [1] 7
#> attr(,"class")
#> [1] "qG"          "na.included"
#> attr(,"unique_ints")
#> [1] 1 2 3 4 5 6 7
#> attr(,"sort_unique_x")
#> [1] 1960 1970 1980 1990 2000 2010 2020
#> attr(,"sort_unique_x")attr(,"label")
#> [1] "Decade"
#> attr(,"range_x")
#> [1] 1960 2020
#> attr(,"range_x")attr(,"label")
#> [1] "Decade"
#> attr(,"step_x")
#> [1] 10

# Here a large number of levels is generated, which is expensive
timeid(wlddev$date, factor = TRUE)
#>  [1] 1961-01-01 1962-01-01 1963-01-01 1964-01-01 1965-01-01 1966-01-01
#>  [7] 1967-01-01 1968-01-01 1969-01-01 1970-01-01 1971-01-01 1972-01-01
#> [13] 1973-01-01 1974-01-01 1975-01-01 1976-01-01 1977-01-01 1978-01-01
#> [19] 1979-01-01 1980-01-01 1981-01-01 1982-01-01 1983-01-01 1984-01-01
#> [25] 1985-01-01 1986-01-01 1987-01-01 1988-01-01 1989-01-01 1990-01-01
#>  [ reached 'max' / getOption("max.print") -- omitted 13146 entries ]
#> 21916 Levels: 1961-01-01 < 1961-01-02 < 1961-01-03 < 1961-01-04 < ... < 2021-01-01
tid <- timeid(wlddev$date, extra = TRUE) # Much faster
str(tid)
#>  'qG' int [1:13176] 1 366 731 1096 1462 1827 2192 2557 2923 3288 ...
#>  - attr(*, "N.groups")= int 21916
#>  - attr(*, "unique_ints")= int [1:61] 1 366 731 1096 1462 1827 2192 2557 2923 3288 ...
#>  - attr(*, "sort_unique_x")= Date[1:61], format: "1961-01-01" "1962-01-01" ...
#>  - attr(*, "range_x")= Date[1:2], format: "1961-01-01" "2021-01-01"
#>  - attr(*, "step_x")= num 1

# The reason for step = 1 are leap years with 366 days every 4 years
diff(attr(tid, "unique"))
#>  [1] 365 365 365 366 365 365 365 366 365 365 365 366 365 365 365 366 365 365 365
#> [20] 366 365 365 365 366 365 365 365 366 365 365
#>  [ reached 'max' / getOption("max.print") -- omitted 30 entries ]

# So in this case simple factor generation gives a better result
qF(wlddev$date, ordered = TRUE, na.exclude = FALSE)
#>  [1] 1961-01-01 1962-01-01 1963-01-01 1964-01-01 1965-01-01 1966-01-01
#>  [7] 1967-01-01 1968-01-01 1969-01-01 1970-01-01 1971-01-01 1972-01-01
#> [13] 1973-01-01 1974-01-01 1975-01-01 1976-01-01 1977-01-01 1978-01-01
#> [19] 1979-01-01 1980-01-01 1981-01-01 1982-01-01 1983-01-01 1984-01-01
#> [25] 1985-01-01 1986-01-01 1987-01-01 1988-01-01 1989-01-01 1990-01-01
#>  [ reached 'max' / getOption("max.print") -- omitted 13146 entries ]
#> attr(,"label")
#> [1] Date Recorded (Fictitious)
#> 61 Levels: 1961-01-01 < 1962-01-01 < 1963-01-01 < 1964-01-01 < ... < 2021-01-01

# The best way to deal with this data would be to convert it
# to zoo::yearmon and then use timeid:
timeid(zoo::as.yearmon(wlddev$date), factor = TRUE, extra = TRUE)
#>  [1] Jan 1961 Jan 1962 Jan 1963 Jan 1964 Jan 1965 Jan 1966 Jan 1967 Jan 1968
#>  [9] Jan 1969 Jan 1970 Jan 1971 Jan 1972 Jan 1973 Jan 1974 Jan 1975 Jan 1976
#> [17] Jan 1977 Jan 1978 Jan 1979 Jan 1980 Jan 1981 Jan 1982 Jan 1983 Jan 1984
#> [25] Jan 1985 Jan 1986 Jan 1987 Jan 1988 Jan 1989 Jan 1990
#>  [ reached 'max' / getOption("max.print") -- omitted 13146 entries ]
#> attr(,"unique_ints")
#>  [1]  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
#> [26] 26 27 28 29 30
#>  [ reached 'max' / getOption("max.print") -- omitted 31 entries ]
#> attr(,"sort_unique_x")
#>  [1] Jan 1961 Jan 1962 Jan 1963 Jan 1964 Jan 1965 Jan 1966 Jan 1967 Jan 1968
#>  [9] Jan 1969 Jan 1970 Jan 1971 Jan 1972 Jan 1973 Jan 1974 Jan 1975 Jan 1976
#> [17] Jan 1977 Jan 1978 Jan 1979 Jan 1980 Jan 1981 Jan 1982 Jan 1983 Jan 1984
#> [25] Jan 1985 Jan 1986 Jan 1987 Jan 1988 Jan 1989 Jan 1990
#>  [ reached 'max' / getOption("max.print") -- omitted 31 entries ]
#> attr(,"range_x")
#> [1] Jan 1961 Jan 2021
#> attr(,"step_x")
#> [1] 1
#> 61 Levels: Jan 1961 < Jan 1962 < Jan 1963 < Jan 1964 < ... < Jan 2021

options(oldopts)
rm(oldopts, tid)
```
