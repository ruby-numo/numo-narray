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
        NArray::DComplex(.., NY, NX)
         NX = (2**IP) * (3**IQ) * (5**IR)<% if d>1 %>
         NY = (2**JP) * (3**JQ) * (5**JR)<% if d>2 %>
         NZ = (2**KP) * (3**KQ) * (5**KR)<% end; end %>
  @param [Numeric] iopt
    Transform direction.
         -1 FOR FORWARD TRANSFORM
         +1 FOR INVERSE TRANSFORM
  @return [NArray::DComplex]
    Result COMPLEX narray:
        NArray::DComplex(.., NY, NX)
  @raise  [FFTE::RadixError] if NX<%if d>1%>, NY<% if d>2%>, NZ<%end;end%>
    is not (2^p)*(3^q)*(5^r).
*/
<% $funcs.push func="zfft#{d}d" %>
static VALUE
nary_ffte_<%=func%>(int argc, VALUE *args, VALUE mod)
{
    ndfunc_t *func;
    narray_t *na;
    VALUE vres, vna, viopt=INT2NUM(1);
    int ndim;
    integer iopt=0;
<% if d==1 %>
    fft_opt_t *g;
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

    func = ndfunc_alloc(iter_fft_zfft<%=d%>d, NO_LOOP, 1, 0, cDComplex);
    func->args[0].dim = <%=d%>;

    vres = na_copy(vna);

<% if d==1 %>
    g = ALLOCA_N(fft_opt_t,1);
    g->b = ALLOC_N(dcomplex,n1*2);
    zfft1d_(NULL, &n1, &iopt, g->b);
    g->iopt = NUM2INT(viopt);
    ndloop_do3(func, g, 1, vres);
    xfree(g->b);
<% else %>
    zfft<%=d%>d_(NULL,  <%=argmap(d){|i|"&n#{i}"}%>, &iopt);
    iopt = NUM2INT(viopt);
    ndloop_do3(func, &iopt, 1, vres);
<% end %>

    ndfunc_free(func);

    return vres;
}
<% end %>
