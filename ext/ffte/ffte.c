/*
  FFTE: A Fast Fourier Transform Package
  http://www.ffte.jp/
  Computes Discrete Fourier Transforms of
  1-, 2- and 3- dimensional sequences of length (2^p)*(3^q)*(5^r).
  Copyright(C) 2000-2004,2008-2011 Daisuke Takahashi

  Ruby/NArray wrapper of FFTE
  FFTE is converted to C-code using f2c.
  Copyright(C) 2013 Masahiro Tanaka
*/

#include <ruby.h>
#include "narray.h"
#include "template.h"
//#include "f2c.h"
typedef int integer;

typedef struct {
    dcomplex *b;
    integer iopt;
    integer dummy;
} fft_opt_t;

static VALUE rb_mFFTE;
static VALUE eRadixError;

static inline
int is235radix(integer n) {
    if (n<=1) {return 0;}
    while (n % 5 == 0) {n /= 5;}
    while (n % 3 == 0) {n /= 3;}
    return (n & (n-1)) ? 0 : 1;
}

static inline fft_opt_t *
alloc_fft_opt(int nb, integer iopt, volatile VALUE *v)
{
    fft_opt_t *g;
    size_t sz1,sz2;
    char *ptr;
    sz1 = sizeof(fft_opt_t);
    sz2 = sizeof(dcomplex)*nb;
    ptr = xmalloc(sz1+sz2);
    g = (fft_opt_t*)ptr;
    ptr += sz1;
    g->b = (dcomplex*)ptr;
    g->iopt = iopt;
    *v = Data_Wrap_Struct(rb_cData,0,0,g);
    return g;
}


int zfft1d_(dcomplex *a, integer *n, integer *iopt, dcomplex *b);
int zfft2d_(dcomplex *a, integer *nx, integer *ny, integer *iopt);
int zfft3d_(dcomplex *a, integer *nx, integer *ny, integer *nz, integer *iopt);


// 1-dimentional 2,3,4,5,8-radix FFT
static void
iter_fft_zfft1d(na_loop_t *const lp)
{
    char   *p1;
    integer iopt;
    integer n1;

    fft_opt_t *g;



    //n1 = lp->n[0];
    n1 = lp->args[0].shape[0];

    p1 = ((lp)->args[0]).ptr + ((lp)->iter[0]).pos;


    g = (fft_opt_t*)(lp->opt_ptr);
    iopt = g->iopt;
    zfft1d_((dcomplex*)p1, &n1, &iopt, g->b);

}


/*
  1-dimentional COMPLEX FFT (Fast Fourier Transform)
  using Radix-2,3,4,5,8 FFT routine.
  Calculates on each last 1-dimention.
  @overload zfft1d(narray,[iopt])
  @param [NArray::DComplex] narray
    >=1-dimentional REAL NArray.
        NArray::DComplex(.., NX)
         NX = (2**IP) * (3**IQ) * (5**IR)
  @param [Numeric] iopt
    Transform direction.
         -1 FOR FORWARD TRANSFORM
         +1 FOR INVERSE TRANSFORM
  @return [NArray::DComplex]
    Result COMPLEX narray:
        NArray::DComplex(.., NX)
  @raise  [FFTE::RadixError] if NX
    is not (2^p)*(3^q)*(5^r).
*/

static VALUE
nary_ffte_zfft1d(int argc, VALUE *args, VALUE mod)
{
    narray_t *na;
    VALUE vres, viopt=INT2NUM(1);
    volatile VALUE vna;
    int ndim;
    integer iopt=0;
    ndfunc_arg_in_t ain[1] = {{cDComplex,1}};
    ndfunc_t ndf = { iter_fft_zfft1d, NO_LOOP, 1, 0, ain, 0 };

    fft_opt_t *g;
    volatile VALUE vopt;

    integer n1;

    rb_scan_args(argc, args, "11", &vna, &viopt);
    GetNArray(vna,na);
    ndim = NA_NDIM(na);
    if (ndim<1) {
        rb_raise(eRadixError,"ndim(=%d) should >= 1",ndim);
    }

    n1 = NA_SHAPE(na)[NA_NDIM(na)-1];
    if (!is235radix(n1)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",NA_NDIM(na)-1,n1);
    }


    vres = na_copy(vna);


    g = alloc_fft_opt(n1*2, NUM2INT(viopt), &vopt);
    zfft1d_(NULL, &n1, &iopt, g->b);
    na_ndloop3(&ndf, g, 1, vres);


    return vres;
}

