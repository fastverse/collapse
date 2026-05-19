# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working
with code in this repository.

## Overview

**collapse** is a high-performance C/C++-based R package for advanced
data transformation and statistical computing. The package provides: -
Fast grouped and weighted statistical functions with OpenMP
multithreading - Class-agnostic architecture supporting base R, tibble,
data.table, sf, plm, and other extensions - ~13,775 lines of R code and
~28,625 lines of C/C++ code - Comprehensive test suite with 48 test
files

## Build and Test Commands

### Development Workflow

``` r

# Load package for development
devtools::load_all()

# Run all tests
devtools::test()

# Run specific test file
testthat::test_file("tests/testthat/test-GRP.R")

# Run package check
devtools::check()

# Build documentation
devtools::document()
```

### Command Line Build

``` bash
# Build package tarball
R CMD build .

# Check package (comprehensive tests)
R CMD check collapse_*.tar.gz

# Install from source
R CMD INSTALL collapse_*.tar.gz

# Run tests directly
Rscript -e "testthat::test_dir('tests/testthat')"
```

### CI/CD

- GitHub Actions runs R-CMD-check on macOS, Windows, and Ubuntu
- Tests against R-devel, R-release, and R-oldrel-1
- Code coverage tracked via codecov
- Documentation auto-deployed via pkgdown

## Architecture

### Code Organization

**R/ directory (52 files)** - `f*` prefix: Fast statistical functions
(fmean.R, fmedian.R, fvar_fsd.R, etc.) - `GRP.R` (1,428 lines): Core
grouping object using radix sort or hash methods -
`fsubset_ftransform_fmutate.R` (1,220 lines): Data manipulation (subset,
transform, mutate) - `collap.R` (793 lines): Advanced aggregation
framework - `pivot.R` (823 lines): Pivot operations
(wider/longer/recast) - `join.R` (473 lines): Fast join operations -
`fbetween_fwithin.R`, `fhdbetween_fhdwithin.R`: Between/within group
transformations - `fdiff_fgrowth.R`, `flag.R`: Time series operations
(differences, growth rates, lags/leads) - `qsu.R`, `descr.R`:
Statistical summaries - `list_functions.R` (647 lines): Recursive list
operations - `indexing.R` (741 lines): Time series and panel data
indexing - `zzz.R` (195 lines): Package initialization, namespace
masking system

**src/ directory (94 files)** - `collapse_c.h` (195 lines): Main C
header with all function signatures and OpenMP macros -
`base_radixsort.c` (2,158 lines): Fast radix sorting (adapted from base
R) - `fdiff_fgrowth.cpp` (1,854 lines): Differencing and growth rate
calculations - `fvar_fsd.cpp` (1,689 lines): Variance and standard
deviation - `fnth_fmedian_fquantile.c` (1,627 lines): Quantile
functions - `TRA.c` (1,416 lines): Transformation operations framework -
`programming.c` (1,284 lines): Programming helper functions - `fmode.c`
(1,248 lines): Mode calculation - `kit_dup.c` (1,169 lines): Duplicate
handling (adapted from kit package) - `match.c` (1,161 lines): Fast
matching operations - `flag.cpp` (1,083 lines): Lag/lead operations -
`data.table_*.c`: Integration with data.table internals (rbindlist,
subset, utils) - `Makevars`, `Makevars.win`: OpenMP compilation
configuration

**tests/testthat/ (48 files)** - `test-<function>.R` pattern for each
major function - Comprehensive tests against base R equivalents - Tests
with NA/Inf values, different data types, grouping, and weights

### Key Architectural Patterns

