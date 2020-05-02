#! /usr/bin/env ruby

# Build gems for Windows by using fake RbConfig::CONFIG by rake-compiler.
fake_path  = File.join(Dir.pwd, 'fake.rb')
if File.exist? fake_path
  $:.unshift(Dir.pwd)
  require 'fake'
end

thisdir = File.dirname(__FILE__)
libpath = File.absolute_path(File.dirname(__FILE__))+"/../../../../lib"
$LOAD_PATH.unshift libpath

require_relative "narray_def"

$line_number = false

while true
  if ARGV[0] == "-l"
    $line_number = true
    ARGV.shift
  elsif ARGV[0] == "-o"
    ARGV.shift
    $output = ARGV.shift
    require "fileutils"
    FileUtils.rm_f($output)
  else
    break
  end
end

if ARGV.size != 1
  puts "usage:\n  ruby #{$0} [-l] erb_base [type_file]"
  exit 1
end

type_file, = ARGV
type_name = File.basename(type_file,".rb")

erb_dir = ["tmpl"]
erb_dir.unshift("tmpl_bit") if (type_name == "bit")
erb_dir.map!{|d| File.join(thisdir,d)}

code = DefLib.new do
  set line_number: $line_number
  set erb_dir: erb_dir
  set erb_suffix: ".c"
  set ns_var: "mNumo"

  set file_name: $output||""
  set include_files: ["numo/types/#{type_name}.h"]
  set lib_name: "numo_"+type_name

  if (::RbConfig::CONFIG['target_cpu'] == 'x86_64') or  (::RbConfig::CONFIG['target_cpu'] == 'x64')
    set is_simd: true
  else
    set is_simd: false
  end

  def_class do
    extend NArrayMethod
    extend NArrayType
    eval File.read(type_file), binding, type_file
    eval File.read(File.join(thisdir,"spec.rb")), binding, "spec.rb"
  end
end.result

if $output
  File.write($output, code)
else
  $stdout.write(code)
end
