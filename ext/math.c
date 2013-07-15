/*
  math.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <ruby.h>
#include "narray.h"

VALUE mNMath;
EXTERN VALUE mDFloatMath, mDComplexMath;
static ID id_send;

VALUE
nary_type_s_upcast(VALUE type1, VALUE type2)
{
    VALUE upcast_hash;
    VALUE result_type;

    if (type1==type2) return type1;
    upcast_hash = rb_const_get(type1, rb_intern("UPCAST"));
    result_type = rb_hash_aref(upcast_hash, type2);
    if (NIL_P(result_type)) {
        if (TYPE(type2)==T_CLASS) {
            if ( RTEST(rb_class_inherited_p(type2,cNArray)) ) {
                upcast_hash = rb_const_get(type2, rb_intern("UPCAST"));
                result_type = rb_hash_aref(upcast_hash, type1);
            }
        }
    }
    return result_type;
}


VALUE nary_math_cast2(VALUE type1, VALUE type2)
{
    if ( RTEST(rb_class_inherited_p( type1, cNArray )) ){
	return nary_type_s_upcast( type1, type2 );
    }
    if ( RTEST(rb_class_inherited_p( type2, cNArray )) ){
	return nary_type_s_upcast( type2, type1 );
    }
    if ( RTEST(rb_class_inherited_p( type1, rb_cNumeric )) &&
	 RTEST(rb_class_inherited_p( type2, rb_cNumeric )) ){
	if ( RTEST(rb_class_inherited_p( type1, rb_cComplex)) ||
	     RTEST(rb_class_inherited_p( type2, rb_cComplex )) ){
	    return rb_cComplex;
	}
	return rb_cFloat;
    }
    return type2;
}


VALUE na_array_type(VALUE);

VALUE nary_mathcast(int argc, VALUE *argv)
{
    VALUE type;
    int i;
    type = rb_cFixnum;
    for (i=0; i<argc; i++) {
	type = nary_math_cast2( type, na_array_type(argv[i]) );
	if (NIL_P(type)) {
	    rb_raise(rb_eTypeError,"%s is unknown for NArray::Math",
		     rb_class2name(argv[i]));
	}
    }
    return type;
}


/*
  Dispatches method to Math module of upcasted type,
  eg, NArray::DFloat::Math.
  @overload method_missing(name,x,...)
  @param [Symbol] name  method name.
  @param [NArray,Numeric] x  input array.
  @return [NArray] result.
*/
VALUE nary_math_method_missing(int argc, VALUE *argv, VALUE mod)
{
    VALUE type, ans, typemod, hash;
    if (argc>1) {
	type = nary_mathcast(argc-1,argv+1);

	hash = rb_const_get(mod, rb_intern("DISPATCH"));
	typemod = rb_hash_aref( hash, type );
	if (NIL_P(typemod)) {
	    rb_raise(rb_eTypeError,"%s is unknown for NArray::Math",
		     rb_class2name(type));
	}

	ans = rb_funcall2(typemod,id_send,argc,argv);

	if (!RTEST(rb_class_inherited_p(type,cNArray)) &&
	    IsNArray(ans) ) {
	    ans = rb_funcall(ans,rb_intern("extract"),0);
	}
	return ans;
    }
    rb_raise(rb_eArgError,"argument or method missing");
    return Qnil;
}


void
Init_nary_math()
{
    VALUE hCast;

    mNMath = rb_define_module_under(cNArray, "Math");
    rb_define_singleton_method(mNMath, "method_missing", nary_math_method_missing, -1);

    //mDFloatMath = rb_define_module_under(mNMath, "DFloat");
    //mDComplexMath = rb_define_module_under(mNMath, "DComplex");

    hCast = rb_hash_new();
    rb_define_const(mNMath, "DISPATCH", hCast);
    rb_hash_aset(hCast, cInt64,      mDFloatMath);
    rb_hash_aset(hCast, cInt32,      mDFloatMath);
    rb_hash_aset(hCast, cInt16,      mDFloatMath);
    rb_hash_aset(hCast, cInt8,       mDFloatMath);
    rb_hash_aset(hCast, cUInt64,     mDFloatMath);
    rb_hash_aset(hCast, cUInt32,     mDFloatMath);
    rb_hash_aset(hCast, cUInt16,     mDFloatMath);
    rb_hash_aset(hCast, cUInt8,      mDFloatMath);
    rb_hash_aset(hCast, cDFloat,     mDFloatMath);
    rb_hash_aset(hCast, cDFloat,     mDFloatMath);
    rb_hash_aset(hCast, cDComplex,   mDComplexMath);
    rb_hash_aset(hCast, rb_cFixnum,  rb_mMath);
    rb_hash_aset(hCast, rb_cBignum,  rb_mMath);
    rb_hash_aset(hCast, rb_cInteger, rb_mMath);
    rb_hash_aset(hCast, rb_cFloat,   rb_mMath);
    rb_hash_aset(hCast, rb_cComplex, mDComplexMath);

    id_send = rb_intern("send");
}
