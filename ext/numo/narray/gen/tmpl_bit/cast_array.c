static VALUE
<%=c_func%>(VALUE rary)
{
    VALUE vnc, nary;
    narray_t *na;
    na_compose_t *nc;

    vnc = na_ary_composition(rary);
    Data_Get_Struct(vnc, na_compose_t, nc);
    nary = rb_narray_new(cT, nc->ndim, nc->shape);
    RB_GC_GUARD(vnc);
    GetNArray(nary,na);
    if (na->size > 0) {
        <%=find_tmpl("allocate").c_func%>(nary);
        <%=find_tmpl("store_array").c_func%>(nary, rary);
    }
    return nary;
}
