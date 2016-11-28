/*
  <%=type_name%>.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2016 by Masahiro TANAKA
*/

#include <ruby.h>
#include <math.h>
#include "narray.h"
#include "SFMT.h"
#include "template.h"
#include "<%=type_name%>.h"

//#define cT  <%=type_var%>
VALUE cT;
//#define cRT <%=real_type_var%>

#ifdef mTM
VALUE mTM;
#endif
