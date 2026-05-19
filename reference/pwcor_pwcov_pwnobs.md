# (Pairwise, Weighted) Correlations, Covariances and Observation Counts

Computes (pairwise, weighted) Pearson's correlations, covariances and
observation counts. Pairwise correlations and covariances can be
computed together with observation counts and p-values, and output as 3D
array (default) or list of matrices. `pwcor` and `pwcov` offer an
elaborate print method.

## Usage

``` r
pwcor(X, ..., w = NULL, N = FALSE, P = FALSE, array = TRUE, use = "pairwise.complete.obs")

pwcov(X, ..., w = NULL, N = FALSE, P = FALSE, array = TRUE, use = "pairwise.complete.obs")

pwnobs(X)

# S3 method for class 'pwcor'
print(x, digits = .op[["digits"]], sig.level = 0.05,
      show = c("all","lower.tri","upper.tri"), spacing = 1L, return = FALSE, ...)

# S3 method for class 'pwcov'
print(x, digits = .op[["digits"]], sig.level = 0.05,
      show = c("all","lower.tri","upper.tri"), spacing = 1L, return = FALSE, ...)
```

## Arguments

- X:

  a matrix or data.frame, for `pwcor` and `pwcov` all columns must be
  numeric. All functions are faster on matrices, so converting is
  advised for large data (see
  [`qM`](https://fastverse.org/collapse/reference/quick-conversion.md)).

- x:

  an object of class 'pwcor' / 'pwcov'.

- w:

  numeric. A vector of (frequency) weights.

- N:

  logical. `TRUE` also computes pairwise observation counts.

- P:

  logical. `TRUE` also computes pairwise p-values (same as
  [`cor.test`](https://rdrr.io/r/stats/cor.test.html) and
  `Hmisc::rcorr`).

- array:

  logical. If `N = TRUE` or `P = TRUE`, `TRUE` (default) returns output
  as 3D array whereas `FALSE` returns a list of matrices.

- use:

  argument passed to [`cor`](https://rdrr.io/r/stats/cor.html) /
  [`cov`](https://rdrr.io/r/stats/cor.html). If
  `use != "pairwise.complete.obs"`, `sum(complete.cases(X))` is used for
  `N`, and p-values are computed accordingly.

- digits:

  integer. The number of digits to round to in print.

- sig.level:

  numeric. P-value threshold below which a `'*'` is displayed above
  significant coefficients if `P = TRUE`.

- show:

  character. The part of the correlation / covariance matrix to display.

- spacing:

  integer. Controls the spacing between different reported quantities in
  the printout of the matrix: 0 - compressed, 1 - single space, 2 -
  double space.

- return:

  logical. `TRUE` returns the formatted object from the print method for
  exporting. The default is to return `x` invisibly.

- ...:

  other arguments passed to [`cor`](https://rdrr.io/r/stats/cor.html) or
  [`cov`](https://rdrr.io/r/stats/cor.html). Only sensible if
  `P = FALSE`.

## Value

a numeric matrix, 3D array or list of matrices with the computed
statistics. For `pwcor` and `pwcov` the object has a class 'pwcor' and
'pwcov', respectively.

## Note

`weights::wtd.cors` is imported for weighted pairwise correlations
(written in C for speed). For weighted correlations with bootstrap SE's
see `weights::wtd.cor` (bootstrap can be slow). Weighted correlations
for complex surveys are implemented in `jtools::svycor`. An equivalent
and faster implementation of `pwcor` (without weights) is provided in
`Hmisc::rcorr` (written in Fortran).

## See also

[`qsu`](https://fastverse.org/collapse/reference/qsu.md), [Summary
Statistics](https://fastverse.org/collapse/reference/summary-statistics.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
mna <- na_insert(mtcars)
pwcor(mna)
#>        mpg   cyl  disp    hp  drat    wt  qsec    vs    am  gear  carb
#> mpg     1   -.86  -.81  -.71   .74  -.87   .38   .67   .53   .41  -.55
#> cyl   -.86    1    .91   .81  -.75   .79  -.58  -.86  -.50  -.54   .53
#> disp  -.81   .91    1    .85  -.79   .91  -.36  -.72  -.66  -.75   .38
#> hp    -.71   .81   .85    1   -.36   .60  -.72  -.69  -.05  -.06   .74
#> drat   .74  -.75  -.79  -.36    1   -.75   .10   .48   .70   .70  -.14
#> wt    -.87   .79   .91   .60  -.75    1   -.16  -.58  -.62  -.58   .42
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
pwcov(mna)
#>            mpg       cyl      disp        hp      drat        wt      qsec
#> mpg      34.37     -9.68   -588.25   -264.29      2.37     -5.04      4.22
#> cyl      -9.68      3.35    219.83     98.14     -0.79      1.53     -2.03
#> disp   -588.25    219.83  16172.58   5973.18    -58.42    127.09    -74.75
#> hp     -264.29     98.14   5973.18   4545.18    -12.04     40.86    -97.99
#> drat      2.37     -0.79    -58.42    -12.04      0.31     -0.43      0.11
#> wt       -5.04      1.53    127.09     40.86     -0.43      1.04     -0.30
#>             vs        am      gear      carb
#> mpg       2.06      1.40      1.89     -5.33
#> cyl      -0.82     -0.41     -0.75      1.65
#> disp    -49.07    -38.96    -67.91     66.84
#> hp      -23.37     -1.74     -3.53     84.37
#> drat      0.13      0.16      0.29     -0.11
#> wt       -0.31     -0.28     -0.46      0.75
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
pwnobs(mna)
#>      mpg cyl disp hp drat wt qsec vs am gear carb
#> mpg   29  26   26 26   27 26   26 26 26   26   27
#> cyl   26  29   26 26   26 26   26 26 26   27   26
#> disp  26  26   29 26   26 26   26 26 26   26   26
#> hp    26  26   26 29   26 27   26 26 27   27   27
#> drat  27  26   26 26   29 26   26 27 27   26   26
#> wt    26  26   26 27   26 29   27 27 26   27   26
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
pwcor(mna, N = TRUE)
#>            mpg       cyl      disp        hp      drat        wt      qsec
#> mpg    1  (29) -.86 (26) -.81 (26) -.71 (26)  .74 (27) -.87 (26)  .38 (26)
#> cyl  -.86 (26)   1  (29)  .91 (26)  .81 (26) -.75 (26)  .79 (26) -.58 (26)
#> disp -.81 (26)  .91 (26)   1  (29)  .85 (26) -.79 (26)  .91 (26) -.36 (26)
#> hp   -.71 (26)  .81 (26)  .85 (26)   1  (29) -.36 (26)  .60 (27) -.72 (26)
#> drat  .74 (27) -.75 (26) -.79 (26) -.36 (26)   1  (29) -.75 (26)  .10 (26)
#> wt   -.87 (26)  .79 (26)  .91 (26)  .60 (27) -.75 (26)   1  (29) -.16 (27)
#>             vs        am      gear      carb
#> mpg   .67 (26)  .53 (26)  .41 (26) -.55 (27)
#> cyl  -.86 (26) -.50 (26) -.54 (27)  .53 (26)
#> disp -.72 (26) -.66 (26) -.75 (26)  .38 (26)
#> hp   -.69 (26) -.05 (27) -.06 (27)  .74 (27)
#> drat  .48 (27)  .70 (27)  .70 (26) -.14 (26)
#> wt   -.58 (27) -.62 (26) -.58 (27)  .42 (26)
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
pwcor(mna, P = TRUE)
#>         mpg    cyl   disp     hp   drat     wt   qsec     vs     am   gear
#> mpg     1    -.86*  -.81*  -.71*   .74*  -.87*   .38    .67*   .53*   .41*
#> cyl   -.86*    1     .91*   .81*  -.75*   .79*  -.58*  -.86*  -.50*  -.54*
#> disp  -.81*   .91*    1     .85*  -.79*   .91*  -.36   -.72*  -.66*  -.75*
#> hp    -.71*   .81*   .85*    1    -.36    .60*  -.72*  -.69*  -.05   -.06 
#> drat   .74*  -.75*  -.79*  -.36     1    -.75*   .10    .48*   .70*   .70*
#> wt    -.87*   .79*   .91*   .60*  -.75*    1    -.16   -.58*  -.62*  -.58*
#>        carb
#> mpg   -.55*
#> cyl    .53*
#> disp   .38 
#> hp     .74*
#> drat  -.14 
#> wt     .42*
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
pwcor(mna, N = TRUE, P = TRUE)
#>             mpg        cyl       disp         hp       drat         wt
#> mpg    1   (29) -.86* (26) -.81* (26) -.71* (26)  .74* (27) -.87* (26)
#> cyl  -.86* (26)   1   (29)  .91* (26)  .81* (26) -.75* (26)  .79* (26)
#> disp -.81* (26)  .91* (26)   1   (29)  .85* (26) -.79* (26)  .91* (26)
#> hp   -.71* (26)  .81* (26)  .85* (26)   1   (29) -.36  (26)  .60* (27)
#> drat  .74* (27) -.75* (26) -.79* (26) -.36  (26)   1   (29) -.75* (26)
#> wt   -.87* (26)  .79* (26)  .91* (26)  .60* (27) -.75* (26)   1   (29)
#>            qsec         vs         am       gear       carb
#> mpg   .38  (26)  .67* (26)  .53* (26)  .41* (26) -.55* (27)
#> cyl  -.58* (26) -.86* (26) -.50* (26) -.54* (27)  .53* (26)
#> disp -.36  (26) -.72* (26) -.66* (26) -.75* (26)  .38  (26)
#> hp   -.72* (26) -.69* (26) -.05  (27) -.06  (27)  .74* (27)
#> drat  .10  (26)  .48* (27)  .70* (27)  .70* (26) -.14  (26)
#> wt   -.16  (27) -.58* (27) -.62* (26) -.58* (27)  .42* (26)
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
aperm(pwcor(mna, N = TRUE, P = TRUE))
#> , , mpg
#> 
#>   mpg   cyl  disp    hp  drat    wt  qsec    vs    am  gear  carb
#> r   1  -.86  -.81  -.71   .74  -.87   .38   .67   .53   .41  -.55
#> N  29    26    26    26    27    26    26    26    26    26    27
#> P       .00   .00   .00   .00   .00   .06   .00   .01   .04   .00
#> 
#> , , cyl
#> 
#>     mpg    cyl  disp    hp  drat    wt  qsec    vs    am  gear  carb
#> r  -.86   1.00   .91   .81  -.75   .79  -.58  -.86  -.50  -.54   .53
#> N    26     29    26    26    26    26    26    26    26    27    26
#> P   .00          .00   .00   .00   .00   .00   .00   .01   .00   .01
#> 
#> , , disp
#> 
#>     mpg   cyl disp    hp
#> r  -.81   .91    1   .85
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 8 slices ] 
print(pwcor(mna, N = TRUE, P = TRUE), digits = 3, sig.level = 0.01, show = "lower.tri")
#>              mpg         cyl        disp          hp        drat          wt
#> mpg    1    (29)                                                            
#> cyl  -.859* (26)   1    (29)                                                
#> disp -.811* (26)  .913* (26)   1    (29)                                    
#> hp   -.713* (26)  .812* (26)  .852* (26)   1    (29)                        
#> drat  .736* (27) -.753* (26) -.795* (26) -.361  (26)   1    (29)            
#> wt   -.866* (26)  .793* (26)  .910* (26)  .601* (27) -.748* (26)   1    (29)
#>             qsec          vs          am        gear        carb
#> mpg                                                             
#> cyl                                                             
#> disp                                                            
#> hp                                                              
#> drat                                                            
#> wt                                                              
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
pwcor(mna, N = TRUE, P = TRUE, array = FALSE)
#> $r
#>        mpg    cyl  disp    hp  drat    wt  qsec    vs     am  gear   carb
#> mpg      1   -.86  -.81  -.71   .74  -.87   .38   .67    .53   .41   -.55
#> cyl   -.86   1.00   .91   .81  -.75   .79  -.58  -.86   -.50  -.54    .53
#> disp  -.81    .91     1   .85  -.79   .91  -.36  -.72   -.66  -.75    .38
#> hp    -.71    .81   .85     1  -.36   .60  -.72  -.69   -.05  -.06    .74
#> drat   .74   -.75  -.79  -.36     1  -.75   .10   .48    .70   .70   -.14
#> wt    -.87    .79   .91   .60  -.75     1  -.16  -.58   -.62  -.58    .42
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
#> 
#> $N
#>      mpg cyl disp  hp drat  wt qsec  vs  am gear carb
#> mpg   29  26   26  26   27  26   26  26  26   26   27
#> cyl   26  29   26  26   26  26   26  26  26   27   26
#> disp  26  26   29  26   26  26   26  26  26   26   26
#> hp    26  26   26  29   26  27   26  26  27   27   27
#> drat  27  26   26  26   29  26   26  27  27   26   26
#> wt    26  26   26  27   26  29   27  27  26   27   26
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
#> 
#> $P
#>       mpg  cyl disp   hp drat   wt qsec   vs   am gear carb
#> mpg        .00  .00  .00  .00  .00  .06  .00  .01  .04  .00
#> cyl   .00       .00  .00  .00  .00  .00  .00  .01  .00  .01
#> disp  .00  .00       .00  .00  .00  .07  .00  .00  .00  .06
#> hp    .00  .00  .00       .07  .00  .00  .00  .79  .75  .00
#> drat  .00  .00  .00  .07       .00  .61  .01  .00  .00  .51
#> wt    .00  .00  .00  .00  .00       .42  .00  .00  .00  .03
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
#> 
print(pwcor(mna, N = TRUE, P = TRUE, array = FALSE), show = "lower.tri")
#> $r
#>        mpg    cyl  disp    hp  drat    wt  qsec    vs     am  gear   carb
#> mpg      1                                                               
#> cyl   -.86   1.00                                                        
#> disp  -.81    .91     1                                                  
#> hp    -.71    .81   .85     1                                            
#> drat   .74   -.75  -.79  -.36     1                                      
#> wt    -.87    .79   .91   .60  -.75     1                                
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
#> 
#> $N
#>      mpg cyl disp  hp drat  wt qsec  vs  am gear carb
#> mpg   29                                             
#> cyl   26  29                                         
#> disp  26  26   29                                    
#> hp    26  26   26  29                                
#> drat  27  26   26  26   29                           
#> wt    26  26   26  27   26  29                       
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
#> 
#> $P
#>       mpg  cyl disp   hp drat   wt qsec   vs   am gear carb
#> mpg                                                        
#> cyl   .00                                                  
#> disp  .00  .00                                             
#> hp    .00  .00  .00                                        
#> drat  .00  .00  .00  .07                                   
#> wt    .00  .00  .00  .00  .00                              
#>  [ reached 'max' / getOption("max.print") -- omitted 5 rows ]
#> 

```
