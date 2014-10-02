/*
  narray.h
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2003 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#ifndef TEMPLATE_H
#define TEMPLATE_H

#define INIT_COUNTER( lp, c )                   \
    {   c = (lp)->n[0]; }

#define INIT_PTR( lp, i, pt, st )                               \
    {                                                           \
        pt = ((lp)->args[i]).ptr + ((lp)->iter[i]).pos;         \
        st = ((lp)->iter[i]).step;                              \
    }

#define INIT_PTR_IDX( lp, i, pt, st, id )                       \
    {                                                           \
        pt = ((lp)->args[i]).ptr + ((lp)->iter[i]).pos;         \
        st = ((lp)->iter[i]).step;                              \
        id = ((lp)->iter[i]).idx;                               \
    }

#define INIT_ELMSIZE( lp, i, es )                               \
    {                                                           \
        es = ((lp)->args[i]).elmsz;                             \
    }

#define INIT_PTR_BIT( lp, i, ad, ps, st, id )           \
    {                                                   \
        ps = ((lp)->iter[i]).pos;                       \
        ad = (BIT_DIGIT*)(((lp)->args[i]).ptr) + ps/NB; \
        ps %= NB;                                       \
        st = ((lp)->iter[i]).step;                      \
        id = ((lp)->iter[i]).idx;                       \
    }

#define GET_DATA( ptr, type, val )                 \
    {                                              \
        val = *(type*)(ptr);                       \
    }

#define SET_DATA( ptr, type, val )                 \
    {                                              \
        *(type*)(ptr) = val;                       \
    }

#define GET_DATA_STRIDE( ptr, step, type, val )    \
    {                                              \
        val = *(type*)(ptr);                       \
        ptr += step;                               \
    }

#define GET_DATA_INDEX( ptr, idx, type, val )     \
    {                                           \
        val = *(type*)(ptr + *idx);             \
        idx++;                                  \
    }

#define SET_DATA_STRIDE( ptr, step, type, val ) \
    {                                           \
        *(type*)(ptr) = val;                    \
        ptr += step;                            \
    }

#define SET_DATA_INDEX( ptr, idx, type, val )   \
    {                                           \
        *(type*)(ptr + *idx) = val;             \
        idx++;                                  \
    }

#define LOAD_BIT( adr, pos, val )                       \
    {                                                   \
        size_t dig = (pos) / NB;                        \
        int    bit = (pos) % NB;                        \
        val = (((BIT_DIGIT*)(adr))[dig]>>(bit)) & 1u;   \
    }

#define LOAD_BIT_STEP( adr, pos, step, idx, val )       \
    {                                                   \
        size_t dig; int bit;                            \
        if (idx) {                                      \
            dig = ((pos) + *(idx)) / NB;                \
            bit = ((pos) + *(idx)) % NB;                \
            idx++;                                      \
        } else {                                        \
            dig = (pos) / NB;                           \
            bit = (pos) % NB;                           \
            pos += step;                                \
        }                                               \
        val = (((BIT_DIGIT*)(adr))[dig]>>bit) & 1u;     \
    }

#define STORE_BIT(adr,pos,val)                  \
    {                                           \
        size_t dig = (pos) / NB;                \
        int    bit = (pos) % NB;                \
        ((BIT_DIGIT*)(adr))[dig] =              \
            (((BIT_DIGIT*)(adr))[dig] & ~(1u<<(bit))) | ((val)<<(bit)); \
    }
// val -> val&1 ??

#define STORE_BIT_STEP( adr, pos, step, idx, val )\
    {                                           \
        size_t dig; int bit;                    \
        if (idx) {                              \
            dig = ((pos) + *(idx)) / NB;        \
            bit = ((pos) + *(idx)) % NB;        \
            idx++;                              \
        } else {                                \
            dig = (pos) / NB;                   \
            bit = (pos) % NB;                   \
            pos += step;                        \
        }                                       \
        ((BIT_DIGIT*)(adr))[dig] =              \
            (((BIT_DIGIT*)(adr))[dig] & ~(1u<<(bit))) | ((val)<<(bit)); \
    }
// val -> val&1 ??

#endif /* ifndef NARRAY_H */
