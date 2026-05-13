/*
 * Standalone C benchmark: multiple-accumulator optimization for fmean
 *
 * Compares baseline (single accumulator, analogous to #pragma omp simd) vs
 * optimized (FMEAN_N_ACC=4 independent accumulators) across:
 *   - na.rm=FALSE (plain summation / N)
 *   - na.rm=TRUE  (conditional summation with NA count)
 *   - weighted    (sum(x*w) / sum(w))
 *
 * Build (no OpenMP):
 *   gcc -O2 -o bench_fmean benchmark_fmean.c -lm && ./bench_fmean
 *
 * Build (with OpenMP, GCC):
 *   gcc -O2 -fopenmp -o bench_fmean benchmark_fmean.c -lm && ./bench_fmean
 *
 * Build (with OpenMP, clang on macOS with libomp):
 *   cc -O2 -Xclang -fopenmp -lomp -o bench_fmean benchmark_fmean.c -lm && ./bench_fmean
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define FMEAN_N_ACC 4
#define NA_REAL     R_NaN   /* placeholder; actual NaN used for timing */
#undef  NA_REAL
#define NA_REAL     (0.0/0.0)

#define NISNAN(x)   (!isnan(x))

/* ---------- baseline implementations (single accumulator) ---------- */

static double baseline_double_narm0(const double *px, int l) {
    double mean = 0.0;
    for(int i = 0; i < l; ++i) mean += px[i];
    return mean / l;
}

static double baseline_double_narm1(const double *px, int l) {
    int j = 1, n = 1;
    double mean = px[0];
    while(isnan(mean) && j != l) mean = px[j++];
    if(j != l) {
        for(int i = j; i < l; ++i) {
            int tmp = !isnan(px[i]);
            mean += tmp ? px[i] : 0.0;
            n   += tmp;
        }
    }
    return mean / n;
}

static double baseline_weighted(const double *px, const double *pw, int l) {
    double mean = 0.0, sumw = 0.0;
    for(int i = 0; i < l; ++i) { mean += px[i]*pw[i]; sumw += pw[i]; }
    return mean / sumw;
}

/* ---------- optimized implementations (FMEAN_N_ACC accumulators) ---------- */

static double opt_double_narm0(const double *px, int l) {
    double acc[FMEAN_N_ACC] = {0.0};
    int rem = l % FMEAN_N_ACC;
    for(int i = 0; i < rem; ++i) acc[0] += px[i];
    for(int i = rem; i < l; i += FMEAN_N_ACC) {
        for(int k = 0; k < FMEAN_N_ACC; ++k) acc[k] += px[i+k];
    }
    double mean = 0.0;
    for(int k = 0; k < FMEAN_N_ACC; ++k) mean += acc[k];
    return mean / l;
}

static double opt_double_narm1(const double *px, int l) {
    int j = 1, n = 1;
    double mean = px[0];
    while(isnan(mean) && j != l) mean = px[j++];
    if(j != l) {
        double acc[FMEAN_N_ACC]  = {0.0};
        int    nacc[FMEAN_N_ACC] = {0};
        acc[0] = mean; nacc[0] = 1;
        int rem = (l - j) % FMEAN_N_ACC;
        for(int i = j; i < j + rem; ++i) {
            int tmp = !isnan(px[i]);
            acc[0] += tmp ? px[i] : 0.0;
            nacc[0] += tmp;
        }
        for(int i = j + rem; i < l; i += FMEAN_N_ACC) {
            for(int k = 0; k < FMEAN_N_ACC; ++k) {
                int tmp = !isnan(px[i+k]);
                acc[k]  += tmp ? px[i+k] : 0.0;
                nacc[k] += tmp;
            }
        }
        double sum = 0.0; int count = 0;
        for(int k = 0; k < FMEAN_N_ACC; ++k) { sum += acc[k]; count += nacc[k]; }
        return sum / count;
    }
    return mean / n;
}

static double opt_weighted(const double *px, const double *pw, int l) {
    double macc[FMEAN_N_ACC] = {0.0}, wacc[FMEAN_N_ACC] = {0.0};
    int rem = l % FMEAN_N_ACC;
    for(int i = 0; i < rem; ++i) { macc[0] += px[i]*pw[i]; wacc[0] += pw[i]; }
    for(int i = rem; i < l; i += FMEAN_N_ACC) {
        for(int k = 0; k < FMEAN_N_ACC; ++k) {
            macc[k] += px[i+k] * pw[i+k];
            wacc[k] += pw[i+k];
        }
    }
    double mean = 0.0, sumw = 0.0;
    for(int k = 0; k < FMEAN_N_ACC; ++k) { mean += macc[k]; sumw += wacc[k]; }
    return mean / sumw;
}

/* ---------- timing helper ---------- */

typedef struct { double min_us; double median_us; } TimingResult;

