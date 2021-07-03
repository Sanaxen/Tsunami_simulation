#ifndef _DC3D_H
#define _DC3D_H

//#define DC3D_ORG_TYPE
//#define DC3D_FLOAT_TYPE
//#define DC3D_DOUBLE_TYPE
//#define USE_CPPAMP

#ifdef DC3D_ORG_TYPE
typedef float real;
typedef double doublereal;
typedef long int integer;
#define float_tt(x)	static_cast<float>(x)
#define double_tt(x)	static_cast<double>(x)
#endif

#ifdef DC3D_FLOAT_TYPE
typedef float real;
typedef float doublereal;
typedef long int integer;
#define float_tt(x)	static_cast<float>(x)
#define double_tt(x)	static_cast<float>(x)
#endif

#ifdef DC3D_DOUBLE_TYPE
typedef double real;
typedef double doublereal;
typedef long int integer;
#define float_tt(x)	x
#define double_tt(x)	static_cast<double>(x)
//#undef USE_CPPAMP
#endif

#ifdef USE_CPPAMP
#include <amp.h>
#include <amp_math.h>
using namespace concurrency;

#define ACC_RESTRICTION restrict(amp)
#ifdef DC3D_FLOAT_TYPE
#define ACC_MATH	fast_math::
#else
#define ACC_MATH	precise_math::
#endif

#else
#define ACC_RESTRICTION
#define ACC_MATH
#endif

#include <vector>

typedef
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
} c1_t;


typedef
union {
	struct {
		doublereal alp1, alp2, alp3, alp4, alp5, sd, cd, sdsd, cdcd, sdcd,
			s2d, c2d;
	} _1;
	struct {
		doublereal dummy[5], sd, cd;
	} _2;
} c0_t;


typedef
struct {
	doublereal xi2, et2, q2, r__, r2, r3, r5, y, d__, tt, alx, ale, x11, y11,
		x32, y32, ey, ez, fy, fz, gy, gz, hy, hz;
} c2_t;

typedef struct
{
	c0_t c0;
	c1_t c1;
	c2_t c2;
} c012_t;

int ua0_(c012_t&, doublereal *, doublereal *, doublereal *,
	doublereal *, doublereal *, doublereal *, doublereal *,
	doublereal *) ACC_RESTRICTION;
int ub0_(c012_t&, doublereal *, doublereal *, doublereal *,
	doublereal *, doublereal *, doublereal *, doublereal *,
	doublereal *, doublereal *) ACC_RESTRICTION;
int uc0_(c012_t&, doublereal *, doublereal *,
			doublereal *, doublereal *, doublereal *, doublereal *,
			doublereal *, doublereal *, doublereal *) ACC_RESTRICTION;

int dccon0_(c012_t&, doublereal *, doublereal *) ACC_RESTRICTION;
int dccon1_(c012_t&, doublereal *, doublereal *, doublereal *) ACC_RESTRICTION;
int dccon2_(c012_t&, doublereal *, doublereal *, doublereal *q,
	doublereal *, doublereal *, integer *, integer *) ACC_RESTRICTION;

int ua_(c012_t&, doublereal *, doublereal *, doublereal *,
	doublereal *, doublereal *, doublereal *, doublereal *) ACC_RESTRICTION;
int ub_(c012_t&, doublereal *, doublereal *, doublereal *,
	doublereal *, doublereal *, doublereal *, doublereal *) ACC_RESTRICTION;
int uc_(c012_t&, doublereal *, doublereal *, doublereal *,
	doublereal *, doublereal *, doublereal *, doublereal *, doublereal *) ACC_RESTRICTION;

int dc3d0_(c012_t& c012, real *alpha, real *x, real *y, real *z__, real *
	depth, real *dip, real *pot1, real *pot2, real *pot3, real *pot4, 
	real *ux, real *uy, real *uz, real *uxx, real *uyx, real *uzx, real *
	uxy, real *uyy, real *uzy, real *uxz, real *uyz, real *uzz, integer *
	iret) ACC_RESTRICTION;

