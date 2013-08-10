/*
  intern.h
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#ifndef INTERN_H
#define INTERN_H

VALUE rb_narray_new(VALUE elem, int ndim, size_t *shape);
VALUE rb_narray_debug_info(VALUE);

VALUE na_make_view(VALUE self);
VALUE na_reduce_dimension(int argc, VALUE *argv, VALUE self);

VALUE na_check_ladder(VALUE self, int start_dim);
void na_setup_shape(narray_t *na, int ndim, size_t *shape);

VALUE na_transpose_map(VALUE self, int *map);
VALUE na_flatten_dim(VALUE self, int sd);
VALUE na_flatten_by_reduce(int argc, VALUE *argv, VALUE self);

VALUE na_sort_main(int argc, VALUE *argv, volatile VALUE self, na_iter_func_t iter_func);
VALUE na_median_main(int argc, VALUE *argv, volatile VALUE self, na_iter_func_t iter_func);

VALUE na_sort_index_main(int argc, VALUE *argv, VALUE self, void (*func_qsort)());

char *na_get_pointer_for_write(VALUE);
char *na_get_pointer_for_read(VALUE);
char *na_get_pointer_at_origin_for_read(VALUE);

size_t na_get_offset(VALUE self);

VALUE na_s_allocate(VALUE klass);
VALUE na_s_allocate_view(VALUE klass);

void na_alloc_shape(narray_t *na, int ndim);
void na_alloc_index(narray_t *na);
void na_alloc_data(VALUE self);

void na_copy_flags(VALUE src, VALUE dst);

VALUE na_flatten(VALUE);
VALUE na_dup(VALUE);
VALUE na_copy(VALUE);
VALUE na_upcast(VALUE type1, VALUE type2);

stridx_t *na_get_stride(VALUE v);

void na_release_lock(VALUE);

void na_index_arg_to_internal_order(int argc, VALUE *argv, VALUE self);

//ndfunc_t *ndfunc_alloc(na_iter_func_t func, int has_loop, int narg, int nres, ...);
//ndfunc_t *ndfunc_alloc2(na_iter_func_t func, unsigned int flag, int narg, int nres, VALUE *etypes);
//void ndfunc_free(ndfunc_t* nf);

VALUE na_ndloop(ndfunc_t *nf, int argc, ...);
VALUE na_ndloop2(ndfunc_t *nf, VALUE args);
VALUE na_ndloop3(ndfunc_t *nf, void *ptr, int argc, ...);
VALUE na_ndloop4(ndfunc_t *nf, void *ptr, VALUE args);

VALUE na_ndloop_cast_narray_to_rarray(ndfunc_t *nf, VALUE nary, VALUE fmt);
VALUE na_ndloop_cast_rarray_to_narray(ndfunc_t *nf, VALUE rary, VALUE nary);
void  na_ndloop_inspect(VALUE nary, VALUE buf, na_text_func_t func, VALUE opt);

VALUE na_info_str(VALUE);

size_t na_get_elmsz(VALUE);

boolean na_test_reduce(VALUE reduce, int dim);

size_t *na_mdarray_investigate(VALUE ary, int *ndim, VALUE *type);

//void na_copy_bytes(na_loop_t *const itr);

VALUE na_debug_set(VALUE mod, VALUE flag);

void nary_step_array_index(VALUE self, size_t ary_size, size_t *plen, ssize_t *pbeg, ssize_t *pstep);

VALUE na_store(VALUE self, VALUE src);

void nary_step_sequence(VALUE self, size_t *plen, double *pbeg, double *pstep);

VALUE nary_init_accum_aref0(VALUE self, VALUE reduce);


#endif /* ifndef INTERN_H */
