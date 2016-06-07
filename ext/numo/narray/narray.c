/*
  narray.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2016 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#define NARRAY_C
#include <ruby.h>
#include "numo/narray.h"
#include "extconf.h"
#include <assert.h>

/* global variables within this module */
VALUE numo_cNArray;
VALUE rb_mNumo;
VALUE nary_eCastError;
VALUE nary_eShapeError;
VALUE nary_eOperationError;
VALUE nary_eDimensionError;

static ID id_contiguous_stride;
//static ID id_element_bit_size;
//static ID id_element_byte_size;

VALUE cPointer;

static ID id_allocate;

VALUE sym_reduce;
VALUE sym_option;
VALUE sym_loop_opt;
VALUE sym_init;

VALUE na_cStep;
#ifndef HAVE_RB_CCOMPLEX
VALUE rb_cComplex;
#endif

void Init_nary_data();
void Init_nary_step();
void Init_nary_index();
void Init_nary_bit();
void Init_nary_int8();
void Init_nary_int16();
void Init_nary_int32();
void Init_nary_int64();
void Init_nary_uint8();
void Init_nary_uint16();
void Init_nary_uint32();
void Init_nary_uint64();
void Init_nary_sfloat();
void Init_nary_scomplex();
void Init_nary_dfloat();
void Init_nary_dcomplex();
void Init_nary_math();
void Init_nary_rand();
void Init_nary_array();
void Init_nary_struct();
void Init_nary_robject();


static void
rb_narray_debug_info_nadata(VALUE self)
{
    narray_data_t *na;
    GetNArrayData(self,na);

    printf("  ptr    = 0x%"SZF"x\n", (size_t)(na->ptr));
}


static VALUE
rb_narray_debug_info_naview(VALUE self)
{
    int i;
    narray_view_t *na;
    size_t *idx;
    size_t j;
    GetNArrayView(self,na);

    printf("  data   = 0x%"SZF"x\n", (size_t)na->data);
    printf("  offset = %"SZF"d\n", (size_t)na->offset);
    printf("  stridx = 0x%"SZF"x\n", (size_t)na->stridx);

    if (na->stridx) {
        printf("  stridx = [");
        for (i=0; i<na->base.ndim; i++) {
            if (SDX_IS_INDEX(na->stridx[i])) {

                idx = SDX_GET_INDEX(na->stridx[i]);
                printf("  index[%d]=[", i);
                for (j=0; j<na->base.shape[i]; j++) {
                    printf(" %"SZF"d", idx[j]);
                }
                printf(" ] ");

            } else {
                printf(" %"SZF"d", SDX_GET_STRIDE(na->stridx[i]));
            }
        }
        printf(" ]\n");
    }
    return Qnil;
}


VALUE
rb_narray_debug_info(VALUE self)
{
    int i;
    narray_t *na;
    GetNArray(self,na);

    printf("%s:\n",rb_class2name(CLASS_OF(self)));
    printf("  id     = 0x%"SZF"x\n", self);
    printf("  type   = %d\n", na->type);
    printf("  flag   = [%d,%d]\n", na->flag[0], na->flag[1]);
    printf("  size   = %"SZF"d\n", na->size);
    printf("  ndim   = %d\n", na->ndim);
    printf("  shape  = 0x%"SZF"x\n", (size_t)na->shape);
    if (na->shape) {
        printf("  shape  = [");
        for (i=0;i<na->ndim;i++)
            printf(" %"SZF"d", na->shape[i]);
        printf(" ]\n");
    }

    switch(na->type) {
    case NARRAY_DATA_T:
    case NARRAY_FILEMAP_T:
        rb_narray_debug_info_nadata(self);
        break;
    case NARRAY_VIEW_T:
        rb_narray_debug_info_naview(self);
        break;
    }
    return Qnil;
}


void
na_free(narray_data_t* na)
{
    assert(na->base.type==NARRAY_DATA_T);

    if (na->ptr != NULL) {
        xfree(na->ptr);
        na->ptr = NULL;
    }
    if (na->base.size > 0) {
        if (na->base.shape != NULL && na->base.shape != &(na->base.size)) {
            xfree(na->base.shape);
            na->base.shape = NULL;
        }
    }
    xfree(na);
}

static void
na_free_view(narray_view_t* na)
{
    assert(na->base.type==NARRAY_VIEW_T);

    if (na->stridx != NULL) {
        xfree(na->stridx);
        na->stridx = NULL;
    }
    if (na->base.size > 0) {
        if (na->base.shape != NULL && na->base.shape != &(na->base.size)) {
            xfree(na->base.shape);
            na->base.shape = NULL;
        }
    }
    xfree(na);
}

static void
na_gc_mark_view(narray_view_t* na)
{
    rb_gc_mark(na->data);
}

VALUE
na_s_allocate(VALUE klass)
{
    narray_data_t *na = ALLOC(narray_data_t);

    na->base.ndim = 0;
    na->base.type = NARRAY_DATA_T;
    na->base.flag[0] = 0;
    na->base.flag[1] = 0;
    na->base.size = 0;
    na->base.shape = NULL;
    na->base.reduce = INT2FIX(0);
    na->ptr = NULL;
    return Data_Wrap_Struct(klass, 0, na_free, na);
}


