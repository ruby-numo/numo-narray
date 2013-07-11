/*
  data.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-201 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

#include <ruby.h>
#include "narray.h"

#include "template.h"

/*
VALUE cDFloat, cDComplex;
VALUE cInt32, cInt24, cInt16, cInt8;
VALUE cUInt32, cUInt24, cUInt16, cUInt8;
VALUE cInt64, cInt48;
VALUE cUInt64, cUInt48;
VALUE cBit, cRObject;
VALUE cPointer;

VALUE cComplex;

ID id_add;
ID id_sub;
ID id_mul;
ID id_div;
ID id_mod;
ID id_pow;
ID id_bit_and;
ID id_bit_or;
ID id_bit_xor;
ID id_eq;
ID id_ne;
ID id_gt;
ID id_ge;
ID id_lt;
ID id_le;

*/


VALUE
nary_s_upcast(VALUE type1, VALUE type2)
{
    VALUE upcast_hash;
    VALUE result_type;

    //if (TYPE(type2)==T_CLASS) {
    //	if (RTEST(rb_class_inherited_p(type2,rb_cInteger))) {
    //	    type2 = rb_cInteger;
    //	}
    //}
    if (type1==type2) return type1;
    upcast_hash = rb_const_get(type1, rb_intern("UPCAST"));
    result_type = rb_hash_aref(upcast_hash, type2);
    if (NIL_P(result_type)) {
	if (TYPE(type2)==T_CLASS) {
	    if (RTEST(rb_class_inherited_p(type2,cNArray))) {
		upcast_hash = rb_const_get(type2, rb_intern("UPCAST"));
		result_type = rb_hash_aref(upcast_hash, type1);
	    }
	}
    }
    return result_type;
}


static VALUE
nary_coerce(VALUE x, VALUE y)
{
    VALUE type;

    type = nary_s_upcast(CLASS_OF(x), CLASS_OF(y));
    //puts("pass1");
    //y = nary_s_cast(type,y);
    y = rb_funcall(type,rb_intern("cast"),1,y);
    //puts("pass2");
    return rb_assoc_new(y, x);
}


/*
VALUE
na_dispatch_operation(VALUE self, VALUE other, ID opid)
{
    VALUE type;
    type = nary_s_upcast(CLASS_OF(self), CLASS_OF(other));
    if (rb_obj_is_kind_of(type, rb_cClass)) {
        if (RTEST(rb_class_inherited_p(type, cNArray))) {
	    return rb_funcall(type, opid, 2, self, other);
	}
    }
    rb_raise(nary_eCastError, "upcast failed for %s and %s",
	     rb_class2name(CLASS_OF(self)),rb_class2name(CLASS_OF(other)));
    return Qnil;
}

#define DISPATCH_FUNC(opname)				\
 VALUE na_##opname(VALUE self, VALUE other)		\
 {  na_dispatch_operation(self, other, id_##opname); }\

DISPATCH_FUNC(add)
DISPATCH_FUNC(sub)
DISPATCH_FUNC(mul)
DISPATCH_FUNC(div)
DISPATCH_FUNC(mod)
//DISPATCH_FUNC(pow)
DISPATCH_FUNC(eq)
DISPATCH_FUNC(ne)
DISPATCH_FUNC(gt)
DISPATCH_FUNC(ge)
DISPATCH_FUNC(lt)
DISPATCH_FUNC(le)
DISPATCH_FUNC(bit_and)
DISPATCH_FUNC(bit_or)
DISPATCH_FUNC(bit_xor)


VALUE
na_pow(VALUE self, VALUE other)
{
    VALUE type;
    if (rb_obj_is_kind_of(other,cInt32)) {
        return rb_funcall(CLASS_OF(self), id_pow, 2, self, other);
    } else {
        type = nary_s_upcast(CLASS_OF(self), CLASS_OF(other));
	if (rb_obj_is_kind_of(type, rb_cClass)) {
	    if (RTEST(rb_class_inherited_p(type, cNArray))) {
	      return rb_funcall(type, id_pow, 2, self, other);
	    }
	}
    }
    rb_raise(nary_eCastError, "upcast failed for %s and %s",
	     rb_class2name(CLASS_OF(self)),rb_class2name(CLASS_OF(other)));
    return Qnil;
}
*/

// ---------------------------------------------------------------------

#if 0
#define UNITDATA ssize_t

