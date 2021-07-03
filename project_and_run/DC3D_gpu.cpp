/* C:\Users\vaio6\Desktop\f2c\bin\bin\DC3Dfortran.f -- translated by f2c (version 20090411).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "DC3D_gpu.h"
#include <math.h>

#define abs(x)	fabs((x))

#ifdef _OPENMP
#include <omp.h>



#define static_def	/* */


/* Common Block Declarations */

typedef struct {
	union {
		struct {
			doublereal dummy[8], r__;
		} _1;
		struct {
			doublereal p, q, s, t, xy, x2, y2, d2, r__, r2, r3, r5, qr, qrx, a3,
				a5, b3, c3, uy, vy, wy, uz, vz, wz;
		} _2;
		struct {
			doublereal p, q, s, t, xy, x2, y2, d2, r__, r2, r3, r5, qr, qrx, a3,
				a5, b3, c3;
		} _3;
	};
}c1__;

c1__ *c1_;


typedef struct {
	union {
		struct {
			doublereal alp1, alp2, alp3, alp4, alp5, sd, cd, sdsd, cdcd, sdcd,
				s2d, c2d;
		} _1;
		struct {
			doublereal dummy[5], sd, cd;
		} _2;
	};
}c0__;

c0__ *c0_;

typedef
struct {
	doublereal xi2, et2, q2, r__, r2, r3, r5, y, d__, tt, alx, ale, x11, y11,
		x32, y32, ey, ez, fy, fz, gy, gz, hy, hz;
} c2__;

c2__ *c2_;

void new_env_variables(int n)restrict(amp)
{
	c0_ = new c0__[n + 1];
	c1_ = new c1__[n + 1];
	c2_ = new c2__[n + 1];
}
void delete_env_variables()restrict(amp)
{
	delete[] c0_;
	delete[] c1_;
	delete[] c2_;
}

#define c1_1 (c1_[0]._1)
#define c1_2 (c1_[0]._2)
#define c1_3 (c1_[0]._3)
#define c0_1 (c0_[0]._1)
#define c0_2 (c0_[0]._2)
#define c2_1 (c2_[0])
#else
/* Common Block Declarations */

union {
	struct {
		doublereal dummy[8], r__;
	} _1;
	struct {
		doublereal p, q, s, t, xy, x2, y2, d2, r__, r2, r3, r5, qr, qrx, a3,
			a5, b3, c3, uy, vy, wy, uz, vz, wz;
	} _2;
	struct {
		doublereal p, q, s, t, xy, x2, y2, d2, r__, r2, r3, r5, qr, qrx, a3,
			a5, b3, c3;
	} _3;
} c1_;

#define c1_1 (c1_._1)
#define c1_2 (c1_._2)
#define c1_3 (c1_._3)

union {
	struct {
		doublereal alp1, alp2, alp3, alp4, alp5, sd, cd, sdsd, cdcd, sdcd,
			s2d, c2d;
	} _1;
	struct {
		doublereal dummy[5], sd, cd;
	} _2;
} c0_;

#define c0_1 (c0_._1)
#define c0_2 (c0_._2)

struct {
	doublereal xi2, et2, q2, r__, r2, r3, r5, y, d__, tt, alx, ale, x11, y11,
		x32, y32, ey, ez, fy, fz, gy, gz, hy, hz;
} c2_;

#define c2_1 c2_

void new_env_variables()
{
}
void delete_env_variables()
{
}
#define static_def	static

#endif

int uc0_gpu(doublereal *x, doublereal *y, doublereal *d__,
	doublereal *z__, doublereal *pot1, doublereal *pot2, doublereal *pot3,
	doublereal *pot4, doublereal *u) restrict(amp);

int ub0_gpu(doublereal *x, doublereal *y, doublereal *d__,
	doublereal *z__, doublereal *pot1, doublereal *pot2, doublereal *pot3,
	doublereal *pot4, doublereal *u) restrict(amp);

int ua0_gpu(doublereal *x, doublereal *y, doublereal *d__,
	doublereal *pot1, doublereal *pot2, doublereal *pot3, doublereal *
	pot4, doublereal *u) restrict(amp);

int dccon0_gpu(doublereal *alpha, doublereal *dip) restrict(amp);
int dc3d_gpu(real *alpha, real *x, real *y, real *z__, real *
	depth, real *dip, real *al1, real *al2, real *aw1, real *aw2, real *
	disl1, real *disl2, real *disl3, real *ux, real *uy, real *uz, real *
	uxx, real *uyx, real *uzx, real *uxy, real *uyy, real *uzy, real *uxz,
	real *uyz, real *uzz, integer *iret)  restrict(amp);

int ua_gpu(doublereal *xi, doublereal *et, doublereal *q,
	doublereal *disl1, doublereal *disl2, doublereal *disl3, doublereal *
	u) restrict(amp);
int ub_gpu(doublereal *xi, doublereal *et, doublereal *q,
	doublereal *disl1, doublereal *disl2, doublereal *disl3, doublereal *
	u) restrict(amp);
int uc_gpu(doublereal *xi, doublereal *et, doublereal *q,
	doublereal *z__, doublereal *disl1, doublereal *disl2, doublereal *
	disl3, doublereal *u)  restrict(amp);
int dccon1_gpu(doublereal *x, doublereal *y, doublereal *d__) restrict(amp);

int dccon2_gpu(doublereal *xi, doublereal *et, doublereal *q,
	doublereal *sd, doublereal *cd, integer *kxi, integer *ket) restrict(amp);

/* Subroutine */ int dc3d0_gpu(real *alpha, real *x, real *y, real *z__, real *
	depth, real *dip, real *pot1, real *pot2, real *pot3, real *pot4,
	real *ux, real *uy, real *uz, real *uxx, real *uyx, real *uzx, real *
	uxy, real *uyy, real *uzy, real *uxz, real *uyz, real *uzz, integer *
	iret) restrict(amp)
{
	/* Initialized data */

	static_def doublereal f0 = 0.;

	static_def integer i__;
	static_def doublereal u[12], dd, du, xx, yy, zz;
	//extern /* Subroutine */ int ua0_gpu(doublereal *, doublereal *, doublereal *,
	//	doublereal *, doublereal *, doublereal *, doublereal *,
	//	doublereal *), ub0_gpu(doublereal *, doublereal *, doublereal *,
	//		doublereal *, doublereal *, doublereal *, doublereal *,
	//		doublereal *, doublereal *), uc0_gpu(doublereal *, doublereal *,
	//			doublereal *, doublereal *, doublereal *, doublereal *,
	//			doublereal *, doublereal *, doublereal *);
	static_def doublereal pp1, pp2, pp3, pp4, dua[12], dub[12], duc[12], ddip;
	//extern /* Subroutine */ int dccon0__gpu(doublereal *, doublereal *), dccon1__gpu(
	//	doublereal *, doublereal *, doublereal *);
	static_def doublereal aalpha;


	/* ******************************************************************** */
	/* *****                                                          ***** */
	/* *****    DISPLACEMENT AND STRAIN AT DEPTH                      ***** */
	/* *****    DUE TO BURIED POINT SOURCE IN A SEMIINFINITE MEDIUM   ***** */
	/* *****                         CODED BY  Y.OKADA ... SEP.1991   ***** */
	/* *****                         REVISED     NOV.1991, MAY.2002   ***** */
	/* *****                                                          ***** */
	/* ******************************************************************** */

	/* ***** INPUT */
	/* *****   ALPHA : MEDIUM CONSTANT  (LAMBDA+MYU)/(LAMBDA+2*MYU) */
	/* *****   X,Y,Z : COORDINATE OF OBSERVING POINT */
	/* *****   DEPTH : SOURCE DEPTH */
	/* *****   DIP   : DIP-ANGLE (DEGREE) */
	/* *****   POT1-POT4 : STRIKE-, DIP-, TENSILE- AND INFLATE-POTENCY */
	/* *****       POTENCY=(  MOMENT OF DOUBLE-COUPLE  )/MYU     FOR POT1,2 */
	/* *****       POTENCY=(INTENSITY OF ISOTROPIC PART)/LAMBDA  FOR POT3 */
	/* *****       POTENCY=(INTENSITY OF LINEAR DIPOLE )/MYU     FOR POT4 */

	/* ***** OUTPUT */
	/* *****   UX, UY, UZ  : DISPLACEMENT ( UNIT=(UNIT OF POTENCY) / */
	/* *****               :                     (UNIT OF X,Y,Z,DEPTH)**2  ) */
	/* *****   UXX,UYX,UZX : X-DERIVATIVE ( UNIT= UNIT OF POTENCY) / */
	/* *****   UXY,UYY,UZY : Y-DERIVATIVE        (UNIT OF X,Y,Z,DEPTH)**3  ) */
	/* *****   UXZ,UYZ,UZZ : Z-DERIVATIVE */
	/* *****   IRET        : RETURN CODE */
	/* *****               :   =0....NORMAL */
	/* *****               :   =1....SINGULAR */
	/* *****               :   =2....POSITIVE Z WAS GIVEN */

	/* ----- */
	*iret = 0;
	if (*z__ > 0.f) {
		*iret = 2;
		*ux = f0;
		*uy = f0;
		*uz = f0;
		*uxx = f0;
		*uyx = f0;
		*uzx = f0;
		*uxy = f0;
		*uyy = f0;
		*uzy = f0;
		*uxz = f0;
		*uyz = f0;
		*uzz = f0;
		return 0;
	}
	/* ----- */
	for (i__ = 1; i__ <= 12; ++i__) {
		u[i__ - 1] = f0;
		dua[i__ - 1] = f0;
		dub[i__ - 1] = f0;
		duc[i__ - 1] = f0;
		/* L111: */
	}
	aalpha = *alpha;
	ddip = *dip;
	dccon0_gpu(&aalpha, &ddip);
	/* ====================================== */
	/* =====  REAL-SOURCE CONTRIBUTION  ===== */
	/* ====================================== */
	xx = *x;
	yy = *y;
	zz = *z__;
	dd = *depth + *z__;
	dccon1_gpu(&xx, &yy, &dd);
	if (c1_1.r__ == f0) {
		*iret = 1;
		*ux = f0;
		*uy = f0;
		*uz = f0;
		*uxx = f0;
		*uyx = f0;
		*uzx = f0;
		*uxy = f0;
		*uyy = f0;
		*uzy = f0;
		*uxz = f0;
		*uyz = f0;
		*uzz = f0;
		return 0;
	}
	/* ----- */
	pp1 = *pot1;
	pp2 = *pot2;
	pp3 = *pot3;
	pp4 = *pot4;
	ua0_gpu(&xx, &yy, &dd, &pp1, &pp2, &pp3, &pp4, dua);
	/* ----- */
	for (i__ = 1; i__ <= 12; ++i__) {
		if (i__ < 10) {
			u[i__ - 1] -= dua[i__ - 1];
		}
		if (i__ >= 10) {
			u[i__ - 1] += dua[i__ - 1];
		}
		/* L222: */
	}
	/* ======================================= */
	/* =====  IMAGE-SOURCE CONTRIBUTION  ===== */
	/* ======================================= */
	dd = *depth - *z__;
	dccon1_gpu(&xx, &yy, &dd);
	ua0_gpu(&xx, &yy, &dd, &pp1, &pp2, &pp3, &pp4, dua);
	ub0_gpu(&xx, &yy, &dd, &zz, &pp1, &pp2, &pp3, &pp4, dub);
	uc0_gpu(&xx, &yy, &dd, &zz, &pp1, &pp2, &pp3, &pp4, duc);
	/* ----- */
	for (i__ = 1; i__ <= 12; ++i__) {
		du = dua[i__ - 1] + dub[i__ - 1] + zz * duc[i__ - 1];
		if (i__ >= 10) {
			du += duc[i__ - 10];
		}
		u[i__ - 1] += du;
		/* L333: */
	}
	/* ===== */
	*ux = u[0];
	*uy = u[1];
	*uz = u[2];
	*uxx = u[3];
	*uyx = u[4];
	*uzx = u[5];
	*uxy = u[6];
	*uyy = u[7];
	*uzy = u[8];
	*uxz = u[9];
	*uyz = u[10];
	*uzz = u[11];
	return 0;
	/* ======================================= */
	/* =====  IN CASE OF SINGULAR (R=0)  ===== */
	/* ======================================= */
} /* dc3d0_ */

