/*
  nstrut.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2007 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <ruby.h>
//#include <math.h>
////#include <version.h>
#include "narray.h"
////#include "narray_local.h"
//#include "bytedata.h"
//
#include "template.h"
//#include "template_int.h"
//#include "template_data.h"
//#include "template_comp.h"

VALUE cStruct;

/*
VALUE cNStMember;

typedef struct {
    VALUE type;
    VALUE name;
    size_t ofs;
    size_t size;
} nst_member_t;


static void
nst_member_mark(nst_member_t *m)
{
    rb_gc_mark(m->type);
    rb_gc_mark(m->name);
}

static VALUE
nst_member_s_allocate(VALUE klass)
{
    nst_member_t *m = ALLOC(nst_member_t);
    m->type = Qnil;
    m->name = Qnil;
    m->ofs  = 0;
    m->size = 0;
    return Data_Wrap_Struct(klass, nst_member_mark, 0, m);
}

static VALUE
nst_member_initialize(VALUE self, VALUE type, VALUE name, VALUE vofs, VALUE vsize)
{
    nst_member_t *m;
    Data_Get_Struct(self, nst_member_t, m);
    m->type = type;
    m->name = name;
    m->ofs  = NUM2SIZE(vofs);
    m->size = NUM2SIZE(vsize);
    return self;
}

static VALUE
nst_member_new(VALUE klass, VALUE type, VALUE name, VALUE vofs, VALUE vsize){
    VALUE mem = nst_member_s_allocate(klass);
    nst_member_initialize(mem, type, name, vofs, vsize);
    return mem;
}
*/



VALUE
nst_definition(VALUE nst, VALUE idx)
{
    long i;
    VALUE def = rb_const_get(CLASS_OF(nst), rb_intern("DEFINITIONS"));
    long  len = RARRAY_LEN(def);

    if (TYPE(idx) == T_STRING || TYPE(idx) == T_SYMBOL) {
	ID    id  = rb_to_id(idx);
	for (i=0; i<len; i++) {
	    VALUE key = RARRAY_PTR(RARRAY_PTR(def)[i])[0];
	    if (SYM2ID(key) == id) {
		return RARRAY_PTR(def)[i];
	    }
	}
	rb_name_error(id, "no member '%s' in struct", rb_id2name(id));
	return Qnil;		/* not reached */
	//return rb_struct_aref_id(s, rb_to_id(idx));
    }

    i = NUM2LONG(idx);
    if (i<-len || i>=len)
        rb_raise(rb_eIndexError,
		 "offset %ld out of range of struct(size:%ld)", i, len);
    return RARRAY_PTR(def)[i];
}

VALUE
nst_definitions(VALUE nst)
{
    return rb_const_get(CLASS_OF(nst), rb_intern("DEFINITIONS"));
}


void na_copy_array_structure(VALUE self, VALUE view);

VALUE
nst_field(VALUE self, VALUE idx)
{
    int  ndim, i;
    narray_t *na, *nv, *nt;
    VALUE def, type, ofs, view;
    ssize_t *stride1, *stride2, *stride3;
    size_t *shape;

    def  = nst_definition(self, idx);
    type = RARRAY_PTR(def)[1];
    ofs  = RARRAY_PTR(def)[2];

    GetNArray(self,na);
    if (NIL_P(na->data)) {
	na_get_pointer_for_write(self);
	//rb_raise(rb_eRuntimeError, "na->data not allocated");
    }

    stride1 = na_get_stride(self);

    if (TYPE(type)==T_CLASS) {
	if (RTEST(rb_class_inherited_p(type,cNArray))) {
	    view = rb_narray_new(type, na->ndim, na->shape);
	    na_copy_array_structure(self, view);
	    GetNArray(view,na);
	    na->offset += NUM2SIZE(ofs);
	    na_copy_flags(self, view);
	    return view;
	}
    } else if (rb_obj_is_kind_of(type,cNArray)) {
	GetNArray(type,nt);
	ndim = na->ndim + nt->ndim;
	shape = ALLOCA_N(size_t,ndim);
	for (i=0; i<na->ndim; i++) {
	    shape[i] = na->shape[i];
	}
	for (; i<ndim; i++) {
	    shape[i] = nt->shape[i-na->ndim];
	}
	view = rb_narray_new(CLASS_OF(type), ndim, shape);
	//na_copy_array_structure(self, view);
	GetNArray(view,nv);
	nv->data = na->data;
	nv->offset = na->offset + NUM2SIZE(ofs);
	stride2 = na_get_stride(view);
	//stride3 = na_get_stride(type);
	for (i=0; i<na->ndim; i++) {
	    stride2[i] = stride1[i];
	}
	//for (i=0; i<nt->ndim; i++) {
	//    stride2[i+na->ndim] = stride3[i];
	//}
	na_copy_flags(type, view);
	return view;
    }
    rb_raise(rb_eTypeError, "invalid type");
    return Qnil;
}


