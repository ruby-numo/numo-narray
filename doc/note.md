# Miscellaneous notes for Numo::NArray

## Running Tests

```shell
# All tests
bundle exec rake test

# Specific test
bundle exec ruby test/narray_test.rb
bundle exec ruby test/bit_test.rb
```

## YARD documents generation

(in advance, install yard gem)

  ```shell
$ cd "${HOME}/.gem/ruby/2.?/gems/numo-narray-0.9.?.?/ext/numo/narray"
$ make doc
yard doc *.c types/*.c
...
```
