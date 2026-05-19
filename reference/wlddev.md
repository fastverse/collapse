# World Development Dataset

This dataset contains 5 indicators from the World Bank's World
Development Indicators (WDI) database: (1) GDP per capita, (2) Life
expectancy at birth, (3) GINI index, (4) Net ODA and official aid
received and (5) Population. The panel data is balanced and covers 216
present and historic countries from 1960-2020 (World Bank aggregates and
regional entities are excluded).

Apart from the indicators the data contains a number of identifiers
(character country name, factor ISO3 country code, World Bank region and
income level, numeric year and decade) and 2 generated variables: A
logical variable indicating whether the country is an OECD member, and a
fictitious variable stating the date the data was recorded. These
variables were added so that all common data-types are represented in
this dataset, making it an ideal test-dataset for certain *collapse*
functions.

## Usage

``` r
data("wlddev")
```

## Format

A data frame with 13176 observations on the following 13 variables. All
variables are labeled e.g. have a 'label' attribute.

- `country`:

  *chr* Country Name

- `iso3c`:

  *fct* Country Code

- `date`:

  *date* Date Recorded (Fictitious)

- `year`:

  *int* Year

- `decade`:

  *int* Decade

- `region`:

  *fct* World Bank Region

- `income`:

  *fct* World Bank Income Level

- `OECD`:

  *log* Is OECD Member Country?

- `PCGDP`:

  *num* GDP per capita (constant 2010 US\$)

- `LIFEEX`:

  *num* Life expectancy at birth, total (years)

- `GINI`:

  *num* GINI index (World Bank estimate)

- `ODA`:

  *num* Net official development assistance and official aid received
  (constant 2018 US\$)

- `POP`:

  *num* Population, total

## Source

<https://data.worldbank.org/>, accessed via the `WDI` package. The codes
for the series are
`c("NY.GDP.PCAP.KD", "SP.DYN.LE00.IN", "SI.POV.GINI", "DT.ODA.ALLD.KD", "SP.POP.TOTL")`.

## See also

