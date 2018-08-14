<% if is_int and %w[div mod divmod].include? name %>
#define check_intdivzero(y)              \
    if ((y)==0) {                        \
        lp->err_type = rb_eZeroDivError; \
        return;                          \
    }
<% else %>
#define check_intdivzero(y) {}
<% end %>

static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t   i=0;
    size_t   n;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;

<% if is_simd and is_float and !is_complex and !is_object and %w[add sub mul div].include? name %>
    size_t cnt;
    size_t cnt_simd_loop = -1;
  <% if is_double_precision %>
    __m128d a;
    __m128d b;
  <% else %>
    __m128 a;
    __m128 b;
  <% end %>
    size_t num_pack; // Number of elements packed for SIMD.
    num_pack = SIMD_ALIGNMENT_SIZE / sizeof(dtype);
<% end %>
    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);

    //<% if need_align %>
    if (is_aligned(p1,sizeof(dtype)) &&
        is_aligned(p2,sizeof(dtype)) &&
        is_aligned(p3,sizeof(dtype)) ) {

        if (s1 == sizeof(dtype) &&
            s2 == sizeof(dtype) &&
            s3 == sizeof(dtype) ) {
<% if is_simd and is_float and !is_complex and !is_object and %w[add sub mul div].include? name %>
            // Check number of elements. & Check same alignment.
            if ((n >= num_pack) && is_same_aligned3(&((dtype*)p1)[i], &((dtype*)p2)[i], &((dtype*)p3)[i], SIMD_ALIGNMENT_SIZE)){
                // Calculate up to the position just before the start of SIMD computation.
                cnt = get_count_of_elements_not_aligned_to_simd_size(&((dtype*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(dtype));
                if (p1 == p3) { // inplace case
                    for (; i < cnt; i++) {
                        ((dtype*)p1)[i] = m_<%=name%>(((dtype*)p1)[i],((dtype*)p2)[i]);
                    }
                } else {
                    for (; i < cnt; i++) {
                        ((dtype*)p3)[i] = m_<%=name%>(((dtype*)p1)[i],((dtype*)p2)[i]);
                    }
                }

                // Get the count of SIMD computation loops.
                cnt_simd_loop = (n - i) % num_pack;

                // SIMD computation.
                if (p1 == p3) { // inplace case
                    for(; i < n - cnt_simd_loop; i += num_pack){
                        a = _mm_load_<%=simd_type%>(&((dtype*)p1)[i]);
                        b = _mm_load_<%=simd_type%>(&((dtype*)p2)[i]);
                        a = _mm_<%=name%>_<%=simd_type%>(a, b);
                        _mm_store_<%=simd_type%>(&((dtype*)p1)[i], a);
                    }
                } else {
                    for(; i < n - cnt_simd_loop; i += num_pack){
                        a = _mm_load_<%=simd_type%>(&((dtype*)p1)[i]);
                        b = _mm_load_<%=simd_type%>(&((dtype*)p2)[i]);
                        a = _mm_<%=name%>_<%=simd_type%>(a, b);
                        _mm_stream_<%=simd_type%>(&((dtype*)p3)[i], a);
                    }
                }
            }

            // Compute the remainder of the SIMD operation.
            if (cnt_simd_loop != 0){
<% end %>
                if (p1 == p3) { // inplace case
                    for (; i<n; i++) {
                        check_intdivzero(((dtype*)p2)[i]);
                        ((dtype*)p1)[i] = m_<%=name%>(((dtype*)p1)[i],((dtype*)p2)[i]);
                    }
                } else {
                    for (; i<n; i++) {
                        check_intdivzero(((dtype*)p2)[i]);
                        ((dtype*)p3)[i] = m_<%=name%>(((dtype*)p1)[i],((dtype*)p2)[i]);
                    }
                }
<% if is_simd and is_float and !is_complex and !is_object and %w[add sub mul div].include? name %>
            }
<% end %>
            return;
        }

        if (is_aligned_step(s1,sizeof(dtype)) &&
            is_aligned_step(s2,sizeof(dtype)) &&
            is_aligned_step(s3,sizeof(dtype)) ) {
            //<% end %>

            if (s2 == 0){ // Broadcasting from scalar value.
                check_intdivzero(*(dtype*)p2);
                if (s1 == sizeof(dtype) &&
                    s3 == sizeof(dtype) ) {
<% if is_simd and is_float and !is_complex and !is_object and %w[add sub mul div].include? name %>
                    // Broadcast a scalar value and use it for SIMD computation.
                    b = _mm_load1_<%=simd_type%>(&((dtype*)p2)[0]);

                    // Check number of elements. & Check same alignment.
                    if ((n >= num_pack) && is_same_aligned2(&((dtype*)p1)[i], &((dtype*)p3)[i], SIMD_ALIGNMENT_SIZE)){
                        // Calculate up to the position just before the start of SIMD computation.
                        cnt = get_count_of_elements_not_aligned_to_simd_size(&((dtype*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(dtype));
                        if (p1 == p3) { // inplace case
                            for (; i < cnt; i++) {
                                ((dtype*)p1)[i] = m_<%=name%>(((dtype*)p1)[i],*(dtype*)p2);
                            }
                        } else {
                            for (; i < cnt; i++) {
                                ((dtype*)p3)[i] = m_<%=name%>(((dtype*)p1)[i],*(dtype*)p2);
                            }
                        }

                        // Get the count of SIMD computation loops.
                        cnt_simd_loop = (n - i) % num_pack;

                        // SIMD computation.
                        if (p1 == p3) { // inplace case
                            for(; i < n - cnt_simd_loop; i += num_pack){
                                a = _mm_load_<%=simd_type%>(&((dtype*)p1)[i]);
                                a = _mm_<%=name%>_<%=simd_type%>(a, b);
                                _mm_store_<%=simd_type%>(&((dtype*)p1)[i], a);
                            }
                        } else {
                            for(; i < n - cnt_simd_loop; i += num_pack){
                                a = _mm_load_<%=simd_type%>(&((dtype*)p1)[i]);
                                a = _mm_<%=name%>_<%=simd_type%>(a, b);
                                _mm_stream_<%=simd_type%>(&((dtype*)p3)[i], a);
                            }
                        }
                    }

                    // Compute the remainder of the SIMD operation.
                    if (cnt_simd_loop != 0){
<% end %>
                        if (p1 == p3) { // inplace case
                            for (; i<n; i++) {
                                ((dtype*)p1)[i] = m_<%=name%>(((dtype*)p1)[i],*(dtype*)p2);
                            }
                        } else {
                            for (; i<n; i++) {
                                ((dtype*)p3)[i] = m_<%=name%>(((dtype*)p1)[i],*(dtype*)p2);
                            }
                        }
<% if is_simd and is_float and !is_complex and !is_object and %w[add sub mul div].include? name %>
                    }
<% end %>
                } else {
                    for (i=0; i<n; i++) {
                        *(dtype*)p3 = m_<%=name%>(*(dtype*)p1,*(dtype*)p2);
                        p1 += s1;
                        p3 += s3;
                    }
                }
            } else {
                if (p1 == p3) { // inplace case
                    for (i=0; i<n; i++) {
                        check_intdivzero(*(dtype*)p2);
                        *(dtype*)p1 = m_<%=name%>(*(dtype*)p1,*(dtype*)p2);
                        p1 += s1;
                        p2 += s2;
                    }
                } else {
                    for (i=0; i<n; i++) {
                        check_intdivzero(*(dtype*)p2);
                        *(dtype*)p3 = m_<%=name%>(*(dtype*)p1,*(dtype*)p2);
                        p1 += s1;
                        p2 += s2;
                        p3 += s3;
                    }
                }
            }

            return;
            //<% if need_align %>
        }
    }
    for (i=0; i<n; i++) {
        dtype x, y, z;
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        check_intdivzero(y);
        z = m_<%=name%>(x,y);
        SET_DATA_STRIDE(p3,s3,dtype,z);
    }
    //<% end %>
}
#undef check_intdivzero

static VALUE
<%=c_func%>_self(VALUE self, VALUE other)
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iter%>, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary <%=name%>.
  @overload <%=op_map%> other
  @param [Numo::NArray,Numeric] other
  @return [Numo::NArray] self <%=op_map%> other
*/
static VALUE
<%=c_func(1)%>(VALUE self, VALUE other)
{
    <% if is_object %>
    return <%=c_func%>_self(self, other);
    <% else %>
    VALUE klass, v;

    klass = na_upcast(rb_obj_class(self),rb_obj_class(other));
    if (klass==cT) {
        return <%=c_func%>_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, <%=id_op%>, 1, other);
    }
    <% end %>
}
