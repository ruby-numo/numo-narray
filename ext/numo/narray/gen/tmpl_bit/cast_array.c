static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t i, n;
    size_t i1, n1;
    VALUE  v1, *ptr;
    BIT_DIGIT *a2;
    size_t p2;
    size_t s2, *idx2;
    VALUE  x;
    double y;
    BIT_DIGIT z;
    size_t len, c;
    double beg, step;

    v1 = lp->args[0].value;
    ptr = &v1;
    INIT_COUNTER(lp, n);
    INIT_PTR_BIT_IDX(lp, 1, a2, p2, s2, idx2);

    switch(TYPE(v1)) {
    case T_ARRAY:
        n1 = RARRAY_LEN(v1);
        ptr = RARRAY_PTR(v1);
        break;
    case T_NIL:
        n1 = 0;
        break;
    default:
        n1 = 1;
    }
    if (idx2) {
        <% ["STORE_BIT(a2, p2+*idx2, z); idx2++;",
            "STORE_BIT(a2, p2, z); p2+=s2;"].each_with_index do |x,i| %>
        for (i=i1=0; i1<n1 && i<n; i++,i1++) {
            x = ptr[i1];
            if (rb_obj_is_kind_of(x, rb_cRange) || rb_obj_is_kind_of(x, na_cStep)) {
                nary_step_sequence(x,&len,&beg,&step);
                for (c=0; c<len && i<n; c++,i++) {
                    y = beg + step * c;
                    z = m_from_double(y);
                    <%= x %>
                }
            }
            if (TYPE(x) != T_ARRAY) {
                if (x == Qnil) x = INT2FIX(0);
                z = m_num_to_data(x);
                <%= x %>
            }
        }
        z = m_zero;
        for (; i<n; i++) {
            <%= x %>
        }
        <% if i<1 %>
    } else {
        <% end
        end %>
    }
}

static VALUE
<%=c_func%>(VALUE rary)
{
    volatile VALUE vnc, nary;
    narray_t *na;
    na_compose_t *nc;
    ndfunc_arg_in_t ain[2] = {{rb_cArray,0},{Qnil,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP, 2,0, ain,0};

    vnc = na_ary_composition(rary);
    Data_Get_Struct(vnc, na_compose_t, nc);
    nary = rb_narray_new(cT, nc->ndim, nc->shape);
    GetNArray(nary,na);
    if (na->size > 0) {
        <%=find_tmpl("allocate").c_func%>(nary);
        na_ndloop_cast_rarray_to_narray(&ndf, rary, nary);
    }
    return nary;
}
