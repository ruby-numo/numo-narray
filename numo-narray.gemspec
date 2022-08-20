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

  if RUBY_VERSION < '2.3' # Ruby 2.2.x
    spec.add_development_dependency "bundler", "~> 1.3", "< 1.14.0"
  else
    spec.add_development_dependency "bundler", ">= 2.2.33"
  end
  spec.add_development_dependency "rake", ">= 12.3.3"
  spec.add_development_dependency "rake-compiler", "~> 1.1"
  spec.add_development_dependency "test-unit"
end
