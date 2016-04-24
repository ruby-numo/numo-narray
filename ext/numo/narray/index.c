/*
  index.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
//#define NARRAY_C

#include <string.h>
#include <ruby.h>
#include "numo/narray.h"
#include "numo/template.h"

typedef struct {
    size_t  n;
    size_t  beg;
    ssize_t step;
    size_t *idx;
    int     reduce;
    int     orig_dim;
} na_index_arg_t;


static void
print_index_arg(na_index_arg_t *q, int n)
{
    int i;
    printf("na_index_arg_t = 0x%"SZF"x {\n",(size_t)q);
    for (i=0; i<n; i++) {
        printf("  q[%d].n=%"SZF"d\n",i,q[i].n);
        printf("  q[%d].beg=%"SZF"d\n",i,q[i].beg);
        printf("  q[%d].step=%"SZF"d\n",i,q[i].step);
        printf("  q[%d].idx=0x%"SZF"x\n",i,(size_t)q[i].idx);
        printf("  q[%d].reduce=0x%x\n",i,q[i].reduce);
        printf("  q[%d].orig_dim=%d\n",i,q[i].orig_dim);
    }
    printf("}\n");
}

static VALUE sym_ast;
static VALUE sym_all;
//static VALUE sym_reduce;
static VALUE sym_minus;
static VALUE sym_new;
static VALUE sym_reverse;
static VALUE sym_plus;
static VALUE sym_sum;
static VALUE sym_tilde;
static VALUE sym_rest;
static VALUE id_beg;
static VALUE id_end;
static VALUE id_exclude_end;

static int
na_index_preprocess(VALUE args, int na_ndim)
{
    int i, count_new=0, count_rest=0;
    VALUE a;

    for (i=0; i<RARRAY_LEN(args); i++) {
        a = rb_ary_entry(args, i);

        switch (TYPE(a)) {

        case T_SYMBOL:
            if (a==sym_new || a==sym_minus) {
                RARRAY_ASET(args, i, sym_new);
                count_new++;
            }
            if (a==sym_rest || a==sym_tilde) {
                RARRAY_ASET(args, i, Qfalse);
                count_rest++;
            }
        }
    }

    if (count_rest>1)
        rb_raise(rb_eIndexError, "multiple rest-dimension is not allowd");

    if (count_rest==0 && count_new==0 && i==1)
        return 0;

    if (count_rest==0 && i-count_new != na_ndim)
        rb_raise(rb_eIndexError, "# of index=%i != narray.ndim=%i",
                 i-count_new, na_ndim);

    return count_new;
}


void
na_index_set_step(na_index_arg_t *q, int i, size_t n, size_t beg, ssize_t step)
{
    q->n    = n;
    q->beg  = beg;
    q->step = step;
    q->idx  = NULL;
    q->reduce = 0;
    q->orig_dim = i;
}


void
na_index_set_scalar(na_index_arg_t *q, int i, ssize_t size, ssize_t x)
{
    if (x < -size || x >= size)
        rb_raise(rb_eRangeError,
                  "array index (%"SZF"d) is out of array size (%"SZF"d)",
                  x, size);
    if (x < 0)
        x += size;
    q->n    = 1;
    q->beg  = x;
    q->step = 0;
    q->idx  = NULL;
    q->reduce = 0;
    q->orig_dim = i;
}


static void
na_index_parse_each(volatile VALUE a, ssize_t size, int i, na_index_arg_t *q)
{
    int k;
    ssize_t beg, end, step, n, x;
    size_t *idx;

    switch(TYPE(a)) {

    case T_FIXNUM:
        na_index_set_scalar(q,i,size,FIX2LONG(a));
        break;

    case T_BIGNUM:
        na_index_set_scalar(q,i,size,NUM2SSIZE(a));
        break;

    case T_FLOAT:
        na_index_set_scalar(q,i,size,NUM2SSIZE(a));
        break;

    case T_NIL:
    case T_TRUE:
        na_index_set_step(q,i,size,0,1);
        break;

    case T_SYMBOL:
        if (a==sym_all || a==sym_ast) {
            na_index_set_step(q,i,size,0,1);
        }
        else if (a==sym_reverse) {
            na_index_set_step(q,i,size,size-1,-1);
        }
        else if (a==sym_new) {
            na_index_set_step(q,i,1,0,1);
        }
        else if (a==sym_reduce || a==sym_sum || a==sym_plus) {
            na_index_set_step(q,i,size,0,1);
            q->reduce = 1;
        }
        break;

    case T_ARRAY:
        n = RARRAY_LEN(a);
        idx = ALLOC_N(size_t, n);
        for (k=0; k<n; k++) {
            x = NUM2SIZE(RARRAY_PTR(a)[k]);
            // range check
            if (x < -size || x >= size)
                rb_raise(rb_eRangeError,
                          " array index[%d]=%lu is out of array size (%ld)",
                          k, x, size);
            if (x < 0)
                x += size;
            idx[k] = x;
        }
        q->n    = n;
        q->beg  = 0;
        q->step = 1;
        q->idx  = idx;
        q->reduce = 0;
        q->orig_dim = i;
        break;

    default:
        // Range object
        if (rb_obj_is_kind_of(a, rb_cRange)) {
            step = 1;

            beg = NUM2LONG(rb_funcall(a,id_beg,0));
            if (beg<0) {
                beg += size;
            }

            end = NUM2LONG(rb_funcall(a,id_end,0));
            if (end<0) {
                end += size;
            }

            if (RTEST(rb_funcall(a,id_exclude_end,0))) {
                end--;
            }
            if (beg < -size || beg >= size ||
                 end < -size || end >= size) {
                rb_raise(rb_eRangeError,
                          "beg=%ld,end=%ld is out of array size (%ld)",
                          beg, end, size);
            }
            n = end-beg+1;
            if (n<0) n=0;
            na_index_set_step(q,i,n,beg,step);
        }
        // Num::Step Object
        else if (rb_obj_is_kind_of(a, na_cStep)) {
            nary_step_array_index(a, size, (size_t*)(&n), &beg, &step);
            na_index_set_step(q,i,n,beg,step);
        } else {
            rb_raise(rb_eIndexError, "not allowed type");
        }
        // write me

        /*
        // NArray index
        if (NA_IsNArray(a)) {
        GetNArray(a,na);
        size = na_ary_to_index(na,shape,sl);
        } else
        */

    }
}


