typedef u_int64_t dtype;
typedef u_int64_t rtype;

#define m_num_to_data(x) ((dtype)NUM2UINT64(x))
#define m_data_to_num(x) UINT642NUM((u_int64_t)(x))
#define m_extract(x)     UINT642NUM((u_int64_t)*(dtype*)(x))
#define m_sprintf(s,x)   sprintf(s,"%"INT64FMT"u",(u_int64_t)(x))
#define m_rand           ((dtype)gen_rand64())

#include "uint_macro.h"
