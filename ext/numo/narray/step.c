/*
  step.c
  Numerical Array Extension for Ruby
    (C) Copyright 2007,2013 by Masahiro TANAKA
*/
#include <ruby.h>
#include <math.h>

#include "numo/narray.h"

#if defined(__FreeBSD__) && __FreeBSD__ < 4
#include <floatingpoint.h>
#endif

#ifdef HAVE_FLOAT_H
#include <float.h>
#endif

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#ifndef DBL_EPSILON
#define DBL_EPSILON 2.2204460492503131e-16
#endif

static ID id_beg, id_end, id_len, id_step, id_excl;

//#define EXCL(r) RTEST(rb_ivar_get((r), id_excl))
#define EXCL(r) RTEST(rb_funcall((r), rb_intern("exclude_end?"), 0))

#ifndef HAVE_RB_ARITHMETIC_SEQUENCE_EXTRACT
#define SET_EXCL(r,v) rb_ivar_set((r), id_excl, (v) ? Qtrue : Qfalse)

static void
step_init(
  VALUE self,
  VALUE beg,
  VALUE end,
  VALUE step,
  VALUE len,
  VALUE excl
)
{
    if (RTEST(len)) {
        if (!(FIXNUM_P(len) || TYPE(len)==T_BIGNUM)) {
            rb_raise(rb_eArgError, "length must be Integer");
        }
        if (RTEST(rb_funcall(len,rb_intern("<"),1,INT2FIX(0)))) {
            rb_raise(rb_eRangeError,"length must be non negative");
        }
    }
    rb_ivar_set(self, id_beg, beg);
    rb_ivar_set(self, id_end, end);
    rb_ivar_set(self, id_len, len);
    rb_ivar_set(self, id_step, step);
    SET_EXCL(self, excl);
}

static VALUE
nary_step_new2(
  VALUE range,
  VALUE step,
  VALUE len
)
{
    VALUE beg, end, excl;
    VALUE self = rb_obj_alloc(na_cStep);

    //beg = rb_ivar_get(range, id_beg);
    beg = rb_funcall(range, id_beg, 0);
    //end = rb_ivar_get(range, id_end);
    end = rb_funcall(range, id_end, 0);
    excl = rb_funcall(range, rb_intern("exclude_end?"), 0);

    step_init(self, beg, end, step, len, excl);
    return self;
}


/*
 *  call-seq:
 *     Step.new(start, end, step=nil, length=nil)    => step
 *     Step.new(range, step=nil, length=nil)         => step
 *
 *  Constructs a step using three parameters among <i>start</i>,
 *  <i>end</i>, <i>step</i> and <i>length</i>.  <i>start</i>,
 *  <i>end</i> parameters can be replaced with <i>range</i>.  If the
 *  <i>step</i> is omitted (or supplied with nil), then calculated
 *  from <i>length</i> or definded as 1.
 */

static VALUE
step_initialize( int argc, VALUE *argv, VALUE self )
{
    VALUE a, b=Qnil, c=Qnil, d=Qnil, e=Qnil;

    rb_scan_args(argc, argv, "13", &a, &b, &c, &d);
    /* Selfs are immutable, so that they should be initialized only once. */
    if (rb_ivar_defined(self, id_beg)) {
        rb_name_error(rb_intern("initialize"), "`initialize' called twice");
    }
    if (rb_obj_is_kind_of(a,rb_cRange)) {
        if (argc>3) {
            rb_raise(rb_eArgError, "extra argument");
        }
        d = c;
        c = b;
        e = rb_funcall(a, rb_intern("exclude_end?"), 0);
        //b = rb_ivar_get(a, id_end);
        b = rb_funcall(a, id_end, 0);
        //a = rb_ivar_get(a, id_beg);
        a = rb_funcall(a, id_beg, 0);
    }
    step_init(self, a, b, c, d, e);
    return Qnil;
}

/*
 *  call-seq:
 *     step.begin  => obj
 *     step.first  => obj
 *
 *  Returns the start of <i>step</i>.
 */

static VALUE
step_first( VALUE self )
{
    return rb_ivar_get(self, id_beg);
}

/*
 *  call-seq:
 *     step.end    => obj
 *     step.last   => obj
 *
 *  Returns the object that defines the end of <i>step</i>.
 */

static VALUE
step_last( VALUE self )
{
    return rb_ivar_get(self, id_end);
}

/*
 *  call-seq:
 *     step.length  => obj
 *     step.size    => obj
 *
 *  Returns the length of <i>step</i>.
 */

static VALUE
step_length( VALUE self )
{
    return rb_ivar_get(self, id_len);
}

/*
 *  call-seq:
 *     step.step    => obj
 *
 *  Returns the step of <i>step</i>.
 */

