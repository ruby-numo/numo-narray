/*
  Store elements to Numo::<%=class_name%> from other.
  @overload store(other)
  @param [Object] other
  @return [Numo::<%=class_name%>] self
*/
static VALUE
<%=c_func%>(VALUE self, VALUE obj)
{

    VALUE r;

    <% Store.definitions.each do |x| %>
    if (<%=x.condition%>) {
        <%=x.c_func%>(self,obj);
        return self;
    }
    <% end %>

    if (IsNArray(obj)) {
        r = rb_funcall(obj, rb_intern("coerce_cast"), 1, cT);
        if (CLASS_OF(r)==cT) {
            <%=c_func%>(self,r);
            return self;
        }
    }

    rb_raise(nary_eCastError, "unknown conversion from %s to %s",
             rb_class2name(CLASS_OF(obj)),
             rb_class2name(CLASS_OF(self)));
    return self;
}
