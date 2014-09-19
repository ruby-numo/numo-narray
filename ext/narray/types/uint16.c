#line 1 "dtype.erb.c"
/*
  uint16.c
#line 3 "dtype.erb.c"
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011,2013 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

#include <ruby.h>
#include <math.h>
#include "narray.h"
#include "SFMT.h"
#include "template.h"
#include "uint16.h"

VALUE cT;
#ifdef mTM
VALUE mTM;
#endif

#line 217 "dtype.erb.c"
static VALUE nary_uint16_store(VALUE,VALUE);

#line 1 "tmpl/allocate.c"
static VALUE
nary_uint16_allocate(VALUE self)
#line 3 "tmpl/allocate.c"
{
    narray_t *na;
    char *ptr;

    GetNArray(self,na);

    switch(NA_TYPE(na)) {
    case NARRAY_DATA_T:
        ptr = NA_DATA_PTR(na);
        if (na->size > 0 && ptr == NULL) {
            ptr = xmalloc(sizeof(dtype) * na->size);
            
#line 22 "tmpl/allocate.c"
            NA_DATA_PTR(na) = ptr;
        }
        break;
    case NARRAY_VIEW_T:
        rb_funcall(NA_VIEW_DATA(na), id_allocate, 0);
        break;
    case NARRAY_FILEMAP_T:
        //ptr = ((narray_filemap_t*)na)->ptr;
        // to be implemented
    default:
        rb_bug("invalid narray type : %d",NA_TYPE(na));
    }
    return self;
}



#line 1 "tmpl/extract.c"
/*
  Extract element value as Ruby Object if self is dimensionless NArray,
  otherwise returns self.
  @overload extract
  @return [Numeric,NArray]
*/
VALUE
nary_uint16_extract(VALUE self)
{
    volatile VALUE v;
    char *ptr;
    narray_t *na;
    GetNArray(self,na);

    if (na->ndim==0) {
        ptr = na_get_pointer_for_read(self) + na_get_offset(self);
        v = m_extract(ptr);
        na_release_lock(self);
        return v;
    }
    return self;
}



#line 1 "tmpl/store_numeric.c"
static VALUE
nary_uint16_new_dim0(dtype x)
{
    narray_t *na;
    VALUE v;
    dtype *ptr;

    v = rb_narray_new(cT, 0, NULL);
    GetNArray(v,na);
    ptr = (dtype*)(char*)na_get_pointer_for_write(v);
    *ptr = x;
    na_release_lock(v);
    return v;
}

static VALUE
nary_uint16_store_numeric(VALUE self, VALUE obj)
#line 18 "tmpl/store_numeric.c"
{
    dtype x;
    x = m_num_to_data(obj);
    obj = nary_uint16_new_dim0(x);
    nary_uint16_store(self,obj);
#line 23 "tmpl/store_numeric.c"
    return self;
}



#line 1 "tmpl/cast_array.c"
static void
iter_uint16_store_array(na_loop_t *const lp)
#line 3 "tmpl/cast_array.c"
{
    size_t i, n;
    size_t i1, n1;
    VALUE  v1, *ptr;
    char   *p2;
    size_t s2, *idx2;
    VALUE  x;
    double y;
    dtype  z;
    size_t len, c;
    double beg, step;

    v1 = lp->args[0].value;
    ptr = &v1;
    INIT_COUNTER(lp, n);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);

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
    }
    if (idx2) {
        
#line 34 "tmpl/cast_array.c"
        for (i=i1=0; i1<n1 && i<n; i++,i1++) {
            x = ptr[i1];
            if (rb_obj_is_kind_of(x, rb_cRange) || rb_obj_is_kind_of(x, na_cStep)) {
                nary_step_sequence(x,&len,&beg,&step);
                for (c=0; c<len && i<n; c++,i++) {
                    y = beg + step * c;
                    z = m_from_double(y);
                    SET_DATA_INDEX(p2, idx2, dtype, z)
#line 42 "tmpl/cast_array.c"
                }
            }
            else if (TYPE(x) != T_ARRAY) {
                if (x == Qnil) x = INT2FIX(0);
                z = m_num_to_data(x);
                SET_DATA_INDEX(p2, idx2, dtype, z)
#line 48 "tmpl/cast_array.c"
            }
        }
        z = m_zero;
        for (; i<n; i++) {
            SET_DATA_INDEX(p2, idx2, dtype, z)
#line 53 "tmpl/cast_array.c"
        }
        
#line 55 "tmpl/cast_array.c"
    } else {
        
#line 34 "tmpl/cast_array.c"
        for (i=i1=0; i1<n1 && i<n; i++,i1++) {
            x = ptr[i1];
            if (rb_obj_is_kind_of(x, rb_cRange) || rb_obj_is_kind_of(x, na_cStep)) {
                nary_step_sequence(x,&len,&beg,&step);
                for (c=0; c<len && i<n; c++,i++) {
                    y = beg + step * c;
                    z = m_from_double(y);
                    SET_DATA_STRIDE(p2, s2, dtype, z)
#line 42 "tmpl/cast_array.c"
                }
            }
            else if (TYPE(x) != T_ARRAY) {
                if (x == Qnil) x = INT2FIX(0);
                z = m_num_to_data(x);
                SET_DATA_STRIDE(p2, s2, dtype, z)
#line 48 "tmpl/cast_array.c"
            }
        }
        z = m_zero;
        for (; i<n; i++) {
            SET_DATA_STRIDE(p2, s2, dtype, z)
#line 53 "tmpl/cast_array.c"
        }
        
#line 58 "tmpl/cast_array.c"
    }
}

static VALUE
nary_uint16_cast_array(VALUE rary)
#line 63 "tmpl/cast_array.c"
{
    volatile VALUE vnc, nary;
    narray_t *na;
    na_compose_t *nc;
    ndfunc_arg_in_t ain[2] = {{rb_cArray,0},{Qnil,0}};
    ndfunc_t ndf = { iter_uint16_store_array, FULL_LOOP, 2, 0, ain, 0 };

    vnc = na_ary_composition(rary);
    Data_Get_Struct(vnc, na_compose_t, nc);
    nary = rb_narray_new(cT, nc->ndim, nc->shape);
    GetNArray(nary,na);
    if (na->size > 0) {
        nary_uint16_allocate(nary);
#line 76 "tmpl/cast_array.c"
        na_ndloop_cast_rarray_to_narray(&ndf, rary, nary);
    }
    return nary;
}



#line 1 "tmpl/store_array.c"
static VALUE
nary_uint16_store_array(VALUE self, VALUE obj)
#line 3 "tmpl/store_array.c"
{
    return nary_uint16_store(self,nary_uint16_cast_array(obj));
}



#line 1 "tmpl/store_from.c"
static void
iter_uint16_store_dfloat(na_loop_t *const lp)
#line 3 "tmpl/store_from.c"
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    double x;
#line 8 "tmpl/store_from.c"
    dtype y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,double,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,double,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,double,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,double,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    }
}


static VALUE
nary_uint16_store_dfloat(VALUE self, VALUE obj)
#line 47 "tmpl/store_from.c"
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{Qnil,0}};
    ndfunc_t ndf = { iter_uint16_store_dfloat, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, obj);
    return self;
}



#line 1 "tmpl/store_from.c"
static void
iter_uint16_store_sfloat(na_loop_t *const lp)
#line 3 "tmpl/store_from.c"
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    float x;
#line 8 "tmpl/store_from.c"
    dtype y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,float,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,float,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,float,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,float,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    }
}


static VALUE
nary_uint16_store_sfloat(VALUE self, VALUE obj)
#line 47 "tmpl/store_from.c"
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{Qnil,0}};
    ndfunc_t ndf = { iter_uint16_store_sfloat, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, obj);
    return self;
}



#line 1 "tmpl/store_from.c"
static void
iter_uint16_store_int64(na_loop_t *const lp)
#line 3 "tmpl/store_from.c"
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    int64_t x;
#line 8 "tmpl/store_from.c"
    dtype y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,int64_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,int64_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,int64_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,int64_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    }
}


static VALUE
nary_uint16_store_int64(VALUE self, VALUE obj)
#line 47 "tmpl/store_from.c"
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{Qnil,0}};
    ndfunc_t ndf = { iter_uint16_store_int64, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, obj);
    return self;
}



#line 1 "tmpl/store_from.c"
static void
iter_uint16_store_int32(na_loop_t *const lp)
#line 3 "tmpl/store_from.c"
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    int32_t x;
#line 8 "tmpl/store_from.c"
    dtype y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,int32_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,int32_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,int32_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,int32_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    }
}


