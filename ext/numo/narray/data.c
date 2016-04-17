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

    map = ALLOC_N(int,ndim);
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
    xfree(map);
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


/*
 *  call-seq:
 *     narray.sort() => narray
 *     narray.sort(dim) => narray
 *
 *  Return an index array of sort result.
 *
 *     Numo::DFloat[3,4,1,2].sort_index => Numo::DFloat[1,2,3,4]
 */

static VALUE
na_sort_view(int argc, VALUE *argv, VALUE self)
{
    int sort_dim;
    int ndim;
    int i;
    size_t tmp_shp;
    stridx_t tmp_sdx;
    narray_t *na;
    narray_view_t *nav;
    volatile VALUE view;

    if (argc==0) {
        view = na_flatten_dim(self,0);
    }
    else if (argc==1) {
        GetNArray(self,na);
        ndim = na->ndim;

        // sort dimension
        sort_dim = NUM2INT(argv[0]);
        if (sort_dim < -ndim || sort_dim >= ndim) {
            rb_raise(rb_eArgError,"dimension out of range (%d for %d)", sort_dim, ndim);
        }
        if (sort_dim < 0) {
            sort_dim += ndim;
        }

        // view
        if (NA_TYPE(self) == NARRAY_VIEW_T) {
            view = self;
        } else {
            view = na_make_view(self);
        }
        GetNArrayView(view,nav);

        // transpose
        if (sort_dim != ndim-1) {
            tmp_sdx = nav->stridx[sort_dim];
            tmp_shp = nav->base.shape[sort_dim];
            for (i=sort_dim; i<ndim-1; i++) {
                nav->stridx[i] = nav->stridx[i+1];
                nav->base.shape[i] = nav->base.shape[i+1];
            }
            nav->stridx[i] = tmp_sdx;
            nav->base.shape[i] = tmp_shp;
        }
    }
    else {
        rb_raise(rb_eArgError,"wrong number of arguments (%d for 1 or 0)",argc);
        return Qnil;
    }
    return view;
}

VALUE
na_sort_main(int argc, VALUE *argv, volatile VALUE self, na_iter_func_t iter_func)
{
    int ndim;
    narray_t *na;
    narray_view_t *nav;
    volatile VALUE view;
    int elmsz;
    char *buf;

    ndfunc_arg_in_t ain[1] = {{Qnil,1}}; /* user.dim=1 */
    ndfunc_t ndf = { iter_func, NO_LOOP, 1, 0, ain, 0 };


    if (!TEST_INPLACE(self)) {
        self = na_copy(self);
    }

    GetNArray(self,na);
    ndim = na->ndim;
    if (ndim==0) {
        return self;
    }

    view = na_sort_view(argc, argv, self);
    GetNArrayView(view,nav);
    ndim = nav->base.ndim;

    if (SDX_IS_INDEX(nav->stridx[ndim-1])) {
        elmsz = na_get_elmsz(view);
        buf = ALLOC_N(char, elmsz * nav->base.shape[ndim-1]);
        // info = Data_Wrap_Struct(rb_cData,0,0,buf);
    } else {
        buf = NULL;
    }
    //func = ndfunc_alloc(iter_func, NO_LOOP, 1, 0, Qnil);
    //func->args[0].dim = 1;
    //ndloop_do3(func, buf, 1, view);
    na_ndloop3(&ndf, buf, 1, view);
    if (buf) xfree(buf);
    //ndfunc_free(func);

    return self;
}


VALUE
na_median_main(int argc, VALUE *argv, volatile VALUE self, na_iter_func_t iter_func)
{
    int ndim;
    narray_t *na;
    narray_view_t *nav;
    volatile VALUE view, result;
    int elmsz;
    char *buf;

    ndfunc_arg_in_t ain[1] = {{Qnil,1}}; /* user.dim=1 */
    ndfunc_arg_out_t aout[1] = {{INT2FIX(0),0}};
    //ndfunc_t ndf = { iter_func, NO_LOOP, 1, 0, ain, 0 };
    ndfunc_t ndf = { iter_func, NO_LOOP, 1, 1, ain, aout };

    GetNArray(self,na);
    ndim = na->ndim;
    if (ndim==0) {
        return self;
    }

    view = na_sort_view(argc, argv, self);
    GetNArrayView(view,nav);
    ndim = nav->base.ndim;

    elmsz = na_get_elmsz(view);
    //printf("ndim=%d elmsz=%d  nav->base.shape[ndim-1]=%d\n",
    //       ndim, elmsz, nav->base.shape[ndim-1]);
    buf = ALLOC_N(char, elmsz * nav->base.shape[ndim-1]);

    //func = ndfunc_alloc(iter_func, NO_LOOP, 1, 1, Qnil, INT2FIX(0));
    //func->args[0].dim = 1;
    //result = ndloop_do3(func, buf, 1, view);
    result = na_ndloop3(&ndf, buf, 1, view);

    xfree(buf);

    return result;
}


//----------------------------------------------------------------------

