/*
  FFTE: A Fast Fourier Transform Package
  http://www.ffte.jp/
  Computes Discrete Fourier Transforms of
  1-, 2- and 3- dimensional sequences of length (2^p)*(3^q)*(5^r).
  Copyright(C) 2000-2004,2008-2011 Daisuke Takahashi

  Ruby/NArray wrapper of FFTE
  FFTE is converted to C-code using f2c.
  Copyright(C) 2013 Masahiro Tanaka
*/

#include <ruby.h>
#include "narray.h"
#include "template.h"
//#include "f2c.h"
typedef int integer;

typedef struct {
    dcomplex *a;
    dcomplex *b;
    integer iopt;
} fft_opt_t;

static VALUE rb_mFFTE;
static VALUE eRadixError;

static inline
int is235radix(integer n) {
    if (n<=1) {return 0;}
    while (n % 5 == 0) {n /= 5;}
    while (n % 3 == 0) {n /= 3;}
    return (n & (n-1)) ? 0 : 1;
}

<%
def argmap(arg)
  case arg
  when Numeric
    arg = 1..arg
  end
  arg.map do |x|
    yield(x)
  end.join(",")
end
$funcs = []
%>
