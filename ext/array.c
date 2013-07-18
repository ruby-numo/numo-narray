/*
  array.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <ruby.h>
#include "narray.h"
//#include "narray_local.h"

/* Multi-Dimensional Array Investigation */
typedef struct {
  size_t shape;
  VALUE  val;
} na_mdai_item_t;

typedef struct {
    int   n;
    na_mdai_item_t *item;
    int   type;    // Ruby numeric type - investigated separately
    VALUE natype;  // NArray type
    VALUE int_max;
} na_mdai_t;

// Order of Ruby object.
enum { NA_NONE, NA_BIT, NA_INT32, NA_INT64, NA_RATIONAL,
       NA_DFLOAT, NA_DCOMPLEX, NA_ROBJ, NA_NTYPES };

VALUE
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
	    v = rb_funcall(v,rb_intern("abs"),0);
	    if (RTEST(rb_funcall(v,rb_intern("<="),1,int32_max))) {
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
	    v = rb_funcall(v,rb_intern("abs"),0);
	    if (RTEST(rb_funcall(v,rb_intern("<="),1,int32_max))) {
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
	if (CLASS_OF(v) == rb_const_get( rb_cObject, rb_intern("Complex") )) {
	    return NA_DCOMPLEX;
	}
    }
    return NA_ROBJ;
}


#define MDAI_ATTR_TYPE(tp,v,attr) \
 {tp = na_object_type(tp,rb_funcall(v,rb_intern(attr),0));}

void na_mdai_object_type(na_mdai_t *mdai, VALUE v)
{
    if (IsNArray(v)) {
	if (NIL_P(mdai->natype)) {
	    mdai->natype = CLASS_OF(v);
	} else {
	    mdai->natype = rb_funcall(CLASS_OF(v),rb_intern("cast_type"),
					1,mdai->natype);
	}
    } else if (rb_obj_is_kind_of(v, rb_cRange)) {
        MDAI_ATTR_TYPE(mdai->type,v,"begin");
        MDAI_ATTR_TYPE(mdai->type,v,"end");
    } else if (rb_obj_is_kind_of(v, na_cStep)) {
        MDAI_ATTR_TYPE(mdai->type,v,"begin");
        MDAI_ATTR_TYPE(mdai->type,v,"end");
        MDAI_ATTR_TYPE(mdai->type,v,"step");
    } else {
	mdai->type = na_object_type(mdai->type,v);
    }
}


static na_mdai_t *
na_mdai_alloc(VALUE ary)
{
    int i, n=2;
    na_mdai_t *mdai;

    mdai = ALLOC(na_mdai_t);
    mdai->n = n;
    mdai->item = ALLOC_N( na_mdai_item_t, n );
    for (i=0; i<n; i++) {
	mdai->item[i].shape = 0;
	mdai->item[i].val = Qnil;
    }
    mdai->item[0].val = ary;
    mdai->type = NA_NONE;
    mdai->natype = Qnil;

    return mdai;
}

static void
na_mdai_realloc(na_mdai_t *mdai, int n_extra)
{
    int i, n;

    i = mdai->n;
    mdai->n += n_extra;
    n = mdai->n;
    REALLOC_N( mdai->item, na_mdai_item_t, n );
    for (; i<n; i++) {
	mdai->item[i].shape = 0;
	mdai->item[i].val = Qnil;
    }
}