static VALUE
nst_s_new(int argc, VALUE *argv, VALUE klass)
{
    VALUE name=Qnil, rest, size;
    long i;
    VALUE st, members;
    ID id;

    rb_scan_args(argc, argv, "0*", &rest);
    if (RARRAY_LEN(rest)>0) {
	name = RARRAY_PTR(rest)[0];
	if (!NIL_P(name)) {
	    VALUE tmp = rb_check_string_type(name);
	    if (!NIL_P(tmp)) {
		rb_ary_shift(rest);
	    } else {
		name = Qnil;
	    }
	}
    }

    /*
    for (i=0; i<RARRAY(rest)->len; i++) {
        id = rb_to_id(RARRAY(rest)->ptr[i]);
        RARRAY(rest)->ptr[i] = ID2SYM(id);
    }
    if (!NIL_P(name)) {
        VALUE tmp = rb_check_string_type(name);

        if (NIL_P(tmp)) {
            id = rb_to_id(name);
            rb_ary_unshift(rest, ID2SYM(id));
            name = Qnil;
        }
    }
    st = make_struct(name, rest, klass);
    */

    if (NIL_P(name)) {
        st = rb_class_new(klass);
        rb_make_metaclass(st, RBASIC(klass)->klass);
        rb_class_inherited(klass, st);
    }
    else {
        char *cname = StringValuePtr(name);
        id = rb_intern(cname);
        if (!rb_is_const_id(id)) {
            rb_name_error(id, "identifier %s needs to be constant", cname);
        }
        if (rb_const_defined_at(klass, id)) {
            rb_warn("redefining constant Struct::%s", cname);
            rb_mod_remove_const(klass, ID2SYM(id));
        }
        st = rb_define_class_under(klass, rb_id2name(id), klass);
    }

    rb_iv_set(st, "__members__", rb_ary_new());
    rb_iv_set(st, "__offset__", INT2FIX(0));

    if (rb_block_given_p()) {
        rb_mod_module_eval(0, 0, st);
    }

    size = rb_iv_get(st, "__offset__");
    members = rb_iv_get(st, "__members__");
    printf("size=%d\n",NUM2INT(size));
    rb_define_const(st, CONTIGUOUS_STRIDE, size);
    rb_define_const(st, ELEMENT_BYTE_SIZE, size);
    rb_define_const(st, ELEMENT_BIT_SIZE,  rb_funcall(size,'*',1,INT2FIX(8)));

    OBJ_FREEZE(members);
    rb_define_const(st, "DEFINITIONS", members);

    rb_define_singleton_method(st, "new", rb_class_new_instance, -1);
    rb_define_singleton_method(st, "[]", rb_class_new_instance, -1);

    return st;
}


/*
static VALUE
nst_s_add_type(VALUE nst, VALUE type, VALUE name)
{
    VALUE ofs, size;
    ID id;

    id = rb_to_id(name);
    name = ID2SYM(id);
    if (rb_obj_is_kind_of(type,cNArray)) {
	narray_t *na;
	GetNArray(type,na);
	type = rb_narray_new(CLASS_OF(type),na->ndim,na->shape);
    }
    size = rb_funcall(type, rb_intern("byte_size"), 0);
    ofs  = rb_iv_get(nst, "__offset__");
    //mem  = nst_member_new(cNStMember, type, name, ofs, size);
    rb_iv_set(nst, "__offset__", rb_funcall(ofs,'+',1,size));
    rb_ary_push(rb_iv_get(nst,"__members__"), rb_ary_new3(4,name,type,ofs,size));
    return Qnil;
}
*/



