# Fast Grouping and Ordering

*collapse* provides the following functions to efficiently group and
order data:

- [`radixorder(v)`](https://fastverse.org/collapse/reference/radixorder.md),
  provides fast radix-ordering through direct access to the method
  [`order(..., method = "radix")`](https://rdrr.io/r/base/order.html),
  as well as the possibility to return some attributes very useful for
  grouping data and finding unique elements. The function
  [`roworder(v)`](https://fastverse.org/collapse/reference/roworder.md)
  efficiently reorders a data frame.

- [`group(v)`](https://fastverse.org/collapse/reference/group.md)
  provides fast grouping in first-appearance order of rows, based on a
  hashing algorithm in C. Objects have class 'qG', see below.

- [`GRP`](https://fastverse.org/collapse/reference/GRP.md) creates
  *collapse* grouping objects of class 'GRP' based on
  [`radixorder`](https://fastverse.org/collapse/reference/radixorder.md)
  or [`group`](https://fastverse.org/collapse/reference/group.md). 'GRP'
  objects form the central building block for grouped operations and
  programming in *collapse* and are very efficient inputs to all
  *collapse* functions supporting grouped operations.

- [`fgroup_by`](https://fastverse.org/collapse/reference/GRP.md)
  provides a fast replacement for
  [`dplyr::group_by`](https://dplyr.tidyverse.org/reference/group_by.html),
  creating a grouped data frame (or data.table / tibble etc.) with a
  'GRP' object attached. This grouped frame can be used for grouped
  operations using *collapse*'s fast functions.

- [`fmatch`](https://fastverse.org/collapse/reference/fmatch.md) is a
  fast alternative to [`match`](https://rdrr.io/r/base/match.html),
  which also supports matching of data frame rows.

- [`funique`](https://fastverse.org/collapse/reference/funique.md) is a
  faster version of [`unique`](https://rdrr.io/r/base/unique.html). The
  data frame method also allows selecting unique rows according to a
  subset of the columns.
  [`fnunique`](https://fastverse.org/collapse/reference/funique.md)
  efficiently calculates the number of unique values/rows.
  [`fduplicated`](https://fastverse.org/collapse/reference/funique.md)
  is a fast alternative to
  [`duplicated`](https://rdrr.io/r/base/duplicated.html).
  [`any_duplicated`](https://fastverse.org/collapse/reference/funique.md)
  is a simpler and faster alternative to
  [`anyDuplicated`](https://rdrr.io/r/base/duplicated.html).

- [`fcount(v)`](https://fastverse.org/collapse/reference/fcount.md)
  computes group counts based on a subset of columns in the data, and is
  a fast replacement for
  [`dplyr::count`](https://dplyr.tidyverse.org/reference/count.html).

- [`qF`](https://fastverse.org/collapse/reference/qF.md), shorthand for
  'quick-factor' implements very fast factor generation from atomic
  vectors using either radix ordering `method = "radix"` or hashing
  `method = "hash"`. Factors can also be used for efficient grouped
  programming with *collapse* functions, especially if they are
  generated using `qF(x, na.exclude = FALSE)` which assigns a level to
  missing values and attaches a class 'na.included' ensuring that no
  additional missing value checks are executed by *collapse* functions.

- [`qG`](https://fastverse.org/collapse/reference/qF.md), shorthand for
  'quick-group', generates a kind of factor-light without the levels
  attribute but instead an attribute providing the number of levels.
  Optionally the levels / groups can be attached, but without converting
  them to character. Objects have a class 'qG', which is also recognized
  in the *collapse* ecosystem.

- [`fdroplevels`](https://fastverse.org/collapse/reference/fdroplevels.md)
  is a substantially faster replacement for
  [`droplevels`](https://rdrr.io/r/base/droplevels.html).

- [`finteraction`](https://fastverse.org/collapse/reference/qF.md) is a
  fast alternative to
  [`interaction`](https://rdrr.io/r/base/interaction.html) implemented
  as a wrapper around `as_factor_GRP(GRP(...))`. It can be used to
  generate a factor from multiple vectors, factors or a list of vectors
  / factors. Unused factor levels are always dropped.

- [`groupid`](https://fastverse.org/collapse/reference/groupid.md) is a
  generalization of
  [`data.table::rleid`](https://rdrr.io/pkg/data.table/man/rleid.html)
  providing a run-length type group-id from atomic vectors. It is
  generalization as it also supports passing an ordering vector and
  skipping missing values. For example
  [`qF`](https://fastverse.org/collapse/reference/qF.md) and
  [`qG`](https://fastverse.org/collapse/reference/qF.md) with
  `method = "radix"` are essentially implemented using
  `groupid(x, radixorder(x))`.

- [`seqid`](https://fastverse.org/collapse/reference/seqid.md) is a
  specialized function which creates a group-id from sequences of
  integer values. For any regular panel dataset
  `groupid(id, order(id, time))` and `seqid(time, order(id, time))`
  provide the same id variable.
  [`seqid`](https://fastverse.org/collapse/reference/seqid.md) is
  especially useful for identifying discontinuities in time-sequences.

- [`timeid`](https://fastverse.org/collapse/reference/timeid.md) is a
  specialized function to convert integer or double vectors representing
  time (such as 'Date', 'POSIXct' etc.) to factor or 'qG' object based
  on the greatest common divisor of elements (thus preserving gaps in
  time intervals).

## Table of Functions

|  |  |  |  |  |
|----|----|----|----|----|
| *Function / S3 Generic* |  | *Methods* |  | *Description* |
| [`radixorder(v)`](https://fastverse.org/collapse/reference/radixorder.md) |  | No methods, for data frames and vectors |  | Radix-based ordering + grouping information |
| [`roworder(v)`](https://fastverse.org/collapse/reference/roworder.md) |  | No methods, for data frames incl. pdata.frame |  | Row sorting/reordering |
| [`group(v)`](https://fastverse.org/collapse/reference/group.md) |  | No methods, for data frames and vectors |  | Hash-based grouping + grouping information |
| [`GRP`](https://fastverse.org/collapse/reference/GRP.md) |  | `default, GRP, factor, qG, grouped_df, pseries, pdata.frame` |  | Fast grouping and a flexible grouping object |
| [`fgroup_by`](https://fastverse.org/collapse/reference/GRP.md) |  | No methods, for data frames |  | Fast grouped data frame |
| [`fmatch`](https://fastverse.org/collapse/reference/fmatch.md) |  | No methods, for vectors and data frames |  | Fast matching |
| [`funique`](https://fastverse.org/collapse/reference/funique.md), [`fnunique`](https://fastverse.org/collapse/reference/funique.md), [`fduplicated`](https://fastverse.org/collapse/reference/funique.md), [`any_duplicated`](https://fastverse.org/collapse/reference/funique.md) |  | `default, data.frame, sf, pseries, pdata.frame, list` |  | Fast (number of) unique values/rows |
| [`fcount(v)`](https://fastverse.org/collapse/reference/fcount.md) |  | Internal generic, supports vectors, matrices, data.frames, lists, grouped_df and pdata.frame |  | Fast group counts |
| [`qF`](https://fastverse.org/collapse/reference/qF.md) |  | No methods, for vectors |  | Quick factor generation |
| [`qG`](https://fastverse.org/collapse/reference/qF.md) |  | No methods, for vectors |  | Quick grouping of vectors and a 'factor-light' class |
| [`fdroplevels`](https://fastverse.org/collapse/reference/fdroplevels.md) |  | `factor, data.frame, list` |  | Fast removal of unused factor levels |
| [`finteraction`](https://fastverse.org/collapse/reference/qF.md) |  | No methods, for data frames and vectors |  | Fast interactions |
| [`groupid`](https://fastverse.org/collapse/reference/groupid.md) |  | No methods, for vectors |  | Run-length type group-id |
| [`seqid`](https://fastverse.org/collapse/reference/seqid.md) |  | No methods, for integer vectors |  | Run-length type integer sequence-id |
| [`timeid`](https://fastverse.org/collapse/reference/timeid.md) |  | No methods, for integer or double vectors |  | Integer-id from time/date sequences |

## See also

[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md),
[Data Frame
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Time Series and Panel
Series](https://fastverse.org/collapse/reference/time-series-panel-series.md)
