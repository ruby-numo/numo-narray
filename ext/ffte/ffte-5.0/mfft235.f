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
C     RADIX-2, 3, 4, 5 AND 8 MULTIPLE FFT ROUTINE
C
C     FORTRAN77 SOURCE PROGRAM
C
C     WRITTEN BY DAISUKE TAKAHASHI
C
      SUBROUTINE MFFT235A(A,B,W,NS,N,IP)
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
            CALL FFT8B(A,B,W(J),NS*M,L)
          ELSE
            CALL FFT8B(B,A,W(J),NS*M,L)
          END IF
          KEY=-KEY
        ELSE
          IF (KEY .GE. 0) THEN
            CALL FFT8B(A,A,W(J),NS*M,L)
          ELSE
            CALL FFT8B(B,A,W(J),NS*M,L)
          END IF
        END IF
        M=M*8
        J=J+L*7
   10 CONTINUE
      DO 20 K=1,IP(3)
        L=L/5
        IF (L .GE. 2) THEN
          IF (KEY .GE. 0) THEN
            CALL FFT5B(A,B,W(J),NS*M,L)
          ELSE
            CALL FFT5B(B,A,W(J),NS*M,L)
          END IF
          KEY=-KEY
        ELSE
          IF (KEY .GE. 0) THEN
            CALL FFT5B(A,A,W(J),NS*M,L)
          ELSE
            CALL FFT5B(B,A,W(J),NS*M,L)
          END IF
        END IF
        M=M*5
        J=J+L*4
   20 CONTINUE
      DO 30 K=1,KP4
        L=L/4
        IF (L .GE. 2) THEN
          IF (KEY .GE. 0) THEN
            CALL FFT4B(A,B,W(J),NS*M,L)
          ELSE
            CALL FFT4B(B,A,W(J),NS*M,L)
          END IF
          KEY=-KEY
        ELSE
          IF (KEY .GE. 0) THEN
            CALL FFT4B(A,A,W(J),NS*M,L)
          ELSE
            CALL FFT4B(B,A,W(J),NS*M,L)
          END IF
        END IF
        M=M*4
        J=J+L*3
   30 CONTINUE
      DO 40 K=1,IP(2)
        L=L/3
        IF (L .GE. 2) THEN
          IF (KEY .GE. 0) THEN
            CALL FFT3B(A,B,W(J),NS*M,L)
          ELSE
            CALL FFT3B(B,A,W(J),NS*M,L)
          END IF
          KEY=-KEY
        ELSE
          IF (KEY .GE. 0) THEN
            CALL FFT3B(A,A,W(J),NS*M,L)
          ELSE
            CALL FFT3B(B,A,W(J),NS*M,L)
          END IF
        END IF
        M=M*3
        J=J+L*2
   40 CONTINUE
      IF (IP(1) .EQ. 1) THEN
        IF (KEY .GE. 0) THEN
          CALL FFT2(A,A,NS*M)
        ELSE
          CALL FFT2(B,A,NS*M)
        END IF
      END IF
      RETURN
      END
      SUBROUTINE MFFT235B(A,B,W,NS,N,IP)
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
            CALL FFT8(A,B,W(J),NS*M,L)
          ELSE
            CALL FFT8(B,A,W(J),NS*M,L)
          END IF
          KEY=-KEY
        ELSE
          IF (KEY .GE. 0) THEN
            CALL FFT8(A,B,W(J),NS*M,L)
          ELSE
            CALL FFT8(B,B,W(J),NS*M,L)
          END IF
        END IF
        M=M*8
        J=J+L*7
   10 CONTINUE
      DO 20 K=1,IP(3)
        L=L/5
        IF (L .GE. 2) THEN
          IF (KEY .GE. 0) THEN
            CALL FFT5(A,B,W(J),NS*M,L)
          ELSE
            CALL FFT5(B,A,W(J),NS*M,L)
          END IF
          KEY=-KEY
        ELSE
          IF (KEY .GE. 0) THEN
            CALL FFT5(A,B,W(J),NS*M,L)
          ELSE
            CALL FFT5(B,B,W(J),NS*M,L)
          END IF
        END IF
        M=M*5
        J=J+L*4
   20 CONTINUE
      DO 30 K=1,KP4
        L=L/4
        IF (L .GE. 2) THEN
          IF (KEY .GE. 0) THEN
            CALL FFT4(A,B,W(J),NS*M,L)
          ELSE
            CALL FFT4(B,A,W(J),NS*M,L)
          END IF
          KEY=-KEY
        ELSE
          IF (KEY .GE. 0) THEN
            CALL FFT4(A,B,W(J),NS*M,L)
          ELSE
            CALL FFT4(B,B,W(J),NS*M,L)
          END IF
        END IF
        M=M*4
        J=J+L*3
   30 CONTINUE
      DO 40 K=1,IP(2)
        L=L/3
        IF (L .GE. 2) THEN
          IF (KEY .GE. 0) THEN
            CALL FFT3(A,B,W(J),NS*M,L)
          ELSE
            CALL FFT3(B,A,W(J),NS*M,L)
          END IF
          KEY=-KEY
        ELSE
          IF (KEY .GE. 0) THEN
            CALL FFT3(A,B,W(J),NS*M,L)
          ELSE
            CALL FFT3(B,B,W(J),NS*M,L)
          END IF
        END IF
        M=M*3
        J=J+L*2
   40 CONTINUE
      IF (IP(1) .EQ. 1) THEN
        IF (KEY .GE. 0) THEN
          CALL FFT2(A,B,NS*M)
        ELSE
          CALL FFT2(B,B,NS*M)
        END IF
      END IF
      RETURN
      END
      SUBROUTINE ZTRANS(A,B,NX,NY)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 A(*),B(*)
      DIMENSION LNX(3),LNY(3)
