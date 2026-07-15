#include "collapse_c.h"
#include "data.table.h"
// #ifndef USE_RINTERNALS
// #define USE_RINTERNALS
// #endif
// #include "base_radixsort.h"
#include <math.h>

void matCopyAttr(SEXP out, SEXP x, SEXP Rdrop, int ng) {
  SEXP dn = getAttrib(x, R_DimNamesSymbol);
  SEXP cn = isNull(dn) ? R_NilValue : VECTOR_ELT(dn, 1); // PROTECT ??
  if(ng == 0 && asLogical(Rdrop)) {
    if(length(cn)) setAttrib(out, R_NamesSymbol, cn);
  } else {
    int nprotect = 1;
    SEXP dim = PROTECT(duplicate(getAttrib(x, R_DimSymbol)));
    INTEGER(dim)[0] = ng == 0 ? 1 : ng;
    dimgets(out, dim);
    if(length(cn)) {
      ++nprotect;
      SEXP dn = PROTECT(allocVector(VECSXP, 2));
      SET_VECTOR_ELT(dn, 0, R_NilValue);
      SET_VECTOR_ELT(dn, 1, cn);
      dimnamesgets(out, dn);
    }
    if(!isObject(x)) copyMostAttrib(x, out);
    UNPROTECT(nprotect);
  }
}

void DFcopyAttr(SEXP out, SEXP x, int ng) {
  SHALLOW_DUPLICATE_ATTRIB(out, x);
  if(isObject(x)) { // No attributes for plain lists
    if(ng == 0) {
      setAttrib(out, R_RowNamesSymbol, ScalarInteger(1));
    } else {
      SEXP rn = PROTECT(allocVector(INTSXP, 2)); // Needed here, now unsafe to pass uninitialized vectors to R_RowNamesSymbol.
      INTEGER(rn)[0] = NA_INTEGER;
      INTEGER(rn)[1] = -ng;
      setAttrib(out, R_RowNamesSymbol, rn);
      UNPROTECT(1);
    }
  }
}

// Faster than rep_len(value, n) and slightly faster than matrix(value, n) (which in turn is faster than rep_len)...
SEXP falloc(SEXP value, SEXP n, SEXP simplify)  {
  int l = asInteger(n), tval = TYPEOF(value), isat = isVectorAtomic(value);
  if((length(value) > 1 && isat) || asLogical(simplify) == 0) {
    isat = 0;
    tval = VECSXP;
  }
  SEXP out = PROTECT(allocVector(isat ? tval : VECSXP, l));
  switch(tval) {
    case INTSXP:
    case LGLSXP: {
      int val = asInteger(value), *pout = INTEGER(out);
      if(val == 0) memset(pout, 0, l*sizeof(int));
      else for(int i = 0; i != l; ++i) pout[i] = val;
      break;
    }
    case REALSXP: {
      double val = asReal(value), *pout = REAL(out);
      if(val == 0.0) memset(pout, 0, l*sizeof(double));
      else for(int i = 0; i != l; ++i) pout[i] = val;
      break;
    }
    case STRSXP: {
      SEXP val = asChar(value), *pout = SEXPPTR(out);
      for(int i = 0; i != l; ++i) pout[i] = val;
      break;
    }
    case CPLXSXP: {
      Rcomplex val = asComplex(value), *pout = COMPLEX(out);
      for(int i = 0; i != l; ++i) pout[i] = val;
      break;
    }
    case RAWSXP: {
      Rbyte val = RAW(value)[0], *pout = RAW(out);
      for(int i = 0; i != l; ++i) pout[i] = val;
      break;
    }
    default: {
      SEXP *pout = SEXPPTR(out);
      if(asLogical(simplify) && tval == VECSXP && length(value) == 1)
        value = VECTOR_ELT(value, 0);
      for(int i = 0; i != l; ++i) pout[i] = value;
      break;
    }
  }
  if(isat) copyMostAttrib(value, out);
  UNPROTECT(1);
  return out;
}


SEXP groups2GRP(SEXP x, SEXP lx, SEXP gs) {
  int l = length(x);
  SEXP out = PROTECT(allocVector(INTSXP, asInteger(lx)));
  int *pout = INTEGER(out)-1, *pgs = INTEGER(gs);
  // SEXP *px = VECTOR_PTR(x); // -> Depreciated interface: https://github.com/hadley/r-internals/blob/ea892fa79bbffe961e78dbe9c90ce4ca3bf2d9bc/vectors.md
  // Matt Dowle Commented:
  // VECTOR_PTR does exist but returns 'not safe to return vector pointer' when USE_RINTERNALS is not defined.
  // VECTOR_DATA and LIST_POINTER exist too but call VECTOR_PTR. All are clearly not intended to be used by packages.
  // The concern is overhead inside VECTOR_ELT() biting when called repetitively in a loop like we do here. That's why
  // we take the R API (INTEGER()[i], REAL()[i], etc) outside loops for the simple types even when not parallel. For this
  // type list case (VECSXP) it might be that some items are ALTREP for example, so we really should use the heavier
  // _ELT accessor (VECTOR_ELT) inside the loop in this case.
  const SEXP *px = SEXPPTR_RO(x);

  for(int j = 0; j != l; ++j) { // This can go in any direction..
    // SEXP column = VECTOR_ELT(x, j);
    int *pcolumn = INTEGER(px[j]), jp = j+1;
    for(int i = pgs[j]; i--; ) pout[pcolumn[i]] = jp; // This can go in any direction...
  }
  UNPROTECT(1);
  return out;
}