static VALUE
nary_uint16_store_int32(VALUE self, VALUE obj)
#line 47 "tmpl/store_from.c"
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{Qnil,0}};
    ndfunc_t ndf = { iter_uint16_store_int32, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, obj);
    return self;
}



#line 1 "tmpl/store_from.c"
static void
iter_uint16_store_int16(na_loop_t *const lp)
#line 3 "tmpl/store_from.c"
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    int16_t x;
#line 8 "tmpl/store_from.c"
    dtype y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,int16_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,int16_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,int16_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,int16_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    }
}


static VALUE
nary_uint16_store_int16(VALUE self, VALUE obj)
#line 47 "tmpl/store_from.c"
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{Qnil,0}};
    ndfunc_t ndf = { iter_uint16_store_int16, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, obj);
    return self;
}



#line 1 "tmpl/store_from.c"
static void
iter_uint16_store_int8(na_loop_t *const lp)
#line 3 "tmpl/store_from.c"
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    int8_t x;
#line 8 "tmpl/store_from.c"
    dtype y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,int8_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,int8_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,int8_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,int8_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    }
}


static VALUE
nary_uint16_store_int8(VALUE self, VALUE obj)
#line 47 "tmpl/store_from.c"
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{Qnil,0}};
    ndfunc_t ndf = { iter_uint16_store_int8, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, obj);
    return self;
}



#line 1 "tmpl/store_from.c"
static void
iter_uint16_store_uint64(na_loop_t *const lp)
#line 3 "tmpl/store_from.c"
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    u_int64_t x;
#line 8 "tmpl/store_from.c"
    dtype y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,u_int64_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,u_int64_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,u_int64_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,u_int64_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    }
}


static VALUE
nary_uint16_store_uint64(VALUE self, VALUE obj)
#line 47 "tmpl/store_from.c"
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{Qnil,0}};
    ndfunc_t ndf = { iter_uint16_store_uint64, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, obj);
    return self;
}



#line 1 "tmpl/store_from.c"
static void
iter_uint16_store_uint32(na_loop_t *const lp)
#line 3 "tmpl/store_from.c"
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    u_int32_t x;
#line 8 "tmpl/store_from.c"
    dtype y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,u_int32_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,u_int32_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,u_int32_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,u_int32_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    }
}


static VALUE
nary_uint16_store_uint32(VALUE self, VALUE obj)
#line 47 "tmpl/store_from.c"
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{Qnil,0}};
    ndfunc_t ndf = { iter_uint16_store_uint32, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, obj);
    return self;
}



#line 1 "tmpl/store_from.c"
static void
iter_uint16_store_uint16(na_loop_t *const lp)
#line 3 "tmpl/store_from.c"
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    u_int16_t x;
#line 8 "tmpl/store_from.c"
    dtype y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,u_int16_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,u_int16_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,u_int16_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,u_int16_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    }
}


static VALUE
nary_uint16_store_uint16(VALUE self, VALUE obj)
#line 47 "tmpl/store_from.c"
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{Qnil,0}};
    ndfunc_t ndf = { iter_uint16_store_uint16, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, obj);
    return self;
}



#line 1 "tmpl/store_from.c"
static void
iter_uint16_store_uint8(na_loop_t *const lp)
#line 3 "tmpl/store_from.c"
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    u_int8_t x;
#line 8 "tmpl/store_from.c"
    dtype y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,u_int8_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,u_int8_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,u_int8_t,x);
                y = m_from_real(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,u_int8_t,x);
                y = m_from_real(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    }
}


static VALUE
nary_uint16_store_uint8(VALUE self, VALUE obj)
#line 47 "tmpl/store_from.c"
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{Qnil,0}};
    ndfunc_t ndf = { iter_uint16_store_uint8, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, obj);
    return self;
}



#line 1 "tmpl/store.c"
/*
  Store elements to NArray::UInt16 from other.
  @overload store(other)
  @param [Object] other
  @return [NArray::UInt16] self
*/
static VALUE
nary_uint16_store(VALUE self, VALUE obj)
#line 9 "tmpl/store.c"
{
    VALUE r;

    
#line 13 "tmpl/store.c"
    if (FIXNUM_P(obj) || TYPE(obj)==T_FLOAT || TYPE(obj)==T_BIGNUM || rb_obj_is_kind_of(obj,rb_cComplex)) {
        nary_uint16_store_numeric(self,obj);
#line 15 "tmpl/store.c"
        return self;
    }
    
#line 13 "tmpl/store.c"
    if (TYPE(obj)==T_ARRAY) {
        nary_uint16_store_array(self,obj);
#line 15 "tmpl/store.c"
        return self;
    }
    
#line 13 "tmpl/store.c"
    if (rb_obj_is_kind_of(obj,cDFloat)) {
        nary_uint16_store_dfloat(self,obj);
#line 15 "tmpl/store.c"
        return self;
    }
    
#line 13 "tmpl/store.c"
    if (rb_obj_is_kind_of(obj,cSFloat)) {
        nary_uint16_store_sfloat(self,obj);
#line 15 "tmpl/store.c"
        return self;
    }
    
#line 13 "tmpl/store.c"
    if (rb_obj_is_kind_of(obj,cInt64)) {
        nary_uint16_store_int64(self,obj);
#line 15 "tmpl/store.c"
        return self;
    }
    
#line 13 "tmpl/store.c"
    if (rb_obj_is_kind_of(obj,cInt32)) {
        nary_uint16_store_int32(self,obj);
#line 15 "tmpl/store.c"
        return self;
    }
    
#line 13 "tmpl/store.c"
    if (rb_obj_is_kind_of(obj,cInt16)) {
        nary_uint16_store_int16(self,obj);
#line 15 "tmpl/store.c"
        return self;
    }
    
#line 13 "tmpl/store.c"
    if (rb_obj_is_kind_of(obj,cInt8)) {
        nary_uint16_store_int8(self,obj);
#line 15 "tmpl/store.c"
        return self;
    }
    
#line 13 "tmpl/store.c"
    if (rb_obj_is_kind_of(obj,cUInt64)) {
        nary_uint16_store_uint64(self,obj);
#line 15 "tmpl/store.c"
        return self;
    }
    
#line 13 "tmpl/store.c"
    if (rb_obj_is_kind_of(obj,cUInt32)) {
        nary_uint16_store_uint32(self,obj);
#line 15 "tmpl/store.c"
        return self;
    }
    
#line 13 "tmpl/store.c"
    if (rb_obj_is_kind_of(obj,cUInt16)) {
        nary_uint16_store_uint16(self,obj);
#line 15 "tmpl/store.c"
        return self;
    }
    
#line 13 "tmpl/store.c"
    if (rb_obj_is_kind_of(obj,cUInt8)) {
        nary_uint16_store_uint8(self,obj);
#line 15 "tmpl/store.c"
        return self;
    }
    
#line 18 "tmpl/store.c"

    if (IsNArray(obj)) {
        r = rb_funcall(obj, rb_intern("coerce_cast"), 1, cT);
        if (CLASS_OF(r)==cT) {
            nary_uint16_store(self,r);
#line 23 "tmpl/store.c"
            return self;
        }
    }

    rb_raise(nary_eCastError, "unknown conversion from %s to %s",
             rb_class2name(CLASS_OF(obj)),
             rb_class2name(CLASS_OF(self)));
    return self;
}



#line 1 "tmpl/cast.c"
/*
  Cast object to NArray::UInt16.
  @overload [](elements)
  @overload cast(array)
  @param [Numeric,Array] elements
  @param [Array] array
  @return [NArray::UInt16]
*/
static VALUE
nary_uint16_s_cast(VALUE type, VALUE obj)
#line 11 "tmpl/cast.c"
{
    VALUE v;
    narray_t *na;
    dtype x;

    if (CLASS_OF(obj)==cT) {
        return obj;
    }
    if (RTEST(rb_obj_is_kind_of(obj,rb_cNumeric))) {
        x = m_num_to_data(obj);
        return nary_uint16_new_dim0(x);
    }
    if (RTEST(rb_obj_is_kind_of(obj,rb_cArray))) {
        return nary_uint16_cast_array(obj);
    }
    if (IsNArray(obj)) {
        GetNArray(obj,na);
        v = rb_narray_new(cT, NA_NDIM(na), NA_SHAPE(na));
        if (NA_SIZE(na) > 0) {
            nary_uint16_allocate(v);
            nary_uint16_store(v,obj);
#line 32 "tmpl/cast.c"
        }
        return v;
    }
    rb_raise(nary_eCastError,"cannot cast to %s",rb_class2name(type));
    return Qnil;
}



#line 1 "tmpl/coerce_cast.c"
/*
   @return [nil]
*/
static VALUE
nary_uint16_coerce_cast(VALUE value, VALUE type)
#line 6 "tmpl/coerce_cast.c"
{
    return Qnil;
}



