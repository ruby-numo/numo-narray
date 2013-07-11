/*
  narray.h
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2003 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#ifndef NARRAY_H
#define NARRAY_H

#include <math.h>
#include "compat.h"

//#include "narray_config.h"

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#define NARRAY_VERSION "0.9.0.1"
#define NARRAY_VERSION_CODE 901

/*
  Data types used in NArray :
  Please modify these types if your system has any different type.
*/
#if 0
#ifndef HAVE_U_INT8_T
typedef unsigned char          u_int8_t; /* NA_BYTE */
#endif

#ifndef HAVE_INT16_T
# if SIZEOF_SHORT == 2
typedef short                  int16_t;  /* NA_SINT */
# else
---->> Please define int16_t manually because sizeof(short) != 2. <<----
# endif
#endif /* HAVE_INT16_T */

#ifndef HAVE_INT32_T
# if SIZEOF_LONG == 4
typedef long                   int32_t;  /* NA_LINT */
# else
#  if SIZEOF_INT == 4
typedef int                    int32_t;  /* NA_LINT */
#  else
---->> Please define int32_t manually because sizeof(long) != 4. <<----
#  endif
# endif
#endif /* HAVE_INT32_T */

#ifndef HAVE_U_INT32_T || HAVE_UINT32_T
# if SIZEOF_LONG == 4
typedef unsigned long                   u_int32_t;  /* NA_LINT */
# else
#  if SIZEOF_INT == 4
typedef unsigned int                    u_int32_t;  /* NA_LINT */
#  else
---->> Please define u_int32_t manually because sizeof(long) != 4. <<----
#  endif
# endif
#endif /* HAVE_U_INT32_T */

#endif // 0

#ifndef HAVE_U_INT32_T
#ifdef HAVE_UINT32_T
  typedef uint32_t u_int32_t;
#endif
#endif

#ifndef HAVE_U_INT64_T
#ifdef HAVE_UINT64_T
  typedef uint64_t u_int64_t;
#endif
#endif



#if   SIZEOF_VOIDP==SIZEOF_LONG
#define NUM2SIZE(x) NUM2ULONG(x)
#define NUM2SSIZE(x) NUM2LONG(x)
#define SIZE2NUM(x) ULONG2NUM(x)
#define SSIZE2NUM(x) LONG2NUM(x)
#define SZF "l"
#elif SIZEOF_VOIDP==SIZEOF_LONG_LONG
#define NUM2SIZE(x) NUM2ULL(x)
#define NUM2SSIZE(x) NUM2LL(x)
#define SIZE2NUM(x) ULL2NUM(x)
#define SSIZE2NUM(x) LL2NUM(x)
#define SZF "ll"
#endif

#if   SIZEOF_LONG==8
#define NUM2INT64(x) NUM2LONG(x)
#define INT642NUM(x) LONG2NUM(x)
#define NUM2UINT64(x) NUM2ULONG(x)
#define UINT642NUM(x) ULONG2NUM(x)
#define INT64FMT "l"
#elif SIZEOF_LONG_LONG==8
#define NUM2INT64(x) NUM2LL(x)
#define INT642NUM(x) LL2NUM(x)
#define NUM2UINT64(x) NUM2ULL(x)
#define UINT642NUM(x) ULL2NUM(x)
#define INT64FMT "ll"
#endif

#if   SIZEOF_LONG==4
#define NUM2INT32(x) NUM2LONG(x)
#define INT322NUM(x) LONG2NUM(x)
#define NUM2UINT32(x) NUM2ULONG(x)
#define UINT322NUM(x) ULONG2NUM(x)
#define INT32FMT "l"
#elif SIZEOF_INT==4
#define NUM2INT32(x) NUM2INT(x)
#define INT322NUM(x) UINT2NUM(x)
#define NUM2UINT32(x) NUM2UINT(x)
#define UINT322NUM(x) UINT2NUM(x)
#define INT32FMT ""
#endif



//typedef struct { float r,i; }  scomplex;
//typedef struct { double r,i; } dcomplex;


#ifndef HAVE_TYPE_BOOLEAN
typedef int boolean;
#endif
#ifndef FALSE                   /* in case these macros already exist */
#define FALSE   0               /* values of boolean */
#endif
#ifndef TRUE
#define TRUE    1
#endif

typedef struct { float dat[2]; }  scomplex;
typedef struct { double dat[2]; } dcomplex;
typedef int fortran_integer;


extern int na_debug_flag;

