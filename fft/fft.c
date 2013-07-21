#include <ruby.h>
#include "narray.h"
#include "template.h"

/*
  Fast Fourier/Cosine/Sine Transform
  using program code developed by Takuya OOURA.
  http://www.kurims.kyoto-u.ac.jp/~ooura/fft-j.html
*/

typedef dcomplex ctype;
typedef double   rtype;

typedef struct {
    rtype *a;
    rtype *t;
    int   *ip;
    rtype *w;
    int    sign;
} fft_opt_t;

static VALUE rb_mFFT;
static VALUE eRadixError;

void cdft(int n2, int isig, double *a, int *ip, rtype *w);
void rdft(int, int, double *, int *, double *);
void ddct(int, int, double *, int *, double *);
void ddst(int, int, double *, int *, double *);
void dfct(int, double *, double *, int *, double *);
void dfst(int, double *, double *, int *, double *);

static void
iter_fft_cdft(na_loop_t *const lp)
{
    size_t  i, n;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dcomplex  x;
    dcomplex *a;
    fft_opt_t *g;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    g = (fft_opt_t*)(lp->opt_ptr);
    a = (dcomplex*)(g->a);
    for (i=0; i<n; i++) {
        LOAD_DATA_STEP(p1, s1, idx1, dcomplex, x);
        a[i] = x;
    }

    cdft(n*2, g->sign, (double*)a, g->ip, g->w);

    for (i=0; i<n; i++) {
        x = a[i];
        STORE_DATA_STEP(p2, s2, idx2, dcomplex, x);
    }
}

/*
  Complex DFT (Discrete Fourier Transform),
  Fast Version (Split-Radix).
  @overload cdft(narray,[sign])
  @param [NArray::DComplex] narray
  @param [Numeric] sign  0 or 1 or -1.
  @return [NArray::DComplex]
*/
static VALUE
nary_fft_cdft(int argc, VALUE *args, VALUE mod)
{
    ndfunc_t *func;
    narray_t *na;
    VALUE vres, vna, vsign=INT2NUM(1);
    fft_opt_t *g;
    int n;

    rb_scan_args(argc, args, "12", &vna, &vsign);
    GetNArray(vna,na);
    n = NA_SHAPE(na)[NA_NDIM(na)-1];
    if (n & (n-1)) {
        rb_raise(eRadixError,"array size(=%d) is not 2**n",n);
    }

    func = ndfunc_alloc(iter_fft_cdft, NO_LOOP,
                        1, 1, cDComplex, cDComplex);
    func->args[0].dim = 1;
    func->args[1].dim = 1;
    func->args[1].aux.shape[0] = n;

    g = ALLOCA_N(fft_opt_t,1);
    g->sign = NUM2INT(vsign);
    g->a    = ALLOC_N(rtype,n*2);
    g->t    = ALLOC_N(rtype,0);
    g->ip   = ALLOC_N(int,2+sqrt(n));
    g->w    = ALLOC_N(rtype,n/2);
    g->ip[0]= 0;

    vres = ndloop_do3(func, g, 1, vna);
    ndfunc_free(func);

    xfree(g->a);
    xfree(g->t);
    xfree(g->ip);
    xfree(g->w);
    return vres;
}

static void
iter_fft_rdft(na_loop_t *const lp)
{
    size_t  i, n;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    double  x;
    double *a;
    fft_opt_t *g;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    g = (fft_opt_t*)(lp->opt_ptr);
    a = (double*)(g->a);
    for (i=0; i<n; i++) {
        LOAD_DATA_STEP(p1, s1, idx1, double, x);
        a[i] = x;
    }

    rdft(n, g->sign, a, g->ip, g->w);

    for (i=0; i<n; i++) {
        x = a[i];
        STORE_DATA_STEP(p2, s2, idx2, double, x);
    }
}

/*
  Real DFT (Discrete Fourier Transform),
  Fast Version (Split-Radix).
  @overload rdft(narray,[sign])
  @param [NArray::DFloat] narray
  @param [Numeric] sign  0 or 1 or -1.
  @return [NArray::DFloat]
*/
static VALUE
nary_fft_rdft(int argc, VALUE *args, VALUE mod)
{
    ndfunc_t *func;
    narray_t *na;
    VALUE vres, vna, vsign=INT2NUM(1);
    fft_opt_t *g;
    int n;

    rb_scan_args(argc, args, "12", &vna, &vsign);
    GetNArray(vna,na);
    n = NA_SHAPE(na)[NA_NDIM(na)-1];
    if (n & (n-1)) {
        rb_raise(eRadixError,"array size(=%d) is not 2**n",n);
    }

    func = ndfunc_alloc(iter_fft_rdft, NO_LOOP,
                        1, 1, cDFloat, cDFloat);
    func->args[0].dim = 1;
    func->args[1].dim = 1;
    func->args[1].aux.shape[0] = n;

    g = ALLOCA_N(fft_opt_t,1);
    g->sign = NUM2INT(vsign);
    g->a    = ALLOC_N(rtype,n);
    g->t    = ALLOC_N(rtype,0);
    g->ip   = ALLOC_N(int,2+sqrt(n/2));
    g->w    = ALLOC_N(rtype,n/2);
    g->ip[0]= 0;

    vres = ndloop_do3(func, g, 1, vna);
    ndfunc_free(func);

    xfree(g->a);
    xfree(g->t);
    xfree(g->ip);
    xfree(g->w);
    return vres;
}

