C
C     FFTE: A FAST FOURIER TRANSFORM PACKAGE
C
C     (C) COPYRIGHT SOFTWARE, 2000-2004, 2008-2011, ALL RIGHTS RESERVED
C                BY
C         DAISUKE TAKAHASHI
C         FACULTY OF ENGINEERING, INFORMATION AND SYSTEMS
C         UNIVERSITY OF TSUKUBA
C         1-1-1 TENNODAI, TSUKUBA, IBARAKI 305-8573, JAPAN
C         E-MAIL: daisuke@cs.tsukuba.ac.jp
C
C
C     RADIX-2, 3, 4, 5 AND 8 FFT ROUTINE
C
C     FORTRAN77 SOURCE PROGRAM
C
C     WRITTEN BY DAISUKE TAKAHASHI
C
      SUBROUTINE FFT235(A,B,W,N,IP)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 A(*),B(*),W(*)
      DIMENSION IP(*)
C
      IF (IP(1) .NE. 1) THEN
        KP4=2-MOD(IP(1)+2,3)
        KP8=(IP(1)-KP4)/3
      ELSE
        KP4=0
        KP8=0
      END IF
C
      KEY=1
      J=1
      L=N
      M=1
      DO 10 K=1,KP8
        L=L/8
        IF (L .GE. 2) THEN
          IF (KEY .GE. 0) THEN
            CALL FFT8(A,B,W(J),M,L)
          ELSE
            CALL FFT8(B,A,W(J),M,L)
          END IF
          KEY=-KEY
        ELSE
          IF (KEY .GE. 0) THEN
            CALL FFT8(A,A,W(J),M,L)
          ELSE
            CALL FFT8(B,A,W(J),M,L)
          END IF
        END IF
        M=M*8
        J=J+L*7
   10 CONTINUE
      DO 20 K=1,IP(3)
        L=L/5
        IF (L .GE. 2) THEN
          IF (KEY .GE. 0) THEN
            CALL FFT5(A,B,W(J),M,L)
          ELSE
            CALL FFT5(B,A,W(J),M,L)
          END IF
          KEY=-KEY
        ELSE
          IF (KEY .GE. 0) THEN
            CALL FFT5(A,A,W(J),M,L)
          ELSE
            CALL FFT5(B,A,W(J),M,L)
          END IF
        END IF
        M=M*5
        J=J+L*4
   20 CONTINUE
      DO 30 K=1,KP4
        L=L/4
        IF (L .GE. 2) THEN
          IF (KEY .GE. 0) THEN
            CALL FFT4(A,B,W(J),M,L)
          ELSE
            CALL FFT4(B,A,W(J),M,L)
          END IF
          KEY=-KEY
        ELSE
          IF (KEY .GE. 0) THEN
            CALL FFT4(A,A,W(J),M,L)
          ELSE
            CALL FFT4(B,A,W(J),M,L)
          END IF
        END IF
        M=M*4
        J=J+L*3
   30 CONTINUE
      DO 40 K=1,IP(2)
        L=L/3
        IF (L .GE. 2) THEN
          IF (KEY .GE. 0) THEN
            CALL FFT3(A,B,W(J),M,L)
          ELSE
            CALL FFT3(B,A,W(J),M,L)
          END IF
          KEY=-KEY
        ELSE
          IF (KEY .GE. 0) THEN
            CALL FFT3(A,A,W(J),M,L)
          ELSE
            CALL FFT3(B,A,W(J),M,L)
          END IF
        END IF
        M=M*3
        J=J+L*2
   40 CONTINUE
      IF (IP(1) .EQ. 1) THEN
        IF (KEY .GE. 0) THEN
          CALL FFT2(A,A,M)
        ELSE
          CALL FFT2(B,A,M)
        END IF
      END IF
      RETURN
      END
      SUBROUTINE FFT3(A,B,W,M,L)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 A(*),B(*),W(*)
C
      IF (M .EQ. 1) THEN
        CALL FFT3A(A,B,W,L)
      ELSE
        CALL FFT3B(A,B,W,M,L)
      END IF
      RETURN
      END
      SUBROUTINE FFT4(A,B,W,M,L)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 A(*),B(*),W(*)
C
      IF (M .EQ. 1) THEN
        CALL FFT4A(A,B,W,L)
      ELSE
        CALL FFT4B(A,B,W,M,L)
      END IF
      RETURN
      END
      SUBROUTINE FFT5(A,B,W,M,L)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 A(*),B(*),W(*)
C
      IF (M .EQ. 1) THEN
        CALL FFT5A(A,B,W,L)
      ELSE
        CALL FFT5B(A,B,W,M,L)
      END IF
      RETURN
      END
      SUBROUTINE FFT8(A,B,W,M,L)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 A(*),B(*),W(*)
C
      IF (M .EQ. 1) THEN
        CALL FFT8A(A,B,W,L)
      ELSE
        CALL FFT8B(A,B,W,M,L)
      END IF
      RETURN
      END
      SUBROUTINE SETTBL(W,N)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 W(*)
      DIMENSION IP(3)
C
      CALL FACTOR(N,IP)
