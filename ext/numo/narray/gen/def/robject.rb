class_name      "RObject"
ctype           "VALUE"
real_class_name "RObject"
real_ctype      "VALUE"

has_math      false
is_bit        false
is_int        true
is_unsigned   false
is_float      true
is_real       true
is_complex    false
is_object     true
is_comparable true
is_double_precision false

upcast_rb "Integer"
upcast_rb "Float"
upcast_rb "Complex"

upcast "DComplex", "RObject"
upcast "SComplex", "RObject"
upcast "DFloat",   "RObject"
upcast "SFloat",   "RObject"
upcast "Int64",    "RObject"
upcast "Int32",    "RObject"
upcast "Int16",    "RObject"
upcast "Int8",     "RObject"
upcast "UInt64",   "RObject"
upcast "UInt32",   "RObject"
upcast "UInt16",   "RObject"
upcast "UInt8",    "RObject"