/* Subroutine */ int ua0_gpu(doublereal *x, doublereal *y, doublereal *d__,
	doublereal *pot1, doublereal *pot2, doublereal *pot3, doublereal *
	pot4, doublereal *u) restrict(amp)
{
	/* Initialized data */

	static_def doublereal f0 = 0.;
	static_def doublereal f1 = 1.;
	static_def doublereal f3 = 3.;
	static_def doublereal pi2 = 6.283185307179586f;

	static_def integer i__;
	static_def doublereal du[12];


	/* ******************************************************************** */
	/* *****    DISPLACEMENT AND STRAIN AT DEPTH (PART-A)             ***** */
	/* *****    DUE TO BURIED POINT SOURCE IN A SEMIINFINITE MEDIUM   ***** */
	/* ******************************************************************** */

	/* ***** INPUT */
	/* *****   X,Y,D : STATION COORDINATES IN FAULT SYSTEM */
	/* *****   POT1-POT4 : STRIKE-, DIP-, TENSILE- AND INFLATE-POTENCY */
	/* ***** OUTPUT */
	/* *****   U(12) : DISPLACEMENT AND THEIR DERIVATIVES */

		/* Parameter adjustments */
	--u;

	/* Function Body */
/* ----- */
	for (i__ = 1; i__ <= 12; ++i__) {
		/* L111: */
		u[i__] = f0;
	}
	/* ====================================== */
	/* =====  STRIKE-SLIP CONTRIBUTION  ===== */
	/* ====================================== */
	if (*pot1 != f0) {
		du[0] = c0_1.alp1 * c1_2.q / c1_2.r3 + c0_1.alp2 * c1_2.x2 * c1_2.qr;
		du[1] = c0_1.alp1 * *x / c1_2.r3 * c0_1.sd + c0_1.alp2 * c1_2.xy *
			c1_2.qr;
		du[2] = -c0_1.alp1 * *x / c1_2.r3 * c0_1.cd + c0_1.alp2 * *x * *d__ *
			c1_2.qr;
		du[3] = *x * c1_2.qr * (-c0_1.alp1 + c0_1.alp2 * (f1 + c1_2.a5));
		du[4] = c0_1.alp1 * c1_2.a3 / c1_2.r3 * c0_1.sd + c0_1.alp2 * *y *
			c1_2.qr * c1_2.a5;
		du[5] = -c0_1.alp1 * c1_2.a3 / c1_2.r3 * c0_1.cd + c0_1.alp2 * *d__ *
			c1_2.qr * c1_2.a5;
		du[6] = c0_1.alp1 * (c0_1.sd / c1_2.r3 - *y * c1_2.qr) + c0_1.alp2 *
			f3 * c1_2.x2 / c1_2.r5 * c1_2.uy;
		du[7] = f3 * *x / c1_2.r5 * (-c0_1.alp1 * *y * c0_1.sd + c0_1.alp2 * (
			*y * c1_2.uy + c1_2.q));
		du[8] = f3 * *x / c1_2.r5 * (c0_1.alp1 * *y * c0_1.cd + c0_1.alp2 * *
			d__ * c1_2.uy);
		du[9] = c0_1.alp1 * (c0_1.cd / c1_2.r3 + *d__ * c1_2.qr) + c0_1.alp2 *
			f3 * c1_2.x2 / c1_2.r5 * c1_2.uz;
		du[10] = f3 * *x / c1_2.r5 * (c0_1.alp1 * *d__ * c0_1.sd + c0_1.alp2 *
			*y * c1_2.uz);
		du[11] = f3 * *x / c1_2.r5 * (-c0_1.alp1 * *d__ * c0_1.cd + c0_1.alp2
			* (*d__ * c1_2.uz - c1_2.q));
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L222: */
			u[i__] += *pot1 / pi2 * du[i__ - 1];
		}
	}
	/* =================================== */
	/* =====  DIP-SLIP CONTRIBUTION  ===== */
	/* =================================== */
	if (*pot2 != f0) {
		du[0] = c0_1.alp2 * *x * c1_2.p * c1_2.qr;
		du[1] = c0_1.alp1 * c1_2.s / c1_2.r3 + c0_1.alp2 * *y * c1_2.p *
			c1_2.qr;
		du[2] = -c0_1.alp1 * c1_2.t / c1_2.r3 + c0_1.alp2 * *d__ * c1_2.p *
			c1_2.qr;
		du[3] = c0_1.alp2 * c1_2.p * c1_2.qr * c1_2.a5;
		du[4] = -c0_1.alp1 * f3 * *x * c1_2.s / c1_2.r5 - c0_1.alp2 * *y *
			c1_2.p * c1_2.qrx;
		du[5] = c0_1.alp1 * f3 * *x * c1_2.t / c1_2.r5 - c0_1.alp2 * *d__ *
			c1_2.p * c1_2.qrx;
		du[6] = c0_1.alp2 * f3 * *x / c1_2.r5 * c1_2.vy;
		du[7] = c0_1.alp1 * (c0_1.s2d / c1_2.r3 - f3 * *y * c1_2.s / c1_2.r5)
			+ c0_1.alp2 * (f3 * *y / c1_2.r5 * c1_2.vy + c1_2.p * c1_2.qr)
			;
		du[8] = -c0_1.alp1 * (c0_1.c2d / c1_2.r3 - f3 * *y * c1_2.t / c1_2.r5)
			+ c0_1.alp2 * f3 * *d__ / c1_2.r5 * c1_2.vy;
		du[9] = c0_1.alp2 * f3 * *x / c1_2.r5 * c1_2.vz;
		du[10] = c0_1.alp1 * (c0_1.c2d / c1_2.r3 + f3 * *d__ * c1_2.s /
			c1_2.r5) + c0_1.alp2 * f3 * *y / c1_2.r5 * c1_2.vz;
		du[11] = c0_1.alp1 * (c0_1.s2d / c1_2.r3 - f3 * *d__ * c1_2.t /
			c1_2.r5) + c0_1.alp2 * (f3 * *d__ / c1_2.r5 * c1_2.vz -
				c1_2.p * c1_2.qr);
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L333: */
			u[i__] += *pot2 / pi2 * du[i__ - 1];
		}
	}
	/* ======================================== */
	/* =====  TENSILE-FAULT CONTRIBUTION  ===== */
	/* ======================================== */
	if (*pot3 != f0) {
		du[0] = c0_1.alp1 * *x / c1_2.r3 - c0_1.alp2 * *x * c1_2.q * c1_2.qr;
		du[1] = c0_1.alp1 * c1_2.t / c1_2.r3 - c0_1.alp2 * *y * c1_2.q *
			c1_2.qr;
		du[2] = c0_1.alp1 * c1_2.s / c1_2.r3 - c0_1.alp2 * *d__ * c1_2.q *
			c1_2.qr;
		du[3] = c0_1.alp1 * c1_2.a3 / c1_2.r3 - c0_1.alp2 * c1_2.q * c1_2.qr *
			c1_2.a5;
		du[4] = -c0_1.alp1 * f3 * *x * c1_2.t / c1_2.r5 + c0_1.alp2 * *y *
			c1_2.q * c1_2.qrx;
		du[5] = -c0_1.alp1 * f3 * *x * c1_2.s / c1_2.r5 + c0_1.alp2 * *d__ *
			c1_2.q * c1_2.qrx;
		du[6] = -c0_1.alp1 * f3 * c1_2.xy / c1_2.r5 - c0_1.alp2 * *x *
			c1_2.qr * c1_2.wy;
		du[7] = c0_1.alp1 * (c0_1.c2d / c1_2.r3 - f3 * *y * c1_2.t / c1_2.r5)
			- c0_1.alp2 * (*y * c1_2.wy + c1_2.q) * c1_2.qr;
		du[8] = c0_1.alp1 * (c0_1.s2d / c1_2.r3 - f3 * *y * c1_2.s / c1_2.r5)
			- c0_1.alp2 * *d__ * c1_2.qr * c1_2.wy;
		du[9] = c0_1.alp1 * f3 * *x * *d__ / c1_2.r5 - c0_1.alp2 * *x *
			c1_2.qr * c1_2.wz;
		du[10] = -c0_1.alp1 * (c0_1.s2d / c1_2.r3 - f3 * *d__ * c1_2.t /
			c1_2.r5) - c0_1.alp2 * *y * c1_2.qr * c1_2.wz;
		du[11] = c0_1.alp1 * (c0_1.c2d / c1_2.r3 + f3 * *d__ * c1_2.s /
			c1_2.r5) - c0_1.alp2 * (*d__ * c1_2.wz - c1_2.q) * c1_2.qr;
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L444: */
			u[i__] += *pot3 / pi2 * du[i__ - 1];
		}
	}
	/* ========================================= */
	/* =====  INFLATE SOURCE CONTRIBUTION  ===== */
	/* ========================================= */
	if (*pot4 != f0) {
		du[0] = -c0_1.alp1 * *x / c1_2.r3;
		du[1] = -c0_1.alp1 * *y / c1_2.r3;
		du[2] = -c0_1.alp1 * *d__ / c1_2.r3;
		du[3] = -c0_1.alp1 * c1_2.a3 / c1_2.r3;
		du[4] = c0_1.alp1 * f3 * c1_2.xy / c1_2.r5;
		du[5] = c0_1.alp1 * f3 * *x * *d__ / c1_2.r5;
		du[6] = du[4];
		du[7] = -c0_1.alp1 * c1_2.b3 / c1_2.r3;
		du[8] = c0_1.alp1 * f3 * *y * *d__ / c1_2.r5;
		du[9] = -du[5];
		du[10] = -du[8];
		du[11] = c0_1.alp1 * c1_2.c3 / c1_2.r3;
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L555: */
			u[i__] += *pot4 / pi2 * du[i__ - 1];
		}
	}
	return 0;
} /* ua0_ */

