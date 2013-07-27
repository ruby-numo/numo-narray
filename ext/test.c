#include <ruby.h>

typedef void (*na_iter_func_t) _((void *const));

// spec of arguments passed to user function
typedef struct NDFUNC_ARG {
    VALUE   type;    // argument types
    int     dim;     // # of dimension of argument handled by user function
    VALUE   init;
    size_t *shape;
} ndfunc_arg_t;


typedef struct NDF_ARG_IN {
    VALUE   type;    // argument types
    int     dim;     // # of dimension of argument handled by user function
} ndf_arg_in_t;

typedef struct NDF_ARG_OUT {
    VALUE   type;    // argument types
    VALUE   init;
    int     dim;     // # of dimension of argument handled by user function
    size_t *shape;
} ndf_arg_out_t;

// spec of user function
typedef struct NDFUNCTION {
    na_iter_func_t func; // user function
    unsigned int flag;   // what kind of loop user function supports
    int nin;             // # of arguments
    int nout;            // # of results
    ndf_arg_in_t *ain;   // spec of input arguments
    ndf_arg_out_t *aout; // spec of output result
    void *option;        // option types
} ndfunc_t;

VALUE cT = Qnil;


/*
static VALUE
na_ndloop_v(ndfunc_arg_t *ndf, int argc, VALUE *argv)
{
    int narg;

    narg = ndf->nin + ndf->nout;
    if (narg != argc) {
        rb_raise(rb_eArgError,"wrong number of arguments (%d for %d)", argc, min);
    }
}
*/



static void
iter_dfloat_add(void *const lp)
{
}

#if 0
// Rubyメソッドに対応するC関数を定義
static VALUE
_nary_dfloat_s_add_v(VALUE mod, int argc, VALUE *argv)
{
    size_t shape[2];
    VALUE  opts[2];
    ndfunc_arg_t argt[3] = {/*in*/{cT,0},{cT,0},/*out*/{cT,0}};
    ndfunc_t ndf = { iter_dfloat_add, HAS_LOOP, 2, 1, argt, opts };
    argt[2].init = Qnil;
    return na_ndloop_v(&ndf, argc, argv);
}
#endif

#define HAS_LOOP 0
// Rubyメソッドに対応するC関数を定義
static VALUE
_nary_dfloat_add(VALUE self, VALUE other)
{
    size_t shape[2];
    VALUE opts[2];
    ndf_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndf_arg_out_t aout[1] = {{cT,Qnil,0}};
    ndfunc_t ndf = { iter_dfloat_add, HAS_LOOP, 2, 1, ain, aout, opts };
    return na_ndloop(&ndf, self, other);
}

//return na_ndloop_reduce(&ndf, self, argc, argv);

ID id_cast;
ID id_add;

static VALUE
nary_dfloat_add(VALUE self, VALUE other)
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return _nary_dfloat_add(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, id_add, 1, other);
    }
}
