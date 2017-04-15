    /*
      Document-class: <%= full_class_name %>
      <%= description %>
    */
    cT = rb_define_class_under(<%=ns_var%>, "<%=class_name%>", cNArray);

  <% for x in class_alias %>
    // alias of <%=class_name%>
    rb_define_const(<%=ns_var%>, "<%=x%>", <%=type_var%>);
  <% end %>
  <% if has_math %>
    mTM = rb_define_module_under(cT, "Math");
  <% end %>
  <% if is_bit %>
    rb_define_const(cT, "ELEMENT_BIT_SIZE",  INT2FIX(1));
    rb_define_const(cT, "ELEMENT_BYTE_SIZE", rb_float_new(1.0/8));
    rb_define_const(cT, "CONTIGUOUS_STRIDE", INT2FIX(1));
  <% else %>
    rb_define_const(cT, ELEMENT_BIT_SIZE,  INT2FIX(sizeof(dtype)*8));
    rb_define_const(cT, ELEMENT_BYTE_SIZE, INT2FIX(sizeof(dtype)));
    rb_define_const(cT, CONTIGUOUS_STRIDE, INT2FIX(sizeof(dtype)));
  <% end %>
  <% if is_object %>
    rb_undef_method(rb_singleton_class(cT),"from_binary");
    rb_undef_method(cT,"to_binary");
    rb_undef_method(cT,"swap_byte");
    rb_undef_method(cT,"to_network");
    rb_undef_method(cT,"to_vacs");
    rb_undef_method(cT,"to_host");
    rb_undef_method(cT,"to_swapped");
  <% end %>
    hCast = rb_hash_new();
    rb_define_const(cT, "UPCAST", hCast);
    rb_hash_aset(hCast, rb_cArray,   cT);
    <% for x in upcast %>
    <%= x %><% end %>

    <% @children.each do |m| %>
    <%= m.init_def %><% end %>
    rb_define_singleton_method(cT, "[]", <%=find("cast").c_func%>, -2);