#ifndef NARRAY_C
extern VALUE cNArray;
extern VALUE nary_eCastError;
extern VALUE nary_eShapeError;

//EXTERN const int na_sizeof[NA_NTYPES+1];
#endif


/* global variables within this module */
extern VALUE cNArray;
extern VALUE cBit;
extern VALUE cDFloat, cDComplex;
extern VALUE cSFloat, cSComplex;
extern VALUE cInt64, cInt32, cInt16, cInt8;
extern VALUE cUInt64, cUInt32, cUInt16, cUInt8;
extern VALUE cRObject;
extern VALUE na_cStep;
extern VALUE cComplex;
extern ID id_contiguous_stride;
extern ID id_element_bit_size;
extern ID id_element_byte_size;
extern ID id_contiguous_stride;

extern ID id_add;
extern ID id_sub;
extern ID id_mul;
extern ID id_div;
extern ID id_mod;
extern ID id_pow;
extern ID id_bit_and;
extern ID id_bit_or;
extern ID id_bit_xor;
extern ID id_eq;
extern ID id_nearly_eq;
extern ID id_ne;
extern ID id_gt;
extern ID id_ge;
extern ID id_lt;
extern ID id_le;

extern ID id_real;
extern ID id_imag;

extern ID id_mark;
extern ID id_info;
extern VALUE sym_mark;
extern VALUE sym_info;

#define NARRAY_DATA_T     0x1
#define NARRAY_VIEW_T     0x2
#define NARRAY_FILEMAP_T  0x3

typedef struct RNArray {
    unsigned char ndim;     // # of dimensions
    unsigned char type;
    unsigned char flag[2];  // flags
    size_t   size;          // # of total elements
    size_t  *shape;         // # of elements for each dimension
    VALUE    mark;
} narray_t;


typedef struct RNArrayData {
    narray_t base;
    char    *ptr;
} narray_data_t;


typedef union {
    ssize_t stride;
    size_t *index;
} stridx_t;

typedef struct RNArrayView {
    narray_t base;
    VALUE    data;       // data object
    size_t   offset;     // dataポインタから先頭データの位置へのオフセット
                         // : elm.unit_bits を単位とする
                         // address_unit  pointer_unit access_unit data_unit
                         // elm.step_unit = elm.bit_size / elm.access_unit
                         // elm.step_unit = elm.size_bits / elm.unit_bits
    stridx_t *stridx;    // stride or indices of data pointer for each dimension
} narray_view_t;


typedef struct RNArrayFileMap {
    narray_t base;
    char    *ptr;
#ifdef WIN32
    HANDLE hFile;
    HANDLE hMap;
#else // POSIX mmap
    int prot;
    int flag;
#endif
} narray_filemap_t;


static inline narray_t *
na_get_narray_t(VALUE obj)
{
    narray_t *na;

    Check_Type(obj, T_DATA);
    na = (narray_t*)DATA_PTR(obj);
    return na;
}

static inline narray_t *
_na_get_narray_t(VALUE obj, unsigned char na_type)
{
    narray_t *na;

    Check_Type(obj, T_DATA);
    na = (narray_t*)DATA_PTR(obj);
    if (na->type != na_type) {
	rb_bug("unknown type 0x%x (0x%x given)", na_type, na->type);
    }
    return na;
}

#define na_get_narray_data_t(obj) (narray_data_t*)_na_get_narray_t(obj,NARRAY_DATA_T)
#define na_get_narray_view_t(obj) (narray_view_t*)_na_get_narray_t(obj,NARRAY_VIEW_T)
#define na_get_narray_filemap_t(obj) (narray_filemap_t*)_na_get_narray_t(obj,NARRAY_FILEMAP_T)

#define GetNArray(obj,var)  Data_Get_Struct(obj, narray_t, var)
#define GetNArrayView(obj,var)  Data_Get_Struct(obj, narray_view_t, var)
#define GetNArrayData(obj,var)  Data_Get_Struct(obj, narray_data_t, var)

#define SDX_IS_STRIDE(x) ((x).stride&0x1)
#define SDX_IS_INDEX(x)  (!SDX_IS_STRIDE(x))
#define SDX_GET_STRIDE(x) ((x).stride>>1)
#define SDX_GET_INDEX(x)  ((x).index)

#define SDX_SET_STRIDE(x,s) ((x).stride=((s)<<1)|0x1)
#define SDX_SET_INDEX(x,idx) ((x).index=idx)

