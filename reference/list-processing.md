# List Processing

*collapse* provides the following set of functions to efficiently work
with lists of R objects:

- **Search and Identification**

  - [`is_unlistable`](https://fastverse.org/collapse/reference/is_unlistable.md)
    checks whether a (nested) list is composed of atomic objects in all
    final nodes, and thus unlistable to an atomic vector using
    [`unlist`](https://rdrr.io/r/base/unlist.html).

  - [`ldepth`](https://fastverse.org/collapse/reference/ldepth.md)
    determines the level of nesting of the list (i.e. the maximum number
    of nodes of the list-tree).

  - [`has_elem`](https://fastverse.org/collapse/reference/extract_list.md)
    searches elements in a list using element names, regular expressions
    applied to element names, or a function applied to the elements, and
    returns `TRUE` if any matches were found.

- **Subsetting**

  - [`atomic_elem`](https://fastverse.org/collapse/reference/extract_list.md)
    examines the top-level of a list and returns a sublist with the
    atomic elements. Conversely
    [`list_elem`](https://fastverse.org/collapse/reference/extract_list.md)
    returns the sublist of elements which are themselves lists or
    list-like objects.

  - [`reg_elem`](https://fastverse.org/collapse/reference/extract_list.md)
    and
    [`irreg_elem`](https://fastverse.org/collapse/reference/extract_list.md)
    are recursive versions of the former.
    [`reg_elem`](https://fastverse.org/collapse/reference/extract_list.md)
    extracts the 'regular' part of the list-tree leading to atomic
    elements in the final nodes, while
    [`irreg_elem`](https://fastverse.org/collapse/reference/extract_list.md)
    extracts the 'irregular' part of the list tree leading to non-atomic
    elements in the final nodes. (*Tip*: try calling both on an `lm`
    object). Naturally for all lists `l`, `is_unlistable(reg_elem(l))`
    evaluates to `TRUE`.

  - [`get_elem`](https://fastverse.org/collapse/reference/extract_list.md)
    extracts elements from a list using element names, regular
    expressions applied to element names, a function applied to the
    elements, or element-indices used to subset the lowest-level
    sub-lists. by default the result is presented as a simplified list
    containing all matching elements. With the `keep.tree` option
    however
    [`get_elem`](https://fastverse.org/collapse/reference/extract_list.md)
    can also be used to subset lists i.e. maintain the full tree but cut
    off non-matching branches.

- **Splitting and Transposition**

  - [`rsplit`](https://fastverse.org/collapse/reference/rsplit.md)
    recursively splits a vector or data frame into subsets according to
    combinations of (multiple) vectors / factors - by default returning
    a (nested) list. If `flatten = TRUE`, the list is flattened yielding
    the same result as
    [`split`](https://rdrr.io/pkg/data.table/man/split.html). `rsplit`
    is also faster than
    [`split`](https://rdrr.io/pkg/data.table/man/split.html),
    particularly for data frames.

  - [`t_list`](https://fastverse.org/collapse/reference/t_list.md)
    efficiently transposes nested lists of lists, such as those obtained
    from splitting a data frame by multiple variables using
    [`rsplit`](https://fastverse.org/collapse/reference/rsplit.md).

- **Apply Functions**

  - [`rapply2d`](https://fastverse.org/collapse/reference/rapply2d.md)
    is a recursive version of
    [`lapply`](https://rdrr.io/r/base/lapply.html) with two key
    differences to [`rapply`](https://rdrr.io/r/base/rapply.html) to
    apply a function to nested lists of data frames or other list-based
    objects.

- **Unlisting / Row-Binding**

  - [`unlist2d`](https://fastverse.org/collapse/reference/unlist2d.md)
    efficiently unlists unlistable lists in 2-dimensions and creates a
    data frame (or *data.table*) representation of the list. This is
    done by recursively flattening and row-binding R objects in the list
    while creating identifier columns for each level of the list-tree
    and (optionally) saving the row-names of the objects in a separate
    column.
    [`unlist2d`](https://fastverse.org/collapse/reference/unlist2d.md)
    can thus also be understood as a recursive generalization of
    `do.call(rbind, l)`, for lists of vectors, data frames, arrays or
    heterogeneous objects. A simpler version for non-recursive
    row-binding lists of lists / data.frames, is also available by
    [`rowbind`](https://fastverse.org/collapse/reference/rowbind.md).

## Table of Functions

|  |  |  |
|----|----|----|
| *Function* |  | *Description* |
| [`is_unlistable`](https://fastverse.org/collapse/reference/is_unlistable.md) |  | Checks if list is unlistable |
| [`ldepth`](https://fastverse.org/collapse/reference/ldepth.md) |  | Level of nesting / maximum depth of list-tree |
| [`has_elem`](https://fastverse.org/collapse/reference/extract_list.md) |  | Checks if list contains a certain element |
| [`get_elem`](https://fastverse.org/collapse/reference/extract_list.md) |  | Subset list / extract certain elements |
| [`atomic_elem`](https://fastverse.org/collapse/reference/extract_list.md) |  | Top-level subset atomic elements |
| [`list_elem`](https://fastverse.org/collapse/reference/extract_list.md) |  | Top-level subset list/list-like elements |
| [`reg_elem`](https://fastverse.org/collapse/reference/extract_list.md) |  | Recursive version of `atomic_elem`: Subset / extract 'regular' part of list |
| [`irreg_elem`](https://fastverse.org/collapse/reference/extract_list.md) |  | Subset / extract non-regular part of list |
| [`rsplit`](https://fastverse.org/collapse/reference/rsplit.md) |  | Recursively split vectors or data frames / lists |
| [`t_list`](https://fastverse.org/collapse/reference/t_list.md) |  | Transpose lists of lists |
| [`rapply2d`](https://fastverse.org/collapse/reference/rapply2d.md) |  | Recursively apply functions to lists of data objects |
| [`unlist2d`](https://fastverse.org/collapse/reference/unlist2d.md) |  | Recursively unlist/row-bind lists of data objects in 2D, to data frame or *data.table* |
| [`rowbind`](https://fastverse.org/collapse/reference/rowbind.md) |  | Non-recursive binding of lists of lists / data.frames. |

## See also

[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)
