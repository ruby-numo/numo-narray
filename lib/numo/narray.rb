begin
  major, minor, _ = RUBY_VERSION.split(/\./)
  require "#{major}.#{minor}/numo/narray.so"
rescue LoadError
  require "numo/narray.so"
end

require "numo/narray/extra"
