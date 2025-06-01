# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)

open("ext/numo/narray/numo/narray.h") do |f|
  f.each_line do |l|
    if /NARRAY_VERSION "([\d.]+)"/ =~ l
      NARRAY_VERSION = $1
      break
    end
  end
end

Gem::Specification.new do |spec|
  spec.name          = "numo-narray"
  spec.version       = NARRAY_VERSION
  spec.authors       = ["Masahiro TANAKA"]
  spec.email         = ["masa16.tanaka@gmail.com"]
  spec.description   = %q{Numo::NArray - New NArray class library in Ruby/Numo.}
  spec.summary       = %q{alpha release of Numo::NArray - New NArray class library in Ruby/Numo (NUmerical MOdule)}
  spec.homepage      = "https://github.com/ruby-numo/numo-narray"
  spec.license       = "BSD-3-Clause"
  spec.required_ruby_version = '>= 2.2'

  spec.files         = `git ls-files Gemfile README.md Rakefile lib ext numo-narray.gemspec spec`.split($/)
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib"]
  spec.extensions    = ["ext/numo/narray/extconf.rb"]
end
