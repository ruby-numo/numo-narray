    /*
      Document-module: <%= full_module_name %>
      <%= description %>
    */
    <%  if module_var != ns_var %>
    <%=module_var%> = rb_define_module_under(<%=ns_var%>, "<%=module_name%>");
    <%  end %>
    <% @children.each do |m| %>
    <%= m.init_def %><% end %>

    rb_include_module(cT, mTM);