/* Subroutine */ int ub0_gpu(doublereal *x, doublereal *y, doublereal *d__,
	doublereal *z__, doublereal *pot1, doublereal *pot2, doublereal *pot3,
	doublereal *pot4, doublereal *u)restrict(amp)
{
	/* Initialized data */

	static_def doublereal f0 = 0.;
	static_def doublereal f1 = 1.;
	static_def doublereal f2 = 2.;
	static_def doublereal f3 = 3.;
	static_def doublereal f4 = 4.;
	static_def doublereal f5 = 5.;
	static_def doublereal f8 = 8.;
	static_def doublereal f9 = 9.;
	static_def doublereal pi2 = 6.283185307179586f;

	static_def doublereal c__;
	static_def integer i__;
	static_def doublereal d12, d32, d33, d53, d54, rd, du[12], fi1, fi2, fi3, fi4,
		fi5, fj1, fj2, fj3, fj4, fk1, fk2, fk3;


	/* ******************************************************************** */
	/* *****    DISPLACEMENT AND STRAIN AT DEPTH (PART-B)             ***** */
	/* *****    DUE TO BURIED POINT SOURCE IN A SEMIINFINITE MEDIUM   ***** */
	/* ******************************************************************** */

	/* ***** INPUT */
	/* *****   X,Y,D,Z : STATION COORDINATES IN FAULT SYSTEM */
	/* *****   POT1-POT4 : STRIKE-, DIP-, TENSILE- AND INFLATE-POTENCY */
	/* ***** OUTPUT */
	/* *****   U(12) : DISPLACEMENT AND THEIR DERIVATIVES */

		/* Parameter adjustments */
	--u;

	/* Function Body */
/* ----- */
	c__ = *d__ + *z__;
	rd = c1_2.r__ + *d__;
	d12 = f1 / (c1_2.r__ * rd * rd);
	d32 = d12 * (f2 * c1_2.r__ + *d__) / c1_2.r2;
	d33 = d12 * (f3 * c1_2.r__ + *d__) / (c1_2.r2 * rd);
	d53 = d12 * (f8 * c1_2.r2 + f9 * c1_2.r__ * *d__ + f3 * c1_2.d2) / (
		c1_2.r2 * c1_2.r2 * rd);
	d54 = d12 * (f5 * c1_2.r2 + f4 * c1_2.r__ * *d__ + c1_2.d2) / c1_2.r3 *
		d12;
	/* ----- */
	fi1 = *y * (d12 - c1_2.x2 * d33);
	fi2 = *x * (d12 - c1_2.y2 * d33);
	fi3 = *x / c1_2.r3 - fi2;
	fi4 = -c1_2.xy * d32;
	fi5 = f1 / (c1_2.r__ * rd) - c1_2.x2 * d32;
	fj1 = -f3 * c1_2.xy * (d33 - c1_2.x2 * d54);
	fj2 = f1 / c1_2.r3 - f3 * d12 + f3 * c1_2.x2 * c1_2.y2 * d54;
	fj3 = c1_2.a3 / c1_2.r3 - fj2;
	fj4 = -f3 * c1_2.xy / c1_2.r5 - fj1;
	fk1 = -(*y) * (d32 - c1_2.x2 * d53);
	fk2 = -(*x) * (d32 - c1_2.y2 * d53);
	fk3 = -f3 * *x * *d__ / c1_2.r5 - fk2;
	/* ----- */
	for (i__ = 1; i__ <= 12; ++i__) {
		/* L111: */
		u[i__] = f0;
	}
	/* ====================================== */
	/* =====  STRIKE-SLIP CONTRIBUTION  ===== */
	/* ====================================== */
	if (*pot1 != f0) {
		du[0] = -c1_2.x2 * c1_2.qr - c0_1.alp3 * fi1 * c0_1.sd;
		du[1] = -c1_2.xy * c1_2.qr - c0_1.alp3 * fi2 * c0_1.sd;
		du[2] = -c__ * *x * c1_2.qr - c0_1.alp3 * fi4 * c0_1.sd;
		du[3] = -(*x) * c1_2.qr * (f1 + c1_2.a5) - c0_1.alp3 * fj1 * c0_1.sd;
		du[4] = -(*y) * c1_2.qr * c1_2.a5 - c0_1.alp3 * fj2 * c0_1.sd;
		du[5] = -c__ * c1_2.qr * c1_2.a5 - c0_1.alp3 * fk1 * c0_1.sd;
		du[6] = -f3 * c1_2.x2 / c1_2.r5 * c1_2.uy - c0_1.alp3 * fj2 * c0_1.sd;
		du[7] = -f3 * c1_2.xy / c1_2.r5 * c1_2.uy - *x * c1_2.qr - c0_1.alp3 *
			fj4 * c0_1.sd;
		du[8] = -f3 * c__ * *x / c1_2.r5 * c1_2.uy - c0_1.alp3 * fk2 *
			c0_1.sd;
		du[9] = -f3 * c1_2.x2 / c1_2.r5 * c1_2.uz + c0_1.alp3 * fk1 * c0_1.sd;
		du[10] = -f3 * c1_2.xy / c1_2.r5 * c1_2.uz + c0_1.alp3 * fk2 *
			c0_1.sd;
		du[11] = f3 * *x / c1_2.r5 * (-c__ * c1_2.uz + c0_1.alp3 * *y *
			c0_1.sd);
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L222: */
			u[i__] += *pot1 / pi2 * du[i__ - 1];
		}
	}
	/* =================================== */
	/* =====  DIP-SLIP CONTRIBUTION  ===== */
	/* =================================== */
	if (*pot2 != f0) {
		du[0] = -(*x) * c1_2.p * c1_2.qr + c0_1.alp3 * fi3 * c0_1.sdcd;
		du[1] = -(*y) * c1_2.p * c1_2.qr + c0_1.alp3 * fi1 * c0_1.sdcd;
		du[2] = -c__ * c1_2.p * c1_2.qr + c0_1.alp3 * fi5 * c0_1.sdcd;
		du[3] = -c1_2.p * c1_2.qr * c1_2.a5 + c0_1.alp3 * fj3 * c0_1.sdcd;
		du[4] = *y * c1_2.p * c1_2.qrx + c0_1.alp3 * fj1 * c0_1.sdcd;
		du[5] = c__ * c1_2.p * c1_2.qrx + c0_1.alp3 * fk3 * c0_1.sdcd;
		du[6] = -f3 * *x / c1_2.r5 * c1_2.vy + c0_1.alp3 * fj1 * c0_1.sdcd;
		du[7] = -f3 * *y / c1_2.r5 * c1_2.vy - c1_2.p * c1_2.qr + c0_1.alp3 *
			fj2 * c0_1.sdcd;
		du[8] = -f3 * c__ / c1_2.r5 * c1_2.vy + c0_1.alp3 * fk1 * c0_1.sdcd;
		du[9] = -f3 * *x / c1_2.r5 * c1_2.vz - c0_1.alp3 * fk3 * c0_1.sdcd;
		du[10] = -f3 * *y / c1_2.r5 * c1_2.vz - c0_1.alp3 * fk1 * c0_1.sdcd;
		du[11] = -f3 * c__ / c1_2.r5 * c1_2.vz + c0_1.alp3 * c1_2.a3 /
			c1_2.r3 * c0_1.sdcd;
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L333: */
			u[i__] += *pot2 / pi2 * du[i__ - 1];
		}
	}
	/* ======================================== */
	/* =====  TENSILE-FAULT CONTRIBUTION  ===== */
	/* ======================================== */
	if (*pot3 != f0) {
		du[0] = *x * c1_2.q * c1_2.qr - c0_1.alp3 * fi3 * c0_1.sdsd;
		du[1] = *y * c1_2.q * c1_2.qr - c0_1.alp3 * fi1 * c0_1.sdsd;
		du[2] = c__ * c1_2.q * c1_2.qr - c0_1.alp3 * fi5 * c0_1.sdsd;
		du[3] = c1_2.q * c1_2.qr * c1_2.a5 - c0_1.alp3 * fj3 * c0_1.sdsd;
		du[4] = -(*y) * c1_2.q * c1_2.qrx - c0_1.alp3 * fj1 * c0_1.sdsd;
		du[5] = -c__ * c1_2.q * c1_2.qrx - c0_1.alp3 * fk3 * c0_1.sdsd;
		du[6] = *x * c1_2.qr * c1_2.wy - c0_1.alp3 * fj1 * c0_1.sdsd;
		du[7] = c1_2.qr * (*y * c1_2.wy + c1_2.q) - c0_1.alp3 * fj2 *
			c0_1.sdsd;
		du[8] = c__ * c1_2.qr * c1_2.wy - c0_1.alp3 * fk1 * c0_1.sdsd;
		du[9] = *x * c1_2.qr * c1_2.wz + c0_1.alp3 * fk3 * c0_1.sdsd;
		du[10] = *y * c1_2.qr * c1_2.wz + c0_1.alp3 * fk1 * c0_1.sdsd;
		du[11] = c__ * c1_2.qr * c1_2.wz - c0_1.alp3 * c1_2.a3 / c1_2.r3 *
			c0_1.sdsd;
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L444: */
			u[i__] += *pot3 / pi2 * du[i__ - 1];
		}
	}
	/* ========================================= */
	/* =====  INFLATE SOURCE CONTRIBUTION  ===== */
	/* ========================================= */
	if (*pot4 != f0) {
		du[0] = c0_1.alp3 * *x / c1_2.r3;
		du[1] = c0_1.alp3 * *y / c1_2.r3;
		du[2] = c0_1.alp3 * *d__ / c1_2.r3;
		du[3] = c0_1.alp3 * c1_2.a3 / c1_2.r3;
		du[4] = -c0_1.alp3 * f3 * c1_2.xy / c1_2.r5;
		du[5] = -c0_1.alp3 * f3 * *x * *d__ / c1_2.r5;
		du[6] = du[4];
		du[7] = c0_1.alp3 * c1_2.b3 / c1_2.r3;
		du[8] = -c0_1.alp3 * f3 * *y * *d__ / c1_2.r5;
		du[9] = -du[5];
		du[10] = -du[8];
		du[11] = -c0_1.alp3 * c1_2.c3 / c1_2.r3;
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L555: */
			u[i__] += *pot4 / pi2 * du[i__ - 1];
		}
	}
	return 0;
} /* ub0_ */