[`GGDC10S`](https://fastverse.org/collapse/reference/GGDC10S.md),
[Collapse
Overview](https://fastverse.org/collapse/reference/collapse-documentation.md)

## Examples

``` r
data(wlddev)

# Panel-summarizing the 5 series
qsu(wlddev, pid = ~iso3c, cols = 9:13, vlabels = TRUE)
#> , , PCGDP: GDP per capita (constant 2010 US$)
#> 
#>              N/T        Mean          SD          Min         Max
#> Overall     9470   12048.778  19077.6416     132.0776  196061.417
#> Between      206  12962.6054  20189.9007     253.1886   141200.38
#> Within   45.9709   12048.778   6723.6808  -33504.8721  76767.5254
#> 
#> , , LIFEEX: Life expectancy at birth, total (years)
#> 
#>              N/T     Mean       SD      Min      Max
#> Overall    11670  64.2963  11.4764   18.907  85.4171
#> Between      207  64.9537   9.8936  40.9663  85.4171
#> Within   56.3768  64.2963   6.0842  32.9068  84.4198
#> 
#> , , GINI: Gini index (World Bank estimate)
#> 
#>              N/T     Mean      SD      Min      Max
#> Overall     1744  38.5341  9.2006     20.7     65.8
#> Between      167  39.4233  8.1356  24.8667  61.7143
#> Within   10.4431  38.5341  2.9277  25.3917  55.3591
#> 
#> , , ODA: Net official development assistance and official aid received (constant 2018 US$)
#> 
#>              N/T        Mean          SD              Min             Max
#> Overall     8608  454'720131  868'712654      -997'679993  2.56715605e+10
#> Between      178  439'168412  569'049959       468717.916  3.62337432e+09
#> Within   48.3596  454'720131  650'709624  -2.44379420e+09  2.45610972e+10
#> 
#> , , POP: Population, total
#> 
#>              N/T         Mean           SD          Min             Max
#> Overall    12919  24'245971.6   102'120674         2833  1.39771500e+09
#> Between      216    24'178573  98'616506.7    8343.3333  1.08786967e+09
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 1 row ] 

# By Region
qsu(wlddev, by = ~region, cols = 9:13, vlabels = TRUE)
#> , , PCGDP: GDP per capita (constant 2010 US$)
#> 
#>                                N        Mean          SD         Min
#> East Asia & Pacific         1467  10513.2441  14383.5507    132.0776
#> Europe & Central Asia       2243  25992.9618  26435.1316    366.9354
#> Latin America & Caribbean   1976   7628.4477   8818.5055   1005.4085
#> Middle East & North Africa   842  13878.4213  18419.7912    578.5996
#> North America                180    48699.76  24196.2855  16405.9053
#> South Asia                   382   1235.9256   1611.2232    265.9625
#> Sub-Saharan Africa          2380   1840.0259   2596.0104    164.3366
#>                                    Max
#> East Asia & Pacific         71992.1517
#> Europe & Central Asia       196061.417
#> Latin America & Caribbean   88391.3331
#> Middle East & North Africa  116232.753
#> North America               113236.091
#> South Asia                    8476.564
#> Sub-Saharan Africa          20532.9523
#> 
#> , , LIFEEX: Life expectancy at birth, total (years)
#> 
#>                                N     Mean       SD      Min      Max
#> East Asia & Pacific         1807  65.9445  10.1633   18.907   85.078
#> Europe & Central Asia       3046  72.1625   5.7602   45.369  85.4171
#> Latin America & Caribbean   2107  68.3486   7.3768   41.762  82.1902
#> Middle East & North Africa  1226  66.2508   9.8306   29.919  82.8049
#> North America                144  76.2867   3.5734  68.8978  82.0488
#> South Asia                   480  57.5585  11.3004   32.446   78.921
#> Sub-Saharan Africa          2860   51.581   8.6876   26.172  74.5146
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 3 slices ] 

# Panel-summary by region
qsu(wlddev, by = ~region, pid = ~iso3c, cols = 9:13, vlabels = TRUE)
#> , , Overall, PCGDP: GDP per capita (constant 2010 US$)
#> 
#>                              N/T        Mean          SD         Min
#> East Asia & Pacific         1467  10513.2441  14383.5507    132.0776
#> Europe & Central Asia       2243  25992.9618  26435.1316    366.9354
#> Latin America & Caribbean   1976   7628.4477   8818.5055   1005.4085
#> Middle East & North Africa   842  13878.4213  18419.7912    578.5996
#> North America                180    48699.76  24196.2855  16405.9053
#> South Asia                   382   1235.9256   1611.2232    265.9625
#> Sub-Saharan Africa          2380   1840.0259   2596.0104    164.3366
#>                                    Max
#> East Asia & Pacific         71992.1517
#> Europe & Central Asia       196061.417
#> Latin America & Caribbean   88391.3331
#> Middle East & North Africa  116232.753
#> North America               113236.091
#> South Asia                    8476.564
#> Sub-Saharan Africa          20532.9523
#> 
#> , , Between, PCGDP: GDP per capita (constant 2010 US$)
#> 
#>                             N/T        Mean          SD         Min         Max
#> East Asia & Pacific          34  10513.2441   12771.742    444.2899  39722.0077
#> Europe & Central Asia        56  25992.9618   24051.035    809.4753   141200.38
#> Latin America & Caribbean    38   7628.4477   8470.9708   1357.3326  77403.7443
#> Middle East & North Africa   20  13878.4213  17251.6962   1069.6596  64878.4021
#> North America                 3    48699.76  18604.4369  35260.4708  74934.5874
#> South Asia                    8   1235.9256   1488.3669      413.68   6621.5002
#> Sub-Saharan Africa           47   1840.0259   2234.3254    253.1886   9922.0052
#> 
#>  [ reached 'max' / getOption("max.print") -- omitted 13 slices ] 

# Pairwise correlations: Ovarall
print(pwcor(get_vars(wlddev, 9:13), N = TRUE, P = TRUE), show = "lower.tri")
#>               PCGDP        LIFEEX         GINI          ODA           POP
#> PCGDP    1   (9470)                                                      
#> LIFEEX  .57* (9022)   1   (11670)                                        
#> GINI   -.44* (1735)  -.35* (1742)   1   (1744)                           
#> ODA    -.16* (7128)  -.02  (8142) -.20* (1109)   1   (8608)              
#> POP    -.06* (9470)  .03* (11659)  .04  (1744)  .31* (8597)   1   (12919)

# Pairwise correlations: Between Countries
print(pwcor(fmean(get_vars(wlddev, 9:13), wlddev$iso3c), N = TRUE, P = TRUE), show = "lower.tri")
#>              PCGDP      LIFEEX        GINI         ODA         POP
#> PCGDP    1   (206)                                                
#> LIFEEX  .60* (199)   1   (207)                                    
#> GINI   -.42* (165) -.40* (165)   1   (167)                        
#> ODA    -.25* (172) -.21* (172) -.19* (145)   1   (178)            
#> POP    -.07  (206) -.02  (207) -.04  (167)  .50* (178)   1   (216)

# Pairwise correlations: Within Countries
print(pwcor(fwithin(get_vars(wlddev, 9:13), wlddev$iso3c), N = TRUE, P = TRUE), show = "lower.tri")
#>               PCGDP        LIFEEX         GINI          ODA           POP
#> PCGDP    1   (9470)                                                      
#> LIFEEX  .31* (9022)   1   (11670)                                        
#> GINI   -.01  (1735)  -.16* (1742)   1   (1744)                           
#> ODA    -.01  (7128)   .17* (8142) -.08* (1109)   1   (8608)              
#> POP     .06* (9470)  .29* (11659)  .01  (1744) -.11* (8597)   1   (12919)
```
