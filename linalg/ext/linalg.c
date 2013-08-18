#include "ruby.h"
#include "narray.h"

extern VALUE Init_nary_dfloat_linalg();
extern VALUE Init_nary_sfloat_linalg();
extern VALUE Init_nary_dcomplex_linalg();
extern VALUE Init_nary_scomplex_linalg();

/*
  Dispatches method to NArray::Linalg module of upcasted type,
  eg, NArray::DFloat::Linalg.
  @overload method_missing(name,x,...)
  @param [Symbol] name  method name.
  @param [NArray,Numeric] x  input array.
  @return [NArray] result.
*/
VALUE nary_linalg_method_missing(int argc, VALUE *argv, VALUE mlinalg)
{
    VALUE type, mod, hash;
    if (argc>1) {
	type = nary_mathcast(argc-1,argv+1);
	hash = rb_const_get(mlinalg, rb_intern("DISPATCH"));
	mod = rb_hash_aref(hash, type);
	if (NIL_P(mod)) {
	    rb_raise(rb_eTypeError,"%s is unknown for NArray::Math",
		     rb_class2name(type));
	}
	return rb_funcall2(mod,rb_intern("send"),argc,argv);
    }
    rb_raise(rb_eArgError,"argument or method missing");
    return Qnil;
}

void
Init_linalg()
{
    VALUE hCast, mLinalg, mod;

    mLinalg = rb_define_module_under(cNArray, "Linalg");
    rb_define_singleton_method(mLinalg, "method_missing",
                               nary_linalg_method_missing, -1);

    hCast = rb_hash_new();
    rb_define_const(mLinalg, "DISPATCH", hCast);

    rb_hash_aset(hCast, cDFloat, Init_nary_dfloat_linalg());
    rb_hash_aset(hCast, cSFloat, Init_nary_sfloat_linalg());
    rb_hash_aset(hCast, cDComplex, Init_nary_dcomplex_linalg());
    rb_hash_aset(hCast, cSComplex, Init_nary_scomplex_linalg());
}