static size_t
na_index_parse_args(VALUE args, narray_t *na, na_index_arg_t *q, int nd)
{
    int i, j, k, l, nidx;
    size_t total=1;
    VALUE *idx;

    nidx = RARRAY_LEN(args);
    idx = RARRAY_PTR(args);

    for (i=j=k=0; i<nidx; i++) {
        // rest dimension
        if (idx[i]==Qfalse) {
            for (l = nd - (nidx-1); l>0; l--) {
                na_index_parse_each(Qtrue, na->shape[k], k, &q[j]);
                if (q[j].n > 1) {
                    total *= q[j].n;
                }
                j++;
                k++;
            }
        }
        // new dimension
        else if (idx[i]==sym_new) {
            na_index_parse_each(idx[i], 1, k, &q[j]);
            j++;
        }
        // other dimention
        else {
            na_index_parse_each(idx[i], na->shape[k], k, &q[j]);
            if (q[j].n > 1) {
                total *= q[j].n;
            }
            j++;
            k++;
        }
    }
    return total;
}


static void
na_index_aref_nadata(narray_data_t *na1, narray_view_t *na2,
                     na_index_arg_t *q, ssize_t elmsz, int ndim, int keep_dim)
{
    int i, j;
    ssize_t size, k, total=1;
    ssize_t stride1;
    ssize_t *stride;
    size_t  *index;
    ssize_t beg, step;
    VALUE m;
    //stridx_t sdx2;

    stride = ALLOC_N(ssize_t, na1->base.ndim);

    i = na1->base.ndim - 1;
    stride[i] = elmsz;
    for (; i>0; i--) {
        stride[i-1] = stride[i] * na1->base.shape[i];
    }

    for (i=j=0; i<ndim; i++) {
        stride1 = stride[q[i].orig_dim];

        // numeric index -- trim dimension
        if (!keep_dim && q[i].n==1 && q[i].step==0) {
            beg  = q[i].beg;
            na2->offset += stride1 * beg;
            continue;
        }

        na2->base.shape[j] = size = q[i].n;

        if (q[i].reduce != 0) {
            m = rb_funcall(INT2FIX(1),rb_intern("<<"),1,INT2FIX(j));
            na2->base.reduce = rb_funcall(m,rb_intern("|"),1,na2->base.reduce);
        }

        // array index
        if (q[i].idx != NULL) {
            index = q[i].idx;
            SDX_SET_INDEX(na2->stridx[j],index);
            for (k=0; k<size; k++) {
                index[k] = index[k] * stride1;
            }
        } else {
            beg  = q[i].beg;
            step = q[i].step;
            na2->offset += stride1*beg;
            SDX_SET_STRIDE(na2->stridx[j], stride1*step);
        }
        j++;
        total *= size;
    }
    na2->base.size = total;
    xfree(stride);
}


