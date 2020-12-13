def_id "cast"
def_id "mulsum"
def_id "to_a"
if is_complex
  def_id "real"
  def_id "imag"
else
  def_id "divmod"
end
if is_float
  def_id "nearly_eq"
  def_id "copysign"
end
if is_int
  def_id "<<","left_shift"
  def_id ">>","right_shift"
end
if is_comparable && !is_object
  def_id "gt"
  def_id "ge"
  def_id "lt"
  def_id "le"
end
if is_comparable
  def_id "nan"
end
if is_object
  def_id "bit_and"
  def_id "bit_or"
  def_id "bit_xor"
  def_id "bit_not"
  def_id "abs"
  def_id "reciprocal"
  def_id "square"
  def_id "floor"
  def_id "round"
  def_id "ceil"
  def_id "truncate"
  def_id "nan?"
  def_id "infinite?"
  def_id "finite?"
  def_id "-@","minus"
  def_id "==","eq"
  def_id "!=","ne"
  def_id ">" ,"gt"
  def_id ">=","ge"
  def_id "<" ,"lt"
  def_id "<=","le"
  def_id "<=>","ufo"
else
  def_id "eq"
  def_id "ne"
end

if is_int && !is_object
  def_id "minlength" # for bincount
end

# Constatnts

if is_bit
  def_const "ELEMENT_BIT_SIZE",  "INT2FIX(1)"
  def_const "ELEMENT_BYTE_SIZE", "rb_float_new(1.0/8)"
  def_const "CONTIGUOUS_STRIDE", "INT2FIX(1)"
else
  def_const "ELEMENT_BIT_SIZE",  "INT2FIX(sizeof(dtype)*8)"
  def_const "ELEMENT_BYTE_SIZE", "INT2FIX(sizeof(dtype))"
  def_const "CONTIGUOUS_STRIDE", "INT2FIX(sizeof(dtype))"
end

if !is_object
  if is_float
    def_const "EPSILON", "M_EPSILON"
  end
  if is_float || is_int
    def_const "MAX", "M_MAX"
    def_const "MIN", "M_MIN"
  end
end

# Un-define

if is_object
  undef_singleton_method "from_binary"
  undef_method "to_binary"
  undef_method "swap_byte"
  undef_method "to_network"
  undef_method "to_vacs"
  undef_method "to_host"
  undef_method "to_swapped"
end

# Allocation

def_alloc_func "alloc_func"
def_method "allocate"

# Type conversion

def_method "extract"
def_method "new_dim0"

def_method "store" do
  extend StoreFrom
  store_numeric
  store_from "Bit"
  if is_complex
    store_from "DComplex","dcomplex","m_from_dcomplex"
    store_from "SComplex","scomplex","m_from_scomplex"
  end
  store_from "DFloat","double",   "m_from_real"
  store_from "SFloat","float",    "m_from_real"
  store_from "Int64", "int64_t",  "m_from_int64"
  store_from "Int32", "int32_t",  "m_from_int32"
  store_from "Int16", "int16_t",  "m_from_sint"
  store_from "Int8",  "int8_t",   "m_from_sint"
  store_from "UInt64","u_int64_t","m_from_uint64"
  store_from "UInt32","u_int32_t","m_from_uint32"
  store_from "UInt16","u_int16_t","m_from_sint"
  store_from "UInt8", "u_int8_t", "m_from_sint"
  store_from "RObject", "VALUE",  "m_num_to_data"
  store_array
end

def_method "extract_data"

def_method "cast_array"
def_singleton_method "cast"

def_method "aref", op:"[]"
def_method "aset", op:"[]="

def_method "coerce_cast"
def_method "to_a"
def_method "fill"
def_method "format"
def_method "format_to_a"
def_method "inspect"


# Array manipulation

def_method "each"
unary "map" if !is_bit
def_method "each_with_index"

if is_bit
  unary  "copy"
  unary  "not", "~"
  binary "and", "&"
  binary "or" , "|"
  binary "xor", "^"
  binary "eq"
  bit_count "count_true"
  def_alias "count_1","count_true"
  def_alias "count","count_true"
  bit_count "count_false"
  def_alias "count_0","count_false"
  bit_reduce "all?", 1
  bit_reduce "any?", 0
  def_method "none?", "none_p"
  def_method "where"
  def_method "where2"
  def_method "mask"
else

def_method "map_with_index"

# Arithmetic

unary2 "abs", "rtype", "cRT"

binary "add", "+"
binary "sub", "-"
binary "mul", "*"
binary "div", "/"

if !is_complex
  binary "mod", "%"
  binary2 "divmod"
end


if !is_bit
  pow
  def_id "**","pow"
  def_alias "pow","**"
end

unary "minus", "-@"
unary "reciprocal"
unary "sign"
unary "square"

# Complex

if is_complex
  unary "conj"
  unary "im"
  unary2 "real", "rtype", "cRT"
  unary2 "imag", "rtype", "cRT"
  unary2 "arg",  "rtype", "cRT"
  def_alias "angle","arg"
  set2 "set_imag", "rtype", "cRT"
  set2 "set_real", "rtype", "cRT"
  def_alias "imag=","set_imag"
  def_alias "real=","set_real"
