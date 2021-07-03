#ifndef _DC3D_GPU_H
#define _DC3D_GPU_H

typedef float real;

#if 0
typedef float doublereal;
typedef long int integer;
#else
typedef float doublereal;
typedef int integer;
#endif

#include <amp.h>
#include <amp_math.h>
using namespace concurrency;

#include <vector>

int dc3d0_gpu(real *alpha, real *x, real *y, real *z__, real *
	depth, real *dip, real *pot1, real *pot2, real *pot3, real *pot4, 
	real *ux, real *uy, real *uz, real *uxx, real *uyx, real *uzx, real *
	uxy, real *uyy, real *uzy, real *uxz, real *uyz, real *uzz, integer *
	iret) restrict(amp);

int dc3d_gpu(real *alpha, real *x, real *y, real *z__, real *
	depth, real *dip, real *al1, real *al2, real *aw1, real *aw2, real *
	disl1, real *disl2, real *disl3, real *ux, real *uy, real *uz, real *
	uxx, real *uyx, real *uzx, real *uxy, real *uyy, real *uzy, real *uxz,
	 real *uyz, real *uzz, integer *iret) restrict(amp);

//î}éøíËêî alp = 2.0/3.0;

inline int dc3d_2_gpu( float x, float y, float z, float depth, float dip, float al1, float al2, float aw1, float aw2,
	float disl1, float disl2, float disl3, float& ux, float& uy, float& uz,
	float& uxx, float& uyx, float& uzx,
	float& uxy, float& uyy, float& uzy,
	float& uxz, float& uyz, float& uzz, int& iret) restrict(amp)
{
	real alpha = 0.666666666667f;// == 2.0f/3.0f;
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

	int stat = dc3d_gpu(&alpha, &_x, &_y, &_z, &_depth, &_dip, &_al1, &_al2, &_aw1, &_aw2, &_disl1, &_disl2, &_disl3,
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

inline int dc3d_3_gpu( float strike, float x, float y, float z, float depth, float dip, float al1, float al2, float aw1, float aw2,
	float disl1, float disl2, float disl3, float& ux, float& uy, float& uz,
	float& uxx, float& uyx, float& uzx,
	float& uxy, float& uyy, float& uzy,
	float& uxz, float& uyz, float& uzz, int& iret) restrict(amp)
{
  float C = fast_math::cos ( - strike*3.1415926535f/180.0f );
  float S = fast_math::sin ( - strike*3.1415926535f/180.0f );
  float CC = C * C;
  float CS = C * S;
  float SS = S * S;

  float X1 = C * x - S * y;
  float Y1 = S * x + C * y;
  //Y1 = - Y1;
  float Z1 = z;

  float UX1, UY1, UZ1;
  float UXX1, UYX1, UZX1;
  float UXY1, UYY1, UZY1;
  float UXZ1, UYZ1, UZZ1;

  int stat = dc3d_2_gpu( X1, Y1, Z1, depth, dip, al1, al2, aw1, aw2,
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

inline int dc3d_3_gpu(
	const float C, const float S, const float CC, const float CS, const float SS,
	float strike, float x, float y, float z, float depth, float dip, float al1, float al2, float aw1, float aw2,
	float disl1, float disl2, float disl3, float& ux, float& uy, float& uz, int& iret) restrict(amp)
{
	float uxx, uyx, uzx;
	float uxy, uyy, uzy;
	float uxz, uyz, uzz;

	//float C = cos ( - strike*3.1415926535/180.0 );
	//float S = sin ( - strike*3.1415926535/180.0 );
	//float CC = C * C;
	//float CS = C * S;
	//float SS = S * S;

	float X1 = C * x - S * y;
	float Y1 = S * x + C * y;
	Y1 = - Y1;
	float Z1 = z;

	float UX1, UY1, UZ1;
	float UXX1, UYX1, UZX1;
	float UXY1, UYY1, UZY1;
	float UXZ1, UYZ1, UZZ1;

	int stat = dc3d_2_gpu( X1, Y1, Z1, depth, dip, al1, al2, aw1, aw2,
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
