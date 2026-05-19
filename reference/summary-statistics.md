# Summary Statistics

*collapse* provides the following functions to efficiently summarize and
examine data:

- [`qsu`](https://fastverse.org/collapse/reference/qsu.md), shorthand
  for quick-summary, is an extremely fast summary command inspired by
  the (xt)summarize command in the STATA statistical software. It
  computes a set of 7 statistics (nobs, mean, sd, min, max, skewness and
  kurtosis) using a numerically stable one-pass method. Statistics can
  be computed weighted, by groups, and also within-and between entities
  (for multilevel / panel data).

- [`qtab`](https://fastverse.org/collapse/reference/qtab.md), shorthand
  for quick-table, is a faster and more versatile alternative to
  [`table`](https://rdrr.io/r/base/table.html). Notably, it also
  supports tabulations with frequency weights, as well as computing a
  statistic over combinations of variables. 'qtab's inherit the 'table'
  class, allowing for seamless application of 'table' methods.

- [`descr`](https://fastverse.org/collapse/reference/descr.md) computes
  a concise and detailed description of a data frame, including (sorted)
  frequency tables for categorical variables and various statistics and
  quantiles for numeric variables. It is inspired by `Hmisc::describe`,
  but about 10x faster.

- [`pwcor`](https://fastverse.org/collapse/reference/pwcor_pwcov_pwnobs.md),
  [`pwcov`](https://fastverse.org/collapse/reference/pwcor_pwcov_pwnobs.md)
  and
  [`pwnobs`](https://fastverse.org/collapse/reference/pwcor_pwcov_pwnobs.md)
  compute (weighted) pairwise correlations, covariances and observation
  counts on matrices and data frames. Pairwise correlations and
  covariances can be computed together with observation counts and
  p-values. The elaborate print method displays all of these statistics
  in a single correlation table.

- [`varying`](https://fastverse.org/collapse/reference/varying.md) very
  efficiently checks for the presence of any variation in data
  (optionally) within groups (such as panel-identifiers). A variable is
  variant if it has at least 2 distinct non-missing data points.

## Table of Functions

|  |  |  |  |  |
|----|----|----|----|----|
| *Function / S3 Generic* |  | *Methods* |  | *Description* |
| [`qsu`](https://fastverse.org/collapse/reference/qsu.md) |  | `default, matrix, data.frame, grouped_df, pseries, pdata.frame, sf` |  | Fast (grouped, weighted, panel-decomposed) summary statistics |
| [`qtab`](https://fastverse.org/collapse/reference/qtab.md) |  | No methods, for data frames or vectors |  | Fast (weighted) cross tabulation |
| [`descr`](https://fastverse.org/collapse/reference/descr.md) |  | `default, grouped_df` (default method handles most objects) |  | Detailed statistical description of data frame |
| [`pwcor`](https://fastverse.org/collapse/reference/pwcor_pwcov_pwnobs.md) |  | No methods, for matrices or data frames |  | Pairwise (weighted) correlations |
| [`pwcov`](https://fastverse.org/collapse/reference/pwcor_pwcov_pwnobs.md) |  | No methods, for matrices or data frames |  | Pairwise (weighted) covariances |
| [`pwnobs`](https://fastverse.org/collapse/reference/pwcor_pwcov_pwnobs.md) |  | No methods, for matrices or data frames |  | Pairwise observation counts |
| [`varying`](https://fastverse.org/collapse/reference/varying.md) |  | `default, matrix, data.frame, pseries, pdata.frame, grouped_df` |  | Fast variation check |

## See also

[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md),
[Fast Statistical
Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
