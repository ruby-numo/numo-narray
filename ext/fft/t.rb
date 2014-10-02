require "../ext/narray"
require "./fft"

p n = 2**24
p n = 12345
p n = 2**20
p a = NArray::DComplex[1..n]
p x = FFT.cdft(a,1)
p y = FFT.cdft(x,-1)
p (a-y/n).abs.max

p a = NArray::DFloat[1..n]
p x = FFT.rdft(a,1)
p y = FFT.rdft(x,-1)
p (a-y*(2.0/n)).abs.max

p a = NArray::DFloat[1..n]
p x = FFT.ddct(a,1)
p y = FFT.ddct(x,-1)
y[0] /= 2
p (a-y*(2.0/n)).abs.max

p a = NArray::DFloat[1..n]
p x = FFT.ddst(a,1)
p y = FFT.ddst(x,-1)
y[0] /= 2
p (a-y*(2.0/n)).abs.max