static int cmp_double(const void *a, const void *b) {
    double da = *(const double*)a, db = *(const double*)b;
    return (da > db) - (da < db);
}

static TimingResult time_fn_double(double (*fn)(const double*, int),
                                   const double *px, int l,
                                   int iters) {
    double *times = (double*)malloc(iters * sizeof(double));
    double sink = 0.0;
    for(int it = 0; it < iters; ++it) {
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        sink += fn(px, l);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        times[it] = (t1.tv_sec - t0.tv_sec) * 1e6 +
                    (t1.tv_nsec - t0.tv_nsec) / 1e3;
    }
    (void)sink;
    qsort(times, iters, sizeof(double), cmp_double);
    TimingResult r = { times[0], times[iters/2] };
    free(times);
    return r;
}

typedef double (*wfn_t)(const double*, const double*, int);

static TimingResult time_fn_weighted(wfn_t fn,
                                     const double *px, const double *pw,
                                     int l, int iters) {
    double *times = (double*)malloc(iters * sizeof(double));
    double sink = 0.0;
    for(int it = 0; it < iters; ++it) {
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        sink += fn(px, pw, l);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        times[it] = (t1.tv_sec - t0.tv_sec) * 1e6 +
                    (t1.tv_nsec - t0.tv_nsec) / 1e3;
    }
    (void)sink;
    qsort(times, iters, sizeof(double), cmp_double);
    TimingResult r = { times[0], times[iters/2] };
    free(times);
    return r;
}

/* ---------- main ---------- */

int main(void) {
    const int iters = 500;
    const int sizes[] = {1000, 10000, 100000, 500000, 1000000};
    const int nsizes = (int)(sizeof(sizes)/sizeof(sizes[0]));

    printf("%-12s  %-25s  %10s  %10s  %8s\n",
           "n", "function", "min (µs)", "median (µs)", "speedup");
    printf("%s\n", "--------------------------------------------------------------------");

    for(int si = 0; si < nsizes; ++si) {
        int l = sizes[si];

        /* Allocate and fill with pseudo-random doubles (LCG) */
        double *px = (double*)malloc(l * sizeof(double));
        double *pw = (double*)malloc(l * sizeof(double));
        unsigned long state = 42;
        for(int i = 0; i < l; ++i) {
            state = state * 6364136223846793005ULL + 1442695040888963407ULL;
            px[i] = (double)(state >> 33) / (double)(1ULL << 31) - 1.0;
            state = state * 6364136223846793005ULL + 1442695040888963407ULL;
            pw[i] = (double)(state >> 33) / (double)(1ULL << 31) + 1.0; /* positive weights */
        }

        /* Inject ~5% NAs for the na.rm=TRUE test */
        double *px_na = (double*)malloc(l * sizeof(double));
        memcpy(px_na, px, l * sizeof(double));
        for(int i = 0; i < l; i += 20) px_na[i] = 0.0/0.0; /* NaN */

        /* --- na.rm=FALSE --- */
        TimingResult b0 = time_fn_double(baseline_double_narm0, px, l, iters);
        TimingResult o0 = time_fn_double(opt_double_narm0,      px, l, iters);
        printf("n=%-10d  %-25s  %10.1f  %10.1f  %7.2fx\n",
               l, "baseline  na.rm=FALSE", b0.min_us, b0.median_us, 1.0);
        printf("%-12s  %-25s  %10.1f  %10.1f  %7.2fx\n",
               "", "optimized na.rm=FALSE", o0.min_us, o0.median_us,
               b0.median_us / o0.median_us);

        /* --- na.rm=TRUE --- */
        TimingResult b1 = time_fn_double(baseline_double_narm1, px_na, l, iters);
        TimingResult o1 = time_fn_double(opt_double_narm1,      px_na, l, iters);
        printf("%-12s  %-25s  %10.1f  %10.1f  %7.2fx\n",
               "", "baseline  na.rm=TRUE",  b1.min_us, b1.median_us, 1.0);
        printf("%-12s  %-25s  %10.1f  %10.1f  %7.2fx\n",
               "", "optimized na.rm=TRUE",  o1.min_us, o1.median_us,
               b1.median_us / o1.median_us);

        /* --- weighted --- */
        TimingResult bw = time_fn_weighted(baseline_weighted, px, pw, l, iters);
        TimingResult ow = time_fn_weighted(opt_weighted,      px, pw, l, iters);
        printf("%-12s  %-25s  %10.1f  %10.1f  %7.2fx\n",
               "", "baseline  weighted",    bw.min_us, bw.median_us, 1.0);
        printf("%-12s  %-25s  %10.1f  %10.1f  %7.2fx\n",
               "", "optimized weighted",    ow.min_us, ow.median_us,
               bw.median_us / ow.median_us);

        printf("\n");
        free(px); free(pw); free(px_na);
    }
    return 0;
}
