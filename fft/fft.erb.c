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
<%
require "./fft_tmpl.rb"
$defs.each do |x|
$params = x
%>
static void
iter_fft_<%=name%>(na_loop_t *const lp)
{
    size_t  i, n;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    <%=dtype%>  x;
    <%=dtype%> *a;
    fft_opt_t *g;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    g = (fft_opt_t*)(lp->opt_ptr);
    a = (<%=dtype%>*)(g->a);
    for (i=0; i<n; i++) {
        LOAD_DATA_STEP(p1, s1, idx1, <%=dtype%>, x);
        a[i] = x;
    }

    <%=name%>(<%=args%>);

    for (i=0; i<n; i++) {
        x = a[i];
        STORE_DATA_STEP(p2, s2, idx2, <%=dtype%>, x);
    }
}

/*<%=doc%>*/
static VALUE
nary_fft_<%=name%>(int argc, VALUE *args, VALUE mod)
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

    func = ndfunc_alloc(iter_fft_<%=name%>, NO_LOOP,
                        1, 1, <%=class_var%>, <%=class_var%>);
    func->args[0].dim = 1;
    func->args[1].dim = 1;
    func->args[1].aux.shape[0] = n;

    g = ALLOCA_N(fft_opt_t,1);
    g->sign = NUM2INT(vsign);
    g->a    = ALLOC_N(rtype,<%=n_a%>);
    g->t    = ALLOC_N(rtype,<%=n_t%>);
    g->ip   = ALLOC_N(int,<%=n_ip%>);
    g->w    = ALLOC_N(rtype,<%=n_w%>);
    g->ip[0]= 0;

    vres = ndloop_do3(func, g, 1, vna);
    ndfunc_free(func);

    xfree(g->a);
    xfree(g->t);
    xfree(g->ip);
    xfree(g->w);
    return vres;
}
<% end %>

void
Init_fft()
{
    rb_mFFT = rb_define_module("FFT");
    eRadixError = rb_define_class_under(rb_mFFT, "RadixError", rb_eStandardError);

<% $defs.each do |x|
   $params = x %>
    rb_define_module_function(rb_mFFT, "<%=name%>", nary_fft_<%=name%>, -1);
<% end %>
}