static void
na_index_aref_naview(narray_view_t *na1, narray_view_t *na2,
                   na_index_arg_t *q, int ndim, int keep_dim)
{
    int i, j;
    ssize_t size, k, total=1;
    size_t  last;
    ssize_t stride1;
    ssize_t beg, step;
    size_t *index;
    VALUE m;
    stridx_t sdx1;

    for (i=j=0; i<ndim; i++) {

        sdx1 = na1->stridx[q[i].orig_dim];

        // numeric index -- trim dimension
        if (!keep_dim && q[i].n==1 && q[i].step==0) {
            if (SDX_IS_INDEX(sdx1)) {
                na2->offset += SDX_GET_INDEX(sdx1)[q[i].beg];
            } else {
                na2->offset += SDX_GET_STRIDE(sdx1)*q[i].beg;
            }
            continue;
        }

        na2->base.shape[j] = size = q[i].n;

        if (q[i].reduce != 0) {
            m = rb_funcall(INT2FIX(1),rb_intern("<<"),1,INT2FIX(j));
            na2->base.reduce = rb_funcall(m,rb_intern("|"),1,na2->base.reduce);
        }

        // array index
        if (q[i].idx != NULL) {
            index = q[i].idx;
            SDX_SET_INDEX(na2->stridx[j],index);

            if (SDX_IS_INDEX(sdx1)) {
                // index <- index
                for (k=0; k<size; k++) {
                    index[k] = SDX_GET_INDEX(sdx1)[index[k]];
                }
            }
            else {
                // index <- step
                stride1 = SDX_GET_STRIDE(sdx1);
                if (stride1<0) {
                    stride1 = -stride1;
                    last = na1->base.shape[q[i].orig_dim] - 1;
                    if (na2->offset < last * stride1) {
                        rb_raise(rb_eStandardError,"bug: negative offset");
                    }
                    na2->offset -= last * stride1;
                    for (k=0; k<size; k++) {
                        index[k] = (last - index[k]) * stride1;
                    }
                } else {
                    for (k=0; k<size; k++) {
                        index[k] = index[k] * stride1;
                    }
                }
            }
        } else {
            beg  = q[i].beg;
            step = q[i].step;
            // step <- index
            if (SDX_IS_INDEX(sdx1)) {
                index = ALLOC_N(size_t, size);
                SDX_SET_INDEX(na2->stridx[j],index);
                for (k=0; k<size; k++) {
                    index[k] = SDX_GET_INDEX(sdx1)[beg+step*k];
                }
            }
            else {
                // step <- step
                stride1 = SDX_GET_STRIDE(sdx1);
                na2->offset += stride1*beg;
                SDX_SET_STRIDE(na2->stridx[j], stride1*step);
            }
        }
        j++;
        total *= size;
    }
    na2->base.size = total;
}


