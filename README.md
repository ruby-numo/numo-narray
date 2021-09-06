# Numo::NArray
[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/ruby-numo/numo-narray/HEAD?filepath=100-narray-exercises.ipynb)
[![Build Status](https://github.com/ruby-numo/numo-narray/workflows/build/badge.svg)](https://github.com/ruby-numo/numo-narray/actions)

[GitHub](https://github.com/ruby-numo/numo-narray)
 | [RubyGems](https://rubygems.org/gems/numo-narray)

Numo::NArray is an Numerical N-dimensional Array class
for fast processing and easy manipulation of multi-dimensional numerical data,
similar to numpy.ndaray.
This project is the successor to [Ruby/NArray](http://masa16.github.io/narray/).

## Documentation

All documents are primitive.

* [Numo::NArray API Doc](http://ruby-numo.github.io/numo-narray/yard/index.html)
* [Numo::NArray vs numpy](https://github.com/ruby-numo/numo-narray/wiki/Numo-vs-numpy)
* [Numo::NArray vs ndarray](https://github.com/ruby-numo/numo-narray/wiki/Numo-vs-ndarray)
* [Numo::NArray Overview](https://github.com/ruby-numo/numo-narray/wiki/Numo::NArray-Overview-(Japanese)) (in Japanese)

## Related Projects

* [Numo::Linalg](https://github.com/ruby-numo/numo-linalg) - Linear Algebra library with [LAPACK](http://www.netlib.org/lapack/).
* [Numo::GSL](https://github.com/ruby-numo/numo-gsl) - Ruby interface for [GSL (GNU Scientific Library)](http://www.gnu.org/software/gsl/).
* [Numo::FFTW](https://github.com/ruby-numo/numo-fftw) - Ruby/Numo interface to [FFTW (A Discrete Fourier Transform library](http://www.fftw.org/).
* [Numo::FFTE](https://github.com/ruby-numo/numo-ffte) - Ruby interface for [FFTE (A Fast Fourier Transform library with radix-2,3,5)](http://www.ffte.jp/).
* [Numo::Gnuplot](https://github.com/ruby-numo/numo-gnuplot) - Simple and easy-to-use Gnuplot interface.

## Installation

### Requirement

Ruby ver 2.2 and later.

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

For more examples, check out the **narray version of 100 numpy exercises**.
  - [Wiki version](https://github.com/ruby-numo/numo-narray/wiki/100-narray-exercises) of the 100 exercises
  - [Reproducible version](https://mybinder.org/v2/gh/ruby-numo/numo-narray/HEAD?filepath=100-narray-exercises.ipynb) of the 100 exercises (With this, you can try narray without creating an environment locally.)

## Development

### Build

```shell
git clone https://github.com/ruby-numo/numo-narray
cd numo-narray
bundle install
bundle exec rake compile
```

### Run tests

```shell
bundle exec rake test
```

Tips: You may run tests defined in a specified line as:

```shell
bundle exec ruby test/bit_test.rb --location 27
```