// Note: Only supports numeric data!!!!
SEXP lassign(SEXP x, SEXP s, SEXP rows, SEXP fill) {
  int l = length(x), tr = TYPEOF(rows), ss = asInteger(s), rs = LENGTH(rows);
  SEXP out = PROTECT(allocVector(VECSXP, l));
  // SEXP *px = VECTOR_PTR(x); // -> Depreciated interface: https://github.com/hadley/r-internals/blob/ea892fa79bbffe961e78dbe9c90ce4ca3bf2d9bc/vectors.md
  const SEXP *px = SEXPPTR_RO(x);
  double dfill = asReal(fill);

  if(tr == INTSXP) {
    int *rowsv = INTEGER(rows); //, vs = ss * sizeof(double);
    for(int j = l; j--; ) {
      SEXP column = px[j]; // VECTOR_ELT(x, j);
      if(length(column) != rs) error("length(rows) must match nrow(x)");
      SEXP outj;
      SET_VECTOR_ELT(out, j, outj = allocVector(REALSXP, ss));
      double *pcolumn = REAL(column), *poutj = REAL(outj);
      // memset(poutj, dfill, vs); // cannot memset missing values... can only memset 0
      for(int i = ss; i--; ) poutj[i] = dfill;
      for(int i = 0; i != rs; ++i) poutj[rowsv[i]-1] = pcolumn[i];
      SHALLOW_DUPLICATE_ATTRIB(outj, column);
    }
  } else if(tr == LGLSXP) {
    int *rowsv = LOGICAL(rows);
    if(ss != rs) error("length(rows) must match length(s) if rows is a logical vector");
    for(int j = l; j--; ) {
      SEXP column = px[j]; // VECTOR_ELT(x, j);
      SEXP outj;
      SET_VECTOR_ELT(out, j, outj = allocVector(REALSXP, ss));
      double *pcolumn = REAL(column), *poutj = REAL(outj);
      for(int i = 0, k = 0; i != rs; ++i) poutj[i] = rowsv[i] ? pcolumn[k++] : dfill;
      SHALLOW_DUPLICATE_ATTRIB(outj, column);
    }
  } else error("rows must be positive integers or a logical vector");
  SHALLOW_DUPLICATE_ATTRIB(out, x);
  UNPROTECT(1);
  return out;
}

SEXP gwhich_first(SEXP x, SEXP g, SEXP target) {
  if(!inherits(g, "GRP")) error("Internal error: g must be an object of class 'GRP'.");
  const int ng = asInteger(VECTOR_ELT(g, 0)), *pg = INTEGER_RO(VECTOR_ELT(g, 1)), l = length(VECTOR_ELT(g, 1));
  if(l != length(x)) error("length(x) must match length(g).");
  if(ng != length(target)) error("length(target) must match number of groups.");
  if(TYPEOF(x) != TYPEOF(target)) error("x is of type %s whereas target is of type %s.", type2char(TYPEOF(x)), type2char(TYPEOF(target)));

  SEXP res = PROTECT(allocVector(INTSXP, ng));
  if(ng == 0) {
    UNPROTECT(1);
    return res;
  }
  memset(INTEGER(res), 0, ng*sizeof(int));
  int *pres = INTEGER(res)-1;

  switch(TYPEOF(x)) {
    case INTSXP:
    case LGLSXP: {
      const int *px = INTEGER_RO(x), *pt = INTEGER_RO(target)-1;
      for(int i = 0; i != l; ++i) if(pres[pg[i]] == 0 && px[i] == pt[pg[i]]) pres[pg[i]] = i+1;
      break;
    }
    case REALSXP: {
      const double *px = REAL_RO(x), *pt = REAL_RO(target)-1;
      for(int i = 0; i != l; ++i) if(pres[pg[i]] == 0 && (px[i] == pt[pg[i]] || (ISNAN(px[i]) && ISNAN(pt[pg[i]])))) pres[pg[i]] = i+1;
      break;
    }
    case STRSXP: {
      const SEXP *px = STRING_PTR_RO(x), *pt = STRING_PTR_RO(target)-1;
      for(int i = 0; i != l; ++i) if(pres[pg[i]] == 0 && px[i] == pt[pg[i]]) pres[pg[i]] = i+1;
      break;
    }
    default: error("Unsupported type %s", type2char(TYPEOF(x)));
  }

  UNPROTECT(1);
  return res;
}