// 2-dimentional 2,3,4,5,8-radix FFT
static void
iter_fft_zfft2d(na_loop_t *const lp)
{
    char   *p1;
    integer iopt;
    integer n1,n2;



    //n1 = lp->n[0];
    n1 = lp->args[0].shape[0];

    //n2 = lp->n[1];
    n2 = lp->args[0].shape[1];

    p1 = ((lp)->args[0]).ptr + ((lp)->iter[0]).pos;


    iopt = *(integer*)(lp->opt_ptr);
    zfft2d_((dcomplex*)p1, &n1,&n2, &iopt);

}


/*
  2-dimentional COMPLEX FFT (Fast Fourier Transform)
  using Radix-2,3,4,5,8 FFT routine.
  Calculates on each last 2-dimention.
  @overload zfft2d(narray,[iopt])
  @param [NArray::DComplex] narray
    >=2-dimentional REAL NArray.
        NArray::DComplex(.., NY, NX)
         NX = (2**IP) * (3**IQ) * (5**IR)
         NY = (2**JP) * (3**JQ) * (5**JR)
  @param [Numeric] iopt
    Transform direction.
         -1 FOR FORWARD TRANSFORM
         +1 FOR INVERSE TRANSFORM
  @return [NArray::DComplex]
    Result COMPLEX narray:
        NArray::DComplex(.., NY, NX)
  @raise  [FFTE::RadixError] if NX, NY
    is not (2^p)*(3^q)*(5^r).
*/

static VALUE
nary_ffte_zfft2d(int argc, VALUE *args, VALUE mod)
{
    narray_t *na;
    VALUE vres, viopt=INT2NUM(1);
    volatile VALUE vna;
    int ndim;
    integer iopt=0;
    ndfunc_arg_in_t ain[1] = {{cDComplex,2}};
    ndfunc_t ndf = { iter_fft_zfft2d, NO_LOOP, 1, 0, ain, 0 };

    integer n1,n2;

    rb_scan_args(argc, args, "11", &vna, &viopt);
    GetNArray(vna,na);
    ndim = NA_NDIM(na);
    if (ndim<2) {
        rb_raise(eRadixError,"ndim(=%d) should >= 2",ndim);
    }

    n1 = NA_SHAPE(na)[NA_NDIM(na)-1];
    if (!is235radix(n1)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",NA_NDIM(na)-1,n1);
    }

    n2 = NA_SHAPE(na)[NA_NDIM(na)-2];
    if (!is235radix(n2)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",NA_NDIM(na)-2,n2);
    }


    vres = na_copy(vna);


    zfft2d_(NULL,  &n1,&n2, &iopt);
    iopt = NUM2INT(viopt);
    na_ndloop3(&ndf, &iopt, 1, vres);


    return vres;
}

// 3-dimentional 2,3,4,5,8-radix FFT
static void
iter_fft_zfft3d(na_loop_t *const lp)
{
    char   *p1;
    integer iopt;
    integer n1,n2,n3;



    //n1 = lp->n[0];
    n1 = lp->args[0].shape[0];

    //n2 = lp->n[1];
    n2 = lp->args[0].shape[1];

    //n3 = lp->n[2];
    n3 = lp->args[0].shape[2];

    p1 = ((lp)->args[0]).ptr + ((lp)->iter[0]).pos;


    iopt = *(integer*)(lp->opt_ptr);
    zfft3d_((dcomplex*)p1, &n1,&n2,&n3, &iopt);

}


/*
  3-dimentional COMPLEX FFT (Fast Fourier Transform)
  using Radix-2,3,4,5,8 FFT routine.
  Calculates on each last 3-dimention.
  @overload zfft3d(narray,[iopt])
  @param [NArray::DComplex] narray
    >=3-dimentional REAL NArray.
        NArray::DComplex(.., NZ, NY, NX)
         NX = (2**IP) * (3**IQ) * (5**IR)
         NY = (2**JP) * (3**JQ) * (5**JR)
         NZ = (2**KP) * (3**KQ) * (5**KR)
  @param [Numeric] iopt
    Transform direction.
         -1 FOR FORWARD TRANSFORM
         +1 FOR INVERSE TRANSFORM
  @return [NArray::DComplex]
    Result COMPLEX narray:
        NArray::DComplex(.., NZ, NY, NX)
  @raise  [FFTE::RadixError] if NX, NY, NZ
    is not (2^p)*(3^q)*(5^r).
*/

