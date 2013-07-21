void
Init_ffte()
{
    rb_mFFTE = rb_define_module("FFTE");
    // Radix Error
    eRadixError = rb_define_class_under(rb_mFFTE, "RadixError", rb_eStandardError);

<% $funcs.each do |f| %>
    rb_define_module_function(rb_mFFTE, "<%=f%>", nary_ffte_<%=f%>, -1);
<% end %>
}