VALUE
na_aref_md(int argc, VALUE *argv, VALUE self, int keep_dim)
{
    VALUE view, args;
    narray_t *na1;
    narray_view_t *na2;
    int i, nd, ndim, count_new;
    na_index_arg_t *q;
    ssize_t elmsz;

    GetNArray(self,na1);

    //printf("argc=%d\n",argc);

    args = rb_ary_new4(argc,argv);

    count_new = na_index_preprocess(args, na1->ndim);

    if (RARRAY_LEN(args)==1) {
        // fix me
        self = na_flatten(self);
        GetNArray(self,na1);
    }
    ndim = na1->ndim + count_new;
    q = ALLOCA_N(na_index_arg_t, ndim);
    na_index_parse_args(args, na1, q, ndim);

    if (na_debug_flag) print_index_arg(q,ndim);

    if (keep_dim) {
        nd = ndim;
    } else {
        for (i=nd=0; i<ndim; i++) {
            if (q[i].n>1 || q[i].step!=0) {
                nd++;
            }
        }
    }

    view = na_s_allocate_view(CLASS_OF(self));

    na_copy_flags(self, view);
    GetNArrayView(view,na2);

    na_alloc_shape((narray_t*)na2, nd);

    na2->stridx = ALLOC_N(stridx_t,nd);

    switch(na1->type) {
    case NARRAY_DATA_T:
    case NARRAY_FILEMAP_T:
        elmsz = na_get_elmsz(self);
        na_index_aref_nadata((narray_data_t *)na1,na2,q,elmsz,ndim,keep_dim);
        na2->data = self;
        break;
    case NARRAY_VIEW_T:
        na_index_aref_naview((narray_view_t *)na1,na2,q,ndim,keep_dim);
        na2->data = ((narray_view_t *)na1)->data;
        break;
    }

    return view;
}




/* method: [](idx1,idx2,...,idxN) */
VALUE
na_aref_main(int nidx, VALUE *idx, VALUE self, int keep_dim)
{
    na_index_arg_to_internal_order(nidx, idx, self);

    if (nidx==0) {
        return na_copy(self);
    }
    if (nidx==1) {
      if (CLASS_OF(*idx)==numo_cBit) {
        rb_funcall(*idx,rb_intern("mask"),1,self);
      }
    }
    return na_aref_md(nidx, idx, self, keep_dim);
}


/* method: [](idx1,idx2,...,idxN) */
static VALUE na_aref(int argc, VALUE *argv, VALUE self)
{
    VALUE view;
    view = na_aref_main(argc, argv, self, 0);
    return rb_funcall(view, rb_intern("extract"), 0);
}


/* method: slice(idx1,idx2,...,idxN) */
static VALUE na_slice(int argc, VALUE *argv, VALUE self)
{
    return na_aref_main(argc, argv, self, 1);
}




/* method: []=(idx1,idx2,...,idxN,val) */
static VALUE
na_aset(int argc, VALUE *argv, VALUE self)
{
    VALUE a;
    argc--;

    if (argc==0)
        na_store(self, argv[argc]);
    else {
        a = na_aref_main(argc, argv, self, 0);
        na_store(a, argv[argc]);
    }
    return argv[argc];
}


// convert reduce dims to 0-th element
// for initialization of min/max func
// ['*,+,*'] -> [true,0,true]
VALUE nary_init_accum_aref0(VALUE self, VALUE reduce)
{
    narray_t *na;
    VALUE a;
    ID id_bra;
    unsigned long m;
    int i, ndim;

    GetNArray(self,na);
    ndim = na->ndim;
    a = rb_ary_new();
    if (FIXNUM_P(reduce)) {
        m = NUM2ULONG(reduce);
        if (m==0)
            for (i=0; i<ndim; i++)
                rb_ary_push(a,INT2FIX(0));
        else
            for (i=0; i<ndim; i++)
                if ((m>>i) & 1u)
                    rb_ary_push(a,INT2FIX(0));
                else
                    rb_ary_push(a,Qtrue);
    } else {
        id_bra = rb_intern("[]");
        for (i=0; i<ndim; i++)
            if (rb_funcall(reduce,id_bra,1,INT2FIX(i)) == INT2FIX(1))
                rb_ary_push(a,INT2FIX(0));
            else
                rb_ary_push(a,Qtrue);
    }
    return na_aref_md(RARRAY_LEN(a), RARRAY_PTR(a), self, 0);
}