static size_t *
na_mdai_free(na_mdai_t *mdai, int *ndim, VALUE *type)
{
    int i;
    size_t *shape=NULL;
    VALUE tp;

    // Dimension
    for (i=0; i < mdai->n && mdai->item[i].shape > 0; i++) ;
    *ndim = i;

    if (*ndim>0) {
	// Shape
	shape = ALLOC_N(size_t,i);
	for (i=0; i<*ndim; i++) {
	    shape[i] = mdai->item[i].shape;
	}
	// DataType
	switch(mdai->type) {
	case NA_BIT:
	    tp = cBit;
	    break;
	case NA_INT32:
	    tp = cInt32;
	    break;
	case NA_INT64:
	    tp = cInt64;
	    break;
	case NA_DFLOAT:
	    tp = cDFloat;
	    break;
	case NA_DCOMPLEX:
	    tp = cDComplex;
	    break;
	case NA_ROBJ:
	    tp = cRObject;
	    break;
	default:
	    tp = Qnil;
	}
	if (!NIL_P(mdai->natype)) {
	    if (NIL_P(tp)) {
		tp = mdai->natype;
	    } else {
		tp = rb_funcall(mdai->natype,rb_intern("cast_type"),1,tp);
	    }
	}
	*type = tp;
    }
    xfree(mdai->item);
    xfree(mdai);
    return shape;
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
    //struct RArray *ary;

    val = mdai->item[ndim-1].val;
    len = RARRAY_LEN(val);

    for (i=0; i < RARRAY_LEN(val); i++) {
        v = RARRAY_PTR(val)[i];

	if (TYPE(v) == T_ARRAY) {
	    /* check recursive array */
	    for (j=0; j<ndim; j++) {
		if (mdai->item[j].val == v)
		    rb_raise(rb_eStandardError,
			     "cannot convert from a recursive Array to NArray");
	    }
	    if ( ndim >= mdai->n ) {
		na_mdai_realloc(mdai,2);
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
		    if ( ndim+na->ndim > mdai->n ) {
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



static VALUE
na_s_array_shape(VALUE mod, VALUE ary)
{
    int  i, ndim;
    VALUE type=Qnil;
    size_t *shape;
    na_mdai_t *mdai;
    VALUE vshape;

    if (TYPE(ary)!=T_ARRAY) {
	// 0-dimension
	return rb_ary_new();
    }
    // investigate MD-Array
    mdai = na_mdai_alloc(ary);
    na_mdai_investigate(mdai,1);
    // obtain properties
    shape = na_mdai_free(mdai,&ndim,&type);
    // make shape object
    vshape = rb_ary_new();
    for (i=0; i<ndim; i++) {
	rb_ary_push( vshape, SIZE2NUM(shape[i]) );
    }
    xfree(shape);
    return vshape;
}


VALUE
na_array_type(VALUE ary)
{
    int   ndim;
    VALUE type=Qnil;
    size_t *shape;
    na_mdai_t *mdai;

    switch(TYPE(ary)) {
    case T_ARRAY:
	mdai = na_mdai_alloc(ary);
	na_mdai_investigate(mdai,1);
	shape = na_mdai_free(mdai,&ndim,&type);
	xfree(shape);
	return type;
    }
    return CLASS_OF(ary);
}

static VALUE
na_s_array_type(VALUE mod, VALUE ary)
{
    return na_array_type(ary);
}


static VALUE
na_mdai(VALUE mod, VALUE ary)
{
    int  i, ndim;
    VALUE type=Qnil;
    size_t *shape;
    na_mdai_t *mdai;
    if (TYPE(ary)!=T_ARRAY) {
	printf("ndim=%d\n",0);
	return Qnil;
    }
    mdai  = na_mdai_alloc(ary);
    na_mdai_investigate(mdai,1);
    shape = na_mdai_free(mdai,&ndim,&type);
    printf("\ntype=%lx\n",type);
    printf("ndim=%d\n",ndim);
    for (i=0; i<ndim; i++) {
	printf(" shape[%d]=%ld\n",i,shape[i]);
    }
    return Qnil;
}

/*
size_t *
na_mdarray_investigate(VALUE ary, int *ndim, VALUE *type)
{
    int f;
    size_t *shape;
    na_mdai_t *mdai;

    if (TYPE(ary) != T_ARRAY) {
	puts("not Array");
	*ndim = 0;
	return NULL;
    }
    mdai = na_mdai_alloc(ary);
    f = na_mdai_investigate(mdai,1);
    shape = na_mdai_free(mdai,ndim,type);
    if (f) {
	*ndim = 1;
	shape = ALLOC(size_t);
	shape[0] = 0;
    }
    return shape;
}
*/

size_t *
na_mdarray_investigate(VALUE obj, int *ndim, VALUE *type)
{
    int i;
    size_t *shape;
    na_mdai_t *mdai;
    narray_t *na;

    switch(TYPE(obj)) {
    case T_ARRAY:
        mdai = na_mdai_alloc(obj);
        i = na_mdai_investigate(mdai,1);
        shape = na_mdai_free(mdai,ndim,type);
        if (i) {
            *ndim = 1;
            shape = ALLOC(size_t);
            shape[0] = 0;
        }
        return shape;
    case T_DATA:
        if (rb_obj_is_kind_of(obj,cNArray)==Qtrue) {
            GetNArray(obj,na);
            *ndim = NA_NDIM(na);
            shape = ALLOC_N(size_t,*ndim);
            for (i=0; i<*ndim; i++) {
                shape[i] = NA_SHAPE(na)[i];
            }
            return shape;
        }
    }
    *ndim = 0;
    return NULL;
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
    int ndim;
    VALUE type=Qnil;
    size_t *shape;
    na_mdai_t *mdai;

    if (TYPE(ary)!=T_ARRAY) {
        rb_bug("Argument is not array");
    }
    mdai = na_mdai_alloc(ary);
    na_mdai_investigate(mdai,1);
    shape = na_mdai_free(mdai,&ndim,&type);
    xfree(shape);
    if (RTEST(rb_funcall(type, rb_intern("<="), 1, cNArray))) {
        rb_raise(nary_eCastError, "cannot convert to NArray");
    }
    return rb_funcall(type,rb_intern("cast"),1,ary);
}


void
Init_nary_array()
{
    rb_define_singleton_method(cNArray, "mdai", na_mdai, 1);
    rb_define_singleton_method(cNArray, "array_shape", na_s_array_shape, 1);
    rb_define_singleton_method(cNArray, "array_type", na_s_array_type, 1);

    rb_define_singleton_method(cNArray, "[]", nary_s_bracket, -2);
}
