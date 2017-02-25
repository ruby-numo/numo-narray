# Miscellaneous notes for Numo::NArray

## Running RSpec

(in advance, install gem with --development option)

  ```shell
$ "${HOME}/.gem/ruby/2.?/bin/rspec" "${HOME}/.gem/ruby/2.?/gems/numo-narray-0.9.?.?/spec/bit_spec.rb"
$ "${HOME}/.gem/ruby/2.?/bin/rspec" "${HOME}/.gem/ruby/2.?/gems/numo-narray-0.9.?.?/spec/narray_spec.rb"
```

## YARD documents generation

(in advance, install yard gem)

  ```shell
$ cd "${HOME}/.gem/ruby/2.?/gems/numo-narray-0.9.?.?/ext/numo/narray"
$ make doc
yard doc *.c types/*.c
...
```