C
      IF (IP(1) .NE. 1) THEN
        KP4=2-MOD(IP(1)+2,3)
        KP8=(IP(1)-KP4)/3
      ELSE
        KP4=0
        KP8=0
      END IF
C
      J=1
      L=N
      DO 10 K=1,KP8
        L=L/8
        CALL SETTBL0(W(J),8,L)
        J=J+L*7
   10 CONTINUE
      DO 20 K=1,IP(3)
        L=L/5
        CALL SETTBL0(W(J),5,L)
        J=J+L*4
   20 CONTINUE
      DO 30 K=1,KP4
        L=L/4
        CALL SETTBL0(W(J),4,L)
        J=J+L*3
   30 CONTINUE
      DO 40 K=1,IP(2)
        L=L/3
        CALL SETTBL0(W(J),3,L)
        J=J+L*2
   40 CONTINUE
      RETURN
      END
      SUBROUTINE SETTBL0(W,M,L)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 W(M-1,*)
C
      PI2=8.0D0*DATAN(1.0D0)
      PX=-PI2/(DBLE(M)*DBLE(L))
      DO 20 J=1,L
!DIR$ VECTOR ALIGNED
        DO 10 I=1,M-1
          TEMP=PX*DBLE(I)*DBLE(J-1)
          W(I,J)=DCMPLX(DCOS(TEMP),DSIN(TEMP))
   10   CONTINUE
   20 CONTINUE
      RETURN
      END
      SUBROUTINE SETTBL2(W,NX,NY)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 W(NX,*)
C
      PI2=8.0D0*DATAN(1.0D0)
      PX=-PI2/(DBLE(NX)*DBLE(NY))
!$OMP PARALLEL DO PRIVATE(TEMP)
      DO 20 J=1,NY
!DIR$ VECTOR ALIGNED
        DO 10 I=1,NX
          TEMP=PX*DBLE(I-1)*DBLE(J-1)
          W(I,J)=DCMPLX(DCOS(TEMP),DSIN(TEMP))
   10   CONTINUE
   20 CONTINUE
      RETURN
      END
      SUBROUTINE FACTOR(N,IP)
      DIMENSION IP(*)
C
      IP(1)=0
      IP(2)=0
      IP(3)=0
      N2=N
      IF (MOD(N,2) .NE. 0 .AND. MOD(N,3) .NE. 0 .AND.
     1    MOD(N,5) .NE. 0) RETURN
   10 IF (N2 .LE. 1) RETURN
      IF (MOD(N2,2) .EQ. 0) THEN
        IP(1)=IP(1)+1
        N2=N2/2
        GO TO 10
      ELSE IF (MOD(N2,3) .EQ. 0) THEN
        IP(2)=IP(2)+1
        N2=N2/3
        GO TO 10
      ELSE IF (MOD(N2,5) .EQ. 0) THEN
        IP(3)=IP(3)+1
        N2=N2/5
        GO TO 10
      END IF
      RETURN
      END
      SUBROUTINE FACTOR8(N,IP)
      DIMENSION IP(*)
      INTEGER*8 N,N2
C
      IP(1)=0
      IP(2)=0
      IP(3)=0
      N2=N
      IF (MOD(N,2) .NE. 0 .AND. MOD(N,3) .NE. 0 .AND.
     1    MOD(N,5) .NE. 0) RETURN
   10 IF (N2 .LE. 1) RETURN
      IF (MOD(N2,2) .EQ. 0) THEN
        IP(1)=IP(1)+1
        N2=N2/2
        GO TO 10
      ELSE IF (MOD(N2,3) .EQ. 0) THEN
        IP(2)=IP(2)+1
        N2=N2/3
        GO TO 10
      ELSE IF (MOD(N2,5) .EQ. 0) THEN
        IP(3)=IP(3)+1
        N2=N2/5
        GO TO 10
      END IF
      RETURN
      END
      SUBROUTINE GETNXNY(N,NX,NY)
      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION IP(3),LNX(3),LNY(3)
C
      ISQRTN=IDINT(DSQRT(DBLE(N)))
      CALL FACTOR(N,IP)
      DO 10 I=1,3
        LNX(I)=0
   10 CONTINUE
      IRES=ISQRTN
      DO 40 K=0,(IP(3)+1)/2
        DO 30 J=0,(IP(2)+1)/2
          DO 20 I=0,(IP(1)+1)/2
            NX=(2**I)*(3**J)*(5**K)
            IF (NX .LE. ISQRTN) THEN
              IRES2=ISQRTN-NX
              IF (IRES2 .LT. IRES) THEN
                LNX(1)=I
                LNX(2)=J
                LNX(3)=K
                IRES=IRES2
              END IF
            END IF
   20     CONTINUE
   30   CONTINUE
   40 CONTINUE
      DO 50 I=1,3
        LNY(I)=IP(I)-LNX(I)
   50 CONTINUE
      NX=(2**LNX(1))*(3**LNX(2))*(5**LNX(3))
      NY=(2**LNY(1))*(3**LNY(2))*(5**LNY(3))
      RETURN
      END