VALUE
na_s_allocate_view(VALUE klass)
{
    narray_view_t *na = ALLOC(narray_view_t);

    na->base.ndim = 0;
    na->base.type = NARRAY_VIEW_T;
    na->base.flag[0] = 0;
    na->base.flag[1] = 0;
    na->base.size = 0;
    na->base.shape = NULL;
    na->base.reduce = INT2FIX(0);
    na->data = Qnil;
    na->offset = 0;
    na->stridx = NULL;
    return Data_Wrap_Struct(klass, na_gc_mark_view, na_free_view, na);
}


//static const size_t zero=0;

void
na_array_to_internal_shape(VALUE self, VALUE ary, size_t *shape)
{
    size_t    i, n, c, s;
    VALUE     v;
    narray_t *na;
    int       flag = 0;

    n = RARRAY_LEN(ary);

    if (RTEST(self)) {
        GetNArray(self, na);
        flag = TEST_COLUMN_MAJOR(na);
    }
    if (flag) {
        c = n-1;
        s = -1;
    } else {
        c = 0;
        s = 1;
    }
    for (i=0; i<n; i++) {
        v = RARRAY_AREF(ary,i);
        if (!FIXNUM_P(v) && !rb_obj_is_kind_of(v, rb_cInteger)) {
            rb_raise(rb_eTypeError, "array size must be Integer");
        }
        if (RTEST(rb_funcall(v, rb_intern("<"), 1, INT2FIX(0)))) {
            rb_raise(rb_eArgError,"size must be non-negative");
        }
        shape[c] = NUM2SIZE(v);
        c += s;
    }
}



void
na_alloc_shape(narray_t *na, int ndim)
{
    na->ndim = ndim;
    na->size = 0;
    if (ndim == 0) {
        na->shape = NULL;
    }
    else if (ndim == 1) {
        na->shape = &(na->size);
    }
    else if (ndim < 0) {
        rb_raise(nary_eDimensionError,"ndim=%d is negative", ndim);
    }
    else if (ndim > NA_MAX_DIMENSION) {
        rb_raise(nary_eDimensionError,"ndim=%d is too many", ndim);
    } else {
        na->shape = ALLOC_N(size_t, ndim);
    }
}

void
na_setup_shape(narray_t *na, int ndim, size_t *shape)
{
    int i;
    size_t size;

    na_alloc_shape(na, ndim);

    if (ndim==0) {
        na->size = 1;
    }
    else if (ndim==1) {
        na->size = shape[0];
    }
    else {
        for (i=0, size=1; i<ndim; i++) {
            na->shape[i] = shape[i];
            size *= shape[i];
        }
        na->size = size;
    }
}

void
na_setup(VALUE self, int ndim, size_t *shape)
{
    narray_t *na;
    GetNArray(self,na);
    na_setup_shape(na, ndim, shape);
}


/*
 *  call-seq:
 *     Numo::DataType.new(shape)             => narray
 *     Numo::DataType.new(size1, size2, ...) => narray
 *
 *  Constructs a narray using the given <i>DataType</i> and <i>shape</i> or
 *  <i>sizes</i>.
 */
static VALUE
na_initialize(VALUE self, VALUE args)
{
    VALUE v;
    size_t *shape=NULL;
    int ndim;

    if (RARRAY_LEN(args) == 1) {
        v = RARRAY_AREF(args,0);
        if (TYPE(v) != T_ARRAY) {
            v = args;
        }
    } else {
        v = args;
    }
        ndim = RARRAY_LEN(v);
        if (ndim > NA_MAX_DIMENSION) {
            rb_raise(rb_eArgError,"ndim=%d exceeds maximum dimension",ndim);
        }
        shape = ALLOCA_N(size_t, ndim);
        // setup size_t shape[] from VALUE shape argument
        na_array_to_internal_shape(self, v, shape);
    na_setup(self, ndim, shape);

    return self;
}


VALUE
rb_narray_new(VALUE klass, int ndim, size_t *shape)
{
    volatile VALUE obj;

    obj = rb_funcall(klass, id_allocate, 0);
    na_setup(obj, ndim, shape);
    return obj;
}


VALUE
rb_narray_view_new(VALUE klass, int ndim, size_t *shape)
{
    volatile VALUE obj;

    obj = na_s_allocate_view(klass);
    na_setup(obj, ndim, shape);
    return obj;
}


/*
  Replaces the contents of self with the contents of other narray.
  Used in dup and clone method.
  @overload initialize_copy(other)
  @param [Numo::NArray] other
  @return [Numo::NArray] self
 */
static VALUE
na_initialize_copy(VALUE self, VALUE orig)
{
    narray_t *na;
    GetNArray(orig,na);

    na_setup(self,NA_NDIM(na),NA_SHAPE(na));
    na_store(self,orig);
    na_copy_flags(orig,self);
    return self;
}


/*
 *  call-seq:
 *     zeros(shape)  => narray
 *     zeros(size1,size2,...)  => narray
 *
 *  Returns a zero-filled narray with <i>shape</i>.
 *  This singleton method is valid not for NArray class itself
 *  but for typed NArray subclasses, e.g., DFloat, Int64.
 *  @example
 *    a = Numo::DFloat.zeros(3,5)
 *    => Numo::DFloat#shape=[3,5]
 *    [[0, 0, 0, 0, 0],
 *     [0, 0, 0, 0, 0],
 *     [0, 0, 0, 0, 0]]
 */
