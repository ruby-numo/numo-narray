/*
  module definition: <%= full_module_name %>
*/

<%  if module_var != ns_var %>
VALUE <%=module_var%>;
<%  end %>

<%= method_code %>