/* Subroutine */ int uc0_gpu(doublereal *x, doublereal *y, doublereal *d__,
	doublereal *z__, doublereal *pot1, doublereal *pot2, doublereal *pot3,
	doublereal *pot4, doublereal *u) restrict(amp)
{
	/* Initialized data */

	static_def doublereal f0 = 0.;
	static_def doublereal f1 = 1.;
	static_def doublereal f2 = 2.;
	static_def doublereal f3 = 3.;
	static_def doublereal f5 = 5.;
	static_def doublereal f7 = 7.;
	static_def doublereal f10 = 10.;
	static_def doublereal f15 = 15.;
	static_def doublereal pi2 = 6.283185307179586f;

	static_def doublereal c__;
	static_def integer i__;
	static_def doublereal a7, b5, b7, c5, q2, c7, d7, r7, du[12], dr5, qr5, qr7;


	/* ******************************************************************** */
	/* *****    DISPLACEMENT AND STRAIN AT DEPTH (PART-B)             ***** */
	/* *****    DUE TO BURIED POINT SOURCE IN A SEMIINFINITE MEDIUM   ***** */
	/* ******************************************************************** */

	/* ***** INPUT */
	/* *****   X,Y,D,Z : STATION COORDINATES IN FAULT SYSTEM */
	/* *****   POT1-POT4 : STRIKE-, DIP-, TENSILE- AND INFLATE-POTENCY */
	/* ***** OUTPUT */
	/* *****   U(12) : DISPLACEMENT AND THEIR DERIVATIVES */

		/* Parameter adjustments */
	--u;

	/* Function Body */
/* ----- */
	c__ = *d__ + *z__;
	q2 = c1_3.q * c1_3.q;
	r7 = c1_3.r5 * c1_3.r2;
	a7 = f1 - f7 * c1_3.x2 / c1_3.r2;
	b5 = f1 - f5 * c1_3.y2 / c1_3.r2;
	b7 = f1 - f7 * c1_3.y2 / c1_3.r2;
	c5 = f1 - f5 * c1_3.d2 / c1_3.r2;
	c7 = f1 - f7 * c1_3.d2 / c1_3.r2;
	d7 = f2 - f7 * q2 / c1_3.r2;
	qr5 = f5 * c1_3.q / c1_3.r2;
	qr7 = f7 * c1_3.q / c1_3.r2;
	dr5 = f5 * *d__ / c1_3.r2;
	/* ----- */
	for (i__ = 1; i__ <= 12; ++i__) {
		/* L111: */
		u[i__] = f0;
	}
	/* ====================================== */
	/* =====  STRIKE-SLIP CONTRIBUTION  ===== */
	/* ====================================== */
	if (*pot1 != f0) {
		du[0] = -c0_1.alp4 * c1_3.a3 / c1_3.r3 * c0_1.cd + c0_1.alp5 * c__ *
			c1_3.qr * c1_3.a5;
		du[1] = f3 * *x / c1_3.r5 * (c0_1.alp4 * *y * c0_1.cd + c0_1.alp5 *
			c__ * (c0_1.sd - *y * qr5));
		du[2] = f3 * *x / c1_3.r5 * (-c0_1.alp4 * *y * c0_1.sd + c0_1.alp5 *
			c__ * (c0_1.cd + *d__ * qr5));
		du[3] = c0_1.alp4 * f3 * *x / c1_3.r5 * (f2 + c1_3.a5) * c0_1.cd -
			c0_1.alp5 * c__ * c1_3.qrx * (f2 + a7);
		du[4] = f3 / c1_3.r5 * (c0_1.alp4 * *y * c1_3.a5 * c0_1.cd +
			c0_1.alp5 * c__ * (c1_3.a5 * c0_1.sd - *y * qr5 * a7));
		du[5] = f3 / c1_3.r5 * (-c0_1.alp4 * *y * c1_3.a5 * c0_1.sd +
			c0_1.alp5 * c__ * (c1_3.a5 * c0_1.cd + *d__ * qr5 * a7));
		du[6] = du[4];
		du[7] = f3 * *x / c1_3.r5 * (c0_1.alp4 * b5 * c0_1.cd - c0_1.alp5 *
			f5 * c__ / c1_3.r2 * (f2 * *y * c0_1.sd + c1_3.q * b7));
		du[8] = f3 * *x / c1_3.r5 * (-c0_1.alp4 * b5 * c0_1.sd + c0_1.alp5 *
			f5 * c__ / c1_3.r2 * (*d__ * b7 * c0_1.sd - *y * c7 * c0_1.cd)
			);
		du[9] = f3 / c1_3.r5 * (-c0_1.alp4 * *d__ * c1_3.a5 * c0_1.cd +
			c0_1.alp5 * c__ * (c1_3.a5 * c0_1.cd + *d__ * qr5 * a7));
		du[10] = f15 * *x / r7 * (c0_1.alp4 * *y * *d__ * c0_1.cd + c0_1.alp5
			* c__ * (*d__ * b7 * c0_1.sd - *y * c7 * c0_1.cd));
		du[11] = f15 * *x / r7 * (-c0_1.alp4 * *y * *d__ * c0_1.sd +
			c0_1.alp5 * c__ * (f2 * *d__ * c0_1.cd - c1_3.q * c7));
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L222: */
			u[i__] += *pot1 / pi2 * du[i__ - 1];
		}
	}
	/* =================================== */
	/* =====  DIP-SLIP CONTRIBUTION  ===== */
	/* =================================== */
	if (*pot2 != f0) {
		du[0] = c0_1.alp4 * f3 * *x * c1_3.t / c1_3.r5 - c0_1.alp5 * c__ *
			c1_3.p * c1_3.qrx;
		du[1] = -c0_1.alp4 / c1_3.r3 * (c0_1.c2d - f3 * *y * c1_3.t / c1_3.r2)
			+ c0_1.alp5 * f3 * c__ / c1_3.r5 * (c1_3.s - *y * c1_3.p *
				qr5);
		du[2] = -c0_1.alp4 * c1_3.a3 / c1_3.r3 * c0_1.sdcd + c0_1.alp5 * f3 *
			c__ / c1_3.r5 * (c1_3.t + *d__ * c1_3.p * qr5);
		du[3] = c0_1.alp4 * f3 * c1_3.t / c1_3.r5 * c1_3.a5 - c0_1.alp5 * f5 *
			c__ * c1_3.p * c1_3.qr / c1_3.r2 * a7;
		du[4] = f3 * *x / c1_3.r5 * (c0_1.alp4 * (c0_1.c2d - f5 * *y * c1_3.t
			/ c1_3.r2) - c0_1.alp5 * f5 * c__ / c1_3.r2 * (c1_3.s - *y *
				c1_3.p * qr7));
		du[5] = f3 * *x / c1_3.r5 * (c0_1.alp4 * (f2 + c1_3.a5) * c0_1.sdcd -
			c0_1.alp5 * f5 * c__ / c1_3.r2 * (c1_3.t + *d__ * c1_3.p *
				qr7));
		du[6] = du[4];
		du[7] = f3 / c1_3.r5 * (c0_1.alp4 * (f2 * *y * c0_1.c2d + c1_3.t * b5)
			+ c0_1.alp5 * c__ * (c0_1.s2d - f10 * *y * c1_3.s / c1_3.r2
				- c1_3.p * qr5 * b7));
		du[8] = f3 / c1_3.r5 * (c0_1.alp4 * *y * c1_3.a5 * c0_1.sdcd -
			c0_1.alp5 * c__ * ((f3 + c1_3.a5) * c0_1.c2d + *y * c1_3.p *
				dr5 * qr7));
		du[9] = f3 * *x / c1_3.r5 * (-c0_1.alp4 * (c0_1.s2d - c1_3.t * dr5) -
			c0_1.alp5 * f5 * c__ / c1_3.r2 * (c1_3.t + *d__ * c1_3.p *
				qr7));
		du[10] = f3 / c1_3.r5 * (-c0_1.alp4 * (*d__ * b5 * c0_1.c2d + *y * c5
			* c0_1.s2d) - c0_1.alp5 * c__ * ((f3 + c1_3.a5) * c0_1.c2d + *
				y * c1_3.p * dr5 * qr7));
		du[11] = f3 / c1_3.r5 * (-c0_1.alp4 * *d__ * c1_3.a5 * c0_1.sdcd -
			c0_1.alp5 * c__ * (c0_1.s2d - f10 * *d__ * c1_3.t / c1_3.r2 +
				c1_3.p * qr5 * c7));
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L333: */
			u[i__] += *pot2 / pi2 * du[i__ - 1];
		}
	}
	/* ======================================== */
	/* =====  TENSILE-FAULT CONTRIBUTION  ===== */
	/* ======================================== */
	if (*pot3 != f0) {
		du[0] = f3 * *x / c1_3.r5 * (-c0_1.alp4 * c1_3.s + c0_1.alp5 * (c__ *
			c1_3.q * qr5 - *z__));
		du[1] = c0_1.alp4 / c1_3.r3 * (c0_1.s2d - f3 * *y * c1_3.s / c1_3.r2)
			+ c0_1.alp5 * f3 / c1_3.r5 * (c__ * (c1_3.t - *y + *y *
				c1_3.q * qr5) - *y * *z__);
		du[2] = -c0_1.alp4 / c1_3.r3 * (f1 - c1_3.a3 * c0_1.sdsd) - c0_1.alp5
			* f3 / c1_3.r5 * (c__ * (c1_3.s - *d__ + *d__ * c1_3.q * qr5)
				- *d__ * *z__);
		du[3] = -c0_1.alp4 * f3 * c1_3.s / c1_3.r5 * c1_3.a5 + c0_1.alp5 * (
			c__ * c1_3.qr * qr5 * a7 - f3 * *z__ / c1_3.r5 * c1_3.a5);
		du[4] = f3 * *x / c1_3.r5 * (-c0_1.alp4 * (c0_1.s2d - f5 * *y *
			c1_3.s / c1_3.r2) - c0_1.alp5 * f5 / c1_3.r2 * (c__ * (c1_3.t
				- *y + *y * c1_3.q * qr7) - *y * *z__));
		du[5] = f3 * *x / c1_3.r5 * (c0_1.alp4 * (f1 - (f2 + c1_3.a5) *
			c0_1.sdsd) + c0_1.alp5 * f5 / c1_3.r2 * (c__ * (c1_3.s - *d__
				+ *d__ * c1_3.q * qr7) - *d__ * *z__));
		du[6] = du[4];
		du[7] = f3 / c1_3.r5 * (-c0_1.alp4 * (f2 * *y * c0_1.s2d + c1_3.s *
			b5) - c0_1.alp5 * (c__ * (f2 * c0_1.sdsd + f10 * *y * (c1_3.t
				- *y) / c1_3.r2 - c1_3.q * qr5 * b7) + *z__ * b5));
		du[8] = f3 / c1_3.r5 * (c0_1.alp4 * *y * (f1 - c1_3.a5 * c0_1.sdsd) +
			c0_1.alp5 * (c__ * (f3 + c1_3.a5) * c0_1.s2d - *y * dr5 * (
				c__ * d7 + *z__)));
		du[9] = f3 * *x / c1_3.r5 * (-c0_1.alp4 * (c0_1.c2d + c1_3.s * dr5) +
			c0_1.alp5 * (f5 * c__ / c1_3.r2 * (c1_3.s - *d__ + *d__ *
				c1_3.q * qr7) - f1 - *z__ * dr5));
		du[10] = f3 / c1_3.r5 * (c0_1.alp4 * (*d__ * b5 * c0_1.s2d - *y * c5 *
			c0_1.c2d) + c0_1.alp5 * (c__ * ((f3 + c1_3.a5) * c0_1.s2d - *
				y * dr5 * d7) - *y * (f1 + *z__ * dr5)));
		du[11] = f3 / c1_3.r5 * (-c0_1.alp4 * *d__ * (f1 - c1_3.a5 *
			c0_1.sdsd) - c0_1.alp5 * (c__ * (c0_1.c2d + f10 * *d__ * (
				c1_3.s - *d__) / c1_3.r2 - c1_3.q * qr5 * c7) + *z__ * (f1 +
					c5)));
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L444: */
			u[i__] += *pot3 / pi2 * du[i__ - 1];
		}
	}
	/* ========================================= */
	/* =====  INFLATE SOURCE CONTRIBUTION  ===== */
	/* ========================================= */
	if (*pot4 != f0) {
		du[0] = c0_1.alp4 * f3 * *x * *d__ / c1_3.r5;
		du[1] = c0_1.alp4 * f3 * *y * *d__ / c1_3.r5;
		du[2] = c0_1.alp4 * c1_3.c3 / c1_3.r3;
		du[3] = c0_1.alp4 * f3 * *d__ / c1_3.r5 * c1_3.a5;
		du[4] = -c0_1.alp4 * f15 * c1_3.xy * *d__ / r7;
		du[5] = -c0_1.alp4 * f3 * *x / c1_3.r5 * c5;
		du[6] = du[4];
		du[7] = c0_1.alp4 * f3 * *d__ / c1_3.r5 * b5;
		du[8] = -c0_1.alp4 * f3 * *y / c1_3.r5 * c5;
		du[9] = du[5];
		du[10] = du[8];
		du[11] = c0_1.alp4 * f3 * *d__ / c1_3.r5 * (f2 + c5);
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L555: */
			u[i__] += *pot4 / pi2 * du[i__ - 1];
		}
	}
	return 0;
} /* uc0_ */

