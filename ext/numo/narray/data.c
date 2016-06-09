/*
  data.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011,2013 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

#include <ruby.h>
#include "numo/narray.h"
#include "numo/template.h"

// ---------------------------------------------------------------------

#define LOOP_UNARY_PTR(lp,proc)                    \
{                                                  \
    size_t  i;                                     \
    ssize_t s1, s2;                                \
    char   *p1, *p2;                               \
    size_t *idx1, *idx2;                           \
    INIT_COUNTER(lp, i);                           \
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);             \
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);             \
    if (idx1) {                                    \
        if (idx2) {                                \
            for (; i--;) {                         \
                proc((p1+*idx1), (p2+*idx2));      \
                idx1++;                            \
                idx2++;                            \
            }                                      \
        } else {                                   \
            for (; i--;) {                         \
                proc((p1+*idx1), p2);              \
                idx1++;                            \
                p2 += s2;                          \
            }                                      \
        }                                          \
    } else {                                       \
        if (idx2) {                                \
            for (; i--;) {                         \
                proc(p1, (p1+*idx2));              \
                p1 += s1;                          \
                idx2++;                            \
            }                                      \
        } else {                                   \
            for (; i--;) {                         \
                proc(p1, p2);                      \
                p1 += s1;                          \
                p2 += s2;                          \
            }                                      \
        }                                          \
    }                                              \
}

#define m_memcpy(src,dst) memcpy(dst,src,e)
void
iter_copy_bytes(na_loop_t *const lp)
{
    size_t e;
    e = lp->args[0].elmsz;
    LOOP_UNARY_PTR(lp,m_memcpy);
}

VALUE
na_copy(VALUE self)
{
    VALUE v;
    ndfunc_arg_in_t ain[1] = {{Qnil,0}};
    ndfunc_arg_out_t aout[1] = {{INT2FIX(0),0}};
    ndfunc_t ndf = { iter_copy_bytes, FULL_LOOP, 1, 1, ain, aout };

    v = na_ndloop(&ndf, 1, self);
    return v;
}


VALUE
na_store(VALUE self, VALUE src)
{
    return rb_funcall(self,rb_intern("store"),1,src);
}

// ---------------------------------------------------------------------

#define m_swap_byte(q1,q2)       \
    {                            \
        size_t j;                \
        memcpy(b1,q1,e);         \
        for (j=0; j<e; j++) {    \
            b2[e-1-j] = b1[j];   \
        }                        \
        memcpy(q2,b2,e);         \
    }

static void
iter_swap_byte(na_loop_t *const lp)
{
    char   *b1, *b2;
    size_t  e;

    e = lp->args[0].elmsz;
    b1 = ALLOCA_N(char, e);
    b2 = ALLOCA_N(char, e);
    LOOP_UNARY_PTR(lp,m_swap_byte);
}

static VALUE
nary_swap_byte(VALUE self)
{
    VALUE v;
    ndfunc_arg_in_t ain[1] = {{Qnil,0}};
    ndfunc_arg_out_t aout[1] = {{INT2FIX(0),0}};
    ndfunc_t ndf = { iter_swap_byte, FULL_LOOP|NDF_ACCEPT_BYTESWAP,
                     1, 1, ain, aout };

    v = na_ndloop(&ndf, 1, self);
    if (self!=v) {
        na_copy_flags(self, v);
    }
    REVERSE_BYTE_SWAPPED(v);
    return v;
}


static VALUE
nary_to_network(VALUE self)
{
    if (TEST_NETWORK_ORDER(self)) {
        return self;
    }
    return rb_funcall(self, rb_intern("swap_byte"), 0);
}

static VALUE
nary_to_vacs(VALUE self)
{
    if (TEST_VACS_ORDER(self)) {
        return self;
    }
    return rb_funcall(self, rb_intern("swap_byte"), 0);
}

static VALUE
nary_to_host(VALUE self)
{
    if (TEST_HOST_ORDER(self)) {
        return self;
    }
    return rb_funcall(self, rb_intern("swap_byte"), 0);
}

static VALUE
nary_to_swapped(VALUE self)
{
    if (TEST_BYTE_SWAPPED(self)) {
        return self;
    }
    return rb_funcall(self, rb_intern("swap_byte"), 0);
}


//----------------------------------------------------------------------


VALUE
na_transpose_map(VALUE self, int *map)
{
    int  i, ndim;
    size_t *shape;
    stridx_t *stridx;
    narray_view_t *na;
    volatile VALUE view;

    view = na_make_view(self);
    GetNArrayView(view,na);

    ndim = na->base.ndim;
    shape = ALLOCA_N(size_t,ndim);
    stridx = ALLOCA_N(stridx_t,ndim);

    for (i=0; i<ndim; i++) {
	shape[i] = na->base.shape[i];
	stridx[i] = na->stridx[i];
    }
    for (i=0; i<ndim; i++) {
	na->base.shape[i] = shape[map[i]];
	na->stridx[i] = stridx[map[i]];
    }
    return view;
}


#define SWAP(a,b,tmp) {tmp=a;a=b;b=tmp;}

VALUE
na_transpose(int argc, VALUE *argv, VALUE self)
{
    int ndim, *map, tmp;
    int row_major;
    int i, j, c, r;
    size_t len;
    ssize_t beg, step;
    volatile VALUE v, view;
    narray_t *na1;

    GetNArray(self,na1);
    ndim = na1->ndim;
    row_major = TEST_COLUMN_MAJOR( self );

    map = ALLOCA_N(int,ndim);
    for (i=0;i<ndim;i++) {
	map[i] = i;
    }
    if (argc==0) {
	SWAP(map[ndim-1], map[ndim-2], tmp);
	goto new_object;
    }
    if (argc==2) {
	if (TYPE(argv[0])==T_FIXNUM && TYPE(argv[1])==T_FIXNUM) {
	    i = FIX2INT(argv[0]);
	    j = FIX2INT(argv[1]);
	    if (row_major) {
		i = ndim-1-i;
		j = ndim-1-j;
	    }
	    SWAP( map[i], map[j], tmp );
	    goto new_object;
	}
    }
    for (i=argc,c=ndim-1; i;) {
	v = argv[--i];
	if (TYPE(v)==T_FIXNUM) {
	    beg = FIX2INT(v);
	    len = 1;
	    step = 0;
	} else if (rb_obj_is_kind_of(v,rb_cRange) || rb_obj_is_kind_of(v,na_cStep)) {
            // write me
	    nary_step_array_index(v, ndim, &len, &beg, &step);
            //printf("len=%d beg=%d step=%d\n",len,beg,step);
	}
	for (j=len; j; ) {
	    r = beg + step*(--j);
	    if (row_major) {
		r = ndim-1-r;
            }
	    if ( c < 0 ) {
		rb_raise(rb_eArgError, "too many dims");
            }
	    map[c--] = r;
	    //printf("r=%d\n",r);
	}
    }

 new_object:
    view = na_transpose_map(self,map);
    return view;
}

//----------------------------------------------------------------------

/* private function for reshape */
static VALUE
na_reshape(int argc, VALUE *argv, VALUE self)
{
    int    i, unfixed=-1;
    size_t total=1;
    size_t *shape; //, *shape_save;
    narray_t *na;
    VALUE    copy;

    if (argc == 0) {
        rb_raise(rb_eRuntimeError, "No argrument");
    }
    GetNArray(self,na);
    if (NA_SIZE(na) == 0) {
        rb_raise(rb_eRuntimeError, "cannot reshape empty array");
    }

    /* get shape from argument */
    shape = ALLOCA_N(size_t,argc);
    for (i=0; i<argc; ++i) {
        switch(TYPE(argv[i])) {
        case T_FIXNUM:
            total *= shape[i] = NUM2INT(argv[i]);
            break;
        case T_NIL:
        case T_TRUE:
            unfixed = i;
            break;
        default:
            rb_raise(rb_eArgError,"illegal type");
        }
    }

    if (unfixed>=0) {
        if (NA_SIZE(na) % total != 0)
            rb_raise(rb_eArgError, "Total size size must be divisor");
        shape[unfixed] = NA_SIZE(na) / total;
    }
    else if (total !=  NA_SIZE(na)) {
        rb_raise(rb_eArgError, "Total size must be same");
    }

    copy = na_copy(self);
    GetNArray(copy,na);
    //shape_save = NA_SHAPE(na);
    na_setup_shape(na,argc,shape);
    //if (NA_SHAPE(na) != shape_save) {
    //    xfree(shape_save);
    //}
    return copy;
}


