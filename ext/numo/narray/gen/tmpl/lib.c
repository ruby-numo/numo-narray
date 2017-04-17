/*
  <%= file_name %>
  Ruby/Numo::GSL - GSL wrapper for Ruby/Numo::NArray

  created on: 2017-03-11
  Copyright (C) 2017 Masahiro Tanaka
*/

#include <ruby.h>
#include <assert.h>
#include "numo/narray.h"
#include "numo/template.h"
#include "SFMT.h"

#define m_map(x) m_num_to_data(rb_yield(m_data_to_num(x)))

<% id_decl.each do |x| %>
<%= x %>
<% end %>

<% include_files.each do |f| %>
#include <<%=f%>>
<% end %>

VALUE cT;
extern VALUE cRT;

<% children.each do |c|%>
<%= c.result+"\n\n" %>
<% end %>

void
Init_<%=lib_name%>(void)
{
    VALUE hCast, <%=ns_var%>;

    <%=ns_var%> = rb_define_module("Numo");

    <% id_assign.each do |x| %>
    <%= x %><% end %>

<% children.each do |c| %>
<%= c.init_def %>
<% end %>
}
