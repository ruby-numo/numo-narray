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

<%
def argmap(arg)
  case arg
  when Numeric
    arg = 1..arg
  end
  arg.map do |x|
    yield(x)
  end.join(",")
end
$funcs = []
%>
int zfft1d_(dcomplex *a, integer *n, integer *iopt, dcomplex *b);
int zfft2d_(dcomplex *a, integer *nx, integer *ny, integer *iopt);
int zfft3d_(dcomplex *a, integer *nx, integer *ny, integer *nz, integer *iopt);

<% (1..3).each do |d| %>
// <%=d%>-dimentional 2,3,4,5,8-radix FFT
static void
iter_fft_zfft<%=d%>d(na_loop_t *const lp)
{
    char   *p1;
    integer iopt;
    integer <%=argmap(d){|i|"n#{i}"}%>;
<% if d==1 %>
    fft_opt_t *g;
<% end %>

<% (1..d).each do |i| %>
    n<%=i%> = lp->n[<%=i-1%>];
<% end %>
    p1 = ((lp)->args[0]).ptr + ((lp)->iter[0]).pos;

<% if d==1 %>
    g = (fft_opt_t*)(lp->opt_ptr);
    iopt = g->iopt;
    zfft<%=d%>d_((dcomplex*)p1, <%=argmap(d){|i|"&n#{i}"}%>, &iopt, g->b);
<% else %>
    iopt = *(integer*)(lp->opt_ptr);
    zfft<%=d%>d_((dcomplex*)p1, <%=argmap(d){|i|"&n#{i}"}%>, &iopt);
<% end %>
}


/*
  <%=d%>-dimentional COMPLEX FFT (Fast Fourier Transform)
  using Radix-2,3,4,5,8 FFT routine.
  Calculates on each last <%=d%>-dimention.
  @overload zfft<%=d%>d(narray,[iopt])
  @param [NArray::DComplex] narray
    >=<%=d%>-dimentional REAL NArray.
        NArray::DComplex(.., <%if d>2%>NZ, <%end;if d>1%>NY, <%end%>NX)
         NX = (2**IP) * (3**IQ) * (5**IR)<% if d>1 %>
         NY = (2**JP) * (3**JQ) * (5**JR)<% if d>2 %>
         NZ = (2**KP) * (3**KQ) * (5**KR)<% end; end %>
  @param [Numeric] iopt
    Transform direction.
         -1 FOR FORWARD TRANSFORM
         +1 FOR INVERSE TRANSFORM
  @return [NArray::DComplex]
    Result COMPLEX narray:
        NArray::DComplex(.., <%if d>2%>NZ, <%end;if d>1%>NY, <%end%>NX)
  @raise  [FFTE::RadixError] if NX<%if d>1%>, NY<% if d>2%>, NZ<%end;end%>
    is not (2^p)*(3^q)*(5^r).
*/
<% $funcs.push func="zfft#{d}d" %>
static VALUE
nary_ffte_<%=func%>(int argc, VALUE *args, VALUE mod)
{
    narray_t *na;
    VALUE vres, viopt=INT2NUM(1);
    volatile VALUE vna;
    int ndim;
    integer iopt=0;
    ndfunc_arg_in_t ain[1] = {{cDComplex,<%=d%>}};
    ndfunc_t ndf = { iter_fft_zfft<%=d%>d, NO_LOOP, 1, 0, ain, 0 };
<% if d==1 %>
    fft_opt_t *g;
    volatile VALUE vopt;
<% end %>
    integer <%=argmap(d){|i|"n#{i}"}%>;

    rb_scan_args(argc, args, "11", &vna, &viopt);
    GetNArray(vna,na);
    ndim = NA_NDIM(na);
    if (ndim<<%=d%>) {
        rb_raise(eRadixError,"ndim(=%d) should >= <%=d%>",ndim);
    }
<% (1..d).each do |i| %>
    n<%=i%> = NA_SHAPE(na)[NA_NDIM(na)-<%=i%>];
    if (!is235radix(n<%=i%>)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",NA_NDIM(na)-<%=i%>,n<%=i%>);
    }
<% end %>

    vres = na_copy(vna);

<% if d==1 %>
    g = alloc_fft_opt(n1*2, NUM2INT(viopt), &vopt);
    zfft1d_(NULL, &n1, &iopt, g->b);
    na_ndloop3(&ndf, g, 1, vres);
<% else %>
    zfft<%=d%>d_(NULL,  <%=argmap(d){|i|"&n#{i}"}%>, &iopt);
    iopt = NUM2INT(viopt);
    na_ndloop3(&ndf, &iopt, 1, vres);
<% end %>

    return vres;
}
<% end %>
int zdfft2d_(dcomplex *a, integer *nx, integer *ny, integer *iopt, dcomplex *b);
int zdfft3d_(dcomplex *a, integer *nx, integer *ny, integer *nz, integer *iopt, dcomplex *b);

