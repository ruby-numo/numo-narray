static dtype pow_<%=type_name%>(dtype x, int p)
{
    dtype r = m_one;
    switch(p) {
    case 2: return m_square(x);
    case 3: return m_mul(m_square(x),x);
    case 1: return x;
    case 0: return m_one;
    }
    if (p<0)  return m_zero;
    while (p) {
        if ((p%2) == 1) r = m_mul(r,x);
        x = m_square(x);
        p /= 2;
    }
    return r;
}