//----------------------------------------------------------------------

VALUE
na_flatten_dim(VALUE self, int sd)
{
    int i, nd, fd;
    size_t j;
    size_t *c, *pos, *idx1, *idx2;
    size_t stride;
    size_t  *shape, size;
    stridx_t sdx;
    narray_t *na;
    narray_view_t *na1, *na2;
    volatile VALUE view;

    GetNArray(self,na);
    nd = na->ndim;

    if (sd<0 || sd>=nd) {
        rb_bug("na_flaten_dim: start_dim (%d) out of range",sd);
    }

    // new shape
    shape = ALLOCA_N(size_t,sd+1);
    for (i=0; i<sd; i++) {
        shape[i] = na->shape[i];
    }
    size = 1;
    for (i=sd; i<nd; i++) {
        size *= na->shape[i];
    }
    shape[sd] = size;

    // new object
    view = na_s_allocate_view(CLASS_OF(self));
    na_copy_flags(self, view);
    GetNArrayView(view, na2);

    // new stride
    na_setup_shape((narray_t*)na2, sd+1, shape);
    na2->stridx = ALLOC_N(stridx_t,sd+1);

    switch(na->type) {
    case NARRAY_DATA_T:
    case NARRAY_FILEMAP_T:
        stride = na_get_elmsz(self);
        for (i=sd+1; i--; ) {
            //printf("data: i=%d stride=%d\n",i,stride);
            SDX_SET_STRIDE(na2->stridx[i],stride);
            stride *= shape[i];
        }
        na2->offset = 0;
        na2->data = self;
        break;
    case NARRAY_VIEW_T:
        GetNArrayView(self, na1);
        na2->data = na1->data;
        na2->offset = na1->offset;
        for (i=0; i<sd; i++) {
            if (SDX_IS_INDEX(na1->stridx[i])) {
                idx1 = SDX_GET_INDEX(na1->stridx[i]);
                idx2 = ALLOC_N(size_t, shape[i]);
                for (j=0; j<shape[i]; j++) {
                    idx2[j] = idx1[j];
                }
                SDX_SET_INDEX(na2->stridx[i],idx2);
            } else {
                na2->stridx[i] = na1->stridx[i];
                //printf("view: i=%d stridx=%d\n",i,SDX_GET_STRIDE(sdx));
            }
        }
        // flat dimenion == last dimension
        if (RTEST(na_check_ladder(self,sd))) {
        //if (0) {
            na2->stridx[sd] = na1->stridx[nd-1];
        } else {
            // set index
            idx2 = ALLOC_N(size_t, shape[sd]);
            SDX_SET_INDEX(na2->stridx[sd],idx2);
            // init for md-loop
            fd = nd-sd;
            c = ALLOC_N(size_t, fd);
            for (i=0; i<fd; i++) c[i]=0;
            pos = ALLOC_N(size_t, fd+1);
            pos[0] = 0;
            // md-loop
            for (i=j=0;;) {
                for (; i<fd; i++) {
                    sdx = na1->stridx[i+sd];
                    if (SDX_IS_INDEX(sdx)) {
                        pos[i+1] = pos[i] + SDX_GET_INDEX(sdx)[c[i]];
                    } else {
                        pos[i+1] = pos[i] + SDX_GET_STRIDE(sdx)*c[i];
                    }
                }
                idx2[j++] = pos[i];
                for (;;) {
                    if (i==0) goto loop_end;
                    i--;
                    c[i]++;
                    if (c[i] < na1->base.shape[i+sd]) break;
                    c[i] = 0;
                }
            }
        loop_end:
            xfree(pos);
            xfree(c);
        }
        break;
    }
    return view;
}

