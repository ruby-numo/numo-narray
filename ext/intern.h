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

//VALUE na_sort_main(int argc, VALUE *argv, volatile VALUE self, void (*func_qsort)());
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

stridx_t *na_get_stride(VALUE v);

void na_release_lock(VALUE);

VALUE nary_s_upcast(VALUE type1, VALUE type2);


void na_index_arg_to_internal_order(int argc, VALUE *argv, VALUE self);



ndfunc_t *ndfunc_alloc(na_iter_func_t func, int has_loop, int narg, int nres, ...);
ndfunc_t *ndfunc_alloc2(na_iter_func_t func, unsigned int flag, int narg, int nres, VALUE *etypes);
void ndfunc_free(ndfunc_t* nf);

VALUE ndloop_cast_narray_to_rarray(ndfunc_t *nf, VALUE nary, VALUE fmt);;

VALUE ndloop_do(ndfunc_t *nf, int argc, ...);
VALUE ndloop_do2(ndfunc_t *nf, VALUE args);
VALUE ndloop_do3(ndfunc_t *nf, void *ptr, int argc, ...);
VALUE ndloop_do4(ndfunc_t *nf, void *ptr, VALUE args);

/*
VALUE ndfunc_execute_reduce(ndfunc_t *nf, VALUE reduce, int argc, ...);
VALUE ndfunc_execute_to_rarray(ndfunc_t *nf, VALUE arg, VALUE info);
//VALUE ndfunc_execute_from_rarray(ndfunc_t *nf, VALUE arg);
void ndloop_execute_store_array(ndfunc_t *nf, VALUE rary, VALUE nary);
void ndfunc_debug_print(VALUE ary, na_simple_func_t func, VALUE opt);
//void ndfunc_execute_io(VALUE ary, na_simple_func_t func, VALUE io);
//VALUE ndfunc_execute_to_text(VALUE ary, na_text_func_t func, VALUE opt);
void ndfunc_execute_inspect(VALUE ary, VALUE str, na_text_func_t func, VALUE opt);
*/

//void ndfunc_cast_array_to_narray(ndfunc_t *nf, VALUE rary, VALUE nary);
void ndloop_cast_rarray_to_narray(ndfunc_t *nf, VALUE rary, VALUE nary);


VALUE na_info_str(VALUE);

size_t na_get_elmsz(VALUE);

boolean na_test_reduce(VALUE reduce, int dim);

size_t *na_mdarray_investigate(VALUE ary, int *ndim, VALUE *type);


//void na_copy_bytes(na_loop_t *const itr);


VALUE na_debug_set(VALUE mod, VALUE flag);


void nary_step_array_index(VALUE self, size_t ary_size, size_t *plen, ssize_t *pbeg, ssize_t *pstep);


VALUE na_store(VALUE self, VALUE src);

void nary_step_sequence(VALUE self, size_t *plen, double *pbeg, double *pstep);

void ndloop_do_inspect(VALUE ary, VALUE buf, na_text_func_t func, VALUE opt);

VALUE na_upcast(VALUE type1, VALUE type2);

VALUE nary_init_accum_aref0(VALUE self, VALUE reduce);





#if 0
void rand_norm(double *a);



VALUE rb_narray_single_dim_view(VALUE self);




int na_parse_dimension(int argc, VALUE *argv, int dimc, int *dimv, int flag);




void nary_step_sequence(VALUE self, size_t *plen, double *pbeg, double *pstep);

VALUE nary_type_s_upcast(VALUE type1, VALUE type2);


void rb_scan_kw_args __((VALUE, ...));

VALUE na_aref(int argc, VALUE *argv, VALUE self);
VALUE na_aref_md(int argc, VALUE *argv, VALUE self, int keep_dim);
VALUE nary_init_accum_aref0(VALUE self, VALUE reduce);

int n_bits(u_int64_t a);

VALUE nary_pointer_new(VALUE a1);
VALUE nary_pointer_index(VALUE ptr, void *beg, ssize_t step, VALUE idxclass);
#endif

#endif /* ifndef INTERN_H */