<% (2..3).each do |d| %>
// <%=d%>-dimentional 2,3,4,5,8-radix FFT
static void
iter_fft_zdfft<%=d%>d(na_loop_t *const lp)
{
    char *p1, *p2;
    integer <%=argmap(d){|i|"n#{i}"}%>;
    integer iopt=1;
    size_t i, n;
    dcomplex *b;

    n1 = n = (lp->n[<%=d-1%>]-1)*2;
<% (2..d).each do |i| %>
    n<%=i%> = lp->n[<%=d-i%>];
    n *= n<%=i%>;
<% end %>
    b = (dcomplex*)(lp->opt_ptr);
    p1 = ((lp)->args[0]).ptr + ((lp)->iter[0]).pos;
    p2 = ((lp)->args[1]).ptr + ((lp)->iter[1]).pos;

    zdfft<%=d%>d_((dcomplex*)p1, <%=argmap(d){|i|"&n#{i}"}%>, &iopt, b);

    for (i=0; i<n; i++) {
        ((double*)p2)[i] = ((double*)p1)[i];
    }
}


/*
  <%=d%>-dimentional COMPLEX-TO-REAL FFT (Fast Fourier Transform)
  using Radix-2,3,4,5,8 FFT routine.
  Calculates on each last <%=d%>-dimention.
  INVERSE TRANSFORM.
  @overload zdfft<%=d%>d(narray)
  @param [NArray::DComplex] narray
    >=<%=d%>-dimentional COMPLEX NArray.
        NArray::DComplex(.., <%if d>2%>NZ, <%end;if d>1%>NY, <%end%>NX/2+1)
         NX = (2**IP) * (3**IQ) * (5**IR)
         NY = (2**JP) * (3**JQ) * (5**JR)<% if d==3 %>
         NZ = (2**KP) * (3**KQ) * (5**KR)<% end %>
  @return [NArray::DFloat]
    Result REAL narray:
        NArray::DFloat(.., <%if d>2%>NZ, <%end;if d>1%>NY, <%end%>NX)
  @raise  [FFTE::RadixError] if NX, NY<%if d>2%>, NZ<%end%>
    is not (2^p)*(3^q)*(5^r).
*/
<% $funcs.push func="zdfft#{d}d" %>
static VALUE
nary_ffte_<%=func%>(int argc, VALUE *args, VALUE mod)
{
    narray_t *na;
    VALUE vres;
    volatile VALUE vb, vna;
    int ndim;
    integer iopt=0;
    dcomplex *b;
    integer <%=argmap(d){|i|"n#{i}"}%>;
    size_t n=1;
    size_t shape[<%=d%>];
    ndfunc_arg_in_t ain[1] = {{cDComplex,<%=d%>}};
    ndfunc_arg_out_t aout[1] = {{cDFloat,<%=d%>,shape}};
    ndfunc_t ndf = { iter_fft_zdfft<%=d%>d, NO_LOOP, 1, 1, ain, aout };

    rb_scan_args(argc, args, "10", &vna);
    GetNArray(vna,na);
    ndim = NA_NDIM(na);
    if (ndim<<%=d%>) {
        rb_raise(eRadixError,"ndim(=%d) should >= <%=d%>",ndim);
    }
<% (1..d).each do |i| %>
    n<%=i%> = NA_SHAPE(na)[NA_NDIM(na)-<%=i%>];
    n *= n<%=i%>;
<% if i==1 %>
    shape[<%=d-1%>] = (n<%=i%>-1)*2;
<% else %>
    shape[<%=d-i%>] =  n<%=i%>;
<% end %>
    if (!is235radix(shape[<%=d-i%>])) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",
                 NA_NDIM(na)-<%=i%>,shape[<%=d-i%>]);
    }
<% end %>

    vna = na_copy(vna);
    b = ALLOC_N(dcomplex,n);
    vb = Data_Wrap_Struct(rb_cData,0,0,b);

    zdfft<%=d%>d_(NULL, <%=argmap(d){|i|"&n#{i}"}%>, &iopt, b);
    vres = na_ndloop3(&ndf, b, 1, vna);

    return vres;
}
<% end %>
int dzfft2d_(dcomplex *a, integer *nx, integer *ny, integer *iopt, dcomplex *b);
int dzfft3d_(dcomplex *a, integer *nx, integer *ny, integer *nz, integer *iopt, dcomplex *b);