VALUE
na_flatten(VALUE self)
{
    return na_flatten_dim(self,0);
}


VALUE
 na_flatten_by_reduce(int argc, VALUE *argv, VALUE self)
{
    size_t  sz_reduce=1;
    int     i, j, ndim;
    int     nd_reduce=0, nd_rest=0;
    int    *dim_reduce, *dim_rest;
    int    *map;
    volatile VALUE view, reduce;
    narray_t *na;

    //puts("pass1");
    //rb_p(self);
    reduce = na_reduce_dimension(argc, argv, 1, &self);
    //reduce = INT2FIX(1);
    //rb_p(self);
    //puts("pass2");

    if (reduce==INT2FIX(0)) {
	//puts("pass flatten_dim");
        //rb_funcall(self,rb_intern("debug_info"),0);
        //rb_p(self);
	view = na_flatten_dim(self,0);
        //rb_funcall(view,rb_intern("debug_info"),0);
        //rb_p(view);
    } else {
        //printf("reduce=0x%x\n",NUM2INT(reduce));
	GetNArray(self,na);
	ndim = na->ndim;
	if (ndim==0) {
	    rb_raise(rb_eStandardError,"cannot flatten scalar(dim-0 array)");
	    return Qnil;
	}
	map = ALLOC_N(int,ndim);
	dim_reduce = ALLOC_N(int,ndim);
	dim_rest = ALLOC_N(int,ndim);
	for (i=0; i<ndim; i++) {
	    if (na_test_reduce( reduce, i )) {
		sz_reduce *= na->shape[i];
		//printf("i=%d, nd_reduce=%d, na->shape[i]=%ld\n", i, nd_reduce, na->shape[i]);
		dim_reduce[nd_reduce++] = i;
	    } else {
		//shape[nd_rest] = na->shape[i];
		//sz_rest *= na->shape[i];
		//printf("i=%d, nd_rest=%d, na->shape[i]=%ld\n", i, nd_rest, na->shape[i]);
		dim_rest[nd_rest++] = i;
	    }
	}
	for (i=0; i<nd_rest; i++) {
	    map[i] = dim_rest[i];
	    //printf("dim_rest[i=%d]=%d\n",i,dim_rest[i]);
	    //printf("map[i=%d]=%d\n",i,map[i]);
	}
	for (j=0; j<nd_reduce; j++,i++) {
	    map[i] = dim_reduce[j];
	    //printf("dim_reduce[j=%d]=%d\n",j,dim_reduce[j]);
	    //printf("map[i=%d]=%d\n",i,map[i]);
	}
	xfree(dim_reduce);
	xfree(dim_rest);
        //for (i=0; i<ndim; i++) {
        //    printf("map[%d]=%d\n",i,map[i]);
        //}
	//puts("pass transpose_map");
	view = na_transpose_map(self,map);
	xfree(map);
        //rb_p(view);
	//rb_funcall(view,rb_intern("debug_print"),0);

	//puts("pass flatten_dim");
	view = na_flatten_dim(view,nd_rest);
	//rb_funcall(view,rb_intern("debug_print"),0);
        //rb_p(view);
    }
    return view;
}