static VALUE
na_s_zeros(int argc, const VALUE *argv, VALUE klass)
{
    VALUE obj;
    obj = rb_class_new_instance(argc, argv, klass);
    return rb_funcall(obj, rb_intern("fill"), 1, INT2FIX(0));
}


/*
 *  call-seq:
 *     ones(shape)  => narray
 *     ones(size1,size2,...)  => narray
 *
 *  Returns a one-filled narray with <i>shape</i>.
 *  This singleton method is valid not for NArray class itself
 *  but for typed NArray subclasses, e.g., DFloat, Int64.
 *  @example
 *    a = Numo::DFloat.ones(3,5)
 *    => Numo::DFloat#shape=[3,5]
 *    [[1, 1, 1, 1, 1],
 *     [1, 1, 1, 1, 1],
 *     [1, 1, 1, 1, 1]]
 */
static VALUE
na_s_ones(int argc, const VALUE *argv, VALUE klass)
{
    VALUE obj;
    obj = rb_class_new_instance(argc, argv, klass);
    return rb_funcall(obj, rb_intern("fill"), 1, INT2FIX(1));
}


/*
  Returns a NArray with shape=(n,n) whose diagonal elements are 1, otherwise 0.
  @overload  eye(n)
  @param [Integer] n  Size of NArray. Creates 2-D NArray with shape=(n,n)
  @return [Numo::NArray]  created NArray.
  @example
    a = Numo::DFloat.eye(3)
    => Numo::DFloat#shape=[3,3]
    [[1, 0, 0],
     [0, 1, 0],
     [0, 0, 1]]
*/
static VALUE
na_s_eye(int argc, const VALUE *argv, VALUE klass)
{
    VALUE obj;
    VALUE tmp[2];

    if (argc==0) {
        rb_raise(rb_eArgError,"No argument");
    }
    else if (argc==1) {
        tmp[0] = tmp[1] = argv[0];
        argv = tmp;
        argc = 2;
    }
    obj = rb_class_new_instance(argc, argv, klass);
    return rb_funcall(obj, rb_intern("eye"), 0);
}


char *
na_get_pointer(VALUE self)
{
    narray_t *na;
    GetNArray(self,na);

    switch(NA_TYPE(na)) {
    case NARRAY_DATA_T:
        return NA_DATA_PTR(na);
    case NARRAY_FILEMAP_T:
        return ((narray_filemap_t*)na)->ptr;
    case NARRAY_VIEW_T:
        return na_get_pointer(NA_VIEW_DATA(na));
        //puts("pass NARRAY_VIEW_T in na_get_pointer_for_write");
        //ptr += ((narray_view_t*)na)->offset;
        //ptr += NA_VIEW_OFFSET(na);
        break;
    default:
        rb_raise(rb_eRuntimeError,"invalid NA_TYPE");
    }
    return NULL;
}

char *
na_get_pointer_for_write(VALUE self)
{
    char *ptr;
    narray_t *na;
    GetNArray(self,na);

    if (OBJ_FROZEN(self)) {
        rb_raise(rb_eRuntimeError, "cannot write to frozen NArray.");
    }

    if (NA_TYPE(na) == NARRAY_DATA_T) {
        ptr = NA_DATA_PTR(na);
        if (na->size > 0 && ptr == NULL) {
            rb_funcall(self, id_allocate, 0);
            ptr = NA_DATA_PTR(na);
        }
    } else {
        ptr = na_get_pointer(self);
        if (NA_SIZE(na) > 0 && ptr == NULL) {
            rb_raise(rb_eRuntimeError,"cannot write to unallocated NArray");
        }
    }

    //NA_SET_LOCK(na);

    return ptr;
}

char *
na_get_pointer_for_read(VALUE self)
{
    char  *ptr;
    narray_t *na;
    GetNArray(self,na);

    //if (NA_TEST_LOCK(na)) {
    //    rb_raise(rb_eRuntimeError, "cannot read locked NArray.");
    //}

    if (NA_TYPE(na) == NARRAY_DATA_T) {
        ptr = NA_DATA_PTR(na);
    } else {
        ptr = na_get_pointer(self);
    }

    if (NA_SIZE(na) > 0 && ptr == NULL) {
        rb_raise(rb_eRuntimeError,"cannot read unallocated NArray");
    }

    //NA_SET_LOCK(na);

    return ptr;
}


void
na_release_lock(VALUE self)
{
    narray_t *na;
    GetNArray(self,na);

    NA_UNSET_LOCK(na);

    switch(NA_TYPE(na)) {
    case NARRAY_VIEW_T:
        na_release_lock(NA_VIEW_DATA(na));
        break;
    }
}

