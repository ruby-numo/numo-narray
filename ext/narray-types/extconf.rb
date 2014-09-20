require 'mkmf'
$CFLAGS="-g -O0 -Wall"
$INCFLAGS += " -I../narray"

srcs = %w(
bit
int8
int16
int32
int64
uint8
uint16
uint32
uint64
sfloat
dfloat
scomplex
dcomplex
robject
)

$objs = srcs.collect{|i| i + ".o"}

create_makefile('narray-types')
