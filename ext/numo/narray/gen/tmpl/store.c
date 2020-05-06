<% children.each do |c|%>
<%= c.result %>

<% end %>
/*
  Store elements to Numo::<%=class_name%> from other.
  @overload store(other)
  @param [Object] other
  @return [Numo::<%=class_name%>] self
*/
static VALUE
<%=c_func(1)%>(VALUE self, VALUE obj)
{
    VALUE r, klass;

    klass = rb_obj_class(obj);

    <% definitions.each do |x| %>
    if (<%=x.condition("klass")%>) {
        <%=x.c_func%>(self,obj);
        return self;
    }
    <% end %>

    if (IsNArray(obj)) {
        r = rb_funcall(obj, rb_intern("coerce_cast"), 1, cT);
        if (rb_obj_class(r)==cT) {
            <%=c_func%>(self,r);
            return self;
        }
    }

    <% if is_object %>
    robject_store_numeric(self,obj);
    <% else %>
    rb_raise(nary_eCastError, "unknown conversion from %s to %s",
             rb_class2name(rb_obj_class(obj)),
             rb_class2name(rb_obj_class(self)));
    <% end %>
    return self;
}