#define RNARRAY(val)		((narray_t*)DATA_PTR(val))
#define RNARRAY_DATA(val)	((narray_data_t*)DATA_PTR(val))
#define RNARRAY_VIEW(val)	((narray_view_t*)DATA_PTR(val))
#define RNARRAY_FILEMAP(val)	((narray_filemap_t*)DATA_PTR(val))

#define RNARRAY_NDIM(val)	(RNARRAY(val)->ndim)
#define RNARRAY_TYPE(val)	(RNARRAY(val)->type)
#define RNARRAY_FLAG(val)	(RNARRAY(val)->flag)
#define RNARRAY_SIZE(val)	(RNARRAY(val)->size)
#define RNARRAY_SHAPE(val)	(RNARRAY(val)->shape)
#define RNARRAY_MARK(val)	(RNARRAY(val)->mark)

#define RNARRAY_DATA_PTR(val)	 (RNARRAY_DATA(val)->ptr)
#define RNARRAY_VIEW_DATA(val)	 (RNARRAY_VIEW(val)->data)
#define RNARRAY_VIEW_OFFSET(val) (RNARRAY_VIEW(val)->offset)
#define RNARRAY_VIEW_STRIDX(val) (RNARRAY_VIEW(val)->stridx)

#define NA_NDIM(na)	(((narray_t*)na)->ndim)
#define NA_TYPE(na)	(((narray_t*)na)->type)
#define NA_FLAG(na)	(((narray_t*)na)->flag)
#define NA_FLAG0(na)	(((narray_t*)na)->flag[0])
#define NA_FLAG1(na)	(((narray_t*)na)->flag[1])
#define NA_SIZE(na)	(((narray_t*)na)->size)
#define NA_SHAPE(na)	(((narray_t*)na)->shape)
#define NA_MARK(na)	(((narray_t*)na)->mark)

#define NA_DATA_PTR(na)         (((narray_data_t*)na)->ptr)
#define NA_VIEW_DATA(na)	(((narray_view_t*)na)->data)
#define NA_VIEW_OFFSET(na)	(((narray_view_t*)na)->offset)
#define NA_VIEW_STRIDX(na)	(((narray_view_t*)na)->stridx)

#define NA_FILEMAP_PTR(na)	(((narray_filemap_t*)na)->ptr)


#define NA_FL0_TEST(x,f) (NA_FLAG0(x)&(f))
#define NA_FL1_TEST(x,f) (NA_FLAG1(x)&(f))

#define NA_FL0_SET(x,f) do {NA_FLAG0(x) |= (f);} while(0)
#define NA_FL1_SET(x,f) do {NA_FLAG1(x) |= (f);} while(0)

#define NA_FL0_UNSET(x,f) do {NA_FLAG0(x) &= ~(f);} while(0)
#define NA_FL1_UNSET(x,f) do {NA_FLAG1(x) &= ~(f);} while(0)

#define NA_FL0_REVERSE(x,f) do {NA_FLAG0(x) ^= (f);} while(0)
#define NA_FL1_REVERSE(x,f) do {NA_FLAG1(x) ^= (f);} while(0)

#define NA_TEST_LOCK(x)  NA_FL0_TEST(x,NA_FL_LOCK)
#define NA_SET_LOCK(x)   NA_FL0_SET(x,NA_FL_LOCK)
#define NA_UNSET_LOCK(x) NA_FL0_UNSET(x,NA_FL_LOCK)


/* FLAGS
   - row-major / column-major
   - Overwrite or not
   - byteswapp
   - Extensible?
   - matrix or not
*/

#define NA_FL_LOCK         (0x1<<0)
#define NA_FL_COLUMN_MAJOR (0x1<<1)
#define NA_FL_BYTE_SWAPPED (0x1<<2)
#define NA_FL_INPLACE      (0x1<<3)

#define TEST_COLUMN_MAJOR(x)   NA_FL0_TEST(x,NA_FL_COLUMN_MAJOR)
#define TEST_ROW_MAJOR(x)    (!NA_FL0_TEST(x,NA_FL_COLUMN_MAJOR))
#define TEST_BYTE_SWAPPED(x)   NA_FL0_TEST(x,NA_FL_BYTE_SWAPPED)
#define TEST_HOST_ORDER(x)   (!TEST_BYTE_SWAPPED(x))

#define TEST_INPLACE(x)        NA_FL0_TEST(x,NA_FL_INPLACE)
#define SET_INPLACE(x)         NA_FL0_SET(x,NA_FL_INPLACE)
#define UNSET_INPLACE(x)       NA_FL0_UNSET(x,NA_FL_INPLACE)