// fix name, ex, allow_stride_for_flatten_view
VALUE
na_check_ladder(VALUE self, int start_dim)
{
    int i;
    ssize_t st0, st1;
    narray_t *na1;
    narray_view_t *na;
    GetNArray(self,na1);

    //puts("pass ladder");

    if (start_dim < -na1->ndim || start_dim >= na1->ndim) {
        rb_bug("start_dim (%d) out of range",start_dim);
    }

    switch(na1->type) {
    case NARRAY_DATA_T:
    case NARRAY_FILEMAP_T:
        return Qtrue;
    case NARRAY_VIEW_T:
        GetNArrayView(self,na);
        // negative dim -> position from last dim
        if (start_dim < 0) {
            start_dim += na->base.ndim;
        }
        // not ladder if it has index
        for (i=start_dim; i<na->base.ndim; i++) {
            if (SDX_IS_INDEX(na->stridx[i]))
                return Qfalse;
        }
        // check stride
        i = start_dim;
        st0 = SDX_GET_STRIDE(na->stridx[i]);
        for (i++; i<na->base.ndim; i++) {
            st1 = SDX_GET_STRIDE(na->stridx[i]);
            if (st0 != (ssize_t)(st1*na->base.shape[i])) {
                return Qfalse;
            }
            st0 = st1;
        }
        return Qtrue;
    }
    return Qtrue;
}


/*
stridx_t *
na_get_stride(VALUE v)
{
    int i;
    size_t st;
    stridx_t *stridx=NULL;
    narray_t *na;
    GetNArray(v,na);

    switch(NA_TYPE(na)) {
    case NARRAY_DATA_T:
        if (NA_DATA_PTR(na)==NULL) {
            rb_raise(rb_eRuntimeError,"cannot read no-data NArray");
        }
        break;
    case NARRAY_FILEMAP_T:
        break;
    case NARRAY_VIEW_T:
        stridx = NA_STRIDX(na);
        break;
    default:
        rb_raise(rb_eRuntimeError,"invalid narray internal type");
    }

    if (!stridx) {
        stridx = ALLOC_N(stridx_t, na->ndim);
        st = NUM2SIZE(rb_const_get(CLASS_OF(v), id_contiguous_stride));
        //printf("step_unit=%ld, CLASS_OF(v)=%lx\n",st, CLASS_OF(v));
        for (i=na->ndim; i>0;) {
            SDX_SET_STRIDE(stridx[--i],st);
            st *= na->shape[i];
        }
    }

    return stridx;
}
*/


/* method: size() -- returns the total number of typeents */
static VALUE
na_size(VALUE self)
{
    narray_t *na;
    GetNArray(self,na);
    return SIZE2NUM(na->size);
}


/* method: size() -- returns the total number of typeents */
static VALUE
na_ndim(VALUE self)
{
    narray_t *na;
    GetNArray(self,na);
    return INT2NUM(na->ndim);
}


/* method: shape() -- returns shape, array of the size of dimensions */
static VALUE
 na_shape(VALUE self)
{
    volatile VALUE v;
    narray_t *na;
    size_t i, n, c, s;

    GetNArray(self,na);
    n = NA_NDIM(na);
    if (TEST_COLUMN_MAJOR(na)) {
        c = n-1;
        s = -1;
    } else {
        c = 0;
        s = 1;
    }
    v = rb_ary_new2(n);
    for (i=0; i<n; i++) {
        rb_ary_push(v, SIZE2NUM(na->shape[c]));
        c += s;
    }
    return v;
}


size_t
na_get_elmsz(VALUE vna)
{
    return NUM2SIZE(rb_const_get(CLASS_OF(vna), id_contiguous_stride));
}

size_t
na_dtype_elmsz(VALUE klass)
{
    return NUM2SIZE(rb_const_get(klass, id_contiguous_stride));
}

size_t
na_get_offset(VALUE self)
{
    narray_t *na;
    GetNArray(self,na);

    switch(na->type) {
    case NARRAY_DATA_T:
    case NARRAY_FILEMAP_T:
        return 0;
    case NARRAY_VIEW_T:
        return NA_VIEW_OFFSET(na);
    }
    return 0;
}


void
na_index_arg_to_internal_order(int argc, VALUE *argv, VALUE self)
{
    int i,j;
    VALUE tmp;

    if (TEST_COLUMN_MAJOR(self)) {
        for (i=0,j=argc-1; i<argc/2; i++,j--) {
            tmp = argv[i];
            argv[i] = argv[j];
            argv[j] = tmp;
        }
    }
}

void
na_copy_flags(VALUE src, VALUE dst)
{
    narray_t *na1, *na2;

    GetNArray(src,na1);
    GetNArray(dst,na2);

    na2->flag[0] = na1->flag[0];
    na2->flag[1] = na1->flag[1];

    RBASIC(dst)->flags |= (RBASIC(src)->flags) &
        (FL_USER1|FL_USER2|FL_USER3|FL_USER4|FL_USER5|FL_USER6|FL_USER7);
}


VALUE
na_original_data(VALUE self)
{
    narray_t *na;
    narray_view_t *nv;

    GetNArray(self,na);
    switch(na->type) {
    case NARRAY_VIEW_T:
        GetNArrayView(self, nv);
        return nv->data;
    }
    return self;
}


//----------------------------------------------------------------------

/*
 *  call-seq:
 *     narray.view => narray
 *
 *  Return view of NArray
 */
