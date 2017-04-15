/*
  module definition: <%= full_module_name %>
*/

<%  if module_var != ns_var %>
static VALUE <%=module_var%>;
<%  end %>

<%= method_code %>
