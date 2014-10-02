FFTE: A Fast Fourier Transform Package

Description:
    A package to compute Discrete Fourier Transforms of
    1-, 2- and 3- dimensional sequences of length (2^p)*(3^q)*(5^r).

Files:
    dzfft2d.f   : 2-D real-to-complex FFT routine
    dzfft3d.f   : 3-D real-to-complex FFT routine
    fft235.f    : Radix-2,3,4,5 and 8 FFT routine
    kernel.f    : Radix-2,3,4,5 and 8 FFT kernel routine
    mfft235.f   : Radix-2,3,4,5 and 8 multiple FFT routine
    param.h     : Header file for parameters
    readme.txt  : Readme file
    vzfft1d.f   : 1-D complex FFT routine (for vector machines)
    vzfft2d.f   : 2-D complex FFT routine (for vector machines)
    vzfft3d.f   : 3-D complex FFT routine (for vector machines)
    zdfft2d.f   : 2-D complex-to-real FFT routine
    zdfft3d.f   : 3-D complex-to-real FFT routine
    zfft1d.f    : 1-D complex FFT routine
    zfft2d.f    : 2-D complex FFT routine
    zfft3d.f    : 3-D complex FFT routine
    tests/      : Test Directory
        Makefile       : Makefile for test programs
        Makefile.vec   : Makefile for test programs (for vector machines)
        rspeed2d.f     : Speed test program for dzfft2d
        rspeed3d.f     : Speed test program for dzfft3d
        rtest2d.f      : Test program for dzfft2d and zdfft2d
        rtest3d.f      : Test program for dzfft3d and zdfft3d
        speed1d.f      : Speed test program for zfft1d
        speed2d.f      : Speed test program for zfft2d
        speed3d.f      : Speed test program for zfft3d
        test1d.f       : Test program for zfft1d
        test2d.f       : Test program for zfft2d
        test3d.f       : Test program for zfft3d
    mpi/        : MPI version Directory
        pdzfft2d.f : Parallel 2-D real-to-complex FFT routine
        pdzfft3d.f : Parallel 3-D real-to-complex FFT routine
        pvzfft1d.f : Parallel 1-D complex FFT routine (for vector machines)
        pvzfft2d.f : Parallel 2-D complex FFT routine (for vector machines)
        pvzfft3d.f : Parallel 3-D complex FFT routine (for vector machines)
        pzdfft2d.f : Parallel 2-D complex-to-real FFT routine
        pzdfft3d.f : Parallel 3-D complex-to-real FFT routine
        pzfft1d.f  : Parallel 1-D complex FFT routine
        pzfft2d.f  : Parallel 2-D complex FFT routine
        pzfft3d.f  : Parallel 3-D complex FFT routine
        pzfft3dv.f : Parallel 3-D complex FFT routine (with 2-D decomposition)
        tests/     : Test Directory
            Makefile       : Makefile for test programs
            Makefile.vec   : Makefile for test programs (for vector machines)
            prspeed2d.f    : Speed test program for pdzfft2d
            prspeed3d.f    : Speed test program for pdzfft3d
            prtest2d.f     : Test program for pdzfft2d and pzdfft2d
            prtest3d.f     : Test program for pdzfft3d and pzdfft3d
            pspeed1d.f     : Speed test program for pzfft1d
            pspeed2d.f     : Speed test program for pzfft2d
            pspeed3d.f     : Speed test program for pzfft3d
            pspeed3dv.f    : Speed test program for pzfft3dv
            ptest1d.f      : Test program for pzfft1d
            ptest2d.f      : Test program for pzfft2d
            ptest3d.f      : Test program for pzfft3d
            ptest3dv.f     : Test program for pzfft3dv

Reference:
    1. Daisuke Takahashi: A Blocking Algorithm for FFT on Cache-Based
       Processors, Proc. 9th International Conference on High
       Performance Computing and Networking Europe (HPCN Europe 2001),
       Lecture Notes in Computer Science, No. 2110, pp. 551--554,
       Springer-Verlag, (2001).

    2. Daisuke Takahashi: A Blocking Algorithm for Parallel 1-D FFT on
       Shared-Memory Parallel Computers, Proc. 6th International
       Conference on Applied Parallel Computing (PARA 2002),
       Lecture Notes in Computer Science, No. 2367, pp. 380--389,
       Springer-Verlag, (2002).

    3. Daisuke Takahashi: Efficient implementation of parallel
       three-dimensional FFT on clusters of PCs, Computer Physics
       Communications, Vol. 152, pp. 144--150, (2003).

    4. Daisuke Takahashi: A parallel 1-D FFT algorithm for the Hitachi
       SR8000, Parallel Computing, Vol. 29, pp. 679--690, (2003).

    5. Daisuke Takahashi: A Hybrid MPI/OpenMP Implementation of a
       Parallel 3-D FFT on SMP Clusters, Proc. 6th International
       Conference on Parallel Processing and Applied Mathematics
       (PPAM 2005), Lecture Notes in Computer Science, No. 3911,
       pp. 970--977, Springer-Verlag, (2006).

    6. Daisuke Takahashi: An Implementation of Parallel 3-D FFT with
       2-D Decomposition on a Massively Parallel Cluster of Multi-core
       Processors, Proc. 8th International Conference on Parallel
       Processing and Applied Mathematics (PPAM 2009), Part I,
       Workshop on Memory Issues on Multi- and Manycore Platforms,
       Lecture Notes in Computer Science, No. 6067, pp. 606--614,
       Springer-Verlag, (2010).

Copyright:
    Copyright(C), 2000-2004, 2008-2011, Daisuke Takahashi
    Faculty of Engineering, Information and Systems
    University of Tsukuba
    1-1-1 Tennodai, Tsukuba, Ibaraki 305-8573, Japan
    e-mail: daisuke@cs.tsukuba.ac.jp
    You may use, copy, modify this code for any purpose (include
    commercial use) and without fee.
    You may distribute this ORIGINAL package.
