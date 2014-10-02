FC = mpif90
FCFLAGS = -O3 -fomit-frame-pointer -fopenmp -I../..
#FCFLAGS = -O3 -xHost -openmp -I../..

all: ptest1d ptest2d ptest3d ptest3dv prtest2d prtest3d pspeed1d pspeed2d pspeed3d pspeed3dv prspeed2d prspeed3d

ptest1d : ptest1d.o pzfft1d.o zfft1d.o fft235.o kernel.o
	$(FC) $(FCFLAGS) ptest1d.o pzfft1d.o zfft1d.o fft235.o kernel.o -o ptest1d

ptest2d : ptest2d.o pzfft2d.o fft235.o kernel.o
	$(FC) $(FCFLAGS) ptest2d.o pzfft2d.o fft235.o kernel.o -o ptest2d

ptest3d : ptest3d.o pzfft3d.o fft235.o kernel.o
	$(FC) $(FCFLAGS) ptest3d.o pzfft3d.o fft235.o kernel.o -o ptest3d

prtest2d : prtest2d.o pdzfft2d.o pzdfft2d.o fft235.o kernel.o
	$(FC) $(FCFLAGS) prtest2d.o pdzfft2d.o pzdfft2d.o fft235.o kernel.o -o prtest2d

prtest3d : prtest3d.o pdzfft3d.o pzdfft3d.o fft235.o kernel.o
	$(FC) $(FCFLAGS) prtest3d.o pdzfft3d.o pzdfft3d.o fft235.o kernel.o -o prtest3d

ptest3dv : ptest3dv.o pzfft3dv.o fft235.o kernel.o
	$(FC) $(FCFLAGS) ptest3dv.o pzfft3dv.o fft235.o kernel.o -o ptest3dv

ptest1d.o : ptest1d.f ../../param.h
	$(FC) $(FCFLAGS) -c ptest1d.f -o ptest1d.o

ptest2d.o : ptest2d.f ../../param.h
	$(FC) $(FCFLAGS) -c ptest2d.f -o ptest2d.o

ptest3d.o : ptest3d.f ../../param.h
	$(FC) $(FCFLAGS) -c ptest3d.f -o ptest3d.o

ptest3dv.o : ptest3dv.f ../../param.h
	$(FC) $(FCFLAGS) -c ptest3dv.f -o ptest3dv.o

prtest2d.o : prtest2d.f ../../param.h
	$(FC) $(FCFLAGS) -c prtest2d.f -o prtest2d.o

prtest3d.o : prtest3d.f ../../param.h
	$(FC) $(FCFLAGS) -c prtest3d.f -o prtest3d.o

pspeed1d : pspeed1d.o pzfft1d.o zfft1d.o fft235.o kernel.o
	$(FC) $(FCFLAGS) pspeed1d.o pzfft1d.o zfft1d.o fft235.o kernel.o -o pspeed1d

pspeed2d : pspeed2d.o pzfft2d.o fft235.o kernel.o
	$(FC) $(FCFLAGS) pspeed2d.o pzfft2d.o fft235.o kernel.o -o pspeed2d

pspeed3d : pspeed3d.o pzfft3d.o fft235.o kernel.o
	$(FC) $(FCFLAGS) pspeed3d.o pzfft3d.o fft235.o kernel.o -o pspeed3d

pspeed3dv : pspeed3dv.o pzfft3dv.o fft235.o kernel.o
	$(FC) $(FCFLAGS) pspeed3dv.o pzfft3dv.o fft235.o kernel.o -o pspeed3dv

prspeed2d : prspeed2d.o pdzfft2d.o fft235.o kernel.o
	$(FC) $(FCFLAGS) prspeed2d.o pdzfft2d.o fft235.o kernel.o -o prspeed2d

prspeed3d : prspeed3d.o pdzfft3d.o fft235.o kernel.o
	$(FC) $(FCFLAGS) prspeed3d.o pdzfft3d.o fft235.o kernel.o -o prspeed3d

pspeed1d.o : pspeed1d.f ../../param.h
	$(FC) $(FCFLAGS) -c pspeed1d.f -o pspeed1d.o

pspeed2d.o : pspeed2d.f ../../param.h
	$(FC) $(FCFLAGS) -c pspeed2d.f -o pspeed2d.o

pspeed3d.o : pspeed3d.f ../../param.h
	$(FC) $(FCFLAGS) -c pspeed3d.f -o pspeed3d.o

pspeed3dv.o : pspeed3dv.f ../../param.h
	$(FC) $(FCFLAGS) -c pspeed3dv.f -o pspeed3dv.o

prspeed2d.o : prspeed2d.f ../../param.h
	$(FC) $(FCFLAGS) -c prspeed2d.f -o prspeed2d.o

prspeed3d.o : prspeed3d.f ../../param.h
	$(FC) $(FCFLAGS) -c prspeed3d.f -o prspeed3d.o

pzfft1d.o : ../pzfft1d.f ../../param.h
	$(FC) $(FCFLAGS) -c ../pzfft1d.f -o pzfft1d.o

pzfft2d.o : ../pzfft2d.f ../../param.h
	$(FC) $(FCFLAGS) -c ../pzfft2d.f -o pzfft2d.o

pzfft3d.o : ../pzfft3d.f ../../param.h
	$(FC) $(FCFLAGS) -c ../pzfft3d.f -o pzfft3d.o

pzfft3dv.o : ../pzfft3dv.f ../../param.h
	$(FC) $(FCFLAGS) -c ../pzfft3dv.f -o pzfft3dv.o

pdzfft2d.o : ../pdzfft2d.f ../../param.h
	$(FC) $(FCFLAGS) -c ../pdzfft2d.f -o pdzfft2d.o

pzdfft2d.o : ../pzdfft2d.f ../../param.h
	$(FC) $(FCFLAGS) -c ../pzdfft2d.f -o pzdfft2d.o

pdzfft3d.o : ../pdzfft3d.f ../../param.h
	$(FC) $(FCFLAGS) -c ../pdzfft3d.f -o pdzfft3d.o

pzdfft3d.o : ../pzdfft3d.f ../../param.h
	$(FC) $(FCFLAGS) -c ../pzdfft3d.f -o pzdfft3d.o

zfft1d.o : ../../zfft1d.f ../../param.h
	$(FC) $(FCFLAGS) -c ../../zfft1d.f -o zfft1d.o

fft235.o : ../../fft235.f
	$(FC) $(FCFLAGS) -c ../../fft235.f -o fft235.o

kernel.o : ../../kernel.f
	$(FC) $(FCFLAGS) -c ../../kernel.f -o kernel.o

clean:
	rm -f ptest1d ptest2d ptest3d ptest3dv prtest2d prtest3d pspeed1d pspeed2d pspeed3d pspeed3dv prspeed2d prspeed3d *.o
