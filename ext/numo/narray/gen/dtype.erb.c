/*
  <%=type_name%>.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011,2013-2016 by Masahiro TANAKA
*/

#include <ruby.h>
#include <math.h>
#include "numo/narray.h"
#include "numo/template.h"
#include "SFMT.h"

#define m_map(x) m_num_to_data(rb_yield(m_data_to_num(x)))

<%
eval open(File.join(File.dirname(__FILE__),"spec.rb")).read

IdVar.declaration.each do |x| %>
<%= x %><%
end
%>

#include "numo/types/<%=type_name%>.h"

VALUE cT;
extern VALUE cRT;
#ifdef mTM
VALUE mTM;
#endif

static VALUE <%= find_tmpl('store').c_func %>(VALUE,VALUE);
<%
Function.codes.each do |x| %>
<%= x %>
<% end %>

void
Init_nary_<%=tp%>()
{
    volatile VALUE hCast;

    cT = rb_define_class_under(mNumo, "<%=class_name%>", cNArray);
    <% for x in class_alias %>
    // alias of <%=class_name%>
    rb_define_const(mNumo, "<%=x%>", <%=type_var%>);
    <% end %>
    <% if has_math %>
    mTM = rb_define_module_under(cT, "Math"); <% end %>

    <% if is_bit %>
    rb_define_const(cT, "ELEMENT_BIT_SIZE",  INT2FIX(1));
    rb_define_const(cT, "ELEMENT_BYTE_SIZE", rb_float_new(1.0/8));
    rb_define_const(cT, "CONTIGUOUS_STRIDE", INT2FIX(1));
    <% else %>
    rb_define_const(cT, ELEMENT_BIT_SIZE,  INT2FIX(sizeof(dtype)*8));
    rb_define_const(cT, ELEMENT_BYTE_SIZE, INT2FIX(sizeof(dtype)));
    rb_define_const(cT, CONTIGUOUS_STRIDE, INT2FIX(sizeof(dtype)));
    <% end %>

    rb_define_singleton_method(cT, "[]", <%=cast_func%>, -2);

    <% if is_object %>
    rb_undef_method(rb_singleton_class(cT),"from_binary");
    rb_undef_method(cT,"to_binary");
    <% end %>

    <% Function.definitions.each do |x| %>
    <%= x %><% end %>
    <% IdVar.assignment.each do |x| %>
    <%= x %><% end %>

    hCast = rb_hash_new();
    rb_define_const(cT, "UPCAST", hCast);
    rb_hash_aset(hCast, rb_cArray,   cT);
    <% for x in upcast %>
    <%= x %><% end %>
}
