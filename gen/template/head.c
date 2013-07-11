/*
  <%=type_name%>.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

#include <ruby.h>
#include <math.h>
#include "narray.h"
#include "SFMT.h"
#include "template.h"
#include "<%=type_name%>.h"

VALUE <%=type_var%>;

#define cT  <%=type_var%>
#define cRT <%=real_type_var%>

<% if has_math %>
VALUE <%=math_var%>;
#define mTM <%=math_var%>
<% end %>
