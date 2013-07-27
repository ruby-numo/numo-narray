require 'rbconfig.rb'

#RbConfig::MAKEFILE_CONFIG["optflags"] = "-g3 -gdwarf-2"

require 'mkmf'

$CFLAGS="-g -O0"
$INCFLAGS = "-Itypes #$INCFLAGS"

srcs = %w(
ndloop
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
#have_library("m")
#have_func("sincos")
#have_func("asinh")

$objs = srcs.collect{|i| i+".o"}

create_conf_h("narray_config.h")

create_makefile('narray')
