/*
  dfloat.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2007 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <ruby.h>
#include <math.h>
//#include <version.h>
#include "narray.h"
#include "template.h"

//#include <atlas/cblas.h>
#include <cblas.h>

// SUBROUTINE DGEMM(TRANSA,TRANSB,M,N,K,ALPHA,A,LDA,B,LDB,BETA,C,LDC)
fortran_integer 
dgemm_(char*, char*,
       fortran_integer*, fortran_integer*, fortran_integer*,
       double*, double*, fortran_integer*,
       double*, fortran_integer*, 
       double*, double*, fortran_integer*);

static void
dfloat_dot_mm_loop( na_iterator_t *const itr, VALUE info )
{
    static char *trans = "n";
    double alpha = 1.0;
    double beta = 0.0;
    double *a, *b, *c;
    fortran_integer m, n, k;
    fortran_integer lda, ldb, ldc;

    // a[k,m], b[n,k], c[n,m]
    a = (double*)(itr[0].ptr+itr[0].pos);
    b = (double*)(itr[1].ptr+itr[1].pos);
    c = (double*)(itr[2].ptr+itr[2].pos);
    m = itr[0+3*1].n;
    n = itr[1+3*0].n;
    k = itr[1+3*1].n;
    //printf("m=%ld, n=%ld, k=%ld\n", m, n, k);
    lda = itr[0].step / sizeof(double);
    ldb = itr[1].step / sizeof(double);
    ldc = itr[2].step / sizeof(double);
    //printf("lda=%ld, ldb=%ld, ldc=%ld\n", lda, ldb, ldc);
    //dgemm_(trans,trans,&m,&n,&k,&alpha,a,&lda,b,&ldb,&beta,c,&ldc);
    cblas_dgemm(CblasColMajor,CblasNoTrans,CblasNoTrans,
		m,n,k,alpha,a,lda,b,ldb,beta,c,ldc);
}

//void cblas_dgemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
//                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
//                 const int K, const double alpha, const double *A,
//                 const int lda, const double *B, const int ldb,
//                 const double beta, double *C, const int ldc);

/*
static void
dfloat_dot_mm_loop( na_iterator_t *const itr, VALUE info )
{
    size_t  i;
    char   *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    ssize_t *idx1, *idx2, *idx3;
    double    x, y;
    static char *trans = "n";
    double alpha = 1.0;
    double beta = 0.0;
    double *a, *b, *c;
    fortran_integer m, n, k;

    //INIT_COUNTER( itr, i );
    INIT_PTR( itr, 0, p1, s1, idx1 );
    INIT_PTR( itr, 1, p2, s2, idx2 );
    INIT_PTR( itr, 2, p3, s3, idx3 );
    // double A[K,M], B[N,K], C[N,M];
    i = itr[0+3*0].n;
    m = itr[0+3*2].n;
    n = itr[1+3*1].n;
    k = itr[1+3*2].n;
    //printf("i=%d, m=%d, n=%d, k=%d\n", i, m, n, k);
    if (idx1||idx2||idx3) {
	for (; i--; ) {
	    //LOAD_DATA_PTR_STEP( p1, s1, idx1, double, a );
	    //LOAD_DATA_PTR_STEP( p2, s2, idx2, double, b );
	    //LOAD_DATA_PTR_STEP( p3, s3, idx3, double, c );
	}
    } else {
	for (; i--; ) {
	    dgemm_(trans,trans,&m,&n,&k,&alpha,p1,&m,p2,&k,&beta,p3,&m);
	    p1+=s1;
	    p2+=s2;
	    p3+=s3;
	}
    }
}
*/


 // Error Class ??
#define NA_CHECK_DIM_GE(na,nd)					\
    if ((na)->ndim<(nd)) {					\
	rb_raise(num_eShapeError,				\
		 "n-dimension=%d, but >=%d is expected",	\
		 (na)->ndim, (nd));				\
    }

