    /*
      Document-module: <%= full_module_name %>
      <%= description %>
    */
    <%  if module_var != ns_var %>
    <%=module_var%> = rb_define_module_under(<%=ns_var%>, "<%=module_name%>");
    <%  end %>
    <% @children.each do |m| %>
    <%= m.init_def %><% end %>

    //  how to do this?
    //rb_extend_object(cT, mTM);
