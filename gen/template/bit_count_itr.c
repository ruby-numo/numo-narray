static void
<%=c_iterator%>_<%=int_t%>(na_loop_t *const lp)
{
    size_t  i;
    BIT_DIGIT *a1;
    size_t  p1;
    char   *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    BIT_DIGIT x=0;
    <%=int_t%>  y;
    <%=int_t%> *p;

    INIT_COUNTER(lp, i);
    INIT_PTR_BIT(lp, 0, a1, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    if (idx1||idx2) {
        for (; i--;) {
            LOAD_BIT_STEP(a1, p1, s1, idx1, x);
            if (idx2) {
                p = (<%=int_t%>*)(p2 + *idx2);
                idx2++;
            } else {
                p = (<%=int_t%>*)(p2);
                p2 += s2;
            }
                y = *p;
            if (condition) {
                y++;
                *p = y;
            }
        }
    } else {
        for (; i--;) {
            LOAD_BIT(a1, p1, x);
            p1+=s1;
            y = *(<%=int_t%>*)p2;
            if (condition) {
                y++;
                *(<%=int_t%>*)p2 = y;
            }

            p2+=s2;
        }
    }
}