static VALUE
nary_ffte_zfft3d(int argc, VALUE *args, VALUE mod)
{
    narray_t *na;
    VALUE vres, viopt=INT2NUM(1);
    volatile VALUE vna;
    int ndim;
    integer iopt=0;
    ndfunc_arg_in_t ain[1] = {{cDComplex,3}};
    ndfunc_t ndf = { iter_fft_zfft3d, NO_LOOP, 1, 0, ain, 0 };

    integer n1,n2,n3;

    rb_scan_args(argc, args, "11", &vna, &viopt);
    GetNArray(vna,na);
    ndim = NA_NDIM(na);
    if (ndim<3) {
        rb_raise(eRadixError,"ndim(=%d) should >= 3",ndim);
    }

    n1 = NA_SHAPE(na)[NA_NDIM(na)-1];
    if (!is235radix(n1)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",NA_NDIM(na)-1,n1);
    }

    n2 = NA_SHAPE(na)[NA_NDIM(na)-2];
    if (!is235radix(n2)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",NA_NDIM(na)-2,n2);
    }

    n3 = NA_SHAPE(na)[NA_NDIM(na)-3];
    if (!is235radix(n3)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",NA_NDIM(na)-3,n3);
    }


    vres = na_copy(vna);


    zfft3d_(NULL,  &n1,&n2,&n3, &iopt);
    iopt = NUM2INT(viopt);
    na_ndloop3(&ndf, &iopt, 1, vres);


    return vres;
}



int zdfft2d_(dcomplex *a, integer *nx, integer *ny, integer *iopt, dcomplex *b);
int zdfft3d_(dcomplex *a, integer *nx, integer *ny, integer *nz, integer *iopt, dcomplex *b);


// 2-dimentional 2,3,4,5,8-radix FFT
static void
iter_fft_zdfft2d(na_loop_t *const lp)
{
    char *p1, *p2;
    integer n1,n2;
    integer iopt=1;
    size_t i, n;
    dcomplex *b;

    //n1 = n = (lp->n[1]-1)*2;
    n1 = n = (lp->args[0].shape[1]-1)*2;

    //n2 = lp->n[0];
    n2 = lp->args[0].shape[0];
    n *= n2;

    b = (dcomplex*)(lp->opt_ptr);
    p1 = ((lp)->args[0]).ptr + ((lp)->iter[0]).pos;
    p2 = ((lp)->args[1]).ptr + ((lp)->iter[1]).pos;

    zdfft2d_((dcomplex*)p1, &n1,&n2, &iopt, b);

    for (i=0; i<n; i++) {
        ((double*)p2)[i] = ((double*)p1)[i];
    }
}


/*
  2-dimentional COMPLEX-TO-REAL FFT (Fast Fourier Transform)
  using Radix-2,3,4,5,8 FFT routine.
  Calculates on each last 2-dimention.
  INVERSE TRANSFORM.
  @overload zdfft2d(narray)
  @param [NArray::DComplex] narray
    >=2-dimentional COMPLEX NArray.
        NArray::DComplex(.., NY, NX/2+1)
         NX = (2**IP) * (3**IQ) * (5**IR)
         NY = (2**JP) * (3**JQ) * (5**JR)
  @return [NArray::DFloat]
    Result REAL narray:
        NArray::DFloat(.., NY, NX)
  @raise  [FFTE::RadixError] if NX, NY
    is not (2^p)*(3^q)*(5^r).
*/

