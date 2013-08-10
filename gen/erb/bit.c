/*
  bit.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <ruby.h>
#include "narray.h"
#include "template.h"

<%
require "./cogen_lib"
$embed = true
class_name "Bit"
%>

#define cT <%=type_var%>

typedef int dtype;

#define m_load_data(ptr,pos) load_data(ptr,pos)
#define m_sprintf(s,x)   sprintf(s,"%1d",(int)(x))

#define m_copy(x)  (x)
#define m_not(x)   (~(x))
#define m_and(x,y) ((x)&(y))
#define m_or(x,y)  ((x)|(y))
#define m_xor(x,y) ((x)^(y))
#define m_eq(x,y)  (~((x)^(y)))
#define m_count_true(x)  (x!=0)
#define m_count_false(x) (x==0)

static inline dtype load_data(void *ptr, size_t pos) {
    return (((BIT_DIGIT*)(ptr))[(pos)/NB]>>((pos)%NB)) & 1u;
}

VALUE <%=type_var%>;


static VALUE nary_cast_array_to_bit(VALUE ary);


static VALUE
nary_bit_cast_numeric(VALUE val)
{
    narray_t *na;
    VALUE v;
    BIT_DIGIT *ptr, b=2;
    size_t dig_ofs;
    int    bit_ofs;

    if (FIXNUM_P(val)) {
        b = FIX2INT(val);
    } else if (val==Qtrue) {
        b = 1;
    } else if (val==Qfalse) {
        b = 0;
    }
    if (b!=0 && b!=1) {
        rb_raise(rb_eArgError, "bit can be cast from 0 or 1 or true or false");
    }

    v = rb_narray_new(cT, 0, NULL);
    GetNArray(v,na);
    //dig_ofs = na->offset / NB;
    //bit_ofs = na->offset % NB;
    dig_ofs = 0;
    bit_ofs = 0;
    ptr = (BIT_DIGIT*)na_get_pointer_for_write(v) + dig_ofs;
    *ptr = (*ptr & ~(1u<<bit_ofs)) | (b<<bit_ofs);
    na_release_lock(v);
    return v;
}


static VALUE
nary_bit_s_cast(VALUE type, VALUE obj)
{
    VALUE r;

    if (CLASS_OF(obj)==cT) {
        return obj;
    } else if (TYPE(obj)==T_ARRAY) {
        //shape = rb_funcall(cNArray,rb_intern("array_shape"),1,obj);
        return nary_cast_array_to_bit(obj);
    } else if (TYPE(obj)==T_FLOAT || FIXNUM_P(obj) || TYPE(obj)==T_BIGNUM) {
        //printf("cast float\n");
        return nary_bit_cast_numeric(obj);
    }

    if (IsNArray(obj)) {
        r = rb_funcall(obj, rb_intern("coerce_cast"), 1, cT);
        if (RTEST(r)) {
            return r;
        }
    }

    rb_raise(nary_eCastError, "unknown conversion from %s to %s",
             rb_class2name(CLASS_OF(obj)),
             rb_class2name(type));
    return Qnil;
}

static VALUE
nary_bit_coerce_cast(VALUE value, VALUE type)
{
    return Qnil;
}

//----------------------------------------------------------------------

void iter_bit_print(char *ptr, size_t pos, VALUE opt)
{
    int x;
    LOAD_BIT(ptr,pos,x);
    //printf("%d", (((BIT_DIGIT*)ptr)[pos/NB] >> (pos%NB)) & 1u);
    printf("%d", x);
}


static VALUE
format_bit(VALUE fmt, int x)
{
    if (NIL_P(fmt)) {
        char s[4];
        int n;
        n = sprintf(s,"%1d",x);
        return rb_str_new(s,n);
    }
    return rb_funcall(fmt, '%', 1, INT2FIX(x));
}

static VALUE
 bit_inspect_element(char *ptr, size_t pos, VALUE fmt)
{
    int x;
    LOAD_BIT(ptr,pos,x);
    return format_bit(fmt, x);
}
 VALUE
 nary_bit_inspect(VALUE ary)
{
    VALUE str = na_info_str(ary);
    na_ndloop_inspect(ary, str, bit_inspect_element, Qnil);
    return str;
}


static void
iter_bit_format(na_loop_t *const lp)
{
    size_t  i;
    BIT_DIGIT *a1, x=0;
    size_t     p1;
    char      *p2;
    ssize_t    s1, s2;
    size_t    *idx1;
    VALUE y;
    VALUE fmt = *(VALUE*)(lp->opt_ptr);

    INIT_COUNTER(lp, i);
    INIT_PTR_BIT(lp, 0, a1, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2);

    if (idx1) {
        for (; i--;) {
            LOAD_BIT(a1, p1 + *idx1, x); idx1++;
            y = format_<%=tp%>(fmt, x);
            SET_DATA_STRIDE(p2, s2, VALUE, y);
        }
    } else {
        for (; i--;) {
            LOAD_BIT(a1, p1, x);         p1+=s1;
            y = format_<%=tp%>(fmt, x);
            SET_DATA_STRIDE(p2, s2, VALUE, y);
        }
    }
}

static VALUE
nary_bit_format(int argc, VALUE *argv, VALUE self)
{
    VALUE fmt=Qnil;
    ndfunc_arg_in_t ain[2] = {{Qnil,0},{sym_option}};
    ndfunc_arg_out_t aout[1] = {{cRObject,0}};
    ndfunc_t ndf = { iter_bit_format, FULL_LOOP, 2, 1, ain, aout };

    rb_scan_args(argc, argv, "01", &fmt);
    return na_ndloop(&ndf, 2, self, fmt);
}

static void
iter_bit_format_to_a(na_loop_t *const lp)
{
    size_t  i;
    BIT_DIGIT *a1, x=0;
    size_t     p1;
    //char      *p2;
    ssize_t    s1; //, s2;
    size_t   *idx1; //, *idx2;
    VALUE y;
    VALUE fmt = *(VALUE*)(lp->opt_ptr);
    volatile VALUE a;

    INIT_COUNTER(lp, i);
    INIT_PTR_BIT(lp, 0, a1, p1, s1, idx1);
    a = rb_ary_new2(i);
    rb_ary_push(lp->args[1].value, a);
    //INIT_PTR(lp, 1, p2, s2, idx2);
    for (; i--;) {
        LOAD_BIT_STEP(a1, p1, s1, idx1, x);
        y = format_bit(fmt, x);
        rb_ary_push(a,y);
    }
}
static VALUE
nary_bit_format_to_a(int argc, VALUE *argv, VALUE self)
{
    volatile VALUE fmt=Qnil;
    ndfunc_arg_in_t ain[3] = {{Qnil,0},{sym_loop_opt},{sym_option}};
    ndfunc_arg_out_t aout[1] = {{rb_cArray,0}}; // dummy?
    ndfunc_t ndf = { iter_bit_format_to_a, FULL_LOOP, 3, 1, ain, aout };

    rb_scan_args(argc, argv, "01", &fmt);
    return na_ndloop_cast_narray_to_rarray(&ndf, self, fmt);
}




static void
iter_bit_fill(na_loop_t *const lp)
{
    size_t  n;
    size_t  p3;
    ssize_t s3;
    size_t *idx3;
    int     len;
    BIT_DIGIT *a3;
    BIT_DIGIT  y;
    VALUE x = *(VALUE*)(lp->opt_ptr);

    if (x==INT2FIX(0) || x==Qfalse) {
        y = 0;
    } else
    if (x==INT2FIX(1) || x==Qtrue) {
        y = ~(BIT_DIGIT)0;
    } else {
        rb_raise(rb_eArgError, "invalid value for Bit");
    }

    INIT_COUNTER(lp, n);
    INIT_PTR_BIT(lp, 0, a3, p3, s3, idx3);
    if (s3!=1 || idx3) {
        y = y&1;
        for (; n--;) {
            STORE_BIT_STEP(a3, p3, s3, idx3, y);
        }
    } else {
        if (p3>0 || n<NB) {
            len = NB - p3;
            if ((int)n<len) len=n;
            *a3 = (y & (SLB(len)<<p3)) | (*a3 & ~(SLB(len)<<p3));
            a3++;
            n -= len;
        }
        for (; n>=NB; n-=NB) {
            *(a3++) = y;
        }
        if (n>0) {
            *a3 = (y & SLB(n)) | (*a3 & BALL<<n);
        }
    }
}


static VALUE
nary_bit_fill(VALUE self, VALUE val)
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{sym_option}};
    ndfunc_t ndf = { iter_bit_fill, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, val);
    return self;
}


void
bit_cast_to_robj(na_loop_t *const lp)
{
    size_t     i;
    ssize_t    s1; //, s2;
    size_t    *idx1; //, *idx2;
    BIT_DIGIT *a1;
    size_t     p1;
    //char      *p2;
    BIT_DIGIT  x=0;
    VALUE      y;
    volatile VALUE a;

    INIT_COUNTER(lp, i);
    INIT_PTR_BIT(lp, 0, a1, p1, s1, idx1);
    //INIT_PTR    (lp, 1,     p2, s2, idx2);
    a = rb_ary_new2(i);
    rb_ary_push(lp->args[1].value, a);
    for (; i--;) {
        LOAD_BIT_STEP(a1, p1, s1, idx1, x);
        y = INT2FIX(x);
        rb_ary_push(a,y);
    }
}


static VALUE
nary_bit_cast_to_rarray(VALUE self)
{
    ndfunc_arg_in_t ain[3] = {{Qnil,0},{sym_loop_opt},{sym_option}};
    ndfunc_arg_out_t aout[1] = {{rb_cArray,0}}; // dummy?
    ndfunc_t ndf = { bit_cast_to_robj, FULL_LOOP, 3, 1, ain, aout };
    return na_ndloop_cast_narray_to_rarray(&ndf, self, Qnil);
//
//    VALUE v;
//    ndfunc_t *func;
//
//    func = ndfunc_alloc(bit_cast_to_robj, FULL_LOOP,
//                        1, 1, cT, rb_cArray);
//    v = ndloop_cast_narray_to_rarray(func, self, Qnil);
//    ndfunc_free(func);
//    return v;
}


static void
iter_cast_rarray_to_bit(na_loop_t *const lp)
{
    size_t i, n, n1;
    VALUE  v1, *ptr;
    BIT_DIGIT *a2;
    size_t p2;
    size_t s2, *idx2;
    VALUE  x;
    BIT_DIGIT y;

    INIT_COUNTER(lp, n);
    v1 = lp->args[0].value;
    INIT_PTR_BIT(lp, 1, a2, p2, s2, idx2);

    switch(TYPE(v1)) {
    case T_ARRAY:
        n1 = RARRAY_LEN(v1);
        ptr = RARRAY_PTR(v1);
        break;
    case T_NIL:
        n1 = 0;
        break;
    default:
        n1 = 1;
        ptr = &v1;
    }
    for (i=0; i<n1 && i<n; i++) {
        x = ptr[i];
        y = 2;
        if (FIXNUM_P(x)) {
            y = FIX2INT(x);
        } else if (x==Qtrue) {
            y = 1;
        } else if (x==Qfalse) {
            y = 0;
        } else if (x==Qnil) {
            y = 0;
        }
        if (y!=0 && y!=1) {
            rb_raise(rb_eArgError, "bit can be cast from 0 or 1 or true or false");
        }
        STORE_BIT_STEP(a2, p2, s2, idx2, y);
    }
    y = 0;
    for (; i<n; i++) {
        STORE_BIT_STEP(a2, p2, s2, idx2, y);
    }
}



static VALUE
nary_cast_array_to_bit(VALUE rary)
{
    int nd;
    size_t *shape;
    VALUE tp, nary;
    ndfunc_arg_in_t ain[2] = {{Qnil,0},{rb_cArray,0}};
    ndfunc_t ndf = { iter_cast_rarray_to_bit, FULL_LOOP, 2, 0, ain, 0 };

    shape = na_mdarray_investigate(rary, &nd, &tp);
    nary = rb_narray_new(cT, nd, shape);
    na_alloc_data(nary);
    xfree(shape);
    //func = ndfunc_alloc(iter_cast_rarray_to_bit, FULL_LOOP,
    //                    2, 0, Qnil, rb_cArray);
    //ndloop_cast_rarray_to_narray(func, rary, nary);
    na_ndloop_cast_rarray_to_narray(&ndf, rary, nary);
    //ndfunc_free(func);
    return nary;
}



static VALUE
nary_bit_extract(VALUE self)
{
    BIT_DIGIT *ptr, val;
    size_t pos;
    narray_t *na;
    GetNArray(self,na);

    if (na->ndim==0) {
        pos = na_get_offset(self);
        ptr = (BIT_DIGIT*)na_get_pointer_for_read(self);
        //pos = na->offset;
        val = ((*((ptr)+(pos)/NB)) >> ((pos)%NB)) & 1u;
        na_release_lock(self);
        return INT2FIX(val);
    }
    return self;
}

<%=bit_binary "and" %>
<%=bit_binary "or" %>
<%=bit_binary "xor" %>
<%=bit_binary "eq" %>
<%=bit_unary  "not" %>
<%=bit_unary  "copy" %>
<%=bit_count  "count_true" %>
<%=bit_count  "count_false" %>

VALUE
nary_bit_store(VALUE dst, VALUE src)
{
    // check and fix me
    ndfunc_arg_in_t ain[2] = {{INT2FIX(1),0},{Qnil,0}};
    ndfunc_t ndf = { iter_bit_copy, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, src, dst);
    return src;
}

VALUE na_aref(int argc, VALUE *argv, VALUE self);

/* method: []=(idx1,idx2,...,idxN,val) */
static VALUE
nary_bit_aset(int argc, VALUE *argv, VALUE self)
{
    VALUE a;
    argc--;

    if (argc==0)
        nary_bit_store(self, argv[argc]);
    else {
        a = na_aref(argc, argv, self);
        nary_bit_store(a, argv[argc]);
    }
    return argv[argc];
}


