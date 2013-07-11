define_type  "UInt32", "u_int32_t"
define_real  "UInt32", "u_int32_t"

has_math    false
is_bit      false
is_int      true
is_float    false
is_complex  false
is_object   false
is_real     true
is_comparable  true

upcast "DComplex", "DComplex"
upcast "SComplex", "SComplex"
upcast "DFloat", "DFloat"
upcast "SFloat", "SFloat"
upcast "Int64",  "Int64"
upcast "Int32",  "Int32"
upcast "Int16",  "Int32"
upcast "Int8",   "Int32"
upcast "UInt64", "UInt64"
upcast "UInt32"
upcast "UInt16"
upcast "UInt8"
upcast "Complex", "DComplex"
