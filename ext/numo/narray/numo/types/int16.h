typedef int16_t dtype;
typedef int16_t rtype;
#define cT  numo_cInt16
#define cRT cT

#define m_num_to_data(x) ((dtype)NUM2INT(x))
#define m_data_to_num(x) INT2NUM((int)(x))
#define m_extract(x)     INT2NUM((int)*(dtype*)(x))
#define m_sprintf(s,x)   sprintf(s,"%d",(int)(x))
#define m_rand           ((dtype)gen_rand32())

#include "int_macro.h"
