static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i=0, n;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype   x;

<% if is_simd and !is_complex and %w[sqrt].include? name %>
    size_t cnt;
    size_t cnt_simd_loop = -1;
    <% if is_double_precision %>
    __m128d a;
    <% else %>
    __m128 a;
    <% end %>
    size_t num_pack; // Number of elements packed for SIMD.
    num_pack = SIMD_ALIGNMENT_SIZE / sizeof(dtype);
<% end %>
    INIT_COUNTER(lp, n);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);

    if (idx1) {
        if (idx2) {
            for (i=0; i<n; i++) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                x = m_<%=name%>(x);
                SET_DATA_INDEX(p2,idx2,dtype,x);
            }
        } else {
            for (i=0; i<n; i++) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                x = m_<%=name%>(x);
                SET_DATA_STRIDE(p2,s2,dtype,x);
            }
        }
    } else {
        if (idx2) {
            for (i=0; i<n; i++) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                x = m_<%=name%>(x);
                SET_DATA_INDEX(p2,idx2,dtype,x);
            }
        } else {
            //<% if need_align %>
            if (is_aligned(p1,sizeof(dtype)) &&
                is_aligned(p2,sizeof(dtype)) ) {
                if (s1 == sizeof(dtype) &&
                    s2 == sizeof(dtype) ) {
                    //<% if is_simd and !is_complex and %w[sqrt].include? name %>
                    // Check number of elements. & Check same alignment.
                    if ((n >= num_pack) && is_same_aligned2(&((dtype*)p1)[i], &((dtype*)p2)[i], SIMD_ALIGNMENT_SIZE)){
                        // Calculate up to the position just before the start of SIMD computation.
                        cnt = get_count_of_elements_not_aligned_to_simd_size(&((dtype*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(dtype));
                        for (i=0; i < cnt; i++) {
                            ((dtype*)p2)[i] = m_<%=name%>(((dtype*)p1)[i]);
                        }

                        // Get the count of SIMD computation loops.
                        cnt_simd_loop = (n - i) % num_pack;

                        // SIMD computation.
                        if (p1 == p2) { // inplace case
                            for(; i < n - cnt_simd_loop; i += num_pack){
                                a = _mm_load_<%=simd_type%>(&((dtype*)p1)[i]);
                                a = _mm_<%=name%>_<%=simd_type%>(a);
                                _mm_store_<%=simd_type%>(&((dtype*)p1)[i], a);
                            }
                        } else {
                            for(; i < n - cnt_simd_loop; i += num_pack){
                                a = _mm_load_<%=simd_type%>(&((dtype*)p1)[i]);
                                a = _mm_<%=name%>_<%=simd_type%>(a);
                                _mm_stream_<%=simd_type%>(&((dtype*)p2)[i], a);
                            }
                        }

                    }
                    // Compute the remainder of the SIMD operation.
                    if (cnt_simd_loop != 0){
                        //<% end %>
                        for (; i<n; i++) {
                            ((dtype*)p2)[i] = m_<%=name%>(((dtype*)p1)[i]);
                        }
                        //<% if is_simd and !is_complex and %w[sqrt].include? name %>
                    }
                    //<% end %>
                    return;
                }
                if (is_aligned_step(s1,sizeof(dtype)) &&
                    is_aligned_step(s2,sizeof(dtype)) ) {
                    //<% end %>
                    for (i=0; i<n; i++) {
                        *(dtype*)p2 = m_<%=name%>(*(dtype*)p1);
                        p1 += s1;
                        p2 += s2;
                    }
                    return;
                    //<% if need_align %>
                }
            }
            for (i=0; i<n; i++) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                x = m_<%=name%>(x);
                SET_DATA_STRIDE(p2,s2,dtype,x);
            }
            //<% end %>
        }
    }
}

/*
  Calculate <%=name%>(x).
  @overload <%=name%>(x)
  @param [Numo::NArray,Numeric] x  input value
  @return [Numo::<%=class_name%>] result of <%=name%>(x).
*/
static VALUE
<%=c_func(1)%>(VALUE mod, VALUE a1)
{
    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iter%>, FULL_LOOP, 1, 1, ain, aout };

    return na_ndloop(&ndf, 1, a1);
}
