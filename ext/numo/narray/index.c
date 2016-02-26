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
//#include <version.h>
#include "narray.h"
//#include "narray_local.h"
//#include "bytedata.h"

#include "template.h"

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

static ID id_ast;
static ID id_all;
//static ID id_minus;
static ID id_new;
static ID id_reverse;
static ID id_plus;
static ID id_sum;
static ID id_tilde;
static ID id_rest;
static ID id_beg;
static ID id_end;
static ID id_exclude_end;

struct StrSplit {
    long    n;
    char **ptr;
    long   *len;
};

static void
free_split(struct StrSplit *sp)
{
    //if (sp->ptr!=NULL) xfree(sp->ptr);
    //if (sp->len!=NULL) xfree(sp->len);
    xfree(sp);
}

static struct StrSplit *
str_split(char *s, long len, char sep)
{
    long i, c, n=1, alen;
    struct StrSplit *sp;

    for (i=0; i<len; i++) {
        if (s[i]==sep) {
            n++;
        }
    }
    //printf("sep='%c' n=%d s=\"%s\"\n",sep,n,s);

    sp = xmalloc(sizeof(struct StrSplit) + sizeof(char*)*n + sizeof(long)*n);
    sp->n = n;
    sp->ptr = (char**)((char*)sp + sizeof(struct StrSplit));
    sp->len = (long*)((char*)sp + sizeof(struct StrSplit) +  sizeof(char*)*n);

    sp->ptr[0] = &(s[0]);

    if (n==1) {
        sp->len[0] = len;
        return sp;
    }

    c = 1;
    alen = 0;
    for (i=0; i<len; i++) {
        if (s[i]==sep) {
            sp->ptr[c] = &(s[i+1]);
            sp->len[c-1] = alen;
            alen = 0;
            c++;
        } else {
            alen++;
        }
    }
    sp->len[c-1] = alen;
    return sp;
}


static void
str_trim(const char **s, long *n)
{
    if (*n==0) return;

    // trim prefix white space
    while ((*s)[0]==' ' || (*s)[0]=='\t' || (*s)[0]=='\n' || (*s)[0]=='\r') {
        (*s)++;
        (*n)--;
        if (*n==0) return;
    }

    // trim trailing white space
    while ((*s)[*n-1]==' ' || (*s)[*n-1]=='\t' || (*s)[*n-1]=='\n' || (*s)[*n-1]=='\r') {
        (*n)--;
        if (*n==0) return;
    }
}


static int
match_int(const char *s, long n)
{
    long i=0;

    str_trim(&s, &n);

    if (n==0) return 0;

    // sign
    if (s[i]=='+' || s[i]=='-') {
        if (++i==n) return 0;
    }

    // whitespace
    while (s[i]==' ' || s[i]=='\t' || s[i]=='\n' || s[i]=='\r') {
        if (++i==n) return 1;
    }

    // digit
    if (s[i]>='0' && s[i]<='9') {
        if (++i==n) return 1;
    } else {
        return 0;
    }
    while (s[i]>='0' && s[i]<='9') {
        if (++i==n) return 1;
    }

    /*
    // trailing whitespace
    while (s[i]==' ' || s[i]=='\t' || s[i]=='\n' || s[i]=='\r') {
        if (++i==n) return 1;
    }
    */
    return 0;
}

static int
match_all(const char *s, long n)
{
    str_trim(&s, &n);
    switch(n) {
    case 0:
        return 1;
    case 1:
        if (s[0]=='*') return 1;
        break;
    case 3:
        if (strncmp(s,"all",3)==0) return 1;
        break;
    }
    return 0;
}

static int
match_rev(const char *s, long n)
{
    str_trim(&s, &n);
    switch(n) {
    case 3:
        if (strncmp(s,"rev",3)==0) return 1;
        break;
    case 7:
        if (strncmp(s,"reverse",7)==0) return 1;
        break;
    }
    return 0;
}

static int
match_new(const char *s, long n)
{
    str_trim(&s, &n);
    switch(n) {
    case 1:
        if (s[0]=='-') return 1;
        break;
    case 3:
        if (strncmp(s,"new",3)==0) return 1;
        break;
    }
    return 0;
}

static int
match_rest(const char *s, long n)
{
    str_trim(&s, &n);
    switch(n) {
    case 1:
        if (s[0]=='~') return 1;
        break;
    case 4:
        if (strncmp(s,"rest",4)==0) return 1;
        break;
    }
    return 0;
}

static int
match_reduce(const char *s, long n)
{
    str_trim(&s, &n);
    switch(n) {
    case 1:
        if (s[0]=='+') return 1;
        break;
    case 3:
        if (strncmp(s,"sum",3)==0) return 1;
        break;
    case 4:
        if (strncmp(s,"reduce",4)==0) return 1;
        break;
    }
    return 0;
}

static long
strn2long(const char *s, long n)
{
    char *buf;
    long value;

    buf = xmalloc(sizeof(char)*(n+1));
    strncpy(buf, s, n);
    buf[n] = '\0';
    value = atol(buf);
    xfree(buf);
    return value;
}