#define NA_CHECK_CONTIGUOUS_LAST_DIM(na,st)				\
    {   if ((na)->index) {						\
	    if ((na)->index[(na)->ndim-1])				\
		rb_raise(num_eShapeError,				\
			 "last dim is not contiguous (with index)");	\
	}								\
	if ((na)->stride[(na)->ndim-1]!=(st)) {				\
	    rb_raise(num_eShapeError,					\
		     "last dim is not contiguous (stride=%d)",		\
		     (na)->stride[(na)->ndim-1]);			\
	}								\
    }

#define NA_CHECK_CONTIGUOUS_AT_DIM(na,dim,st)				\
    {   if ((na)->index) {						\
	    if ((na)->index[((dim)<0)?((dim)+(na)->ndim):(dim)])	\
		rb_raise(num_eShapeError,				\
			 "last dim is not contiguous (with index)");	\
	}								\
	if ((na)->stride[((dim)<0)?((dim)+(na)->ndim):(dim)]!=(st)) {	\
	    rb_raise(num_eShapeError,					\
		     "last dim is not contiguous (stride=%d)",		\
		     (na)->stride[((dim)<0)?((dim)+(na)->ndim):(dim)]); \
	}								\
    }

static VALUE
num_dfloat_s_dot_mm( VALUE mod, VALUE a1, VALUE a2 )
{
    ndfunc_t *func;
    VALUE v;
    narray_t *na1, *na2;
    size_t n11,n12,n21,n22;

    GetNArray(a1,na1);
    GetNArray(a2,na2);
    NA_CHECK_DIM_GE(na1,2);
    NA_CHECK_DIM_GE(na2,2);
    NA_CHECK_CONTIGUOUS_LAST_DIM(na1,sizeof(double));
    NA_CHECK_CONTIGUOUS_LAST_DIM(na2,sizeof(double));
    n11 = na1->shape[na1->ndim-2]; // k
    n12 = na1->shape[na1->ndim-1]; // m
    n21 = na2->shape[na2->ndim-2]; // n
    n22 = na2->shape[na2->ndim-1]; // k
    if (n11!=n22) {
	rb_raise(num_eShapeError,"matrix dimension mismatch");
    }
    func = ndfunc_alloc( dfloat_dot_mm_loop, NO_LOOP,
			 2, 1, cDFloat, cDFloat, cDFloat );
    func->args[0].dim = 2;
    func->args[1].dim = 2;
    func->args[2].dim = 2;
    func->args[2].aux.shape_p = ALLOC_N(size_t,2);
    func->args[2].aux.shape_p[0] = n21; // n
    func->args[2].aux.shape_p[1] = n12; // m
    //printf("%lx\n", func->args[2].aux.shape_p);
    //printf("n11=%d, n12=%d, n21=%d, n22=%d\n", n11, n12, n21, n22);
    v = ndfunc_execute( func, 2, a1, a2 );
    ndfunc_free(func);
    return v;
}


//DGESV - the solution to a real system of linear equations  A * X = B,
int dgesv_( fortran_integer *n, fortran_integer *nrhs,
	    double *a, fortran_integer *lda,
	    fortran_integer *ipiv,
	    double *b, fortran_integer *ldb,
	    fortran_integer *info );

