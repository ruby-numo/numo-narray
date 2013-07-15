class_name      "SComplex"
class_alias     "Complex64"
ctype           "scomplex"
real_class_name "SFloat"
real_ctype      "float"

has_math      true
is_bit        false
is_int        false
is_float      true
is_real       false
is_complex    true
is_object     false
is_comparable false

upcast_rb "Integer"
upcast_rb "Float"
upcast_rb "Complex"

upcast "DComplex", "DComplex"
upcast "SComplex", "SComplex"
upcast "DFloat",   "DComplex"
upcast "SFloat",   "SComplex"
upcast "Int64",    "SComplex"
upcast "Int32",    "SComplex"
upcast "Int16",    "SComplex"
upcast "Int8",     "SComplex"
upcast "UInt64",   "SComplex"
upcast "UInt32",   "SComplex"
upcast "UInt16",   "SComplex"
upcast "UInt8",    "SComplex"
