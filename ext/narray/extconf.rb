require 'mkmf'

$CFLAGS="-g -O0 -Wall"
##$CFLAGS=" $(cflags) -m64 -msse2 -funroll-loops"
#$CFLAGS=" $(cflags) -O3"
$INCFLAGS += " -I../narray-types"

srcs = %w(
narray
array
step
index
ndloop
data
math
SFMT
struct
rand
)

# util method
def create_conf_h(file)
  print "creating #{file}\n"
  hfile = open(file, "w")
  for line in $defs
    line =~ /^-D(.*)/
    hfile.printf "#define %s 1\n", $1
  end
  hfile.close
end

if have_header("sys/types.h")
  header = "sys/types.h"
else
  header = nil
end

have_type("boolean", header)
have_type("int32_t", header)
unless have_type("u_int32_t", header)
 have_type("uint32_t",header)
end
have_type("int64_t", header)
unless have_type("u_int64_t", header)
 have_type("uint64_t", header)
end

have_var("rb_cComplex")

$objs = srcs.collect{|i| i+".o"}

create_conf_h("narray_config.h")

create_makefile('narray/narray')