static inline ssize_t
na_range_check(ssize_t pos, ssize_t size, int dim)
{
    ssize_t idx=pos;

    if (idx < 0) idx += size;
    if (idx < 0 || idx >= size) {
        rb_raise(rb_eIndexError, "index=%"SZF"d out of shape[%d]=%"SZF"d",
                 pos, dim, size);
    }
    return idx;
}


ssize_t
na_get_scalar_position(VALUE self, int argc, VALUE *argv, ssize_t stride)
{
    int i;
    ssize_t x, s, m, pos, *idx;
    narray_t *na;
    narray_view_t *nv;
    stridx_t sdx;

    GetNArray(self,na);
    if (na->size == 0) {
        rb_raise(rb_eRuntimeError, "cannot get index of empty array");
        return -1;
    }
    if (argc != 1 && argc != na->ndim) {
        return -1;
    }
    idx = ALLOCA_N(ssize_t, argc);
    for (i=0; i<argc; i++) {
        switch(TYPE(argv[i])) {
        case T_FIXNUM:
            idx[i] = FIX2LONG(argv[i]);
            break;
        case T_BIGNUM:
        case T_FLOAT:
            idx[i] = NUM2SSIZE(argv[i]);
            break;
        default:
            return -1;
        }
    }
    switch(na->type) {
    case NARRAY_VIEW_T:
        GetNArrayView(self,nv);
        pos = nv->offset;
        if (argc==1) {
            x = na_range_check(idx[0], na->size, 0);
            for (i=na->ndim-1; i>=0; i--) {
                s = na->shape[i];
                m = x % s;
                x = x / s;
                sdx = nv->stridx[i];
                if (SDX_IS_INDEX(sdx)) {
                    pos += SDX_GET_INDEX(sdx)[m];
                } else {
                    pos += SDX_GET_STRIDE(sdx)*m;
                }
            }
        } else {
            for (i=argc-1; i>=0; i--) {
                x = na_range_check(idx[i], na->shape[i], i);
                sdx = nv->stridx[i];
                if (SDX_IS_INDEX(sdx)) {
                    pos += SDX_GET_INDEX(sdx)[x];
                } else {
                    pos += SDX_GET_STRIDE(sdx)*x;
                }
            }
        }
        break;
    default:
        if (!stride) {
            stride = na_get_elmsz(self);
        }
        if (argc==1) {
            x = na_range_check(idx[0], na->size, 0);
            pos = stride*x;
        } else {
            pos = 0;
            for (i=argc-1; i>=0; i--) {
                x = na_range_check(idx[i], na->shape[i], i);
                pos += stride*x;
                stride *= na->shape[i];
            }
        }
    }
    return pos;
}


void
Init_nary_index()
{
    rb_define_method(cNArray, "[]", na_aref, -1);
    rb_define_method(cNArray, "slice", na_slice, -1);
    rb_define_method(cNArray, "[]=", na_aset, -1);

    sym_ast = ID2SYM(rb_intern("*"));
    sym_all = ID2SYM(rb_intern("all"));
    sym_minus = ID2SYM(rb_intern("-"));
    sym_new = ID2SYM(rb_intern("new"));
    sym_reverse = ID2SYM(rb_intern("reverse"));
    sym_plus = ID2SYM(rb_intern("+"));
    //sym_reduce = ID2SYM(rb_intern("reduce"));
    sym_sum = ID2SYM(rb_intern("sum"));
    sym_tilde = ID2SYM(rb_intern("~"));
    sym_rest = ID2SYM(rb_intern("rest"));
    id_beg = rb_intern("begin");
    id_end = rb_intern("end");
    id_exclude_end = rb_intern("exclude_end?");
}