typedef struct {
    size_t count;
    char  *idx0;
    char  *idx1;
    size_t elmsz;
} where_opt_t;

#define STORE_INT(ptr, esz, x) memcpy(ptr,&(x),esz)

static void
iter_bit_where(na_loop_t *const lp)
{
    size_t  i;
    BIT_DIGIT *a;
    size_t  p;
    ssize_t s;
    size_t *idx;
    BIT_DIGIT x=0;
    char   *idx0, *idx1;
    size_t  count;
    size_t  e;
    where_opt_t *g;

    g = (where_opt_t*)(lp->opt_ptr);
    count = g->count;
    idx0  = g->idx0;
    idx1  = g->idx1;
    e     = g->elmsz;
    INIT_COUNTER(lp, i);
    INIT_PTR_BIT(lp, 0, a, p, s, idx);
    if (idx) {
        for (; i--;) {
            LOAD_BIT(a, p+*idx, x);
            idx++;
            if (x==0) {
                if (idx0) {
                    STORE_INT(idx0,e,count);
                    idx0 += e;
                }
            } else {
                if (idx1) {
                    STORE_INT(idx1,e,count);
                    idx1 += e;
                }
            }
            count++;
        }
    } else {
        for (; i--;) {
            LOAD_BIT(a, p, x);
            p+=s;
            if (x==0) {
                if (idx0) {
                    STORE_INT(idx0,e,count);
                    idx0 += e;
                }
            } else {
                if (idx1) {
                    STORE_INT(idx1,e,count);
                    idx1 += e;
                }
            }
            count++;
        }
    }
    g->count = count;
    g->idx0  = idx0;
    g->idx1  = idx1;
}