void
iter_copy_bytes(na_loop_t *const lp)
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    char   *q1, *q2;
    size_t *idx1, *idx2;
    size_t  j, e;

    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    e = lp->args[0].elmsz;
    for (; i--;) {
        if (idx1) {
            q1 = p1 + *idx1;
            idx1++;
        } else {
            q1 = p1;
            p1 += s1;
        }
        if (idx2) {
            q2 = p2 + *idx2;
            idx2++;
        } else {
            q2 = p2;
            p2 += s2;
        }
        for (j=e; j>=sizeof(UNITDATA); j-=sizeof(UNITDATA)) {
            *(UNITDATA*)q2 = *(UNITDATA*)q1;
            //*(UNITDATA*)q1 = *(UNITDATA*)q2;
            q1 += sizeof(UNITDATA);
            q2 += sizeof(UNITDATA);
        }
        for (; j--;) {
            *q2++ = *q1++;
            //*q1++ = *q2++;
        }
    }
}
#endif


void
iter_copy_bytes(na_loop_t *const lp)
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    char   *q1, *q2;
    size_t *idx1, *idx2;
    size_t  e;

    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    e = lp->args[0].elmsz;
    for (; i--;) {
        if (idx1) {
            q1 = p1 + *idx1;
            idx1++;
        } else {
            q1 = p1;
            p1 += s1;
        }
        if (idx2) {
            q2 = p2 + *idx2;
            idx2++;
        } else {
            q2 = p2;
            p2 += s2;
        }
        memcpy(q2, q1, e);
    }
}


VALUE
na_copy(VALUE self)
{
    ndfunc_t *func;
    volatile VALUE v;

    func = ndfunc_alloc(iter_copy_bytes, HAS_LOOP, 1, 1, Qnil, INT2FIX(0));
    v = ndloop_do(func, 1, self);
    ndfunc_free(func);
    return v;
}




VALUE
na_store(VALUE self, VALUE src)
{
    ndfunc_t *func;

    //if (na_debug_flag) rb_p(dst);
    //if (na_debug_flag) rb_p(src);

    // note that order of argument is inverted.

    func = ndfunc_alloc(iter_copy_bytes, HAS_LOOP, 2, 0, INT2FIX(1), Qnil);
    //func = ndfunc_alloc(iter_copy_bytes, HAS_LOOP, 2, 0, Qnil, INT2FIX(0));
    //ndloop_do(func, 2, dst, src);
    ndloop_do(func, 2, src, self);
    ndfunc_free(func);
    return self;
}

// ---------------------------------------------------------------------

/*
VALUE
na_flatten(VALUE self)
{
    volatile VALUE v;
    size_t *shape;
    narray_t *na;

    v = na_copy(self);
    GetNArray(v,na);

    if (na->ndim > 1) {
        shape = na->shape;
        na->shape = &(na->size);
        na->ndim = 1;
        if (shape && shape!=&(na->size)) {
            xfree(shape);
        }
    }
    return v;
}
*/

// ---------------------------------------------------------------------

 /*
static VALUE
nary_byte_size(VALUE self)
{
    VALUE velmsz;
    narray_t *na;
    size_t sz;

    GetNArray(self,na);
    velmsz = rb_const_get(CLASS_OF(self), rb_intern(ELEMENT_BYTE_SIZE));
    sz = SIZE2NUM(NUM2SIZE(velmsz) * na->size);
    return sz;
}


static VALUE
nary_s_byte_size(VALUE type)
{
    return rb_const_get(type, rb_intern(ELEMENT_BYTE_SIZE));
}
 */

// ---------------------------------------------------------------------

static void
iter_swap_byte(na_loop_t *const lp)
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    char   *q1, *q2;
    char   *b1, *b2;
    size_t *idx1, *idx2;
    size_t  e;
    size_t  j;

    INIT_COUNTER(lp, i);
    INIT_PTR_ELM(lp, 0, p1, s1, idx1, e);
    INIT_PTR(lp, 1, p2, s2, idx2);
    b1 = ALLOCA_N(char, e);
    b2 = ALLOCA_N(char, e);
    for (; i--;) {
        if (idx1) {
	    q1 = p1 + *idx1;
	    idx1++;
        } else {
	    q1 = p1;
	    p1 += s1;
	}
        if (idx2) {
	    q2 = p2 + *idx2;
	    idx2++;
        } else {
	    q2 = p2;
	    p2 += s2;
	}
	memcpy(b1,q1,e);
	for (j=0; j<e; j++) {
	  b2[e-1-j] = b1[j];
	}
	memcpy(q2,b2,e);
    }
}


