require 'rbconfig.rb'

#RbConfig::MAKEFILE_CONFIG["optflags"] = "-g3 -gdwarf-2"

require 'mkmf'

$CFLAGS="-g -O0"
$INCFLAGS = "-I../../ext -I../../ext/types #$INCFLAGS"

srcs = %w(
linalg
linalg_d
linalg_s
)
$objs = srcs.collect{|i| i+".o"}
fflags = ""

# # GNU FORTRAN v4
# if have_library("gfortran")
#   $defs.push "-fPIC -DGNU_FORTRAN"
#   fc = "gfortran"
#   if false # have_library("gomp")
#     fflags += "-fopenmp"
#   end
# #
# # GNU FORTRAN v3
# elsif have_library("g77")
#   $defs.push "-fPIC -DGNU_FORTRAN"
#   fc = "g77"
# elsif have_library('f2c')
#   $defs.push "-DF2C"
# else
#   puts "failed"
#   exit
# end

have_library("blas")
have_library("lapack")
create_makefile('linalg')

# if $makefile_created
#   puts "appending extra install tasks to Makefile"
#   File.open("Makefile","a") do |w|
#     w.print <<EOL
# F77 = #{fc}
# FFLAGS = -O3 -fPIC -Iffte-5.0 #{fflags} -fomit-frame-pointer
# EOL
#   end
# end
