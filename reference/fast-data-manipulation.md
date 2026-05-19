# Fast Data Manipulation

*collapse* provides the following functions for fast manipulation of
(mostly) data frames.

- [`fselect`](https://fastverse.org/collapse/reference/select_replace_vars.md)
  is a much faster alternative to
  [`dplyr::select`](https://dplyr.tidyverse.org/reference/select.html)
  to select columns using expressions involving column names.
  [`get_vars`](https://fastverse.org/collapse/reference/select_replace_vars.md)
  is a more versatile and programmer friendly function to efficiently
  select and replace columns by names, indices, logical vectors, regular
  expressions, or using functions to identify columns.

- [`num_vars`](https://fastverse.org/collapse/reference/select_replace_vars.md),
  [`cat_vars`](https://fastverse.org/collapse/reference/select_replace_vars.md),
  [`char_vars`](https://fastverse.org/collapse/reference/select_replace_vars.md),
  [`fact_vars`](https://fastverse.org/collapse/reference/select_replace_vars.md),
  [`logi_vars`](https://fastverse.org/collapse/reference/select_replace_vars.md)
  and
  [`date_vars`](https://fastverse.org/collapse/reference/select_replace_vars.md)
  are convenience functions to efficiently select and replace columns by
  data type.

- [`add_vars`](https://fastverse.org/collapse/reference/select_replace_vars.md)
  efficiently adds new columns at any position within a data frame
  (default at the end). This can be done vie replacement (i.e.
  `add_vars(data) <- newdata`) or returning the appended data, e.g.,
  `add_vars(data, newdata1, newdata2, ...)`. It is thus also an
  efficient alternative to
  [`cbind.data.frame`](https://rdrr.io/r/base/cbind.html).

- [`rowbind`](https://fastverse.org/collapse/reference/rowbind.md)
  efficiently combines data frames / lists row-wise. The implementation
  is derived from
  [`data.table::rbindlist`](https://rdrr.io/pkg/data.table/man/rbindlist.html),
  it is also a fast alternative to
  [`rbind.data.frame`](https://rdrr.io/r/base/cbind.html).

- [`join`](https://fastverse.org/collapse/reference/join.md) provides
  fast, class-agnostic, and verbose table joins.

- [`pivot`](https://fastverse.org/collapse/reference/pivot.md)
  efficiently reshapes data, supporting longer, wider and recast
  pivoting, as well as multi-column-pivots and pivots taking along
  variable labels.

- [`fsubset`](https://fastverse.org/collapse/reference/fsubset.md) is a
  much faster version of [`subset`](https://rdrr.io/r/base/subset.html)
  to efficiently subset vectors, matrices and data frames. If the
  non-standard evaluation offered by
  [`fsubset`](https://fastverse.org/collapse/reference/fsubset.md) is
  not needed, the function
  [`ss`](https://fastverse.org/collapse/reference/fsubset.md) is a much
  faster and more secure alternative to `[.data.frame`.

- [`fslice(v)`](https://fastverse.org/collapse/reference/fslice.md) is a
  much faster alternative to `dplyr::slice_[head|tail|min|max]` for
  filtering/deduplicating matrix-like objects (by groups).

- [`fsummarise`](https://fastverse.org/collapse/reference/fsummarise.md)
  is a much faster version of
  [`dplyr::summarise`](https://dplyr.tidyverse.org/reference/summarise.html),
  especially when used together with the [Fast Statistical
  Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md)
  and [`fgroup_by`](https://fastverse.org/collapse/reference/GRP.md).

- [`fmutate`](https://fastverse.org/collapse/reference/ftransform.md) is
  a much faster version of
  [`dplyr::mutate`](https://dplyr.tidyverse.org/reference/mutate.html),
  especially when used together with the [Fast Statistical
  Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
  the fast [Data Transformation
  Functions](https://fastverse.org/collapse/reference/data-transformations.md),
  and [`fgroup_by`](https://fastverse.org/collapse/reference/GRP.md).

- [`ftransform(v)`](https://fastverse.org/collapse/reference/ftransform.md)
  is a much faster version of
  [`transform`](https://rdrr.io/r/base/transform.html), which also
  supports list input and nested pipelines.
  [`settransform(v)`](https://fastverse.org/collapse/reference/ftransform.md)
  does all of that by reference, i.e. it assigns to the calling
  environment.
  [`fcompute(v)`](https://fastverse.org/collapse/reference/ftransform.md)
  is similar to
  [`ftransform(v)`](https://fastverse.org/collapse/reference/ftransform.md)
  but only returns modified/computed columns.

- [`roworder`](https://fastverse.org/collapse/reference/roworder.md) is
  a fast substitute for
  [`dplyr::arrange`](https://dplyr.tidyverse.org/reference/arrange.html),
  but the syntax is inspired by
  [`data.table::setorder`](https://rdrr.io/pkg/data.table/man/setorder.html).

- [`colorder`](https://fastverse.org/collapse/reference/colorder.md)
  efficiently reorders columns in a data frame, see also
  [`data.table::setcolorder`](https://rdrr.io/pkg/data.table/man/setcolorder.html).

- [`frename`](https://fastverse.org/collapse/reference/frename.md) is a
  fast substitute for
  [`dplyr::rename`](https://dplyr.tidyverse.org/reference/rename.html),
  to efficiently rename various objects.
  [`setrename`](https://fastverse.org/collapse/reference/frename.md)
  renames objects by reference.
  [`relabel`](https://fastverse.org/collapse/reference/frename.md) and
  [`setrelabel`](https://fastverse.org/collapse/reference/frename.md) do
  the same thing for variable labels (see also
  [`vlabels`](https://fastverse.org/collapse/reference/small-helpers.md)).

## Table of Functions

|  |  |  |  |  |
|----|----|----|----|----|
| *Function / S3 Generic* |  | *Methods* |  | *Description* |
| [`fselect(<-)`](https://fastverse.org/collapse/reference/select_replace_vars.md) |  | No methods, for data frames |  | Fast select or replace columns (non-standard evaluation) |
| [`get_vars(<-)`](https://fastverse.org/collapse/reference/select_replace_vars.md), [`num_vars(<-)`](https://fastverse.org/collapse/reference/select_replace_vars.md), [`cat_vars(<-)`](https://fastverse.org/collapse/reference/select_replace_vars.md), [`char_vars(<-)`](https://fastverse.org/collapse/reference/select_replace_vars.md), [`fact_vars(<-)`](https://fastverse.org/collapse/reference/select_replace_vars.md), [`logi_vars(<-)`](https://fastverse.org/collapse/reference/select_replace_vars.md), [`date_vars(<-)`](https://fastverse.org/collapse/reference/select_replace_vars.md) |  | No methods, for data frames |  | Fast select or replace columns |
| [`add_vars(<-)`](https://fastverse.org/collapse/reference/select_replace_vars.md) |  | No methods, for data frames |  | Fast add columns |
| [`rowbind`](https://fastverse.org/collapse/reference/rowbind.md) |  | No methods, for lists of lists/data frames |  | Fast row-binding lists |
| [`join`](https://fastverse.org/collapse/reference/join.md) |  | No methods, for data frames |  | Fast table joins |
| [`pivot`](https://fastverse.org/collapse/reference/pivot.md) |  | No methods, for data frames |  | Fast reshaping |
| [`fsubset`](https://fastverse.org/collapse/reference/fsubset.md) |  | `default, matrix, data.frame, pseries, pdata.frame` |  | Fast subset data (non-standard evaluation) |
| [`ss`](https://fastverse.org/collapse/reference/fsubset.md) |  | No methods, for data frames |  | Fast subset data frames |
| [`fslice(v)`](https://fastverse.org/collapse/reference/fslice.md) |  | No methods, for matrices and data frames |  | Fast slicing of rows |
| [`fsummarise`](https://fastverse.org/collapse/reference/fsummarise.md) |  | No methods, for data frames |  | Fast data aggregation |
| [`fmutate`](https://fastverse.org/collapse/reference/ftransform.md), [`(f/set)transform(v)(<-)`](https://fastverse.org/collapse/reference/ftransform.md) |  | No methods, for data frames |  | Compute, modify or delete columns (non-standard evaluation) |
| [`fcompute(v)`](https://fastverse.org/collapse/reference/ftransform.md) |  | No methods, for data frames |  | Compute or modify columns, returned in a new data frame (non-standard evaluation) |
| [`roworder(v)`](https://fastverse.org/collapse/reference/roworder.md) |  | No methods, for data frames incl. pdata.frame |  | Reorder rows and return data frame (standard and non-standard evaluation) |
| [`colorder(v)`](https://fastverse.org/collapse/reference/colorder.md) |  | No methods, for data frames |  | Reorder columns and return data frame (standard and non-standard evaluation) |
| [`(f/set)rename`](https://fastverse.org/collapse/reference/frename.md), [`(set)relabel`](https://fastverse.org/collapse/reference/frename.md) |  | No methods, for all objects with 'names' attribute |  | Rename and return object / relabel columns in a data frame. |

## See also

[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md),
[Quick Data
Conversion](https://fastverse.org/collapse/reference/quick-conversion.md),
[Recode and Replace
Values](https://fastverse.org/collapse/reference/recode-replace.md)
