require "bundler/gem_tasks"
begin

task :doc do
  dir = "ext/numo/narray"
  src = %w[array.c data.c index.c math.c narray.c rand.c struct.c].
    map{|s| File.join(dir,s)} +
    [File.join(dir,"t_*.c"), "lib/numo/narray/extra.rb"]
  sh "cd ext/numo/narray; ruby extconf.rb; make src"
  sh "rm -rf yard .yardoc; yard doc -o yard -m markdown -r README.md #{src.join(' ')}"
end

require "rake/extensiontask"
require "rake_compiler_dock"
require "shellwords"

spec = Bundler::GemHelper.gemspec

cross_platforms = ["x86-mingw32", "x64-mingw32"]
Rake::ExtensionTask.new("numo/narray", spec) do |ext|
  ext.cross_compile = true
  ext.cross_platform = cross_platforms
end

pkg_dir = "pkg"
windows_gem_paths = cross_platforms.collect do |platform|
  File.join(pkg_dir, "#{spec.full_name}-#{platform}.gem")
end

namespace :build do
  directory pkg_dir

  desc "Build gems for Windows into the pkg directory"
  task :windows => pkg_dir do
    build_dir = "tmp/windows"
    rm_rf build_dir
    mkdir_p build_dir

    commands = [
      ["git", "clone", "file://#{Dir.pwd}/.git", build_dir],
      ["cd", build_dir],
      ["bundle"],
      [
        "rake",
        "RUBY_CC_VERSION=2.5.0:2.4.0:2.3.0:2.2.2:2.1.6",
        "cross",
        "native",
        "gem",
      ],
    ]
    raw_commands = commands.collect do |command|
      Shellwords.join(command)
    end
    raw_command_line = raw_commands.join(" && ")

    RakeCompilerDock.sh(raw_command_line)

    cp(Dir.glob("#{build_dir}/#{pkg_dir}/*.gem"),
       "#{pkg_dir}/")
  end
end

namespace :release do
  task :windows => "build:windows" do
    windows_gem_paths.each do |path|
      ruby("-S", "gem", "push", path)
    end
  end
end

rescue LoadError
end

require 'rake/testtask'
Rake::TestTask.new(:test) do |t|
  t.libs << "test"
  t.libs << "lib"
  t.verbose = false
  t.warning = false
  t.test_files = FileList['test/**/*_test.rb']
end
