set name:                "dcomplex"
set type_name:           "dcomplex"
set full_class_name:     "Numo::DComplex"
set class_name:          "DComplex"
set class_alias:         "Complex64"
set class_var:           "cT"
set ctype:               "dcomplex"
set real_class_name:     "DFloat"
set real_ctype:          "double"

set has_math:            true
set is_bit:              false
set is_int:              false
set is_unsigned:         false
set is_float:            true
set is_real:             false
set is_complex:          true
set is_object:           false
set is_comparable:       false
set is_double_precision: true
set need_align:          true

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
