set name:                "dfloat"
set type_name:           "dfloat"
set full_class_name:     "Numo::DFloat"
set class_name:          "DFloat"
set class_alias:         "Float64"
set class_var:           "cT"
set ctype:               "double"
set simd_type:           "pd"

set has_math:            true
set is_bit:              false
set is_int:              false
set is_unsigned:         false
set is_float:            true
set is_complex:          false
set is_object:           false
set is_real:             true
set is_comparable:       true
set is_double_precision: true
set need_align:          true

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
