/*
  linalg_<%=type_name%>.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2007,2013 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

#include <ruby.h>
#include "narray.h"
#include "template.h"
#include "<%=type_name%>.h"
//#include <blas.h>

#define cCT c<%=complex_class_name%>
typedef <%=complex_type%> ctype;

static VALUE mTL;

// Error Class ??
#define CHECK_DIM_GE(na,nd)                                     \
    if ((na)->ndim<(nd)) {                                      \
        rb_raise(nary_eShapeError,                              \
                 "n-dimension=%d, but >=%d is expected",        \
                 (na)->ndim, (nd));                             \
    }

<%
mod_var "mTL"
%w[
  matmul
  solve
].map{|a| def_singleton(a,2,a,:mod_var=>"mTL")}

%w[
  eigen
].map{|a| def_singleton(a,1,a,:mod_var=>"mTL")}

#def_alias "mmdot", "matmul"
Function.codes.each do |x| %>
<%= x %>
<% end %>

VALUE
Init_nary_<%=type_name%>_linalg()
{
    mTL = rb_define_module_under(cT, "Linalg");

    <% Function.definitions.each do |x| %><%= x %>
    <% end %>
    return mTL;
}