static void
iter_fft_ddct(na_loop_t *const lp)
{
    size_t  i, n;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    double  x;
    double *a;
    fft_opt_t *g;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    g = (fft_opt_t*)(lp->opt_ptr);
    a = (double*)(g->a);
    for (i=0; i<n; i++) {
        LOAD_DATA_STEP(p1, s1, idx1, double, x);
        a[i] = x;
    }

    ddct(n, g->sign, a, g->ip, g->w);

    for (i=0; i<n; i++) {
        x = a[i];
        STORE_DATA_STEP(p2, s2, idx2, double, x);
    }
}

/*
  DCT (Discrete Cosine Transform),
  Fast Version (Split-Radix).
  @overload ddct(narray,[sign])
  @param [NArray::DFloat] narray
  @param [Numeric] sign  0 or 1 or -1.
  @return [NArray::DFloat]
*/
static VALUE
nary_fft_ddct(int argc, VALUE *args, VALUE mod)
{
    ndfunc_t *func;
    narray_t *na;
    VALUE vres, vna, vsign=INT2NUM(1);
    fft_opt_t *g;
    int n;

    rb_scan_args(argc, args, "12", &vna, &vsign);
    GetNArray(vna,na);
    n = NA_SHAPE(na)[NA_NDIM(na)-1];
    if (n & (n-1)) {
        rb_raise(eRadixError,"array size(=%d) is not 2**n",n);
    }

    func = ndfunc_alloc(iter_fft_ddct, NO_LOOP,
                        1, 1, cDFloat, cDFloat);
    func->args[0].dim = 1;
    func->args[1].dim = 1;
    func->args[1].aux.shape[0] = n;

    g = ALLOCA_N(fft_opt_t,1);
    g->sign = NUM2INT(vsign);
    g->a    = ALLOC_N(rtype,n);
    g->t    = ALLOC_N(rtype,0);
    g->ip   = ALLOC_N(int,2+sqrt(n/2));
    g->w    = ALLOC_N(rtype,n*5/4);
    g->ip[0]= 0;

    vres = ndloop_do3(func, g, 1, vna);
    ndfunc_free(func);

    xfree(g->a);
    xfree(g->t);
    xfree(g->ip);
    xfree(g->w);
    return vres;
}

static void
iter_fft_ddst(na_loop_t *const lp)
{
    size_t  i, n;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    double  x;
    double *a;
    fft_opt_t *g;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    g = (fft_opt_t*)(lp->opt_ptr);
    a = (double*)(g->a);
    for (i=0; i<n; i++) {
        LOAD_DATA_STEP(p1, s1, idx1, double, x);
        a[i] = x;
    }

    ddst(n, g->sign, a, g->ip, g->w);

    for (i=0; i<n; i++) {
        x = a[i];
        STORE_DATA_STEP(p2, s2, idx2, double, x);
    }
}

/*
  DST (Discrete Sine Transform),
  Fast Version (Split-Radix).
  @overload ddst(narray,[sign])
  @param [NArray::DFloat] narray
  @param [Numeric] sign  0 or 1 or -1.
  @return [NArray::DFloat]
*/
static VALUE
nary_fft_ddst(int argc, VALUE *args, VALUE mod)
{
    ndfunc_t *func;
    narray_t *na;
    VALUE vres, vna, vsign=INT2NUM(1);
    fft_opt_t *g;
    int n;

    rb_scan_args(argc, args, "12", &vna, &vsign);
    GetNArray(vna,na);
    n = NA_SHAPE(na)[NA_NDIM(na)-1];
    if (n & (n-1)) {
        rb_raise(eRadixError,"array size(=%d) is not 2**n",n);
    }

    func = ndfunc_alloc(iter_fft_ddst, NO_LOOP,
                        1, 1, cDFloat, cDFloat);
    func->args[0].dim = 1;
    func->args[1].dim = 1;
    func->args[1].aux.shape[0] = n;

    g = ALLOCA_N(fft_opt_t,1);
    g->sign = NUM2INT(vsign);
    g->a    = ALLOC_N(rtype,n);
    g->t    = ALLOC_N(rtype,0);
    g->ip   = ALLOC_N(int,2+sqrt(n/2));
    g->w    = ALLOC_N(rtype,n*5/4);
    g->ip[0]= 0;

    vres = ndloop_do3(func, g, 1, vna);
    ndfunc_free(func);

    xfree(g->a);
    xfree(g->t);
    xfree(g->ip);
    xfree(g->w);
    return vres;
}