#line 1 "tmpl/to_a.c"
void
iter_uint16_to_a(na_loop_t *const lp)
#line 3 "tmpl/to_a.c"
{
    size_t i, s1;
    char *p1;
    size_t *idx1;
    dtype x;
    volatile VALUE a, y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    a = rb_ary_new2(i);
    rb_ary_push(lp->args[1].value, a);
    if (idx1) {
        for (; i--;) {
            GET_DATA_INDEX(p1,idx1,dtype,x);
            y = m_data_to_num(x);
            rb_ary_push(a,y);
        }
    } else {
        for (; i--;) {
            GET_DATA_STRIDE(p1,s1,dtype,x);
            y = m_data_to_num(x);
            rb_ary_push(a,y);
        }
    }
}

/*
  Convert self to Array.
  @overload to_a
  @return [Array]
*/
static VALUE
nary_uint16_to_a(VALUE self)
#line 36 "tmpl/to_a.c"
{
    ndfunc_arg_in_t ain[3] = {{Qnil,0},{sym_loop_opt},{sym_option}};
    ndfunc_arg_out_t aout[1] = {{rb_cArray,0}}; // dummy?
    ndfunc_t ndf = { iter_uint16_to_a, FULL_LOOP_NIP, 3, 1, ain, aout };
    return na_ndloop_cast_narray_to_rarray(&ndf, self, Qnil);
}



#line 1 "tmpl/fill.c"
static void
iter_uint16_fill(na_loop_t *const lp)
#line 3 "tmpl/fill.c"
{
    size_t   i;
    char    *p1;
    ssize_t  s1;
    size_t  *idx1;
    VALUE    x = lp->option;
    dtype    y;
    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    y = m_num_to_data(x);
    if (idx1) {
        for (; i--;) {
            SET_DATA_INDEX(p1,idx1,dtype,y);
        }
    } else {
        for (; i--;) {
            SET_DATA_STRIDE(p1,s1,dtype,y);
        }
    }
}

/*
  Fill elements with other.
  @overload fill other
  @param [Numeric] other
  @return [NArray::UInt16] self.
*/
static VALUE
nary_uint16_fill(VALUE self, VALUE val)
#line 32 "tmpl/fill.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{sym_option}};
    ndfunc_t ndf = { iter_uint16_fill, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, val);
    return self;
}



#line 1 "tmpl/format.c"
static VALUE
format_uint16(VALUE fmt, dtype* x)
{
    // fix-me
    char s[48];
    int n;

    if (NIL_P(fmt)) {
        n = m_sprintf(s,*x);
        return rb_str_new(s,n);
    }
    return rb_funcall(fmt, '%', 1, m_data_to_num(*x));
}

static void
iter_uint16_format(na_loop_t *const lp)
#line 17 "tmpl/format.c"
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1;
    dtype *x;
    VALUE y;
    VALUE fmt = lp->option;
    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2);
    if (idx1) {
        for (; i--;) {
            x = (dtype*)(p1+*idx1); idx1++;
            y = format_uint16(fmt, x);
            SET_DATA_STRIDE(p2, s2, VALUE, y);
        }
    } else {
        for (; i--;) {
            x = (dtype*)p1;         p1+=s1;
            y = format_uint16(fmt, x);
            SET_DATA_STRIDE(p2, s2, VALUE, y);
        }
    }
}

/*
  Format elements into strings.
  @overload format format
  @param [String] format
  @return [NArray::RObject] array of formated strings.
*/
static VALUE
nary_uint16_format(int argc, VALUE *argv, VALUE self)
#line 51 "tmpl/format.c"
{
    VALUE fmt=Qnil;

    ndfunc_arg_in_t ain[2] = {{Qnil,0},{sym_option}};
    ndfunc_arg_out_t aout[1] = {{cRObject,0}};
    ndfunc_t ndf = { iter_uint16_format, FULL_LOOP_NIP, 2, 1, ain, aout };

    rb_scan_args(argc, argv, "01", &fmt);
    return na_ndloop(&ndf, 2, self, fmt);
}



#line 1 "tmpl/format_to_a.c"
static void
iter_uint16_format_to_a(na_loop_t *const lp)
#line 3 "tmpl/format_to_a.c"
{
    size_t  i;
    char   *p1;
    ssize_t s1;
    size_t *idx1;
    dtype *x;
    VALUE y;
    volatile VALUE a;
    VALUE fmt = lp->option;
    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    a = rb_ary_new2(i);
    rb_ary_push(lp->args[1].value, a);
    if (idx1) {
        for (; i--;) {
            x = (dtype*)(p1 + *idx1);  idx1++;
            y = format_uint16(fmt, x);
            rb_ary_push(a,y);
        }
    } else {
        for (; i--;) {
            x = (dtype*)p1;  p1+=s1;
            y = format_uint16(fmt, x);
            rb_ary_push(a,y);
        }
    }
}

/*
  Format elements into strings.
  @overload format_to_a format
  @param [String] format
  @return [Array] array of formated strings.
*/
static VALUE
nary_uint16_format_to_a(int argc, VALUE *argv, VALUE self)
#line 39 "tmpl/format_to_a.c"
{
    volatile VALUE fmt=Qnil;
    ndfunc_arg_in_t ain[3] = {{Qnil,0},{sym_loop_opt},{sym_option}};
    ndfunc_arg_out_t aout[1] = {{rb_cArray,0}}; // dummy?
    ndfunc_t ndf = { iter_uint16_format_to_a, FULL_LOOP_NIP, 3, 1, ain, aout };

    rb_scan_args(argc, argv, "01", &fmt);
    return na_ndloop_cast_narray_to_rarray(&ndf, self, fmt);
}



#line 1 "tmpl/inspect.c"
static VALUE
iter_uint16_inspect(char *ptr, size_t pos, VALUE fmt)
#line 3 "tmpl/inspect.c"
{
    return format_uint16(fmt, (dtype*)(ptr+pos));
}

/*
  Returns a string containing a human-readable representation of NArray.
  @overload inspect
  @return [String]
*/
VALUE
nary_uint16_inspect(VALUE ary)
#line 14 "tmpl/inspect.c"
{
    VALUE str = na_info_str(ary);
    na_ndloop_inspect(ary, str, iter_uint16_inspect, Qnil);
    return str;
}



#line 1 "tmpl/unary2.c"
static void
iter_uint16_abs(na_loop_t *const lp)
#line 3 "tmpl/unary2.c"
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype   x;
    rtype y;
#line 10 "tmpl/unary2.c"
    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx1) {
        if (idx2) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                y = m_abs(x);
                SET_DATA_INDEX(p2,idx2,rtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                y = m_abs(x);
                SET_DATA_STRIDE(p2,s2,rtype,y);
            }
        }
    } else {
        if (idx2) {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                y = m_abs(x);
                SET_DATA_INDEX(p2,idx2,rtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                y = m_abs(x);
                SET_DATA_STRIDE(p2,s2,rtype,y);
            }
        }
    }
}


/*
  abs of self.
#line 47 "tmpl/unary2.c"
  @overload abs
  @return [NArray::UInt16] abs of self.
*/
static VALUE
nary_uint16_abs(VALUE self)
#line 52 "tmpl/unary2.c"
{
    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cRT,0}};
    ndfunc_t ndf = { iter_uint16_abs, FULL_LOOP, 1, 1, ain, aout };

    return na_ndloop(&ndf, 1, self);
}



#line 1 "tmpl/binary.c"
static void
iter_uint16_add(na_loop_t *const lp)
#line 3 "tmpl/binary.c"
{
    size_t   i, n;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;
    dtype    x, y;
    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (i=n; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        x = m_add(x,y);
#line 22 "tmpl/binary.c"
        SET_DATA_STRIDE(p3,s3,dtype,x);
    }
}

static VALUE
nary_uint16_add_self(VALUE self, VALUE other)
#line 28 "tmpl/binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_add, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary add.
  @overload + other
  @param [NArray,Numeric] other
  @return [NArray] add of self and other.
*/
static VALUE
nary_uint16_add(VALUE self, VALUE other)
#line 44 "tmpl/binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_add_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, '+', 1, other);
    }
}



#line 1 "tmpl/binary.c"
static void
iter_uint16_sub(na_loop_t *const lp)
#line 3 "tmpl/binary.c"
{
    size_t   i, n;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;
    dtype    x, y;
    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (i=n; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        x = m_sub(x,y);
#line 22 "tmpl/binary.c"
        SET_DATA_STRIDE(p3,s3,dtype,x);
    }
}