static VALUE
nary_ffte_zdfft2d(int argc, VALUE *args, VALUE mod)
{
    narray_t *na;
    VALUE vres;
    volatile VALUE vb, vna;
    int ndim;
    integer iopt=0;
    dcomplex *b;
    integer n1,n2;
    size_t n=1;
    size_t shape[2];
    ndfunc_arg_in_t ain[1] = {{cDComplex,2}};
    ndfunc_arg_out_t aout[1] = {{cDFloat,2,shape}};
    ndfunc_t ndf = { iter_fft_zdfft2d, NO_LOOP, 1, 1, ain, aout };

    rb_scan_args(argc, args, "10", &vna);
    GetNArray(vna,na);
    ndim = NA_NDIM(na);
    if (ndim<2) {
        rb_raise(eRadixError,"ndim(=%d) should >= 2",ndim);
    }

    n1 = NA_SHAPE(na)[NA_NDIM(na)-1];
    n *= n1;

    shape[1] = (n1-1)*2;

    if (!is235radix(shape[1])) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",
                 NA_NDIM(na)-1,shape[1]);
    }

    n2 = NA_SHAPE(na)[NA_NDIM(na)-2];
    n *= n2;

    shape[0] =  n2;

    if (!is235radix(shape[0])) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",
                 NA_NDIM(na)-2,shape[0]);
    }


    vna = na_copy(vna);
    b = ALLOC_N(dcomplex,n);
    vb = Data_Wrap_Struct(rb_cData,0,0,b);

    zdfft2d_(NULL, &n1,&n2, &iopt, b);
    vres = na_ndloop3(&ndf, b, 1, vna);

    return vres;
}

// 3-dimentional 2,3,4,5,8-radix FFT
static void
iter_fft_zdfft3d(na_loop_t *const lp)
{
    char *p1, *p2;
    integer n1,n2,n3;
    integer iopt=1;
    size_t i, n;
    dcomplex *b;

    //n1 = n = (lp->n[2]-1)*2;
    n1 = n = (lp->args[0].shape[2]-1)*2;

    //n2 = lp->n[1];
    n2 = lp->args[0].shape[1];
    n *= n2;

    //n3 = lp->n[0];
    n3 = lp->args[0].shape[0];
    n *= n3;

    b = (dcomplex*)(lp->opt_ptr);
    p1 = ((lp)->args[0]).ptr + ((lp)->iter[0]).pos;
    p2 = ((lp)->args[1]).ptr + ((lp)->iter[1]).pos;

    zdfft3d_((dcomplex*)p1, &n1,&n2,&n3, &iopt, b);

    for (i=0; i<n; i++) {
        ((double*)p2)[i] = ((double*)p1)[i];
    }
}


/*
  3-dimentional COMPLEX-TO-REAL FFT (Fast Fourier Transform)
  using Radix-2,3,4,5,8 FFT routine.
  Calculates on each last 3-dimention.
  INVERSE TRANSFORM.
  @overload zdfft3d(narray)
  @param [NArray::DComplex] narray
    >=3-dimentional COMPLEX NArray.
        NArray::DComplex(.., NZ, NY, NX/2+1)
         NX = (2**IP) * (3**IQ) * (5**IR)
         NY = (2**JP) * (3**JQ) * (5**JR)
         NZ = (2**KP) * (3**KQ) * (5**KR)
  @return [NArray::DFloat]
    Result REAL narray:
        NArray::DFloat(.., NZ, NY, NX)
  @raise  [FFTE::RadixError] if NX, NY, NZ
    is not (2^p)*(3^q)*(5^r).
*/

static VALUE
nary_ffte_zdfft3d(int argc, VALUE *args, VALUE mod)
{
    narray_t *na;
    VALUE vres;
    volatile VALUE vb, vna;
    int ndim;
    integer iopt=0;
    dcomplex *b;
    integer n1,n2,n3;
    size_t n=1;
    size_t shape[3];
    ndfunc_arg_in_t ain[1] = {{cDComplex,3}};
    ndfunc_arg_out_t aout[1] = {{cDFloat,3,shape}};
    ndfunc_t ndf = { iter_fft_zdfft3d, NO_LOOP, 1, 1, ain, aout };

    rb_scan_args(argc, args, "10", &vna);
    GetNArray(vna,na);
    ndim = NA_NDIM(na);
    if (ndim<3) {
        rb_raise(eRadixError,"ndim(=%d) should >= 3",ndim);
    }

    n1 = NA_SHAPE(na)[NA_NDIM(na)-1];
    n *= n1;

    shape[2] = (n1-1)*2;

    if (!is235radix(shape[2])) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",
                 NA_NDIM(na)-1,shape[2]);
    }

    n2 = NA_SHAPE(na)[NA_NDIM(na)-2];
    n *= n2;

    shape[1] =  n2;

    if (!is235radix(shape[1])) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",
                 NA_NDIM(na)-2,shape[1]);
    }

    n3 = NA_SHAPE(na)[NA_NDIM(na)-3];
    n *= n3;

    shape[0] =  n3;

    if (!is235radix(shape[0])) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",
                 NA_NDIM(na)-3,shape[0]);
    }


    vna = na_copy(vna);
    b = ALLOC_N(dcomplex,n);
    vb = Data_Wrap_Struct(rb_cData,0,0,b);

    zdfft3d_(NULL, &n1,&n2,&n3, &iopt, b);
    vres = na_ndloop3(&ndf, b, 1, vna);

    return vres;
}



