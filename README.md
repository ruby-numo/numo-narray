# Numo::NArray - New NArray class library for Ruby/Numo (NUmerical MOdule)

[![Build Status](https://travis-ci.org/ruby-numo/narray.svg?branch=master)](https://travis-ci.org/ruby-numo/narray)

under development

## Related Projects
* [Numo::Linalg](https://github.com/ruby-numo/linalg): Linear Algebra library with [LAPACK](http://www.netlib.org/lapack/).
* [Numo::GSL](https://github.com/ruby-numo/gsl): Ruby interface for [GSL (GNU Scientific Library)](http://www.gnu.org/software/gsl/).
* [Numo::FFTE](https://github.com/ruby-numo/ffte): Ruby interface for [FFTE (A Fast Fourier Transform library with radix-2,3,5)](http://www.ffte.jp/).

## Installation
### Ubuntu, Debian
```shell
apt install -y git ruby gcc ruby-dev rake make
git clone git://github.com/ruby-numo/narray
cd narray
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
For more examples, check out this [narray version of 100 numpy exercises](https://github.com/ruby-numo/narray/wiki/100-narray-exercises).

## numo-array status compared to numpy

https://github.com/ruby-numo/narray/wiki/Numo-vs-numpy

## [NArray Tentative API Document](http://ruby-numo.github.io/narray/narray/frames.html)
