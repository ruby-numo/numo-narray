class_name      "DComplex"
class_alias     "Complex128"
ctype           "dcomplex"
real_class_name "DFloat"
real_ctype      "double"

has_math      true
is_bit        false
is_int        false
is_float      true
is_real       false
is_complex    true
is_object     false
is_comparable false
is_double_precision true

upcast_rb "Integer"
upcast_rb "Float"
upcast_rb "Complex"

upcast "RObject",  "RObject"
upcast "DComplex", "DComplex"
upcast "SComplex", "DComplex"
upcast "DFloat",   "DComplex"
upcast "SFloat",   "DComplex"
upcast "Int64",    "DComplex"
upcast "Int32",    "DComplex"
upcast "Int16",    "DComplex"
upcast "Int8",     "DComplex"
upcast "UInt64",   "DComplex"
upcast "UInt32",   "DComplex"
upcast "UInt16",   "DComplex"
upcast "UInt8",    "DComplex"