VALUE
na_make_view(VALUE self)
{
    int i, nd;
    size_t  j;
    size_t *idx1, *idx2;
    ssize_t stride;
    narray_t *na;
    narray_view_t *na1, *na2;
    volatile VALUE view;

    GetNArray(self,na);
    nd = na->ndim;

    view = na_s_allocate_view(CLASS_OF(self));

    na_copy_flags(self, view);
    GetNArrayView(view, na2);

    na_setup_shape((narray_t*)na2, nd, na->shape);
    na2->stridx = ALLOC_N(stridx_t,nd);

    switch(na->type) {
    case NARRAY_DATA_T:
    case NARRAY_FILEMAP_T:
        stride = na_get_elmsz(self);
        for (i=nd; i--;) {
            SDX_SET_STRIDE(na2->stridx[i],stride);
            stride *= na->shape[i];
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

    return view;
}


//----------------------------------------------------------------------

/*
 *  call-seq:
 *     narray.expand_dims(dim) => narray view
 *
 *  Expand the shape of an array. Insert a new axis with size=1
 *  at a given dimension.
 *  @param [Integer] dim  dimension at which new axis is inserted.
 *  @return [Numo::NArray]  result narray view.
 */
VALUE
na_expand_dims(VALUE self, VALUE vdim)
{
    int  i, j, nd, dim;
    size_t *shape, *na_shape;
    stridx_t *stridx, *na_stridx;
    narray_t *na;
    narray_view_t *na2;
    VALUE view;

    GetNArray(self,na);
    nd = na->ndim;

    dim = NUM2INT(vdim);
    if (dim < -nd-1 || dim > nd) {
        rb_raise(nary_eDimensionError,"invalid axis (%d for %dD NArray)",
                 dim,nd);
    }
    if (dim < 0) {
        dim += nd+1;
    }

    view = na_make_view(self);
    GetNArrayView(view, na2);

    shape = ALLOC_N(size_t,nd+1);
    stridx = ALLOC_N(stridx_t,nd+1);
    na_shape = na2->base.shape;
    na_stridx = na2->stridx;

    for (i=j=0; i<=nd; i++) {
        if (i==dim) {
            shape[i] = 1;
            SDX_SET_STRIDE(stridx[i],0);
        } else {
            shape[i] = na_shape[j];
            stridx[i] = na_stridx[j];
            j++;
        }
    }

    na2->stridx = stridx;
    xfree(na_stridx);
    na2->base.shape = shape;
    xfree(na_shape);
    na2->base.ndim++;
    return view;
}

//----------------------------------------------------------------------

/*
 *  call-seq:
 *     narray.reverse([dim0,dim1,..]) => narray
 *
 *  Return reversed view along specified dimeinsion
 */
VALUE
nary_reverse(int argc, VALUE *argv, VALUE self)
{
    int i, nd;
    size_t  j, n;
    size_t  offset;
    size_t *idx1, *idx2;
    ssize_t stride;
    ssize_t sign;
    narray_t *na;
    narray_view_t *na1, *na2;
    VALUE view;
    VALUE reduce;

    reduce = na_reduce_dimension(argc, argv, 1, &self);

    GetNArray(self,na);
    nd = na->ndim;

    view = na_s_allocate_view(CLASS_OF(self));

    na_copy_flags(self, view);
    GetNArrayView(view, na2);

    na_setup_shape((narray_t*)na2, nd, na->shape);
    na2->stridx = ALLOC_N(stridx_t,nd);

    switch(na->type) {
    case NARRAY_DATA_T:
    case NARRAY_FILEMAP_T:
        stride = na_get_elmsz(self);
        offset = 0;
        for (i=nd; i--;) {
            if (na_test_reduce(reduce,i)) {
                offset += (na->shape[i]-1)*stride;
                sign = -1;
            } else {
                sign = 1;
            }
            SDX_SET_STRIDE(na2->stridx[i],stride*sign);
            stride *= na->shape[i];
        }
        na2->offset = offset;
        na2->data = self;
        break;
    case NARRAY_VIEW_T:
        GetNArrayView(self, na1);
        offset = na1->offset;
        for (i=0; i<nd; i++) {
            n = na1->base.shape[i];
            if (SDX_IS_INDEX(na1->stridx[i])) {
                idx1 = SDX_GET_INDEX(na1->stridx[i]);
                idx2 = ALLOC_N(size_t,n);
                if (na_test_reduce(reduce,i)) {
                    for (j=0; j<n; j++) {
                        idx2[n-1-j] = idx1[j];
                    }
                } else {
                    for (j=0; j<n; j++) {
                        idx2[j] = idx1[j];
                    }
                }
                SDX_SET_INDEX(na2->stridx[i],idx2);
            } else {
                stride = SDX_GET_STRIDE(na1->stridx[i]);
                if (na_test_reduce(reduce,i)) {
                    offset += (n-1)*stride;
                    SDX_SET_STRIDE(na2->stridx[i],-stride);
                } else {
                    na2->stridx[i] = na1->stridx[i];
                }
            }
        }
        na2->offset = offset;
        na2->data = na1->data;
        break;
    }

    return view;
}

//----------------------------------------------------------------------

VALUE
numo_na_upcast(VALUE type1, VALUE type2)
{
    VALUE upcast_hash;
    VALUE result_type;

    if (type1==type2) {
        return type1;
    }
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

/*
  Returns an array containing other and self,
  both are converted to upcasted type of NArray.
  Note that NArray has distinct UPCAST mechanism.
  Coerce is used for operation between non-NArray and NArray.
  @overload coerce(other)
  @param [Object] other  numeric object.
  @return [Array]  NArray-casted [other,self]
*/
static VALUE
nary_coerce(VALUE x, VALUE y)
{
    VALUE type;

    type = numo_na_upcast(CLASS_OF(x), CLASS_OF(y));
    y = rb_funcall(type,rb_intern("cast"),1,y);
    return rb_assoc_new(y , x);
}


/*
  Returns total byte size of NArray.
  @return [Integer] byte size.
 */
static VALUE
nary_byte_size(VALUE self)
{
    VALUE velmsz; //klass,
    narray_t *na;
    size_t sz;

    GetNArray(self,na);
    velmsz = rb_const_get(CLASS_OF(self), rb_intern(ELEMENT_BYTE_SIZE));
    sz = SIZE2NUM(NUM2SIZE(velmsz) * na->size);
    return sz;
}

/*
  Returns byte size of one element of NArray.
  @return [Numeric] byte size.
 */
static VALUE
nary_s_byte_size(VALUE type)
{
    return rb_const_get(type, rb_intern(ELEMENT_BYTE_SIZE));
}


/*
  Returns a new 1-D array initialized from binary raw data in a string.
  @overload from_string(string,[shape])
  @param [String] string  Binary raw data.
  @param [Array] shape  array of integers representing array shape.
  @return [Numo::NArray] NArray containing binary data.
 */
static VALUE
nary_s_from_string(int argc, VALUE *argv, VALUE type)
{
    size_t len, str_len, elmsz;
    size_t *shape;
    char *ptr;
    int i, nd, narg;
    VALUE vstr,vshape,vna;

    narg = rb_scan_args(argc,argv,"11",&vstr,&vshape);
    str_len = RSTRING_LEN(vstr);
    elmsz = na_dtype_elmsz(type);
    if (narg==2) {
        nd = RARRAY_LEN(vshape);
        if (nd == 0 || nd > NA_MAX_DIMENSION) {
            rb_raise(nary_eDimensionError,"too long or empty shape (%d)", nd);
        }
        shape = ALLOCA_N(size_t,nd);
        len = 1;
        for (i=0; i<nd; ++i) {
            len *= shape[i] = NUM2SIZE(RARRAY_AREF(vshape,i));
        }
        if (len*elmsz != str_len) {
            rb_raise(rb_eArgError, "size mismatch");
        }
    } else {
        nd = 1;
        shape = ALLOCA_N(size_t,nd);
        shape[0] = len = str_len / elmsz;
        if (len == 0) {
            rb_raise(rb_eArgError, "string is empty or too short");
        }
    }

    vna = rb_narray_new(type, nd, shape);
    ptr = na_get_pointer_for_write(vna);

    memcpy(ptr, RSTRING_PTR(vstr), elmsz*len);

    return vna;
}

/*
  Returns stfing containing the raw data bytes in NArray.
  @overload to_string()
  @return [String] String object containing binary raw data.
 */
static VALUE
nary_to_string(VALUE self)
{
    size_t len, esz;
    char *ptr;
    volatile VALUE v[2];
    narray_t *na;

    GetNArray(self,na);
    if (na->type == NARRAY_VIEW_T) {
        v[0] = na_copy(self);
    } else {
        v[0] = self;
    }
    esz = na_get_elmsz(v[0]);
    len = na->size * esz;
    ptr = na_get_pointer_for_read(v[0]);
    v[1] = rb_usascii_str_new(ptr,len);
    return v[1];
}


/*
  Cast self to another NArray datatype.
  @overload cast_to(datatype)
  @param [Class] datatype NArray datatype.
  @return [Numo::NArray]
 */
static VALUE
nary_cast_to(VALUE obj, VALUE type)
{
    return rb_funcall(type, rb_intern("cast"), 1, obj);
}



boolean
na_test_reduce(VALUE reduce, int dim)
{
    size_t m;

    if (!RTEST(reduce))
        return 0;
    if (FIXNUM_P(reduce)) {
        m = FIX2LONG(reduce);
        if (m==0) return 1;
        return (m & (1u<<dim)) ? 1 : 0;
    } else {
        return (rb_funcall(reduce,rb_intern("[]"),1,INT2FIX(dim))==INT2FIX(1)) ?
            1 : 0 ;
    }
}


VALUE
na_reduce_dimension(int argc, VALUE *argv, int naryc, VALUE *naryv)
{
    int ndim, ndim0;
    int row_major;
    int i, r;
    size_t j;
    size_t len;
    ssize_t beg, step;
    VALUE v;
    narray_t *na;
    size_t m;
    VALUE reduce;

    if (naryc<1) {
        rb_raise(rb_eRuntimeError,"must be positive: naryc=%d", naryc);
    }
    GetNArray(naryv[0],na);
    reduce = na->reduce;
    if (argc==0) {
        //printf("pass argc=0 reduce=%d\n",NUM2INT(reduce));
        return reduce;
    }
    ndim = ndim0 = na->ndim;
    row_major = TEST_COLUMN_MAJOR(naryv[0]);
    for (i=1; i<naryc; i++) {
        GetNArray(naryv[i],na);
        if (TEST_COLUMN_MAJOR(naryv[i]) != row_major) {
            rb_raise(nary_eDimensionError,"dimension order is different");
        }
        if (na->ndim > ndim) {
            ndim = na->ndim;
        }
    }
    if (ndim != ndim0) {
        j = FIX2ULONG(reduce) << (ndim-ndim0);
        reduce = ULONG2NUM(j);
        if (!FIXNUM_P(reduce)) {
            rb_raise(nary_eDimensionError,"reduce has too many bits");
        }
    }
    //printf("argc=%d\n",argc);

    m = 0;
    reduce = Qnil;
    for (i=0; i<argc; i++) {
        v = argv[i];
        //printf("argv[%d]=",i);rb_p(v);
        if (TYPE(v)==T_FIXNUM) {
            beg = FIX2INT(v);
            if (beg<0) beg+=ndim;
            if (beg>=ndim || beg<0) {
                rb_raise(nary_eDimensionError,"dimension is out of range");
            }
            len = 1;
            step = 0;
            //printf("beg=%d step=%d len=%d\n",beg,step,len);
        } else if (rb_obj_is_kind_of(v,rb_cRange) ||
                   rb_obj_is_kind_of(v,na_cStep)) {
            nary_step_array_index( v, ndim, &len, &beg, &step );
        } else {
            rb_raise(nary_eDimensionError, "invalid dimension argument %s",
                     rb_obj_classname(v));
        }
        for (j=0; j<len; j++) {
            r = beg + step*j;
            if (row_major)
                r = ndim-1-r;
            if (reduce==Qnil) {
              if ( r < (ssize_t)sizeof(size_t) ) {
                    m |= ((size_t)1) << r;
                    continue;
                } else {
                    reduce = SIZE2NUM(m);
                }
            }
            v = rb_funcall( INT2FIX(1), rb_intern("<<"), 1, INT2FIX(r) );
            reduce = rb_funcall( reduce, rb_intern("|"), 1, v );
        }
    }
    if (reduce==Qnil) reduce = SIZE2NUM(m);
    return reduce;
}

//--------------------------------------

/*
void
na_index_arg_to_internal_order( int argc, VALUE *argv, VALUE self )
{
    int i,j;
    VALUE tmp;

    if (TEST_COLUMN_MAJOR(self)) {
	for (i=0,j=argc-1; i<argc/2; i++,j--) {
	    tmp = argv[i];
	    argv[i] = argv[j];
	    argv[j] = tmp;
	}
    }
}

VALUE
na_index_array_to_internal_order( VALUE args, VALUE self )
{
    int i,j;

    if (TEST_COLUMN_MAJOR(self)) {
	return rb_funcall( args, rb_intern("reverse"), 0 );
    }
    return args;
}
*/


/*
  Return true if column major.
*/
VALUE na_column_major_p( VALUE self )
{
    if (TEST_COLUMN_MAJOR(self))
	return Qtrue;
    else
	return Qfalse;
}

/*
  Return true if row major.
*/
VALUE na_row_major_p( VALUE self )
{
    if (TEST_ROW_MAJOR(self))
	return Qtrue;
    else
	return Qfalse;
}


/*
  Return true if byte swapped.
*/
VALUE na_byte_swapped_p( VALUE self )
{
    if (TEST_BYTE_SWAPPED(self))
      return Qtrue;
    return Qfalse;
}

/*
  Return true if not byte swapped.
*/
VALUE na_host_order_p( VALUE self )
{
    if (TEST_BYTE_SWAPPED(self))
      return Qfalse;
    return Qtrue;
}


/*
  Returns view of narray with inplace flagged.
  @return [Numo::NArray] view of narray with inplace flag.
*/
VALUE na_inplace( VALUE self )
{
    VALUE view = self;
    //view = na_clone(self);
    SET_INPLACE(view);
    return view;
}

/*
  Set inplace flag to self.
  @return [Numo::NArray] self
*/
VALUE na_inplace_bang( VALUE self )
{
    SET_INPLACE(self);
    return self;
}

VALUE na_inplace_store( VALUE self, VALUE val )
{
    if (self==val)
        return self;
    else
        return na_store( self, val );
}

/*
  Return true if inplace flagged.
*/
VALUE na_inplace_p( VALUE self )
{
    if (TEST_INPLACE(self))
        return Qtrue;
    else
        return Qfalse;
}

/*
  Unset inplace flag to self.
  @return [Numo::NArray] self
*/
VALUE na_out_of_place_bang( VALUE self )
{
    UNSET_INPLACE(self);
    return self;
}

int na_debug_flag=0;

VALUE na_debug_set(VALUE mod, VALUE flag)
{
    na_debug_flag = RTEST(flag);
    return Qnil;
}

double na_profile_value=0;

VALUE na_profile(VALUE mod)
{
    return rb_float_new(na_profile_value);
}

VALUE na_profile_set(VALUE mod, VALUE val)
{
    na_profile_value = NUM2DBL(val);
    return val;
}


/*
  Equality of self and other in view of numerical array.
  i.e., both arrays have same shape and corresponding elements are equal.
  @overload == other
  @param [Object] other
  @return [Boolean] true if self and other is equal.
*/
VALUE
na_equal(VALUE self, volatile VALUE other)
{
    volatile VALUE bool;
    narray_t *na1, *na2;
    int i;

    GetNArray(self,na1);

    if (!rb_obj_is_kind_of(other,cNArray)) {
        other = rb_funcall(CLASS_OF(self), rb_intern("cast"), 1, other);
    }

    GetNArray(other,na2);
    if (na1->ndim != na2->ndim) {
        return Qfalse;
    }
    for (i=0; i<na1->ndim; i++) {
        if (na1->shape[i] != na2->shape[i]) {
            return Qfalse;
        }
    }
    bool = rb_funcall(self, rb_intern("eq"), 1, other);
    return (rb_funcall(bool, rb_intern("count_false"), 0)==INT2FIX(0)) ? Qtrue : Qfalse;
}



/* initialization of NArray Class */
void
Init_narray()
{
    mNumo = rb_define_module("Numo");

    /* define NArray class */
    cNArray = rb_define_class_under(mNumo, "NArray", rb_cObject);

#ifndef HAVE_RB_CCOMPLEX
    rb_require("complex");
    rb_cComplex = rb_const_get(rb_cObject, rb_intern("Complex"));
#endif

    nary_eCastError = rb_define_class_under(cNArray, "CastError", rb_eStandardError);
    nary_eShapeError = rb_define_class_under(cNArray, "ShapeError", rb_eStandardError);
    nary_eOperationError = rb_define_class_under(cNArray, "OperationError", rb_eStandardError);
    nary_eDimensionError = rb_define_class_under(cNArray, "DimensionError", rb_eStandardError);

    rb_define_singleton_method(cNArray, "debug=", na_debug_set, 1);
    rb_define_singleton_method(cNArray, "profile", na_profile, 0);
    rb_define_singleton_method(cNArray, "profile=", na_profile_set, 1);

    /* Ruby allocation framework  */
    rb_define_alloc_func(cNArray, na_s_allocate);
    rb_define_method(cNArray, "initialize", na_initialize, -2);
    rb_define_method(cNArray, "initialize_copy", na_initialize_copy, 1);

    rb_define_singleton_method(cNArray, "zeros", na_s_zeros, -1);
    rb_define_singleton_method(cNArray, "ones", na_s_ones, -1);
    rb_define_singleton_method(cNArray, "eye", na_s_eye, -1);

    rb_define_method(cNArray, "size", na_size, 0);
    rb_define_alias (cNArray, "length","size");
    rb_define_alias (cNArray, "total","size");
    rb_define_method(cNArray, "shape", na_shape, 0);
    rb_define_method(cNArray, "ndim", na_ndim,0);
    rb_define_alias (cNArray, "rank","ndim");

    rb_define_method(cNArray, "debug_info", rb_narray_debug_info, 0);

    rb_define_method(cNArray, "view", na_make_view, 0);
    rb_define_method(cNArray, "expand_dims", na_expand_dims, 1);
    rb_define_method(cNArray, "reverse", nary_reverse, -1);

    rb_define_singleton_method(cNArray, "upcast", numo_na_upcast, 1);
    rb_define_singleton_method(cNArray, "byte_size", nary_s_byte_size, 0);

    rb_define_singleton_method(cNArray, "from_string", nary_s_from_string, -1);
    rb_define_method(cNArray, "to_string",  nary_to_string, 0);

    rb_define_method(cNArray, "byte_size",  nary_byte_size, 0);

    rb_define_method(cNArray, "cast_to", nary_cast_to, 1);

    rb_define_method(cNArray, "coerce", nary_coerce, 1);

    rb_define_method(cNArray, "column_major?", na_column_major_p, 0);
    rb_define_method(cNArray, "row_major?", na_row_major_p, 0);
    rb_define_method(cNArray, "byte_swapped?", na_byte_swapped_p, 0);
    rb_define_method(cNArray, "host_order?", na_host_order_p, 0);

    rb_define_method(cNArray, "inplace", na_inplace, 0);
    rb_define_method(cNArray, "inplace?", na_inplace_p, 0);
    rb_define_method(cNArray, "inplace!", na_inplace_bang, 0);
    rb_define_method(cNArray, "out_of_place!", na_out_of_place_bang, 0);
    rb_define_alias (cNArray, "not_inplace!", "out_of_place!");

    rb_define_method(cNArray, "==", na_equal, 1);

    id_allocate = rb_intern("allocate");
    id_contiguous_stride = rb_intern(CONTIGUOUS_STRIDE);
    //id_element_bit_size = rb_intern(ELEMENT_BIT_SIZE);
    //id_element_byte_size = rb_intern(ELEMENT_BYTE_SIZE);

    sym_reduce   = ID2SYM(rb_intern("reduce"));
    sym_option   = ID2SYM(rb_intern("option"));
    sym_loop_opt = ID2SYM(rb_intern("loop_opt"));
    sym_init     = ID2SYM(rb_intern("init"));

    Init_nary_step();
    Init_nary_index();

    Init_nary_data();

    Init_nary_dcomplex();
    Init_nary_dfloat();
    Init_nary_scomplex();
    Init_nary_sfloat();

    Init_nary_int64();
    Init_nary_uint64();
    Init_nary_int32();
    Init_nary_uint32();
    Init_nary_int16();
    Init_nary_uint16();
    Init_nary_int8();
    Init_nary_uint8();
    Init_nary_bit();

    Init_nary_math();

    Init_nary_rand();
    Init_nary_array();
    Init_nary_struct();
    Init_nary_robject();
}
