/*
  qsort.c
  Numerical Array Extension for Ruby
    (C) Copyright 2007-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

/*
 *      qsort.c: standard quicksort algorithm
 *
 *      Modifications from vanilla NetBSD source:
 *        Add do ... while() macro fix
 *        Remove __inline, _DIAGASSERTs, __P
 *        Remove ill-considered "swap_cnt" switch to insertion sort,
 *        in favor of a simple check for presorted input.
 *
 *      CAUTION: if you change this file, see also qsort_arg.c
 *
 *      $PostgreSQL: pgsql/src/port/qsort.c,v 1.12 2006/10/19 20:56:22 tgl Exp $
 */

/*      $NetBSD: qsort.c,v 1.13 2003/08/07 16:43:42 agc Exp $   */

/*-
 * Copyright (c) 1992, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *        may be used to endorse or promote products derived from this software
 *        without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.      IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef QSORT_INCL
#define QSORT_INCL
#define Min(x, y)               ((x) < (y) ? (x) : (y))

#define swap(type,a,b) \
    do {type tmp=*(type*)(a); *(type*)(a)=*(type*)(b); *(type*)(b)=tmp;} while(0)

#define vecswap(type, a, b, n) if ((n)>0) swap(type,(a),(b))

#define MED3(a,b,c)                                     \
    (cmpgt(b,a) ?                                       \
     (cmpgt(c,b) ? b : (cmpgt(c,a) ? c : a))            \
     : (cmpgt(b,c) ? b : (cmpgt(c,a) ? a : c)))
#endif

#undef qsort_dtype
#define qsort_dtype <%=dtype%>
#undef qsort_cast
#define qsort_cast <%=dcast%>

void
<%=tp%>_qsort(void *a, size_t n, ssize_t es)
{
    char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
    int  d, r, presorted;

 loop:
    if (n < 7) {
        for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
            for (pl = pm; pl > (char *) a && cmpgt(pl - es, pl);
                 pl -= es)
                swap(qsort_dtype, pl, pl - es);
        return;
    }
    presorted = 1;
    for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es) {
        if (cmpgt(pm - es, pm)) {
            presorted = 0;
            break;
        }
    }
    if (presorted)
        return;
    pm = (char *) a + (n / 2) * es;
    if (n > 7) {
        pl = (char *) a;
        pn = (char *) a + (n - 1) * es;
        if (n > 40) {
            d = (n / 8) * es;
            pl = MED3(pl, pl + d, pl + 2 * d);
            pm = MED3(pm - d, pm, pm + d);
            pn = MED3(pn - 2 * d, pn - d, pn);
        }
        pm = MED3(pl, pm, pn);
    }
    swap(qsort_dtype, a, pm);
    pa = pb = (char *) a + es;
    pc = pd = (char *) a + (n - 1) * es;
    for (;;) {
        while (pb <= pc && (r = cmp(pb, a)) <= 0) {
            if (r == 0) {
                swap(qsort_dtype, pa, pb);
                pa += es;
            }
            pb += es;
        }
        while (pb <= pc && (r = cmp(pc, a)) >= 0) {
            if (r == 0) {
                swap(qsort_dtype, pc, pd);
                pd -= es;
            }
            pc -= es;
        }
        if (pb > pc)
            break;
        swap(qsort_dtype, pb, pc);
        pb += es;
        pc -= es;
    }
    pn = (char *) a + n * es;
    r = Min(pa - (char *) a, pb - pa);
    vecswap(qsort_dtype, a, pb - r, r);
    r = Min(pd - pc, pn - pd - es);
    vecswap(qsort_dtype, pb, pn - r, r);
    if ((r = pb - pa) > es)
        <%=tp%>_qsort(a, r / es, es);
    if ((r = pd - pc) > es) {
        a = pn - r;
        n = r / es;
        goto loop;
    }
}