SEXP gslice_multi(SEXP g, SEXP o, SEXP Rn, SEXP first)  {
  if(!inherits(g, "GRP")) error("Internal error: g must be an object of class 'GRP'.");
  const int n = asInteger(Rn), ng = asInteger(VECTOR_ELT(g, 0)), l = length(VECTOR_ELT(g, 1)),
    *pg = INTEGER_RO(VECTOR_ELT(g, 1)), *pgs = INTEGER_RO(VECTOR_ELT(g, 2));

  int lvec = 0;
  #pragma omp simd reduction(+:lvec)
  for(int i = 0; i < ng; ++i) lvec += n <= pgs[i] ? n : pgs[i];

  SEXP res = PROTECT(allocVector(INTSXP, lvec));
  int *sizes = (int*)R_Calloc(ng+1, int);
  int *pres = INTEGER(res);

  if(isNull(o)) {
    if(asLogical(first)) {
      for(int i = 0, k = 0; i != l; ++i) if(n > sizes[pg[i]]++) pres[k++] = i+1;
    } else {
      for(int i = l, k = lvec; i--; ) if(n > sizes[pg[i]]++) pres[--k] = i+1;
    }
  } else {
    if(length(o) != l) error("length(o) must match length(g)");
    const int *po = INTEGER(o);
    if(asLogical(first)) {
      for(int i = 0, k = 0; i != l; ++i) if(n > sizes[pg[po[i]-1]]++) pres[k++] = po[i];
    } else {
      for(int i = l, k = lvec; i--; ) if(n > sizes[pg[po[i]-1]]++) pres[--k] = po[i];
    }
  }

  R_Free(sizes);
  UNPROTECT(1);
  return res;
}

// SEXP CasChar(SEXP x) {
//  return coerceVector(x, STRSXP);
// }

/* Inspired by:
 * do_list2env : .Internal(list2env(x, envir))
 */
SEXP multiassign(SEXP lhs, SEXP rhs, SEXP envir) {
  if(TYPEOF(lhs) != STRSXP) error("lhs needs to be character");
  int n = length(lhs);
  if(n == 1) { // lazy_duplicate appears not necessary (copy-on modify is automatically implemented, and <- also does not use it).
    SEXP nam = installChar(STRING_ELT(lhs, 0));
    defineVar(nam, rhs, envir);
    return R_NilValue;
  }
  if(length(rhs) != n) error("length(lhs) must be equal to length(rhs)");
  const SEXP *plhs = SEXPPTR_RO(lhs);
  switch(TYPEOF(rhs)) { // installTrChar translates to native encoding, installChar does the same now, but also is available on older systems.
    case REALSXP: {
      double *prhs = REAL(rhs);
      for(int i = 0; i < n; ++i) {
        SEXP nam = installChar(plhs[i]);
        defineVar(nam, ScalarReal(prhs[i]), envir);
      }
      break;
    }
    case INTSXP: {
      int *prhs = INTEGER(rhs);
      for(int i = 0; i < n; ++i) {
        SEXP nam = installChar(plhs[i]);
        defineVar(nam, ScalarInteger(prhs[i]), envir);
      }
      break;
    }
    case STRSXP: {
      const SEXP *prhs = SEXPPTR_RO(rhs);
      for(int i = 0; i < n; ++i) {
        SEXP nam = installChar(plhs[i]);
        defineVar(nam, ScalarString(prhs[i]), envir);
      }
      break;
    }
    case LGLSXP: {
      int *prhs = LOGICAL(rhs);
      for(int i = 0; i < n; ++i) {
        SEXP nam = installChar(plhs[i]);
        defineVar(nam, ScalarLogical(prhs[i]), envir);
      }
      break;
    }
    case VECSXP: { // lazy_duplicate appears not necessary (copy-on modify is automatically implemented, and <- also does not use it).
      for(int i = 0; i < n; ++i) {
        SEXP nam = installChar(plhs[i]);
        defineVar(nam, VECTOR_ELT(rhs, i), envir);
      }
      break;
    }
    default: {
      SEXP rhsl = PROTECT(coerceVector(rhs, VECSXP));
      for(int i = 0; i < n; ++i) {
        SEXP nam = installChar(plhs[i]);
        defineVar(nam, VECTOR_ELT(rhsl, i), envir);
      }
      UNPROTECT(1);
    }
  }
  return R_NilValue;
}


SEXP vlabels(SEXP x, SEXP attrn, SEXP usenam) {
  if(!isString(attrn)) error("'attrn' must be of mode character");
  if(length(attrn) != 1) error("exactly one attribute 'attrn' must be given");
  SEXP sym_attrn = PROTECT(installChar(STRING_ELT(attrn, 0)));
  int l = length(x);
  if(TYPEOF(x) != VECSXP) {
    SEXP labx = getAttrib(x, sym_attrn);
    UNPROTECT(1);
    if(labx == R_NilValue) return ScalarString(NA_STRING);
    return labx;
  }
  SEXP res = PROTECT(allocVector(STRSXP, l));
  SEXP *pres = SEXPPTR(res);
  const SEXP *px = SEXPPTR_RO(x);
  for(int i = 0; i < l; ++i) {
    SEXP labxi = getAttrib(px[i], sym_attrn);
    if(TYPEOF(labxi) == STRSXP) pres[i] = STRING_ELT(labxi, 0);
    else if(labxi == R_NilValue) pres[i] = NA_STRING;
    else {
      PROTECT(labxi);
      pres[i] = asChar(labxi);
      UNPROTECT(1);
    }
  }
  if(asLogical(usenam)) {
    SEXP nam = getAttrib(x, R_NamesSymbol);
    if(TYPEOF(nam) != NILSXP) namesgets(res, nam);
  }
  UNPROTECT(2);
  return res;
}