int dc3d_(c012_t& c012, real *alpha, real *x, real *y, real *z__, real *
	depth, real *dip, real *al1, real *al2, real *aw1, real *aw2, real *
	disl1, real *disl2, real *disl3, real *ux, real *uy, real *uz, real *
	uxx, real *uyx, real *uzx, real *uxy, real *uyy, real *uzy, real *uxz,
	 real *uyz, real *uzz, integer *iret) ACC_RESTRICTION;

//î}éøíËêî alp = 2.0/3.0;

inline int dc3d_2(c012_t& c012, doublereal x, doublereal y, doublereal z, doublereal depth, doublereal dip, doublereal al1, doublereal al2, doublereal aw1, doublereal aw2,
	doublereal disl1, doublereal disl2, doublereal disl3, doublereal& ux, doublereal& uy, doublereal& uz,
	doublereal& uxx, doublereal& uyx, doublereal& uzx,
	doublereal& uxy, doublereal& uyy, doublereal& uzy,
	doublereal& uxz, doublereal& uyz, doublereal& uzz, int& iret) ACC_RESTRICTION
{
	real alpha = 2.0f/3.0f;
	real _x = (real)x;
	real _y = (real)y;
	real _z = (real)z;
	real _depth = (real)depth;
	real _dip = (real)dip;
	real _al1 = (real)al1;
	real _al2 = (real)al2;
	real _aw1 = (real)aw1;
	real _aw2 = (real)aw2;
	real _disl1 = (real)disl1;
	real _disl2 = (real)disl2;
	real _disl3 = (real)disl3;

	real _ux, _uy, _uz;
	real _uxx, _uyx, _uzx;
	real _uxy, _uyy, _uzy;
	real _uxz, _uyz, _uzz;
	integer _iret;

	int stat = dc3d_(c012, &alpha, &_x, &_y, &_z, &_depth, &_dip, &_al1, &_al2, &_aw1, &_aw2, &_disl1, &_disl2, &_disl3,
		&_ux, &_uy, &_uz,
		&_uxx, &_uyx, &_uzx,
		&_uxy, &_uyy, &_uzy,
		&_uxz, &_uyz, &_uzz, &_iret);
	ux = _ux;
	uy = _uy;
	uz = _uz;
	uxx = _uxx;
	uyx = _uyx;
	uzx = _uzx;
	uxy = _uxy;
	uyy = _uyy;
	uzy = _uzy;
	uxz = _uxz;
	uyz = _uyz;
	uzz = _uzz;
	iret = _iret;

	return stat;
}

