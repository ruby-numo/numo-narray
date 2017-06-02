set name:                "uint16"
set type_name:           "uint16"
set full_class_name:     "Numo::UInt16"
set class_name:          "UInt16"
set class_var:           "cT"
set ctype:               "u_int16_t"

set has_math:            false
set is_bit:              false
set is_int:              true
set is_unsigned:         true
set is_float:            false
set is_complex:          false
set is_object:           false
set is_real:             true
set is_comparable:       true
set is_double_precision: false
set need_align:          true

upcast_rb "Integer"
upcast_rb "Float", "DFloat"
upcast_rb "Complex", "DComplex"

upcast "RObject",  "RObject"
upcast "DComplex", "DComplex"
upcast "SComplex", "SComplex"
upcast "DFloat", "DFloat"
upcast "SFloat", "SFloat"
upcast "Int64",  "Int64"
upcast "Int32",  "Int32"
upcast "Int16",  "Int16"
upcast "Int8",   "Int16"
upcast "UInt64", "UInt64"
upcast "UInt32", "UInt32"
upcast "UInt16"
upcast "UInt8"