// Note: ind can be NULL...
SEXP setvlabels(SEXP x, SEXP attrn, SEXP value, SEXP ind) { // , SEXP sc
 if(!isString(attrn)) error("'attrn' must be of mode character");
 if(length(attrn) != 1) error("exactly one attribute 'attrn' must be given");
 if(TYPEOF(x) != VECSXP) error("X must be a list");
 int nprotect = 1, l = length(x), tv = TYPEOF(value); // , scl = asLogical(sc);
 const SEXP *px = SEXPPTR_RO(x); // , xsc;
 // if(scl) { // Create shallow copy
 //   if(INHERITS(x, char_datatable)) {
 //     xsc = PROTECT(Calloccol(x));
 //   } else {
 //     xsc = PROTECT(shallow_duplicate(x));
 //   }
 //   ++nprotect;
 //   px = SEXPPTR(xsc);
 // }
 const SEXP *pv = px;
 if(tv != NILSXP) {
   if(tv == VECSXP || tv == STRSXP) {
    pv = SEXPPTR_RO(value);
   } else {
    SEXP vl = PROTECT(coerceVector(value, VECSXP));
    pv = SEXPPTR_RO(vl); ++nprotect;
   }
 }
 SEXP sym_attrn = PROTECT(installChar(STRING_ELT(attrn, 0)));
 if(length(ind) == 0) {
   if(tv != NILSXP && l != length(value)) error("length(x) must match length(value)");
   if(tv == NILSXP) {
     for(int i = 0; i < l; ++i) setAttrib(px[i], sym_attrn, R_NilValue);
   } else if(tv == STRSXP) {
     for(int i = 0; i < l; ++i) setAttrib(px[i], sym_attrn, ScalarString(pv[i]));
   } else {
     for(int i = 0; i < l; ++i) setAttrib(px[i], sym_attrn, pv[i]);
   }
 } else {
   if(TYPEOF(ind) != INTSXP) error("vlabels<-: ind must be of type integer");
   int li = length(ind), *pind = INTEGER(ind), ii;
   if(tv != NILSXP && li != length(value)) error("length(ind) must match length(value)");
   if(li == 0 || li > l) error("vlabels<-: length(ind) must be > 0 and <= length(x)");
   if(tv == NILSXP) {
     for(int i = 0; i < li; ++i) {
       ii = pind[i]-1;
       if(ii < 0 || ii >= l) error("vlabels<-: ind must be between 1 and length(x)");
       setAttrib(px[ii], sym_attrn, R_NilValue);
     }
   } else if(tv == STRSXP) {
     for(int i = 0; i < li; ++i) {
       ii = pind[i]-1;
       if(ii < 0 || ii >= l) error("vlabels<-: ind must be between 1 and length(x)");
       setAttrib(px[ii], sym_attrn, ScalarString(pv[i]));
     }
   } else {
     for(int i = 0; i < li; ++i) {
       ii = pind[i]-1;
       if(ii < 0 || ii >= l) error("vlabels<-: ind must be between 1 and length(x)");
       setAttrib(px[ii], sym_attrn, pv[i]);
     }
   }
 }
 UNPROTECT(nprotect);
 // return scl ? xsc : x;
 return x;
}


SEXP Cissorted(SEXP x, SEXP strictly) {
   return ScalarLogical(FALSE == isUnsorted(x, (Rboolean)asLogical(strictly)));
}

SEXP fcrosscolon(SEXP x, SEXP ngp, SEXP y, SEXP ckna) {
  int l = length(x), narm = asLogical(ckna);
  if(l != length(y)) error("length mismatch");
  if(TYPEOF(x) != INTSXP) error("x needs to be integer");
  if(TYPEOF(y) != INTSXP) error("y needs to be integer");
  int ng = asInteger(ngp), *px = INTEGER(x), *py = INTEGER(y);
  if(ng > INT_MAX / 2) error("Table larger than INT_MAX/2");

  if(narm) {
    for(int i = 0; i != l; ++i) {
      if(px[i] != NA_INTEGER) {
        if(py[i] == NA_INTEGER) px[i] = NA_INTEGER;
        else px[i] += (py[i] - 1) * ng;
      }
    }
  } else {
    for(int i = 0; i != l; ++i) px[i] += (py[i] - 1) * ng;
  }

  return R_NilValue;
}

SEXP fwtabulate(SEXP x, SEXP w, SEXP ngp, SEXP ckna) {
  int l = length(x), narm = asLogical(ckna), ng = asInteger(ngp), nwl = isNull(w);
  if(TYPEOF(x) != INTSXP) error("x needs to be integer");
  // if(ng > INT_MAX/2) error("Table larger than INT_MAX/2");

  SEXP tab = PROTECT(allocVector(nwl ? INTSXP : REALSXP, ng));
  int *px = INTEGER(x);

  if(nwl) {
    int *ptab = INTEGER(tab);
    memset(ptab, 0, sizeof(int) * ng); --ptab;
    if(narm) {
      for(int i = 0; i != l; ++i) if(px[i] != NA_INTEGER) ++ptab[px[i]];
    } else {
      for(int i = 0; i != l; ++i) ++ptab[px[i]];
    }
  } else {
    if(length(w) != l) error("length(w) must be equal to length(x)");
    double *ptab = REAL(tab);
    memset(ptab, 0.0, sizeof(double) * ng); --ptab;
    switch(TYPEOF(w)) {
      case REALSXP: {
        double *pw = REAL(w);
        if(narm) {
          for(int i = 0; i != l; ++i) if(px[i] != NA_INTEGER && NISNAN(pw[i])) ptab[px[i]] += pw[i];
        } else {
          for(int i = 0; i != l; ++i) if(NISNAN(pw[i])) ptab[px[i]] += pw[i];
        }
        break;
      }
      case INTSXP:
      case LGLSXP: {
        int *pw = INTEGER(w);
        if(narm) {
          for(int i = 0; i != l; ++i) if(px[i] != NA_INTEGER && pw[i] != NA_INTEGER) ptab[px[i]] += pw[i];
        } else {
          for(int i = 0; i != l; ++i) if(pw[i] != NA_INTEGER) ptab[px[i]] += pw[i];
        }
        break;
      }
      default: error("Unsupported weights type!");
    }
  }

  UNPROTECT(1);
  return tab;
}