static VALUE
nstruct_add_type(VALUE type, int argc, VALUE *argv, VALUE nst)
{
    VALUE ofs, size;
    ID id;
    int i;
    VALUE name=Qnil;
    size_t *shape=NULL;
    int ndim=0;

    for (i=0; i<argc; i++) {
	switch(TYPE(argv[i])) {
	case T_STRING:
	case T_SYMBOL:
	    if (NIL_P(name)) {
		name = argv[i];
		break;
	    }
	    rb_raise(rb_eArgError,"multiple name in struct definition");
	case T_ARRAY:
	    if (shape) {
		rb_raise(rb_eArgError,"multiple shape in struct definition");
	    }
	    ndim = RARRAY_LEN(argv[i]);
	    if (ndim > 128) {
		rb_raise(rb_eArgError,"too large number of dimensions");
	    }
	    if (ndim > 0) {
		shape = ALLOCA_N(size_t, ndim);
		na_to_internal_shape(Qnil, argv[i], shape);
	    }
	    break;
	}
    }

    id = rb_to_id(name);
    name = ID2SYM(id);
    if (shape) {
	type = rb_narray_new(type,ndim,shape);
    } else if (rb_obj_is_kind_of(type,cNArray)) {
	narray_t *na;
	GetNArray(type,na);
	type = rb_narray_new(CLASS_OF(type),na->ndim,na->shape);
    }
    size = rb_funcall(type, rb_intern("byte_size"), 0);
    ofs  = rb_iv_get(nst, "__offset__");
    //mem  = nst_member_new(cNStMember, type, name, ofs, size);
    rb_iv_set(nst, "__offset__", rb_funcall(ofs,'+',1,size));
    rb_ary_push(rb_iv_get(nst,"__members__"), rb_ary_new3(4,name,type,ofs,size));
    return Qnil;
}




static VALUE
nst_extract(VALUE self)
{
    return self;
}


static VALUE
nst_nstruct_to_object(VALUE vns)
{
    VALUE def, type, vofs, vsize, velm;
    VALUE vne, vary;
    VALUE defs = rb_const_get(CLASS_OF(vns), rb_intern("DEFINITIONS"));
    long  i, len = RARRAY_LEN(defs);
    size_t size, ofs;
    narray_t *ns, *ne, *nt;

    GetNArray(vns,ns);
    if (ns->data==Qnil)
	rb_raise(rb_eRuntimeError,"data not allocated");
    if (ns->ndim!=0)
	rb_raise(rb_eRuntimeError,"Struct dimention must be 0");
    if (ns->size!=1)
	rb_raise(rb_eRuntimeError,"Struct size must be 1");

    vary = rb_ary_new2(len);

    for (i=0; i<len; i++) {
	def = RARRAY_PTR(defs)[i];
	type  = RARRAY_PTR(def)[1];
	vofs  = RARRAY_PTR(def)[2];
	ofs   = NUM2SIZE(vofs);
	vsize = RARRAY_PTR(def)[3];
	size  = NUM2SIZE(vsize);
	if (rb_obj_is_kind_of(type,cNArray)) {
	    vne = type;
	    //GetNArray(type,nt);
	    //vne = rb_narray_new(CLASS_OF(type),nt->ndim,nt->shape);
	    //GetNArray(vne,ne);
	    //ne->data = ns->data;
	    //ne->offset = ns->offset + ofs;
	    //velm = rb_funcall(vne,rb_intern("to_a"),0);
	    //rb_ary_push(vary, velm);
	} else {
	    vne = rb_narray_new(type,0,NULL);
	}
	GetNArray(vne,ne);
	ne->data = ns->data;
	ne->offset = ns->offset + ofs;
	velm = rb_funcall(vne,rb_intern("to_a"),0);
	ne->data = Qnil;
	ne->offset = 0;
	rb_ary_push(vary, velm);
    }
    return vary;
}


