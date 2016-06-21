require 'rbconfig.rb'

#RbConfig::MAKEFILE_CONFIG["optflags"] = "-g3 -gdwarf-2"

require 'mkmf'

require "erb"

if RUBY_VERSION < "2.0.0"
  puts "Numo::NArray requires Ruby version 2.0 or later."
  exit(1)
end

#$CFLAGS="-g3 -O0 -Wall"
#$CFLAGS=" $(cflags) -O3 -m64 -msse2 -funroll-loops"
#$CFLAGS=" $(cflags) -O3"
$INCFLAGS = "-Itypes #$INCFLAGS"

$INSTALLFILES = Dir.glob(%w[numo/*.h numo/types/*.h]).map{|x| [x,'$(archdir)'] }
if /cygwin|mingw/ =~ RUBY_PLATFORM
  $INSTALLFILES << ['libnarray.a', '$(archdir)']
end

srcs = %w(
narray
array
step
index
ndloop
data
types/bit
types/int8
types/int16
types/int32
types/int64
types/uint8
types/uint16
types/uint32
types/uint64
types/sfloat
types/dfloat
types/scomplex
types/dcomplex
types/robject
math
SFMT
struct
rand
)

=begin
have_header("atlas/cblas.h")
have_library("atlas")

if have_library("blas")
  if have_library("lapack")
    srcs.push "linalg"
    $defs.push "-DHAVE_LAPACK"
  else
    #$defs.delete "-DHAVE_LAPACK"
  end
end
=end

if have_header("stdbool.h")
  stdbool = "stdbool.h"
else
  stdbool = nil
end

if have_header("stdint.h")
  stdint = "stdint.h"
elsif have_header("sys/types.h")
  stdint = "sys/types.h"
else
  stdint = nil
end

have_type("bool", stdbool)
unless have_type("u_int8_t", stdint)
  have_type("uint8_t",stdint)
end
unless have_type("u_int16_t", stdint)
  have_type("uint16_t",stdint)
end
have_type("int32_t", stdint)
unless have_type("u_int32_t", stdint)
  have_type("uint32_t",stdint)
end
have_type("int64_t", stdint)
unless have_type("u_int64_t", stdint)
  have_type("uint64_t", stdint)
end
#have_library("m")
#have_func("sincos")
#have_func("asinh")
have_func("exp10")

have_var("rb_cComplex")
have_var("rb_cFixnum")
#have_func("rb_alloc_tmp_buffer", "ruby.h")
#have_func("rb_free_tmp_buffer", "ruby.h")

$objs = srcs.collect{|i| i+".o"}

create_header

depend_path = File.join(__dir__, "depend")
File.open(depend_path, "w") do |depend|
  depend_erb_path = File.join(__dir__, "depend.erb")
  File.open(depend_erb_path, "r") do |depend_erb|
    erb = ERB.new(depend_erb.read)
    erb.filename = depend_erb_path
    depend.print(erb.result)
  end
end

create_makefile('numo/narray')
