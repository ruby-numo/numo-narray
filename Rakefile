require "bundler/gem_tasks"

task :doc do
  dir = "ext/numo/narray"
  src = %w[array.c data.c index.c math.c narray.c rand.c struct.c].
    map{|s| File.join(dir,s)} +
    [File.join(dir,"t_*.c"), "lib/numo/narray/extra.rb"]
  sh "cd ext/numo/narray; ruby extconf.rb; make src"
  sh "rm -rf yard .yardoc; yard doc -o yard -m markdown -r README.md #{src.join(' ')}"
end

require 'rake/testtask'
Rake::TestTask.new(:test) do |t|
  t.libs << "test"
  t.libs << "lib"
  t.verbose = false
  t.warning = false
  t.test_files = FileList['test/**/*_test.rb']
end

require 'rake/extensiontask'
Rake::ExtensionTask.new('numo/narray')
