# Time Series and Panel Series

*collapse* provides a flexible and powerful set of functions and classes
to work with time-dependent data:

- [`findex_by/iby`](https://fastverse.org/collapse/reference/indexing.md)
  creates an 'indexed_frame': a flexible structure that can be imposed
  upon any data-frame like object and facilitates **indexed (time-aware)
  computations on time series and panel data**. Indexed frames are
  composed of 'indexed_series', which can also be created from vector
  and matrix-based objects using the `reindex` function. Further
  functions `findex/ix`, `unindex`, `is_irregular` and `to_plm` help
  operate these classes, check for irregularity, and ensure *plm*
  compatibility. Methods are defined for various time series, data
  transformation and data manipulation functions in *collapse*.

- [`timeid`](https://fastverse.org/collapse/reference/timeid.md)
  efficiently converts numeric time sequences, such as 'Date' or
  'POSIXct' vectors, to a **time-factor / integer id**, where a
  unit-step represents the greatest common divisor of the underlying
  sequence.

- [`flag`](https://fastverse.org/collapse/reference/flag.md), and the
  lag- and lead- operators
  [`L`](https://fastverse.org/collapse/reference/flag.md) and
  [`F`](https://fastverse.org/collapse/reference/flag.md) are S3
  generics to efficiently compute sequences of **lags and leads** on
  regular or irregular / unbalanced time series and panel data.

- Similarly,
  [`fdiff`](https://fastverse.org/collapse/reference/fdiff.md),
  [`fgrowth`](https://fastverse.org/collapse/reference/fgrowth.md), and
  the operators
  [`D`](https://fastverse.org/collapse/reference/fdiff.md),
  [`Dlog`](https://fastverse.org/collapse/reference/fdiff.md) and
  [`G`](https://fastverse.org/collapse/reference/fgrowth.md) are S3
  generics to efficiently compute sequences of suitably lagged / leaded
  and iterated **differences, log-differences and growth rates**.
  [`fdiff/D/Dlog`](https://fastverse.org/collapse/reference/fdiff.md)
  can also compute **quasi-differences** of the form \\x_t - \rho
  x\_{t-1}\\.

- [`fcumsum`](https://fastverse.org/collapse/reference/fcumsum.md) is an
  S3 generic to efficiently compute **cumulative sums** on time series
  and panel data. In contrast to
  [`cumsum`](https://rdrr.io/r/base/cumsum.html), it can handle missing
  values and supports both grouped and indexed / ordered computations.

- [`psmat`](https://fastverse.org/collapse/reference/psmat.md) is an S3
  generic to efficiently convert panel-vectors / 'indexed_series' and
  data frames / 'indexed_frame's to **panel series matrices and 3D
  arrays**, respectively (where time, individuals and variables receive
  different dimensions, allowing for fast indexation, visualization, and
  computations).

- [`psacf`](https://fastverse.org/collapse/reference/psacf.md),
  [`pspacf`](https://fastverse.org/collapse/reference/psacf.md) and
  [`psccf`](https://fastverse.org/collapse/reference/psacf.md) are S3
  generics to compute estimates of the **auto-, partial auto- and cross-
  correlation or covariance functions** for panel-vectors /
  'indexed_series', and multivariate versions for data frames /
  'indexed_frame's.

## Table of Functions

|  |  |  |  |  |
|----|----|----|----|----|
| *S3 Generic* |  | *Methods* |  | *Description* |
| [`findex_by/iby`](https://fastverse.org/collapse/reference/indexing.md), `findex/ix`, `reindex`, `unindex`, `is_irregular`, `to_plm` |  | For vectors, matrices and data frames / lists. |  | Fast and flexible time series and panel data classes 'indexed_series' and 'indexed_frame'. |
| [`timeid`](https://fastverse.org/collapse/reference/timeid.md) |  | For time sequences represented by integer or double vectors / objects. |  | Generate integer time-id/factor |
| [`flag/L/F`](https://fastverse.org/collapse/reference/flag.md) |  | `default, matrix, data.frame, pseries, pdata.frame, grouped_df` |  | Compute (sequences of) lags and leads |
| [`fdiff/D/Dlog`](https://fastverse.org/collapse/reference/fdiff.md) |  | `default, matrix, data.frame, pseries, pdata.frame, grouped_df` |  | Compute (sequences of lagged / leaded and iterated) (quasi-)differences or log-differences |
| [`fgrowth/G`](https://fastverse.org/collapse/reference/fgrowth.md) |  | `default, matrix, data.frame, pseries, pdata.frame, grouped_df` |  | Compute (sequences of lagged / leaded and iterated) growth rates (exact, via log-differencing, or compounded) |
| [`fcumsum`](https://fastverse.org/collapse/reference/fcumsum.md) |  | `default, matrix, data.frame, pseries, pdata.frame, grouped_df` |  | Compute cumulative sums |
| [`psmat`](https://fastverse.org/collapse/reference/psmat.md) |  | `default, pseries, data.frame, pdata.frame` |  | Convert panel data to matrix / array |
| [`psacf`](https://fastverse.org/collapse/reference/psacf.md) |  | `default, pseries, data.frame, pdata.frame` |  | Compute ACF on panel data |
| [`pspacf`](https://fastverse.org/collapse/reference/psacf.md) |  | `default, pseries, data.frame, pdata.frame` |  | Compute PACF on panel data |
| [`psccf`](https://fastverse.org/collapse/reference/psacf.md) |  | `default, pseries, data.frame, pdata.frame` |  | Compute CCF on panel data |

## See also

[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md),
[Data
Transformations](https://fastverse.org/collapse/reference/data-transformations.md)
