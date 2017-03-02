class_name      "DFloat"
class_alias     "Float64"
ctype           "double"

has_math      true
is_bit        false
is_int        false
is_unsigned   false
is_float      true
is_complex    false
is_object     false
is_real       true
is_comparable true
is_double_precision true

upcast_rb "Integer"
upcast_rb "Float"
upcast_rb "Complex", "DComplex"

upcast "RObject",  "RObject"
upcast "DComplex", "DComplex"
upcast "SComplex", "DComplex"
upcast "DFloat",   "DFloat"
upcast "SFloat",   "DFloat"
upcast "Int64",    "DFloat"
upcast "Int32",    "DFloat"
upcast "Int16",    "DFloat"
upcast "Int8",     "DFloat"
upcast "UInt64",   "DFloat"
upcast "UInt32",   "DFloat"
upcast "UInt16",   "DFloat"
upcast "UInt8",    "DFloat"