static void
dfloat_solve_loop( na_iterator_t *const itr, VALUE opt )
{
    size_t  i1, i2, n1, n2, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    char   *x, *ipiv;
    double *a, *q2;
    fortran_integer n, nrhs, info;

    INIT_PTR( itr, 0+4*0, p1, s1, idx1 );
    INIT_PTR( itr, 0+4*1, p2, s2, idx2 );
    n1 = itr[0+4*0].n;
    n2 = itr[0+4*1].n;
    //printf("n1=%d, n2=%d\n", n1, n2);
    Data_Get_Struct(opt,double,a);
    q2 = a;
    // copy matrix-A
    for (i1=0; i1<n1; i1++) {
	if (idx1) {
	    p2 = p1 + *idx1;
	    idx1++;
	} else {
	    p2 = p1;
	    p1 += s1;
	}
	if (idx2) {
	    for (i2=0; i2<n2; i2++) {
		*(q2++) = *(double*)(p2 + *idx2);
		idx2++;
	    }
	} else {
	    for (i2=0; i2<n2; i2++) {
		*(q2++) = *(double*)(p2);
		p2 += s2;
	    }
	}
    }

    //a = itr[0].ptr+itr[0].pos;
    //b = itr[1].ptr+itr[1].pos;
    x    = itr[2].ptr+itr[2].pos;
    ipiv = itr[3].ptr+itr[3].pos;
    n    = itr[1+4*1].n;
    nrhs = itr[1+4*0].n;
    //printf("n=%ld, nrhs=%ld\n", n, nrhs);

    dgesv_(&n, &nrhs, a, &n, (fortran_integer*)ipiv, (double*)x, &n, &info );
    //printf("info=%d\n", info);
}

static VALUE
num_dfloat_s_solve( VALUE mod, VALUE a1, VALUE a2 )
{
    ndfunc_t *func;
    VALUE  v, opt;
    narray_t *na1, *na2;
    size_t  n11, n12, n21, n22;
    double *data;

    GetNArray(a1,na1); // A
    GetNArray(a2,na2); // B
    NA_CHECK_DIM_GE(na1,2);
    NA_CHECK_DIM_GE(na2,2);
    n11 = na1->shape[na1->ndim-2]; // n
    n12 = na1->shape[na1->ndim-1]; // n
    n21 = na2->shape[na2->ndim-2]; // n
    n22 = na2->shape[na2->ndim-1]; // nrhs
    if (n11!=n12 || n11!=n21) {
	rb_raise(num_eShapeError,"matrix dimension mismatch");
    }
    func = ndfunc_alloc( dfloat_solve_loop, NO_LOOP,
			 2, 2, cDFloat, cDFloat, cDFloat, cInt32 );
    func->args[0].dim = 2;
    func->args[1].dim = 2;
    func->args[2].dim = 2;
    func->args[2].aux.shape_p = ALLOC_N(size_t,2);
    func->args[2].aux.shape_p[0] = n21; // n
    func->args[2].aux.shape_p[1] = n22; // nrhs
    func->args[2].init = a2; // copy B to X
    func->args[3].dim = 1;
    func->args[3].aux.shape[0] = n11;
    //printf("%lx\n", func->args[2].aux.shape_p);
    //printf("n11=%d, n12=%d, n21=%d, n22=%d\n", n11, n12, n21, n22);

    // Buffer memory
    data = ALLOC_N(double, n11*n12);
    opt = Data_Wrap_Struct(rb_cData,0,0,data);
    // execution
    v = ndfunc_execute( func, 3, a1, a2, opt );
    ndfunc_free(func);
    return RARRAY_PTR(v)[0];
}



static void
dfloat_unit_loop( na_iterator_t *const itr, VALUE opt )
{
    char *p1, *p2;
    size_t i, j, n, n1, n2;
    ssize_t s1, s2;
    ssize_t *idx1, *idx2;

    n2 = itr[0+1*0].n;
    n1 = itr[0+1*1].n;
    //printf("n1=%ld, n2=%ld\n", n1, n2);
    INIT_PTR( itr, 0+1*0, p2, s2, idx2 );
    INIT_PTR( itr, 0+1*1, p1, s1, idx1 );
    for (j=0; j<n2; j++) {
	p1 = p2;
	for (i=0; i<n1; i++) {
	    STORE_DATA(p1,double,0.0);
	    p1 += s1;
	}
	p2 += s2;
    }
    p2 = itr[0+1*0].ptr + itr[0+1*0].pos;
    s2 = itr[0+1*0].step + itr[0+1*1].step;
    n = (n1<n2) ? n1:n2;
    for (i=0; i<n; i++) {
	STORE_DATA(p2,double,1.0);
	p2 += s2;
    }
}