static VALUE
nary_uint16_sub_self(VALUE self, VALUE other)
#line 28 "tmpl/binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_sub, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary sub.
  @overload - other
  @param [NArray,Numeric] other
  @return [NArray] sub of self and other.
*/
static VALUE
nary_uint16_sub(VALUE self, VALUE other)
#line 44 "tmpl/binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_sub_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, '-', 1, other);
    }
}



#line 1 "tmpl/binary.c"
static void
iter_uint16_mul(na_loop_t *const lp)
#line 3 "tmpl/binary.c"
{
    size_t   i, n;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;
    dtype    x, y;
    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (i=n; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        x = m_mul(x,y);
#line 22 "tmpl/binary.c"
        SET_DATA_STRIDE(p3,s3,dtype,x);
    }
}

static VALUE
nary_uint16_mul_self(VALUE self, VALUE other)
#line 28 "tmpl/binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_mul, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary mul.
  @overload * other
  @param [NArray,Numeric] other
  @return [NArray] mul of self and other.
*/
static VALUE
nary_uint16_mul(VALUE self, VALUE other)
#line 44 "tmpl/binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_mul_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, '*', 1, other);
    }
}



#line 1 "tmpl/binary.c"
static void
iter_uint16_div(na_loop_t *const lp)
#line 3 "tmpl/binary.c"
{
    size_t   i, n;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;
    dtype    x, y;
    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (i=n; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        if (y==0) {
#line 17 "tmpl/binary.c"
            lp->err_type = rb_eZeroDivError;
            return;
        }
        x = m_div(x,y);
#line 22 "tmpl/binary.c"
        SET_DATA_STRIDE(p3,s3,dtype,x);
    }
}

static VALUE
nary_uint16_div_self(VALUE self, VALUE other)
#line 28 "tmpl/binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_div, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary div.
  @overload / other
  @param [NArray,Numeric] other
  @return [NArray] div of self and other.
*/
static VALUE
nary_uint16_div(VALUE self, VALUE other)
#line 44 "tmpl/binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_div_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, '/', 1, other);
    }
}



#line 1 "tmpl/binary.c"
static void
iter_uint16_mod(na_loop_t *const lp)
#line 3 "tmpl/binary.c"
{
    size_t   i, n;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;
    dtype    x, y;
    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (i=n; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        if (y==0) {
#line 17 "tmpl/binary.c"
            lp->err_type = rb_eZeroDivError;
            return;
        }
        x = m_mod(x,y);
#line 22 "tmpl/binary.c"
        SET_DATA_STRIDE(p3,s3,dtype,x);
    }
}

static VALUE
nary_uint16_mod_self(VALUE self, VALUE other)
#line 28 "tmpl/binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_mod, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary mod.
  @overload % other
  @param [NArray,Numeric] other
  @return [NArray] mod of self and other.
*/
static VALUE
nary_uint16_mod(VALUE self, VALUE other)
#line 44 "tmpl/binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_mod_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, '%', 1, other);
    }
}



#line 1 "tmpl/binary2.c"
static void
iter_uint16_divmod(na_loop_t *const lp)
#line 3 "tmpl/binary2.c"
{
    size_t   i, n;
    char    *p1, *p2, *p3, *p4;
    ssize_t  s1, s2, s3, s4;
    dtype    x, y, a, b;
    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    INIT_PTR(lp, 3, p4, s4);
    for (i=n; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        if (y==0) {
#line 18 "tmpl/binary2.c"
            lp->err_type = rb_eZeroDivError;
            return;
        }
        m_divmod(x,y,a,b);
#line 23 "tmpl/binary2.c"
        SET_DATA_STRIDE(p3,s3,dtype,a);
        SET_DATA_STRIDE(p4,s4,dtype,b);
    }
}

static VALUE
nary_uint16_divmod_self(VALUE self, VALUE other)
#line 30 "tmpl/binary2.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[2] = {{cT,0},{cT,0}};
    ndfunc_t ndf = { iter_uint16_divmod, STRIDE_LOOP, 2, 2, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary divmod.
  @overload divmod other
  @param [NArray,Numeric] other
  @return [NArray] divmod of self and other.
*/
static VALUE
nary_uint16_divmod(VALUE self, VALUE other)
#line 46 "tmpl/binary2.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_divmod_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, id_divmod, 1, other);
    }
}



#line 1 "tmpl/pow.c"
static void
iter_uint16_pow(na_loop_t *const lp)
#line 3 "tmpl/pow.c"
{
    size_t  i;
    char    *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    dtype    x, y;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        x = m_pow(x,y);
        SET_DATA_STRIDE(p3,s3,dtype,x);
    }
}

static void
iter_uint16_pow_int32(na_loop_t *const lp)
#line 22 "tmpl/pow.c"
{
    size_t  i;
    char   *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    dtype   x;
    int32_t y;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,int32_t,y);
        x = m_pow_int(x,y);
        SET_DATA_STRIDE(p3,s3,dtype,x);
    }
}

static VALUE
nary_uint16_pow_self(VALUE self, VALUE other)
#line 42 "tmpl/pow.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_in_t ain_i[2] = {{cT,0},{cInt32,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_pow, STRIDE_LOOP, 2, 1, ain, aout };
    ndfunc_t ndf_i = { iter_uint16_pow_int32, STRIDE_LOOP, 2, 1, ain_i, aout };

    // fixme : use na.integer?
    if (FIXNUM_P(other) || rb_obj_is_kind_of(other,cInt32)) {
        return na_ndloop(&ndf_i, 2, self, other);
    } else {
        return na_ndloop(&ndf, 2, self, other);
    }
}

/*
  pow.
#line 59 "tmpl/pow.c"
  @overload pow other
  @param [NArray,Numeric] other
  @return [NArray] self pow other.
*/
static VALUE
nary_uint16_pow(VALUE self, VALUE other)
#line 65 "tmpl/pow.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_pow_self(self,other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, id_pow, 1, other);
    }
}



#line 1 "tmpl/unary.c"
static void
iter_uint16_minus(na_loop_t *const lp)
#line 3 "tmpl/unary.c"
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype   x;
    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx1) {
        if (idx2) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                x = m_minus(x);
                SET_DATA_INDEX(p2,idx2,dtype,x);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                x = m_minus(x);
                SET_DATA_STRIDE(p2,s2,dtype,x);
            }
        }
    } else {
        if (idx2) {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                x = m_minus(x);
                SET_DATA_INDEX(p2,idx2,dtype,x);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                x = m_minus(x);
                SET_DATA_STRIDE(p2,s2,dtype,x);
            }
        }
    }
}

/*
  Unary minus.
  @overload -@
  @return [NArray::UInt16] minus of self.
*/
static VALUE
nary_uint16_minus(VALUE self)
#line 50 "tmpl/unary.c"
{
    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_minus, FULL_LOOP, 1, 1, ain, aout };

    return na_ndloop(&ndf, 1, self);
}



#line 1 "tmpl/unary.c"
static void
iter_uint16_inverse(na_loop_t *const lp)
#line 3 "tmpl/unary.c"
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype   x;
    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx1) {
        if (idx2) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                x = m_inverse(x);
                SET_DATA_INDEX(p2,idx2,dtype,x);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                x = m_inverse(x);
                SET_DATA_STRIDE(p2,s2,dtype,x);
            }
        }
    } else {
        if (idx2) {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                x = m_inverse(x);
                SET_DATA_INDEX(p2,idx2,dtype,x);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                x = m_inverse(x);
                SET_DATA_STRIDE(p2,s2,dtype,x);
            }
        }
    }
}

/*
  Unary inverse.
  @overload inverse
  @return [NArray::UInt16] inverse of self.
*/
static VALUE
nary_uint16_inverse(VALUE self)
#line 50 "tmpl/unary.c"
{
    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_inverse, FULL_LOOP, 1, 1, ain, aout };

    return na_ndloop(&ndf, 1, self);
}



#line 1 "tmpl/cond_binary.c"
static void
iter_uint16_eq(na_loop_t *const lp)
#line 3 "tmpl/cond_binary.c"
{
    size_t  i;
    char   *p1, *p2;
    BIT_DIGIT *a3;
    size_t  p3;
    ssize_t s1, s2, s3;
    size_t *idx3;
    dtype   x, y;
    BIT_DIGIT b;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR_BIT(lp, 2, a3, p3, s3, idx3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        b = (m_eq(x,y)) ? 1:0;
        STORE_BIT(a3,p3,b);
        p3+=s3;
    }
}

