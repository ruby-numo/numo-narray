void
Init_nary_<%=tp%>()
{
    volatile VALUE hCast;

    cT = rb_define_class_under(cNArray, "<%=class_name%>", cNArray);
    <% for x in class_alias %>
    // alias of <%=class_name%>
    rb_define_const(cNArray, "<%=x%>", <%=type_var%>);
    <% end %>
    <% if has_math %>
    mTM = rb_define_module_under(cT, "Math"); <%end%>

    rb_define_const(cT, ELEMENT_BIT_SIZE,  INT2FIX(sizeof(dtype)*8));
    rb_define_const(cT, ELEMENT_BYTE_SIZE, INT2FIX(sizeof(dtype)));
    rb_define_const(cT, CONTIGUOUS_STRIDE, INT2FIX(sizeof(dtype)));

    //rb_define_singleton_method(cNArray, "<%=class_name%>", <%=Cast.c_instance_method%>, 1);
    rb_define_singleton_method(cT, "[]", <%=Cast.c_instance_method%>, -2);

    <% Template::INIT.each do |x| %>
    <%=x%><% end %>

    hCast = rb_hash_new();
    rb_define_const(cT, "UPCAST", hCast);
    rb_hash_aset(hCast, rb_cArray,   cT);
    <% for x in upcast %>
    <%=x%><% end %>
}
