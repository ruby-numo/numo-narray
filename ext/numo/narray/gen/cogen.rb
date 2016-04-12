#! /usr/bin/env ruby

require_relative "erbpp"
require_relative "narray_def"
if ARGV[0] == "-l"
  require_relative "line_number"
  ARGV.shift
end

unless (1..2) === ARGV.size
  puts "usage:\n  ruby #{$0} [-l] erb_path [type_file]"
  exit 1
end

erb_path, type_file = ARGV
DataType.new(erb_path, type_file).run
