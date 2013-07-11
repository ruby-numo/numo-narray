
static VALUE
<%=Cast.c_instance_method%>(VALUE type, VALUE obj)
{
    VALUE r;

    if (CLASS_OF(obj)==cT) {
        return obj;
    }
    <% Cast::INIT.each do |x| %>
    if (<%=x.condition%>) {
        return <%=x.c_function%>(obj);
    }
    <% end %>

    if (IsNArray(obj)) {
        r = rb_funcall(obj, rb_intern("coerce_cast"), 1, cT);
        if (RTEST(r)) {
            return r;
        }
    }
    rb_raise(nary_eCastError, "unknown conversion from %s to %s",
             rb_class2name(CLASS_OF(obj)),
             rb_class2name(type));
    return r;
}