/* Subroutine */ int dc3d_gpu(real *alpha, real *x, real *y, real *z__, real *
	depth, real *dip, real *al1, real *al2, real *aw1, real *aw2, real *
	disl1, real *disl2, real *disl3, real *ux, real *uy, real *uz, real *
	uxx, real *uyx, real *uzx, real *uxy, real *uyy, real *uzy, real *uxz,
	real *uyz, real *uzz, integer *iret)  restrict(amp)
{
	/* Initialized data */

	static_def doublereal f0 = 0.;
	static_def doublereal eps = 1e-6f;

	/* Builtin functions */
	//double sqrt(doublereal);

	/* Local variables */
	static_def doublereal d__;
	static_def integer i__, j, k;
	static_def doublereal p, q, u[12], r12, r21, r22, et[2], du[12];
	//extern /* Subroutine */ int ua_(doublereal *, doublereal *, doublereal *,
	//	doublereal *, doublereal *, doublereal *, doublereal *), ub_(
	//		doublereal *, doublereal *, doublereal *, doublereal *,
	//		doublereal *, doublereal *, doublereal *), uc_(doublereal *,
	//			doublereal *, doublereal *, doublereal *, doublereal *,
	//			doublereal *, doublereal *, doublereal *);
	static_def doublereal xi[2], zz, dd1, dd2, dd3, dua[12], dub[12], duc[12];
	static_def integer ket[2], kxi[2];
	static_def doublereal ddip;
	//extern /* Subroutine */ int dccon0_(doublereal *, doublereal *), dccon2_(
	//	doublereal *, doublereal *, doublereal *, doublereal *,
	//	doublereal *, integer *, integer *);
	static_def doublereal aalpha;


	/* ******************************************************************** */
	/* *****                                                          ***** */
	/* *****    DISPLACEMENT AND STRAIN AT DEPTH                      ***** */
	/* *****    DUE TO BURIED FINITE FAULT IN A SEMIINFINITE MEDIUM   ***** */
	/* *****              CODED BY  Y.OKADA ... SEP.1991              ***** */
	/* *****              REVISED ... NOV.1991, APR.1992, MAY.1993,   ***** */
	/* *****                          JUL.1993, MAY.2002              ***** */
	/* ******************************************************************** */

	/* ***** INPUT */
	/* *****   ALPHA : MEDIUM CONSTANT  (LAMBDA+MYU)/(LAMBDA+2*MYU) */
	/* *****   X,Y,Z : COORDINATE OF OBSERVING POINT */
	/* *****   DEPTH : DEPTH OF REFERENCE POINT */
	/* *****   DIP   : DIP-ANGLE (DEGREE) */
	/* *****   AL1,AL2   : FAULT LENGTH RANGE */
	/* *****   AW1,AW2   : FAULT WIDTH RANGE */
	/* *****   DISL1-DISL3 : STRIKE-, DIP-, TENSILE-DISLOCATIONS */

	/* ***** OUTPUT */
	/* *****   UX, UY, UZ  : DISPLACEMENT ( UNIT=(UNIT OF DISL) */
	/* *****   UXX,UYX,UZX : X-DERIVATIVE ( UNIT=(UNIT OF DISL) / */
	/* *****   UXY,UYY,UZY : Y-DERIVATIVE        (UNIT OF X,Y,Z,DEPTH,AL,AW) ) */
	/* *****   UXZ,UYZ,UZZ : Z-DERIVATIVE */
	/* *****   IRET        : RETURN CODE */
	/* *****               :   =0....NORMAL */
	/* *****               :   =1....SINGULAR */
	/* *****               :   =2....POSITIVE Z WAS GIVEN */

	/* ----- */
	*iret = 0;
	if (*z__ > 0.f) {
		*iret = 2;
		*ux = f0;
		*uy = f0;
		*uz = f0;
		*uxx = f0;
		*uyx = f0;
		*uzx = f0;
		*uxy = f0;
		*uyy = f0;
		*uzy = f0;
		*uxz = f0;
		*uyz = f0;
		*uzz = f0;
		return 0;
	}
	/* ----- */
	for (i__ = 1; i__ <= 12; ++i__) {
		u[i__ - 1] = f0;
		dua[i__ - 1] = f0;
		dub[i__ - 1] = f0;
		duc[i__ - 1] = f0;
		/* L111: */
	}
	aalpha = *alpha;
	ddip = *dip;
	dccon0_gpu(&aalpha, &ddip);
	/* ----- */
	zz = *z__;
	dd1 = *disl1;
	dd2 = *disl2;
	dd3 = *disl3;
	xi[0] = *x - *al1;
	xi[1] = *x - *al2;
	if (fast_math::fabs(xi[0]) < eps) {
		xi[0] = f0;
	}
	if (fast_math::fabs(xi[1]) < eps) {
		xi[1] = f0;
	}
	/* ====================================== */
	/* =====  REAL-SOURCE CONTRIBUTION  ===== */
	/* ====================================== */
	d__ = *depth + *z__;
	p = *y * c0_2.cd + d__ * c0_2.sd;
	q = *y * c0_2.sd - d__ * c0_2.cd;
	et[0] = p - *aw1;
	et[1] = p - *aw2;
	if (fast_math::fabs(q) < eps) {
		q = f0;
	}
	if (fast_math::fabs(et[0]) < eps) {
		et[0] = f0;
	}
	if (fast_math::fabs(et[1]) < eps) {
		et[1] = f0;
	}
	/* -------------------------------- */
	/* ----- REJECT SINGULAR CASE ----- */
	/* -------------------------------- */
	/* ----- ON FAULT EDGE */
	if (q == f0 && (xi[0] * xi[1] <= f0 && et[0] * et[1] == f0 || et[0] * et[
		1] <= f0 && xi[0] * xi[1] == f0)) {
		*iret = 1;
		*ux = f0;
		*uy = f0;
		*uz = f0;
		*uxx = f0;
		*uyx = f0;
		*uzx = f0;
		*uxy = f0;
		*uyy = f0;
		*uzy = f0;
		*uxz = f0;
		*uyz = f0;
		*uzz = f0;
		return 0;
	}
	/* ----- ON NEGATIVE EXTENSION OF FAULT EDGE */
	kxi[0] = 0;
	kxi[1] = 0;
	ket[0] = 0;
	ket[1] = 0;
	r12 = fast_math::sqrt(xi[0] * xi[0] + et[1] * et[1] + q * q);
	r21 = fast_math::sqrt(xi[1] * xi[1] + et[0] * et[0] + q * q);
	r22 = fast_math::sqrt(xi[1] * xi[1] + et[1] * et[1] + q * q);
	if (xi[0] < f0 && r21 + xi[1] < eps) {
		kxi[0] = 1;
	}
	if (xi[0] < f0 && r22 + xi[1] < eps) {
		kxi[1] = 1;
	}
	if (et[0] < f0 && r12 + et[1] < eps) {
		ket[0] = 1;
	}
	if (et[0] < f0 && r22 + et[1] < eps) {
		ket[1] = 1;
	}
	/* ===== */
	for (k = 1; k <= 2; ++k) {
		for (j = 1; j <= 2; ++j) {
			dccon2_gpu(&xi[j - 1], &et[k - 1], &q, &c0_2.sd, &c0_2.cd, &kxi[k -
				1], &ket[j - 1]);
			ua_gpu(&xi[j - 1], &et[k - 1], &q, &dd1, &dd2, &dd3, dua);
			/* ----- */
			for (i__ = 1; i__ <= 10; i__ += 3) {
				du[i__ - 1] = -dua[i__ - 1];
				du[i__] = -dua[i__] * c0_2.cd + dua[i__ + 1] * c0_2.sd;
				du[i__ + 1] = -dua[i__] * c0_2.sd - dua[i__ + 1] * c0_2.cd;
				if (i__ < 10) {
					//goto L220;
				}
				else
				{
					du[i__ - 1] = -du[i__ - 1];
					du[i__] = -du[i__];
					du[i__ + 1] = -du[i__ + 1];
				}
//			L220:
				;
			}
			for (i__ = 1; i__ <= 12; ++i__) {
				if (j + k != 3) {
					u[i__ - 1] += du[i__ - 1];
				}
				if (j + k == 3) {
					u[i__ - 1] -= du[i__ - 1];
				}
				/* L221: */
			}
			/* ----- */
			/* L222: */
		}
		/* L223: */
	}
	/* ======================================= */
	/* =====  IMAGE-SOURCE CONTRIBUTION  ===== */
	/* ======================================= */
	d__ = *depth - *z__;
	p = *y * c0_2.cd + d__ * c0_2.sd;
	q = *y * c0_2.sd - d__ * c0_2.cd;
	et[0] = p - *aw1;
	et[1] = p - *aw2;
	if (fast_math::fabs(q) < eps) {
		q = f0;
	}
	if (fast_math::fabs(et[0]) < eps) {
		et[0] = f0;
	}
	if (fast_math::fabs(et[1]) < eps) {
		et[1] = f0;
	}
	/* -------------------------------- */
	/* ----- REJECT SINGULAR CASE ----- */
	/* -------------------------------- */
	/* ----- ON FAULT EDGE */
	if (q == f0 && (xi[0] * xi[1] <= f0 && et[0] * et[1] == f0 || et[0] * et[
		1] <= f0 && xi[0] * xi[1] == f0)) {
		*iret = 1;
		*ux = f0;
		*uy = f0;
		*uz = f0;
		*uxx = f0;
		*uyx = f0;
		*uzx = f0;
		*uxy = f0;
		*uyy = f0;
		*uzy = f0;
		*uxz = f0;
		*uyz = f0;
		*uzz = f0;
		return 0;
	}
	/* ----- ON NEGATIVE EXTENSION OF FAULT EDGE */
	kxi[0] = 0;
	kxi[1] = 0;
	ket[0] = 0;
	ket[1] = 0;
	r12 = fast_math::sqrt(xi[0] * xi[0] + et[1] * et[1] + q * q);
	r21 = fast_math::sqrt(xi[1] * xi[1] + et[0] * et[0] + q * q);
	r22 = fast_math::sqrt(xi[1] * xi[1] + et[1] * et[1] + q * q);
	if (xi[0] < f0 && r21 + xi[1] < eps) {
		kxi[0] = 1;
	}
	if (xi[0] < f0 && r22 + xi[1] < eps) {
		kxi[1] = 1;
	}
	if (et[0] < f0 && r12 + et[1] < eps) {
		ket[0] = 1;
	}
	if (et[0] < f0 && r22 + et[1] < eps) {
		ket[1] = 1;
	}
	/* ===== */
	for (k = 1; k <= 2; ++k) {
		for (j = 1; j <= 2; ++j) {
			dccon2_gpu(&xi[j - 1], &et[k - 1], &q, &c0_2.sd, &c0_2.cd, &kxi[k -
				1], &ket[j - 1]);
			ua_gpu(&xi[j - 1], &et[k - 1], &q, &dd1, &dd2, &dd3, dua);
			ub_gpu(&xi[j - 1], &et[k - 1], &q, &dd1, &dd2, &dd3, dub);
			uc_gpu(&xi[j - 1], &et[k - 1], &q, &zz, &dd1, &dd2, &dd3, duc);
			/* ----- */
			for (i__ = 1; i__ <= 10; i__ += 3) {
				du[i__ - 1] = dua[i__ - 1] + dub[i__ - 1] + *z__ * duc[i__ -
					1];
				du[i__] = (dua[i__] + dub[i__] + *z__ * duc[i__]) * c0_2.cd -
					(dua[i__ + 1] + dub[i__ + 1] + *z__ * duc[i__ + 1]) *
					c0_2.sd;
				du[i__ + 1] = (dua[i__] + dub[i__] - *z__ * duc[i__]) *
					c0_2.sd + (dua[i__ + 1] + dub[i__ + 1] - *z__ * duc[
						i__ + 1]) * c0_2.cd;
				if (i__ < 10) {
					//goto L330;
				}
				else
				{
					du[9] += duc[0];
					du[10] = du[10] + duc[1] * c0_2.cd - duc[2] * c0_2.sd;
					du[11] = du[11] - duc[1] * c0_2.sd - duc[2] * c0_2.cd;
				}
//			L330:
				;
			}
			for (i__ = 1; i__ <= 12; ++i__) {
				if (j + k != 3) {
					u[i__ - 1] += du[i__ - 1];
				}
				if (j + k == 3) {
					u[i__ - 1] -= du[i__ - 1];
				}
				/* L331: */
			}
			/* ----- */
			/* L333: */
		}
		/* L334: */
	}
	/* ===== */
	*ux = u[0];
	*uy = u[1];
	*uz = u[2];
	*uxx = u[3];
	*uyx = u[4];
	*uzx = u[5];
	*uxy = u[6];
	*uyy = u[7];
	*uzy = u[8];
	*uxz = u[9];
	*uyz = u[10];
	*uzz = u[11];
	return 0;
	/* =========================================== */
	/* =====  IN CASE OF SINGULAR (ON EDGE)  ===== */
	/* =========================================== */
} /* dc3d_ */

