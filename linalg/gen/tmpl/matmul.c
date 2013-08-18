// SUBROUTINE DGEMM(TRANSA,TRANSB,M,N,K,ALPHA,A,LDA,B,LDB,BETA,C,LDC)
fortran_integer
<%=blas_char%>gemm_(char*, char*,
       fortran_integer*, fortran_integer*, fortran_integer*,
       dtype*, dtype*, fortran_integer*,
       dtype*, fortran_integer*,
       dtype*, dtype*, fortran_integer*);

static void
<%=c_iter%>(na_loop_t *const lp)
{
    int i,j;
    static char *trans = "n";
    dtype alpha = m_one;
    dtype beta = m_zero;
    dtype *a, *b, *c;
    fortran_integer m, n, k;
    fortran_integer lda, ldb, ldc;

    // a[k,m], b[n,k], c[n,m]
    a = (dtype*)(lp->args[0].ptr + lp->iter[0].pos);
    b = (dtype*)(lp->args[1].ptr + lp->iter[1].pos);
    c = (dtype*)(lp->args[2].ptr + lp->iter[2].pos);
    //m = lp->n[0+3*1];
    //n = lp->n[1+3*0];
    //k = lp->n[1+3*1];
    m = lp->args[0].shape[1]; //n[0+3*1];
    n = lp->args[1].shape[0]; //n[1+3*0];
    k = lp->args[1].shape[1]; //n[1+3*1];

    //printf("m=%ld, n=%ld, k=%ld\n", m, n, k);
    lda = lp->iter[0].step / sizeof(dtype);
    ldb = lp->iter[1].step / sizeof(dtype);
    ldc = lp->iter[2].step / sizeof(dtype);
    //printf("lda=%ld, ldb=%ld, ldc=%ld\n", lda, ldb, ldc);
    //cblas_<%=blas_char%>gemm(CblasColMajor,CblasNoTrans,CblasNoTrans,
    //          m,n,k,alpha,a,lda,b,ldb,beta,c,ldc);
    <%=blas_char%>gemm_(trans,trans,&m,&n,&k,&alpha,a,&lda,b,&ldb,&beta,c,&ldc);
}


/*
  <%=blas_char%>gemm - performs one of the matrix-matrix operations
  C := alpha*op( A )*op( B ) + beta*C,
  @overload solve(narray,[iopt])
  @param [NArray::<%=class_name%>] narray >=2-dimentional NArray.
  @return [NArray::<%=class_name%>]
  @raise

  <%=blas_char%>gemm performs one of the matrix-matrix operations
  where  op( X ) is one of
    op( X ) = X   or   op( X ) = X',
  alpha and beta are scalars, and A, B and C are matrices,
  with op( A ) an m by k matrix, op( B ) a  k by n matrix and
  C an m by n matrix.
*/
static VALUE
<%=c_func%>(VALUE mod, VALUE a1, VALUE a2)
{
    narray_t *na1, *na2;
    size_t    n11, n12, n21, n22;
    size_t    shape[2];
    ndfunc_arg_in_t ain[2] = {{cT,2},{cT,2}};
    ndfunc_arg_out_t aout[1] = {{cT,2,shape}};
    ndfunc_t  ndf = {<%=c_iter%>, NO_LOOP|NDF_STRIDE_LOOP, 2, 1, ain, aout};

    GetNArray(a1,na1);
    GetNArray(a2,na2);
    CHECK_DIM_GE(na1,2);
    CHECK_DIM_GE(na2,2);
    n11 = na1->shape[na1->ndim-2]; // k
    n12 = na1->shape[na1->ndim-1]; // m
    n21 = na2->shape[na2->ndim-2]; // n
    n22 = na2->shape[na2->ndim-1]; // k
    //printf("n11=%ld\n",n11);
    //printf("n12=%ld\n",n12);
    //printf("n21=%ld\n",n21);
    //printf("n22=%ld\n",n22);
    if (n11!=n22) {
        rb_raise(nary_eShapeError,"matrix dimension mismatch");
    }
    shape[0] = n21;
    shape[1] = n12;
    return na_ndloop(&ndf, 2, a1, a2);
}