// ---------------------------------------------------------------------------
// GRP.default with drop = FALSE: build full Cartesian product of factor levels
// (and observed unique values for non-factor columns) as the grouping universe.
//
// X:        original list / data.frame (used for class preservation on groups)
// cols:     list of grouping columns (already subset from X by `by`)
// namby:    character vector of names for the groups data.frame
// retgrp_:  scalar logical, whether to return the groups data.frame
//
// Returns list(N.groups, group.id, group.sizes, group.starts, groups).
// `group.id` is 1-based and free of NAs (NAs become explicit factor levels).
// `group.sizes`/`group.starts` have length Ng (= product of per-column sizes),
// with 0 in `group.sizes` and 0 in `group.starts` for combinations not observed.
// ---------------------------------------------------------------------------
SEXP GRP_default_drop_C(SEXP X, SEXP cols, SEXP namby, SEXP retgrp_) {
  if(TYPEOF(cols) != VECSXP) error("Internal error: cols must be a list");
  int nc = length(cols), retgrp = asLogical(retgrp_);
  if(nc == 0) error("GRP.default(drop = FALSE) requires at least one grouping column");

  int n = length(VECTOR_ELT(cols, 0));
  int nprotect = 0;
  SEXP codes_list = PROTECT(allocVector(VECSXP, nc)); ++nprotect; // per-col 1-based codes
  SEXP levs_list  = PROTECT(allocVector(VECSXP, nc)); ++nprotect; // per-col level/unique-value vectors
  int *isfac = (int*) R_alloc(nc, sizeof(int));
  int *pnl   = (int*) R_alloc(nc, sizeof(int));

  long long Ng_ll = 1;
  for(int j = 0; j < nc; ++j) {
    SEXP col = VECTOR_ELT(cols, j);
    if(length(col) != n) error("All grouping columns must have equal length");

    if(isFactor(col)) {
      isfac[j] = 1;
      SEXP lev = getAttrib(col, R_LevelsSymbol);
      int nl = length(lev);
      const int *pcol = INTEGER_RO(col);

      // Does the factor already have an explicit NA level? Does the column contain NAs?
      int hasNAlev = 0, na_lev_pos = 0;
      for(int k = 0; k < nl; ++k) {
        if(STRING_ELT(lev, k) == NA_STRING) { hasNAlev = 1; na_lev_pos = k + 1; break; }
      }
      int colHasNA = 0;
      for(int i = 0; i < n; ++i) if(pcol[i] == NA_INTEGER) { colHasNA = 1; break; }

      if(colHasNA) {
        // Remap NA codes to an NA level (adding one if necessary). Mirrors addNA2().
        int nl_new = hasNAlev ? nl : nl + 1;
        int na_code = hasNAlev ? na_lev_pos : nl_new;
        SEXP new_lev = lev;
        if(!hasNAlev) {
          new_lev = PROTECT(allocVector(STRSXP, nl_new)); ++nprotect;
          for(int k = 0; k < nl; ++k) SET_STRING_ELT(new_lev, k, STRING_ELT(lev, k));
          SET_STRING_ELT(new_lev, nl, NA_STRING);
        }
        SEXP codes = PROTECT(allocVector(INTSXP, n)); ++nprotect;
        int *pcodes = INTEGER(codes);
        for(int i = 0; i < n; ++i) pcodes[i] = pcol[i] == NA_INTEGER ? na_code : pcol[i];
        SET_VECTOR_ELT(codes_list, j, codes);
        SET_VECTOR_ELT(levs_list,  j, new_lev);
        pnl[j] = nl_new;
      } else {
        // Use factor codes / levels as-is (no allocation)
        SET_VECTOR_ELT(codes_list, j, col);
        SET_VECTOR_ELT(levs_list,  j, lev);
        pnl[j] = nl;
      }
    } else {
      isfac[j] = 0;
      // Non-factor: hash the column. groupVec returns 1-based codes; NAs are
      // assigned a dedicated code (na.included), so combined IDs are NA-free too.
      SEXP g = PROTECT(groupVec(col, retgrp ? ScalarLogical(1) : ScalarLogical(0), ScalarLogical(0)));
      ++nprotect;
      int ng = asInteger(getAttrib(g, sym_n_groups));
      SET_VECTOR_ELT(codes_list, j, g);
      pnl[j] = ng;
      if(retgrp) {
        // Unique values in hash-encounter order = col[starts]
        SEXP starts = getAttrib(g, sym_starts);
        SEXP uvals  = PROTECT(allocVector(TYPEOF(col), ng)); ++nprotect;
        subsetVectorRaw(uvals, col, starts, /*anyNA=*/ false);
        copyMostAttrib(col, uvals);
        SET_VECTOR_ELT(levs_list, j, uvals);
      }
    }

    Ng_ll *= pnl[j];
    if(Ng_ll > INT_MAX)
      error("Total number of group combinations (%lld) exceeds INT_MAX. Consider using drop = TRUE.", Ng_ll);
  }
  int Ng = (int) Ng_ll;

  // --- Combined group.id via stride arithmetic (cf. fcrosscolon) ---
  SEXP gid = PROTECT(allocVector(INTSXP, n)); ++nprotect;
  int *pgid = INTEGER(gid);
  const int *p0 = INTEGER_RO(VECTOR_ELT(codes_list, 0));
  memcpy(pgid, p0, sizeof(int) * n);
  long long stride = pnl[0];
  for(int j = 1; j < nc; ++j) {
    const int *pj = INTEGER_RO(VECTOR_ELT(codes_list, j));
    int s = (int) stride;
    for(int i = 0; i < n; ++i) pgid[i] += (pj[i] - 1) * s;
    stride *= pnl[j];
  }

  // --- group.sizes (cf. fwtabulate) and group.starts (first occurrence) ---
  SEXP gs  = PROTECT(allocVector(INTSXP, Ng)); ++nprotect;
  SEXP gst = PROTECT(allocVector(INTSXP, Ng)); ++nprotect;
  int *pgs = INTEGER(gs), *pgst = INTEGER(gst);
  memset(pgs,  0, sizeof(int) * Ng);
  memset(pgst, 0, sizeof(int) * Ng);
  for(int i = 0; i < n; ++i) {
    int g = pgid[i] - 1;
    ++pgs[g];
    if(pgst[g] == 0) pgst[g] = i + 1;
  }

  // --- groups data.frame: enumerate every combination in column-major order ---
  SEXP groups = R_NilValue;
  if(retgrp) {
    PROTECT(groups = allocVector(VECSXP, nc)); ++nprotect;
    long long stride_j = 1;
    for(int j = 0; j < nc; ++j) {
      int nlj = pnl[j], sj = (int) stride_j;
      SEXP lev = VECTOR_ELT(levs_list, j);

      if(isfac[j]) {
        // Build a fresh factor with the same levels and class as the input column
        SEXP newcol = PROTECT(allocVector(INTSXP, Ng));
        int *pnew = INTEGER(newcol);
        for(int g = 0; g < Ng; ++g) pnew[g] = (g / sj) % nlj + 1;
        setAttrib(newcol, R_LevelsSymbol, lev);
        SEXP origcol = VECTOR_ELT(cols, j);
        SEXP cls = getAttrib(origcol, R_ClassSymbol);
        setAttrib(newcol, R_ClassSymbol, cls);
        SET_VECTOR_ELT(groups, j, newcol);
        UNPROTECT(1);
      } else {
        // Non-factor: subset the per-column unique values by the index vector
        SEXP idx = PROTECT(allocVector(INTSXP, Ng));
        int *pidx = INTEGER(idx);
        for(int g = 0; g < Ng; ++g) pidx[g] = (g / sj) % nlj + 1;
        SEXP newcol = PROTECT(allocVector(TYPEOF(lev), Ng));
        subsetVectorRaw(newcol, lev, idx, /*anyNA=*/ false);
        copyMostAttrib(lev, newcol);
        SET_VECTOR_ELT(groups, j, newcol);
        UNPROTECT(2);
      }
      stride_j *= nlj;
    }
    // Names and (if applicable) class of groups data.frame: copy class etc. from X
    // (like C_subsetDT). Only add data.frame row.names if X is itself a data.frame,
    // matching the drop = TRUE behaviour for plain list inputs.
    namesgets(groups, namby);
    if(inherits(X, "data.frame")) {
      if(isObject(X)) copyMostAttrib(X, groups);
      SEXP rn = PROTECT(allocVector(INTSXP, 2));
      INTEGER(rn)[0] = NA_INTEGER;
      INTEGER(rn)[1] = -Ng;
      setAttrib(groups, R_RowNamesSymbol, rn);
      UNPROTECT(1);
    }
  }

  // --- Assemble result list ---
  SEXP res = PROTECT(allocVector(VECSXP, 5)); ++nprotect;
  SET_VECTOR_ELT(res, 0, ScalarInteger(Ng));
  SET_VECTOR_ELT(res, 1, gid);
  SET_VECTOR_ELT(res, 2, gs);
  SET_VECTOR_ELT(res, 3, gst);
  SET_VECTOR_ELT(res, 4, groups);
  UNPROTECT(nprotect);
  return res;
}

