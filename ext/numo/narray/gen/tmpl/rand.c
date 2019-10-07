<%
if is_int && !is_object
  if /Int64$/ =~ class_name
    rand_bit = 64
  else
    rand_bit = 32
  end
  m_rand = "m_rand(max,shift)"
  shift_def = "int shift;"
  shift_set = "shift = #{rand_bit-1} - msb_pos(max);"
  rand_type = "uint#{rand_bit}_t"
%>

#define HWID (sizeof(dtype)*4)

static int msb_pos(<%=rand_type%> a)
{
    int width = HWID;
    int pos = 0;
    <%=rand_type%> mask = (((dtype)1 << HWID)-1) << HWID;

    if (a==0) {return -1;}

    while (width) {
        if (a & mask) {
            pos += width;
        } else {
            mask >>= width;
        }
        width >>= 1;
        mask &= mask << width;
    }
    return pos;
}

/* generates a random number on [0,max) */
<% if rand_bit == 64 %>
inline static dtype m_rand(uint64_t max, int shift)
{
    uint64_t x;
    do {
        x = gen_rand32();
        x <<= 32;
        x |= gen_rand32();
        x >>= shift;
    } while (x >= max);
    return x;
}
<% else %>
inline static dtype m_rand(uint32_t max, int shift)
{
    uint32_t x;
    do {
        x = gen_rand32();
        x >>= shift;
    } while (x >= max);
    return x;
}
<% end %>
<%
else
  m_rand = "m_rand(max)"
  shift_def = ""
  shift_set = ""
  rand_type = "dtype"
end
%>

typedef struct {
    dtype low;
    <%=rand_type%> max;
} rand_opt_t;

static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1;
    ssize_t  s1;
    size_t  *idx1;
    dtype    x;
    rand_opt_t *g;
    dtype    low;
    <%=rand_type%> max;
    <%=shift_def%>

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    g = (rand_opt_t*)(lp->opt_ptr);
    low = g->low;
    max = g->max;
    <%=shift_set%>

    if (idx1) {
        for (; i--;) {
            x = m_add(<%=m_rand%>,low);
            SET_DATA_INDEX(p1,idx1,dtype,x);
        }
    } else {
        for (; i--;) {
            x = m_add(<%=m_rand%>,low);
            SET_DATA_STRIDE(p1,s1,dtype,x);
        }
    }
}


/*
  Generate uniformly distributed random numbers on self narray.
  @overload rand([[low],high])
  @param [Numeric] low  lower inclusive boundary of random numbers. (default=0)
  @param [Numeric] high  upper exclusive boundary of random numbers. (default=1 or 1+1i for complex types)
  @return [Numo::<%=class_name%>] self.
  @example
    Numo::DFloat.new(6).rand
    # => Numo::DFloat#shape=[6]
    # [0.0617545, 0.373067, 0.794815, 0.201042, 0.116041, 0.344032]

    Numo::DComplex.new(6).rand(5+5i)
    # => Numo::DComplex#shape=[6]
    # [2.69974+3.68908i, 0.825443+0.254414i, 0.540323+0.34354i, 4.52061+2.39322i, ...]

    Numo::Int32.new(6).rand(2,5)
    # => Numo::Int32#shape=[6]
    # [4, 3, 3, 2, 4, 2]
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *args, VALUE self)
{
    rand_opt_t g;
    VALUE v1=Qnil, v2=Qnil;
    dtype high;
    ndfunc_arg_in_t ain[1] = {{OVERWRITE,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP, 1,0, ain,0};

    <% if is_int && !is_object %>
    rb_scan_args(argc, args, "11", &v1, &v2);
    if (v2==Qnil) {
        g.low = m_zero;
        g.max = high = m_num_to_data(v1);
    <% else %>
    rb_scan_args(argc, args, "02", &v1, &v2);
    if (v2==Qnil) {
        g.low = m_zero;
        if (v1==Qnil) {
            <% if is_complex %>
            g.max = high = c_new(1,1);
            <% else %>
            g.max = high = m_one;
            <% end %>
        } else {
            g.max = high = m_num_to_data(v1);
        }
    <% end %>
    } else {
        g.low = m_num_to_data(v1);
        high = m_num_to_data(v2);
        g.max = m_sub(high,g.low);
    }
    <% if is_int && !is_object %>
    if (high <= g.low) {
        rb_raise(rb_eArgError,"high must be larger than low");
    }
    <% end %>
    na_ndloop3(&ndf, &g, 1, self);
    return self;
}
