# Development of Next NArray
## Install
[Enucatl](https://github.com/Enucatl/narray-devel) fixed the installation
procedure, making it standard:
```bash:
gem build narray.gemspec
gem install narray-VERSION.gem
```

## Directories
* ext: C-extension source code.
* gen: Code generator.
* spec: RSpec test code.
* ffte: Sample Wrapper of [FFTE](http://www.ffte.jp/): A Fast Fourier Transform Package
  developed by Prof. Takahashi.
* linalg: Sample Wrapper of [LAPACK](http://www.netlib.org/lapack/).

## Tentative API Document
* [NArray](http://masa16.github.io/narray-devel/narray/frames.html)
* [FFTE](http://masa16.github.io/narray-devel/ffte/frames.html)