// Recursive function: doesn't work in C99 Standard
// int fgcd(int a, int b) {
//    if(b == 0) return a;
//    else return fcgd(b, a % b);
// }

// https://www.datamentor.io/r-programming/examples/gcd-hcf/
// https://stackoverflow.com/questions/7500128/how-to-use-operator-for-float-values-in-c
// https://www.tutorialspoint.com/find-out-the-gcd-of-two-numbers-using-while-loop-in-c-language
static inline double dgcd(double a, double b) {
  double rem;
  while(b > 0.000001) // check for b>0 condition because in a % b, b should not equal to zero
  {
    rem = fmod(a, b);
    a = b;
    b = rem;
  }
  return a;
}

static inline int igcd(int a, int b) {
  int rem;
  while(b != 0) // check for b!=0 condition because in a % b, b should not equal to zero
  {
    rem = a % b;
    a = b;
    b = rem;
  }
  return a;
}

// See as_double_integer64 at https://github.com/truecluster/bit64/blob/master/src/integer64.c
// static inline long long i64gcd(long long a, long long b) {
//   long long rem;
//   while(b != 0) // check for b!=0 condition because in a % b, b should not equal to zero
//   {
//     rem = a % b;
//     a = b;
//     b = rem;
//   }
//   return a;
// }

