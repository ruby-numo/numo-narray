require "bundler/gem_tasks"
begin
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

  windows_gem_paths.each do |path|
    file path do
      Rake::Task["build:windows"].invoke
    end
  end
end

namespace :release do
  task :windows => windows_gem_paths do
    windows_gem_paths.each do |path|
      ruby("-S", "gem", "push", path)
    end
  end
end

rescue LoadError
end