int dzfft2d_(dcomplex *a, integer *nx, integer *ny, integer *iopt, dcomplex *b);
int dzfft3d_(dcomplex *a, integer *nx, integer *ny, integer *nz, integer *iopt, dcomplex *b);


// 2-dimentional 2,3,4,5,8-radix FFT
static void
iter_fft_dzfft2d(na_loop_t *const lp)
{
    char *p1, *p2;
    integer n1,n2;
    integer iopt=-1;
    size_t i, n=1;
    dcomplex *b;


    //n1 = lp->n[1];
    n1 = lp->args[0].shape[1];
    n *= n1;

    //n2 = lp->n[0];
    n2 = lp->args[0].shape[0];
    n *= n2;

    b = (dcomplex*)(lp->opt_ptr);
    p1 = ((lp)->args[0]).ptr + ((lp)->iter[0]).pos;
    p2 = ((lp)->args[1]).ptr + ((lp)->iter[1]).pos;

    for (i=0; i<n; i++) {
        ((double*)p2)[i] = ((double*)p1)[i];
    }
    dzfft2d_((dcomplex*)p2, &n1,&n2, &iopt, b);
}


/*
  2-dimentional REAL-TO-COMPLEX FFT (Fast Fourier Transform)
  using Radix-2,3,4,5,8 FFT routine.
  Calculates on each last 2-dimention.
  FORWARD TRANSFORM.
  @overload dzfft2d(narray)
  @param [NArray::DFloat] narray
    >=2-dimentional REAL NArray.
        NArray::DFloat(.., NY, NX)
         NX = (2**IP) * (3**IQ) * (5**IR)
         NY = (2**JP) * (3**JQ) * (5**JR)
  @return [NArray::DComplex]
    Result COMPLEX narray:
        NArray::DComplex(.., NY, NX/2+1)
  @raise  [FFTE::RadixError] if NX, NY
    is not (2^p)*(3^q)*(5^r).
*/

static VALUE
nary_ffte_dzfft2d(int argc, VALUE *args, VALUE mod)
{
    narray_t *na;
    volatile VALUE vb, vna;
    VALUE vres;
    int ndim;
    integer iopt=0;
    dcomplex *b;
    integer n1,n2;
    size_t n=1;
    size_t shape[2];
    ndfunc_arg_in_t ain[1] = {{cDFloat,2}};
    ndfunc_arg_out_t aout[1] = {{cDComplex,2,shape}};
    ndfunc_t ndf = { iter_fft_dzfft2d, NO_LOOP, 1, 1, ain, aout };

    rb_scan_args(argc, args, "10", &vna);
    GetNArray(vna,na);
    ndim = NA_NDIM(na);
    if (ndim<2) {
        rb_raise(eRadixError,"ndim(=%d) should >= 2",ndim);
    }

    n1 = NA_SHAPE(na)[NA_NDIM(na)-1];
    n *= n1;

    shape[1] = n1/2+1;

    if (!is235radix(n1)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",
                 NA_NDIM(na)-1,shape[1]);
    }

    n2 = NA_SHAPE(na)[NA_NDIM(na)-2];
    n *= n2;

    shape[0] = n2;

    if (!is235radix(n2)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",
                 NA_NDIM(na)-2,shape[0]);
    }


    vna = na_copy(vna);
    b = ALLOC_N(dcomplex,n);
    vb = Data_Wrap_Struct(rb_cData,0,0,b);

    dzfft2d_(NULL, &n1,&n2, &iopt, b);
    vres = na_ndloop3(&ndf, b, 1, vna);

    return vres;
}