// Greatest common divisor of a vector of numeric values
// Note that the function expects positive values only (use abs() in R beforehand)
// Also best to sort values before entering this function. For example c(0.25, 0) gives 0.25, not 0
SEXP vecgcd(SEXP x) {

  int n = length(x);
  if(n == 1) return x;

  switch(TYPEOF(x)) {
    case INTSXP:
    case LGLSXP:
    {
      int *px = INTEGER(x), gcd = px[0];
      for(int i = 1; i < n; ++i) {
        if(gcd <= 1) break;
        gcd = igcd(px[i], gcd);
      }
      if(gcd == 0) return ScalarInteger(1);
      return ScalarInteger(gcd);
      // fixest solution: https://github.com/lrberge/fixest/blob/master/src/misc_funs.cpp
      // int *px = INTEGER(x), gcd = px[0], ok = 0;
      // for(int i = 1; i < n; ++i) if(gcd > px[i]) gcd = px[i];
      // while(ok == 0 && gcd > 1) {
      //   ok = 1;
      //   for(int i = 0; i < n; ++i) {
      //     if(px[i] % gcd != 0) {
      //       gcd--;
      //       ok = 0;
      //       break;
      //     }
      //   }
      // }
    }
    case REALSXP:
    {
      if(inherits(x, "integer64")) error("vgcd does not support integer64. Please convert your vector to double using as.double(x).");
      // if(inherits(x, "integer64")) {
      //   long long *px = (long long *)REAL(x), gcd = px[0];
      //   for(int i = 1; i < n; ++i) {
      //     if(gcd <= 1) break;
      //     gcd = i64gcd(px[i], gcd);
      //   }
      //   SEXP res = gcd == 0 ? ScalarReal(1) : ScalarReal((double)gcd);
      //   copyMostAttrib(x, res);
      //   return res;
      // }
      // TODO: Check if double is integer?
      double *px = REAL(x), gcd = px[0];
      for(int i = 1; i < n; ++i) {
        if(gcd < 0.000001) break;
        gcd = dgcd(px[i], gcd);
      }
      if(gcd < 0.000001) error("GCD is approximately zero");
      return ScalarReal(round(gcd * 1000000) / 1000000);
    }
    default: error("Greatest Common Divisor can only be calculated with integer or numeric data");
  }
  return R_NilValue;
}

// Adapted from https://github.com/wch/r-source/blob/79298c499218846d14500255efd622b5021c10ec/src/main/list.c
/* The following code is used to recursive traverse a block */
/* of code and extract all the function calls present in that code. */

typedef struct {
  SEXP ans;
  int	StoreValues;
  int	ItemCounts;
} FunsWalkData;

static void funswalk(SEXP s, FunsWalkData *d) {
  SEXP name;
  switch(TYPEOF(s)) {
  case SYMSXP:
    name = PRINTNAME(s);
    if(CHAR(name)[0] != '\0') { /* skip blank symbols */
      if(d->StoreValues) SET_STRING_ELT(d->ans, d->ItemCounts, name);
      d->ItemCounts++;
    }
    break;
  case LANGSXP: // https://github.com/hadley/r-internals/blob/ea892fa79bbffe961e78dbe9c90ce4ca3bf2d9bc/pairlists.md
    while(s != R_NilValue) {
      funswalk(CAR(s), d);
      if(TYPEOF(CADR(s)) != LANGSXP) s = CDR(s);
      if(TYPEOF(CADR(s)) != LANGSXP) break;
      s = CDR(s);
    }
    break;
  default: /* it seems the intention is to do nothing here! */
    break;
  }
}

SEXP all_funs(SEXP x) {

  if(TYPEOF(x) != LANGSXP) return allocVector(STRSXP, 0);
  SEXP expr = x;
  int i, savecount;
  FunsWalkData data = {NULL, 0, 0};

  funswalk(expr, &data);
  savecount = data.ItemCounts;

  data.ans = allocVector(STRSXP, data.ItemCounts);

  data.StoreValues = 1;
  data.ItemCounts = 0;
  funswalk(expr, &data);

  if(data.ItemCounts != savecount) {
    PROTECT(expr = data.ans);
    data.ans = allocVector(STRSXP, data.ItemCounts);
    for(i = 0 ; i < data.ItemCounts ; i++)
      SET_STRING_ELT(data.ans, i, STRING_ELT(expr, i));
    UNPROTECT(1);
  }

  return data.ans;
}

