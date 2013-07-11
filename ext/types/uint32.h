typedef u_int32_t dtype;
typedef u_int32_t rtype;

#define m_num_to_data(x) ((dtype)NUM2UINT32(x))
#define m_data_to_num(x) UINT322NUM((u_int32_t)(x))
#define m_extract(x)     UINT322NUM((u_int32_t)*(dtype*)(x))
#define m_sprintf(s,x)   sprintf(s,"%"INT32FMT"u",(u_int32_t)(x))
#define m_rand           ((dtype)gen_rand32())

#include "uint_macro.h"