#define SHAPE_AT(na,dim) (na)->shape[((dim)<0)?((dim)+(na)->ndim):(dim)]

static VALUE
num_dfloat_s_unit( VALUE mod, VALUE self )
{
    ndfunc_t *func;
    narray_t *na;

    GetNArray(self,na);
    //NA_CHECK_DIM_GE(na,2);
    func = ndfunc_alloc( dfloat_unit_loop, NO_LOOP,
			 1, 0, cDFloat );
    func->args[0].dim = 2;
    ndfunc_execute( func, 1, self );
    ndfunc_free(func);
    return self;
}

static VALUE
num_dfloat_s_inv( VALUE mod, VALUE a1 )
{
    VALUE unit;
    narray_t *na;
    size_t shape[2];

    //GetNArray(a1,na);
    //shape[0] = SHAPE_AT(na,-2);
    //shape[1] = SHAPE_AT(na,-1);
    //unit = rb_narray_new( cDFloat, 2, shape );
    unit = num_dfloat_s_unit( mod, na_copy(a1) );
    return num_dfloat_s_solve( mod, a1, unit );
}

VALUE num_math_method_missing(int argc, VALUE *argv, VALUE mod);

void
Init_linalg()
{
    VALUE hCast;
    VALUE mLinalg, mLinalgDFloat, mLinalgDComplex;
    mLinalg = rb_define_module_under(mNum,"Linalg");

    rb_define_singleton_method(mLinalg, "method_missing", num_math_method_missing, -1);

    mLinalgDFloat = rb_define_module_under(mLinalg, "DFloat");
    mLinalgDComplex = rb_define_module_under(mLinalg, "DComplex");

    hCast = rb_hash_new();
    rb_define_const(mLinalg, "TYPE_MODULE", hCast);
    rb_hash_aset(hCast, rb_cFixnum,  mLinalgDFloat);
    rb_hash_aset(hCast, rb_cBignum,  mLinalgDFloat);
    rb_hash_aset(hCast, rb_cInteger, mLinalgDFloat);
    rb_hash_aset(hCast, rb_cFloat,   mLinalgDFloat);
    rb_hash_aset(hCast, cComplex,    mLinalgDComplex);
    rb_hash_aset(hCast, cInt64,      mLinalgDFloat);
    rb_hash_aset(hCast, cInt48,      mLinalgDFloat);
    rb_hash_aset(hCast, cInt32,      mLinalgDFloat);
    rb_hash_aset(hCast, cInt24,      mLinalgDFloat);
    rb_hash_aset(hCast, cInt16,      mLinalgDFloat);
    rb_hash_aset(hCast, cInt8,       mLinalgDFloat);
    rb_hash_aset(hCast, cUInt64,     mLinalgDFloat);
    rb_hash_aset(hCast, cUInt48,     mLinalgDFloat);
    rb_hash_aset(hCast, cUInt32,     mLinalgDFloat);
    rb_hash_aset(hCast, cUInt24,     mLinalgDFloat);
    rb_hash_aset(hCast, cUInt16,     mLinalgDFloat);
    rb_hash_aset(hCast, cUInt8,      mLinalgDFloat);
    rb_hash_aset(hCast, cDFloat,     mLinalgDFloat);
    rb_hash_aset(hCast, cDComplex,   mLinalgDComplex);

    rb_define_singleton_method(mLinalgDFloat, "dot_mm", num_dfloat_s_dot_mm, 2);
    rb_define_singleton_method(mLinalgDFloat, "solve",  num_dfloat_s_solve, 2);
    rb_define_singleton_method(mLinalgDFloat, "set_unit", num_dfloat_s_unit, 1);
    rb_define_singleton_method(mLinalgDFloat, "inv", num_dfloat_s_inv, 1);
}
