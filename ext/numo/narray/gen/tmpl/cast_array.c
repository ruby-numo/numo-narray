static VALUE
<%=c_func(:nodef)%>(VALUE rary)
{
    VALUE nary;
    narray_t *na;

    nary = na_s_new_like(cT, rary);
    GetNArray(nary,na);
    if (na->size > 0) {
        <%=find_tmpl("store").find("array").c_func%>(nary,rary);
    }
    return nary;
}