inline int dc3d_3(c012_t& c012, doublereal C, doublereal S, doublereal CC, doublereal CS, doublereal SS,
	doublereal strike, doublereal x, doublereal y, doublereal z, doublereal depth, doublereal dip, doublereal al1, doublereal al2, doublereal aw1, doublereal aw2,
	doublereal disl1, doublereal disl2, doublereal disl3, doublereal& ux, doublereal& uy, doublereal& uz,
	doublereal& uxx, doublereal& uyx, doublereal& uzx,
	doublereal& uxy, doublereal& uyy, doublereal& uzy,
	doublereal& uxz, doublereal& uyz, doublereal& uzz, int& iret) ACC_RESTRICTION
{
  //doublereal C = cos ( - strike*3.1415926535/180.0 );
  //doublereal S = sin ( - strike*3.1415926535/180.0 );
  //doublereal CC = C * C;
  //doublereal CS = C * S;
  //doublereal SS = S * S;

  doublereal X1 = C * x - S * y;
  doublereal Y1 = S * x + C * y;
  //Y1 = - Y1;
  doublereal Z1 = z;

  doublereal UX1, UY1, UZ1;
  doublereal UXX1, UYX1, UZX1;
  doublereal UXY1, UYY1, UZY1;
  doublereal UXZ1, UYZ1, UZZ1;

  int stat = dc3d_2( c012, X1, Y1, Z1, depth, dip, al1, al2, aw1, aw2,
					disl1, disl2, disl3,  UX1, UY1, UZ1,
					 UXX1, UYX1, UZX1,
					 UXY1, UYY1, UZY1,
					 UXZ1, UYZ1, UZZ1,  iret);

	ux =   C * UX1 + S * UY1;
	uy = - S * UX1 + C * UY1;
	uz = UZ1;
	uxx =   CC * UXX1 + CS * UYX1 - SS * UXY1 + CS * UYY1;
	uxy = - CS * UXX1 - SS * UYX1 + CC * UXY1 + CS * UYY1;
	uyx = - CS * UXX1 + CC * UYX1 - SS * UXY1 + CS * UYY1;
	uyy =   SS * UXX1 - CS * UYX1 - CS * UXY1 + CC * UYY1;
	uxz =   C * UXZ1 + S * UYZ1;
	uyz = - S * UXZ1 + C * UYZ1;
	uzx =   C * UZX1 + S * UZY1;
	uzy = - S * UZX1 + C * UZY1;
	uzz = UZZ1;

	//uy = - uy;
	//uz = - uz;
	//uxy = - uyx;
	//uxz = - uxz;
	//uyx = - uyx;
	//uzx = - uzx;

	return stat;
}

inline int dc3d_3(c012_t& c012, doublereal C, doublereal S, doublereal CC, doublereal CS, doublereal SS,
	doublereal strike, doublereal x, doublereal y, doublereal z, doublereal depth, doublereal dip, doublereal al1, doublereal al2, doublereal aw1, doublereal aw2,
	doublereal disl1, doublereal disl2, doublereal disl3, doublereal& ux, doublereal& uy, doublereal& uz, int& iret)ACC_RESTRICTION
{
	doublereal uxx, uyx, uzx;
	doublereal uxy, uyy, uzy;
	doublereal uxz, uyz, uzz;

	//doublereal C = cos ( - strike*3.1415926535/180.0 );
	//doublereal S = sin ( - strike*3.1415926535/180.0 );
	//doublereal CC = C * C;
	//doublereal CS = C * S;
	//doublereal SS = S * S;

	doublereal X1 = C * x - S * y;
	doublereal Y1 = S * x + C * y;
	Y1 = - Y1;
	doublereal Z1 = z;

	doublereal UX1, UY1, UZ1;
	doublereal UXX1, UYX1, UZX1;
	doublereal UXY1, UYY1, UZY1;
	doublereal UXZ1, UYZ1, UZZ1;

	int stat = dc3d_2( c012, X1, Y1, Z1, depth, dip, al1, al2, aw1, aw2,
			disl1, disl2, disl3,  UX1, UY1, UZ1,
				UXX1, UYX1, UZX1,
				UXY1, UYY1, UZY1,
				UXZ1, UYZ1, UZZ1,  iret);

	ux =   C * UX1 + S * UY1;
	uy = - S * UX1 + C * UY1;
	uz = UZ1;
	uxx =   CC * UXX1 + CS * UYX1 - SS * UXY1 + CS * UYY1;
	uxy = - CS * UXX1 - SS * UYX1 + CC * UXY1 + CS * UYY1;
	uyx = - CS * UXX1 + CC * UYX1 - SS * UXY1 + CS * UYY1;
	uyy =   SS * UXX1 - CS * UYX1 - CS * UXY1 + CC * UYY1;
	uxz =   C * UXZ1 + S * UYZ1;
	uyz = - S * UXZ1 + C * UYZ1;
	uzx =   C * UZX1 + S * UZY1;
	uzy = - S * UZX1 + C * UZY1;
	uzz = UZZ1;

	uy = - uy;
	uz = - uz;
	uxy = - uyx;
	uxz = - uxz;
	uyx = - uyx;
	uzx = - uzx;

	return stat;
}
#endif