static void
iter_sort_index(na_loop_t *const lp)
{
    size_t i, n;
    char *d_ptr, *i_ptr;
    ssize_t d_step, i_step;
    size_t *d_idx, *i_idx;
    ssize_t d_es, i_es;
    ssize_t j, j_ofs, j_step;
    char **ptr;
    char *buf, *d;
    void (*func_qsort)();

    //printf("lp->iter[0].pos=%d\n",lp->iter[0].pos);
    //printf("lp->iter[1].pos=%d\n",lp->iter[1].pos);

    //INIT_COUNTER(lp, n);
    n = lp->args[0].shape[0];
    INIT_PTR_IDX(lp, 0, d_ptr, d_step, d_idx);
    INIT_ELMSIZE(lp, 0, d_es);
    INIT_PTR_IDX(lp, 1, i_ptr, i_step, i_idx);
    INIT_ELMSIZE(lp, 1, i_es);
    if (i_idx) {rb_bug("i_idx is not null");}

    j_ofs = (i_ptr - lp->args[1].ptr) / i_es;
    j_step = i_step / i_es;

    //printf("j_ofs=%d j_step=%d\n",j_ofs,j_step);
    //printf("d_es=%d d_ptr=0x%x d_step=%d\n",d_es,d_ptr,d_step);
    //printf("i_es=%d i_ptr=0x%x i_step=%d\n",i_es,i_ptr,i_step);

    // buffer
    //Data_Get_Struct(lp->info, char, buf);
    buf = (char*)(lp->opt_ptr);
    memcpy(&func_qsort, buf, sizeof(func_qsort));
    //printf("func_qsort=0x%x\n",func_qsort);
    buf += sizeof(func_qsort);
    ptr = (char**)buf;
    //printf("ptr=0x%x\n",ptr);

    if (d_idx) {
        buf += sizeof(char*)*n;
        for (d=buf,i=0; i<n; i++) {
            // copy data to buffer
            memcpy(d, d_ptr+d_idx[i], d_es);
            // pointer to buffer
            ptr[i] = d;
            d += d_es;
        }
        // index sort
        (*func_qsort)(ptr, n, sizeof(void*));
        // convert pointer to index
        switch(i_es) {
        case 4:
            for (i=0; i<n; i++) {
                j = (ptr[i]-buf)/d_es;
                *(int32_t*)i_ptr = (int32_t)(j*j_step+j_ofs);
                i_ptr += i_step;
            }
            break;
        case 8:
            for (i=0; i<n; i++) {
                j = (ptr[i]-buf)/d_es;
                *(int64_t*)i_ptr = (int64_t)(j*j_step+j_ofs);
                i_ptr += i_step;
            }
            break;
        default:
            rb_bug("unexpected index type : %lu-byte element",i_es);
        }
    } else {
        for (i=0; i<n; i++) {
            // pointer to data
            ptr[i] = d_ptr+d_step*i;
            //printf("i=%d ptr=0x%x\n",i, ptr[i]);
        }
        // index sort
        (*func_qsort)(ptr, n, sizeof(void*));
        // convert pointer to index
        switch(i_es) {
        case 4:
            for (i=0; i<n; i++) {
                j = (ptr[i]-d_ptr)/d_step;
                *(int32_t*)i_ptr = (int32_t)(j*j_step+j_ofs);
                //printf("i=%d idx=%d\n",i, *(int32_t*)i_ptr);
                i_ptr += i_step;
            }
            break;
        case 8:
            for (i=0; i<n; i++) {
                j = (ptr[i]-d_ptr)/d_step;
                *(int64_t*)i_ptr = (int64_t)(j*j_step+j_ofs);
                i_ptr += i_step;
            }
            break;
        default:
            rb_bug("unexpected index type : %lu-byte element",i_es);
        }
    }
}


/*
 *  call-seq:
 *     narray.sort_index() => narray
 *     narray.sort_index(dim) => narray
 *
 *  Return an index array of sort result.
 *
 *     Numo::NArray[3,4,1,2].sort_index => Numo::Int32[2,3,0,1]
 */

VALUE
na_sort_index_main(int argc, VALUE *argv, VALUE self,
                   void (*func_qsort)())
{
    size_t size;
    int elmsz;
    narray_t *na;
    narray_view_t *nav;
    volatile VALUE idx, self_view, idx_view;
    char *buf;
    ndfunc_arg_in_t ain[2] = {{Qnil,1},{Qnil,1}}; /* user.dim=1 */
    ndfunc_t ndf = { iter_sort_index, NO_LOOP, 2, 0, ain, 0 };

    GetNArray(self,na);
    if (na->ndim==0) {
        return INT2FIX(0);
    }

    if (na->size > (~(u_int32_t)0)) {
        ;
    } else {
        idx = rb_narray_new(numo_cInt32, na->ndim, na->shape);
    }
    rb_funcall(idx, rb_intern("allocate"), 0);

    idx_view = na_sort_view(argc, argv, idx);
    self_view = na_sort_view(argc, argv, self);

    GetNArrayView(self_view, nav);

    // alloc buffer and wrap it as an object
    size = sizeof(func_qsort);
    size += nav->base.shape[nav->base.ndim-1] * sizeof(void*);
    if (SDX_IS_INDEX(nav->stridx[nav->base.ndim-1])) {
        elmsz = na_get_elmsz(self_view);
        size = nav->base.shape[nav->base.ndim-1] * elmsz;
    }
    //printf("alloc_size = %d\n",size);
    buf = ALLOC_N(char,size);
    //printf("buf=0x%x\n",buf);
    // info = Data_Wrap_Struct(rb_cData,0,0,buf);
    // pass the qsort function
    memcpy(buf, &func_qsort, sizeof(void (*)()));

    //func = ndfunc_alloc(iter_sort_index, NO_LOOP, 2, 0, Qnil, Qnil);
    //func->args[0].dim = 1;
    //func->args[1].dim = 1;
    //ndloop_do3(func, buf, 2, self_view, idx_view);
    na_ndloop3(&ndf, buf, 2, self_view, idx_view);
    xfree(buf);
    //ndfunc_free(func);
    return idx;
}


#ifdef SWAP
#undef SWAP
#endif
#define SWAP(a,b,t) {t=a;a=b;b=t;}

static VALUE
na_new_dimension_for_dot(VALUE self, int pos, int len, boolean transpose)
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