// 3-dimentional 2,3,4,5,8-radix FFT
static void
iter_fft_dzfft3d(na_loop_t *const lp)
{
    char *p1, *p2;
    integer n1,n2,n3;
    integer iopt=-1;
    size_t i, n=1;
    dcomplex *b;


    //n1 = lp->n[2];
    n1 = lp->args[0].shape[2];
    n *= n1;

    //n2 = lp->n[1];
    n2 = lp->args[0].shape[1];
    n *= n2;

    //n3 = lp->n[0];
    n3 = lp->args[0].shape[0];
    n *= n3;

    b = (dcomplex*)(lp->opt_ptr);
    p1 = ((lp)->args[0]).ptr + ((lp)->iter[0]).pos;
    p2 = ((lp)->args[1]).ptr + ((lp)->iter[1]).pos;

    for (i=0; i<n; i++) {
        ((double*)p2)[i] = ((double*)p1)[i];
    }
    dzfft3d_((dcomplex*)p2, &n1,&n2,&n3, &iopt, b);
}


/*
  3-dimentional REAL-TO-COMPLEX FFT (Fast Fourier Transform)
  using Radix-2,3,4,5,8 FFT routine.
  Calculates on each last 3-dimention.
  FORWARD TRANSFORM.
  @overload dzfft3d(narray)
  @param [NArray::DFloat] narray
    >=3-dimentional REAL NArray.
        NArray::DFloat(.., NZ, NY, NX)
         NX = (2**IP) * (3**IQ) * (5**IR)
         NY = (2**JP) * (3**JQ) * (5**JR)
         NZ = (2**KP) * (3**KQ) * (5**KR)
  @return [NArray::DComplex]
    Result COMPLEX narray:
        NArray::DComplex(.., NZ, NY, NX/2+1)
  @raise  [FFTE::RadixError] if NX, NY, NZ
    is not (2^p)*(3^q)*(5^r).
*/

static VALUE
nary_ffte_dzfft3d(int argc, VALUE *args, VALUE mod)
{
    narray_t *na;
    volatile VALUE vb, vna;
    VALUE vres;
    int ndim;
    integer iopt=0;
    dcomplex *b;
    integer n1,n2,n3;
    size_t n=1;
    size_t shape[3];
    ndfunc_arg_in_t ain[1] = {{cDFloat,3}};
    ndfunc_arg_out_t aout[1] = {{cDComplex,3,shape}};
    ndfunc_t ndf = { iter_fft_dzfft3d, NO_LOOP, 1, 1, ain, aout };

    rb_scan_args(argc, args, "10", &vna);
    GetNArray(vna,na);
    ndim = NA_NDIM(na);
    if (ndim<3) {
        rb_raise(eRadixError,"ndim(=%d) should >= 3",ndim);
    }

    n1 = NA_SHAPE(na)[NA_NDIM(na)-1];
    n *= n1;

    shape[2] = n1/2+1;

    if (!is235radix(n1)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",
                 NA_NDIM(na)-1,shape[2]);
    }

    n2 = NA_SHAPE(na)[NA_NDIM(na)-2];
    n *= n2;

    shape[1] = n2;

    if (!is235radix(n2)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",
                 NA_NDIM(na)-2,shape[1]);
    }

    n3 = NA_SHAPE(na)[NA_NDIM(na)-3];
    n *= n3;

    shape[0] = n3;

    if (!is235radix(n3)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",
                 NA_NDIM(na)-3,shape[0]);
    }


    vna = na_copy(vna);
    b = ALLOC_N(dcomplex,n);
    vb = Data_Wrap_Struct(rb_cData,0,0,b);

    dzfft3d_(NULL, &n1,&n2,&n3, &iopt, b);
    vres = na_ndloop3(&ndf, b, 1, vna);

    return vres;
}



void
Init_ffte()
{
    rb_mFFTE = rb_define_module("FFTE");
    // Radix Error
    eRadixError = rb_define_class_under(rb_mFFTE, "RadixError", rb_eStandardError);


    rb_define_module_function(rb_mFFTE, "zfft1d", nary_ffte_zfft1d, -1);

    rb_define_module_function(rb_mFFTE, "zfft2d", nary_ffte_zfft2d, -1);

    rb_define_module_function(rb_mFFTE, "zfft3d", nary_ffte_zfft3d, -1);

    rb_define_module_function(rb_mFFTE, "zdfft2d", nary_ffte_zdfft2d, -1);

    rb_define_module_function(rb_mFFTE, "zdfft3d", nary_ffte_zdfft3d, -1);

    rb_define_module_function(rb_mFFTE, "dzfft2d", nary_ffte_dzfft2d, -1);

    rb_define_module_function(rb_mFFTE, "dzfft3d", nary_ffte_dzfft3d, -1);

}
