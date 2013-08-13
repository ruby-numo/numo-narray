/*
  nstrut.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2007,2013 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <ruby.h>
#include "narray.h"
#include "template.h"

VALUE cStruct;

static inline VALUE
nst_definitions(VALUE nst)
{
    return rb_const_get(CLASS_OF(nst), rb_intern("DEFINITIONS"));
}

static VALUE
nst_definition(VALUE nst, VALUE idx)
{
    long i;
    VALUE def = nst_definitions(nst);
    long  len = RARRAY_LEN(def);

    if (TYPE(idx) == T_STRING || TYPE(idx) == T_SYMBOL) {
        ID id  = rb_to_id(idx);
        for (i=0; i<len; i++) {
            VALUE key = RARRAY_PTR(RARRAY_PTR(def)[i])[0];
            if (SYM2ID(key) == id) {
                return RARRAY_PTR(def)[i];
            }
        }
    } else if (rb_obj_is_kind_of(idx,rb_cNumeric)) {
        i = NUM2LONG(idx);
        if (i<-len || i>=len) {
            rb_raise(rb_eIndexError,"offset %"SZF"u out of range of struct(size:%ld)", i, len);
        }
        return RARRAY_PTR(def)[i];
    }
    return Qnil;
}



void na_copy_array_structure(VALUE self, VALUE view);


VALUE
nst_field_view(VALUE self, VALUE idx)
{
    VALUE def, type, ofs;

    def = nst_definition(self, idx);
    if (!RTEST(def)) {
        rb_raise(rb_eTypeError, "Invalid field: '%s' for struct %s",
                 StringValuePtr(idx), rb_class2name(CLASS_OF(self)));
    }
    type = RARRAY_PTR(def)[1];
    ofs  = RARRAY_PTR(def)[2];
    return na_make_view_struct(self, type, ofs);
}

VALUE
nst_field(VALUE self, VALUE idx)
{
    VALUE obj;
    narray_view_t *nv;

    obj = nst_field_view(self,idx);
    GetNArrayView(obj,nv);
    if (nv->base.ndim==0) {
        obj = rb_funcall(obj,rb_intern("extract"),0);
    }
    return obj;
}

VALUE
nst_field_set(VALUE self, VALUE idx, VALUE other)
{
    VALUE obj;

    obj = nst_field_view(self,idx);
    rb_funcall(obj,rb_intern("store"),1,other);
    return other;
}


static VALUE
nst_method_missing(int argc, VALUE *argv, VALUE self)
{
    VALUE obj;

    if (argc!=1) {
        rb_raise(rb_eArgError,"wrong number of arguments (%d for 1)", argc);
    }
    obj = nst_field(self,argv[0]);
    if (RTEST(obj)) {
        return obj;
    }
    return rb_call_super(argc,argv);
}



/*
  Foo = NArray::Struct.new {
    int8     :byte
    float64  :float, [2,2]
    dcomplex :compl
  }
 */
static VALUE
nst_s_new(int argc, VALUE *argv, VALUE klass)
{
    VALUE name=Qnil, rest, size;
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
    //printf("size=%d\n",NUM2INT(size));
    rb_define_const(st, CONTIGUOUS_STRIDE, size);
    rb_define_const(st, ELEMENT_BYTE_SIZE, size);
    rb_define_const(st, ELEMENT_BIT_SIZE,  rb_funcall(size,'*',1,INT2FIX(8)));

    OBJ_FREEZE(members);
    rb_define_const(st, "DEFINITIONS", members);

    rb_define_singleton_method(st, "new", rb_class_new_instance, -1);
    rb_define_singleton_method(st, "[]", rb_class_new_instance, -1);

    return st;
}


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
            if (ndim > NA_MAX_DIMENSION) {
                rb_raise(rb_eArgError,"too large number of dimensions");
            }
            if (ndim > 0) {
                shape = ALLOCA_N(size_t, ndim);
                na_array_to_internal_shape(Qnil, argv[i], shape);
            }
            break;
        }
    }

    id = rb_to_id(name);
    name = ID2SYM(id);
    if (rb_obj_is_kind_of(type,cNArray)) {
        narray_t *na;
        GetNArray(type,na);
        type = rb_narray_view_new(CLASS_OF(type),na->ndim,na->shape);
    } else {
        type = rb_narray_view_new(type,ndim,shape);
    }
    size = rb_funcall(type, rb_intern("byte_size"), 0);
    ofs  = rb_iv_get(nst, "__offset__");
    rb_iv_set(nst, "__offset__", rb_funcall(ofs,'+',1,size));
    rb_ary_push(rb_iv_get(nst,"__members__"),
                rb_ary_new3(4,name,type,ofs,size));  // <- field definition
    return Qnil;
}




