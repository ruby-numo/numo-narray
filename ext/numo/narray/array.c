/*
  array.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2017 by Masahiro TANAKA
*/
#include <ruby.h>
#include "numo/narray.h"

// mdai: Multi-Dimensional Array Investigation
typedef struct {
  size_t shape;
  VALUE  val;
} na_mdai_item_t;

typedef struct {
    int   capa;
    na_mdai_item_t *item;
    int   type;    // Ruby numeric type - investigated separately
    VALUE na_type;  // NArray type
    VALUE int_max;
} na_mdai_t;

// Order of Ruby object.
enum { NA_NONE, NA_BIT, NA_INT32, NA_INT64, NA_RATIONAL,
       NA_DFLOAT, NA_DCOMPLEX, NA_ROBJ, NA_NTYPES };

static ID id_begin;
static ID id_end;
static ID id_step;
static ID id_abs;
static ID id_cast;
static ID id_le;
static ID id_Complex;

typedef struct {
    int     ndim;
    size_t *shape;
    VALUE   dtype;
} na_compose_t;

static size_t
na_compose_memsize(const void *ptr)
{
    const na_compose_t *nc = (const na_compose_t*)ptr;

    return sizeof(na_compose_t) + nc->ndim * sizeof(size_t);
}

static void
na_compose_free(void *ptr)
{
    na_compose_t *nc = (na_compose_t*)ptr;

    if (nc->shape)
        xfree(nc->shape);
    xfree(nc);
}

static void
na_compose_gc_mark(void* nc)
{
    rb_gc_mark(((na_compose_t*)nc)->dtype);
}

static const rb_data_type_t  compose_data_type = {
    "Numo::NArray/compose",
    {na_compose_gc_mark, na_compose_free, na_compose_memsize,},
    0, 0, RUBY_TYPED_FREE_IMMEDIATELY|RUBY_TYPED_WB_PROTECTED
};

#define WrapCompose(p) TypedData_Wrap_Struct(rb_cData, &compose_data_type, (void*)(p));
#define GetCompose(v,p) TypedData_Get_Struct(v, na_compose_t, &compose_data_type, p)

static VALUE
 na_object_type(int type, VALUE v)
{
    static VALUE int32_max = Qnil;
    if (NIL_P(int32_max))
	int32_max = ULONG2NUM(2147483647);

    switch(TYPE(v)) {

    case T_TRUE:
    case T_FALSE:
	if (type<NA_BIT)
	    return NA_BIT;
	return type;

#if SIZEOF_LONG == 4
    case T_FIXNUM:
	if (type<NA_INT32)
	    return NA_INT32;
	return type;
    case T_BIGNUM:
	if (type<NA_INT64) {
	    v = rb_funcall(v,id_abs,0);
	    if (RTEST(rb_funcall(v,id_le,1,int32_max))) {
		if (type<NA_INT32)
		    return NA_INT32;
	    } else {
		return NA_INT64;
	    }
	}
	return type;

#elif SIZEOF_LONG == 8
    case T_FIXNUM:
	if (type<NA_INT64) {
	    long x = NUM2LONG(v);
	    if (x<0) x=-x;
	    if (x<=2147483647) {
		if (type<NA_INT32)
		    return NA_INT32;
	    } else {
		return NA_INT64;
	    }
	}
	return type;
    case T_BIGNUM:
	if (type<NA_INT64)
	    return NA_INT64;
	return type;
#else
    case T_FIXNUM:
    case T_BIGNUM:
	if (type<NA_INT64) {
	    v = rb_funcall(v,id_abs,0);
	    if (RTEST(rb_funcall(v,id_le,1,int32_max))) {
		if (type<NA_INT32)
		    return NA_INT32;
	    } else {
		return NA_INT64;
	    }
	}
	return type;
#endif

    case T_FLOAT:
	if (type<NA_DFLOAT)
	    return NA_DFLOAT;
	return type;

    case T_NIL:
	return type;

    default:
	if (CLASS_OF(v) == rb_const_get( rb_cObject, id_Complex )) {
	    return NA_DCOMPLEX;
	}
    }
    return NA_ROBJ;
}


