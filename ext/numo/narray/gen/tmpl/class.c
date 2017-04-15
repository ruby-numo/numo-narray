/*
  class definition: <%= full_class_name %>
*/

VALUE <%=class_var%>;

static VALUE <%= find('store').c_func %>(VALUE,VALUE);

<%= method_code %>