static VALUE
 nary_bit_where(VALUE self)
{
    volatile VALUE idx_1;
    size_t size, n_1;
    where_opt_t *g;

    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_t ndf = { iter_bit_where, FULL_LOOP, 1, 0, ain, 0 };
    //func = ndfunc_alloc(iter_bit_where, FULL_LOOP, 1, 0, cT);

    //self = na_flatten(self);
    size = RNARRAY_SIZE(self);
    n_1 = NUM2SIZE(nary_bit_count_true(0, NULL, self));
    g = ALLOCA_N(where_opt_t,1);
    g->count = 0;
    if (size>4294967295ul) {
        idx_1 = rb_narray_new(cInt64, 1, &n_1);
        g->elmsz = 8;
    } else {
        idx_1 = rb_narray_new(cInt32, 1, &n_1);
        g->elmsz = 4;
    }
    g->idx1 = na_get_pointer_for_write(idx_1);
    g->idx0 = NULL;
    //ndloop_do3(func, g, 1, self);
    na_ndloop3(&ndf, g, 1, self);
    na_release_lock(idx_1);
    return idx_1;
}



static VALUE
 nary_bit_where2(VALUE self)
{
    VALUE idx_1, idx_0;
    size_t size, n_1;
    where_opt_t *g;

    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_t ndf = { iter_bit_where, FULL_LOOP, 1, 0, ain, 0 };
    //func = ndfunc_alloc(iter_bit_where, FULL_LOOP, 1, 0, cT);

    size = RNARRAY_SIZE(self);
    n_1 = NUM2SIZE(nary_bit_count_true(0, NULL, self));
    g = ALLOCA_N(where_opt_t,1);
    g->count = 0;
    if (size>4294967295ul) {
        idx_1 = rb_narray_new(cInt64, 1, &n_1);
        idx_0 = rb_narray_new(cInt64, 1, &n_1);
        g->elmsz = 8;
    } else {
        idx_1 = rb_narray_new(cInt32, 1, &n_1);
        idx_0 = rb_narray_new(cInt32, 1, &n_1);
        g->elmsz = 4;
    }
    g->idx1 = na_get_pointer_for_write(idx_1);
    g->idx0 = na_get_pointer_for_write(idx_0);
    //ndloop_do3(func, g, 1, self);
    na_ndloop3(&ndf, g, 1, self);
    na_release_lock(idx_0);
    na_release_lock(idx_1);
    return rb_assoc_new(idx_1,idx_0);
}