#define MDAI_ATTR_TYPE(tp,v,attr) \
 {tp = na_object_type(tp,rb_funcall(v,id_##attr,0));}

void na_mdai_object_type(na_mdai_t *mdai, VALUE v)
{
    if (IsNArray(v)) {
	if (NIL_P(mdai->na_type)) {
	    mdai->na_type = CLASS_OF(v);
	} else {
            mdai->na_type = na_upcast(CLASS_OF(v), mdai->na_type);
	}
    } else if (rb_obj_is_kind_of(v, rb_cRange)) {
        MDAI_ATTR_TYPE(mdai->type,v,begin);
        MDAI_ATTR_TYPE(mdai->type,v,end);
    } else if (rb_obj_is_kind_of(v, na_cStep)) {
        MDAI_ATTR_TYPE(mdai->type,v,begin);
        MDAI_ATTR_TYPE(mdai->type,v,end);
        MDAI_ATTR_TYPE(mdai->type,v,step);
    } else {
	mdai->type = na_object_type(mdai->type,v);
    }
}


static na_mdai_t *
na_mdai_alloc(VALUE ary)
{
    int i, n=4;
    na_mdai_t *mdai;

    mdai = ALLOC(na_mdai_t);
    mdai->capa = n;
    mdai->item = ALLOC_N( na_mdai_item_t, n );
    for (i=0; i<n; i++) {
	mdai->item[i].shape = 0;
	mdai->item[i].val = Qnil;
    }
    mdai->item[0].val = ary;
    mdai->type = NA_NONE;
    mdai->na_type = Qnil;

    return mdai;
}

static void
na_mdai_realloc(na_mdai_t *mdai, int n_extra)
{
    int i, n;

    i = mdai->capa;
    mdai->capa += n_extra;
    n = mdai->capa;
    REALLOC_N( mdai->item, na_mdai_item_t, n );
    for (; i<n; i++) {
	mdai->item[i].shape = 0;
	mdai->item[i].val = Qnil;
    }
}

static void
na_mdai_free(void *ptr)
{
    na_mdai_t *mdai = (na_mdai_t*)ptr;
    xfree(mdai->item);
    xfree(mdai);
}


/* investigate ndim, shape, type of Array */
static int
na_mdai_investigate(na_mdai_t *mdai, int ndim)
{
    ssize_t i;
    int j;
    size_t len, length;
    double dbeg, dstep;
    VALUE  v;
    VALUE  val;

    val = mdai->item[ndim-1].val;
    len = RARRAY_LEN(val);

    for (i=0; i < RARRAY_LEN(val); i++) {
        v = RARRAY_AREF(val,i);

	if (TYPE(v) == T_ARRAY) {
	    /* check recursive array */
	    for (j=0; j<ndim; j++) {
		if (mdai->item[j].val == v)
		    rb_raise(rb_eStandardError,
			     "cannot convert from a recursive Array to NArray");
	    }
	    if ( ndim >= mdai->capa ) {
		na_mdai_realloc(mdai,4);
	    }
	    mdai->item[ndim].val = v;
	    if ( na_mdai_investigate(mdai,ndim+1) ) {
		len--; /* Array is empty */
	    }
	}
	else
        if (rb_obj_is_kind_of(v, rb_cRange) || rb_obj_is_kind_of(v, na_cStep)) {
	    nary_step_sequence(v,&length,&dbeg,&dstep);
	    len += length-1;
	    na_mdai_object_type(mdai,v);
	}
	else {
	    na_mdai_object_type(mdai,v);

	    if (IsNArray(v)) {
		int r;
		narray_t *na;
		GetNArray(v,na);
		if ( na->ndim == 0 ) {
		    len--; /* NArray is empty */
		} else {
		    if ( ndim+na->ndim > mdai->capa ) {
			na_mdai_realloc(mdai,((na->ndim-1)/4+1)*4);
		    }
		    for ( j=0,r=ndim; j < na->ndim  ; j++,r++ ) {
			if ( mdai->item[r].shape < na->shape[j] )
			    mdai->item[r].shape = na->shape[j];
		    }
		}
	    }
	}
    }

    if (len==0) return 1; /* this array is empty */
    if (mdai->item[ndim-1].shape < len) {
	mdai->item[ndim-1].shape = len;
    }
    return 0;
}

static void
na_mdai_result(na_mdai_t *mdai, na_compose_t *nc)
{
    int i, ndim;
    VALUE tp;
    size_t *shape;

    // Dimension
    for (i=0; i < mdai->capa && mdai->item[i].shape > 0; i++) ;
    nc->ndim = ndim = i;
    nc->shape = NULL;
    nc->dtype = Qnil;

    if (ndim>0) {
	// Shape
        nc->shape = shape = ALLOC_N(size_t,ndim);
        for (i=0; i<ndim; i++) {
            shape[i] = mdai->item[i].shape;
        }

	// DataType
	switch(mdai->type) {
	case NA_BIT:
	    tp = numo_cBit;
	    break;
	case NA_INT32:
	    tp = numo_cInt32;
	    break;
	case NA_INT64:
	    tp = numo_cInt64;
	    break;
	case NA_DFLOAT:
	    tp = numo_cDFloat;
	    break;
	case NA_DCOMPLEX:
	    tp = numo_cDComplex;
	    break;
	case NA_ROBJ:
	    tp = numo_cRObject;
	    break;
	default:
	    tp = Qnil;
	}
	if (!NIL_P(mdai->na_type)) {
	    if (NIL_P(tp)) {
		tp = mdai->na_type;
	    } else {
                tp = na_upcast(mdai->na_type,tp);
	    }
	}
	nc->dtype = tp;
    }
}


static size_t
na_mdai_memsize(const void *ptr)
{
    const na_mdai_t *mdai = (const na_mdai_t*)ptr;

    return sizeof(na_mdai_t) + mdai->capa * sizeof(na_mdai_item_t);
}

static const rb_data_type_t mdai_data_type = {
    "Numo::NArray/mdai",
    {NULL, na_mdai_free, na_mdai_memsize,},
    0, 0, RUBY_TYPED_FREE_IMMEDIATELY|RUBY_TYPED_WB_PROTECTED
};

VALUE
na_ary_composition(VALUE ary)
{
    volatile VALUE vmdai, vnc;
    na_mdai_t *mdai;
    na_compose_t *nc;
    int j;

    nc = ALLOC(na_compose_t);
    vnc = WrapCompose(nc);
    if (TYPE(ary) == T_ARRAY) {
        mdai = na_mdai_alloc(ary);
        vmdai = TypedData_Wrap_Struct(rb_cData, &mdai_data_type, (void*)mdai);
        if ( na_mdai_investigate(mdai, 1) ) {
            // empty
            nc->ndim = 1;
            nc->shape = ALLOC_N(size_t, 1);
            nc->shape[0] = 0;
            nc->dtype = Qnil;
        } else {
            na_mdai_result(mdai, nc);
        }
        rb_gc_force_recycle(vmdai);
    } else if (IsNArray(ary)) {
        narray_t *na;
        GetNArray(ary,na);
        nc->ndim = na->ndim;
        nc->shape = ALLOC_N(size_t, na->ndim);
        for (j=0; j<na->ndim; j++) {
            nc->shape[j] = na->shape[j];
        }
        nc->dtype = CLASS_OF(ary);
    } else {
        rb_bug("invalid type for md-array: %s", rb_class2name(CLASS_OF(ary)));
    }
    return vnc;
}


static void
na_ary_composition2(VALUE ary, VALUE *type, VALUE *shape)
{
    VALUE vnc, dshape;
    na_compose_t *nc;
    int i;

    // investigate MD-Array
    vnc = na_ary_composition(ary);
    GetCompose(vnc,nc);
    dshape = rb_ary_new2(nc->ndim);
    for (i=0; i<nc->ndim; i++) {
        rb_ary_push(dshape, SIZET2NUM(nc->shape[i]));
    }
    if (shape) {*shape = dshape;}
    if (type) {*type = nc->dtype;}
    RB_GC_GUARD(vnc);
}

static VALUE
na_s_array_shape(VALUE mod, VALUE ary)
{
    VALUE shape;

    if (TYPE(ary)!=T_ARRAY) {
	// 0-dimension
	return rb_ary_new();
    }
    na_ary_composition2(ary, 0, &shape);
    return shape;
}

static inline void
check_subclass_of_narray(VALUE dtype) {
    if (RTEST(rb_obj_is_kind_of(dtype, rb_cClass))) {
        if (RTEST(rb_funcall(dtype, id_le, 1, cNArray))) {
            return;
        }
    }
    rb_raise(nary_eCastError, "cannot convert to NArray");
}


/*
  Generate new unallocated NArray instance with shape and type defined from obj.
  Numo::NArray.new_like(obj) returns instance whose type is defined from obj.
  Numo::DFloat.new_like(obj) returns DFloat instance.

  @overload new_like(obj)
  @param [Numeric,Array,Numo::NArray] obj
  @return [Numo::NArray]
  @example
    Numo::NArray.new_like([[1,2,3],[4,5,6]])
    => Numo::Int32#shape=[2,3](empty)
    Numo::DFloat.new_like([[1,2],[3,4]])
    => Numo::DFloat#shape=[2,2](empty)
    Numo::NArray.new_like([1,2i,3])
    => Numo::DComplex#shape=[3](empty)
*/
VALUE
na_s_new_like(VALUE type, VALUE obj)
{
    VALUE vnc, newary;
    na_compose_t *nc;

    if (RTEST(rb_obj_is_kind_of(obj,rb_cNumeric))) {
        // investigate type
        if (type == cNArray) {
            vnc = na_ary_composition(rb_ary_new3(1,obj));
            GetCompose(vnc,nc);
            type = nc->dtype;
        }
        check_subclass_of_narray(type);
        newary = nary_new(type, 0, 0);
    } else {
        // investigate MD-Array
        vnc = na_ary_composition(obj);
        GetCompose(vnc,nc);
        if (type == cNArray) {
            type = nc->dtype;
        }
        check_subclass_of_narray(type);
        newary = nary_new(type, nc->ndim, nc->shape);
    }
    RB_GC_GUARD(vnc);
    return newary;
}


VALUE
na_ary_composition_dtype(VALUE ary)
{
    volatile VALUE vnc;
    na_compose_t *nc;

    switch(TYPE(ary)) {
    case T_ARRAY:
        vnc = na_ary_composition(ary);
        GetCompose(vnc,nc);
        return nc->dtype;
    }
    return CLASS_OF(ary);
}

static VALUE
na_s_array_type(VALUE mod, VALUE ary)
{
    return na_ary_composition_dtype(ary);
}




/*
  Generate NArray object. NArray datatype is automatically selected.
  @overload [](elements)
  @param [Numeric,Array] elements
  @return [NArray]
*/
static VALUE
nary_s_bracket(VALUE klass, VALUE ary)
{
    VALUE dtype=Qnil;

    if (TYPE(ary)!=T_ARRAY) {
        rb_bug("Argument is not array");
    }
    dtype = na_ary_composition_dtype(ary);
    check_subclass_of_narray(dtype);
    return rb_funcall(dtype, id_cast, 1, ary);
}


VALUE
nst_check_compatibility(VALUE self, VALUE ary);


/* investigate ndim, shape, type of Array */
static int
na_mdai_for_struct(na_mdai_t *mdai, int ndim)
{
    size_t i;
    int j, r;
    size_t len;
    VALUE  v;
    VALUE  val;
    narray_t *na;

    //fprintf(stderr,"ndim=%d\n",ndim);    rb_p(mdai->na_type);
    if (ndim>4) { abort(); }
    val = mdai->item[ndim].val;

    //fpintf(stderr,"val = ");    rb_p(val);

    if (CLASS_OF(val) == mdai->na_type) {
        GetNArray(val,na);
        if ( ndim+na->ndim > mdai->capa ) {
            abort();
            na_mdai_realloc(mdai,((na->ndim-1)/4+1)*4);
        }
        for ( j=0,r=ndim; j < na->ndim; j++,r++ ) {
            if ( mdai->item[r].shape < na->shape[j] )
                mdai->item[r].shape = na->shape[j];
        }
        return 1;
    }

    if (TYPE(val) == T_ARRAY) {
        /* check recursive array */
        for (j=0; j<ndim-1; j++) {
            if (mdai->item[j].val == val)
                rb_raise(rb_eStandardError,
                         "cannot convert from a recursive Array to NArray");
        }
        //fprintf(stderr,"check:");        rb_p(val);
        // val is a Struct recort
        if (RTEST( nst_check_compatibility(mdai->na_type, val) )) {
            //fputs("compati\n",stderr);
            return 1;
        }
        // otherwise, multi-dimention
        if (ndim >= mdai->capa) {
            //fprintf(stderr,"exeed capa\n");            abort();
            na_mdai_realloc(mdai,4);
        }
        // finally, multidimension-check
        len = RARRAY_LEN(val);
        for (i=0; i < len; i++) {
            v = RARRAY_AREF(val,i);
            if (TYPE(v) != T_ARRAY) {
                //abort();
                return 0;
            }
        }
        for (i=0; i < len; i++) {
            v = RARRAY_AREF(val,i);
            //fprintf(stderr,"check:");            rb_p(v);
            mdai->item[ndim+1].val = v;
            if ( na_mdai_for_struct( mdai, ndim+1 ) == 0 ) {
                //fprintf(stderr,"not struct:");                rb_p(v);
                //abort();
                return 0;
            }
        }
        if (mdai->item[ndim].shape < len) {
            mdai->item[ndim].shape = len;
        }
        return 1;
    }

    //fprintf(stderr,"invalid for struct:");    rb_p(val);    abort();
    return 0;
}


VALUE
na_ary_composition_for_struct(VALUE nstruct, VALUE ary)
{
    volatile VALUE vmdai, vnc;
    na_mdai_t *mdai;
    na_compose_t *nc;

    mdai = na_mdai_alloc(ary);
    mdai->na_type = nstruct;
    vmdai = TypedData_Wrap_Struct(rb_cData, &mdai_data_type, (void*)mdai);
    na_mdai_for_struct(mdai, 0);
    nc = ALLOC(na_compose_t);
    vnc = WrapCompose(nc);
    na_mdai_result(mdai, nc);
    //fprintf(stderr,"nc->ndim=%d\n",nc->ndim);
    rb_gc_force_recycle(vmdai);
    return vnc;
}



void
Init_nary_array()
{
    //rb_define_singleton_method(cNArray, "mdai", na_mdai, 1);
    rb_define_singleton_method(cNArray, "array_shape", na_s_array_shape, 1);
    rb_define_singleton_method(cNArray, "array_type", na_s_array_type, 1);
    rb_define_singleton_method(cNArray, "new_like", na_s_new_like, 1);

    rb_define_singleton_method(cNArray, "[]", nary_s_bracket, -2);

    id_begin   = rb_intern("begin");
    id_end     = rb_intern("end");
    id_step    = rb_intern("step");
    id_cast    = rb_intern("cast");
    id_abs     = rb_intern("abs");
    id_le      = rb_intern("<=");
    id_Complex = rb_intern("Complex");
}
