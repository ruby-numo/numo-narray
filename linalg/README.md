# NArray::Linalg : LAPACK wrapper

## Implemented Methods

    x = NArray::Linalg.matmul(a,b)   (_gemm) Matrix multiply
    x = NArray::Linalg.solve(a,b)    (_gesv) Solve Linear equation usin LU
    x,y = NArray::Linalg.eigen(a,b)  (_geev) Eigen value and Eigen vector

* [LAPACK](http://www.netlib.org/lapack/).
* [GitHub](https://github.com/masa16/narray-devel/tree/master/linalg)
  (in narray-devel)
