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
    int   type;
    VALUE datatype;
    VALUE int_max;
} na_mdai_t;

enum { NA_NONE, NA_BIT, NA_INT32, NA_INT64, NA_DFLOAT, NA_DCOMPLEX, NA_ROBJ, NA_NTYPES };
//Bit, Int32, Int64, DFloat, DComplex, RObject


void na_object_type_(na_mdai_t *mdai, VALUE v)
{
    static VALUE int32_max = Qnil;
    if (NIL_P(int32_max))
	int32_max = ULONG2NUM(2147483647);

    switch(TYPE(v)) {

    case T_TRUE:
    case T_FALSE:
	if (mdai->type<NA_BIT)
	    mdai->type = NA_BIT;
	return;

#if SIZEOF_LONG == 4
    case T_FIXNUM:
	if (mdai->type<NA_INT32)
	    mdai->type = NA_INT32;
	return;
    case T_BIGNUM:
	if (mdai->type<NA_INT64) {
	    v = rb_funcall(v,rb_intern("abs"),0);
	    if (RTEST(rb_funcall(v,rb_intern("<="),1,int32_max))) {
		if (mdai->type<NA_INT32)
		    mdai->type = NA_INT32;
	    } else {
		mdai->type = NA_INT64;
	    }
	}
	return;

#elif SIZEOF_LONG == 8
    case T_FIXNUM:
	if (mdai->type<NA_INT64) {
	    long x = NUM2LONG(v);
	    if (x<0) x=-x;
	    if (x<=2147483647) {
		if (mdai->type<NA_INT32)
		    mdai->type = NA_INT32;
	    } else {
		mdai->type = NA_INT64;
	    }
	}
	return;
    case T_BIGNUM:
	if (mdai->type<NA_INT64)
	    mdai->type = NA_INT64;
	return;
#else
    case T_FIXNUM:
    case T_BIGNUM:
	if (mdai->type<NA_INT64) {
	    v = rb_funcall(v,rb_intern("abs"),0);
	    if (RTEST(rb_funcall(v,rb_intern("<="),1,int32_max))) {
		if (mdai->type<NA_INT32)
		    mdai->type = NA_INT32;
	    } else {
		mdai->type = NA_INT64;
	    }
	}
	return;
#endif

    case T_FLOAT:
	if (mdai->type<NA_DFLOAT)
	    mdai->type = NA_DFLOAT;
	return;

    case T_NIL:
	return;

    default:
	if (IsNArray(v)) {
	    if (NIL_P(mdai->datatype)) {
		mdai->datatype = CLASS_OF(v);
	    } else {
	    	mdai->datatype = rb_funcall(CLASS_OF(v),
					    rb_intern("cast_type"),
					    1,mdai->datatype);
	    }
	    return;
	}
	if (CLASS_OF(v) == rb_const_get( rb_cObject, rb_intern("Complex") )) {
	    mdai->type = NA_DCOMPLEX;
	    return;
	}
    }
    mdai->type = NA_ROBJ;
}


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


