/*
  Convert a data value of obj (with a single element) to dtype.
*/
static dtype
<%=c_func%>(VALUE obj)
{
    narray_t *na;
    dtype  x;
    char  *ptr;
    size_t pos;
    VALUE  r, klass;

    if (IsNArray(obj)) {
        GetNArray(obj,na);
        if (na->size != 1) {
            rb_raise(nary_eShapeError,"narray size should be 1");
        }
        klass = CLASS_OF(obj);
        ptr = na_get_pointer_for_read(obj);
        pos = na_get_offset(obj);
        <% Store.definitions.select{|x| x.class==Store}.each do |x| %>
        if (<%=x.condition("klass")%>) {
            <%=x.extract_data("ptr","pos","x")%>;
            return x;
        }
        <% end %>

        // coerce
        r = rb_funcall(obj, rb_intern("coerce_cast"), 1, cT);
        if (CLASS_OF(r)==cT) {
            return <%=c_func%>(r);
        }
        <% if is_object %>
        return obj;
        <% else %>
        rb_raise(nary_eCastError, "unknown conversion from %s to %s",
                 rb_class2name(CLASS_OF(obj)),
                 rb_class2name(cT));
        <% end %>
    }
    if (TYPE(obj)==T_ARRAY) {
        if (RARRAY_LEN(obj) != 1) {
            rb_raise(nary_eShapeError,"array size should be 1");
        }
        return m_num_to_data(RARRAY_AREF(obj,0));
    }
    return m_num_to_data(obj);
}
