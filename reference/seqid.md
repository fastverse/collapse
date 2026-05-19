# Generate Group-Id from Integer Sequences

`seqid` can be used to group sequences of integers in a vector, e.g.
`seqid(c(1:3, 5:7))` becomes `c(rep(1,3), rep(2,3))`. It also supports
increments `> 1`, unordered sequences, and missing values in the
sequence.

Some applications are to facilitate identification of, and grouped
operations on, (irregular) time series and panels.

## Usage

``` r
seqid(x, o = NULL, del = 1L, start = 1L, na.skip = FALSE,
      skip.seq = FALSE, check.o = TRUE)
```

## Arguments

- x:

  a factor or integer vector. Numeric vectors will be converted to
  integer i.e. rounded downwards.

- o:

  an (optional) integer ordering vector specifying the order by which to
  pass through `x`.

- del:

  integer. The integer deliminating two consecutive points in a
  sequence. `del = 1` lets `seqid` track sequences of the form
  `c(1,2,3,..)`, `del = 2` tracks sequences `c(1,3,5,..)` etc.

- start:

  integer. The starting value of the resulting sequence id. Default is
  starting from 1.

- na.skip:

  logical. `TRUE` skips missing values in the sequence. The default
  behavior is skipping such that `seqid(c(1, NA, 2))` is regarded as one
  sequence and coded as `c(1, NA, 1)`.

- skip.seq:

  logical. If `na.skip = TRUE`, this changes the behavior such that
  missing values are viewed as part of the sequence, i.e.
  `seqid(c(1, NA, 3))` is regarded as one sequence and coded as
  `c(1, NA, 1)`.

- check.o:

  logical. Programmers option: `FALSE` prevents checking that each
  element of `o` is in the range `[1, length(x)]`, it only checks the
  length of `o`. This gives some extra speed, but will terminate R if
  any element of `o` is too large or too small.

## Details

`seqid` was created primarily as a workaround to deal with problems of
computing lagged values, differences and growth rates on irregularly
spaced time series and panels before *collapse* version 1.5.0
([\#26](https://github.com/fastverse/collapse/issues/26)). Now `flag`,
`fdiff` and `fgrowth` natively support irregular data so this workaround
is superfluous, except for iterated differencing which is not yet
supported with irregular data.

The theory of the workaround was to express an irregular time series or
panel series as a regular panel series with a group-id created such that
the time-periods within each group are consecutive. `seqid` makes this
very easy: For an irregular panel with some gaps or repeated values in
the time variable, an appropriate id variable can be generated using
`settransform(data, newid = seqid(time, radixorder(id, time)))`. Lags
can then be computed using `L(data, 1, ~newid, ~time)` etc.

In general, for any regularly spaced panel the identity given by
`identical(groupid(id, order(id, time)), seqid(time, order(id, time)))`
should hold.

For the opposite operation of creating a new time-variable that is
consecutive in each group, see
[`data.table::rowid`](https://rdrr.io/pkg/data.table/man/rowid.html).

## Value

An integer vector of class 'qG'. See
[`qG`](https://fastverse.org/collapse/reference/qF.md).

## See also

[`timeid`](https://fastverse.org/collapse/reference/timeid.md),
[`groupid`](https://fastverse.org/collapse/reference/groupid.md),
[`qG`](https://fastverse.org/collapse/reference/qF.md), [Fast Grouping
and
Ordering](https://fastverse.org/collapse/reference/fast-grouping-ordering.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
## This creates an irregularly spaced panel, with a gap in time for id = 2
data <- data.frame(id = rep(1:3, each = 4),
                   time = c(1:4, 1:2, 4:5, 1:4),
                   value = rnorm(12))
data
#>    id time        value
#> 1   1    1 -1.540203133
#> 2   1    2 -2.131035606
#> 3   1    3  0.377222256
#> 4   1    4  1.693972475
#> 5   2    1  0.512538179
#> 6   2    2 -1.414468126
#> 7   2    4 -0.007331919
#> 8   2    5 -1.392200673
#> 9   3    1 -1.007205635
#> 10  3    2 -1.300076492
#> 11  3    3  0.749182687
#> 12  3    4  1.687936283

## This gave a gaps in time error previous to collapse 1.5.0
L(data, 1, value ~ id, ~time)
#>    id time     L1.value
#> 1   1    1           NA
#> 2   1    2 -1.540203133
#> 3   1    3 -2.131035606
#> 4   1    4  0.377222256
#> 5   2    1           NA
#> 6   2    2  0.512538179
#> 7   2    4           NA
#> 8   2    5 -0.007331919
#> 9   3    1           NA
#> 10  3    2 -1.007205635
#> 11  3    3 -1.300076492
#> 12  3    4  0.749182687

## Generating new id variable (here seqid(time) would suffice as data is sorted)
settransform(data, newid = seqid(time, order(id, time)))
data
#>    id time        value newid
#> 1   1    1 -1.540203133     1
#> 2   1    2 -2.131035606     1
#> 3   1    3  0.377222256     1
#> 4   1    4  1.693972475     1
#> 5   2    1  0.512538179     2
#> 6   2    2 -1.414468126     2
#> 7   2    4 -0.007331919     3
#> 8   2    5 -1.392200673     3
#> 9   3    1 -1.007205635     4
#> 10  3    2 -1.300076492     4
#> 11  3    3  0.749182687     4
#> 12  3    4  1.687936283     4

## Lag the panel this way
L(data, 1, value ~ newid, ~time)
#>    newid time     L1.value
#> 1      1    1           NA
#> 2      1    2 -1.540203133
#> 3      1    3 -2.131035606
#> 4      1    4  0.377222256
#> 5      2    1           NA
#> 6      2    2  0.512538179
#> 7      3    4           NA
#> 8      3    5 -0.007331919
#> 9      4    1           NA
#> 10     4    2 -1.007205635
#> 11     4    3 -1.300076492
#> 12     4    4  0.749182687

## A different possibility: Creating a consecutive time variable
settransform(data, newtime = data.table::rowid(id))
data
#>    id time        value newid newtime
#> 1   1    1 -1.540203133     1       1
#> 2   1    2 -2.131035606     1       2
#> 3   1    3  0.377222256     1       3
#> 4   1    4  1.693972475     1       4
#> 5   2    1  0.512538179     2       1
#> 6   2    2 -1.414468126     2       2
#> 7   2    4 -0.007331919     3       3
#> 8   2    5 -1.392200673     3       4
#> 9   3    1 -1.007205635     4       1
#> 10  3    2 -1.300076492     4       2
#> 11  3    3  0.749182687     4       3
#> 12  3    4  1.687936283     4       4
L(data, 1, value ~ id, ~newtime)
#>    id newtime     L1.value
#> 1   1       1           NA
#> 2   1       2 -1.540203133
#> 3   1       3 -2.131035606
#> 4   1       4  0.377222256
#> 5   2       1           NA
#> 6   2       2  0.512538179
#> 7   2       3 -1.414468126
#> 8   2       4 -0.007331919
#> 9   3       1           NA
#> 10  3       2 -1.007205635
#> 11  3       3 -1.300076492
#> 12  3       4  0.749182687

## With sorted data, the time variable can also just be omitted..
L(data, 1, value ~ id)
#>    id     L1.value
#> 1   1           NA
#> 2   1 -1.540203133
#> 3   1 -2.131035606
#> 4   1  0.377222256
#> 5   2           NA
#> 6   2  0.512538179
#> 7   2 -1.414468126
#> 8   2 -0.007331919
#> 9   3           NA
#> 10  3 -1.007205635
#> 11  3 -1.300076492
#> 12  3  0.749182687
```
