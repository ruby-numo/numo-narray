require 'rbconfig.rb'

#RbConfig::MAKEFILE_CONFIG["optflags"] = "-g3 -gdwarf-2"

require 'mkmf'

# $CFLAGS="-O0"
$INCFLAGS = "-I../narray #$INCFLAGS"

srcs = %w(
fft
fftsg
)
$objs = srcs.collect{|i| i+".o"}

create_makefile('fft')