static VALUE
nary_uint16_eq_self(VALUE self, VALUE other)
#line 27 "tmpl/cond_binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cBit,0}};
    ndfunc_t ndf = { iter_uint16_eq, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Comparison eq other.
  @overload eq other
  @param [NArray,Numeric] other
  @return [NArray::Bit] result of self eq other.
*/
static VALUE
nary_uint16_eq(VALUE self, VALUE other)
#line 43 "tmpl/cond_binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_eq_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, id_eq, 1, other);
    }
}



#line 1 "tmpl/cond_binary.c"
static void
iter_uint16_ne(na_loop_t *const lp)
#line 3 "tmpl/cond_binary.c"
{
    size_t  i;
    char   *p1, *p2;
    BIT_DIGIT *a3;
    size_t  p3;
    ssize_t s1, s2, s3;
    size_t *idx3;
    dtype   x, y;
    BIT_DIGIT b;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR_BIT(lp, 2, a3, p3, s3, idx3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        b = (m_ne(x,y)) ? 1:0;
        STORE_BIT(a3,p3,b);
        p3+=s3;
    }
}

static VALUE
nary_uint16_ne_self(VALUE self, VALUE other)
#line 27 "tmpl/cond_binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cBit,0}};
    ndfunc_t ndf = { iter_uint16_ne, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Comparison ne other.
  @overload ne other
  @param [NArray,Numeric] other
  @return [NArray::Bit] result of self ne other.
*/
static VALUE
nary_uint16_ne(VALUE self, VALUE other)
#line 43 "tmpl/cond_binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_ne_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, id_ne, 1, other);
    }
}



#line 1 "tmpl/binary.c"
static void
iter_uint16_bit_and(na_loop_t *const lp)
#line 3 "tmpl/binary.c"
{
    size_t   i, n;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;
    dtype    x, y;
    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (i=n; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        x = m_bit_and(x,y);
#line 22 "tmpl/binary.c"
        SET_DATA_STRIDE(p3,s3,dtype,x);
    }
}

static VALUE
nary_uint16_bit_and_self(VALUE self, VALUE other)
#line 28 "tmpl/binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_bit_and, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary bit_and.
  @overload & other
  @param [NArray,Numeric] other
  @return [NArray] bit_and of self and other.
*/
static VALUE
nary_uint16_bit_and(VALUE self, VALUE other)
#line 44 "tmpl/binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_bit_and_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, '&', 1, other);
    }
}



#line 1 "tmpl/binary.c"
static void
iter_uint16_bit_or(na_loop_t *const lp)
#line 3 "tmpl/binary.c"
{
    size_t   i, n;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;
    dtype    x, y;
    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (i=n; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        x = m_bit_or(x,y);
#line 22 "tmpl/binary.c"
        SET_DATA_STRIDE(p3,s3,dtype,x);
    }
}

static VALUE
nary_uint16_bit_or_self(VALUE self, VALUE other)
#line 28 "tmpl/binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_bit_or, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary bit_or.
  @overload | other
  @param [NArray,Numeric] other
  @return [NArray] bit_or of self and other.
*/
static VALUE
nary_uint16_bit_or(VALUE self, VALUE other)
#line 44 "tmpl/binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_bit_or_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, '|', 1, other);
    }
}



#line 1 "tmpl/binary.c"
static void
iter_uint16_bit_xor(na_loop_t *const lp)
#line 3 "tmpl/binary.c"
{
    size_t   i, n;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;
    dtype    x, y;
    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (i=n; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        x = m_bit_xor(x,y);
#line 22 "tmpl/binary.c"
        SET_DATA_STRIDE(p3,s3,dtype,x);
    }
}

static VALUE
nary_uint16_bit_xor_self(VALUE self, VALUE other)
#line 28 "tmpl/binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_bit_xor, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary bit_xor.
  @overload ^ other
  @param [NArray,Numeric] other
  @return [NArray] bit_xor of self and other.
*/
static VALUE
nary_uint16_bit_xor(VALUE self, VALUE other)
#line 44 "tmpl/binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_bit_xor_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, '^', 1, other);
    }
}



#line 1 "tmpl/unary.c"
static void
iter_uint16_bit_not(na_loop_t *const lp)
#line 3 "tmpl/unary.c"
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype   x;
    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx1) {
        if (idx2) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                x = m_bit_not(x);
                SET_DATA_INDEX(p2,idx2,dtype,x);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                x = m_bit_not(x);
                SET_DATA_STRIDE(p2,s2,dtype,x);
            }
        }
    } else {
        if (idx2) {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                x = m_bit_not(x);
                SET_DATA_INDEX(p2,idx2,dtype,x);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                x = m_bit_not(x);
                SET_DATA_STRIDE(p2,s2,dtype,x);
            }
        }
    }
}

/*
  Unary bit_not.
  @overload ~
  @return [NArray::UInt16] bit_not of self.
*/
static VALUE
nary_uint16_bit_not(VALUE self)
#line 50 "tmpl/unary.c"
{
    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_bit_not, FULL_LOOP, 1, 1, ain, aout };

    return na_ndloop(&ndf, 1, self);
}



#line 1 "tmpl/cond_binary.c"
static void
iter_uint16_gt(na_loop_t *const lp)
#line 3 "tmpl/cond_binary.c"
{
    size_t  i;
    char   *p1, *p2;
    BIT_DIGIT *a3;
    size_t  p3;
    ssize_t s1, s2, s3;
    size_t *idx3;
    dtype   x, y;
    BIT_DIGIT b;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR_BIT(lp, 2, a3, p3, s3, idx3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        b = (m_gt(x,y)) ? 1:0;
        STORE_BIT(a3,p3,b);
        p3+=s3;
    }
}

static VALUE
nary_uint16_gt_self(VALUE self, VALUE other)
#line 27 "tmpl/cond_binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cBit,0}};
    ndfunc_t ndf = { iter_uint16_gt, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Comparison gt other.
  @overload gt other
  @param [NArray,Numeric] other
  @return [NArray::Bit] result of self gt other.
*/
static VALUE
nary_uint16_gt(VALUE self, VALUE other)
#line 43 "tmpl/cond_binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_gt_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, id_gt, 1, other);
    }
}



#line 1 "tmpl/cond_binary.c"
static void
iter_uint16_ge(na_loop_t *const lp)
#line 3 "tmpl/cond_binary.c"
{
    size_t  i;
    char   *p1, *p2;
    BIT_DIGIT *a3;
    size_t  p3;
    ssize_t s1, s2, s3;
    size_t *idx3;
    dtype   x, y;
    BIT_DIGIT b;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR_BIT(lp, 2, a3, p3, s3, idx3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        b = (m_ge(x,y)) ? 1:0;
        STORE_BIT(a3,p3,b);
        p3+=s3;
    }
}

static VALUE
nary_uint16_ge_self(VALUE self, VALUE other)
#line 27 "tmpl/cond_binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cBit,0}};
    ndfunc_t ndf = { iter_uint16_ge, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Comparison ge other.
  @overload ge other
  @param [NArray,Numeric] other
  @return [NArray::Bit] result of self ge other.
*/
static VALUE
nary_uint16_ge(VALUE self, VALUE other)
#line 43 "tmpl/cond_binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_ge_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, id_ge, 1, other);
    }
}



#line 1 "tmpl/cond_binary.c"
static void
iter_uint16_lt(na_loop_t *const lp)
#line 3 "tmpl/cond_binary.c"
{
    size_t  i;
    char   *p1, *p2;
    BIT_DIGIT *a3;
    size_t  p3;
    ssize_t s1, s2, s3;
    size_t *idx3;
    dtype   x, y;
    BIT_DIGIT b;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR_BIT(lp, 2, a3, p3, s3, idx3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        b = (m_lt(x,y)) ? 1:0;
        STORE_BIT(a3,p3,b);
        p3+=s3;
    }
}

static VALUE
nary_uint16_lt_self(VALUE self, VALUE other)
#line 27 "tmpl/cond_binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cBit,0}};
    ndfunc_t ndf = { iter_uint16_lt, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Comparison lt other.
  @overload lt other
  @param [NArray,Numeric] other
  @return [NArray::Bit] result of self lt other.
*/
static VALUE
nary_uint16_lt(VALUE self, VALUE other)
#line 43 "tmpl/cond_binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_lt_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, id_lt, 1, other);
    }
}



#line 1 "tmpl/cond_binary.c"
static void
iter_uint16_le(na_loop_t *const lp)
#line 3 "tmpl/cond_binary.c"
{
    size_t  i;
    char   *p1, *p2;
    BIT_DIGIT *a3;
    size_t  p3;
    ssize_t s1, s2, s3;
    size_t *idx3;
    dtype   x, y;
    BIT_DIGIT b;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR_BIT(lp, 2, a3, p3, s3, idx3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        b = (m_le(x,y)) ? 1:0;
        STORE_BIT(a3,p3,b);
        p3+=s3;
    }
}