void na_mdai_object_type(na_mdai_t *mdai, VALUE v)
{
    if (IsNArray(v)) {
	if (NIL_P(mdai->datatype)) {
	    mdai->datatype = CLASS_OF(v);
	} else {
	    mdai->datatype = rb_funcall(CLASS_OF(v),rb_intern("cast_type"),
					1,mdai->datatype);
	}
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
    //mdai->type = ALLOC_N( int, NA_NTYPES );
    //for (i=0; i<NA_NTYPES; i++)
    //	mdai->type[i]=0;
    mdai->type = NA_NONE;
    mdai->datatype = Qnil;

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
    int i; //, t, r;
    size_t *shape=NULL;
    VALUE tp;

    //for (t=i=NA_BYTE; i<NA_NTYPES; i++) {
    //  if ( mdai->type[i] > 0 )
    //    t = na_upcast[t][i];
    //}
    //*type = t;

    // Dimension
    for (i=0; i < mdai->n && mdai->item[i].shape > 0; i++) ;
    //for (i=0; i < mdai->n; i++) ;
    *ndim = i;

    if (*ndim>0) {
	// Shape
	shape = ALLOC_N(size_t,i);
	//for (i=0; r-->0; i++) {
	for (i=0; i<*ndim; i++) {
	    //shape[i] = mdai->item[r].shape;
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
	if (!NIL_P(mdai->datatype)) {
	    if (NIL_P(tp)) {
		tp = mdai->datatype;
	    } else {
		tp = rb_funcall(mdai->datatype,rb_intern("cast_type"),1,tp);
	    }
	}
	*type = tp;
    }
    //xfree(mdai->type);
    xfree(mdai->item);
    xfree(mdai);
    return shape;
}


#define EXCL(r) (RTEST(rb_funcall((r),rb_intern("exclude_end?"),0)))

/* Range as a Sequence of numbers */
static void
na_range_to_sequence(VALUE obj, size_t *n, size_t *beg, size_t *step)
{
    int end,len;

    *beg = NUM2INT(rb_ivar_get(obj, rb_intern("begin")));
    end = NUM2INT(rb_ivar_get(obj, rb_intern("end")));
    len = end - *beg;

    /* direction */
    if (len>0) {
	*step = 1;
	if (EXCL(obj)) end--; else len++;
    }
    else if (len<0) {
	len   = -len;
	*step = -1;
	if (EXCL(obj)) end++; else len++;
    }
    else /*if(len==0)*/ {
	*step = 0;
	if (!EXCL(obj)) {
	    len++;
	}
    }
    *n = len;
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
    //ary = RARRAY(mdai->item[ndim-1].val);
    //len = ary->len;
    len = RARRAY_LEN(val);

    //for (i=0; i < ary->len; i++) {
    for (i=0; i < RARRAY_LEN(val); i++) {

	//printf("i=%d\n",i);

        //v = ary->ptr[i];
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
	//if ( rb_obj_is_kind_of(v, rb_cRange) ) {
	//    na_range_to_sequence(v,&length,&start,&dir);
        if (rb_obj_is_kind_of(v, rb_cRange) || rb_obj_is_kind_of(v, na_cStep)) {
	    nary_step_sequence(v,&length,&dbeg,&dstep);
	    len += length-1;
	    //mdai->type[ na_object_type(rb_ivar_get(v, rb_intern("beg"))) ] = 1;
	    //mdai->type[ na_object_type(rb_ivar_get(v, rb_intern("end"))) ] = 1;
	    na_mdai_object_type(mdai,rb_ivar_get(v, rb_intern("begin")));
	    na_mdai_object_type(mdai,rb_ivar_get(v, rb_intern("end")));
	}
	else {

	    //mdai->type[ na_object_type(v) ] = 1;
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
		    //for ( j=na->ndim, r=ndim; j-- > 0  ; r++ ) {
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
    //if (RARRAY(ary)->len < 1) {
    //	// empty array
    //	vshape = rb_ary_new();
    //	rb_ary_push( vshape, INT2FIX(0) );
    //	return vshape;
    //}
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
    //// make shape object
    //vshape = rb_ary_new();
    //for (i=0; i<ndim; i++) {
    //	rb_ary_push( vshape, SIZE2NUM(shape[i]) );
    //}
    //xfree(shape);
    //return vshape;
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
    size_t *shape; //, *idx;
    na_mdai_t *mdai;
    //narray_t *na;
    //VALUE v;

    /* empty array */
    //if (RARRAY(ary)->len < 1) {
    //	return Qnil; //na_make_empty( type, klass );
    //}
    if (TYPE(ary)!=T_ARRAY) {
	printf("ndim=%d\n",0);
	return Qnil;
    }
    mdai  = na_mdai_alloc(ary);
    na_mdai_investigate(mdai,1);
    shape = na_mdai_free(mdai,&ndim,&type);
    printf("\ntype=%lx\n",type);
    //printf("datatype=%d\n",datatype);
    printf("ndim=%d\n",ndim);
    for (i=0; i<ndim; i++) {
	printf(" shape[%d]=%ld\n",i,shape[i]);
    }
    return Qnil;
    //return rb_narray_new( type, ndim, shape );
}


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

void
Init_na_array()
{
    rb_define_singleton_method(cNArray, "mdai", na_mdai, 1);
    rb_define_singleton_method(cNArray, "array_shape", na_s_array_shape, 1);
    rb_define_singleton_method(cNArray, "array_type", na_s_array_type, 1);
}

