/*
  return NArray with cast to the type of self.
  @overload coerce_cast(type)
  @return [nil]
*/
static VALUE
<%=c_func(1)%>(VALUE self, VALUE type)
{
    return Qnil;
}