static VALUE
nst_extract(VALUE self)
{
    return self;
}



/*
static VALUE
nst_object_to_records(VALUE val, VALUE nst_class)
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
*/



static VALUE
nst_record_to_a(VALUE types, VALUE ofsts, size_t pos)
{
    long i, len;
    VALUE elmt, velm, vary;
    size_t ofs;
    narray_view_t *ne;

    len = RARRAY_LEN(types);
    vary = rb_ary_new2(len);

    for (i=0; i<len; i++) {
        ofs  = NUM2SIZE(RARRAY_PTR(ofsts)[i]);
        elmt = RARRAY_PTR(types)[i];
        GetNArrayView(elmt,ne);
        ne->offset = pos + ofs;
        if (ne->base.ndim==0) {
            velm = rb_funcall(elmt,rb_intern("extract"),0);
        } else {
            velm = rb_funcall(elmt,rb_intern("to_a"),0);
        }
        rb_ary_push(vary, velm);
    }
    return vary;
}

static void
iter_nstruct_to_a(na_loop_t *const lp)
{
    VALUE   obj;
    VALUE   opt, types, ofsts;

    opt = lp->option;
    types = RARRAY_PTR(opt)[0];
    ofsts = RARRAY_PTR(opt)[1];

    obj = nst_record_to_a(types,ofsts,lp->iter[0].pos);
    rb_ary_push(lp->args[1].value,obj);
}


static VALUE
nary_nstruct_to_a(VALUE self)
{
    volatile VALUE opt;
    VALUE defs, def, types, type, ofsts, ofst, elmt;
    long  i, len;
    narray_view_t *ne;
    ndfunc_arg_in_t ain[3] = {{Qnil,0},{sym_loop_opt},{sym_option}};
    ndfunc_arg_out_t aout[1] = {{rb_cArray,0}}; // dummy?
    ndfunc_t ndf = { iter_nstruct_to_a, NO_LOOP, 3, 1, ain, aout };

    defs = nst_definitions(self);
    len = RARRAY_LEN(defs);
    types = rb_ary_new2(len);
    ofsts = rb_ary_new2(len);
    for (i=0; i<len; i++) {
        def  = RARRAY_PTR(defs)[i];
        type = RARRAY_PTR(def)[1];
        ofst = RARRAY_PTR(def)[2];
        elmt = na_make_view(type);
        rb_ary_push(types, elmt);
        rb_ary_push(ofsts, ofst);
        GetNArrayView(elmt,ne);
        ne->data = na_original_data(self);
    }
    opt = rb_assoc_new(types,ofsts);
    return na_ndloop_cast_narray_to_rarray(&ndf, self, opt);
}

static inline int
na_is_array(VALUE item) {
    narray_t *na;

    if (TYPE(item) == T_ARRAY) {
        return 1;
    }
    if (RTEST(rb_obj_is_kind_of(item, cNArray))) {
        GetNArray(item,na);
        if (na->ndim>0) {
            return 1;
        } else {
            return 0;
        }
    }
    return 1;
}

static VALUE
nst_array_match_struct(VALUE self, VALUE ary)
{
    VALUE defs, def, type, item;
    long len, i;
    narray_view_t *nt;

    if (TYPE(ary) != T_ARRAY) {
        return Qfalse;
    }
    defs = nst_definitions(self);
    len = RARRAY_LEN(defs);

    if (len != RARRAY_LEN(ary)) {
        return Qfalse;
    }
    for (i=0; i<len; i++) {
        def  = RARRAY_PTR(defs)[i];
        type = RARRAY_PTR(def)[1];
        GetNArrayView(type,nt);
        item = RARRAY_PTR(ary)[i];
        if (nt->base.ndim==0) {
            if (na_is_array(item)) {
                return Qfalse;
            }
        } else {
            if (!na_is_array(item)) {
                return Qfalse;
            }
        }
    }
}

/*
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
*/


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
Init_nary_struct()
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
    rb_define_method(cStruct, "field_set", nst_field_set, 2);
    rb_define_method(cStruct, "extract", nst_extract, 0);
    rb_define_method(cStruct, "method_missing", nst_method_missing, -1);

    //rb_define_method(cStruct, "fill", nary_nstruct_fill, 1);

    //rb_define_method(cStruct, "debug_print", nary_nstruct_debug_print, 0);

    rb_define_method(cStruct, "to_a", nary_nstruct_to_a, 0);

    //rb_define_method(cStruct, "initialize", rb_struct_initialize, -2);
    //rb_define_method(cStruct, "initialize_copy", rb_struct_init_copy, 1);
}
