# *collapse* Package Options

*collapse* is globally configurable to an extent few packages are: the
default value of key function arguments governing the behavior of its
algorithms, and the exported namespace, can be adjusted interactively
through the `set_collapse()` function.

These options are saved in an internal environment called `.op` (for
safety and performance reasons) visible in the documentation of some
functions such as
[`fmean`](https://fastverse.org/collapse/reference/fmean.md). The
contents of this environment can be accessed using `get_collapse()`.

There are also a few options that can be set using
[`options`](https://rdrr.io/r/base/options.html) (retrievable using
[`getOption`](https://rdrr.io/r/base/options.html)). These options
mainly affect package startup behavior.

## Usage

``` r
set_collapse(...)
get_collapse(opts = NULL)
```

## Arguments

- ...:

  either comma separated options, or a single list of options. The
  available options are:

  |  |  |  |
  |----|----|----|
  | `na.rm` |  | logical, default `TRUE`. Sets the default for statistical algorithms such as the [Fast Statistical Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md) to skip missing values. If your data does not have missing values, or only in rare cases, it is recommended to change this to `FALSE` for performance gains. *Note* that this does not affect other (non-statistical) uses of `na.rm` arguments, such as in [`pivot`](https://fastverse.org/collapse/reference/pivot.md). |
  |  |  |  |
  |  |  |  |
  |  |  |  |
  | `sort` |  | logical, default `TRUE`. Sets the default for grouping operations to be sorted. This also applies to factor generation using [`qF`](https://fastverse.org/collapse/reference/qF.md) and tabulation with [`qtab`](https://fastverse.org/collapse/reference/qtab.md), but excludes other uses of `sort` arguments where grouping is not the objective (such as in [`funique`](https://fastverse.org/collapse/reference/funique.md) or [`pivot`](https://fastverse.org/collapse/reference/pivot.md)). In general, sorted grouping (internally using [`radixorder`](https://fastverse.org/collapse/reference/radixorder.md)) is slower than hash-based direct grouping (internally using [`group`](https://fastverse.org/collapse/reference/group.md)). However, if data is pre-sorted, sorted grouping is slightly faster. In general, if records don't need to be sorted or you want to maintain their first-appearance order, changing this to `FALSE` is recommended and often brings substantial performance gains. *Note* that this also affects internal grouping applied when atomic vectors (except for factors) or lists are passed to `g` arguments in [Fast Statistical Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md). |
  |  |  |  |
  |  |  |  |
  |  |  |  |
  | `nthreads` |  | integer, default 1. Sets the default for OpenMP multithreading, available in certain statistical and data manipulation functions. Setting values greater than 1 is strongly recommended with larger datasets. |
  |  |  |  |
  |  |  |  |
  |  |  |  |
  | `stable.algo` |  | logical, default `TRUE`. Option passed to [`fvar()/fsd()`](https://fastverse.org/collapse/reference/fvar_fsd.md) and [`qsu()`](https://fastverse.org/collapse/reference/qsu.md). `FALSE` enables one-pass standard deviation calculation, which is very fast, but might incur catastrophic cancellation if numbers are large and the variance is small. see [`fvar`](https://fastverse.org/collapse/reference/fvar_fsd.md) for details. |
  |  |  |  |
  |  |  |  |
  |  |  |  |
  | `stub` |  | logical, default `TRUE`. Controls whether [transformation operators](https://fastverse.org/collapse/reference/data-transformations.html) (`.OPERATOR_FUN`) such as [`W`](https://fastverse.org/collapse/reference/fbetween_fwithin.md), [`L`](https://fastverse.org/collapse/reference/flag.md), [`STD`](https://fastverse.org/collapse/reference/fscale.md) etc. add prefixes to transformed columns of matrix and data.frame-like objects. |
  |  |  |  |
  |  |  |  |
  |  |  |  |
  | `verbose` |  | integer, default `1`. Print additional (diagnostic) information or messages when executing code. Currently only used in [`join`](https://fastverse.org/collapse/reference/join.md) and [`roworder`](https://fastverse.org/collapse/reference/roworder.md). |
  |  |  |  |
  |  |  |  |
  |  |  |  |
  | `digits` |  | integer, default `2`. Number of digits to print, e.g. in [`descr`](https://fastverse.org/collapse/reference/descr.md) or [`pwcor`](https://fastverse.org/collapse/reference/pwcor_pwcov_pwnobs.md). |
  |  |  |  |
  |  |  |  |
  |  |  |  |
  | `mask` |  | character, default `NULL`. Allows masking existing base R/dplyr functions with faster *collapse* versions, by creating additional functions in the namespace and instantly exporting them: |
  |  |  |  |
  |  |  | For example `set_collapse(mask = "unique")` (or, equivalently, `set_collapse(mask = "funique")`) will create `unique <- funique` in the *collapse* namespace, export [`unique()`](https://rdrr.io/r/base/unique.html), and silently detach and attach the namespace again so R can find it - all in millisecond. Thus calling [`unique()`](https://rdrr.io/r/base/unique.html) afterwards uses the *collapse* version - which is many times faster. `funique` remains available and you can still call [`base::unique`](https://rdrr.io/r/base/unique.html) explicitly. |
  |  |  |  |
  |  |  | All *collapse* functions starting with 'f' can be passed to the option (with or without the 'f') e.g. `set_collapse(mask = c("subset", "transform", "droplevels"))` creates `subset <- fsubset`, `transform <- ftransform` etc. Special functions are `"n"` and `"table"/"qtab"`, and `"%in%"`, which create `n <- GRPN` (for use in `(f)summarise`/`(f)mutate`), `table <- qtab`, and replace `%in%` with a fast version using [`fmatch`](https://fastverse.org/collapse/reference/fmatch.md), respectively. |
  |  |  |  |
  |  |  | There are also a couple of convenience keywords that you can use to mask groups of functions: |
  |  |  |  |
  |  |  | \- `"manip"` adds data manipulation functions: `fsubset, fslice, fslicev, ftransform, ftransform<-, ftransformv, fcompute, fcomputev, fselect, fselect<-, fgroup_by, fgroup_vars, fungroup, fsummarise, fsummarize, fmutate, frename, findex_by, findex`. |
  |  |  |  |
  |  |  | \- `"helper"` adds the functions: `fdroplevels`, `finteraction`, `fmatch`, `funique`, `fnunique`, `fduplicated`, `fcount`, `fcountv`, `fquantile`, `frange`, `fdist`, `fnlevels`, `fnrow` and `fncol`. |
  |  |  |  |
  |  |  | \- `"special"` exports [`n()`](https://dplyr.tidyverse.org/reference/context.html), [`table()`](https://rdrr.io/r/base/table.html) and `%in%`. See above. |
  |  |  |  |
  |  |  | \- `"fast-fun"` adds the functions contained in the macro: `.FAST_FUN`. See also Note. |
  |  |  |  |
  |  |  | \- `"fast-stat-fun"` adds the functions contained in the macro: `.FAST_STAT_FUN`. See also Note. |
  |  |  |  |
  |  |  | \- `"fast-trfm-fun"` adds the functions contained in: `setdiff(.FAST_FUN, .FAST_STAT_FUN)`. See also Note. |
  |  |  |  |
  |  |  | \- `"all"` turns on all of the above. |
  |  |  |  |
  |  |  | The re-attaching of the namespace places *collapse* at the top of the search path (after the global environment), implying that all its exported functions will take priority over other libraries. Users can use [`fastverse::fastverse_conflicts()`](https://fastverse.github.io/fastverse/reference/fastverse_conflicts.html) to check which functions are masked following `set_collapse(mask = ...)`. The option can be changed at any time with immediate effect. Using `set_collapse(mask = NULL)` removes all masked functions from the namespace, and can also be called simply to place *collapse* at the top of the search path. |
  |  |  |  |
  |  |  |  |
  |  |  |  |
  |  |  |  |
  |  |  |  |
  | `remove` |  | character, default `NULL`. Similar to 'mask': allows removing functions from the exported namespace (they are still in the namespace, just no longer exported). All *collapse* functions can be passed here. This argument is always evaluated after 'mask', thus you can also remove masked functions again i.e. after setting a keyword which masks a bunch of functions. There are also a couple of convenience keywords you can specify to bulk-remove certain functions: |
  |  |  |  |
  |  |  | \- `"shorthand"` removes function shorthands: `gv, gv<-, av, av<-, nv, nv<-, gvr, gvr<-, itn, ix, slt, slt<-, sbt, gby, iby, mtt, smr, tfm, tfmv, tfm<-, settfm, settfmv, rnm`. |
  |  |  |  |
  |  |  | \- `"infix"` removes infix functions: `%!=%, %[!]in%, %[!]iin%, %*=%, %+=%, %-=%, %/=%, %=%, %==%, %c*%, %c+%, %c-%, %c/%, %cr%, %r*%, %r+%, %r-%, %r/%, %rr%`. |
  |  |  |  |
  |  |  | \- `"operator"` removes functions contained in the macro: `.OPERATOR_FUN`. |
  |  |  |  |
  |  |  | \- `"old"` removes depreciated functions contained in the macro: `.COLLAPSE_OLD`. |
  |  |  |  |
  |  |  | Like 'mask', the option is alterable and reversible. Specifying `set_collapse(remove = NULL)` restores the exported namespace. Also like 'mask', this option silently detaches and attaches *collapse* again, ensuring that it is at the top of the search path. |

- opts:

  character. A vector of options to receive from `.op`, or `NULL` for a
  list of all options.

## Value

`set_collapse()` returns the old content of `.op` invisibly as a list.
`get_collapse()`, if called with only one option, returns the value of
the option, and otherwise a list.

## Note

Setting keywords "fast-fun", "fast-stat-fun", "fast-trfm-fun" or "all"
with `set_collapse(mask = ...)` will also adjust internal optimization
flags, e.g. in
[`(f)summarise`](https://fastverse.org/collapse/reference/fsummarise.md)
and
[`(f)mutate`](https://fastverse.org/collapse/reference/ftransform.md),
so that these functions - and all expressions containing them - receive
vectorized execution (see examples of
[`(f)summarise`](https://fastverse.org/collapse/reference/fsummarise.md)
and
[`(f)mutate`](https://fastverse.org/collapse/reference/ftransform.md)).
Users should be aware of expressions like
`fmutate(mu = sum(var) / lenth(var))`: this usually gets executed by
groups, but with these keywords set,this will be vectorized (like
`fmutate(mu = fsum(var) / lenth(var))`) implying grouped sum divided by
overall length. In this case `fmutate(mu = base::sum(var) / lenth(var))`
needs to be specified to retain the original result.

*Note* that passing individual functions like
`set_collapse(mask = "(f)sum")` will **not** change internal
optimization flags for these functions. This is to ensure consistency
i.e. you can be either all in (by setting appropriate keywords) or all
out when it comes to vectorized stats with basic R names.

*Note* also that masking does not change documentation links, so you
need to look up the f- version of a function to get the right
documentation.

A safe way to set options affecting startup behavior is by using a
[`.Rprofile`](https://rdrr.io/r/base/Startup.html) file in your user or
project directory (see also
[here](https://www.datacamp.com/doc/r/customizing), the user-level file
is located at `file.path(Sys.getenv("HOME"), ".Rprofile")` and can be
edited using `file.edit(Sys.getenv("HOME"), ".Rprofile")`), or by using
a
[`.fastverse`](https://fastverse.org/fastverse/articles/fastverse_intro.html#custom-fastverse-configurations-for-projects)
configuration file in the project directory.

`options("collapse_remove")` does in fact remove functions from the
namespace and cannot be reversed by `set_collapse(remove = NULL)` once
the package is loaded. It is only reversed by re-loading *collapse*.

## Options Set Using [`options()`](https://rdrr.io/r/base/options.html)

- `"collapse_unused_arg_action"` regulates how generic functions (such
  as the [Fast Statistical
  Functions](https://fastverse.org/collapse/reference/fast-statistical-functions.md))
  in the package react when an unknown argument is passed to a method.
  The default action is `"warning"` which issues a warning. Other
  options are `"error"`, `"message"` or `"none"`, whereby the latter
  enables silent swallowing of such arguments.

- `"collapse_export_F"`, if set to `TRUE`, exports the lead operator `F`
  in the package namespace when loading the package. The operator was
  exported by default until v1.9.0, but is now hidden inside the package
  due to too many problems with
  [`base::F`](https://rdrr.io/r/base/logical.html). Alternatively, the
  operator can be accessed using `collapse:::F`.

- `"collapse_nthreads"`, `"collapse_na_rm"`, `"collapse_sort"`,
  `"collapse_stable_algo"`, `"collapse_verbose"`, `"collapse_digits"`,
  `"collapse_mask"` and `"collapse_remove"` can be set before loading
  the package to initialize `.op` with different defaults (e.g. using an
  [`.Rprofile`](https://rdrr.io/r/base/Startup.html) file). Once loaded,
  these options have no effect, and users need to use `set_collapse()`
  to change them. See also the Note.

## References

Krantz S (2026). *collapse*: Advanced and Fast Statistical Computing and
Data Transformation in R. *Journal of Statistical Software* **116**(1),
1–38. [doi:10.18637/jss.v116.i01](https://doi.org/10.18637/jss.v116.i01)

## See also

[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md),
[collapse-package](https://fastverse.org/collapse/reference/collapse-package.md)

## Examples

``` r
# Setting new values
oldopts <- set_collapse(nthreads = 2, na.rm = FALSE)

# Getting the values
get_collapse()
#> $nthreads
#> [1] 2
#> 
#> $remove
#> NULL
#> 
#> $stable.algo
#> [1] TRUE
#> 
#> $sort
#> [1] TRUE
#> 
#> $digits
#> [1] 2
#> 
#> $stub
#> [1] TRUE
#> 
#> $verbose
#> [1] 1
#> 
#> $mask
#> NULL
#> 
#> $na.rm
#> [1] FALSE
#> 
get_collapse("nthreads")
#> [1] 2

# Resetting
set_collapse(oldopts)
rm(oldopts)

if (FALSE) { # \dontrun{
## This is a typical working setup I use:
library(fastverse)
# Loading other stats packages with fastverse_extend():
# displays versions, checks conflicts, and installs if unavailable
fastverse_extend(qs, fixest, grf, glmnet, install = TRUE)
# Now setting collapse options with some namespace modification
set_collapse(
  nthreads = 4,
  sort = FALSE,
  mask = c("manip", "helper", "special", "mean", "scale"),
  remove = "old"
)
# Final conflicts check (optional)
fastverse_conflicts()

# For some simpler scripts I also use
set_collapse(
  nthreads = 4,
  sort = FALSE,
  mask = "all",
  remove = c("old", "between") # I use data.table::between > fbetween
)

# This is now collapse code
mtcars |>
  subset(mpg > 12) |>
  group_by(cyl) |>
  sum()
} # }

## Changing what happens with unused arguments
oldopts <- options(collapse_unused_arg_action = "message") # default: "warning"
fmean(mtcars$mpg, bla = 1)
#> Unused argument (bla = 1) passed to fmean.default
#> [1] 20.09063

# Now nothing happens, same as base R
options(collapse_unused_arg_action = "none")
fmean(mtcars$mpg, bla = 1)
#> [1] 20.09063
mean(mtcars$mpg, bla = 1)
#> [1] 20.09062

options(oldopts)
rm(oldopts)
```
