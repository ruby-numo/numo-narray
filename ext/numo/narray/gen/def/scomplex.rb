set name:                "scomplex"
set type_name:           "scomplex"
set full_class_name:     "Numo::SComplex"
set class_name:          "SComplex"
set class_alias:         "Complex32"
set class_var:           "cT"
set ctype:               "Scomplex"
set real_class_name:     "SFloat"
set real_ctype:          "float"

set has_math:            true
set is_bit:              false
set is_int:              false
set is_unsigned:         false
set is_float:            true
set is_real:             false
set is_complex:          true
set is_object:           false
set is_comparable:       false
set is_double_precision: false
set need_align:          true

upcast_rb "Integer"
upcast_rb "Float"
upcast_rb "Complex"

upcast "RObject",  "RObject"
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
