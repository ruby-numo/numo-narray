#! /usr/bin/env ruby

libpath = File.absolute_path(File.dirname(__FILE__))+"/../../../../lib"
$LOAD_PATH.unshift libpath

require "erbpp/narray_def"
if ARGV[0] == "-l"
  require "erbpp/line_number"
  ARGV.shift
end

unless (1..2) === ARGV.size
  puts "usage:\n  ruby #{$0} [-l] erb_path [type_file]"
  exit 1
end

erb_path, type_file = ARGV
DataType.new(erb_path, type_file).run