static void
iter_fft_dfct(na_loop_t *const lp)
{
    size_t  i, n;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    double  x;
    double *a;
    fft_opt_t *g;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    g = (fft_opt_t*)(lp->opt_ptr);
    a = (double*)(g->a);
    for (i=0; i<n; i++) {
        LOAD_DATA_STEP(p1, s1, idx1, double, x);
        a[i] = x;
    }

    dfct(n, a, g->t, g->ip, g->w);

    for (i=0; i<n; i++) {
        x = a[i];
        STORE_DATA_STEP(p2, s2, idx2, double, x);
    }
}

/*
  Cosine Transform of RDFT (Real Symmetric DFT),
  Fast Version (Split-Radix).
  @overload dfct(narray)
  @param [NArray::DFloat] narray
  @return [NArray::DFloat]
*/
static VALUE
nary_fft_dfct(int argc, VALUE *args, VALUE mod)
{
    ndfunc_t *func;
    narray_t *na;
    VALUE vres, vna, vsign=INT2NUM(1);
    fft_opt_t *g;
    int n;

    rb_scan_args(argc, args, "12", &vna, &vsign);
    GetNArray(vna,na);
    n = NA_SHAPE(na)[NA_NDIM(na)-1];
    if (n & (n-1)) {
        rb_raise(eRadixError,"array size(=%d) is not 2**n",n);
    }

    func = ndfunc_alloc(iter_fft_dfct, NO_LOOP,
                        1, 1, cDFloat, cDFloat);
    func->args[0].dim = 1;
    func->args[1].dim = 1;
    func->args[1].aux.shape[0] = n;

    g = ALLOCA_N(fft_opt_t,1);
    g->sign = NUM2INT(vsign);
    g->a    = ALLOC_N(rtype,n);
    g->t    = ALLOC_N(rtype,n/2);
    g->ip   = ALLOC_N(int,2+sqrt(n/4));
    g->w    = ALLOC_N(rtype,n*5/8);
    g->ip[0]= 0;

    vres = ndloop_do3(func, g, 1, vna);
    ndfunc_free(func);

    xfree(g->a);
    xfree(g->t);
    xfree(g->ip);
    xfree(g->w);
    return vres;
}

static void
iter_fft_dfst(na_loop_t *const lp)
{
    size_t  i, n;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    double  x;
    double *a;
    fft_opt_t *g;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    g = (fft_opt_t*)(lp->opt_ptr);
    a = (double*)(g->a);
    for (i=0; i<n; i++) {
        LOAD_DATA_STEP(p1, s1, idx1, double, x);
        a[i] = x;
    }

    dfst(n, a, g->t, g->ip, g->w);

    for (i=0; i<n; i++) {
        x = a[i];
        STORE_DATA_STEP(p2, s2, idx2, double, x);
    }
}

/*
  Sine Transform of RDFT (Real Anti-symmetric DFT),
  Fast Version (Split-Radix).
  @overload dfst(narray)
  @param [NArray::DFloat] narray
  @return [NArray::DFloat]
*/
static VALUE
nary_fft_dfst(int argc, VALUE *args, VALUE mod)
{
    ndfunc_t *func;
    narray_t *na;
    VALUE vres, vna, vsign=INT2NUM(1);
    fft_opt_t *g;
    int n;

    rb_scan_args(argc, args, "12", &vna, &vsign);
    GetNArray(vna,na);
    n = NA_SHAPE(na)[NA_NDIM(na)-1];
    if (n & (n-1)) {
        rb_raise(eRadixError,"array size(=%d) is not 2**n",n);
    }

    func = ndfunc_alloc(iter_fft_dfst, NO_LOOP,
                        1, 1, cDFloat, cDFloat);
    func->args[0].dim = 1;
    func->args[1].dim = 1;
    func->args[1].aux.shape[0] = n;

    g = ALLOCA_N(fft_opt_t,1);
    g->sign = NUM2INT(vsign);
    g->a    = ALLOC_N(rtype,n);
    g->t    = ALLOC_N(rtype,n/2);
    g->ip   = ALLOC_N(int,2+sqrt(n/4));
    g->w    = ALLOC_N(rtype,n*5/8);
    g->ip[0]= 0;

    vres = ndloop_do3(func, g, 1, vna);
    ndfunc_free(func);

    xfree(g->a);
    xfree(g->t);
    xfree(g->ip);
    xfree(g->w);
    return vres;
}


void
Init_fft()
{
    rb_mFFT = rb_define_module("FFT");
    eRadixError = rb_define_class_under(rb_mFFT, "RadixError", rb_eStandardError);


    rb_define_module_function(rb_mFFT, "cdft", nary_fft_cdft, -1);

    rb_define_module_function(rb_mFFT, "rdft", nary_fft_rdft, -1);

    rb_define_module_function(rb_mFFT, "ddct", nary_fft_ddct, -1);

    rb_define_module_function(rb_mFFT, "ddst", nary_fft_ddst, -1);

    rb_define_module_function(rb_mFFT, "dfct", nary_fft_dfct, -1);

    rb_define_module_function(rb_mFFT, "dfst", nary_fft_dfst, -1);

}
