require "bundler/gem_tasks"
require "rake/extensiontask"
require "rake_compiler_dock"
require "shellwords"

spec = Bundler::GemHelper.gemspec

Rake::ExtensionTask.new("numo/narray", spec) do |ext|
  ext.cross_compile = true
  ext.cross_platform = ["x86-mingw32", "x64-mingw32"]
end

namespace :build do
  pkg_dir = "pkg"
  directory pkg_dir

  desc "Build gems for Windows into the pkg directory"
  task :windows => pkg_dir do
    ruby_versions = "2.1.6:2.2.2:2.3.0"

    build_dir = "tmp/windows"
    rm_rf build_dir
    mkdir_p build_dir

    commands = [
      ["git", "clone", "file://#{Dir.pwd}/.git", build_dir],
      ["cd", build_dir],
      ["bundle"],
      ["rake", "cross", "native", "gem", "RUBY_CC_VERSION=#{ruby_versions}"],
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
