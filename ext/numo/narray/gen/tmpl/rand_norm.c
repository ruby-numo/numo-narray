typedef struct {
    dtype mu;
    rtype sigma;
} randn_opt_t;

static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1;
    ssize_t  s1;
    size_t  *idx1;
    <% if is_complex %>
    dtype   *a0;
    <% else %>
    dtype   *a0, *a1;
    <% end %>
    dtype    mu;
    rtype    sigma;
    randn_opt_t *g;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    g = (randn_opt_t*)(lp->opt_ptr);
    mu = g->mu;
    sigma = g->sigma;

    if (idx1) {
        <% if is_complex %>
        for (; i--;) {
            a0 = (dtype*)(p1+*idx1);
            m_rand_norm(mu,sigma,a0);
            idx1 += 1;
        }
        <% else %>
        for (; i>1; i-=2) {
            a0 = (dtype*)(p1+*idx1);
            a1 = (dtype*)(p1+*(idx1+1));
            m_rand_norm(mu,sigma,a0,a1);
            idx1 += 2;
        }
        if (i>0) {
            a0 = (dtype*)(p1+*idx1);
            m_rand_norm(mu,sigma,a0,0);
        }
        <% end %>
    } else {
        <% if is_complex %>
        for (; i--;) {
            a0 = (dtype*)(p1);
            m_rand_norm(mu,sigma,a0);
            p1 += s1;
        }
        <% else %>
        for (; i>1; i-=2) {
            a0 = (dtype*)(p1);
            a1 = (dtype*)(p1+s1);
            m_rand_norm(mu,sigma,a0,a1);
            p1 += s1*2;
        }
        if (i>0) {
            a0 = (dtype*)(p1);
            m_rand_norm(mu,sigma,a0,0);
        }
        <% end %>
    }
}

/*
  Generates random numbers from the normal distribution on self narray
  using Box-Muller Transformation.
  @overload rand_norm([mu,[sigma]])
  @param [Numeric] mu  mean of normal distribution. (default=0)
  @param [Numeric] sigma  standard deviation of normal distribution. (default=1)
  @return [Numo::<%=class_name%>] self.
  @example
    Numo::DFloat.new(5,5).rand_norm
    # => Numo::DFloat#shape=[5,5]
    # [[-0.581255, -0.168354, 0.586895, -0.595142, -0.802802],
    #  [-0.326106, 0.282922, 1.68427, 0.918499, -0.0485384],
    #  [-0.464453, -0.992194, 0.413794, -0.60717, -0.699695],
    #  [-1.64168, 0.48676, -0.875871, -1.43275, 0.812172],
    #  [-0.209975, -0.103612, -0.878617, -1.42495, 1.0968]]

    Numo::DFloat.new(5,5).rand_norm(10,0.1)
    # => Numo::DFloat#shape=[5,5]
    # [[9.9019, 9.90339, 10.0826, 9.98384, 9.72861],
    #  [9.81507, 10.0272, 9.91445, 10.0568, 9.88923],
    #  [10.0234, 9.97874, 9.96011, 9.9006, 9.99964],
    #  [10.0186, 9.94598, 9.92236, 9.99811, 9.97003],
    #  [9.79266, 9.95044, 9.95212, 9.93692, 10.2027]]

    Numo::DComplex.new(3,3).rand_norm(5+5i)
    # => Numo::DComplex#shape=[3,3]
    # [[5.84303+4.40052i, 4.00984+6.08982i, 5.10979+5.13215i],
    #  [4.26477+3.99655i, 4.90052+5.00763i, 4.46607+2.3444i],
    #  [4.5528+7.11003i, 5.62117+6.69094i, 5.05443+5.35133i]]
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *args, VALUE self)
{
    int n;
    randn_opt_t g;
    VALUE v1=Qnil, v2=Qnil;
    ndfunc_arg_in_t ain[1] = {{OVERWRITE,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP, 1,0, ain,0};

    n = rb_scan_args(argc, args, "02", &v1, &v2);
    if (n == 0) {
        g.mu = m_zero;
    } else {
        g.mu = m_num_to_data(v1);
    }
    if (n == 2) {
        g.sigma = NUM2DBL(v2);
    } else {
        g.sigma = 1;
    }
    na_ndloop3(&ndf, &g, 1, self);
    return self;
}
