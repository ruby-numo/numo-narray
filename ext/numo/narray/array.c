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
#include "numo/narray.h"
//#include "narray_local.h"

/* Multi-Dimensional Array Investigation */
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
	if (NIL_P(mdai->na_type)) {
	    mdai->na_type = CLASS_OF(v);
	} else {
            mdai->na_type = na_upcast(CLASS_OF(v), CLASS_OF(mdai->na_type));
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
na_mdai_free(na_mdai_t *mdai)
{
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
	//shape = ALLOC_N(size_t,i);
	//for (i=0; i<*ndim; i++) {
	//    shape[i] = mdai->item[i].shape;
	//}
        nc->shape = shape = ALLOC_N(size_t,ndim);
        for (i=0; i<ndim; i++) {
            shape[i] = mdai->item[i].shape;
            //printf("shape[%d]=%d\n",i,shape[i]);
            //rb_ary_push( shape, SIZET2NUM(mdai->item[i].shape) );
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


VALUE
na_ary_composition(VALUE ary)
{
    volatile VALUE vmdai, vnc;
    na_mdai_t *mdai;
    na_compose_t *nc;
    int j;

    nc = ALLOC(na_compose_t);
    vnc = Data_Wrap_Struct(rb_cData, 0, -1, nc);
    if (TYPE(ary) == T_ARRAY) {
        mdai = na_mdai_alloc(ary);
        vmdai = Data_Wrap_Struct(rb_cData, 0, na_mdai_free, mdai);
        na_mdai_investigate(mdai, 1);
        na_mdai_result(mdai, nc);
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


static VALUE
na_s_array_shape(VALUE mod, VALUE ary)
{
    volatile VALUE vnc;
    VALUE shape;
    na_compose_t *nc;
    int i;

    if (TYPE(ary)!=T_ARRAY) {
	// 0-dimension
	return rb_ary_new();
    }
    // investigate MD-Array
    vnc = na_ary_composition(ary);
    Data_Get_Struct(vnc, na_compose_t, nc);
    shape = rb_ary_new2(nc->ndim);
    for (i=0; i<nc->ndim; i++) {
        rb_ary_push( shape, SIZET2NUM(nc->shape[i]) );
    }
    return shape;
}


VALUE
na_ary_composition_dtype(VALUE ary)
{
    volatile VALUE vnc;
    na_compose_t *nc;

    switch(TYPE(ary)) {
    case T_ARRAY:
        vnc = na_ary_composition(ary);
        Data_Get_Struct(vnc, na_compose_t, nc);
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

    if (RTEST(rb_obj_is_kind_of(dtype,rb_cClass))) {
        if (RTEST(rb_funcall(dtype,rb_intern("<="),1,cNArray))) {
            return rb_funcall(dtype,rb_intern("cast"),1,ary);
        }
    }
    rb_raise(nary_eCastError, "cannot convert to NArray");
    return Qnil;
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
    vmdai = Data_Wrap_Struct(rb_cData, 0, na_mdai_free, mdai);
    na_mdai_for_struct(mdai, 0);
    nc = ALLOC(na_compose_t);
    vnc = Data_Wrap_Struct(rb_cData, 0, -1, nc);
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

    rb_define_singleton_method(cNArray, "[]", nary_s_bracket, -2);
}
