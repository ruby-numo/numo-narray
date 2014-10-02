F77 = f95
FFLAGS = -O3 -fomit-frame-pointer -fopenmp -I..
#F77 = ifort
#FFLAGS = -O3 -xHost -openmp -I..

all: test1d test2d test3d rtest2d rtest3d speed1d speed2d speed3d rspeed2d rspeed3d

test1d : test1d.o zfft1d.o fft235.o kernel.o
	$(F77) $(FFLAGS) test1d.o zfft1d.o fft235.o kernel.o -o test1d

test2d : test2d.o zfft2d.o fft235.o kernel.o
	$(F77) $(FFLAGS) test2d.o zfft2d.o fft235.o kernel.o -o test2d

test3d : test3d.o zfft3d.o fft235.o kernel.o
	$(F77) $(FFLAGS) test3d.o zfft3d.o fft235.o kernel.o -o test3d

rtest2d : rtest2d.o dzfft2d.o zdfft2d.o fft235.o kernel.o
	$(F77) $(FFLAGS) rtest2d.o dzfft2d.o zdfft2d.o fft235.o kernel.o -o rtest2d

rtest3d : rtest3d.o dzfft3d.o zdfft3d.o fft235.o kernel.o
	$(F77) $(FFLAGS) rtest3d.o dzfft3d.o zdfft3d.o fft235.o kernel.o -o rtest3d

test1d.o : test1d.f ../param.h
	$(F77) $(FFLAGS) -c test1d.f -o test1d.o

test2d.o : test2d.f ../param.h
	$(F77) $(FFLAGS) -c test2d.f -o test2d.o

test3d.o : test3d.f ../param.h
	$(F77) $(FFLAGS) -c test3d.f -o test3d.o

rtest2d.o : rtest2d.f ../param.h
	$(F77) $(FFLAGS) -c rtest2d.f -o rtest2d.o

rtest3d.o : rtest3d.f ../param.h
	$(F77) $(FFLAGS) -c rtest3d.f -o rtest3d.o

speed1d : speed1d.o zfft1d.o fft235.o kernel.o
	$(F77) $(FFLAGS) speed1d.o zfft1d.o fft235.o kernel.o -o speed1d

speed2d : speed2d.o zfft2d.o fft235.o kernel.o
	$(F77) $(FFLAGS) speed2d.o zfft2d.o fft235.o kernel.o -o speed2d

speed3d : speed3d.o zfft3d.o fft235.o kernel.o
	$(F77) $(FFLAGS) speed3d.o zfft3d.o fft235.o kernel.o -o speed3d

rspeed2d : rspeed2d.o dzfft2d.o fft235.o kernel.o
	$(F77) $(FFLAGS) rspeed2d.o dzfft2d.o fft235.o kernel.o -o rspeed2d

rspeed3d : rspeed3d.o dzfft3d.o fft235.o kernel.o
	$(F77) $(FFLAGS) rspeed3d.o dzfft3d.o fft235.o kernel.o -o rspeed3d

speed1d.o : speed1d.f ../param.h
	$(F77) $(FFLAGS) -c speed1d.f -o speed1d.o

speed2d.o : speed2d.f ../param.h
	$(F77) $(FFLAGS) -c speed2d.f -o speed2d.o

speed3d.o : speed3d.f ../param.h
	$(F77) $(FFLAGS) -c speed3d.f -o speed3d.o

rspeed2d.o : rspeed2d.f ../param.h
	$(F77) $(FFLAGS) -c rspeed2d.f -o rspeed2d.o

rspeed3d.o : rspeed3d.f ../param.h
	$(F77) $(FFLAGS) -c rspeed3d.f -o rspeed3d.o

zfft1d.o : ../zfft1d.f ../param.h
	$(F77) $(FFLAGS) -c ../zfft1d.f -o zfft1d.o

zfft2d.o : ../zfft2d.f ../param.h
	$(F77) $(FFLAGS) -c ../zfft2d.f -o zfft2d.o

zfft3d.o : ../zfft3d.f ../param.h
	$(F77) $(FFLAGS) -c ../zfft3d.f -o zfft3d.o

dzfft2d.o : ../dzfft2d.f ../param.h
	$(F77) $(FFLAGS) -c ../dzfft2d.f -o dzfft2d.o

zdfft2d.o : ../zdfft2d.f ../param.h
	$(F77) $(FFLAGS) -c ../zdfft2d.f -o zdfft2d.o

dzfft3d.o : ../dzfft3d.f ../param.h
	$(F77) $(FFLAGS) -c ../dzfft3d.f -o dzfft3d.o

zdfft3d.o : ../zdfft3d.f ../param.h
	$(F77) $(FFLAGS) -c ../zdfft3d.f -o zdfft3d.o

fft235.o : ../fft235.f
	$(F77) $(FFLAGS) -c ../fft235.f -o fft235.o

kernel.o : ../kernel.f
	$(F77) $(FFLAGS) -c ../kernel.f -o kernel.o

clean:
	rm -f test1d test2d test3d rtest2d rtest3d speed1d speed2d speed3d rspeed2d rspeed3d *.o