static VALUE
step_step( VALUE self )
{
    return rb_ivar_get(self, id_step);
}

/*
 *  call-seq:
 *     step.exclude_end?    => true or false
 *
 *  Returns <code>true</code> if <i>step</i> excludes its end value.
 */
static VALUE
step_exclude_end_p(VALUE self)
{
    return RTEST(rb_ivar_get(self, id_excl)) ? Qtrue : Qfalse;
}
#endif


/*
 *  call-seq:
 *     step.parameters([array_size])    => [start,step,length]
 *
 *  Returns the iteration parameters of <i>step</i>.  If
 *  <i>array_sizse</i> is given, negative array index is considered.
 */

void
nary_step_array_index(VALUE self, size_t ary_size,
                      size_t *plen, ssize_t *pbeg, ssize_t *pstep)
{
    size_t len;
    ssize_t beg=0, step=1;
    VALUE vbeg, vend, vstep, vlen;
    ssize_t end=ary_size;

    //vbeg = rb_ivar_get(self, id_beg);
    //vend = rb_ivar_get(self, id_end);
    vlen = rb_ivar_get(self, id_len);
    vstep = rb_ivar_get(self, id_step);
    vbeg = rb_funcall(self, id_beg, 0);
    vend = rb_funcall(self, id_end, 0);
    //vlen = rb_funcall(self, id_len, 0);
    //vstep = rb_funcall(self, id_step, 0);

    if (RTEST(vbeg)) {
        beg = NUM2SSIZET(vbeg);
        if (beg<0) {
            beg += ary_size;
        }
    }
    if (RTEST(vend)) {
        end = NUM2SSIZET(vend);
        if (end<0) {
            end += ary_size;
        }
    }

    //puts("pass 1");

    if (RTEST(vlen)) {
        len = NUM2SIZET(vlen);
        if (len>0) {
            if (RTEST(vstep)) {
                step = NUM2SSIZET(step);
                if (RTEST(vbeg)) {
                    if (RTEST(vend)) {
                        rb_raise( rb_eStandardError, "verbose Step object" );
                    } else {
                        end = beg + step*(len-1);
                    }
                } else {
                    if (RTEST(vend)) {
                        if (EXCL(self)) {
                            if (step>0) end--;
                            if (step<0) end++;
                        }
                        beg = end - step*(len-1);
                    } else {
                        beg = 0;
                        end = step*(len-1);
                    }
                }
            } else { // no step
                step = 1;
                if (RTEST(vbeg)) {
                    if (RTEST(vend)) {
                        if (EXCL(self)) {
                            if (beg<end) end--;
                            if (beg>end) end++;
                        }
                        if (len>1)
                            step = (end-beg)/(len-1);
                    } else {
                        end = beg + (len-1);
                    }
                } else {
                    if (RTEST(vend)) {
                        if (EXCL(self)) {
                            end--;
                        }
                        beg = end - (len-1);
                    } else {
                        beg = 0;
                        end = len-1;
                    }
                }
            }
        }
    } else { // no len
        if (RTEST(vstep)) {
            step = NUM2SSIZET(vstep);
        } else {
            step = 1;
        }
        if (step>0) {
            if (!RTEST(vbeg)) {
                beg = 0;
            }
            if (!RTEST(vend)) {
                end = ary_size-1;
            }
            else if (EXCL(self)) {
                end--;
            }
            if (beg<=end) {
                len = (end-beg)/step+1;
            } else {
                len = 0;
            }
        } else if (step<0) {
            if (!RTEST(vbeg)) {
                beg = ary_size-1;
            }
            if (!RTEST(vend)) {
                end = 0;
            }
            else if (EXCL(self)) {
                end++;
            }
            if (beg>=end) {
                len = (beg-end)/(-step)+1;
            } else {
                len = 0;
            }
        } else {
            rb_raise( rb_eStandardError, "step must be non-zero" );
        }
    }

    //puts("pass 2");

    if (beg<0 || beg>=(ssize_t)ary_size ||
        end<0 || end>=(ssize_t)ary_size) {
        rb_raise( rb_eRangeError,
                  "beg=%"SZF"d,end=%"SZF"d is out of array size (%"SZF"u)",
                  beg, end, ary_size );
    }
    if (plen) *plen = len;
    if (pbeg) *pbeg = beg;
    if (pstep) *pstep = step;
}