#define REVERSE_BYTE_SWAPPED(x)	NA_FL0_REVERSE((x),NA_FL_BYTE_SWAPPED)

#ifdef DYNAMIC_ENDIAN
#else
#ifdef WORDS_BIGENDIAN
#else // LITTLE_ENDIAN

#define TEST_NETWORK_ORDER(x) TEST_BYTE_SWAPPED(x)
#define TEST_VACS_ORDER(x)    TEST_HOST_ORDER(x)
#endif
#endif

/*
#define FL_NA_readonly / writable
#define FL_NA_resizable / extensible
#define FL_NA_matrix?
*/


/*
typedef struct NA_ITERATOR {
    size_t   n;
    size_t   pos;
    size_t   step;
    size_t   elmsz;
    size_t  *idx;
    char    *ptr;
} na_iterator_t;

//typedef void (*na_iter_func_t) _((na_iterator_t *const));
typedef void (*na_iter_func_t) _((na_iterator_t *const, VALUE info));

typedef void (*na_simple_func_t) _((char*ptr, size_t pos, VALUE opt));

typedef VALUE (*na_text_func_t) _((char*ptr, size_t pos, VALUE opt));
*/


/*
typedef struct NA_FUNC_ARG {
    VALUE type;
    VALUE init;
    int   dim;
    union {
	size_t shape[1];
	size_t *shape_p;
    } aux;
} na_func_arg_t;

typedef struct NDFUNCTION {
    //char *name;
    na_iter_func_t func;
    unsigned int flag;
    int narg;
    int nres;
    na_func_arg_t *args;
} ndfunc_t;


#define NDF_CONTIGUOUS_LOOP	(1<<0) // x[i]
#define NDF_STRIDE_LOOP		(1<<1) // *(x+stride*i)
#define NDF_INDEX_LOOP		(1<<2) // *(x+idx[i])

#define NDF_KEEP_DIM		(1<<3)
#define NDF_ACCEPT_SWAP		(1<<4)

#define NDF_FULL_LOOP (NDF_CONTIGUOUS_LOOP|NDF_STRIDE_LOOP|NDF_INDEX_LOOP)

#define HAS_LOOP       NDF_FULL_LOOP
//#define HAS_INDEX_LOOP 7
//#define HAS_STEP_LOOP  3
//#define HAS_CONTIGUOUS_LOOP 1
#define NO_LOOP	       0

#define NDF_TEST(nf,fl)  ((nf)->flag&(fl))
#define NDF_SET(nf,fl)  {(nf)->flag |= (fl);}
*/


#define IsNArray(obj) (rb_obj_is_kind_of(obj,cNArray)==Qtrue)
//#define IsNElement(obj) (rb_obj_is_kind_of(obj,cNElement)==Qtrue)
//#define IsNdfunction(obj) (rb_obj_is_kind_of(obj,cNdfunction)==Qtrue)

#define DEBUG_PRINT(v) puts(StringValueCStr(rb_funcall(v,rb_intern("inspect"),0)))



#define NA_IsNArray(obj) \
  (rb_obj_is_kind_of(obj,cNArray)==Qtrue)
#define NA_IsArray(obj) \
  (TYPE(obj)==T_ARRAY || rb_obj_is_kind_of(obj,cNArray)==Qtrue)

#define NUM2REAL(v)  NUM2DBL( rb_funcall((v),na_id_real,0) )
#define NUM2IMAG(v)  NUM2DBL( rb_funcall((v),na_id_imag,0) )


/* Function Prototypes */


typedef struct {
    double beg;
    double step;
    size_t count;
} seq_opt_t;

typedef struct {
    u_int64_t max;
    int64_t sign;
    int shift;
} rand_opt_t;



typedef unsigned int BIT_DIGIT;
//#define BYTE_BIT_DIGIT sizeof(BIT_DIGIT)
#define NB     (sizeof(BIT_DIGIT)*8)
#define BALL   (~(BIT_DIGIT)0)
#define SLB(n) (((n)==NB)?~(BIT_DIGIT)0:(~(~(BIT_DIGIT)0<<(n))))

//#include "template.h"

#define ELEMENT_BIT_SIZE  "ELEMENT_BIT_SIZE"
#define ELEMENT_BYTE_SIZE "ELEMENT_BYTE_SIZE"
#define CONTIGUOUS_STRIDE "CONTIGUOUS_STRIDE"

#include "ndloop.h"
//#include "ndfunc.h"

#include "intern.h"

#endif /* ifndef NARRAY_H */