<% (2..3).each do |d| %>
// <%=d%>-dimentional 2,3,4,5,8-radix FFT
static void
iter_fft_dzfft<%=d%>d(na_loop_t *const lp)
{
    char *p1, *p2;
    integer <%=argmap(d){|i|"n#{i}"}%>;
    integer iopt=-1;
    size_t i, n=1;
    dcomplex *b;

<% (1..d).each do |i| %>
    n<%=i%> = lp->n[<%=d-i%>];
    n *= n<%=i%>;
<% end %>
    b = (dcomplex*)(lp->opt_ptr);
    p1 = ((lp)->args[0]).ptr + ((lp)->iter[0]).pos;
    p2 = ((lp)->args[1]).ptr + ((lp)->iter[1]).pos;

    for (i=0; i<n; i++) {
        ((double*)p2)[i] = ((double*)p1)[i];
    }
    dzfft<%=d%>d_((dcomplex*)p2, <%=argmap(d){|i|"&n#{i}"}%>, &iopt, b);
}


/*
  <%=d%>-dimentional REAL-TO-COMPLEX FFT (Fast Fourier Transform)
  using Radix-2,3,4,5,8 FFT routine.
  Calculates on each last <%=d%>-dimention.
  FORWARD TRANSFORM.
  @overload dzfft<%=d%>d(narray)
  @param [NArray::DFloat] narray
    >=<%=d%>-dimentional REAL NArray.
        NArray::DFloat(.., <%if d>2%>NZ, <%end;if d>1%>NY, <%end%>NX)
         NX = (2**IP) * (3**IQ) * (5**IR)
         NY = (2**JP) * (3**JQ) * (5**JR)<% if d==3 %>
         NZ = (2**KP) * (3**KQ) * (5**KR)<% end %>
  @return [NArray::DComplex]
    Result COMPLEX narray:
        NArray::DComplex(.., <%if d>2%>NZ, <%end;if d>1%>NY, <%end%>NX/2+1)
  @raise  [FFTE::RadixError] if NX, NY<%if d>2%>, NZ<%end%>
    is not (2^p)*(3^q)*(5^r).
*/
<% $funcs.push func="dzfft#{d}d" %>
static VALUE
nary_ffte_<%=func%>(int argc, VALUE *args, VALUE mod)
{
    narray_t *na;
    volatile VALUE vb, vna;
    VALUE vres;
    int ndim;
    integer iopt=0;
    dcomplex *b;
    integer <%=argmap(d){|i|"n#{i}"}%>;
    size_t n=1;
    size_t shape[<%=d%>];
    ndfunc_arg_in_t ain[1] = {{cDFloat,<%=d%>}};
    ndfunc_arg_out_t aout[1] = {{cDComplex,<%=d%>,shape}};
    ndfunc_t ndf = { iter_fft_dzfft<%=d%>d, NO_LOOP, 1, 1, ain, aout };

    rb_scan_args(argc, args, "10", &vna);
    GetNArray(vna,na);
    ndim = NA_NDIM(na);
    if (ndim<<%=d%>) {
        rb_raise(eRadixError,"ndim(=%d) should >= <%=d%>",ndim);
    }
<% (1..d).each do |i| %>
    n<%=i%> = NA_SHAPE(na)[NA_NDIM(na)-<%=i%>];
    n *= n<%=i%>;
<% if i==1 %>
    shape[<%=d-1%>] = n<%=i%>/2+1;
<% else %>
    shape[<%=d-i%>] = n<%=i%>;
<% end %>
    if (!is235radix(n<%=i%>)) {
        rb_raise(eRadixError,"%d-th dim length(=%ld) is not 2,3,5-radix",
                 NA_NDIM(na)-<%=i%>,shape[<%=d-i%>]);
    }
<% end %>

    vna = na_copy(vna);
    b = ALLOC_N(dcomplex,n);
    vb = Data_Wrap_Struct(rb_cData,0,0,b);

    dzfft<%=d%>d_(NULL, <%=argmap(d){|i|"&n#{i}"}%>, &iopt, b);
    vres = na_ndloop3(&ndf, b, 1, vna);

    return vres;
}
<% end %>
void
Init_ffte()
{
    rb_mFFTE = rb_define_module("FFTE");
    // Radix Error
    eRadixError = rb_define_class_under(rb_mFFTE, "RadixError", rb_eStandardError);

<% $funcs.each do |f| %>
    rb_define_module_function(rb_mFFTE, "<%=f%>", nary_ffte_<%=f%>, -1);
<% end %>
}
