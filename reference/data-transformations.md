# Data Transformations

*collapse* provides an ensemble of functions to perform common data
transformations efficiently and user friendly:

- [`dapply`](https://fastverse.org/collapse/reference/dapply.md)
  **applies functions to rows or columns** of matrices and data frames,
  preserving the data format.

- [`BY`](https://fastverse.org/collapse/reference/BY.md) is an S3
  generic for efficient **Split-Apply-Combine computing**, similar to
  [`dapply`](https://fastverse.org/collapse/reference/dapply.md).

- A set of arithmetic operators facilitates **row-wise**
  [`%rr%`](https://fastverse.org/collapse/reference/arithmetic.md),
  [`%r+%`](https://fastverse.org/collapse/reference/arithmetic.md),
  [`%r-%`](https://fastverse.org/collapse/reference/arithmetic.md),
  [`%r*%`](https://fastverse.org/collapse/reference/arithmetic.md),
  [`%r/%`](https://fastverse.org/collapse/reference/arithmetic.md) and
  **column-wise**
  [`%cr%`](https://fastverse.org/collapse/reference/arithmetic.md),
  [`%c+%`](https://fastverse.org/collapse/reference/arithmetic.md),
  [`%c-%`](https://fastverse.org/collapse/reference/arithmetic.md),
  [`%c*%`](https://fastverse.org/collapse/reference/arithmetic.md),
  [`%c/%`](https://fastverse.org/collapse/reference/arithmetic.md)
  **replacing and sweeping operations** involving a vector and a matrix
  or data frame / list. Since v1.7, the operators
  [`%+=%`](https://fastverse.org/collapse/reference/efficient-programming.md),
  [`%-=%`](https://fastverse.org/collapse/reference/efficient-programming.md),
  [`%*=%`](https://fastverse.org/collapse/reference/efficient-programming.md)
  and
  [`%/=%`](https://fastverse.org/collapse/reference/efficient-programming.md)
  do column- and element- wise math by reference, and the function
  [`setop`](https://fastverse.org/collapse/reference/efficient-programming.md)
  can also perform sweeping out rows by reference.

- [`(set)TRA`](https://fastverse.org/collapse/reference/TRA.md) is a
  more advanced S3 generic to efficiently perform **(groupwise)
  replacing and sweeping out of statistics**, either by creating a copy
  of the data or by reference. Supported operations are:

  |  |  |  |  |  |
  |----|----|----|----|----|
  | *Integer-id* |  | *String-id* |  | *Description* |
  | 0 |  | "na" or "replace_na" |  | replace only missing values |
  | 1 |  | "fill" or "replace_fill" |  | replace everything |
  | 2 |  | "replace" |  | replace data but preserve missing values |
  | 3 |  | "-" |  | subtract |
  | 4 |  | "-+" |  | subtract group-statistics but add group-frequency weighted average of group statistics |
  | 5 |  | "/" |  | divide |
  | 6 |  | "%" |  | compute percentages |
  | 7 |  | "+" |  | add |
  | 8 |  | "\*" |  | multiply |
  | 9 |  | "%%" |  | modulus |
  | 10 |  | "-%%" |  | subtract modulus |

  All of *collapse*'s [Fast Statistical
  Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
  have a built-in `TRA` argument for faster access (i.e. you can compute
  (groupwise) statistics and use them to transform your data with a
  single function call).

- [`fscale/STD`](https://fastverse.org/collapse/reference/fscale.md) is
  an S3 generic to perform (groupwise and / or weighted) **scaling /
  standardizing** of data and is orders of magnitude faster than
  [`scale`](https://rdrr.io/r/base/scale.html).

- [`fwithin/W`](https://fastverse.org/collapse/reference/fbetween_fwithin.md)
  is an S3 generic to efficiently perform (groupwise and / or weighted)
  **within-transformations / demeaning / centering** of data. Similarly
  [`fbetween/B`](https://fastverse.org/collapse/reference/fbetween_fwithin.md)
  computes (groupwise and / or weighted) **between-transformations /
  averages** (also a lot faster than
  [`ave`](https://rdrr.io/r/stats/ave.html)).

- [`fhdwithin/HDW`](https://fastverse.org/collapse/reference/fhdbetween_fhdwithin.md),
  shorthand for 'higher-dimensional within transform', is an S3 generic
  to efficiently **center data on multiple groups and partial-out linear
  models** (possibly involving many levels of fixed effects and
  interactions). In other words,
  [`fhdwithin/HDW`](https://fastverse.org/collapse/reference/fhdbetween_fhdwithin.md)
  efficiently computes **residuals** from linear models. Similarly
  [`fhdbetween/HDB`](https://fastverse.org/collapse/reference/fhdbetween_fhdwithin.md),
  shorthand for 'higher-dimensional between transformation', computes
  the corresponding means or **fitted values**.

- [`flag/L/F`](https://fastverse.org/collapse/reference/flag.md),
  [`fdiff/D/Dlog`](https://fastverse.org/collapse/reference/fdiff.md)
  and [`fgrowth/G`](https://fastverse.org/collapse/reference/fgrowth.md)
  are S3 generics to compute sequences of **lags / leads** and suitably
  lagged and iterated (quasi-, log-) **differences** and **growth
  rates** on time series and panel data.
  [`fcumsum`](https://fastverse.org/collapse/reference/fcumsum.md)
  flexibly computes (grouped, ordered) cumulative sums. More in [Time
  Series and Panel
  Series](https://fastverse.org/collapse/reference/time-series-panel-series.md).

- `STD, W, B, HDW, HDB, L, D, Dlog` and `G` are parsimonious wrappers
  around the `f-` functions above representing the corresponding
  transformation 'operators'. They have additional capabilities when
  applied to data-frames (i.e. variable selection, formula input,
  auto-renaming and id-variable preservation), and are easier to employ
  in regression formulas, but are otherwise identical in functionality.

## Table of Functions

|  |  |  |  |  |
|----|----|----|----|----|
| *Function / S3 Generic* |  | *Methods* |  | *Description* |
| [`dapply`](https://fastverse.org/collapse/reference/dapply.md) |  | No methods, works with matrices and data frames |  | Apply functions to rows or columns |
| [`BY`](https://fastverse.org/collapse/reference/BY.md) |  | `default, matrix, data.frame, grouped_df` |  | Split-Apply-Combine computing |
| [`%(r/c)(r/+/-/*//)%`](https://fastverse.org/collapse/reference/arithmetic.md) |  | No methods, works with matrices and data frames / lists |  | Row- and column-arithmetic |
| [`(set)TRA`](https://fastverse.org/collapse/reference/TRA.md) |  | `default, matrix, data.frame, grouped_df` |  | Replace and sweep out statistics (by reference) |
| [`fscale/STD`](https://fastverse.org/collapse/reference/fscale.md) |  | `default, matrix, data.frame, pseries, pdata.frame, grouped_df` |  | Scale / standardize data |
| [`fwithin/W`](https://fastverse.org/collapse/reference/fbetween_fwithin.md) |  | `default, matrix, data.frame, pseries, pdata.frame, grouped_df` |  | Demean / center data |
| [`fbetween/B`](https://fastverse.org/collapse/reference/fbetween_fwithin.md) |  | `default, matrix, data.frame, pseries, pdata.frame, grouped_df` |  | Compute means / average data |
| [`fhdwithin/HDW`](https://fastverse.org/collapse/reference/fhdbetween_fhdwithin.md) |  | `default, matrix, data.frame, pseries, pdata.frame` |  | High-dimensional centering and lm residuals |
| [`fhdbetween/HDB`](https://fastverse.org/collapse/reference/fhdbetween_fhdwithin.md) |  | `default, matrix, data.frame, pseries, pdata.frame` |  | High-dimensional averages and lm fitted values |
| [`flag/L/F`](https://fastverse.org/collapse/reference/flag.md), [`fdiff/D/Dlog`](https://fastverse.org/collapse/reference/fdiff.md), [`fgrowth/G`](https://fastverse.org/collapse/reference/fdiff.md), [`fcumsum`](https://fastverse.org/collapse/reference/fcumsum.md) |  | `default, matrix, data.frame, pseries, pdata.frame, grouped_df` |  | (Sequences of) lags / leads, differences, growth rates and cumulative sums |

## See also

[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md),
[Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
[Time Series and Panel
Series](https://fastverse.org/collapse/reference/time-series-panel-series.md)