//----------------------------------------------------------------------

#define MIN(a,b) (((a)<(b))?(a):(b))

/*
  Returns a diagonal view of NArray
  @overload  diagonal([offset,axes])
  @param [Integer] offset  Diagonal offset from the main diagonal.
    The default is 0. k>0 for diagonals above the main diagonal,
    and k<0 for diagonals below the main diagonal.
  @param [Array] axes  Array of axes to be used as the 2-d sub-arrays
    from which the diagonals should be taken. Defaults to last-two
    axes ([-2,-1]).
  @return [Numo::NArray]  diagonal view of NArray.
  @example
    a = Numo::DFloat.new(4,5).seq
    => Numo::DFloat#shape=[4,5]
    [[0, 1, 2, 3, 4],
     [5, 6, 7, 8, 9],
     [10, 11, 12, 13, 14],
     [15, 16, 17, 18, 19]]
    b = a.diagonal(1)
    => Numo::DFloat(view)#shape=[4]
    [1, 7, 13, 19]
    b.store(0)
    a
    => Numo::DFloat#shape=[4,5]
    [[0, 0, 2, 3, 4],
     [5, 6, 0, 8, 9],
     [10, 11, 12, 0, 14],
     [15, 16, 17, 18, 0]]
    b.store([1,2,3,4])
    a
    => Numo::DFloat#shape=[4,5]
    [[0, 1, 2, 3, 4],
     [5, 6, 2, 8, 9],
     [10, 11, 12, 3, 14],
     [15, 16, 17, 18, 4]]
 */
