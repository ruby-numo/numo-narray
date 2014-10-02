// GEEV : calculate EigenValues and EigenVectors
typedef struct {
    dtype *work;
    rtype *rwork;
    dtype *wr, *wi, *vrr;
    fortran_integer lwork;
} geev_work_t;

static void
<%=c_iter%>(na_loop_t *const lp)
{
    char *jobvl="N", *jobvr="V";
    dtype *a;
    ctype *w, *vr;
    size_t i, j, m;
    fortran_integer n, lda, ldvr, lwork, info=0;
    geev_work_t *opt;
    VALUE option;
    dtype *work, *dmy=0;
    <% if ["z","c"].include? blas_char %>
    rtype *rwork;
    <% else %>
    dtype *wr, *wi, *vrr;
    dtype *p1;
    ctype *p2;
    <% end %>

    option = lp->option;
    Data_Get_Struct(option, geev_work_t, opt);

    // a[n,lda], w[n], vr[n,ldvl]
    a   = (dtype*)(lp->args[0].ptr + lp->iter[0].pos);
    w   = (ctype*)(lp->args[1].ptr + lp->iter[1].pos);
    vr  = (ctype*)(lp->args[2].ptr + lp->iter[2].pos);
    n   = m = lp->args[1].shape[0];
    lda  = lp->iter[0].step / sizeof(dtype);
    ldvr = lp->iter[2].step / sizeof(ctype);
    lwork = opt->lwork;

    work = opt->work;
    <% if ["z","c"].include? blas_char %>
    rwork = opt->rwork;
    <%=blas_char%>geev_(jobvl, jobvr, &n, a, &lda, w,
                        dmy, &ldvr, vr, &ldvr,
                        work, &lwork, rwork, &info);
    <% else %>
    wr = opt->wr;
    wi = opt->wi;
    vrr = opt->vrr;
    ldvr = n;
    <%=blas_char%>geev_(jobvl, jobvr, &n, a, &lda, wr, wi,
                        dmy, &ldvr, vrr, &ldvr,
                        work, &lwork, &info);
    for (j=0; j<m; j++) {
        REAL(w[j]) = wr[j];
        IMAG(w[j]) = wi[j];
    }
    for (j=0; j<m; j++) {
        if (IMAG(w[j]) == 0.0) { /* real eigenvalue */
            p1 = &(vrr[j*ldvr]);
            p2 = &(vr[j*ldvr]);
            for (i=0; i<m; i++) {
                REAL(p2[i]) =  p1[i];
                IMAG(p2[i]) =  0;
            }
        } else { /* complex eigenvalue */
            /* v(j)   = VR(:,j) + i*VR(:,j+1) and
               v(j+1) = VR(:,j) - i*VR(:,j+1) */
            p1 = &(vrr[j*ldvr]);
            p2 = &(vr[j*ldvr]);
            for (i=0; i<m; i++) {
                REAL(p2[i]) = p1[i];
                IMAG(p2[i]) = p1[i+ldvr];
                REAL(p2[i+ldvr]) = p1[i];
                IMAG(p2[i+ldvr]) = -p1[i+ldvr];
            }
            j++;
        }
    }
    <% end %>
}


/*
  @overload eigen(a)
  @param [NArray::<%=class_name%>] a >=2-dimentional NArray.
  @return [[NArray::<%=complex_class_name%>,NArray::<%=complex_class_name%>]] pair of eigenvalue and right eigenvector
  @raise

  <%=blas_char%>geev - computes the eigenvalues and the right eigenvectors
  for an N-by-N real nonsymmetric matrix A.
  The right eigenvector v(j) of A satisfies
                        A * v(j) = lambda(j) * v(j)
  where lambda(j) is its eigenvalue.
  The computed eigenvectors are normalized to have
  Euclidean norm equal to 1 and largest component real.
*/
static VALUE
<%=c_func%>(VALUE mod, VALUE a)
{
    char *chr = "N";
    dtype wk[1];
    double *dmy = 0;
    size_t sz[5];
    fortran_integer m, lwork, info=0;
    char *ptr;
    geev_work_t *opt;
    VALUE vopt;

    narray_t *na;
    size_t    n;
    size_t    eval_shape[1];
    size_t    evec_shape[2];
    ndfunc_arg_in_t ain[2] = {{cT,2},{sym_option}};
    ndfunc_arg_out_t aout[2] = {{cCT,1,eval_shape},{cCT,2,evec_shape}};
    ndfunc_t  ndf = {<%=c_iter%>, NO_LOOP, 2, 2, ain, aout};

    GetNArray(a,na);
    CHECK_DIM_GE(na,2);
    n = na->shape[na->ndim-1];
    if (n != na->shape[na->ndim-2]) {
        rb_raise(nary_eShapeError,"not square-matrix");
    }
    eval_shape[0] = n;
    evec_shape[0] = n;
    evec_shape[1] = n;

    m = n;
    lwork = -1;
    info = 0;
    <% if ["z","c"].include? blas_char %>
    <%=blas_char%>geev_(chr, chr, &m, dmy, &m, dmy,
                        dmy, &m,  dmy, &m, wk, &lwork, dmy, &info);
    lwork = REAL(wk[0]);
    sz[0] = ((sizeof(geev_work_t)-1)/16+1)*16;
    sz[1] = sizeof(dtype)*lwork; // work
    sz[2] = sizeof(rtype)*2*n; // rwork
    ptr = ALLOC_N(char,sz[0]+sz[1]+sz[2]);
    opt = (geev_work_t*)ptr;  ptr += sz[0];
    opt->work  = (dtype*)ptr; ptr += sz[1];
    opt->rwork = (rtype*)ptr;
    opt->lwork = lwork;
    <% else %>
    <%=blas_char%>geev_(chr, chr, &m, dmy, &m, dmy, dmy,
                        dmy, &m, dmy, &m, wk, &lwork, &info);
    lwork = wk[0];
    sz[0] = ((sizeof(geev_work_t)-1)/16+1)*16;
    sz[1] = sizeof(dtype)*lwork; // work
    sz[2] = sizeof(dtype)*n; // wr
    sz[3] = sizeof(dtype)*n; // wi
    sz[4] = sizeof(dtype)*n*n; // vrr
    ptr = ALLOC_N(char,sz[0]+sz[1]+sz[2]+sz[3]+sz[4]);
    opt = (geev_work_t*)ptr; ptr += sz[0];
    opt->work = (dtype*)ptr; ptr += sz[1];
    opt->wr   = (dtype*)ptr; ptr += sz[2];
    opt->wi   = (dtype*)ptr; ptr += sz[3];
    opt->vrr  = (dtype*)ptr;
    opt->lwork = lwork;
    <% end %>
    vopt = Data_Wrap_Struct(rb_cData, 0, -1, opt);

    return na_ndloop(&ndf, 2, na_copy(a), vopt);
}
