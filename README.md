# Numo::NArray - New NArray class library for Ruby/Numo (NUmerical MOdule)

[![Binder](http://mybinder.org/badge.svg)](http://mybinder.org/repo/ruby-numo/numo-narray)
[![Build Status](https://travis-ci.org/ruby-numo/numo-narray.svg?branch=master)](https://travis-ci.org/ruby-numo/numo-narray)

[GitHub](https://github.com/ruby-numo/numo-narray)
 | [RubyGems](https://rubygems.org/gems/numo-narray)

Numo::NArray is an Numerical N-dimensional Array class
for fast processing and easy manipulation of multi-dimensional numerical data,
similar to numpy.ndaray.
This project is the successor to [Ruby/NArray](http://masa16.github.io/narray/).

under development

## Documentation
All documents are primitive.

* [Numo::NArray API Doc](http://ruby-numo.github.io/narray/narray/frames.html)
* [Numo::NArray vs numpy](https://github.com/ruby-numo/numo-narray/wiki/Numo-vs-numpy)
* [Numo::NArray vs ndarray](https://github.com/ruby-numo/numo-narray/wiki/Numo-vs-ndarray)
* [Numo::NArray Overview](https://github.com/ruby-numo/numo-narray/wiki/Numo::NArray%E6%A6%82%E8%A6%81) (in Japanese)

## Related Projects
* [Numo::Linalg](https://github.com/ruby-numo/numo-linalg) - Linear Algebra library with [LAPACK](http://www.netlib.org/lapack/).
* [Numo::GSL](https://github.com/ruby-numo/numo-gsl) - Ruby interface for [GSL (GNU Scientific Library)](http://www.gnu.org/software/gsl/).
* [Numo::FFTW](https://github.com/ruby-numo/numo-fftw) - Ruby/Numo interface to [FFTW (A Discrete Fourier Transform library](http://www.fftw.org/).
* [Numo::FFTE](https://github.com/ruby-numo/numo-ffte) - Ruby interface for [FFTE (A Fast Fourier Transform library with radix-2,3,5)](http://www.ffte.jp/).
* [Numo::Gnuplot](https://github.com/ruby-numo/numo-gnuplot) - Simple and easy-to-use Gnuplot interface.

## Installation
### Requirement
Ruby ver 2.1 and later.

### Ubuntu, Debian, Bash on Windows
```shell
apt install -y git ruby gcc ruby-dev rake make
gem install specific_install
gem specific_install https://github.com/ruby-numo/numo-narray.git
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
For more examples, check out this [narray version of 100 numpy exercises](https://github.com/ruby-numo/numo-narray/wiki/100-narray-exercises) (and the [IRuby Notebook](https://github.com/ruby-numo/numo-narray/blob/master/100-narray-exercises.ipynb)).

## Development

### Build

```shell
ruby setup.rb
```

### Run tests

```shell
bundle install
bundle exec rake test
```

Tips: You may run tests defined in a specified line as:

```shell
bundle exec ruby test/bit_test.rb --location 27
```
