# Fast and Verbose Table Joins

Join two data frame like objects `x` and `y` `on` columns. Inspired by
*polars* and by default uses a vectorized hash join algorithm (workhorse
function
[`fmatch`](https://fastverse.org/collapse/reference/fmatch.md)), with
several verbose options.

## Usage

``` r
join(x, y,
     on = NULL,
     how = "left",
     suffix = NULL,
     validate = "m:m",
     multiple = FALSE,
     sort = FALSE,
     keep.col.order = TRUE,
     drop.dup.cols = FALSE,
     verbose = .op[["verbose"]],
     require = NULL,
     column = NULL,
     attr = NULL,
     ...
)
```

## Arguments

- x:

  a data frame-like object. The result will inherit the attributes of
  this object.

- y:

  a data frame-like object to join with `x`.

- on:

  character. vector of columns to join on. `NULL` uses
  `intersect(names(x), names(y))`. Use a named vector to match columns
  named differently in `x` and `y`, e.g. `c("x_id" = "y_id")`.

- how:

  character. Join type: `"left"`, `"right"`, `"inner"`, `"full"`,
  `"semi"` or `"anti"`. The first letter suffices.

- suffix:

  character(1 or 2). Suffix to add to duplicate column names. `NULL`
  renames duplicate `y` columns as `paste(col, y_name, sep = "_")`,
  where `y_name = as.character(substitute(y))` i.e. the name of the data
  frame as passed into the function. In general, passing `suffix` length
  1 will only rename `y`, whereas a length 2 suffix will rename both `x`
  and `y`, respectively. If `verbose > 0` a message will be printed.

- validate:

  character. (Optional) check if join is of specified type. One of
  `"1:1"`, `"1:m"`, `"m:1"` or `"m:m"`. The default `"m:m"` does not
  perform any checks. Checks are done before the actual join step and
  failure results in an error. *Note* that this argument does not affect
  the result, it only triggers a check.

- multiple:

  logical. Handling of rows in `x` with multiple matches in `y`. The
  default `FALSE` takes the first match in `y`. `TRUE` returns every
  match in `y` (a full cartesian product), increasing the size of the
  joined table.

- sort:

  logical. `TRUE` implements a sort-merge-join: a completely separate
  join algorithm that sorts both datasets on the join columns using
  [`radixorder`](https://fastverse.org/collapse/reference/radixorder.md)
  and then matches the rows without hashing. *Note* that in this case
  the result will be sorted by the join columns, whereas `sort = FALSE`
  preserves the order of rows in `x`.

- keep.col.order:

  logical. Keep order of columns in `x`? `FALSE` places the `on` columns
  in front.

- drop.dup.cols:

  instead of renaming duplicate columns in `x` and `y` using `suffix`,
  this option simply drops them: `TRUE` or `"y"` drops them from `y`,
  `"x"` from `x`.

- verbose:

  integer. Prints information about the join. One of 0 (off), 1
  (default, see Details) or 2 (additionally prints the classes of the
  `on` columns). *Note:* `verbose > 0` or `validate != "m:m"` invoke the
  `count` argument to
  [`fmatch`](https://fastverse.org/collapse/reference/fmatch.md), so
  `verbose = 0` is slightly more efficient.

- require:

  (optional) named list of the form
  `list(x = 1, y = 0.5, fail = "warning")` (or `fail.with` if you want
  to be more expressive) giving proportions of records that need to be
  matched and the action if any requirement fails (`"message"`,
  `"warning"`, or `"error"`). Any elements of the list can be omitted,
  the default action is `"error"`.

- column:

  (optional) name for an extra column to generate in the output
  indicating which dataset a record came from. `TRUE` calls this column
  `".join"` (inspired by STATA's '\_merge' column). By default this
  column is generated as the last column, but, if
  `keep.col.order = FALSE`, it is placed after the 'on' columns. The
  column is a factor variable with levels corresponding to the dataset
  names (inferred from the input) or `"matched"` for matched records.
  Alternatively, it is possible to specify a list of 2, where the first
  element is the column name, and the second a length 3 (!) vector of
  levels e.g. `column = list("joined", c("x", "y", "x_y"))`, where
  `"x_y"` replaces `"matched"`. The column has an additional attribute
  `"on.cols"` giving the join columns corresponding to the factor
  levels. See Examples.

- attr:

  (optional) name for attribute providing information about the join
  performed (including the output of
  [`fmatch`](https://fastverse.org/collapse/reference/fmatch.md)) to the
  result. `TRUE` calls this attribute `"join.match"`. *Note:* this also
  invokes the `count` argument to
  [`fmatch`](https://fastverse.org/collapse/reference/fmatch.md).

- ...:

  further arguments to
  [`fmatch`](https://fastverse.org/collapse/reference/fmatch.md) (if
  `sort = FALSE`). Notably, `overid` can bet set to 0 or 2 (default 1)
  to control the matching process if the join condition more than
  identifies the records.

## Details

If `verbose > 0`, `join` prints a compact summary of the join operation
using [`cat`](https://rdrr.io/r/base/cat.html). If the names of `x` and
`y` can be extracted (if `as.character(substitute(x))` yields a single
string) they will be displayed (otherwise 'x' and 'y' are used) followed
by the respective join keys in brackets. This is followed by a summary
of the records used from each table. If `multiple = FALSE`, only the
first matches from `y` are used and counted here (or the first matches
of `x` if `how = "right"`). *Note* that if `how = "full"` any further
matches are simply appended to the results table, thus it may make more
sense to use `multiple = TRUE` with the full join when suspecting
multiple matches.

If `multiple = TRUE`, `join` performs a full cartesian product matching
every key in `x` to every matching key in `y`. This can considerably
increase the size of the resulting table. No memory checks are performed
(your system will simply run out of memory; usually this should not
terminate R).

In both cases, `join` will also determine the average order of the join
as the number of records used from each table divided by the number of
unique matches and display it between the two tables at up to 2 digits.
For example `"<4:1.5>"` means that on average 4 records from `x` match
1.5 records from `y`, implying on average `4*1.5 = 6` records generated
per unique match. If `multiple = FALSE` `"1st"` will be displayed for
the using table (`y` unless `how = "right"`), indicating that there
could be multiple matches but only the first is retained. *Note* that an
order of '1' on either table must not imply that the key is unique as
this value is generated from `round(v, 2)`. To be sure about a keys
uniqueness employ the `validate` argument.

## Value

A data frame-like object of the same type and attributes as `x`.
`"row.names"` of `x` are only preserved in left-join operations.

## See also

[`fmatch`](https://fastverse.org/collapse/reference/fmatch.md),
[`pivot`](https://fastverse.org/collapse/reference/pivot.md), [Data
Frame
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Fast Grouping and
Ordering](https://fastverse.org/collapse/reference/fast-grouping-ordering.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
df1 <- data.frame(
  id1 = c(1, 1, 2, 3),
  id2 = c("a", "b", "b", "c"),
  name = c("John", "Jane", "Bob", "Carl"),
  age = c(35, 28, 42, 50)
)
df2 <- data.frame(
  id1 = c(1, 2, 3, 3),
  id2 = c("a", "b", "c", "e"),
  salary = c(60000, 55000, 70000, 80000),
  dept = c("IT", "Marketing", "Sales", "IT")
)

# Different types of joins
for(i in c("l","i","r","f","s","a"))
    join(df1, df2, how = i) |> print()
#> left join: df1[id1, id2] 3/4 (75%) <1:1st> df2[id1, id2] 3/4 (75%)
#>   id1 id2 name age salary      dept
#> 1   1   a John  35  60000        IT
#> 2   1   b Jane  28     NA      <NA>
#> 3   2   b  Bob  42  55000 Marketing
#> 4   3   c Carl  50  70000     Sales
#> inner join: df1[id1, id2] 3/4 (75%) <1:1st> df2[id1, id2] 3/4 (75%)
#>   id1 id2 name age salary      dept
#> 1   1   a John  35  60000        IT
#> 2   2   b  Bob  42  55000 Marketing
#> 3   3   c Carl  50  70000     Sales
#> right join: df1[id1, id2] 3/4 (75%) <1st:1> df2[id1, id2] 3/4 (75%)
#>   id1 id2 name age salary      dept
#> 1   1   a John  35  60000        IT
#> 2   2   b  Bob  42  55000 Marketing
#> 3   3   c Carl  50  70000     Sales
#> 4   3   e <NA>  NA  80000        IT
#> full join: df1[id1, id2] 3/4 (75%) <1:1st> df2[id1, id2] 3/4 (75%)
#>   id1 id2 name age salary      dept
#> 1   1   a John  35  60000        IT
#> 2   1   b Jane  28     NA      <NA>
#> 3   2   b  Bob  42  55000 Marketing
#> 4   3   c Carl  50  70000     Sales
#> 5   3   e <NA>  NA  80000        IT
#> semi join: df1[id1, id2] 3/4 (75%) <1:1st> df2[id1, id2] 3/4 (75%)
#>   id1 id2 name age
#> 1   1   a John  35
#> 2   2   b  Bob  42
#> 3   3   c Carl  50
#> anti join: df1[id1, id2] 3/4 (75%) <1:1st> df2[id1, id2] 3/4 (75%)
#>   id1 id2 name age
#> 1   1   b Jane  28

# With multiple matches
for(i in c("l","i","r","f","s","a"))
    join(df1, df2, on = "id2", how = i, multiple = TRUE) |> print()
#> left join: df1[id2] 4/4 (100%) <1.33:1> df2[id2] 3/4 (75%)
#> duplicate columns: id1 => renamed using suffix '_df2' for y
#>   id1 id2 name age id1_df2 salary      dept
#> 1   1   a John  35       1  60000        IT
#> 2   1   b Jane  28       2  55000 Marketing
#> 3   2   b  Bob  42       2  55000 Marketing
#> 4   3   c Carl  50       3  70000     Sales
#> inner join: df1[id2] 4/4 (100%) <1.33:1> df2[id2] 3/4 (75%)
#> duplicate columns: id1 => renamed using suffix '_df2' for y
#>   id1 id2 name age id1_df2 salary      dept
#> 1   1   a John  35       1  60000        IT
#> 2   1   b Jane  28       2  55000 Marketing
#> 3   2   b  Bob  42       2  55000 Marketing
#> 4   3   c Carl  50       3  70000     Sales
#> right join: df1[id2] 4/4 (100%) <1.33:1> df2[id2] 3/4 (75%)
#> duplicate columns: id1 => renamed using suffix '_df2' for y
#>   id1 id2 name age id1_df2 salary      dept
#> 1   1   a John  35       1  60000        IT
#> 2   1   b Jane  28       2  55000 Marketing
#> 3   2   b  Bob  42       2  55000 Marketing
#> 4   3   c Carl  50       3  70000     Sales
#> 5  NA   e <NA>  NA       3  80000        IT
#> full join: df1[id2] 4/4 (100%) <1.33:1> df2[id2] 3/4 (75%)
#> duplicate columns: id1 => renamed using suffix '_df2' for y
#>   id1 id2 name age id1_df2 salary      dept
#> 1   1   a John  35       1  60000        IT
#> 2   1   b Jane  28       2  55000 Marketing
#> 3   2   b  Bob  42       2  55000 Marketing
#> 4   3   c Carl  50       3  70000     Sales
#> 5  NA   e <NA>  NA       3  80000        IT
#> semi join: df1[id2] 4/4 (100%) <1.33:1> df2[id2] 3/4 (75%)
#>   id1 id2 name age
#> 1   1   a John  35
#> 2   1   b Jane  28
#> 3   2   b  Bob  42
#> 4   3   c Carl  50
#> anti join: df1[id2] 4/4 (100%) <1.33:1> df2[id2] 3/4 (75%)
#> [1] id1  id2  name age 
#> <0 rows> (or 0-length row.names)

# Adding join column: useful esp. for full join
join(df1, df2, how = "f", column = TRUE)
#> full join: df1[id1, id2] 3/4 (75%) <1:1st> df2[id1, id2] 3/4 (75%)
#>   id1 id2 name age salary      dept   .join
#> 1   1   a John  35  60000        IT matched
#> 2   1   b Jane  28     NA      <NA>     df1
#> 3   2   b  Bob  42  55000 Marketing matched
#> 4   3   c Carl  50  70000     Sales matched
#> 5   3   e <NA>  NA  80000        IT     df2
# Custom column + rearranging
join(df1, df2, how = "f", column = list("join", c("x", "y", "x_y")), keep = FALSE)
#> full join: df1[id1, id2] 3/4 (75%) <1:1st> df2[id1, id2] 3/4 (75%)
#>   id1 id2 join name age salary      dept
#> 1   1   a  x_y John  35  60000        IT
#> 2   1   b    x Jane  28     NA      <NA>
#> 3   2   b  x_y  Bob  42  55000 Marketing
#> 4   3   c  x_y Carl  50  70000     Sales
#> 5   3   e    y <NA>  NA  80000        IT

# Attaching match attribute
str(join(df1, df2, attr = TRUE))
#> left join: df1[id1, id2] 3/4 (75%) <1:1st> df2[id1, id2] 3/4 (75%)
#> 'data.frame':    4 obs. of  6 variables:
#>  $ id1   : num  1 1 2 3
#>  $ id2   : chr  "a" "b" "b" "c"
#>  $ name  : chr  "John" "Jane" "Bob" "Carl"
#>  $ age   : num  35 28 42 50
#>  $ salary: num  60000 NA 55000 70000
#>  $ dept  : chr  "IT" NA "Marketing" "Sales"
#>  - attr(*, "join.match")=List of 3
#>   ..$ call   : language join(x = df1, y = df2, attr = TRUE)
#>   ..$ on.cols:List of 2
#>   .. ..$ x: chr [1:2] "id1" "id2"
#>   .. ..$ y: chr [1:2] "id1" "id2"
#>   ..$ match  : 'qG' int [1:4] 1 NA 2 3
#>   .. ..- attr(*, "N.nomatch")= int 1
#>   .. ..- attr(*, "N.groups")= int 4
#>   .. ..- attr(*, "N.distinct")= int 3
```
