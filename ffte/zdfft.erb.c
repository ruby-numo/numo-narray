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
        NArray::DComplex(.., NY, NX/2+1)
         NX = (2**IP) * (3**IQ) * (5**IR)
         NY = (2**JP) * (3**JQ) * (5**JR)<% if d==3 %>
         NZ = (2**KP) * (3**KQ) * (5**KR)<% end %>
  @return [NArray::DFloat]
    Result REAL narray:
        NArray::DFloat(.., NY, NX)
  @raise  [FFTE::RadixError] if NX, NY<%if d>2%>, NZ<%end%>
    is not (2^p)*(3^q)*(5^r).
*/
<% $funcs.push func="zdfft#{d}d" %>
static VALUE
nary_ffte_<%=func%>(int argc, VALUE *args, VALUE mod)
{
    ndfunc_t *func;
    narray_t *na;
    volatile VALUE vres, vna;
    int ndim;
    integer iopt=0;
    dcomplex *b;
    integer <%=argmap(d){|i|"n#{i}"}%>;
    size_t n=1;
    size_t shape[<%=d%>];

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

    func = ndfunc_alloc(iter_fft_zdfft<%=d%>d, NO_LOOP, 1, 1, cDComplex, cDFloat);
    func->args[0].dim = <%=d%>;
    func->args[1].dim = <%=d%>;
    func->args[1].aux.shape_p = shape;

    vna = na_copy(vna);
    b = ALLOC_N(dcomplex,n);

    zdfft<%=d%>d_(NULL, <%=argmap(d){|i|"&n#{i}"}%>, &iopt, b);
    vres = ndloop_do3(func, b, 1, vna);
    ndfunc_free(func);
    xfree(b);

    return vres;
}
<% end %>