else
  def_alias "conj", "view"
  def_alias "im", "view"
end

def_alias "conjugate","conj"

# base_cond

cond_binary "eq"
cond_binary "ne"

# nearly_eq  : x=~y is true if |x-y| <= (|x|+|y|)*epsilon
if is_float
  cond_binary "nearly_eq"
else
  def_alias "nearly_eq", "eq"
end
def_alias "close_to", "nearly_eq"

# Integer
if is_int
  binary "bit_and", "&"
  binary "bit_or" , "|"
  binary "bit_xor", "^"
  unary  "bit_not", "~"
  binary "left_shift", "<<"
  binary "right_shift", ">>"
  if !is_object
    def_alias "floor", "view"
    def_alias "round", "view"
    def_alias "ceil",  "view"
    def_alias "trunc", "view"
    def_alias "rint",  "view"
  end
end

if is_float
  unary "floor"
  unary "round"
  unary "ceil"
  unary "trunc"
  if !is_object
    unary "rint"
    binary "copysign"
    if !is_complex
      cond_unary "signbit"
      def_method "modf", "unary_ret2"
    end
  end
end

if is_comparable
  cond_binary "gt"
  cond_binary "ge"
  cond_binary "lt"
  cond_binary "le"
  def_alias ">", "gt"
  def_alias ">=","ge"
  def_alias "<", "lt"
  def_alias "<=","le"
  def_method "clip"
end

# Float

if is_float
  cond_unary "isnan"
  cond_unary "isinf"
  cond_unary "isposinf"
  cond_unary "isneginf"
  cond_unary "isfinite"
end

if is_int && !is_object
  if is_unsigned
    accum "sum","u_int64_t","numo_cUInt64"
    accum "prod","u_int64_t","numo_cUInt64"
  else
    accum "sum","int64_t","numo_cInt64"
    accum "prod","int64_t","numo_cInt64"
  end
else
  accum "sum","dtype","cT"
  accum "prod","dtype","cT"
end

if is_double_precision
  accum "kahan_sum","dtype","cT"
end

if is_float
  accum "mean","dtype","cT"
  accum "stddev","rtype","cRT"
  accum "var","rtype","cRT"
  accum "rms","rtype","cRT"
end

if is_comparable
  accum "min","dtype","cT"
  accum "max","dtype","cT"
  accum "ptp","dtype","cT"
  accum_index "max_index"
  accum_index "min_index"
  accum_arg   "argmax"
  accum_arg   "argmin"
  def_method "minmax"
  def_module_function "maximum", "ewcomp", n_arg:2
  def_module_function "minimum", "ewcomp", n_arg:2
end

if is_int && !is_object
  def_method "bincount"
end

cum "cumsum","add"
cum "cumprod","mul"

# dot
accum_binary "mulsum"

# rmsdev
# prod

# shuffle
# histogram

def_method "seq"
if is_float
  def_method "logseq"
end
def_method "eye"
def_alias  "indgen", "seq"

def_method "rand"
if is_float && !is_object
  def_method "rand_norm"
end

# y = a[0] + a[1]*x + a[2]*x^2 + a[3]*x^3 + ... + a[n]*x^n
def_method "poly"

if is_comparable && !is_object
  if is_float
    qsort type_name,"dtype","*(dtype*)","_prnan"
    qsort type_name,"dtype","*(dtype*)","_ignan"
  else
    qsort type_name,"dtype","*(dtype*)"
  end
  def_method "sort"
  if is_float
    qsort type_name+"_index","dtype*","**(dtype**)","_prnan"
    qsort type_name+"_index","dtype*","**(dtype**)","_ignan"
  else
    qsort type_name+"_index","dtype*","**(dtype**)"
  end
  def_method "sort_index"
  def_method "median"
end

# Math
# histogram

if has_math
fn = get(:full_class_name)
cn = get(:class_name)
nm = get(:name)
st = get(:simd_type)
dp = get(:is_double_precision)
algn = get(:need_align)
is_c = is_complex

def_module do
  extend NMathMethod
  set ns_var: "cT"
  set class_name: cn
  set name: "#{nm}_math"
  set full_module_name: fn+"::NMath"
  set module_name: "Math"
  set module_var: "mTM"
  set simd_type: st
  set is_double_precision: dp
  set is_complex: is_c
  set need_align: algn

  math "sqrt"
  math "cbrt"
  math "log"
  math "log2"
  math "log10"
  math "exp"
  math "exp2"
  math "exp10"
  math "sin"
  math "cos"
  math "tan"
  math "asin"
  math "acos"
  math "atan"
  math "sinh"
  math "cosh"
  math "tanh"
  math "asinh"
  math "acosh"
  math "atanh"
  math "sinc"
  if !is_c
    math "atan2",2
    math "hypot",2
    math "erf"
    math "erfc"
    math "log1p"
    math "expm1"
    math "ldexp",2
    math "frexp",1,"frexp"
  end
end
end

end # other than Bit