/* Subroutine */ int ua_gpu(doublereal *xi, doublereal *et, doublereal *q,
	doublereal *disl1, doublereal *disl2, doublereal *disl3, doublereal *
	u) restrict(amp)
{
	/* Initialized data */

	static_def doublereal f0 = 0.;
	static_def doublereal f2 = 2.;
	static_def doublereal pi2 = 6.283185307179586f;

	static_def integer i__;
	static_def doublereal du[12], qx, qy, xy;


	/* ******************************************************************** */
	/* *****    DISPLACEMENT AND STRAIN AT DEPTH (PART-A)             ***** */
	/* *****    DUE TO BURIED FINITE FAULT IN A SEMIINFINITE MEDIUM   ***** */
	/* ******************************************************************** */

	/* ***** INPUT */
	/* *****   XI,ET,Q : STATION COORDINATES IN FAULT SYSTEM */
	/* *****   DISL1-DISL3 : STRIKE-, DIP-, TENSILE-DISLOCATIONS */
	/* ***** OUTPUT */
	/* *****   U(12) : DISPLACEMENT AND THEIR DERIVATIVES */

		/* Parameter adjustments */
	--u;

	/* Function Body */
/* ----- */
	for (i__ = 1; i__ <= 12; ++i__) {
		/* L111: */
		u[i__] = f0;
	}
	xy = *xi * c2_1.y11;
	qx = *q * c2_1.x11;
	qy = *q * c2_1.y11;
	/* ====================================== */
	/* =====  STRIKE-SLIP CONTRIBUTION  ===== */
	/* ====================================== */
	if (*disl1 != f0) {
		du[0] = c2_1.tt / f2 + c0_1.alp2 * *xi * qy;
		du[1] = c0_1.alp2 * *q / c2_1.r__;
		du[2] = c0_1.alp1 * c2_1.ale - c0_1.alp2 * *q * qy;
		du[3] = -c0_1.alp1 * qy - c0_1.alp2 * c2_1.xi2 * *q * c2_1.y32;
		du[4] = -c0_1.alp2 * *xi * *q / c2_1.r3;
		du[5] = c0_1.alp1 * xy + c0_1.alp2 * *xi * c2_1.q2 * c2_1.y32;
		du[6] = c0_1.alp1 * xy * c0_1.sd + c0_1.alp2 * *xi * c2_1.fy +
			c2_1.d__ / f2 * c2_1.x11;
		du[7] = c0_1.alp2 * c2_1.ey;
		du[8] = c0_1.alp1 * (c0_1.cd / c2_1.r__ + qy * c0_1.sd) - c0_1.alp2 *
			*q * c2_1.fy;
		du[9] = c0_1.alp1 * xy * c0_1.cd + c0_1.alp2 * *xi * c2_1.fz + c2_1.y
			/ f2 * c2_1.x11;
		du[10] = c0_1.alp2 * c2_1.ez;
		du[11] = -c0_1.alp1 * (c0_1.sd / c2_1.r__ - qy * c0_1.cd) - c0_1.alp2
			* *q * c2_1.fz;
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L222: */
			u[i__] += *disl1 / pi2 * du[i__ - 1];
		}
	}
	/* ====================================== */
	/* =====    DIP-SLIP CONTRIBUTION   ===== */
	/* ====================================== */
	if (*disl2 != f0) {
		du[0] = c0_1.alp2 * *q / c2_1.r__;
		du[1] = c2_1.tt / f2 + c0_1.alp2 * *et * qx;
		du[2] = c0_1.alp1 * c2_1.alx - c0_1.alp2 * *q * qx;
		du[3] = -c0_1.alp2 * *xi * *q / c2_1.r3;
		du[4] = -qy / f2 - c0_1.alp2 * *et * *q / c2_1.r3;
		du[5] = c0_1.alp1 / c2_1.r__ + c0_1.alp2 * c2_1.q2 / c2_1.r3;
		du[6] = c0_1.alp2 * c2_1.ey;
		du[7] = c0_1.alp1 * c2_1.d__ * c2_1.x11 + xy / f2 * c0_1.sd +
			c0_1.alp2 * *et * c2_1.gy;
		du[8] = c0_1.alp1 * c2_1.y * c2_1.x11 - c0_1.alp2 * *q * c2_1.gy;
		du[9] = c0_1.alp2 * c2_1.ez;
		du[10] = c0_1.alp1 * c2_1.y * c2_1.x11 + xy / f2 * c0_1.cd +
			c0_1.alp2 * *et * c2_1.gz;
		du[11] = -c0_1.alp1 * c2_1.d__ * c2_1.x11 - c0_1.alp2 * *q * c2_1.gz;
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L333: */
			u[i__] += *disl2 / pi2 * du[i__ - 1];
		}
	}
	/* ======================================== */
	/* =====  TENSILE-FAULT CONTRIBUTION  ===== */
	/* ======================================== */
	if (*disl3 != f0) {
		du[0] = -c0_1.alp1 * c2_1.ale - c0_1.alp2 * *q * qy;
		du[1] = -c0_1.alp1 * c2_1.alx - c0_1.alp2 * *q * qx;
		du[2] = c2_1.tt / f2 - c0_1.alp2 * (*et * qx + *xi * qy);
		du[3] = -c0_1.alp1 * xy + c0_1.alp2 * *xi * c2_1.q2 * c2_1.y32;
		du[4] = -c0_1.alp1 / c2_1.r__ + c0_1.alp2 * c2_1.q2 / c2_1.r3;
		du[5] = -c0_1.alp1 * qy - c0_1.alp2 * *q * c2_1.q2 * c2_1.y32;
		du[6] = -c0_1.alp1 * (c0_1.cd / c2_1.r__ + qy * c0_1.sd) - c0_1.alp2 *
			*q * c2_1.fy;
		du[7] = -c0_1.alp1 * c2_1.y * c2_1.x11 - c0_1.alp2 * *q * c2_1.gy;
		du[8] = c0_1.alp1 * (c2_1.d__ * c2_1.x11 + xy * c0_1.sd) + c0_1.alp2 *
			*q * c2_1.hy;
		du[9] = c0_1.alp1 * (c0_1.sd / c2_1.r__ - qy * c0_1.cd) - c0_1.alp2 *
			*q * c2_1.fz;
		du[10] = c0_1.alp1 * c2_1.d__ * c2_1.x11 - c0_1.alp2 * *q * c2_1.gz;
		du[11] = c0_1.alp1 * (c2_1.y * c2_1.x11 + xy * c0_1.cd) + c0_1.alp2 *
			*q * c2_1.hz;
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L444: */
			u[i__] += *disl3 / pi2 * du[i__ - 1];
		}
	}
	return 0;
} /* ua_ */

/* Subroutine */ int ub_gpu(doublereal *xi, doublereal *et, doublereal *q,
	doublereal *disl1, doublereal *disl2, doublereal *disl3, doublereal *
	u) restrict(amp)
{
	/* Initialized data */

	static_def doublereal f0 = 0.;
	static_def doublereal f1 = 1.;
	static_def doublereal f2 = 2.;
	static_def doublereal pi2 = 6.283185307179586f;

	/* Builtin functions */
	//double sqrt(doublereal), atan(doublereal), log(doublereal);

	/* Local variables */
	static_def integer i__;
	static_def doublereal x, d11, rd, du[12], qx, qy, xy, ai1, ai2, aj2, ai4, ai3,
		aj5, ak1, ak3, aj3, aj6, ak2, ak4, aj1, rd2, aj4;


	/* ******************************************************************** */
	/* *****    DISPLACEMENT AND STRAIN AT DEPTH (PART-B)             ***** */
	/* *****    DUE TO BURIED FINITE FAULT IN A SEMIINFINITE MEDIUM   ***** */
	/* ******************************************************************** */

	/* ***** INPUT */
	/* *****   XI,ET,Q : STATION COORDINATES IN FAULT SYSTEM */
	/* *****   DISL1-DISL3 : STRIKE-, DIP-, TENSILE-DISLOCATIONS */
	/* ***** OUTPUT */
	/* *****   U(12) : DISPLACEMENT AND THEIR DERIVATIVES */

		/* Parameter adjustments */
	--u;

	/* Function Body */
/* ----- */
	rd = c2_1.r__ + c2_1.d__;
	d11 = f1 / (c2_1.r__ * rd);
	aj2 = *xi * c2_1.y / rd * d11;
	aj5 = -(c2_1.d__ + c2_1.y * c2_1.y / rd) * d11;
	if (c0_1.cd != f0) {
		if (*xi == f0) {
			ai4 = f0;
		}
		else {
			x = fast_math::sqrt(c2_1.xi2 + c2_1.q2);
			ai4 = f1 / c0_1.cdcd * (*xi / rd * c0_1.sdcd + f2 * fast_math::atan((*et * (
				x + *q * c0_1.cd) + x * (c2_1.r__ + x) * c0_1.sd) / (*xi *
				(c2_1.r__ + x) * c0_1.cd)));
		}
		ai3 = (c2_1.y * c0_1.cd / rd - c2_1.ale + c0_1.sd * fast_math::log(rd)) /
			c0_1.cdcd;
		ak1 = *xi * (d11 - c2_1.y11 * c0_1.sd) / c0_1.cd;
		ak3 = (*q * c2_1.y11 - c2_1.y * d11) / c0_1.cd;
		aj3 = (ak1 - aj2 * c0_1.sd) / c0_1.cd;
		aj6 = (ak3 - aj5 * c0_1.sd) / c0_1.cd;
	}
	else {
		rd2 = rd * rd;
		ai3 = (*et / rd + c2_1.y * *q / rd2 - c2_1.ale) / f2;
		ai4 = *xi * c2_1.y / rd2 / f2;
		ak1 = *xi * *q / rd * d11;
		ak3 = c0_1.sd / rd * (c2_1.xi2 * d11 - f1);
		aj3 = -(*xi) / rd2 * (c2_1.q2 * d11 - f1 / f2);
		aj6 = -c2_1.y / rd2 * (c2_1.xi2 * d11 - f1 / f2);
	}
	/* ----- */
	xy = *xi * c2_1.y11;
	ai1 = -(*xi) / rd * c0_1.cd - ai4 * c0_1.sd;
	ai2 = fast_math::log(rd) + ai3 * c0_1.sd;
	ak2 = f1 / c2_1.r__ + ak3 * c0_1.sd;
	ak4 = xy * c0_1.cd - ak1 * c0_1.sd;
	aj1 = aj5 * c0_1.cd - aj6 * c0_1.sd;
	aj4 = -xy - aj2 * c0_1.cd + aj3 * c0_1.sd;
	/* ===== */
	for (i__ = 1; i__ <= 12; ++i__) {
		/* L111: */
		u[i__] = f0;
	}
	qx = *q * c2_1.x11;
	qy = *q * c2_1.y11;
	/* ====================================== */
	/* =====  STRIKE-SLIP CONTRIBUTION  ===== */
	/* ====================================== */
	if (*disl1 != f0) {
		du[0] = -(*xi) * qy - c2_1.tt - c0_1.alp3 * ai1 * c0_1.sd;
		du[1] = -(*q) / c2_1.r__ + c0_1.alp3 * c2_1.y / rd * c0_1.sd;
		du[2] = *q * qy - c0_1.alp3 * ai2 * c0_1.sd;
		du[3] = c2_1.xi2 * *q * c2_1.y32 - c0_1.alp3 * aj1 * c0_1.sd;
		du[4] = *xi * *q / c2_1.r3 - c0_1.alp3 * aj2 * c0_1.sd;
		du[5] = -(*xi) * c2_1.q2 * c2_1.y32 - c0_1.alp3 * aj3 * c0_1.sd;
		du[6] = -(*xi) * c2_1.fy - c2_1.d__ * c2_1.x11 + c0_1.alp3 * (xy +
			aj4) * c0_1.sd;
		du[7] = -c2_1.ey + c0_1.alp3 * (f1 / c2_1.r__ + aj5) * c0_1.sd;
		du[8] = *q * c2_1.fy - c0_1.alp3 * (qy - aj6) * c0_1.sd;
		du[9] = -(*xi) * c2_1.fz - c2_1.y * c2_1.x11 + c0_1.alp3 * ak1 *
			c0_1.sd;
		du[10] = -c2_1.ez + c0_1.alp3 * c2_1.y * d11 * c0_1.sd;
		du[11] = *q * c2_1.fz + c0_1.alp3 * ak2 * c0_1.sd;
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L222: */
			u[i__] += *disl1 / pi2 * du[i__ - 1];
		}
	}
	/* ====================================== */
	/* =====    DIP-SLIP CONTRIBUTION   ===== */
	/* ====================================== */
	if (*disl2 != f0) {
		du[0] = -(*q) / c2_1.r__ + c0_1.alp3 * ai3 * c0_1.sdcd;
		du[1] = -(*et) * qx - c2_1.tt - c0_1.alp3 * *xi / rd * c0_1.sdcd;
		du[2] = *q * qx + c0_1.alp3 * ai4 * c0_1.sdcd;
		du[3] = *xi * *q / c2_1.r3 + c0_1.alp3 * aj4 * c0_1.sdcd;
		du[4] = *et * *q / c2_1.r3 + qy + c0_1.alp3 * aj5 * c0_1.sdcd;
		du[5] = -c2_1.q2 / c2_1.r3 + c0_1.alp3 * aj6 * c0_1.sdcd;
		du[6] = -c2_1.ey + c0_1.alp3 * aj1 * c0_1.sdcd;
		du[7] = -(*et) * c2_1.gy - xy * c0_1.sd + c0_1.alp3 * aj2 * c0_1.sdcd;
		du[8] = *q * c2_1.gy + c0_1.alp3 * aj3 * c0_1.sdcd;
		du[9] = -c2_1.ez - c0_1.alp3 * ak3 * c0_1.sdcd;
		du[10] = -(*et) * c2_1.gz - xy * c0_1.cd - c0_1.alp3 * *xi * d11 *
			c0_1.sdcd;
		du[11] = *q * c2_1.gz - c0_1.alp3 * ak4 * c0_1.sdcd;
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L333: */
			u[i__] += *disl2 / pi2 * du[i__ - 1];
		}
	}
	/* ======================================== */
	/* =====  TENSILE-FAULT CONTRIBUTION  ===== */
	/* ======================================== */
	if (*disl3 != f0) {
		du[0] = *q * qy - c0_1.alp3 * ai3 * c0_1.sdsd;
		du[1] = *q * qx + c0_1.alp3 * *xi / rd * c0_1.sdsd;
		du[2] = *et * qx + *xi * qy - c2_1.tt - c0_1.alp3 * ai4 * c0_1.sdsd;
		du[3] = -(*xi) * c2_1.q2 * c2_1.y32 - c0_1.alp3 * aj4 * c0_1.sdsd;
		du[4] = -c2_1.q2 / c2_1.r3 - c0_1.alp3 * aj5 * c0_1.sdsd;
		du[5] = *q * c2_1.q2 * c2_1.y32 - c0_1.alp3 * aj6 * c0_1.sdsd;
		du[6] = *q * c2_1.fy - c0_1.alp3 * aj1 * c0_1.sdsd;
		du[7] = *q * c2_1.gy - c0_1.alp3 * aj2 * c0_1.sdsd;
		du[8] = -(*q) * c2_1.hy - c0_1.alp3 * aj3 * c0_1.sdsd;
		du[9] = *q * c2_1.fz + c0_1.alp3 * ak3 * c0_1.sdsd;
		du[10] = *q * c2_1.gz + c0_1.alp3 * *xi * d11 * c0_1.sdsd;
		du[11] = -(*q) * c2_1.hz + c0_1.alp3 * ak4 * c0_1.sdsd;
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L444: */
			u[i__] += *disl3 / pi2 * du[i__ - 1];
		}
	}
	return 0;
} /* ub_ */

