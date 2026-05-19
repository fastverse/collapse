# Apply Functions Across Multiple Columns

`across()` can be used inside
[`fmutate`](https://fastverse.org/collapse/reference/ftransform.md) and
[`fsummarise`](https://fastverse.org/collapse/reference/fsummarise.md)
to apply one or more functions to a selection of columns. It is overall
very similar to
[`dplyr::across`](https://dplyr.tidyverse.org/reference/across.html),
but does not support some `rlang` features, has some additional features
(arguments), and is optimized to work with *collapse*'s,
[`.FAST_FUN`](https://fastverse.org/collapse/reference/fast-statistical-functions.md),
yielding much faster computations.

## Usage

``` r
across(.cols = NULL, .fns, ..., .names = NULL,
       .apply = "auto", .transpose = "auto")

# acr(...) can be used to abbreviate across(...)
```

## Arguments

- .cols:

  select columns using column names and expressions (e.g. `a:b` or
  `c(a, b, c:f)`), column indices, logical vectors, or functions
  yielding a logical value e.g. `is.numeric`. `NULL` applies functions
  to all columns except for grouping columns.

- .fns:

  A function, character vector of functions or list of functions.
  Vectors / lists can be named to yield alternative names in the result
  (see `.names`). This argument is evaluated inside
  [`substitute()`](https://rdrr.io/r/base/substitute.html), and the
  content (not the names of vectors/lists) is checked against
  `.FAST_FUN` and `.OPERATOR_FUN`. Matching functions receive vectorized
  execution, other functions are applied to the data in a standard way.

- ...:

  further arguments to `.fns`. Arguments are evaluated in the data
  environment and split by groups as well (for non-vectorized functions,
  if of the same length as the data).

- .names:

  controls the naming of computed columns. `NULL` generates names of the
  form `coli_funj` if multiple functions are used. `.names = TRUE`
  enables this for a single function, `.names = FALSE` disables it for
  multiple functions (sensible for functions such as `.OPERATOR_FUN`
  that rename columns (if `.apply = FALSE`)). Setting `.names = "flip"`
  generates names of the form `funj_coli`. It is also possible to supply
  a function with two arguments for column and function names e.g.
  `function(c, f) paste0(f, "_", c)`. Finally, you can supply a custom
  vector of names which must match `length(.cols) * length(.fns)`.

- .apply:

  controls whether functions are applied column-by-column (`TRUE`) or to
  multiple columns at once (`FALSE`). The default, `"auto"`, does the
  latter for vectorized functions, which have an efficient data frame
  method. It can also be sensible to use `.apply = FALSE` for
  non-vectorized functions, especially multivariate functions like
  [`lm`](https://rdrr.io/r/stats/lm.html) or
  [`pwcor`](https://fastverse.org/collapse/reference/pwcor_pwcov_pwnobs.md),
  or functions renaming the data. See Examples.

- .transpose:

  with multiple `.fns`, `.transpose` controls whether the result is
  ordered first by column, then by function (`TRUE`), or vice-versa
  (`FALSE`). `"auto"` does the former if all functions yield results of
  the same dimensions (dimensions may differ if `.apply = FALSE`). See
  Examples.

## Note

`across()` does not support *purr*-style lambdas, and does not support
`dplyr`-style predicate functions e.g. `across(where(is.numeric), sum)`,
simply use `across(is.numeric, sum)`. In contrast to `dplyr`, you can
also compute on grouping columns.

Also *note* that `across()` is NOT a function in *collapse* but a known
expression that is internally transformed by `fsummarise()/fmutate()`
into something else. Thus, it cannot be called using qualified names,
i.e., `collapse::across()` does not work and is not necessary if
*collapse* is not attached.

## See also

[`fsummarise`](https://fastverse.org/collapse/reference/fsummarise.md),
[`fmutate`](https://fastverse.org/collapse/reference/ftransform.md),
[Fast Data
Manipulation](https://fastverse.org/collapse/reference/fast-data-manipulation.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
# Basic (Weighted) Summaries
fsummarise(wlddev, across(PCGDP:GINI, fmean, w = POP))
#>      PCGDP   LIFEEX     GINI
#> 1 7956.238 65.88068 39.52428

wlddev |> fgroup_by(region, income) |>
    fsummarise(across(PCGDP:GINI, fmean, w = POP))
#>                        region              income      PCGDP   LIFEEX     GINI
#> 1         East Asia & Pacific         High income 29172.7552 76.83283 32.79182
#> 2         East Asia & Pacific Lower middle income  1756.6480 64.25623 36.07647
#> 3         East Asia & Pacific Upper middle income  2357.6168 68.40768 39.94810
#> 4       Europe & Central Asia         High income 29335.5511 75.66616 32.25404
#> 5       Europe & Central Asia          Low income   803.2234 62.45228 32.22326
#> 6       Europe & Central Asia Lower middle income  2256.9684 68.48909 28.97857
#> 7       Europe & Central Asia Upper middle income  7772.5035 68.01573 38.70512
#> 8   Latin America & Caribbean         High income 10217.0626 73.04484 49.41109
#> 9   Latin America & Caribbean          Low income  1317.9024 55.45075 41.10000
#> 10  Latin America & Caribbean Lower middle income  1913.8993 63.86360 50.65115
#> 11  Latin America & Caribbean Upper middle income  7564.8294 69.46947 52.90072
#> 12 Middle East & North Africa         High income 25889.0715 72.38335 36.93006
#> 13 Middle East & North Africa          Low income  1049.8255 63.62748 35.89218
#> 14 Middle East & North Africa Lower middle income  2015.0739 65.55189 33.21199
#>  [ reached 'max' / getOption("max.print") -- omitted 9 rows ]

# Note that for these we don't actually need across...
fselect(wlddev, PCGDP:GINI) |> fmean(w = wlddev$POP, drop = FALSE)
#>      PCGDP   LIFEEX     GINI
#> 1 7956.238 65.88068 39.52428
wlddev |> fgroup_by(region, income) |>
    fselect(PCGDP:GINI, POP) |> fmean(POP, keep.w = FALSE)
#>                        region              income      PCGDP   LIFEEX     GINI
#> 1         East Asia & Pacific         High income 29172.7552 76.83283 32.79182
#> 2         East Asia & Pacific Lower middle income  1756.6480 64.25623 36.07647
#> 3         East Asia & Pacific Upper middle income  2357.6168 68.40768 39.94810
#> 4       Europe & Central Asia         High income 29335.5511 75.66616 32.25404
#> 5       Europe & Central Asia          Low income   803.2234 62.45228 32.22326
#> 6       Europe & Central Asia Lower middle income  2256.9684 68.48909 28.97857
#> 7       Europe & Central Asia Upper middle income  7772.5035 68.01573 38.70512
#> 8   Latin America & Caribbean         High income 10217.0626 73.04484 49.41109
#> 9   Latin America & Caribbean          Low income  1317.9024 55.45075 41.10000
#> 10  Latin America & Caribbean Lower middle income  1913.8993 63.86360 50.65115
#> 11  Latin America & Caribbean Upper middle income  7564.8294 69.46947 52.90072
#> 12 Middle East & North Africa         High income 25889.0715 72.38335 36.93006
#> 13 Middle East & North Africa          Low income  1049.8255 63.62748 35.89218
#> 14 Middle East & North Africa Lower middle income  2015.0739 65.55189 33.21199
#>  [ reached 'max' / getOption("max.print") -- omitted 9 rows ]
collap(wlddev, PCGDP + LIFEEX + GINI ~ region + income, w = ~ POP, keep.w = FALSE)
#>                        region              income      PCGDP   LIFEEX     GINI
#> 1         East Asia & Pacific         High income 29172.7552 76.83283 32.79182
#> 2         East Asia & Pacific Lower middle income  1756.6480 64.25623 36.07647
#> 3         East Asia & Pacific Upper middle income  2357.6168 68.40768 39.94810
#> 4       Europe & Central Asia         High income 29335.5511 75.66616 32.25404
#> 5       Europe & Central Asia          Low income   803.2234 62.45228 32.22326
#> 6       Europe & Central Asia Lower middle income  2256.9684 68.48909 28.97857
#> 7       Europe & Central Asia Upper middle income  7772.5035 68.01573 38.70512
#> 8   Latin America & Caribbean         High income 10217.0626 73.04484 49.41109
#> 9   Latin America & Caribbean          Low income  1317.9024 55.45075 41.10000
#> 10  Latin America & Caribbean Lower middle income  1913.8993 63.86360 50.65115
#> 11  Latin America & Caribbean Upper middle income  7564.8294 69.46947 52.90072
#> 12 Middle East & North Africa         High income 25889.0715 72.38335 36.93006
#> 13 Middle East & North Africa          Low income  1049.8255 63.62748 35.89218
#> 14 Middle East & North Africa Lower middle income  2015.0739 65.55189 33.21199
#>  [ reached 'max' / getOption("max.print") -- omitted 9 rows ]

# But if we want to use some base R function that reguires argument splitting...
wlddev |> na_omit(cols = "POP") |> fgroup_by(region, income) |>
    fsummarise(across(PCGDP:GINI, weighted.mean, w = POP, na.rm = TRUE))
#>                        region              income      PCGDP   LIFEEX     GINI
#> 1         East Asia & Pacific         High income 29172.7552 76.83283 32.79182
#> 2         East Asia & Pacific Lower middle income  1756.6480 64.25623 36.07647
#> 3         East Asia & Pacific Upper middle income  2357.6168 68.40768 39.94810
#> 4       Europe & Central Asia         High income 29335.5511 75.66616 32.25404
#> 5       Europe & Central Asia          Low income   803.2234 62.45228 32.22326
#> 6       Europe & Central Asia Lower middle income  2256.9684 68.48909 28.97857
#> 7       Europe & Central Asia Upper middle income  7772.5035 68.01573 38.70512
#> 8   Latin America & Caribbean         High income 10217.0626 73.04484 49.41109
#> 9   Latin America & Caribbean          Low income  1317.9024 55.45075 41.10000
#> 10  Latin America & Caribbean Lower middle income  1913.8993 63.86360 50.65115
#> 11  Latin America & Caribbean Upper middle income  7564.8294 69.46947 52.90072
#> 12 Middle East & North Africa         High income 25889.0715 72.38335 36.93006
#> 13 Middle East & North Africa          Low income  1049.8255 63.62748 35.89218
#> 14 Middle East & North Africa Lower middle income  2015.0739 65.55189 33.21199
#>  [ reached 'max' / getOption("max.print") -- omitted 9 rows ]

# Or if we want to apply different functions...
wlddev |> fgroup_by(region, income) |>
    fsummarise(across(PCGDP:GINI, list(mu = fmean, sd = fsd), w = POP),
               POP_sum = fsum(POP), OECD = fmean(OECD))
#>                  region              income   PCGDP_mu   PCGDP_sd LIFEEX_mu
#> 1   East Asia & Pacific         High income 29172.7552 14714.1754  76.83283
#> 2   East Asia & Pacific Lower middle income  1756.6480  1064.2676  64.25623
#> 3   East Asia & Pacific Upper middle income  2357.6168  2457.9024  68.40768
#> 4 Europe & Central Asia         High income 29335.5511 13038.1111  75.66616
#> 5 Europe & Central Asia          Low income   803.2234   307.7395  62.45228
#> 6 Europe & Central Asia Lower middle income  2256.9684   970.2648  68.48909
#> 7 Europe & Central Asia Upper middle income  7772.5035  3184.4987  68.01573
#>   LIFEEX_sd  GINI_mu  GINI_sd     POP_sum      OECD
#> 1  5.964994 32.79182 1.230489 11407808149 0.3076923
#> 2  7.536813 36.07647 4.358228 22174820629 0.0000000
#> 3  7.689033 39.94810 3.120103 69639871478 0.0000000
#> 4  4.175866 32.25404 3.023778 27285316560 0.7027027
#> 5  6.050875 32.22326 1.547793   311485944 0.0000000
#> 6  2.452041 28.97857 4.573107  4511786205 0.0000000
#> 7  4.796135 38.70512 4.233085 16972478305 0.0625000
#>  [ reached 'max' / getOption("max.print") -- omitted 16 rows ]
# Note that the above still detects fmean as a fast function, the names of the list
# are irrelevant, but the function name must be typed or passed as a character vector,
# Otherwise functions will be executed by groups e.g. function(x) fmean(x) won't vectorize

# Same, naming in a different way
wlddev |> fgroup_by(region, income) |>
    fsummarise(across(PCGDP:GINI, list(mu = fmean, sd = fsd), w = POP, .names = "flip"),
               sum_POP = fsum(POP), OECD = fmean(OECD))
#>                  region              income   mu_PCGDP   sd_PCGDP mu_LIFEEX
#> 1   East Asia & Pacific         High income 29172.7552 14714.1754  76.83283
#> 2   East Asia & Pacific Lower middle income  1756.6480  1064.2676  64.25623
#> 3   East Asia & Pacific Upper middle income  2357.6168  2457.9024  68.40768
#> 4 Europe & Central Asia         High income 29335.5511 13038.1111  75.66616
#> 5 Europe & Central Asia          Low income   803.2234   307.7395  62.45228
#> 6 Europe & Central Asia Lower middle income  2256.9684   970.2648  68.48909
#> 7 Europe & Central Asia Upper middle income  7772.5035  3184.4987  68.01573
#>   sd_LIFEEX  mu_GINI  sd_GINI     sum_POP      OECD
#> 1  5.964994 32.79182 1.230489 11407808149 0.3076923
#> 2  7.536813 36.07647 4.358228 22174820629 0.0000000
#> 3  7.689033 39.94810 3.120103 69639871478 0.0000000
#> 4  4.175866 32.25404 3.023778 27285316560 0.7027027
#> 5  6.050875 32.22326 1.547793   311485944 0.0000000
#> 6  2.452041 28.97857 4.573107  4511786205 0.0000000
#> 7  4.796135 38.70512 4.233085 16972478305 0.0625000
#>  [ reached 'max' / getOption("max.print") -- omitted 16 rows ]

# Or we want to do more advanced things..
# Such as nesting data frames..
qTBL(wlddev) |> fgroup_by(region, income) |>
    fsummarise(across(c(PCGDP, LIFEEX, ODA),
               function(x) list(Nest = list(x)),
               .apply = FALSE))
#> # A tibble: 23 × 3
#>    region                    income              Nest                
#>    <fct>                     <fct>               <list>              
#>  1 East Asia & Pacific       High income         <tibble [793 × 3]>  
#>  2 East Asia & Pacific       Lower middle income <tibble [793 × 3]>  
#>  3 East Asia & Pacific       Upper middle income <tibble [610 × 3]>  
#>  4 Europe & Central Asia     High income         <tibble [2,257 × 3]>
#>  5 Europe & Central Asia     Low income          <tibble [61 × 3]>   
#>  6 Europe & Central Asia     Lower middle income <tibble [244 × 3]>  
#>  7 Europe & Central Asia     Upper middle income <tibble [976 × 3]>  
#>  8 Latin America & Caribbean High income         <tibble [1,037 × 3]>
#>  9 Latin America & Caribbean Low income          <tibble [61 × 3]>   
#> 10 Latin America & Caribbean Lower middle income <tibble [244 × 3]>  
#> # ℹ 13 more rows
# Or linear models..
qTBL(wlddev) |> fgroup_by(region, income) |>
    fsummarise(across(c(PCGDP, LIFEEX, ODA),
               function(x) list(Mods = list(lm(PCGDP ~., x))),
               .apply = FALSE))
#> # A tibble: 23 × 3
#>    region                    income              Mods  
#>    <fct>                     <fct>               <list>
#>  1 East Asia & Pacific       High income         <lm>  
#>  2 East Asia & Pacific       Lower middle income <lm>  
#>  3 East Asia & Pacific       Upper middle income <lm>  
#>  4 Europe & Central Asia     High income         <lm>  
#>  5 Europe & Central Asia     Low income          <lm>  
#>  6 Europe & Central Asia     Lower middle income <lm>  
#>  7 Europe & Central Asia     Upper middle income <lm>  
#>  8 Latin America & Caribbean High income         <lm>  
#>  9 Latin America & Caribbean Low income          <lm>  
#> 10 Latin America & Caribbean Lower middle income <lm>  
#> # ℹ 13 more rows
# Or cumputing grouped correlation matrices
qTBL(wlddev) |> fgroup_by(region, income) |>
    fsummarise(across(c(PCGDP, LIFEEX, ODA),
      function(x) qDF(pwcor(x), "Variable"), .apply = FALSE))
#> # A tibble: 69 × 6
#>    region                income              Variable  PCGDP  LIFEEX     ODA
#>    <fct>                 <fct>               <chr>     <dbl>   <dbl>   <dbl>
#>  1 East Asia & Pacific   High income         PCGDP     1      0.662  -0.388 
#>  2 East Asia & Pacific   High income         LIFEEX    0.662  1      -0.444 
#>  3 East Asia & Pacific   High income         ODA      -0.388 -0.444   1     
#>  4 East Asia & Pacific   Lower middle income PCGDP     1      0.395  -0.146 
#>  5 East Asia & Pacific   Lower middle income LIFEEX    0.395  1       0.206 
#>  6 East Asia & Pacific   Lower middle income ODA      -0.146  0.206   1     
#>  7 East Asia & Pacific   Upper middle income PCGDP     1      0.700  -0.378 
#>  8 East Asia & Pacific   Upper middle income LIFEEX    0.700  1       0.0796
#>  9 East Asia & Pacific   Upper middle income ODA      -0.378  0.0796  1     
#> 10 Europe & Central Asia High income         PCGDP     1      0.586  -0.329 
#> # ℹ 59 more rows

# Here calculating 1- and 10-year lags and growth rates of these variables
qTBL(wlddev) |> fgroup_by(country) |>
    fmutate(across(c(PCGDP, LIFEEX, ODA), list(L, G),
                   n = c(1, 10), t = year, .names = FALSE))
#> # A tibble: 13,176 × 25
#>    country  iso3c date        year decade region income OECD  PCGDP LIFEEX  GINI
#>    <chr>    <fct> <date>     <int>  <int> <fct>  <fct>  <lgl> <dbl>  <dbl> <dbl>
#>  1 Afghani… AFG   1961-01-01  1960   1960 South… Low i… FALSE    NA   32.4    NA
#>  2 Afghani… AFG   1962-01-01  1961   1960 South… Low i… FALSE    NA   33.0    NA
#>  3 Afghani… AFG   1963-01-01  1962   1960 South… Low i… FALSE    NA   33.5    NA
#>  4 Afghani… AFG   1964-01-01  1963   1960 South… Low i… FALSE    NA   34.0    NA
#>  5 Afghani… AFG   1965-01-01  1964   1960 South… Low i… FALSE    NA   34.5    NA
#>  6 Afghani… AFG   1966-01-01  1965   1960 South… Low i… FALSE    NA   34.9    NA
#>  7 Afghani… AFG   1967-01-01  1966   1960 South… Low i… FALSE    NA   35.4    NA
#>  8 Afghani… AFG   1968-01-01  1967   1960 South… Low i… FALSE    NA   35.9    NA
#>  9 Afghani… AFG   1969-01-01  1968   1960 South… Low i… FALSE    NA   36.4    NA
#> 10 Afghani… AFG   1970-01-01  1969   1960 South… Low i… FALSE    NA   36.9    NA
#> # ℹ 13,166 more rows
#> # ℹ 14 more variables: ODA <dbl>, POP <dbl>, L1.PCGDP <dbl>, G1.PCGDP <dbl>,
#> #   L10.PCGDP <dbl>, L10G1.PCGDP <dbl>, L1.LIFEEX <dbl>, G1.LIFEEX <dbl>,
#> #   L10.LIFEEX <dbl>, L10G1.LIFEEX <dbl>, L1.ODA <dbl>, G1.ODA <dbl>,
#> #   L10.ODA <dbl>, L10G1.ODA <dbl>
#> 
#> Grouped by:  country  [216 | 61 (0)] 

# Same but variables in different order
qTBL(wlddev) |> fgroup_by(country) |>
    fmutate(across(c(PCGDP, LIFEEX, ODA), list(L, G), n = c(1, 10),
                   t = year, .names = FALSE, .transpose = FALSE))
#> # A tibble: 13,176 × 25
#>    country  iso3c date        year decade region income OECD  PCGDP LIFEEX  GINI
#>    <chr>    <fct> <date>     <int>  <int> <fct>  <fct>  <lgl> <dbl>  <dbl> <dbl>
#>  1 Afghani… AFG   1961-01-01  1960   1960 South… Low i… FALSE    NA   32.4    NA
#>  2 Afghani… AFG   1962-01-01  1961   1960 South… Low i… FALSE    NA   33.0    NA
#>  3 Afghani… AFG   1963-01-01  1962   1960 South… Low i… FALSE    NA   33.5    NA
#>  4 Afghani… AFG   1964-01-01  1963   1960 South… Low i… FALSE    NA   34.0    NA
#>  5 Afghani… AFG   1965-01-01  1964   1960 South… Low i… FALSE    NA   34.5    NA
#>  6 Afghani… AFG   1966-01-01  1965   1960 South… Low i… FALSE    NA   34.9    NA
#>  7 Afghani… AFG   1967-01-01  1966   1960 South… Low i… FALSE    NA   35.4    NA
#>  8 Afghani… AFG   1968-01-01  1967   1960 South… Low i… FALSE    NA   35.9    NA
#>  9 Afghani… AFG   1969-01-01  1968   1960 South… Low i… FALSE    NA   36.4    NA
#> 10 Afghani… AFG   1970-01-01  1969   1960 South… Low i… FALSE    NA   36.9    NA
#> # ℹ 13,166 more rows
#> # ℹ 14 more variables: ODA <dbl>, POP <dbl>, L1.PCGDP <dbl>, L10.PCGDP <dbl>,
#> #   L1.LIFEEX <dbl>, L10.LIFEEX <dbl>, L1.ODA <dbl>, L10.ODA <dbl>,
#> #   G1.PCGDP <dbl>, L10G1.PCGDP <dbl>, G1.LIFEEX <dbl>, L10G1.LIFEEX <dbl>,
#> #   G1.ODA <dbl>, L10G1.ODA <dbl>
#> 
#> Grouped by:  country  [216 | 61 (0)] 
```