static VALUE
nary_swap_byte(VALUE self)
{
    ndfunc_t *func;
    VALUE v;

    func = ndfunc_alloc(iter_swap_byte, FULL_LOOP|NDF_ACCEPT_SWAP,
                        1, 1, Qnil, CLASS_OF(self));
    v = ndloop_do(func, 1, self);
    if (self!=v) {
        na_copy_flags(self, v);
    }
    REVERSE_BYTE_SWAPPED(v);
    ndfunc_free(func);
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
        //puts("pass1");
        return self;
    }
    //puts("pass2");
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
 na_flatten_by_mark(int argc, VALUE *argv, volatile VALUE self)
{
    size_t  sz_mark=1;
    int     i, j, ndim;
    int     nd_mark=0, nd_rest=0;
    int    *dim_mark, *dim_rest;
    int    *map;
    volatile VALUE view, mark;
    narray_t *na;

    //puts("pass1");
    //rb_p(self);
    mark = na_mark_dimension(argc, argv, self);
    //mark = INT2FIX(1);
    //rb_p(self);
    //puts("pass2");

    if (mark==INT2FIX(0)) {
	//puts("pass flatten_dim");
        //rb_funcall(self,rb_intern("debug_info"),0);
        //rb_p(self);
	view = na_flatten_dim(self,0);
        //rb_funcall(view,rb_intern("debug_info"),0);
        //rb_p(view);
    } else {
        //printf("mark=0x%x\n",NUM2INT(mark));
	GetNArray(self,na);
	ndim = na->ndim;
	if (ndim==0) {
	    rb_raise(rb_eStandardError,"cannot flatten scalar(dim-0 array)");
	    return Qnil;
	}
	map = ALLOC_N(int,ndim);
	dim_mark = ALLOC_N(int,ndim);
	dim_rest = ALLOC_N(int,ndim);
	for (i=0; i<ndim; i++) {
	    if (na_test_mark( mark, i )) {
		sz_mark *= na->shape[i];
		//printf("i=%d, nd_mark=%d, na->shape[i]=%ld\n", i, nd_mark, na->shape[i]);
		dim_mark[nd_mark++] = i;
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
	for (j=0; j<nd_mark; j++,i++) {
	    map[i] = dim_mark[j];
	    //printf("dim_mark[j=%d]=%d\n",j,dim_mark[j]);
	    //printf("map[i=%d]=%d\n",i,map[i]);
	}
	xfree(dim_mark);
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
 *     NArray::DFloat[3,4,1,2].sort_index => NArray::DFloat[1,2,3,4]
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
    //volatile VALUE info;
    ndfunc_t *func;

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
    func = ndfunc_alloc(iter_func, NO_LOOP, 1, 0, Qnil);
    func->args[0].dim = 1;
    ndloop_do3(func, buf, 1, view);
    if (buf) xfree(buf);
    ndfunc_free(func);

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
    ndfunc_t *func;

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

    func = ndfunc_alloc(iter_func, NO_LOOP, 1, 1, Qnil, INT2FIX(0));
    func->args[0].dim = 1;
    result = ndloop_do3(func, buf, 1, view);

    xfree(buf);
    ndfunc_free(func);

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

    INIT_COUNTER(lp, n);
    INIT_PTR_ELM(lp, 0, d_ptr, d_step, d_idx, d_es);
    INIT_PTR_ELM(lp, 1, i_ptr, i_step, i_idx, i_es);
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
 *     NArray[3,4,1,2].sort_index => NArray::Int32[2,3,0,1]
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
    ndfunc_t *func;

    GetNArray(self,na);
    if (na->ndim==0) {
        return INT2FIX(0);
    }

    if (na->size > (~(u_int32_t)0)) {
        ;
    } else {
        idx = rb_narray_new(cInt32, na->ndim, na->shape);
    }
    na_alloc_data(idx);

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

    func = ndfunc_alloc(iter_sort_index, NO_LOOP, 2, 0, Qnil, Qnil);
    func->args[0].dim = 1;
    func->args[1].dim = 1;
    ndloop_do3(func, buf, 2, self_view, idx_view);
    xfree(buf);
    ndfunc_free(func);
    return idx;
}

void
Init_nary_data()
{
    /*
    //cDataTypeClassMethod = rb_define_module_under(mNum, "DataTypeClassMethod");

    rb_define_const(cNArray, "UPCAST", rb_hash_new());

    //rb_define_singleton_method(cNArray, "bit_step", nary_s_bit_step, 0);
    //rb_define_singleton_method(cNArray, "bit_size", nary_s_bit_size, 0);
    //rb_define_singleton_method(cNArray, "byte_size", nary_s_byte_size, 0);
    rb_define_singleton_method(cNArray, "cast", nary_s_cast, 1);
    rb_define_singleton_method(cNArray, "_cast", nary_s__cast, 1);
    rb_define_singleton_method(cNArray, "upcast", nary_s_upcast, 1);
    rb_define_singleton_method(cNArray, "cast_type", nary_s_upcast, 1);
    */

  /*
    rb_define_singleton_method(cNArray, "byte_size", nary_s_byte_size, 0);
  */

    rb_define_method(cNArray, "coerce", nary_coerce, 1);

    rb_define_method(cNArray, "copy", na_copy, 0);
    rb_define_method(cNArray, "store", na_store, 1);

    rb_define_method(cNArray, "flatten", na_flatten, 0);
    rb_define_method(cNArray, "transpose", na_transpose, -1);

    /*
    rb_define_method(cNArray, "reshape", na_reshape,-1);
    rb_define_method(cNArray, "reshape!", na_reshape_bang,-1);
    rb_define_alias(cNArray,  "shape=","reshape!");
    */

    /*
    rb_define_method(cNArray, "cast_to", nary_cast_to, 1);
    rb_define_method(cNArray, "_cast_to", nary__cast_to, 1);

    rb_define_method(cNArray, "+",  na_add, 1);
    rb_define_method(cNArray, "-",  na_sub, 1);
    rb_define_method(cNArray, "*",  na_mul, 1);
    rb_define_method(cNArray, "/",  na_div, 1);
    rb_define_method(cNArray, "%",  na_mod, 1);
    rb_define_method(cNArray, "**", na_pow, 1);
    rb_define_method(cNArray, "&",  na_bit_and, 1);
    rb_define_method(cNArray, "|",  na_bit_or,  1);
    rb_define_method(cNArray, "^",  na_bit_xor, 1);
    rb_define_method(cNArray, "and",na_bit_and, 1);
    rb_define_method(cNArray, "or", na_bit_or,  1);
    rb_define_method(cNArray, "xor",na_bit_xor, 1);
    rb_define_method(cNArray, "eq", na_eq, 1);
    rb_define_method(cNArray, "ne", na_ne, 1);
    rb_define_method(cNArray, "gt", na_gt, 1);
    rb_define_method(cNArray, "ge", na_ge, 1);
    rb_define_method(cNArray, "lt", na_lt, 1);
    rb_define_method(cNArray, "le", na_le, 1);
    rb_define_method(cNArray, ">",  na_gt, 1);
    rb_define_method(cNArray, ">=", na_ge, 1);
    rb_define_method(cNArray, "<",  na_lt, 1);
    rb_define_method(cNArray, "<=", na_le, 1);

    rb_define_method(cNArray, "byte_size",  nary_byte_size, 0);
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

    id_add = rb_intern("add");
    id_sub = rb_intern("sub");
    id_mul = rb_intern("mul");
    id_div = rb_intern("div");
    id_mod = rb_intern("mod");
    id_pow = rb_intern("pow");
    id_bit_and = rb_intern("bit_and");
    id_bit_or  = rb_intern("bit_or");
    id_bit_xor = rb_intern("bit_xor");
    id_eq = rb_intern("eq");
    id_ne = rb_intern("ne");
    id_gt = rb_intern("gt");
    id_ge = rb_intern("ge");
    id_lt = rb_intern("lt");
    id_le = rb_intern("le");
    id_nearly_eq = rb_intern("nearly_eq");

    id_real = rb_intern("real");
    id_imag = rb_intern("imag");

    id_mark = rb_intern("mark");
    id_info = rb_intern("info");

    /*
    cDFloat = rb_define_class_under(mNum, "DFloat", cNArray);
    rb_define_const(mNum, "Float64", cDFloat);

    cDComplex = rb_define_class_under(mNum, "DComplex", cNArray);
    rb_define_const(mNum, "Complex128", cDComplex);

    cInt32 = rb_define_class_under(mNum, "Int32", cNArray);
    cInt24 = rb_define_class_under(mNum, "Int24", cInt32);
    cInt16 = rb_define_class_under(mNum, "Int16", cInt32);
    cInt8  = rb_define_class_under(mNum, "Int8",  cInt32);

    cUInt32 = rb_define_class_under(mNum, "UInt32", cInt32);
    cUInt24 = rb_define_class_under(mNum, "UInt24", cUInt32);
    cUInt16 = rb_define_class_under(mNum, "UInt16", cUInt32);
    cUInt8  = rb_define_class_under(mNum, "UInt8",  cUInt32);

    cInt64  = rb_define_class_under(mNum, "Int64", cNArray);
    cInt48  = rb_define_class_under(mNum, "Int48", cInt64);
    cUInt64 = rb_define_class_under(mNum, "UInt64", cInt64);
    cUInt48 = rb_define_class_under(mNum, "UInt48", cUInt64);

    cBit     = rb_define_class_under(mNum, "Bit",   cNArray);
    cRObject = rb_define_class_under(mNum, "RObject", cNArray);

    cPointer = rb_define_class_under(mNum, "Pointer", cNArray);

    rb_require("complex");
    cComplex = rb_const_get(rb_cObject, rb_intern("Complex"));

    Init_math();

    Init_dfloat();
    Init_bit();
    Init_int32();
    Init_uint32();
    Init_int64();
    Init_dcomplex();
    Init_robject();
    Init_nstruct();
    Init_pointer();
    */
}
