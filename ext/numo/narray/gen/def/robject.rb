set name:                "robject"
set type_name:           "robject"
set full_class_name:     "Numo::RObject"
set class_name:          "RObject"
set class_var:           "cT"
set ctype:               "VALUE"
set real_class_name:     "RObject"
set real_ctype:          "VALUE"

set has_math:            false
set is_bit:              false
set is_int:              true
set is_unsigned:         false
set is_float:            true
set is_real:             true
set is_complex:          false
set is_object:           true
set is_comparable:       true
set is_double_precision: false
set need_align:          false

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
