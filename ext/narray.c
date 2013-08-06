/*
  narray.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#define NARRAY_C
#include <ruby.h>
#include "narray.h"
#include "narray_config.h"
#include <assert.h>

/* global variables within this module */
VALUE cNArray;
VALUE nary_eCastError;
VALUE nary_eShapeError;

ID id_contiguous_stride;
ID id_element_bit_size;
ID id_element_byte_size;

VALUE cBit;
VALUE cRObject;
VALUE cPointer;

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
ID id_nearly_eq;

ID id_real;
ID id_imag;

ID id_cast;

ID id_reduce;
ID id_option;
ID id_loop_opt;
ID id_init;

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


static void
na_free(narray_data_t* na)
{
    assert(na->base.type==NARRAY_DATA_T);

    if (na->ptr != NULL) {
        xfree(na->ptr);
    }
    if (na->base.size > 0) {
        if (na->base.shape != NULL && na->base.shape != &(na->base.size))
            xfree(na->base.shape);
    }
    xfree(na);
}

static void
na_free_view(narray_view_t* na)
{
    assert(na->base.type==NARRAY_VIEW_T);

    if (na->stridx != NULL) {
        xfree(na->stridx);
    }
    if (na->base.size > 0) {
        if (na->base.shape != NULL && na->base.shape != &(na->base.size))
            xfree(na->base.shape);
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

static void
na_array_to_internal_shape(VALUE self, VALUE ary, size_t *shape)
{
    size_t i, n, c, s;
    VALUE *ptr;
    narray_t *na;
    GetNArray(self, na);

    n = RARRAY_LEN(ary);
    ptr = RARRAY_PTR(ary);

    if (TEST_COLUMN_MAJOR(na)) {
        c = n-1;
        s = -1;
    } else {
        c = 0;
        s = 1;
    }
    for (i=0; i<n; i++) {
        if (RTEST(rb_funcall(ptr[i], rb_intern("<"), 1, INT2FIX(0)))) {
            rb_raise(rb_eArgError,"size must be non-negative");
        }
        shape[c] = NUM2SIZE(ptr[i]);
        c += s;
    }
}



void
na_alloc_shape(narray_t *na, int ndim)
{
    int i;

    na->ndim = ndim;
    na->size = 0;
    if (ndim == 0) {
        na->shape = NULL;
    }
    else if (ndim == 1) {
        na->shape = &(na->size);
    }
    else if (ndim > 1 && ndim <= 1024) {
        na->shape = ALLOC_N(size_t, ndim);
        for (i=0; i<ndim; i++) {
            na->shape[i] = 0;
        }
    } else {
        rb_raise(rb_eRuntimeError,"negative or too large number of dimensions?");
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
    else if (ndim > 1 && ndim <= 1024) {
        for (i=0, size=1; i<ndim; i++) {
            na->shape[i] = shape[i];
            size *= shape[i];
        }
        na->size = size;
    } else {
        rb_raise(rb_eRuntimeError,"negative or too large number of dimensions?");
    }
}



void
na_setup(VALUE self, int ndim, size_t *shape)
{
    narray_t *na;
    //VALUE velmsz;

    GetNArray(self,na);
    // Shape
    na_setup_shape(na, ndim, shape);
    //na->offset = 0;
}



/*
 *  call-seq:
 *     NArray.new(shape)                => narray
 *     NArray.new(type, shape)                => narray
 *     NArray.new(type, size1, size2, ...)    => narray
 *
 *  Constructs a narray using the given <i>datatype</i> and <i>shape</i> or .
 *  <i>size</i>.  If the second argument is an integer, returns 1-d array.
 */

static VALUE
na_initialize(int argc, VALUE *argv, VALUE self)
{
    //VALUE vtype;
    //VALUE vshape;
    VALUE v;
    size_t *shape=NULL;
    //size_t size;
    int ndim;
    //int i;

    v = argv[0];

    if (TYPE(v) == T_ARRAY) {
        ndim = RARRAY_LEN(v);
        //printf("ndim=%d\n",ndim);
        if (ndim > 255) {
            rb_raise(rb_eArgError,"too large number of dimensions");
        }
        shape = ALLOCA_N(size_t, ndim);
        // setup size_t shape[] from VALUE shape argument
        na_array_to_internal_shape(self, v, shape);
    } else {
        rb_raise(rb_eArgError,"argument is not an Array");
    }
    //printf("ndim=%d\n",ndim);
    na_setup(self, ndim, shape);

    return self;
}


VALUE
rb_narray_new(VALUE klass, int ndim, size_t *shape)
{
    volatile VALUE obj;

    obj = na_s_allocate(klass);
    na_setup(obj, ndim, shape);
    return obj;
}


void
na_alloc_data(VALUE self)
{
    narray_t *na;
    GetNArray(self,na);

    if (NA_TYPE(na)==NARRAY_DATA_T) {
        if (na->size>0 && NA_DATA_PTR(na)==NULL) {
            NA_DATA_PTR(na) = xmalloc(na->size*na_get_elmsz(self));
        }
    }
    else {
        rb_bug("invalid narray type : %d",NA_TYPE(na));
    }
}

/*
  Replaces the contents of self with the contents of other narray.
  Used in dup and clone method.
  @overload initialize_copy(other)
  @param [NArray] other
  @return [NArray] self
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


static size_t
na_bytesize(VALUE self)
{
    size_t bitsz, bytesz;
    narray_t *na;
    GetNArray(self,na);

    bitsz = NUM2SIZE(rb_const_get(CLASS_OF(self), id_element_bit_size));
    if (bitsz<8) {
        bytesz = (bitsz * na->size - 1) / 8 + 1;
    } else {
        bytesz = (bitsz - 1) / 8 + 1;
        bytesz *= na->size;
    }
    return bytesz;
}


char *
na_get_pointer_for_write(VALUE self)
{
    size_t bytesz;
    char *ptr=NULL;
    narray_t *na;

    GetNArray(self,na);

    if (OBJ_FROZEN(self)) {
        rb_raise(rb_eRuntimeError, "cannot write frozen NArray.");
    }

    //if (NA_TEST_LOCK(na)) {
    //    rb_raise(rb_eRuntimeError, "cannot write locked NArray.");
    //}

    if (NA_SIZE(na)==0) {
        return NULL;
    }

    switch(NA_TYPE(na)) {
    case NARRAY_DATA_T:
        ptr = NA_DATA_PTR(na);
        if (ptr==NULL) {
            bytesz = na_bytesize(self);
            ptr = NA_DATA_PTR(na) = ALLOC_N(char,bytesz);
        }
        break;
    case NARRAY_FILEMAP_T:
        ptr = ((narray_filemap_t*)na)->ptr;
        break;
    case NARRAY_VIEW_T:
        ptr = na_get_pointer_for_write(NA_VIEW_DATA(na));
        //puts("pass NARRAY_VIEW_T in na_get_pointer_for_write");
        //ptr += ((narray_view_t*)na)->offset;
        //ptr += NA_VIEW_OFFSET(na);
        break;
    default:
        rb_raise(rb_eRuntimeError,"negative or too large number of dimensions?");
    }

    //NA_SET_LOCK(na);

    return ptr;
}

char *
na_get_pointer_for_read(VALUE self)
{
  //size_t bytesz;
    char  *ptr=NULL;
    narray_t *na;
    GetNArray(self,na);

    //if (NA_TEST_LOCK(na)) {
    //    rb_raise(rb_eRuntimeError, "cannot read locked NArray.");
    //}

    if (NA_SIZE(na)==0) {
        return NULL;
    }

    switch(NA_TYPE(na)) {
    case NARRAY_DATA_T:
        ptr = NA_DATA_PTR(na);
        if (ptr==NULL) {
            rb_bug("cannot read no-data NArray");
            rb_raise(rb_eRuntimeError,"cannot read no-data NArray");
        }
        break;
    case NARRAY_FILEMAP_T:
        ptr = NA_FILEMAP_PTR(na);
        break;
    case NARRAY_VIEW_T:
        ptr = na_get_pointer_for_read(NA_VIEW_DATA(na));
        //ptr += NA_VIEW_OFFSET(na);
        break;
    default:
        rb_raise(rb_eRuntimeError,"invalid narray internal type");
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
        for (; i<na->base.ndim-1; i++) {
            st1 = SDX_GET_STRIDE(na->stridx[i+1]);
            if (st0 != (ssize_t)(st1*na->base.shape[i+1])) {
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






VALUE
na_upcast(VALUE type1, VALUE type2)
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

    type = nary_s_upcast(CLASS_OF(x), CLASS_OF(y));
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
  Cast self to another NArray datatype.
  @overload cast_to(datatype)
  @param [Class] datatype NArray datatype.
  @return [NArray]
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
na_reduce_dimension(int argc, VALUE *argv, VALUE self)
{
    int ndim;
    int row_major;
    int i, r;
    size_t j;
    size_t len;
    ssize_t beg, step;
    VALUE v;
    narray_t *na;
    size_t m;
    volatile VALUE reduce;

    GetNArray(self,na);
    ndim = na->ndim;
    reduce = na->reduce;

    row_major = TEST_COLUMN_MAJOR(self);

    if (argc==0) {
        //printf("pass argc=0 reduce=%d\n",NUM2INT(reduce));
        return reduce;
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
            len = 1;
            step = 0;
            //printf("beg=%d step=%d len=%d\n",beg,step,len);
        } else if (rb_obj_is_kind_of(v,rb_cRange) || rb_obj_is_kind_of(v,na_cStep)) {
            nary_step_array_index( v, ndim, &len, &beg, &step );
        } else {
            rb_raise(rb_eTypeError, "invalid dimension argument %s", rb_obj_classname(v));
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
  @return [NArray] view of narray with inplace flag.
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
  @return [NArray] self
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
  @return [NArray] self
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
    /* define NArray class */
    cNArray = rb_define_class("NArray", rb_cObject);

#ifndef HAVE_RB_CCOMPLEX
    rb_require("complex");
    rb_cComplex = rb_const_get(rb_cObject, rb_intern("Complex"));
#endif

    nary_eCastError = rb_define_class_under(cNArray, "CastError", rb_eStandardError);
    nary_eShapeError = rb_define_class_under(cNArray, "ShapeError", rb_eStandardError);

    rb_define_singleton_method(cNArray, "debug=", na_debug_set, 1);
    rb_define_singleton_method(cNArray, "profile", na_profile, 0);
    rb_define_singleton_method(cNArray, "profile=", na_profile_set, 1);

    /* Ruby allocation framework  */
    rb_define_alloc_func(cNArray, na_s_allocate);
    rb_define_method(cNArray, "initialize", na_initialize, -1);
    rb_define_method(cNArray, "initialize_copy", na_initialize_copy, 1);

    rb_define_method(cNArray, "size", na_size, 0);
    rb_define_alias (cNArray, "length","size");
    rb_define_alias (cNArray, "total","size");
    rb_define_method(cNArray, "shape", na_shape, 0);
    rb_define_method(cNArray, "ndim", na_ndim,0);
    rb_define_alias (cNArray, "rank","ndim");

    rb_define_method(cNArray, "debug_info", rb_narray_debug_info, 0);

    //rb_define_singleton_method(cNArray, "upcast", nary_s_upcast, 1);
    rb_define_singleton_method(cNArray, "byte_size", nary_s_byte_size, 0);

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


    id_contiguous_stride = rb_intern(CONTIGUOUS_STRIDE);
    id_element_bit_size = rb_intern(ELEMENT_BIT_SIZE);
    id_element_byte_size = rb_intern(ELEMENT_BYTE_SIZE);

    id_reduce   = rb_intern("reduce");
    id_option   = rb_intern("option");
    id_loop_opt = rb_intern("loop_opt");
    id_init     = rb_intern("init");
    sym_reduce   = ID2SYM(id_reduce);
    sym_option   = ID2SYM(id_option);
    sym_loop_opt = ID2SYM(id_loop_opt);
    sym_init     = ID2SYM(id_init);

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
}