C
      CALL FACTOR(NX,LNX)
      CALL FACTOR(NY,LNY)
C
      IF (NX .EQ. 1 .OR. NY .EQ. 1) THEN
!DIR$ VECTOR ALIGNED
        DO 10 I=1,NX*NY
          B(I)=A(I)
   10   CONTINUE
        RETURN
      END IF
C
      IF (LNX(1)+LNY(1) .LE. 1) THEN
        CALL ZTRANSA(A,B,NX,NY)
      ELSE
        CALL ZTRANSB(A,B,NX,NY)
      END IF
      RETURN
      END
      SUBROUTINE ZTRANSA(A,B,NX,NY)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 A(NX,*),B(NY,*)
C
      DO 20 I=1,NX
!DIR$ VECTOR ALIGNED
        DO 10 J=1,NY
          B(J,I)=A(I,J)
   10   CONTINUE
   20 CONTINUE
      RETURN
      END
      SUBROUTINE ZTRANSB(A,B,NX,NY)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 A(NX,*),B(NY,*)
C
      IF (NY .GE. NX) THEN
        DO 20 I=0,NX-1
!DIR$ VECTOR ALIGNED
          DO 10 J=1,NX-I
            B(J,I+J)=A(I+J,J)
   10     CONTINUE
   20   CONTINUE
        DO 40 I=1,NY-NX
!DIR$ VECTOR ALIGNED
          DO 30 J=1,NX
            B(I+J,J)=A(J,I+J)
   30     CONTINUE
   40   CONTINUE
        DO 60 I=NY-NX+1,NY-1
!DIR$ VECTOR ALIGNED
          DO 50 J=1,NY-I
            B(I+J,J)=A(J,I+J)
   50     CONTINUE
   60   CONTINUE
      ELSE
        DO 80 I=0,NY-1
!DIR$ VECTOR ALIGNED
          DO 70 J=1,NY-I
            B(I+J,J)=A(J,I+J)
   70     CONTINUE
   80   CONTINUE
        DO 100 I=1,NX-NY
!DIR$ VECTOR ALIGNED
          DO 90 J=1,NY
            B(J,I+J)=A(I+J,J)
   90     CONTINUE
  100   CONTINUE
        DO 120 I=NX-NY+1,NX-1
!DIR$ VECTOR ALIGNED
          DO 110 J=1,NX-I
            B(J,I+J)=A(I+J,J)
  110     CONTINUE
  120   CONTINUE
      END IF
      RETURN
      END
      SUBROUTINE MZTRANSA(A,B,NS,NX,NY)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 A(NS,NX,*),B(NS,NY,*)
C
      IF (NS .EQ. 1) THEN
        CALL ZTRANS(A(1,1,1),B(1,1,1),NX,NY)
      ELSE
        DO 30 I=1,NX
          DO 20 J=1,NY
!DIR$ VECTOR ALIGNED
            DO 10 K=1,NS
              B(K,J,I)=A(K,I,J)
   10       CONTINUE
   20     CONTINUE
   30   CONTINUE
      END IF
      RETURN
      END
      SUBROUTINE MZTRANSB(A,B,NX,NY,NS)
      IMPLICIT REAL*8 (A-H,O-Z)
      COMPLEX*16 A(NX,NY,*),B(NY,NX,*)
C
      DO 10 K=1,NS
        CALL ZTRANS(A(1,1,K),B(1,1,K),NX,NY)
   10 CONTINUE
      RETURN
      END