static VALUE
nary_uint16_le_self(VALUE self, VALUE other)
#line 27 "tmpl/cond_binary.c"
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cBit,0}};
    ndfunc_t ndf = { iter_uint16_le, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Comparison le other.
  @overload le other
  @param [NArray,Numeric] other
  @return [NArray::Bit] result of self le other.
*/
static VALUE
nary_uint16_le(VALUE self, VALUE other)
#line 43 "tmpl/cond_binary.c"
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return nary_uint16_le_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, id_le, 1, other);
    }
}



#line 1 "tmpl/accum.c"
static void
iter_uint16_sum(na_loop_t *const lp)
#line 3 "tmpl/accum.c"
{
    size_t   i;
    char    *p1, *p2;
    ssize_t  s1, s2;
    size_t  *idx1;
    dtype    x, y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2);
    if (s2==0) {
        // Reduce loop
        GET_DATA(p2,dtype,y);
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                m_sum(x,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                m_sum(x,y);
            }
        }
        SET_DATA(p2,dtype,y);
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                GET_DATA(p2,dtype,y);
                m_sum(x,y);
                SET_DATA_STRIDE(p2,s2,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                GET_DATA(p2,dtype,y);
                m_sum(x,y);
                SET_DATA_STRIDE(p2,s2,dtype,y);
            }
        }
    }
}

/*
  Sum of self.
#line 49 "tmpl/accum.c"
  @overload sum(*args)
  @param [Array of Numeric,Range] args  Affected dimensions.
  @return [NArray::UInt16] sum of self.
*/
static VALUE
nary_uint16_sum(int argc, VALUE *argv, VALUE self)
#line 55 "tmpl/accum.c"
{
    VALUE v, reduce;
    ndfunc_arg_in_t ain[3] = {{cT,0},{sym_reduce,0},{sym_init,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_sum, FULL_LOOP_NIP, 3, 1, ain, aout };

    reduce = na_reduce_dimension(argc, argv, self);
    v =  na_ndloop(&ndf, 3, self, reduce, m_sum_init);
    return nary_uint16_extract(v);
}



#line 1 "tmpl/accum.c"
static void
iter_uint16_min(na_loop_t *const lp)
#line 3 "tmpl/accum.c"
{
    size_t   i;
    char    *p1, *p2;
    ssize_t  s1, s2;
    size_t  *idx1;
    dtype    x, y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2);
    if (s2==0) {
        // Reduce loop
        GET_DATA(p2,dtype,y);
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                m_min(x,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                m_min(x,y);
            }
        }
        SET_DATA(p2,dtype,y);
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                GET_DATA(p2,dtype,y);
                m_min(x,y);
                SET_DATA_STRIDE(p2,s2,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                GET_DATA(p2,dtype,y);
                m_min(x,y);
                SET_DATA_STRIDE(p2,s2,dtype,y);
            }
        }
    }
}

/*
  Min of self.
#line 49 "tmpl/accum.c"
  @overload min(*args)
  @param [Array of Numeric,Range] args  Affected dimensions.
  @return [NArray::UInt16] min of self.
*/
static VALUE
nary_uint16_min(int argc, VALUE *argv, VALUE self)
#line 55 "tmpl/accum.c"
{
    VALUE v, reduce;
    ndfunc_arg_in_t ain[3] = {{cT,0},{sym_reduce,0},{sym_init,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_min, FULL_LOOP_NIP, 3, 1, ain, aout };

    reduce = na_reduce_dimension(argc, argv, self);
    v =  na_ndloop(&ndf, 3, self, reduce, m_min_init);
    return nary_uint16_extract(v);
}



#line 1 "tmpl/accum.c"
static void
iter_uint16_max(na_loop_t *const lp)
#line 3 "tmpl/accum.c"
{
    size_t   i;
    char    *p1, *p2;
    ssize_t  s1, s2;
    size_t  *idx1;
    dtype    x, y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2);
    if (s2==0) {
        // Reduce loop
        GET_DATA(p2,dtype,y);
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                m_max(x,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                m_max(x,y);
            }
        }
        SET_DATA(p2,dtype,y);
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                GET_DATA(p2,dtype,y);
                m_max(x,y);
                SET_DATA_STRIDE(p2,s2,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                GET_DATA(p2,dtype,y);
                m_max(x,y);
                SET_DATA_STRIDE(p2,s2,dtype,y);
            }
        }
    }
}

/*
  Max of self.
#line 49 "tmpl/accum.c"
  @overload max(*args)
  @param [Array of Numeric,Range] args  Affected dimensions.
  @return [NArray::UInt16] max of self.
*/
static VALUE
nary_uint16_max(int argc, VALUE *argv, VALUE self)
#line 55 "tmpl/accum.c"
{
    VALUE v, reduce;
    ndfunc_arg_in_t ain[3] = {{cT,0},{sym_reduce,0},{sym_init,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_max, FULL_LOOP_NIP, 3, 1, ain, aout };

    reduce = na_reduce_dimension(argc, argv, self);
    v =  na_ndloop(&ndf, 3, self, reduce, m_max_init);
    return nary_uint16_extract(v);
}



#line 1 "tmpl/seq.c"
static void
iter_uint16_seq(na_loop_t *const lp)
#line 3 "tmpl/seq.c"
{
    size_t  i;
    char   *p1;
    ssize_t s1;
    size_t *idx1;
    double  x, beg, step;
    dtype   y;
    size_t  c;
    seq_opt_t *g;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    g = (seq_opt_t*)(lp->opt_ptr);
    beg  = g->beg;
    step = g->step;
    c    = g->count;
    if (idx1) {
        for (; i--;) {
            x = beg + step * c++;
            y = m_from_double(x);
            *(dtype*)(p1+*idx1) = y;
            idx1++;
        }
    } else {
        for (; i--;) {
            x = beg + step * c++;
            y = m_from_double(x);
            *(dtype*)(p1) = y;
            p1 += s1;
        }
    }
    g->count = c;
}

/*
  Set Sequence of numbers to self NArray.
  @overload seq([beg,[step]])
  @param [Numeric] beg  begining of sequence. (default=0)
  @param [Numeric] step  step of sequence. (default=1)
  @return [NArray::UInt16] self.
*/
static VALUE
nary_uint16_seq(int argc, VALUE *args, VALUE self)
#line 46 "tmpl/seq.c"
{
    seq_opt_t *g;
    VALUE vbeg=Qnil, vstep=Qnil;
    ndfunc_arg_in_t ain[2] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_seq, FULL_LOOP, 1, 0, ain, 0 };

    g = ALLOCA_N(seq_opt_t,1);
    g->beg = 0;
    g->step = 1;
    g->count = 0;
    rb_scan_args(argc, args, "02", &vbeg, &vstep);
    if (vbeg!=Qnil) {g->beg = NUM2DBL(vbeg);}
    if (vstep!=Qnil) {g->step = NUM2DBL(vstep);}

    na_ndloop3(&ndf, g, 1, self);
    return self;
}



#line 1 "tmpl/rand.c"
static void
iter_uint16_rand(na_loop_t *const lp)
#line 3 "tmpl/rand.c"
{
    size_t   i;
    char    *p1;
    ssize_t  s1;
    size_t  *idx1;
    dtype    x;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    if (idx1) {
        for (; i--;) {
            x = m_rand;
            SET_DATA_INDEX(p1,idx1,dtype,x);
        }
    } else {
        for (; i--;) {
            x = m_rand;
            SET_DATA_INDEX(p1,idx1,dtype,x);
        }
    }
}

static VALUE
nary_uint16_rand(VALUE self)
#line 27 "tmpl/rand.c"
{
    ndfunc_arg_in_t ain[1] = {{Qnil,0}};
    ndfunc_t ndf = { iter_uint16_rand, FULL_LOOP, 1, 0, ain, 0 };

    na_ndloop(&ndf, 1, self);
    return self;
}



#line 1 "tmpl/poly.c"
static void
iter_uint16_poly(na_loop_t *const lp)
#line 3 "tmpl/poly.c"
{
    size_t  i;
    dtype  x, y, a;

    x = *(dtype*)(lp->args[0].ptr + lp->iter[0].pos);
    i = lp->narg - 2;
    y = *(dtype*)(lp->args[i].ptr + lp->iter[i].pos);
    for (; --i;) {
        y = m_mul(x,y);
        a = *(dtype*)(lp->args[i].ptr + lp->iter[i].pos);
        y = m_add(y,a);
    }
    i = lp->narg - 1;
    *(dtype*)(lp->args[i].ptr + lp->iter[i].pos) = y;
}

