//?GESV - the solution to a real system of linear equations  A * X = B,
int <%=blas_char%>gesv_( fortran_integer *n, fortran_integer *nrhs,
	    dtype *a, fortran_integer *lda,
	    fortran_integer *ipiv,
	    dtype *b, fortran_integer *ldb,
	    fortran_integer *info );

static void
<%=c_iter%>(na_loop_t *lp)
{
    size_t  i1, i2, n1, n2, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    char   *x, *ipiv;
    dtype  *a, *q2;
    fortran_integer n, nrhs, info;

    INIT_PTR(lp, 0+4*0, p1, s1);
    INIT_PTR(lp, 0+4*1, p2, s2);
    n1 = lp->args[0].shape[0]; //n[0+4*0];
    n2 = lp->args[0].shape[1]; //n[0+4*1];
    //printf("n1=%d, n2=%d\n", n1, n2);
    Data_Get_Struct(lp->option,dtype,a);
    q2 = a;
    // copy matrix-A
    for (i1=0; i1<n1; i1++) {
        p2 = p1;
        p1 += s1;
        for (i2=0; i2<n2; i2++) {
            *(q2++) = *(dtype*)(p2);
            p2 += s2;
        }
    }
    x    = lp->args[2].ptr + lp->iter[2].pos;
    ipiv = lp->args[3].ptr + lp->iter[3].pos;
    n    = lp->args[1].shape[1]; //n[1+4*1];
    nrhs = lp->args[1].shape[0]; //n[1+4*0];
    //printf("n=%ld, nrhs=%ld\n", n, nrhs);

    <%=blas_char%>gesv_(&n, &nrhs, a, &n, (fortran_integer*)ipiv, (dtype*)x, &n, &info );
    //printf("info=%d\n", info);
}

/*
  <%=blas_char%>gesv - computes the solution to a complex system of linear equations  A * X = B,
  @overload solve(narray,[iopt])
  @param [NArray::<%=class_name%>] narray >=2-dimentional NArray.
  @return [NArray::<%=class_name%>]
  @raise

  <%=blas_char%>gesv computes the solution to a complex system of linear equations
     A  *  X = B,
  where A is an N-by-N matrix and X and B are N-by-NRHS matrices.
  The LU decomposition with partial pivoting and row interchanges is used to factor A as
     A = P * L * U,
  where P is a permutation matrix, L is unit lower triangular, and U is upper triangular.
  The factored form of A is then used to solve the system of equations A * X = B.
*/
static VALUE
<%=c_func%>(VALUE mod, VALUE a1, VALUE a2)
{
    volatile VALUE  opt;
    dtype    *data;
    narray_t *na1, *na2;
    size_t    n11, n12, n21, n22;
    size_t    shape1[2];
    size_t    shape2[1];
    ndfunc_arg_in_t ain[4] = {{cT,2},{cT,2},{sym_init,0},{sym_option}};
    ndfunc_arg_out_t aout[2] = {{cT,2,shape1},{cInt32,1,shape2}};
    ndfunc_t  ndf = {<%=c_iter%>, NO_LOOP|NDF_STRIDE_LOOP, 4, 2, ain, aout};

    if (sizeof(fortran_integer)==8) {
        aout[1].type = cInt64;
    }
    GetNArray(a1,na1);
    GetNArray(a2,na2);
    CHECK_DIM_GE(na1,2);
    CHECK_DIM_GE(na2,2);
    n11 = na1->shape[na1->ndim-2]; // n
    n12 = na1->shape[na1->ndim-1]; // n
    n21 = na2->shape[na2->ndim-2]; // n
    n22 = na2->shape[na2->ndim-1]; // nrhs
    if (n11!=n12 || n11!=n21) {
        rb_raise(nary_eShapeError,"matrix dimension mismatch");
    }
    shape1[0] = n21; // n
    shape1[1] = n22; // nrhs
    shape2[0] = n11;
    printf("n11=%ld\n",n11);
    printf("n12=%ld\n",n12);
    printf("n21=%ld\n",n21);
    printf("n22=%ld\n",n22);
    // Work memory
    data = ALLOC_N(dtype, n11*n12);
    opt = Data_Wrap_Struct(rb_cData,0,0,data);
    return na_ndloop(&ndf, 4, a1, a2, a2/*init*/, opt);
}
