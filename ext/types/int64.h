typedef int64_t dtype;
typedef int64_t rtype;

#define m_num_to_data(x) ((dtype)NUM2INT64(x))
#define m_data_to_num(x) INT642NUM((int64_t)(x))
#define m_extract(x)     INT642NUM((int64_t)*(dtype*)(x))
#define m_sprintf(s,x)   sprintf(s,"%"INT64FMT"d",(int64_t)(x))
#define m_rand           ((dtype)gen_rand64())

#include "int_macro.h"