/*
  Polynomial.: a0 + a1*x + a2*x**2 + a3*x**3 + ... + an*x**n
  @overload poly a0, a1, ...
  @param [NArray,Numeric] a0
  @param [NArray,Numeric] a1 , ...
  @return [NArray::UInt16]
*/
static VALUE
nary_uint16_poly(VALUE self, VALUE args)
#line 28 "tmpl/poly.c"
{
    int argc, i;
    VALUE *argv;
    volatile VALUE v, a;
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { iter_uint16_poly, NO_LOOP, 0, 1, 0, aout };

    argc = RARRAY_LEN(args);
    ndf.nin = argc+1;
    ndf.ain = ALLOCA_N(ndfunc_arg_in_t,argc+1);
    for (i=0; i<argc+1; i++) {
        ndf.ain[i].type = cT;
    }
    argv = ALLOCA_N(VALUE,argc+1);
    argv[0] = self;
    for (i=0; i<argc; i++) {
        argv[i+1] = RARRAY_PTR(args)[i];
    }
    a = rb_ary_new4(argc+1, argv);
    v = na_ndloop2(&ndf, a);
    return nary_uint16_extract(v);
}



#line 1 "tmpl/qsort.c"
/*
  qsort.c
  Numerical Array Extension for Ruby
    (C) Copyright 2007-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

/*
 *      qsort.c: standard quicksort algorithm
 *
 *      Modifications from vanilla NetBSD source:
 *        Add do ... while() macro fix
 *        Remove __inline, _DIAGASSERTs, __P
 *        Remove ill-considered "swap_cnt" switch to insertion sort,
 *        in favor of a simple check for presorted input.
 *
 *      CAUTION: if you change this file, see also qsort_arg.c
 *
 *      $PostgreSQL: pgsql/src/port/qsort.c,v 1.12 2006/10/19 20:56:22 tgl Exp $
 */

/*      $NetBSD: qsort.c,v 1.13 2003/08/07 16:43:42 agc Exp $   */

/*-
 * Copyright (c) 1992, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *        may be used to endorse or promote products derived from this software
 *        without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.      IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef QSORT_INCL
#define QSORT_INCL
#define Min(x, y)               ((x) < (y) ? (x) : (y))

#define swap(type,a,b) \
    do {type tmp=*(type*)(a); *(type*)(a)=*(type*)(b); *(type*)(b)=tmp;} while(0)

#define vecswap(type, a, b, n) if ((n)>0) swap(type,(a),(b))

#define MED3(a,b,c)                                     \
    (cmpgt(b,a) ?                                       \
     (cmpgt(c,b) ? b : (cmpgt(c,a) ? c : a))            \
     : (cmpgt(b,c) ? b : (cmpgt(c,a) ? a : c)))
#endif

#undef qsort_dtype
#define qsort_dtype dtype
#undef qsort_cast
#define qsort_cast *(dtype*)

void
uint16_qsort(void *a, size_t n, ssize_t es)
#line 79 "tmpl/qsort.c"
{
    char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
    int  d, r, presorted;

 loop:
    if (n < 7) {
        for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
            for (pl = pm; pl > (char *) a && cmpgt(pl - es, pl);
                 pl -= es)
                swap(qsort_dtype, pl, pl - es);
        return;
    }
    presorted = 1;
    for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es) {
        if (cmpgt(pm - es, pm)) {
            presorted = 0;
            break;
        }
    }
    if (presorted)
        return;
    pm = (char *) a + (n / 2) * es;
    if (n > 7) {
        pl = (char *) a;
        pn = (char *) a + (n - 1) * es;
        if (n > 40) {
            d = (n / 8) * es;
            pl = MED3(pl, pl + d, pl + 2 * d);
            pm = MED3(pm - d, pm, pm + d);
            pn = MED3(pn - 2 * d, pn - d, pn);
        }
        pm = MED3(pl, pm, pn);
    }
    swap(qsort_dtype, a, pm);
    pa = pb = (char *) a + es;
    pc = pd = (char *) a + (n - 1) * es;
    for (;;) {
        while (pb <= pc && (r = cmp(pb, a)) <= 0) {
            if (r == 0) {
                swap(qsort_dtype, pa, pb);
                pa += es;
            }
            pb += es;
        }
        while (pb <= pc && (r = cmp(pc, a)) >= 0) {
            if (r == 0) {
                swap(qsort_dtype, pc, pd);
                pd -= es;
            }
            pc -= es;
        }
        if (pb > pc)
            break;
        swap(qsort_dtype, pb, pc);
        pb += es;
        pc -= es;
    }
    pn = (char *) a + n * es;
    r = Min(pa - (char *) a, pb - pa);
    vecswap(qsort_dtype, a, pb - r, r);
    r = Min(pd - pc, pn - pd - es);
    vecswap(qsort_dtype, pb, pn - r, r);
    if ((r = pb - pa) > es)
        uint16_qsort(a, r / es, es);
#line 143 "tmpl/qsort.c"
    if ((r = pd - pc) > es) {
        a = pn - r;
        n = r / es;
        goto loop;
    }
}




#line 1 "tmpl/sort.c"
static void
iter_uint16_sort(na_loop_t *const lp)
#line 3 "tmpl/sort.c"
{
    size_t i, n;
    char *ptr;
    ssize_t step;
    size_t *idx;
    dtype *buf;

    INIT_COUNTER(lp, n);
    INIT_PTR_IDX(lp, 0, ptr, step, idx);
    if (idx) {
        buf = (dtype*)(lp->opt_ptr);
        for (i=0; i<n; i++) {
            buf[i] = *(dtype*)(ptr+idx[i]);
        }
        uint16_qsort(buf, n, sizeof(dtype));
#line 18 "tmpl/sort.c"
        for (i=0; i<n; i++) {
            *(dtype*)(ptr+idx[i]) = buf[i];
        }
    } else {
        uint16_qsort(ptr, n, step);
#line 23 "tmpl/sort.c"
    }
}

/*
  Returns sorted narray.
  @overload sort
  @return [NArray::UInt16] sorted narray.
*/
static VALUE
nary_uint16_sort(int argc, VALUE *argv, VALUE self)
#line 33 "tmpl/sort.c"
{
    return na_sort_main(argc, argv, self, iter_uint16_sort);
}



#line 1 "tmpl/qsort.c"
/*
  qsort.c
  Numerical Array Extension for Ruby
    (C) Copyright 2007-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

/*
 *      qsort.c: standard quicksort algorithm
 *
 *      Modifications from vanilla NetBSD source:
 *        Add do ... while() macro fix
 *        Remove __inline, _DIAGASSERTs, __P
 *        Remove ill-considered "swap_cnt" switch to insertion sort,
 *        in favor of a simple check for presorted input.
 *
 *      CAUTION: if you change this file, see also qsort_arg.c
 *
 *      $PostgreSQL: pgsql/src/port/qsort.c,v 1.12 2006/10/19 20:56:22 tgl Exp $
 */

/*      $NetBSD: qsort.c,v 1.13 2003/08/07 16:43:42 agc Exp $   */