static VALUE
nst_object_to_nstruct(VALUE val, VALUE nst_class)
{
    VALUE def, type, vofs, vsize, velm;
    VALUE vns, vne;
    VALUE defs = rb_const_get(nst_class, rb_intern("DEFINITIONS"));
    long  i, len = RARRAY_LEN(defs);
    char *ptr;
    size_t size, ofs;
    size_t shape=1;
    narray_t *ns, *ne;

    if (TYPE(val)==T_ARRAY) {
	if (len != RARRAY_LEN(val)) {
	    rb_raise(rb_eArgError,"wrong Array size (%ld), expected %ld",
		     RARRAY_LEN(val), len);
	}
	vns = rb_narray_new(nst_class,0,NULL);
	na_alloc_data(vns);
	GetNArray(vns,ns);
	for (i=0; i<len; i++) {
	    def = RARRAY_PTR(defs)[i];
	    type  = RARRAY_PTR(def)[1];
	    vofs  = RARRAY_PTR(def)[2];
	    ofs   = NUM2SIZE(vofs);
	    vsize = RARRAY_PTR(def)[3];
	    size  = NUM2SIZE(vsize);

	    velm = RARRAY_PTR(val)[i];

	    if (rb_obj_is_kind_of(type,cNArray)) {
		vne = type;
	    } else {
		vne = rb_narray_new(type,0,NULL);
	    }
	    GetNArray(vne,ne);
	    ne->data = ns->data;
	    ne->offset = ns->offset + ofs;
	    rb_funcall(vne,rb_intern("store"),1,velm);
	    ne->data = Qnil;
	    ne->offset = 0;
	}
	return vns;
    }
    rb_raise(rb_eArgError,"wrong argument type");
    return Qnil;
}


static void
iter_nstruct_fill(na_loop_t *const lp)
{
    size_t   i;
    char    *p1, *src;
    ssize_t  s1;
    ssize_t *idx1;
    size_t   e1;
    VALUE opt = *(VALUE*)(lp->opt_ptr);

    INIT_COUNTER(lp, i);
    INIT_PTR_ELM(lp, 0, p1, s1, idx1, e1);
    src = na_get_pointer_for_read(opt);
    if (idx1) {
	for (; i--;) { memcpy(p1+*(idx1++), src, e1);}
    } else {
	for (; i--;) { memcpy(p1, src, e1); p1+=s1; }
    }
}


static VALUE
nary_nstruct_fill(VALUE self, VALUE val)
{
    ndfunc_t *func;
    volatile VALUE vst;

    func = ndfunc_alloc(iter_nstruct_fill, FULL_LOOP, 1, 0, Qnil);
    vst = nst_object_to_nstruct(val, CLASS_OF(self));
    ndfunc_execute(func, 2, self, vst);
    ndfunc_free(func);
    return self;
}


void
cast_nstruct_to_array(na_loop_t *const lp, VALUE self)
{
    size_t i, s1, s2;
    size_t p1;
    char  *p2;
    size_t *idx1, *idx2;
    volatile VALUE x, y;
    narray_t *ns, *na;

    GetNArray(self,ns);
    x = rb_narray_new(CLASS_OF(self),0,NULL);
    GetNArray(x,na);
    na->data = ns->data;

    INIT_COUNTER(lp, i);
    p1 = lp[0].pos;
    s1 = lp[0].step;
    idx1 = lp[0].idx;
    INIT_PTR(lp, 1, p2, s2, idx2);
    for (; i--;) {
	if (idx1) {
	    na->offset = p1 + *idx1;
	    idx1++;
	} else {
	    na->offset = p1;
	    p1 += s1;
	}
	y = nst_nstruct_to_object(x);
	STORE_DATA_STEP(p2, s2, idx2, VALUE, y);
    }
}

static VALUE
nary_cast_nstruct_to_array(VALUE self)
{
    VALUE v;
    ndfunc_t *func;

    func = ndfunc_alloc(cast_nstruct_to_array, FULL_LOOP,
			 1, 1, Qnil, rb_cArray);
    v = ndfunc_execute_to_rarray(func, self, self);
    ndfunc_free(func);
    return v;
}


static void
nstruct_print_loop(char *ptr, size_t pos, VALUE vne)
{
    narray_t *ne;
    VALUE val;
    VALUE sval;
    char *str;
    size_t tmp;

    GetNArray(vne,ne);
    tmp = ne->offset;
    ne->offset += pos;
    val = nst_nstruct_to_object(vne);
    ne->offset = tmp;
    //str = StringValueCStr(rb_funcall(val,rb_intern("inspect"),0));
    sval = rb_funcall(val,rb_intern("inspect"),0);
    str = StringValueCStr(sval);
    printf("%s", str);
}