/* Subroutine */ int uc_gpu(doublereal *xi, doublereal *et, doublereal *q,
	doublereal *z__, doublereal *disl1, doublereal *disl2, doublereal *
	disl3, doublereal *u)  restrict(amp)
{
	/* Initialized data */

	static_def doublereal f0 = 0.;
	static_def doublereal f1 = 1.;
	static_def doublereal f2 = 2.;
	static_def doublereal f3 = 3.;
	static_def doublereal pi2 = 6.283185307179586f;

	static_def doublereal c__, h__;
	static_def integer i__;
	static_def doublereal y0, z0, du[12], x53, y53, z32, z53, qq, qx, qy, qr, xy,
		yy0, cdr, cqx, ppy, ppz, qqy, qqz;


	/* ******************************************************************** */
	/* *****    DISPLACEMENT AND STRAIN AT DEPTH (PART-C)             ***** */
	/* *****    DUE TO BURIED FINITE FAULT IN A SEMIINFINITE MEDIUM   ***** */
	/* ******************************************************************** */

	/* ***** INPUT */
	/* *****   XI,ET,Q,Z   : STATION COORDINATES IN FAULT SYSTEM */
	/* *****   DISL1-DISL3 : STRIKE-, DIP-, TENSILE-DISLOCATIONS */
	/* ***** OUTPUT */
	/* *****   U(12) : DISPLACEMENT AND THEIR DERIVATIVES */

		/* Parameter adjustments */
	--u;

	/* Function Body */
/* ----- */
	c__ = c2_1.d__ + *z__;
	x53 = (c2_1.r2 * 8.f + c2_1.r__ * 9.f * *xi + f3 * c2_1.xi2) * c2_1.x11 *
		c2_1.x11 * c2_1.x11 / c2_1.r2;
	y53 = (c2_1.r2 * 8.f + c2_1.r__ * 9.f * *et + f3 * c2_1.et2) * c2_1.y11 *
		c2_1.y11 * c2_1.y11 / c2_1.r2;
	h__ = *q * c0_1.cd - *z__;
	z32 = c0_1.sd / c2_1.r3 - h__ * c2_1.y32;
	z53 = f3 * c0_1.sd / c2_1.r5 - h__ * y53;
	y0 = c2_1.y11 - c2_1.xi2 * c2_1.y32;
	z0 = z32 - c2_1.xi2 * z53;
	ppy = c0_1.cd / c2_1.r3 + *q * c2_1.y32 * c0_1.sd;
	ppz = c0_1.sd / c2_1.r3 - *q * c2_1.y32 * c0_1.cd;
	qq = *z__ * c2_1.y32 + z32 + z0;
	qqy = f3 * c__ * c2_1.d__ / c2_1.r5 - qq * c0_1.sd;
	qqz = f3 * c__ * c2_1.y / c2_1.r5 - qq * c0_1.cd + *q * c2_1.y32;
	xy = *xi * c2_1.y11;
	qx = *q * c2_1.x11;
	qy = *q * c2_1.y11;
	qr = f3 * *q / c2_1.r5;
	cqx = c__ * *q * x53;
	cdr = (c__ + c2_1.d__) / c2_1.r3;
	yy0 = c2_1.y / c2_1.r3 - y0 * c0_1.cd;
	/* ===== */
	for (i__ = 1; i__ <= 12; ++i__) {
		/* L111: */
		u[i__] = f0;
	}
	/* ====================================== */
	/* =====  STRIKE-SLIP CONTRIBUTION  ===== */
	/* ====================================== */
	if (*disl1 != f0) {
		du[0] = c0_1.alp4 * xy * c0_1.cd - c0_1.alp5 * *xi * *q * z32;
		du[1] = c0_1.alp4 * (c0_1.cd / c2_1.r__ + f2 * qy * c0_1.sd) -
			c0_1.alp5 * c__ * *q / c2_1.r3;
		du[2] = c0_1.alp4 * qy * c0_1.cd - c0_1.alp5 * (c__ * *et / c2_1.r3 -
			*z__ * c2_1.y11 + c2_1.xi2 * z32);
		du[3] = c0_1.alp4 * y0 * c0_1.cd - c0_1.alp5 * *q * z0;
		du[4] = -c0_1.alp4 * *xi * (c0_1.cd / c2_1.r3 + f2 * *q * c2_1.y32 *
			c0_1.sd) + c0_1.alp5 * c__ * *xi * qr;
		du[5] = -c0_1.alp4 * *xi * *q * c2_1.y32 * c0_1.cd + c0_1.alp5 * *xi *
			(f3 * c__ * *et / c2_1.r5 - qq);
		du[6] = -c0_1.alp4 * *xi * ppy * c0_1.cd - c0_1.alp5 * *xi * qqy;
		du[7] = c0_1.alp4 * f2 * (c2_1.d__ / c2_1.r3 - y0 * c0_1.sd) *
			c0_1.sd - c2_1.y / c2_1.r3 * c0_1.cd - c0_1.alp5 * (cdr *
				c0_1.sd - *et / c2_1.r3 - c__ * c2_1.y * qr);
		du[8] = -c0_1.alp4 * *q / c2_1.r3 + yy0 * c0_1.sd + c0_1.alp5 * (cdr *
			c0_1.cd + c__ * c2_1.d__ * qr - (y0 * c0_1.cd + *q * z0) *
			c0_1.sd);
		du[9] = c0_1.alp4 * *xi * ppz * c0_1.cd - c0_1.alp5 * *xi * qqz;
		du[10] = c0_1.alp4 * f2 * (c2_1.y / c2_1.r3 - y0 * c0_1.cd) * c0_1.sd
			+ c2_1.d__ / c2_1.r3 * c0_1.cd - c0_1.alp5 * (cdr * c0_1.cd +
				c__ * c2_1.d__ * qr);
		du[11] = yy0 * c0_1.cd - c0_1.alp5 * (cdr * c0_1.sd - c__ * c2_1.y *
			qr - y0 * c0_1.sdsd + *q * z0 * c0_1.cd);
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L222: */
			u[i__] += *disl1 / pi2 * du[i__ - 1];
		}
	}
	/* ====================================== */
	/* =====    DIP-SLIP CONTRIBUTION   ===== */
	/* ====================================== */
	if (*disl2 != f0) {
		du[0] = c0_1.alp4 * c0_1.cd / c2_1.r__ - qy * c0_1.sd - c0_1.alp5 *
			c__ * *q / c2_1.r3;
		du[1] = c0_1.alp4 * c2_1.y * c2_1.x11 - c0_1.alp5 * c__ * *et * *q *
			c2_1.x32;
		du[2] = -c2_1.d__ * c2_1.x11 - xy * c0_1.sd - c0_1.alp5 * c__ * (
			c2_1.x11 - c2_1.q2 * c2_1.x32);
		du[3] = -c0_1.alp4 * *xi / c2_1.r3 * c0_1.cd + c0_1.alp5 * c__ * *xi *
			qr + *xi * *q * c2_1.y32 * c0_1.sd;
		du[4] = -c0_1.alp4 * c2_1.y / c2_1.r3 + c0_1.alp5 * c__ * *et * qr;
		du[5] = c2_1.d__ / c2_1.r3 - y0 * c0_1.sd + c0_1.alp5 * c__ / c2_1.r3
			* (f1 - f3 * c2_1.q2 / c2_1.r2);
		du[6] = -c0_1.alp4 * *et / c2_1.r3 + y0 * c0_1.sdsd - c0_1.alp5 * (
			cdr * c0_1.sd - c__ * c2_1.y * qr);
		du[7] = c0_1.alp4 * (c2_1.x11 - c2_1.y * c2_1.y * c2_1.x32) -
			c0_1.alp5 * c__ * ((c2_1.d__ + f2 * *q * c0_1.cd) * c2_1.x32
				- c2_1.y * *et * *q * x53);
		du[8] = *xi * ppy * c0_1.sd + c2_1.y * c2_1.d__ * c2_1.x32 +
			c0_1.alp5 * c__ * ((c2_1.y + f2 * *q * c0_1.sd) * c2_1.x32 -
				c2_1.y * c2_1.q2 * x53);
		du[9] = -(*q) / c2_1.r3 + y0 * c0_1.sdcd - c0_1.alp5 * (cdr * c0_1.cd
			+ c__ * c2_1.d__ * qr);
		du[10] = c0_1.alp4 * c2_1.y * c2_1.d__ * c2_1.x32 - c0_1.alp5 * c__ *
			((c2_1.y - f2 * *q * c0_1.sd) * c2_1.x32 + c2_1.d__ * *et * *
				q * x53);
		du[11] = -(*xi) * ppz * c0_1.sd + c2_1.x11 - c2_1.d__ * c2_1.d__ *
			c2_1.x32 - c0_1.alp5 * c__ * ((c2_1.d__ - f2 * *q * c0_1.cd) *
				c2_1.x32 - c2_1.d__ * c2_1.q2 * x53);
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L333: */
			u[i__] += *disl2 / pi2 * du[i__ - 1];
		}
	}
	/* ======================================== */
	/* =====  TENSILE-FAULT CONTRIBUTION  ===== */
	/* ======================================== */
	if (*disl3 != f0) {
		du[0] = -c0_1.alp4 * (c0_1.sd / c2_1.r__ + qy * c0_1.cd) - c0_1.alp5 *
			(*z__ * c2_1.y11 - c2_1.q2 * z32);
		du[1] = c0_1.alp4 * f2 * xy * c0_1.sd + c2_1.d__ * c2_1.x11 -
			c0_1.alp5 * c__ * (c2_1.x11 - c2_1.q2 * c2_1.x32);
		du[2] = c0_1.alp4 * (c2_1.y * c2_1.x11 + xy * c0_1.cd) + c0_1.alp5 * *
			q * (c__ * *et * c2_1.x32 + *xi * z32);
		du[3] = c0_1.alp4 * *xi / c2_1.r3 * c0_1.sd + *xi * *q * c2_1.y32 *
			c0_1.cd + c0_1.alp5 * *xi * (f3 * c__ * *et / c2_1.r5 - f2 *
				z32 - z0);
		du[4] = c0_1.alp4 * f2 * y0 * c0_1.sd - c2_1.d__ / c2_1.r3 +
			c0_1.alp5 * c__ / c2_1.r3 * (f1 - f3 * c2_1.q2 / c2_1.r2);
		du[5] = -c0_1.alp4 * yy0 - c0_1.alp5 * (c__ * *et * qr - *q * z0);
		du[6] = c0_1.alp4 * (*q / c2_1.r3 + y0 * c0_1.sdcd) + c0_1.alp5 * (*
			z__ / c2_1.r3 * c0_1.cd + c__ * c2_1.d__ * qr - *q * z0 *
			c0_1.sd);
		du[7] = -c0_1.alp4 * f2 * *xi * ppy * c0_1.sd - c2_1.y * c2_1.d__ *
			c2_1.x32 + c0_1.alp5 * c__ * ((c2_1.y + f2 * *q * c0_1.sd) *
				c2_1.x32 - c2_1.y * c2_1.q2 * x53);
		du[8] = -c0_1.alp4 * (*xi * ppy * c0_1.cd - c2_1.x11 + c2_1.y *
			c2_1.y * c2_1.x32) + c0_1.alp5 * (c__ * ((c2_1.d__ + f2 * *q *
				c0_1.cd) * c2_1.x32 - c2_1.y * *et * *q * x53) + *xi * qqy);
		du[9] = -(*et) / c2_1.r3 + y0 * c0_1.cdcd - c0_1.alp5 * (*z__ /
			c2_1.r3 * c0_1.sd - c__ * c2_1.y * qr - y0 * c0_1.sdsd + *q *
			z0 * c0_1.cd);
		du[10] = c0_1.alp4 * f2 * *xi * ppz * c0_1.sd - c2_1.x11 + c2_1.d__ *
			c2_1.d__ * c2_1.x32 - c0_1.alp5 * c__ * ((c2_1.d__ - f2 * *q *
				c0_1.cd) * c2_1.x32 - c2_1.d__ * c2_1.q2 * x53);
		du[11] = c0_1.alp4 * (*xi * ppz * c0_1.cd + c2_1.y * c2_1.d__ *
			c2_1.x32) + c0_1.alp5 * (c__ * ((c2_1.y - f2 * *q * c0_1.sd) *
				c2_1.x32 + c2_1.d__ * *et * *q * x53) + *xi * qqz);
		for (i__ = 1; i__ <= 12; ++i__) {
			/* L444: */
			u[i__] += *disl3 / pi2 * du[i__ - 1];
		}
	}
	return 0;
} /* uc_ */