/*-
 * Copyright (c) 1992, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *        may be used to endorse or promote products derived from this software
 *        without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.      IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef QSORT_INCL
#define QSORT_INCL
#define Min(x, y)               ((x) < (y) ? (x) : (y))

#define swap(type,a,b) \
    do {type tmp=*(type*)(a); *(type*)(a)=*(type*)(b); *(type*)(b)=tmp;} while(0)

#define vecswap(type, a, b, n) if ((n)>0) swap(type,(a),(b))

#define MED3(a,b,c)                                     \
    (cmpgt(b,a) ?                                       \
     (cmpgt(c,b) ? b : (cmpgt(c,a) ? c : a))            \
     : (cmpgt(b,c) ? b : (cmpgt(c,a) ? a : c)))
#endif

#undef qsort_dtype
#define qsort_dtype dtype*
#undef qsort_cast
#define qsort_cast **(dtype**)

void
uint16_index_qsort(void *a, size_t n, ssize_t es)
#line 79 "tmpl/qsort.c"
{
    char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
    int  d, r, presorted;

 loop:
    if (n < 7) {
        for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
            for (pl = pm; pl > (char *) a && cmpgt(pl - es, pl);
                 pl -= es)
                swap(qsort_dtype, pl, pl - es);
        return;
    }
    presorted = 1;
    for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es) {
        if (cmpgt(pm - es, pm)) {
            presorted = 0;
            break;
        }
    }
    if (presorted)
        return;
    pm = (char *) a + (n / 2) * es;
    if (n > 7) {
        pl = (char *) a;
        pn = (char *) a + (n - 1) * es;
        if (n > 40) {
            d = (n / 8) * es;
            pl = MED3(pl, pl + d, pl + 2 * d);
            pm = MED3(pm - d, pm, pm + d);
            pn = MED3(pn - 2 * d, pn - d, pn);
        }
        pm = MED3(pl, pm, pn);
    }
    swap(qsort_dtype, a, pm);
    pa = pb = (char *) a + es;
    pc = pd = (char *) a + (n - 1) * es;
    for (;;) {
        while (pb <= pc && (r = cmp(pb, a)) <= 0) {
            if (r == 0) {
                swap(qsort_dtype, pa, pb);
                pa += es;
            }
            pb += es;
        }
        while (pb <= pc && (r = cmp(pc, a)) >= 0) {
            if (r == 0) {
                swap(qsort_dtype, pc, pd);
                pd -= es;
            }
            pc -= es;
        }
        if (pb > pc)
            break;
        swap(qsort_dtype, pb, pc);
        pb += es;
        pc -= es;
    }
    pn = (char *) a + n * es;
    r = Min(pa - (char *) a, pb - pa);
    vecswap(qsort_dtype, a, pb - r, r);
    r = Min(pd - pc, pn - pd - es);
    vecswap(qsort_dtype, pb, pn - r, r);
    if ((r = pb - pa) > es)
        uint16_index_qsort(a, r / es, es);
#line 143 "tmpl/qsort.c"
    if ((r = pd - pc) > es) {
        a = pn - r;
        n = r / es;
        goto loop;
    }
}




#line 1 "tmpl/sort_index.c"
/*
  Returns index narray of sort.
  @overload sort_index
  @return [NArray::UInt16] index narray of sort.
*/
static VALUE
nary_uint16_sort_index(int argc, VALUE *argv, VALUE self)
#line 8 "tmpl/sort_index.c"
{
    return na_sort_index_main(argc, argv, self, uint16_index_qsort);
}



#line 1 "tmpl/median.c"
static void
iter_uint16_median(na_loop_t *const lp)
#line 3 "tmpl/median.c"
{
    size_t i, n;
    char *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype *buf;

    INIT_COUNTER(lp, n);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    buf = (dtype*)(lp->opt_ptr);
    if (idx1) {
        for (i=0; i<n; i++) {
            buf[i] = *(dtype*)(p1+idx1[i]);
        }
    } else {
        for (i=0; i<n; i++) {
            buf[i] = *(dtype*)(p1+s1*i);
        }
    }
    uint16_qsort(buf, n, sizeof(dtype));
#line 24 "tmpl/median.c"
    for (; n; n--) {
        if (!isnan(buf[n-1])) break;
    }
    if (n==0) {
        *(dtype*)p2 = buf[0];
    }
    else if (n%2==0) {
        *(dtype*)p2 = (buf[n/2-1]+buf[n/2])/2;
    }
    else {
        *(dtype*)p2 = buf[(n-1)/2];
    }
}

static VALUE
nary_uint16_median(int argc, VALUE *argv, VALUE self)
#line 40 "tmpl/median.c"
{
    VALUE v;
    v = na_median_main(argc, argv, self, iter_uint16_median);
    return nary_uint16_extract(v);
}



#line 223 "dtype.erb.c"
void
Init_nary_uint16()
{
    volatile VALUE hCast;

    cT = rb_define_class_under(cNArray, "UInt16", cNArray);
    
    

    rb_define_const(cT, ELEMENT_BIT_SIZE,  INT2FIX(sizeof(dtype)*8));
    rb_define_const(cT, ELEMENT_BYTE_SIZE, INT2FIX(sizeof(dtype)));
    rb_define_const(cT, CONTIGUOUS_STRIDE, INT2FIX(sizeof(dtype)));

    rb_define_singleton_method(cT, "[]", nary_uint16_s_cast, -2);

    
    rb_define_method(cT, "allocate", nary_uint16_allocate, 0);
    rb_define_method(cT, "extract", nary_uint16_extract, 0);
    rb_define_method(cT, "store", nary_uint16_store, 1);
    rb_define_singleton_method(cT, "cast", nary_uint16_s_cast, 1);
    rb_define_method(cT, "coerce_cast", nary_uint16_coerce_cast, 1);
    rb_define_method(cT, "to_a", nary_uint16_to_a, 0);
    rb_define_method(cT, "fill", nary_uint16_fill, 1);
    rb_define_method(cT, "format", nary_uint16_format, -1);
    rb_define_method(cT, "format_to_a", nary_uint16_format_to_a, -1);
    rb_define_method(cT, "inspect", nary_uint16_inspect, 0);
    rb_define_method(cT, "abs", nary_uint16_abs, 0);
    rb_define_method(cT, "+", nary_uint16_add, 1);
    rb_define_method(cT, "-", nary_uint16_sub, 1);
    rb_define_method(cT, "*", nary_uint16_mul, 1);
    rb_define_method(cT, "/", nary_uint16_div, 1);
    rb_define_method(cT, "%", nary_uint16_mod, 1);
    rb_define_method(cT, "divmod", nary_uint16_divmod, 1);
    rb_define_method(cT, "**", nary_uint16_pow, 1);
    rb_define_method(cT, "-@", nary_uint16_minus, 0);
    rb_define_method(cT, "inverse", nary_uint16_inverse, 0);
    rb_define_alias(cT, "conj", "copy");
    rb_define_alias(cT, "im", "copy");
    rb_define_alias(cT, "conjugate", "conj");
    rb_define_method(cT, "eq", nary_uint16_eq, 1);
    rb_define_method(cT, "ne", nary_uint16_ne, 1);
    rb_define_alias(cT, "nearly_eq", "eq");
    rb_define_alias(cT, "close_to", "nearly_eq");
    rb_define_method(cT, "&", nary_uint16_bit_and, 1);
    rb_define_method(cT, "|", nary_uint16_bit_or, 1);
    rb_define_method(cT, "^", nary_uint16_bit_xor, 1);
    rb_define_method(cT, "~", nary_uint16_bit_not, 0);
    rb_define_alias(cT, "floor", "copy");
    rb_define_alias(cT, "round", "copy");
    rb_define_alias(cT, "ceil", "copy");
    rb_define_method(cT, "gt", nary_uint16_gt, 1);
    rb_define_method(cT, "ge", nary_uint16_ge, 1);
    rb_define_method(cT, "lt", nary_uint16_lt, 1);
    rb_define_method(cT, "le", nary_uint16_le, 1);
    rb_define_alias(cT, ">", "gt");
    rb_define_alias(cT, ">=", "ge");
    rb_define_alias(cT, "<", "lt");
    rb_define_alias(cT, "<=", "le");
    rb_define_method(cT, "sum", nary_uint16_sum, -1);
    rb_define_method(cT, "min", nary_uint16_min, -1);
    rb_define_method(cT, "max", nary_uint16_max, -1);
    rb_define_method(cT, "seq", nary_uint16_seq, -1);
    rb_define_alias(cT, "indgen", "seq");
    rb_define_method(cT, "rand", nary_uint16_rand, 0);
    rb_define_method(cT, "poly", nary_uint16_poly, -2);
    rb_define_method(cT, "sort", nary_uint16_sort, -1);
    rb_define_method(cT, "sort_index", nary_uint16_sort_index, -1);
    rb_define_method(cT, "median", nary_uint16_median, -1);
#line 244 "dtype.erb.c"

    hCast = rb_hash_new();
    rb_define_const(cT, "UPCAST", hCast);
    rb_hash_aset(hCast, rb_cArray,   cT);
    
    rb_hash_aset(hCast, rb_cFixnum, cT);
    rb_hash_aset(hCast, rb_cBignum, cT);
    rb_hash_aset(hCast, rb_cFloat, cDFloat);
    rb_hash_aset(hCast, rb_cComplex, cDComplex);
    rb_hash_aset(hCast, cRObject, cRObject);
    rb_hash_aset(hCast, cDComplex, cDComplex);
    rb_hash_aset(hCast, cSComplex, cSComplex);
    rb_hash_aset(hCast, cDFloat, cDFloat);
    rb_hash_aset(hCast, cSFloat, cSFloat);
    rb_hash_aset(hCast, cInt64, cInt64);
    rb_hash_aset(hCast, cInt32, cInt32);
    rb_hash_aset(hCast, cInt16, cInt16);
    rb_hash_aset(hCast, cInt8, cInt16);
    rb_hash_aset(hCast, cUInt64, cUInt64);
    rb_hash_aset(hCast, cUInt32, cUInt32);
    rb_hash_aset(hCast, cUInt16, cT);
    rb_hash_aset(hCast, cUInt8, cT);
#line 250 "dtype.erb.c"
}