VALUE
nary_nstruct_debug_print(VALUE self)
{
    narray_t *ns, *ne;
    VALUE vne = rb_narray_new(CLASS_OF(self),0,NULL);

    GetNArray(self,ns);
    GetNArray(vne,ne);
    ne->data = ns->data;
    ne->offset = ns->offset;

    if (NIL_P(ne->data)) {
	puts("(data not allocated)");
	return Qnil;
    }

    ndfunc_debug_print(self, nstruct_print_loop, vne);
    return Qnil;
}


static VALUE
nst_s_add_type(int argc, VALUE *argv, VALUE mod)
{
    if (argc==0)
	rb_raise(rb_eArgError,
		 "wrong number of arguments (%d for 1)", argc);
    nstruct_add_type(argv[0],argc-1,argv+1,mod);
    return Qnil;
}



#define NST_TYPEDEF(tpname,tpclass)                 \
static VALUE                                        \
nst_s_##tpname(VALUE argc, VALUE *argv, VALUE mod)  \
{   nstruct_add_type(tpclass,argc,argv,mod);        \
    return Qnil;                                    \
}

NST_TYPEDEF(int8,cInt8)
NST_TYPEDEF(int16,cInt16)
NST_TYPEDEF(int32,cInt32)
NST_TYPEDEF(int64,cInt64)
NST_TYPEDEF(uint8, cUInt8)
NST_TYPEDEF(uint16,cUInt16)
NST_TYPEDEF(uint32,cUInt32)
NST_TYPEDEF(uint64,cUInt64)
NST_TYPEDEF(dfloat,cDFloat)
NST_TYPEDEF(dcomplex,cDComplex)
NST_TYPEDEF(sfloat,cSFloat)
NST_TYPEDEF(scomplex,cSComplex)


#define rb_define_singleton_alias(klass,name1,name2) \
    rb_define_alias(rb_singleton_class(klass),name1,name2)

void
Init_nstruct()
{
    cStruct = rb_define_class_under(cNArray, "Struct", cNArray);
    //cNStMember = rb_define_class_under(cStruct, "Member", rb_cObject);

    //rb_define_alloc_func(cNStMember, nst_member_s_allocate);
    //rb_define_method(cNStMember, "initialize", nst_member_initialize, -1);

    //rb_undef_alloc_func(cStruct);
    rb_define_singleton_method(cStruct, "new", nst_s_new, -1);
    rb_define_singleton_method(cStruct, "add_type", nst_s_add_type, -1);
    rb_define_singleton_method(cStruct, "int8",   nst_s_int8,   -1);
    rb_define_singleton_method(cStruct, "int16",  nst_s_int16,  -1);
    rb_define_singleton_method(cStruct, "int32",  nst_s_int32,  -1);
    rb_define_singleton_method(cStruct, "int64",  nst_s_int64,  -1);
    rb_define_singleton_method(cStruct, "uint8",  nst_s_uint8,  -1);
    rb_define_singleton_method(cStruct, "uint16", nst_s_uint16, -1);
    rb_define_singleton_method(cStruct, "uint32", nst_s_uint32, -1);
    rb_define_singleton_method(cStruct, "uint64", nst_s_uint64, -1);
    rb_define_singleton_method(cStruct, "sfloat",   nst_s_sfloat, -1);
    rb_define_singleton_alias (cStruct, "float32", "sfloat");
    rb_define_singleton_method(cStruct, "scomplex", nst_s_scomplex, -1);
    rb_define_singleton_alias (cStruct, "complex64", "scomplex");
    rb_define_singleton_method(cStruct, "dfloat",   nst_s_dfloat, -1);
    rb_define_singleton_alias (cStruct, "float64", "dfloat");
    rb_define_singleton_method(cStruct, "dcomplex", nst_s_dcomplex, -1);
    rb_define_singleton_alias (cStruct, "complex128", "dcomplex");

    rb_define_method(cStruct, "definition", nst_definition, 1);
    rb_define_method(cStruct, "definitions", nst_definitions, 0);
    rb_define_method(cStruct, "field", nst_field, 1);
    rb_define_method(cStruct, "extract", nst_extract, 0);
    rb_define_method(cStruct, "fill", nary_nstruct_fill, 1);

    rb_define_method(cStruct, "debug_print", nary_nstruct_debug_print, 0);

    rb_define_method(cStruct, "to_a", nary_cast_nstruct_to_array, 0);

    //rb_define_method(cStruct, "initialize", rb_struct_initialize, -2);
    //rb_define_method(cStruct, "initialize_copy", rb_struct_init_copy, 1);
}