void
nary_step_sequence( VALUE self, size_t *plen, double *pbeg, double *pstep )
{
    VALUE vbeg, vend, vstep, vlen;
    double dbeg, dend, dstep=1, dsize, err;
    size_t size, n;

#ifdef HAVE_RB_ARITHMETIC_SEQUENCE_EXTRACT
    rb_arithmetic_sequence_components_t x;
    rb_arithmetic_sequence_extract(self, &x);

    vbeg = x.begin;
#else
    //vbeg = rb_ivar_get(self, id_beg);
    vbeg = rb_funcall(self, id_beg, 0);
#endif
    dbeg = NUM2DBL(vbeg);

#ifdef HAVE_RB_ARITHMETIC_SEQUENCE_EXTRACT
    vend = x.end;
#else
    //vend = rb_ivar_get(self, id_end);
    vend = rb_funcall(self, id_end, 0);
#endif

    vlen = rb_ivar_get(self, id_len);
#ifdef HAVE_RB_ARITHMETIC_SEQUENCE_EXTRACT
    vstep = x.step;
#else
    vstep = rb_ivar_get(self, id_step);
#endif
    //vlen  = rb_funcall(self, id_len ,0);
    //vstep = rb_funcall(self, id_step,0);

    if (RTEST(vlen)) {
        size = NUM2SIZET(vlen);

        if (!RTEST(vstep)) {
            if (RTEST(vend)) {
                dend = NUM2DBL(vend);
                if (EXCL(self)) {
                    n = size;
                } else {
                    n = size-1;
                }
                if (n>0) {
                    dstep = (dend-dbeg)/n;
                } else {
                    dstep = 1;
                }
            } else {
                dstep = 1;
            }
        }
    } else {
        if (!RTEST(vstep)) {
            dstep = 1;
        } else {
            dstep = NUM2DBL(vstep);
        }
        if (RTEST(vend)) {
            dend = NUM2DBL(vend);
            err = (fabs(dbeg)+fabs(dend)+fabs(dend-dbeg))/fabs(dstep)*DBL_EPSILON;
            if (err>0.5) err=0.5;
            dsize = (dend-dbeg)/dstep;
            if (EXCL(self))
                dsize -= err;
            else
                dsize += err;
            dsize = floor(dsize) + 1;
            if (dsize<0) dsize=0;
            if (isinf(dsize) || isnan(dsize)) {
                rb_raise(rb_eArgError, "not finite size");
            }
            size = dsize;
        } else {
            rb_raise(rb_eArgError, "cannot determine length argument");
        }
    }

    if (plen) *plen = size;
    if (pbeg) *pbeg = dbeg;
    if (pstep) *pstep = dstep;
}

#ifndef HAVE_RB_ARITHMETIC_SEQUENCE_EXTRACT
/*
static VALUE
step_each( VALUE self )
{
    VALUE  a;
    double beg, step;
    size_t i, size;

    a = nary_step_parameters( self, Qnil );
    beg  = NUM2DBL(RARRAY_PTR(a)[0]);
    step = NUM2DBL(RARRAY_PTR(a)[1]);
    size = NUM2SIZET(RARRAY_PTR(a)[2]);

    for (i=0; i<size; i++) {
        rb_yield(rb_float_new(beg+i*step));
    }
    return self;
}
*/

static VALUE
range_with_step( VALUE range, VALUE step )
{
    return nary_step_new2( range, step, Qnil );
}

static VALUE
range_with_length( VALUE range, VALUE len )
{
    return nary_step_new2( range, Qnil, len );
}


static VALUE
nary_s_step( int argc, VALUE *argv, VALUE mod )
{
    VALUE self = rb_obj_alloc(na_cStep);
    step_initialize(argc, argv, self);
    return self;
}


void
Init_nary_step()
{
    na_cStep = rb_define_class_under(cNArray, "Step", rb_cObject);
    rb_include_module(na_cStep, rb_mEnumerable);
    rb_define_method(na_cStep, "initialize", step_initialize, -1);

    //rb_define_method(na_cStep, "each", step_each, 0);

    rb_define_method(na_cStep, "first", step_first, 0);
    rb_define_method(na_cStep, "last", step_last, 0);
    rb_define_method(na_cStep, "begin", step_first, 0);
    rb_define_method(na_cStep, "end", step_last, 0);
    rb_define_method(na_cStep, "step", step_step, 0);
    rb_define_method(na_cStep, "length", step_length, 0);
    rb_define_method(na_cStep, "size", step_length, 0);
    rb_define_method(na_cStep, "exclude_end?", step_exclude_end_p, 0);
    //rb_define_method(na_cStep, "to_s", step_to_s, 0);
    //rb_define_method(na_cStep, "inspect", step_inspect, 0);
    //rb_define_method(na_cStep, "parameters", nary_step_parameters, 1);

    rb_define_method(rb_cRange, "%", range_with_step, 1);
    rb_define_method(rb_cRange, "*", range_with_length, 1);

    rb_define_singleton_method(cNArray, "step", nary_s_step, -1);

    id_beg  = rb_intern("begin");
    id_end  = rb_intern("end");
    id_len  = rb_intern("length");
    id_step = rb_intern("step");
    id_excl = rb_intern("excl");
}
#endif
