
- [x] copy to buffer if non-contiguous.gc
- [x] contract contiguous dimesions.gc

- [ ] transpose and flatten reduce dimeinsion to last user dimension. -- almost done.

- [ ] NArray with 1-d index and md-shape??
- [ ] enable a[a.sort_index] == a.sort for multi-dimensional array -- perhaps done.

- [ ] specify Input/Overwrite array -- already has OVERWRITE

- [x] accept int-array as index argument of aref.gc

- [x] convert binary from/to Stringgc

- [x] Complex#seq allows complex stepgc

- [ ] Frozen array -- checking OBJ_FROZEN.

- [ ] Marshaling -- perhaps done except Bit, Struct.

- [x] inspect Enumerable#step objectgc

- [x] contiguous checkgc

- [x] propagate_nan for min,max,sum,...gc

- [ ] parsing keyword arguments -- use ruby api.

- [x] use erbpp2gc

- [x] switch to TypedData.gc

- [x] how to get the element size value effectivelygc

- [x] introduce keyword :keepdims (keep dimension/axis) for reduce_dimension.gc

- [x] refactoring array.c: avoid using na_compose_t and na_ary_composition.gc

- [x] reshape!gc


- [ ] flatten_dim

- [ ] name of new_zeros, zeros_like etc.

- [ ] NMath.sin => DFloat.sin ??

- [ ] contiguous if na_view->stridx == NULL ?

- [ ] store_binary with bite_swap.

- [ ] write barrier?

- [ ] move "reduce" field from RNArray to RNArrayView?

- [ ] unify RNArrayBase and RNArrayView structure??


- [ ] Thread-locked array??

- [ ] use rb_thread_call_without_gvl??


- [ ] alignment

- [ ] force buffering option

- [ ] update MT Random Number Generator

- [ ] GVL release
- [ ] Ctrl-C stop during method

- [ ] floor method should be same as Ruby floor or C99 floor ??

# constants

- [ ] NAN
- [ ] INF
- [ ] EPS

# Matlab method

- [x] logspace
- [ ] meshgrid
- [x] rot90  
- [x] rad2deg
- [x] deg2rad
- [x] expm1
- [ ] histcount

# from numpy

- [x] <<,>> 
- [x] swapaxis
- [ ] squeeze
- [x] trace
- [x] ptp 
- [x] clip 
- [ ] ravel
- [ ] take
- [ ] put
- [ ] choose choise
- [ ] select
- [ ] partition
- [ ] searchsorted / bsearch
- [ ] compress
- [ ] pad
- [x] tile
- [x] copysign
- [x] bincount
- [ ] unique
- [ ] roll
- [x] repeat
- [ ] logical_and/or/not - Bit & | ^
- [ ] minimum,maximum
- [ ] shuffle
- [ ] interp
- [ ] intersect1d
- [ ] asarray  - cast
- [ ] persentile

# Math

- [ ] csc,sec,cot
- [ ] csch,sech,coth
- [ ] acsc,asec,acot
- [ ] acsch,asech,acoth
