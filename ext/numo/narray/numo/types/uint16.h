typedef u_int16_t dtype;
typedef u_int16_t rtype;
#define cT  numo_cUInt16
#define cRT cT

#define m_num_to_data(x) ((dtype)NUM2UINT(x))
#define m_data_to_num(x) UINT2NUM((unsigned int)(x))
#define m_extract(x)     UINT2NUM((unsigned int)*(dtype*)(x))
#define m_sprintf(s,x)   sprintf(s,"%u",(unsigned int)(x))

#include "uint_macro.h"

#ifndef UINT16_MAX
#define UINT16_MAX (65535)
#endif

#define M_MIN  INT2FIX(0)
#define M_MAX  m_data_to_num(UINT16_MAX)
