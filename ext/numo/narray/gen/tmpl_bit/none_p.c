static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    VALUE v;

    v = <%=find_tmpl("any?").c_func%>(argc,argv,self);

    if (v==Qtrue) {
        return Qfalse;
    } else if (v==Qfalse) {
        return Qtrue;
    }
    return <%=find_tmpl("not").c_func%>(v);
}
