# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require "narray"

Gem::Specification.new do |spec|
  spec.name          = "narray"
  spec.version       = NArray::VERSION
  spec.authors       = ["Masahiro TANAKA"]
  spec.email         = ["masa16.tanaka@gmail.com"]
  spec.description   = "new narray"
  spec.summary       = "dev narray"
  spec.homepage      = ""
  spec.license       = "MIT"

  spec.files         = Dir['lib/**/*.rb'] + Dir['ext/**/*.rb'] + Dir['ext/**/*.h'] + Dir['ext/**/*.c'] + Dir['ext/**/extconf.rb']
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib", "ext"]
  spec.extensions = ["ext/gen/Rakefile", "ext/narray/extconf.rb"]

  spec.add_development_dependency "bundler", "~> 1.3"
  spec.add_development_dependency "rake"
  spec.add_development_dependency "rake-compiler"
end