VALUE
na_diagonal(int argc, VALUE *argv, VALUE self)
{
    int  i, k, nd;
    size_t  j;
    size_t *idx0, *idx1, *diag_idx;
    size_t *shape;
    size_t  diag_size;
    ssize_t stride, stride0, stride1;
    narray_t *na;
    narray_view_t *na1, *na2;
    VALUE view;
    VALUE vofs=0, vaxes=0;
    ssize_t kofs;
    size_t k0, k1;
    int ax[2];

    // check arguments
    if (argc>2) {
        rb_raise(rb_eArgError,"too many arguments (%d for 0..2)",argc);
    }

    for (i=0; i<argc; i++) {
        switch(TYPE(argv[i])) {
        case T_FIXNUM:
            if (vofs) {
                rb_raise(rb_eArgError,"offset is given twice");
            }
            vofs = argv[i];
            break;
        case T_ARRAY:
            if (vaxes) {
                rb_raise(rb_eArgError,"axes-array is given twice");
            }
            vaxes = argv[i];
            break;
        }
    }

    if (vofs) {
        kofs = NUM2SSIZE(vofs);
    } else {
        kofs = 0;
    }

    GetNArray(self,na);
    nd = na->ndim;
    if (nd < 2) {
        rb_raise(nary_eDimensionError,"less than 2-d array");
    }

    if (vaxes) {
        if (RARRAY_LEN(vaxes) != 2) {
            rb_raise(rb_eArgError,"axes must be 2-element array");
        }
        ax[0] = NUM2INT(RARRAY_AREF(vaxes,0));
        ax[1] = NUM2INT(RARRAY_AREF(vaxes,1));
        if (ax[0]<-nd || ax[0]>=nd || ax[1]<-nd || ax[1]>=nd) {
            rb_raise(rb_eArgError,"axis out of range:[%d,%d]",ax[0],ax[1]);
        }
        if (ax[0]<0) {ax[0] += nd;}
        if (ax[1]<0) {ax[1] += nd;}
        if (ax[0]==ax[1]) {
            rb_raise(rb_eArgError,"same axes:[%d,%d]",ax[0],ax[1]);
        }
    } else {
        ax[0] = nd-2;
        ax[1] = nd-1;
    }

    // Diagonal offset from the main diagonal.
    if (kofs >= 0) {
        k0 = 0;
        k1 = kofs;
        if (k1 >= na->shape[ax[1]]) {
            rb_raise(rb_eArgError,"invalid diagonal offset(%ld) for "
                     "last dimension size(%ld)",kofs,na->shape[ax[1]]);
        }
    } else {
        k0 = -kofs;
        k1 = 0;
        if (k0 >= na->shape[ax[0]]) {
            rb_raise(rb_eArgError,"invalid diagonal offset(=%ld) for "
                     "last-1 dimension size(%ld)",kofs,na->shape[ax[0]]);
        }
    }

    diag_size = MIN(na->shape[ax[0]]-k0,na->shape[ax[1]]-k1);

    // new shape
    shape = ALLOCA_N(size_t,nd-1);
    for (i=k=0; i<nd; i++) {
        if (i != ax[0] && i != ax[1]) {
            shape[k++] = na->shape[i];
        }
    }
    shape[k] = diag_size;

    // new object
    view = na_s_allocate_view(CLASS_OF(self));
    na_copy_flags(self, view);
    GetNArrayView(view, na2);

    // new stride
    na_setup_shape((narray_t*)na2, nd-1, shape);
    na2->stridx = ALLOC_N(stridx_t, nd-1);

    switch(na->type) {
    case NARRAY_DATA_T:
    case NARRAY_FILEMAP_T:
        na2->offset = 0;
        na2->data = self;
        stride = stride0 = stride1 = na_get_elmsz(self);
        for (i=nd,k=nd-2; i--; ) {
            if (i==ax[1]) {
                stride1 = stride;
                if (kofs > 0) {
                    na2->offset = kofs*stride;
                }
            } else if (i==ax[0]) {
                stride0 = stride;
                if (kofs < 0) {
                    na2->offset = (-kofs)*stride;
                }
            } else {
                SDX_SET_STRIDE(na2->stridx[--k],stride);
            }
            stride *= na->shape[i];
        }
        SDX_SET_STRIDE(na2->stridx[nd-2],stride0+stride1);
        break;

    case NARRAY_VIEW_T:
        GetNArrayView(self, na1);
        na2->data = na1->data;
        na2->offset = na1->offset;
        for (i=k=0; i<nd; i++) {
            if (i != ax[0] && i != ax[1]) {
                if (SDX_IS_INDEX(na1->stridx[i])) {
                    idx0 = SDX_GET_INDEX(na1->stridx[i]);
                    idx1 = ALLOC_N(size_t, na->shape[i]);
                    for (j=0; j<na->shape[i]; j++) {
                        idx1[j] = idx0[j];
                    }
                    SDX_SET_INDEX(na2->stridx[k],idx1);
                } else {
                    na2->stridx[k] = na1->stridx[i];
                }
                k++;
            }
        }
        if (SDX_IS_INDEX(na1->stridx[ax[0]])) {
            idx0 = SDX_GET_INDEX(na1->stridx[ax[0]]);
            diag_idx = ALLOC_N(size_t, diag_size);
            if (SDX_IS_INDEX(na1->stridx[ax[1]])) {
                idx1 = SDX_GET_INDEX(na1->stridx[ax[1]]);
                for (j=0; j<diag_size; j++) {
                    diag_idx[j] = idx0[j+k0] + idx1[j+k1];
                }
            } else {
                stride1 = SDX_GET_STRIDE(na1->stridx[ax[1]]);
                for (j=0; j<diag_size; j++) {
                    diag_idx[j] = idx0[j+k0] + stride1*(j+k1);
                }
            }
            SDX_SET_INDEX(na2->stridx[nd-2],diag_idx);
        } else {
            stride0 = SDX_GET_STRIDE(na1->stridx[ax[0]]);
            if (SDX_IS_INDEX(na1->stridx[ax[1]])) {
                idx1 = SDX_GET_INDEX(na1->stridx[ax[1]]);
                diag_idx = ALLOC_N(size_t, diag_size);
                for (j=0; j<diag_size; j++) {
                    diag_idx[j] = stride0*(j+k0) + idx1[j+k1];
                }
                SDX_SET_INDEX(na2->stridx[nd-2],diag_idx);
            } else {
                stride1 = SDX_GET_STRIDE(na1->stridx[ax[1]]);
                na2->offset += stride0*k0 + stride1*k1;
                SDX_SET_STRIDE(na2->stridx[nd-2],stride0+stride1);
            }
        }
        break;
    }
    return view;
}