static void
iter_bit_mask(na_loop_t *const lp)
{
    size_t  i;
    BIT_DIGIT *a1;
    size_t  p1, q1;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    BIT_DIGIT x=0;
    char    *p2, *q2, *q3;
    size_t  e2;
    //void  **g;
    //VALUE info = lp->info;

    //Data_Get_Struct(info, void*, g);
    //q3 = *g;
    q3 = *(void**)(lp->opt_ptr);
    INIT_COUNTER(lp, i);
    INIT_PTR_BIT(lp, 0, a1, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    INIT_ELMSIZE(lp, 1, e2)
    for (; i--;) {
      if (idx1) {
        q1 = p1+*idx1;
        idx1++;
      } else {
        q1 = p1;
        p1 += s1;
      }
      if (idx2) {
        q2 = p2+*idx2;
        idx2++;
      } else {
        q2 = p2;
        p2 += s2;
      }
      LOAD_BIT(a1, q1, x);
      if (x!=0) {
        memcpy(q3,q2,e2);
        q3 += e2;
      }
    }
    //*g = q3;
    *(void**)(lp->opt_ptr) = q3;
}


static VALUE
 nary_bit_mask(VALUE mask, VALUE val)
{
    VALUE result;
    size_t size;
    void **g;
    ndfunc_arg_in_t ain[2] = {{cT,0},{Qnil,0}};
    ndfunc_t ndf = { iter_bit_mask, FULL_LOOP, 2, 0, ain, 0 };

    size = NUM2SIZE(nary_bit_count_true(0, NULL, mask));
    result = rb_narray_new(CLASS_OF(val), 1, &size);
    g = ALLOC(void *);
    *g = na_get_pointer_for_write(result);
    //opt = Data_Wrap_Struct(rb_cData,0,0,g);
    //func = ndfunc_alloc(iter_bit_mask, FULL_LOOP, 2, 0, cT, Qnil);
    //ndloop_do3(func, g, 2, mask, val);
    na_ndloop3(&ndf, g, 2, mask, val);
    na_release_lock(result);
    return result;
}

VALUE
nary_bit_all_p(VALUE self)
{
    return (rb_funcall(self, rb_intern("count_false"), 0)==INT2FIX(0)) ? Qtrue : Qfalse;
}

VALUE
nary_bit_any_p(VALUE self)
{
    return (rb_funcall(self, rb_intern("count_true"), 0)!=INT2FIX(0)) ? Qtrue : Qfalse;
}

VALUE
nary_bit_none_p(VALUE self)
{
    return (rb_funcall(self, rb_intern("count_true"), 0)==INT2FIX(0)) ? Qtrue : Qfalse;
}


void
Init_nary_bit()
{
    volatile VALUE hCast;

    cT = rb_define_class_under(cNArray, "Bit", cNArray);

    rb_define_const(cT, "ELEMENT_BIT_SIZE",  INT2FIX(1));
    rb_define_const(cT, "ELEMENT_BYTE_SIZE", rb_float_new(1.0/8));
    rb_define_const(cT, "CONTIGUOUS_STRIDE", INT2FIX(1));

    //rb_define_singleton_method(cNArray, "Bit", nary_bit_s_cast, 1);
    rb_define_singleton_method(cT, "cast", nary_bit_s_cast, 1);
    rb_define_singleton_method(cT, "[]", nary_bit_s_cast, -2);
    rb_define_method(cT, "coerce_cast", nary_bit_coerce_cast, 1);

    <% Template::INIT.each do |x| %>
    <%=x%><% end %>

    rb_define_alias (cT, "count_1","count_true");
    rb_define_alias (cT, "count_0","count_false");
    rb_define_method(cT, "where", nary_bit_where, 0);
    rb_define_method(cT, "where2", nary_bit_where2, 0);
    rb_define_method(cT, "mask", nary_bit_mask, 1);

    rb_define_method(cT, "all?", nary_bit_all_p, 0);
    rb_define_method(cT, "any?", nary_bit_any_p, 0);
    rb_define_method(cT, "none?", nary_bit_none_p, 0);

    rb_define_method(cT, "inspect", nary_bit_inspect, 0);
    rb_define_method(cT, "format", nary_bit_format, -1);
    rb_define_method(cT, "format_to_a", nary_bit_format_to_a, -1);

    rb_define_method(cT, "fill", nary_bit_fill, 1);

    rb_define_method(cT, "to_a", nary_bit_cast_to_rarray, 0);

    rb_define_method(cT, "extract", nary_bit_extract, 0);

    rb_define_method(cT, "copy",  nary_bit_copy, 0);
    rb_define_method(cT, "store", nary_bit_store, 1);
    rb_define_method(cT, "[]=",   nary_bit_aset, -1);

    hCast = rb_hash_new();
    rb_define_const(cT, "UPCAST", hCast);
    rb_hash_aset(hCast, cInt32, cInt32);
    rb_hash_aset(hCast, cInt16, cInt16);
    rb_hash_aset(hCast, cInt8,  cInt8);
}
