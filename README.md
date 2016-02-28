# Numo::NArray - New NArray class library for Ruby/Numo (NUmerical MOdule)
under development

## Directories
* ext: C-extension source code.
* gen: Code generator.
* spec: RSpec test code.
* ffte: Sample Wrapper of [FFTE](http://www.ffte.jp/): A Fast Fourier Transform Package
  developed by Prof. Takahashi.
* linalg: Sample Wrapper of [LAPACK](http://www.netlib.org/lapack/).

## Installation
### Ubuntu, Debian
```shell
apt install -y git ruby gcc ruby-dev rake make
git clone git://github.com/ruby-numo/numo-narray
cd numo-narray
gem build numo-narray.gemspec
gem install numo-narray-0.9.0.1.gem
```

## Quick start
An example
```ruby
[1] pry(main)> require "numo/narray"
=> true
[2] pry(main)> a = Numo::DFloat.new(3,5).seq
=> Numo::DFloat#shape=[3,5]
[[0, 1, 2, 3, 4],
 [5, 6, 7, 8, 9],
 [10, 11, 12, 13, 14]]
[3] pry(main)> a.shape
=> [3, 5]
[4] pry(main)> a.ndim
=> 2
[5] pry(main)> a.class
=> Numo::DFloat
[6] pry(main)> a.size
=> 15
```


## Tentative API Document
* [NArray](http://ruby-numo.github.io/numo-narray/narray/frames.html)
* [FFTE](http://ruby-numo.github.io/numo-narray/ffte/frames.html)
