/*
  ndloop.h
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#ifndef NDLOOP_H
#define NDLOOP_H

typedef struct NA_LOOP_ARGS {
    VALUE    value;
    char    *ptr;
    ssize_t  elmsz;
} na_loop_args_t;

typedef struct NA_LOOP_ITER {
    ssize_t    pos;
    ssize_t    step;
    size_t    *idx;
} na_loop_iter_t;

/*
typedef struct NA_LOOP_ITER2 {
    union {
        na_loop_iter_t i;
        VALUE a;
    };
} na_loop_iter2_t;
*/

typedef struct NA_LOOP {
    int  narg;
    int  ndim;             // n of user dimention
    size_t *n;             // n of elements for each dim
    na_loop_args_t *args;  // for each arg
    na_loop_iter_t *iter;  // for each dim, each arg
    //VALUE  opt_val;
    VALUE  info;
    void  *opt_ptr;
} na_loop_t;

typedef struct NA_MD_LOOP {
    int  narg;
    int  ndim;             // n of total dimention
    size_t *n;             // n of elements for each dim
    na_loop_args_t *args;  // for each arg
    na_loop_iter_t *iter;  // for each dim, each arg
    na_loop_t  user;       // loop in user function
    VALUE  mark;
} na_md_loop_t;


#define LITER(lp,idim,iarg) ((lp)->iter[(idim)*((lp)->narg)+(iarg)])

// ------------------ ndfunc -------------------------------------------

#define NDF_CONTIGUOUS_LOOP     (1<<0) // x[i]
#define NDF_STRIDE_LOOP         (1<<1) // *(x+stride*i)
#define NDF_INDEX_LOOP          (1<<2) // *(x+idx[i])
#define NDF_FULL_LOOP (NDF_CONTIGUOUS_LOOP|NDF_STRIDE_LOOP|NDF_INDEX_LOOP)
#define NDF_KEEP_DIM            (1<<3)
#define NDF_ACCEPT_SWAP         (1<<4)

#define NDF_HAS_MARK_DIM        (1<<5)

#define HAS_LOOP       NDF_FULL_LOOP
#define FULL_LOOP      NDF_FULL_LOOP
#define NO_LOOP        0
#define HAS_MARK       NDF_HAS_MARK_DIM

#define NDF_TEST(nf,fl)  ((nf)->flag&(fl))
#define NDF_SET(nf,fl)  {(nf)->flag |= (fl);}

#define NDF_ARG_READ_ONLY   1
#define NDF_ARG_WRITE_ONLY  2
#define NDF_ARG_READ_WRITE  3

// type of user function
typedef void (*na_iter_func_t) _((na_loop_t *const));
typedef VALUE (*na_text_func_t) _((char *ptr, size_t pos, VALUE opt));

// spec of arguments passed to user function
typedef struct NDFUNC_ARG {
    VALUE type;    // argument types
    VALUE init;    // initial value
    int dim;       // # of dimension of argument handled by user function
    //int flag;    // flag for read/write
    union {
        size_t shape[1];
        size_t *shape_p;
    } aux;         // shape
} ndfunc_arg_t;

// spec of user function
typedef struct NDFUNCTION {
    //char *name;
    na_iter_func_t func; // user function
    unsigned int flag;   // what kind of loop user function supports
    int narg;            // # of arguments
    int nopt;            // # of options
    int nres;            // # of results
    ndfunc_arg_t *args;  // spec of arguments
    VALUE *opt_types;    // option types
} ndfunc_t;


#define NDF_TEST(nf,fl)  ((nf)->flag&(fl))
#define NDF_SET(nf,fl)  {(nf)->flag |= (fl);}

#endif /* NDLOOP_H */
