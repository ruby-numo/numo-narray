/*
   kwarg.c : Process keyword arguments for Ruby

   Copyright (c) 2001 Masahiro TANAKA <masa@ir.isas.ac.jp>

   This program is free software.
   You can distribute/modify this program
   under the same terms as Ruby itself.
   NO WARRANTY.
*/
#include <ruby.h>
#include "compat.h"

/* void rb_scan_kw_args __((VALUE, ...)); */

static VALUE
kw_hash_i(i, tmp)
     VALUE i, tmp;
{
    VALUE key;

    key = RARRAY_PTR(i)[0];
    if (TYPE(key)==T_SYMBOL) {
	key = rb_funcall(key, rb_intern("id2name"), 0);
    } else
	if (TYPE(key)!=T_STRING) {
	    rb_raise(rb_eArgError, "keywords must be String or Symbol");
	}

    rb_hash_aset(tmp, key, RARRAY_PTR(i)[1]);
    return Qnil;
}

#ifdef HAVE_STDARG_PROTOTYPES
#include <stdarg.h>
#define va_init_list(a,b) va_start(a,b)
#else
#include <varargs.h>
#define va_init_list(a,b) va_start(a)
#endif

void
#ifdef HAVE_STDARG_PROTOTYPES
rb_scan_kw_args(VALUE hash, ...)
#else
rb_scan_kw_args(hash, va_alist)
    VALUE hash;
    va_dcl
#endif
{
    va_list vargs;
    va_init_list(vargs, hash);

    char *key;
    VALUE *var, val, str, tmp;

    tmp = rb_hash_new();
    if (TYPE(hash) == T_HASH)
	rb_iterate(rb_each, hash, kw_hash_i, tmp);
    else if (hash != Qnil)
	rb_fatal("rb_san_kw_args: non-hash arg passed");
    
    for (;;) {
	key = va_arg(vargs, char*);
	if (!key) break;
	var = va_arg(vargs, VALUE*);
	//printf("i=%d key=%x, val=%x\n",i,key,val);
	str = rb_str_new2(key);
	val = rb_funcall(tmp, rb_intern("delete"), 1, str);
	if (var) *var = val;
    }
    va_end(vargs);

    if (rb_funcall(tmp, rb_intern("empty?"), 0)==Qfalse) {
	val = rb_funcall(tmp, rb_intern("keys"), 0);
	val = rb_funcall(val, rb_intern("join"), 1, rb_str_new2(","));
	rb_raise(rb_eArgError, "unknown keywords: %s",StringValueCStr(val));
    }
}