SEXP fnrowC(SEXP x) {
  if(TYPEOF(x) == VECSXP) return ScalarInteger(length(x) ? length(VECTOR_ELT(x, 0)) : 0);
  SEXP dim = getAttrib(x, R_DimSymbol);
  if(TYPEOF(dim) != INTSXP) return R_NilValue;
  return ScalarInteger(INTEGER(dim)[0]);
}

// Taken from: https://github.com/r-lib/rlang/blob/main/src/internal/env.c
#define CLP_FRAME_LOCK_MASK (1 << 14)
#define CLP_FRAME_IS_LOCKED(e) (MYEFL(e) & CLP_FRAME_LOCK_MASK)
#define CLP_UNLOCK_FRAME(e) MYSEFL(e, MYEFL(e) & (~CLP_FRAME_LOCK_MASK))

SEXP unlock_collapse_namespace(SEXP env) {
  if(TYPEOF(env) != ENVSXP) error("Unsupported object passed to C_unlock_collapse_namespace: %s", type2char(TYPEOF(env)));
  CLP_UNLOCK_FRAME(env);
  R_unLockBinding(install(".FAST_STAT_FUN_EXT"), env);
  R_unLockBinding(install(".FAST_STAT_FUN_POLD"), env);
  R_unLockBinding(install(".FAST_FUN_MOPS"), env);
  R_unLockBinding(install(".COLLAPSE_ALL_EXPORTS"), env);
  return CLP_FRAME_IS_LOCKED(env) == 0 ? ScalarLogical(1) : ScalarLogical(0);
}


SEXP integer64toREAL(SEXP x) {
  int n = length(x);

  SEXP out = PROTECT(allocVector(REALSXP, n));
  double* restrict p_out = REAL(out);
  const int64_t *p_x = INTEGER64_PTR_RO(x);

  #pragma omp simd
  for (int i = 0; i < n; ++i) {
    p_out[i] = p_x[i] == NA_INTEGER64 ? NA_REAL : (double)p_x[i];
  }

  UNPROTECT(1);
  return out;
}

SEXP funlist(SEXP x) {

  if(TYPEOF(x) != VECSXP) return x;
  int l = length(x);
  if(l < 1) return R_NilValue;
  if(l == 1) return VECTOR_ELT(x, 0);

  int n = 0, nt = 0, mt = 0, elem = 0, nprotect = 0;
  const SEXP *px = SEXPPTR_RO(x);

  // Sum lengths and determine maximum type
  int *types = (int*)R_Calloc(27, int);
  for(int i = 0; i < l; ++i) {
    n += length(px[i]);
    ++types[TYPEOF(px[i])];
  }
  for(int i = 0; i < 27; ++i) {
    if(types[i] > 0) {
      mt = i; ++nt;
    }
  }
  R_Free(types);

  // If more than one type: need to coerce to largest type
  if(nt > 1) {
    SEXP y = PROTECT(allocVector(VECSXP, l)); ++nprotect;
    for(int i = 0; i < l; ++i) {
      if(TYPEOF(px[i]) == mt) {
        elem = i;
        SET_VECTOR_ELT(y, i, px[i]);
      } else SET_VECTOR_ELT(y, i, coerceVector(px[i], mt));
    }
    px = SEXPPTR_RO(y);
  }

  // Now unlisting
  SEXP res = PROTECT(allocVector(mt, n)); ++nprotect; n = 0;

  switch(mt) {
    case INTSXP:
    case LGLSXP: {
      int *pres = INTEGER(res);
      for(int i = 0; i != l; ++i) {
        nt = length(px[i]);
        const int *pxi = INTEGER_RO(px[i]);
        for (int j = 0; j != nt; ++j) pres[n++] = pxi[j];
      }
      break;
    }
    case REALSXP: {
      double *pres = REAL(res);
      for(int i = 0; i != l; ++i) {
        nt = length(px[i]);
        const double *pxi = REAL_RO(px[i]);
        for (int j = 0; j != nt; ++j) pres[n++] = pxi[j];
      }
      break;
    }
    case STRSXP:
    case VECSXP:
    case EXPRSXP: {
      SEXP *pres = SEXPPTR(res);
      for(int i = 0; i != l; ++i) {
        nt = length(px[i]);
        const SEXP *pxi = SEXPPTR_RO(px[i]);
        for (int j = 0; j != nt; ++j) pres[n++] = pxi[j];
      }
      break;
    }
    case CPLXSXP: {
      Rcomplex *pres = COMPLEX(res);
      for(int i = 0; i != l; ++i) {
        nt = length(px[i]);
        const Rcomplex *pxi = COMPLEX_RO(px[i]);
        for (int j = 0; j != nt; ++j) pres[n++] = pxi[j];
      }
      break;
    }
    case RAWSXP: {
      Rbyte *pres = RAW(res);
      for(int i = 0; i != l; ++i) {
        nt = length(px[i]);
        const Rbyte *pxi = RAW_RO(px[i]);
        for (int j = 0; j != nt; ++j) pres[n++] = pxi[j];
      }
      break;
    }
    default: error("unsupported type: %s", type2char(mt));
  }

  if(isObject(px[elem])) copyMostAttrib(px[elem], res);
  UNPROTECT(nprotect);
  return res;
}