//----------------------------------------------------------------------


#ifdef SWAP
#undef SWAP
#endif
#define SWAP(a,b,t) {t=a;a=b;b=t;}

static VALUE
na_new_dimension_for_dot(VALUE self, int pos, int len, bool transpose)
{
    int i, k, nd;
    size_t  j;
    size_t *idx1, *idx2;
    size_t *shape;
    ssize_t stride;
    narray_t *na;
    narray_view_t *na1, *na2;
    size_t shape_n;
    stridx_t stridx_n;
    volatile VALUE view;

    GetNArray(self,na);
    nd = na->ndim;

    view = na_s_allocate_view(CLASS_OF(self));

    na_copy_flags(self, view);
    GetNArrayView(view, na2);

    // new dimension
    if (pos < 0) pos += nd;
    if (pos > nd || pos < 0) {
        rb_raise(rb_eRangeError,"new dimension is out of range");
    }
    nd += len;
    shape = ALLOCA_N(size_t,nd);
    i = k = 0;
    while (i < nd) {
        if (i == pos) {
            for (; len; len--) {
                shape[i++] = 1;
            }
            pos = -1; // new axis done
        } else {
            shape[i++] = na->shape[k++];
        }
    }

    na_setup_shape((narray_t*)na2, nd, shape);
    na2->stridx = ALLOC_N(stridx_t,nd);

    switch(na->type) {
    case NARRAY_DATA_T:
    case NARRAY_FILEMAP_T:
        stride = na_get_elmsz(self);
        for (i=nd; i--;) {
            SDX_SET_STRIDE(na2->stridx[i],stride);
            stride *= shape[i];
        }
        na2->offset = 0;
        na2->data = self;
        break;
    case NARRAY_VIEW_T:
        GetNArrayView(self, na1);
        for (i=0; i<nd; i++) {
            if (SDX_IS_INDEX(na1->stridx[i])) {
                idx1 = SDX_GET_INDEX(na1->stridx[i]);
                idx2 = ALLOC_N(size_t,na1->base.shape[i]);
                for (j=0; j<na1->base.shape[i]; j++) {
                    idx2[j] = idx1[j];
                }
                SDX_SET_INDEX(na2->stridx[i],idx2);
            } else {
                na2->stridx[i] = na1->stridx[i];
            }
        }
        na2->offset = na1->offset;
        na2->data = na1->data;
        break;
    }

    if (transpose) {
	SWAP(na2->base.shape[nd-1], na2->base.shape[nd-2], shape_n);
	SWAP(na2->stridx[nd-1], na2->stridx[nd-2], stridx_n);
    }

    return view;
}