**1. Two-Layer Design** - R layer: Input validation, S3 method dispatch,
attribute handling - C/C++ layer: Core computation with OpenMP
parallelization - Pattern:
[`.Call()`](https://rdrr.io/r/base/CallExternal.html) invokes C
functions named with `_C`, `_mC` (matrix), `_lC` (list/data.frame)
suffixes

**2. Function Signature Pattern** Most statistical functions follow:

``` r
f<name>(x, g = NULL, w = NULL, TRA = NULL, na.rm = TRUE, use.g.names = TRUE, ...)
```

- `x`: data (vector, matrix, or data.frame)
- `g`: grouping variable(s) - converted to GRP object internally
- `w`: weights vector
- `TRA`: transformation operation (see TRA framework below)
- S3 methods for: default, matrix, data.frame, grouped_df, zoo, units,
  pdata.frame, pseries, sf

**3. GRP (Grouping) Object** Central to all grouped operations: -
Created via [`GRP()`](https://fastverse.org/collapse/reference/GRP.md)
or automatically from factors/lists - Contains: `N.groups`,
`group.sizes`, `groups` (names), `group.id`, `group.starts` - Two
algorithms: radix sort (default, stable) or hash-based (faster for many
groups) - Access via
[`GRP()`](https://fastverse.org/collapse/reference/GRP.md),
[`fgroup_by()`](https://fastverse.org/collapse/reference/GRP.md), or
automatic conversion from factors

**4. TRA (Transformation) Framework** 10 transformation operations
applicable to all statistical functions: - `"-"`: center (subtract
statistic) - `"+"`: add statistic - `"*"`: multiply by statistic -
`"/"`: divide by statistic (scale) - `"%"`: compute percentage of
statistic - `"%%"`: modulus - `"-+"`: center and add overall mean -
`"+-"`: add statistic and subtract overall mean - `"replace"`: replace
values with statistic - `"replace_fill"`: replace and fill with
statistic

Example: `fmean(x, g, TRA = "-")` centers x by group means

**5. Namespace Masking System** - Functions can be exported without ‘f’
prefix via `options(collapse_mask = "all")` - Controlled via
`.fastverse` config file or `set_collapse(mask = "manip")` - Allows
[`mean()`](https://rdrr.io/r/base/mean.html),
[`sum()`](https://rdrr.io/r/base/sum.html), etc. to use collapse’s fast
versions - Keywords: “all”, “fast-fun”, “fast-stat-fun”, “helper”,
“manip”, “special”

**6. Class-Agnostic Design** - Consistent S3 methods across all data
structures - Attribute preservation system maintains class-specific
attributes - Functions work identically on base R, tibble, data.table,
sf, plm objects

**7. OpenMP Parallelization** - Controlled via
`set_collapse(nthreads = n)` or `options(collapse_nthreads = n)` -
Automatic fallback if OpenMP not available - Used in: fsum, fmean,
fmode, and other computationally intensive functions - Compilation flags
in Makevars handle platform differences

**8. data.table Integration** - Reuses core algorithms from data.table
(radixsort, rbindlist, subset) under MPL 2.0 - Works natively on
data.table objects - Compatible with `:=` operator - Modified for
collapse’s specific needs

## Common Development Patterns

### Adding a New Statistical Function

1.  **Create R file** `R/f<name>.R`:
    - Define S3 generic:
      `f<name> <- function(x, ...) UseMethod("f<name>")`
    - Implement methods: `f<name>.default`, `f<name>.matrix`,
      `f<name>.data.frame`
    - Add grouping support via GRP object
    - Add TRA argument for transformations
    - Handle attributes with
      [`copyAttrib()`](https://fastverse.org/collapse/reference/small-helpers.md)
      or
      [`copyMostAttrib()`](https://fastverse.org/collapse/reference/small-helpers.md)
2.  **Create C implementation** in `src/`:
    - Add function signature to `collapse_c.h`
    - Implement ungrouped version: `<name>_C()`
    - Implement grouped version with g, starts, sizes parameters
    - Add OpenMP parallelization where beneficial
    - Register in `ExportSymbols.c` or use Rcpp
3.  **Add tests** in `tests/testthat/test-f<name>.R`:
    - Compare against base R equivalent
    - Test with NA values, Inf, different types
    - Test grouped and weighted versions
    - Test with different data structures
4.  **Update documentation**:
    - Add roxygen2 comments
    - Update `collapse-documentation.Rd` if adding new category
    - Add examples

### Working with C/C++ Code

- All C functions callable from R are declared in `collapse_c.h`
- Use `PROTECT`/`UNPROTECT` for R objects in C code
- Check existing patterns in similar functions before implementing
- OpenMP pragmas: `#pragma omp parallel for num_threads(nthreads)`
- Rcpp functions auto-generate interfaces via `RcppExports.cpp`

### Memory Management

- Use
  [`settransform()`](https://fastverse.org/collapse/reference/ftransform.md),
  [`setv()`](https://fastverse.org/collapse/reference/efficient-programming.md)
  for in-place modifications (reference semantics)
- Regular
  [`ftransform()`](https://fastverse.org/collapse/reference/ftransform.md),
  [`fmutate()`](https://fastverse.org/collapse/reference/ftransform.md)
  copy on modify
- [`qDF()`](https://fastverse.org/collapse/reference/quick-conversion.md),
  [`qDT()`](https://fastverse.org/collapse/reference/quick-conversion.md),
  [`qM()`](https://fastverse.org/collapse/reference/quick-conversion.md)
  for fast conversions without attribute copying
- Avoid unnecessary copies in tight loops

### Testing Philosophy

- Every function needs comprehensive tests
- Test against base R or established packages
- Use random data and [`set.seed()`](https://rdrr.io/r/base/Random.html)
  for reproducibility
- Test edge cases: empty data, single row/column, all NA
- Test with grouped_df, data.table, pseries objects

## Package-Specific Conventions

### Function Naming

- `f` prefix: Fast statistical functions
- `q` prefix: Quick conversions (qDF, qDT, qM)
- Single letter operators: B (between), W (within), D (diff), G
  (growth), L (lag), HDB/HDW (high-dim between/within), STD
  (standardize)
- Many functions have shorter aliases in documentation

### Global Options

Access via
[`get_collapse()`](https://fastverse.org/collapse/reference/collapse-options.md)
and
[`set_collapse()`](https://fastverse.org/collapse/reference/collapse-options.md): -
`nthreads`: OpenMP thread count (default: system) - `na.rm`: Default NA
removal (default: TRUE) - `sort`: Default sorting in GRP (default:
TRUE) - `mask`: Namespace masking level - `verbose`: Verbosity level for
operations

### Attribute Handling

- [`copyAttrib()`](https://fastverse.org/collapse/reference/small-helpers.md):
  Copy all attributes
- [`copyMostAttrib()`](https://fastverse.org/collapse/reference/small-helpers.md):
  Copy all except names, dim, dimnames
- [`setAttrib()`](https://fastverse.org/collapse/reference/small-helpers.md):
  Set single attribute
- Class attributes preserved automatically in most functions

### Error Handling

- R layer validates inputs, provides informative errors
- C layer assumes validated inputs for performance
- Use [`ckmatch()`](https://fastverse.org/collapse/reference/fmatch.md)
  for matching with good error messages
- `unused_arg_action()` for handling unexpected arguments

## Key Files to Understand

- `R/zzz.R`: Package initialization, namespace setup
- `R/GRP.R`: Core grouping system
- `R/global_macros.R`: Global options and constants
- `src/collapse_c.h`: Complete C API
- `src/base_radixsort.c`: Fast ordering algorithm
- `tests/testthat/test-GRP.R`: Comprehensive grouping tests
- `vignettes/collapse_documentation.Rmd`: Documentation guide

## Performance Considerations

- collapse functions are highly optimized; avoid calling base R
  equivalents in hot paths
- Use GRP objects explicitly for repeated grouping operations
- Set `nthreads` appropriately for your system
- Use in-place modification functions when appropriate
- Vector operations are faster than grouped operations on small data
- For large data, collapse typically outperforms base R by 2-100x

## Documentation Resources

- Built-in documentation:
  [`help('collapse-documentation')`](https://fastverse.org/collapse/reference/collapse-documentation.md)
- Vignettes: <https://fastverse.org/collapse/articles/>
- arXiv article: <https://arxiv.org/abs/2403.05038>
- GitHub: <https://github.com/fastverse/collapse>