/* Subroutine */ int dccon0_gpu(doublereal *alpha, doublereal *dip) restrict(amp)
{
	/* Initialized data */

	static_def doublereal f0 = 0.;
	static_def doublereal f1 = 1.;
	static_def doublereal f2 = 2.;
	static_def doublereal pi2 = 6.283185307179586f;
	static_def doublereal eps = 1e-6f;

	/* Builtin functions */
	//double sin(doublereal), cos(doublereal);

	/* Local variables */
	static_def doublereal p18;


	/* ******************************************************************* */
	/* *****   CALCULATE MEDIUM CONSTANTS AND FAULT-DIP CONSTANTS    ***** */
	/* ******************************************************************* */

	/* ***** INPUT */
	/* *****   ALPHA : MEDIUM CONSTANT  (LAMBDA+MYU)/(LAMBDA+2*MYU) */
	/* *****   DIP   : DIP-ANGLE (DEGREE) */
	/* ### CAUTION ### IF COS(DIP) IS SUFFICIENTLY SMALL, IT IS SET TO ZERO */

	/* ----- */
	c0_1.alp1 = (f1 - *alpha) / f2;
	c0_1.alp2 = *alpha / f2;
	c0_1.alp3 = (f1 - *alpha) / *alpha;
	c0_1.alp4 = f1 - *alpha;
	c0_1.alp5 = *alpha;
	/* ----- */
	p18 = pi2 / 360.f;
	c0_1.sd = fast_math::sin(*dip * p18);
	c0_1.cd = fast_math::cos(*dip * p18);
	if (fast_math::fabs(c0_1.cd) < eps) {
		c0_1.cd = f0;
		if (c0_1.sd > f0) {
			c0_1.sd = f1;
		}
		if (c0_1.sd < f0) {
			c0_1.sd = -f1;
		}
	}
	c0_1.sdsd = c0_1.sd * c0_1.sd;
	c0_1.cdcd = c0_1.cd * c0_1.cd;
	c0_1.sdcd = c0_1.sd * c0_1.cd;
	c0_1.s2d = f2 * c0_1.sdcd;
	c0_1.c2d = c0_1.cdcd - c0_1.sdsd;
	return 0;
} /* dccon0_ */

/* Subroutine */ int dccon1_gpu(doublereal *x, doublereal *y, doublereal *d__) restrict(amp)
{
	/* Initialized data */

	static_def doublereal f0 = 0.;
	static_def doublereal f1 = 1.;
	static_def doublereal f3 = 3.;
	static_def doublereal f5 = 5.;
	static_def doublereal eps = 1e-6f;

	/* Builtin functions */
	//double sqrt(doublereal);

	/* Local variables */
	static_def doublereal r7;


	/* ********************************************************************** */
	/* *****   CALCULATE STATION GEOMETRY CONSTANTS FOR POINT SOURCE    ***** */
	/* ********************************************************************** */

	/* ***** INPUT */
	/* *****   X,Y,D : STATION COORDINATES IN FAULT SYSTEM */
	/* ### CAUTION ### IF X,Y,D ARE SUFFICIENTLY SMALL, THEY ARE SET TO ZERO */

	/* ----- */
	if (fast_math::fabs(*x) < eps) {
		*x = f0;
	}
	if (fast_math::fabs(*y) < eps) {
		*y = f0;
	}
	if (fast_math::fabs(*d__) < eps) {
		*d__ = f0;
	}
	c1_2.p = *y * c0_2.cd + *d__ * c0_2.sd;
	c1_2.q = *y * c0_2.sd - *d__ * c0_2.cd;
	c1_2.s = c1_2.p * c0_2.sd + c1_2.q * c0_2.cd;
	c1_2.t = c1_2.p * c0_2.cd - c1_2.q * c0_2.sd;
	c1_2.xy = *x * *y;
	c1_2.x2 = *x * *x;
	c1_2.y2 = *y * *y;
	c1_2.d2 = *d__ * *d__;
	c1_2.r2 = c1_2.x2 + c1_2.y2 + c1_2.d2;
	c1_2.r__ = fast_math::sqrt(c1_2.r2);
	if (c1_2.r__ == f0) {
		return 0;
	}
	c1_2.r3 = c1_2.r__ * c1_2.r2;
	c1_2.r5 = c1_2.r3 * c1_2.r2;
	r7 = c1_2.r5 * c1_2.r2;
	/* ----- */
	c1_2.a3 = f1 - f3 * c1_2.x2 / c1_2.r2;
	c1_2.a5 = f1 - f5 * c1_2.x2 / c1_2.r2;
	c1_2.b3 = f1 - f3 * c1_2.y2 / c1_2.r2;
	c1_2.c3 = f1 - f3 * c1_2.d2 / c1_2.r2;
	/* ----- */
	c1_2.qr = f3 * c1_2.q / c1_2.r5;
	c1_2.qrx = f5 * c1_2.qr * *x / c1_2.r2;
	/* ----- */
	c1_2.uy = c0_2.sd - f5 * *y * c1_2.q / c1_2.r2;
	c1_2.uz = c0_2.cd + f5 * *d__ * c1_2.q / c1_2.r2;
	c1_2.vy = c1_2.s - f5 * *y * c1_2.p * c1_2.q / c1_2.r2;
	c1_2.vz = c1_2.t + f5 * *d__ * c1_2.p * c1_2.q / c1_2.r2;
	c1_2.wy = c1_2.uy + c0_2.sd;
	c1_2.wz = c1_2.uz + c0_2.cd;
	return 0;
} /* dccon1_ */

/* Subroutine */ int dccon2_gpu(doublereal *xi, doublereal *et, doublereal *q,
	doublereal *sd, doublereal *cd, integer *kxi, integer *ket) restrict(amp)
{
	/* Initialized data */

	static_def doublereal f0 = 0.;
	static_def doublereal f1 = 1.;
	static_def doublereal f2 = 2.;
	static_def doublereal eps = 1e-6f;

	/* Builtin functions */
	//double sqrt(doublereal), atan(doublereal), log(doublereal);

	/* Local variables */
	static_def doublereal ret, rxi;


	/* ********************************************************************** */
	/* *****   CALCULATE STATION GEOMETRY CONSTANTS FOR FINITE SOURCE   ***** */
	/* ********************************************************************** */

	/* ***** INPUT */
	/* *****   XI,ET,Q : STATION COORDINATES IN FAULT SYSTEM */
	/* *****   SD,CD   : SIN, COS OF DIP-ANGLE */
	/* *****   KXI,KET : KXI=1, KET=1 MEANS R+XI<EPS, R+ET<EPS, RESPECTIVELY */

	/* ### CAUTION ### IF XI,ET,Q ARE SUFFICIENTLY SMALL, THEY ARE SET TO ZER0 */

	/* ----- */
	if (fast_math::fabs(*xi) < eps) {
		*xi = f0;
	}
	if (fast_math::fabs(*et) < eps) {
		*et = f0;
	}
	if (fast_math::fabs(*q) < eps) {
		*q = f0;
	}
	c2_1.xi2 = *xi * *xi;
	c2_1.et2 = *et * *et;
	c2_1.q2 = *q * *q;
	c2_1.r2 = c2_1.xi2 + c2_1.et2 + c2_1.q2;
	c2_1.r__ = fast_math::sqrt(c2_1.r2);
	if (c2_1.r__ == f0) {
		return 0;
	}
	c2_1.r3 = c2_1.r__ * c2_1.r2;
	c2_1.r5 = c2_1.r3 * c2_1.r2;
	c2_1.y = *et * *cd + *q * *sd;
	c2_1.d__ = *et * *sd - *q * *cd;
	/* ----- */
	if (*q == f0) {
		c2_1.tt = f0;
	}
	else {
		c2_1.tt = fast_math::atan(*xi * *et / (*q * c2_1.r__));
	}
	/* ----- */
	if (*kxi == 1) {
		c2_1.alx = -fast_math::log(c2_1.r__ - *xi);
		c2_1.x11 = f0;
		c2_1.x32 = f0;
	}
	else {
		rxi = c2_1.r__ + *xi;
		c2_1.alx = fast_math::log(rxi);
		c2_1.x11 = f1 / (c2_1.r__ * rxi);
		c2_1.x32 = (c2_1.r__ + rxi) * c2_1.x11 * c2_1.x11 / c2_1.r__;
	}
	/* ----- */
	if (*ket == 1) {
		c2_1.ale = -fast_math::log(c2_1.r__ - *et);
		c2_1.y11 = f0;
		c2_1.y32 = f0;
	}
	else {
		ret = c2_1.r__ + *et;
		c2_1.ale = fast_math::log(ret);
		c2_1.y11 = f1 / (c2_1.r__ * ret);
		c2_1.y32 = (c2_1.r__ + ret) * c2_1.y11 * c2_1.y11 / c2_1.r__;
	}
	/* ----- */
	c2_1.ey = *sd / c2_1.r__ - c2_1.y * *q / c2_1.r3;
	c2_1.ez = *cd / c2_1.r__ + c2_1.d__ * *q / c2_1.r3;
	c2_1.fy = c2_1.d__ / c2_1.r3 + c2_1.xi2 * c2_1.y32 * *sd;
	c2_1.fz = c2_1.y / c2_1.r3 + c2_1.xi2 * c2_1.y32 * *cd;
	c2_1.gy = f2 * c2_1.x11 * *sd - c2_1.y * *q * c2_1.x32;
	c2_1.gz = f2 * c2_1.x11 * *cd + c2_1.d__ * *q * c2_1.x32;
	c2_1.hy = c2_1.d__ * *q * c2_1.x32 + *xi * *q * c2_1.y32 * *sd;
	c2_1.hz = c2_1.y * *q * c2_1.x32 + *xi * *q * c2_1.y32 * *cd;
	return 0;
} /* dccon2_ */