//----------------------------------------------------------------------

/*
 *  call-seq:
 *     narray.dot(other) => narray
 *
 *  Returns dot product.
 *
 */

static VALUE
numo_na_dot(VALUE self, VALUE other)
{
    VALUE test, sym_mulsum;
    volatile VALUE a1=self, a2=other;
    ID id_mulsum;
    narray_t *na1, *na2;

    id_mulsum = rb_intern("mulsum");
    sym_mulsum = ID2SYM(id_mulsum);
    test = rb_funcall(a1, rb_intern("respond_to?"), 1, sym_mulsum);
    if (!RTEST(test)) {
        rb_raise(rb_eNoMethodError,"requires mulsum method for dot method");
    }
    GetNArray(a1,na1);
    GetNArray(a2,na2);
    if (na2->ndim > 1) {
        // insert new axis [ ..., last-1-dim, newaxis*other.ndim, last-dim ]
        a1 = na_new_dimension_for_dot(a1, na1->ndim-1, na2->ndim-1, 0);
        // insert & transpose [ newaxis*self.ndim, ..., last-dim, last-1-dim ]
        a2 = na_new_dimension_for_dot(a2, 0, na1->ndim-1, 1);
    }
    return rb_funcall(a1,rb_intern("mulsum"),2,a2,INT2FIX(-1));
}


void
Init_nary_data()
{
    rb_define_method(cNArray, "copy", na_copy, 0);

    rb_define_method(cNArray, "flatten", na_flatten, 0);
    rb_define_method(cNArray, "transpose", na_transpose, -1);

    rb_define_method(cNArray, "reshape", na_reshape,-1);
    /*
    rb_define_method(cNArray, "reshape!", na_reshape_bang,-1);
    rb_define_alias(cNArray,  "shape=","reshape!");
    */
    rb_define_method(cNArray, "diagonal", na_diagonal,-1);

    rb_define_method(cNArray, "swap_byte", nary_swap_byte, 0);
#ifdef DYNAMIC_ENDIAN
#else
#ifdef WORDS_BIGENDIAN
#else // LITTLE_ENDIAN
    rb_define_alias(cNArray, "hton", "swap_byte");
    rb_define_alias(cNArray, "hton", "swap_byte");
    rb_define_alias(cNArray, "network_order?", "byte_swapped?");
    rb_define_alias(cNArray, "little_endian?", "host_order?");
    rb_define_alias(cNArray, "vacs_order?", "host_order?");
#endif
#endif
    rb_define_method(cNArray, "to_network", nary_to_network, 0);
    rb_define_method(cNArray, "to_vacs", nary_to_vacs, 0);
    rb_define_method(cNArray, "to_host", nary_to_host, 0);
    rb_define_method(cNArray, "to_swapped", nary_to_swapped, 0);

    rb_define_method(cNArray, "dot", numo_na_dot, 1);
}