static int
na_index_preprocess(VALUE args, int na_ndim)
{
    int i, j, count_new=0, count_rest=0;
    ID id;
    volatile VALUE a, ary;
    struct StrSplit *sp;

    ary = rb_ary_new();

    for (i=0; i<RARRAY_LEN(args);) {
        a = RARRAY_PTR(args)[i];

        switch (TYPE(a)) {

        case T_STRING:
            sp = str_split(RSTRING_PTR(a), RSTRING_LEN(a), ',');
            for (j=0; j<sp->n; j++) {
                if (match_new(sp->ptr[j],sp->len[j])) {
                    rb_ary_push(ary, ID2SYM(id_new));
                    count_new++;
                }
                else if (match_rest(sp->ptr[j],sp->len[j])) {
                    rb_ary_push(ary, Qfalse);
                    count_rest++;
                }
                else {
                    rb_ary_push(ary,rb_str_new(sp->ptr[j],sp->len[j]));
                }
            }
            i += sp->n; //RARRAY_LEN(a);
            free_split(sp);
            break;

        case T_SYMBOL:
            id = SYM2ID(a);
            if (id==id_new || id==id_minus) {
                //RARRAY_PTR(args)[i] = ID2SYM(id_new);
                a = ID2SYM(id_new);
                count_new++;
            }
            if (id==id_rest || id==id_tilde) {
                //RARRAY_PTR(args)[i] = Qfalse;
                a = Qfalse;
                count_rest++;
            }

            // though
        default:
            rb_ary_push(ary, a);
            i++;
        }
    }

    if (count_rest>1)
        rb_raise(rb_eIndexError, "multiple rest-dimension is not allowd");

    rb_ary_replace(args, ary);

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
    //int rest_dim=0;
    ID id;
    volatile VALUE tmp;

    //char *ptr;
    //long  len;
    struct StrSplit *sp;
    //int   step;
    //char *buf;
    char *s;
    long  l;

    //printf("type(a) = 0x%x\n",TYPE(a));

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
        id = SYM2ID(a);
        if (id==id_all || id==id_ast) {
            na_index_set_step(q,i,size,0,1);
        }
        else if (id==id_reverse) {
            na_index_set_step(q,i,size,size-1,-1);
        }
        else if (id==id_new) {
            na_index_set_step(q,i,1,0,1);
        }
        else if (id==id_reduce || id==id_sum || id==id_plus) {
            na_index_set_step(q,i,size,0,1);
            q->reduce = 1;
        }
        break;

    case T_ARRAY:
        n = RARRAY_LEN(a);
        idx = ALLOC_N(size_t, n);
        //printf("array size n =%ld\n",n);
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

    case T_STRING:
        //a = rb_funcall(a,rb_intern("strip"),0);
        //a = rb_funcall(a,rb_intern("split"),2,rb_str_new2(":"),INT2FIX(-1));
        //puts("t_string");

        sp = str_split(RSTRING_PTR(a), RSTRING_LEN(a), ':');
        step = 1;
        //printf("sp->n=%d\n",sp->n);
        switch (sp->n){
        case 3:
            s = sp->ptr[2];
            l = sp->len[2];
            if (match_all(s,l)) {
                step = 1;
            }
            else if (match_int(s,l)) {
                step = strn2long(s,l);
                if (step==0) {
                    free_split(sp);
                    rb_raise(rb_eIndexError,"step must be non-zero");
                }
            }
            else {
                tmp = rb_str_new(s,l);
                free_split(sp);
                rb_raise(rb_eIndexError,"invalid step in colon range '%s'",
                         StringValuePtr(tmp));
            }
            //printf("step=%d\n",step);


        case 2:
            s = sp->ptr[1];
            l = sp->len[1];
            if (match_all(s,l)) {
                if (step>0)
                    end = -1;
                else
                    end = 0;
            }
            else if (match_int(s,l)) {
                end = strn2long(s,l);
            }
            else {
                tmp = rb_str_new(s,l);
                free_split(sp);
                rb_raise(rb_eIndexError,"invalid end in colon range '%s'",
                         StringValuePtr(tmp));
            }

            if (end<0) {
                end += size;
            }
            //printf("end=%d\n",end);

            s = sp->ptr[0];
            l = sp->len[0];
            if (match_all(s,l)) {
                if (step>0)
                    beg = 0;
                else
                    beg = -1;
            }
            else if (match_int(s,l)) {
                beg = strn2long(s,l);
            }
            else {
                tmp = rb_str_new(s,l);
                free_split(sp);
                rb_raise(rb_eIndexError,"invalid start in colon range '%s'",
                         StringValuePtr(tmp));
            }

            if (beg<0) {
                beg += size;
            }

            //printf("beg=%d\n",beg);

            if (beg < -size || beg >= size || end < -size || end >= size) {
                free_split(sp);
                rb_raise(rb_eRangeError,
                          "beg=%ld,end=%ld is out of array size (%ld)",
                          beg, end, size);
            }

            //n = (end-beg+1)/step;
            n = (end-beg)/step+1;
            if (n<0) n=0;
            //printf("n=%d beg=%ld end=%ld size=%ld", n, beg, end, size);

            na_index_set_step(q,i,n,beg,step);

            break;

        case 1:
            s = sp->ptr[0];
            l = sp->len[0];
            if (match_all(s,l)) {
                na_index_set_step(q,i,size,0,1);
            }
            else if (match_reduce(s,l)) {
                na_index_set_step(q,i,size,0,1);
                q->reduce = 1;
            }
            else if (match_rev(s,l)) {
                na_index_set_step(q,i,size,size-1,-1);
            }
            else if (match_int(s,l)) {
                beg = strn2long(s,l);
                na_index_set_scalar(q,i,size,beg);
            } else {
                tmp = rb_str_new(s,l);
                free_split(sp);
                rb_raise(rb_eIndexError,"invalid string argument '%s'",
                         StringValuePtr(tmp));
            }
            break;

        case 0:
            // all
            na_index_set_step(q,i,size,0,1);
            break;

        default:
            free_split(sp);
            rb_raise(rb_eIndexError, "too many colon");
        }
        free_split(sp);
        break;

    default:
        // Range object
        if (rb_obj_is_kind_of(a, rb_cRange)) {
            step = 1;

            //puts("pass0");

            //beg = NUM2LONG(rb_ivar_get(a, id_beg));
            beg = NUM2LONG(rb_funcall(a,id_beg,0));
            if (beg<0) {
                beg += size;
            }

            //puts("pass1");

            //end = NUM2LONG(rb_ivar_get(a, id_end));
            end = NUM2LONG(rb_funcall(a,id_end,0));
            if (end<0) {
                end += size;
            }

            //puts("pass2");

            if (RTEST(rb_funcall(a,id_exclude_end,0))) {
                end--;
            }

            //puts("pass3");

            if (beg < -size || beg >= size ||
                 end < -size || end >= size) {
                rb_raise(rb_eRangeError,
                          "beg=%ld,end=%ld is out of array size (%ld)",
                          beg, end, size);
            }
            n = end-beg+1;
            if (n<0) n=0;

            //puts("pass");

            na_index_set_step(q,i,n,beg,step);
        }
        // Num::Step Object
        else if (rb_obj_is_kind_of(a, na_cStep)) {
            if (rb_obj_is_kind_of(a, rb_cRange) || rb_obj_is_kind_of(a, na_cStep)) {

                nary_step_array_index(a, size, (size_t*)(&n), &beg, &step);
            /*
            a = nary_step_parameters(a, ULONG2NUM(size));
            beg  = NUM2LONG(RARRAY_PTR(a)[0]);
            step = NUM2LONG(RARRAY_PTR(a)[1]);
            n    = NUM2LONG(RARRAY_PTR(a)[2]);
            */
                na_index_set_step(q,i,n,beg,step);
            }

        // write me

        /*
        // NArray index
        if (NA_IsNArray(a)) {
        GetNArray(a,na);
        size = na_ary_to_index(na,shape,sl);
        } else
        */
            else {
                rb_raise(rb_eIndexError, "not allowed type");
            }
        }
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
        else if ((TYPE(idx[i])==T_SYMBOL) && (SYM2ID(idx[i])==id_new)) {
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
        //sdx2 = na2->stridx[j];
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
    stridx_t sdx1/*, sdx2*/;

    for (i=j=0; i<ndim; i++) {

        sdx1 = na1->stridx[q[i].orig_dim];
        //sdx2 = na2->stridx[j];

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
            //if (na1->index) {
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
static VALUE
na_aref_main(int nidx, VALUE *idx, VALUE self, int keep_dim)
{
    na_index_arg_to_internal_order(nidx, idx, self);

    if (nidx==0) {
        return na_copy(self);
    }
    if (nidx==1) {
      if (CLASS_OF(*idx)==cBit) {
        rb_funcall(*idx,rb_intern("mask"),1,self);
      }
    }
    return na_aref_md(nidx, idx, self, keep_dim);
}


/* method: [](idx1,idx2,...,idxN) */
VALUE na_aref(int argc, VALUE *argv, VALUE self)
{
    VALUE view;
    view = na_aref_main(argc, argv, self, 0);
    //return view;
    return rb_funcall(view, rb_intern("extract"), 0);
}


/* method: slice(idx1,idx2,...,idxN) */
VALUE na_slice(int argc, VALUE *argv, VALUE self)
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
        //a = na_aref(argc, argv, self);
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



void
Init_nary_index()
{
    rb_define_method(cNArray, "[]", na_aref, -1);
    rb_define_method(cNArray, "slice", na_slice, -1);
    rb_define_method(cNArray, "[]=", na_aset, -1);

    id_ast = rb_intern("*");
    id_all = rb_intern("all");
    id_minus = rb_intern("-");
    id_new = rb_intern("new");
    id_reverse = rb_intern("reverse");
    id_plus = rb_intern("+");
    //id_reduce = rb_intern("reduce");
    id_sum = rb_intern("sum");
    id_tilde = rb_intern("~");
    id_rest = rb_intern("rest");
    id_beg = rb_intern("begin");
    id_end = rb_intern("end");
    id_exclude_end = rb_intern("exclude_end?");
}
