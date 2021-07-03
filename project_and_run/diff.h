
#ifdef USE_HIGH_ORDER
#define DIFF(f)			((f[0] - 8*f[1] + 8*f[2] - f[3])*C1_12)
#define DIFFx(f,i,j)	((f ARY(i,j-2) - 8*f ARY(i,j-1) + 8*f ARY(i,j+1) - f ARY(i,j+2))*C1_12)
#define DIFFy(f,i,j)	((f ARY(i-2,j) - 8*f ARY(i-1,j) + 8*f ARY(i+1,j) - f ARY(i+2,j))*C1_12)

#define DIFF_(f)		((3*f[0] - 32*f[1] +168*f[2] - 672*f[3] + 672*f[4] - 168*f[5] + 32*f[6] -3*f[7])*C1_840)
#define DIFFx_(f,i,j)	((3*f ARY(i,j-4) - 32*f ARY(i,j-3) +168*f ARY(i,j-2) - 672*f ARY(i,j-1) + 672*f ARY(i,j+1) - 168*f ARY(i,j+2) + 32*f ARY(i,j+3) -3*f ARY(i,j+4))*C1_840)
#define DIFFy_(f,i,j)	((3*f ARY(i-4,j) - 32*f ARY(i-3,j) +168*f ARY(i-2,j) - 672*f ARY(i-1,j) + 672*f ARY(i+1,j) - 168*f ARY(i+2,j) + 32*f ARY(i+3,j) -3*f ARY(i+4,j))*C1_840)

#define DIFFxy_(f,i,j)	((3*DIFFy_(f,i,j-4) - 32*DIFFy_(f,i,j-3) +168*DIFFy_(f,i,j-2) - 672*DIFFy_(f,i,j-1) + 672*DIFFy_(f,i,j+1) - 168*DIFFy_(f,i,j+2) + 32*DIFFy_(f,i,j+3) -3*DIFFy_(f,i,j+4))*C1_840)

#define DIFFxx_(f,i,j)	((-9*f ARY(i,j-4) + 128*f ARY(i,j-3) - 1008*f ARY(i,j-2) + 8064*f ARY(i,j-1) - 14350*f ARY(i,j) + 8064*f ARY(i,j+1) - 1008*f ARY(i,j+2) + 128*f ARY(i,j+3) - 9*f ARY(i,j+4))*C1_5040)
#define DIFFyy_(f,i,j)	((-9*f ARY(i-4,j) + 128*f ARY(i-3,j) - 1008*f ARY(i-2,j) + 8064*f ARY(i-1,j) - 14350*f ARY(i,j) + 8064*f ARY(i+1,j) - 1008*f ARY(i+2,j) + 128*f ARY(i+3,j) - 9*f ARY(i+4,j))*C1_5040)

#define DIFFxxx_(f,i,j)	((-7*f ARY(i,j-4) + 72*f ARY(i,j-3) - 338*f ARY(i,j-2) + 488*f ARY(i,j-1) - 488*f ARY(i,j+1) + 338*f ARY(i,j+2) - 72*f ARY(i,j+3) + 7*f ARY(i,j+4))*C1_240)
#define DIFFyyy_(f,i,j)	((-7*f ARY(i-4,j) + 72*f ARY(i-3,j) - 338*f ARY(i-2,j) + 488*f ARY(i-1,j) - 488*f ARY(i+1,j) + 338*f ARY(i+2,j) - 72*f ARY(i+3,j) + 7*f ARY(i+4,j))*C1_240)

#define DIFFxyy_(f,i,j)	((3*DIFFyy_(f,i,j-4) - 32*DIFFyy_(f,i,j-3) +168*DIFFyy_(f,i,j-2) - 672*DIFFyy_(f,i,j-1) + 672*DIFFyy_(f,i,j+1) - 168*DIFFyy_(f,i,j+2) + 32*DIFFyy_(f,i,j+3) -3*DIFFyy_(f,i,j+4))*C1_840)
#define DIFFxxy_(f,i,j)	((3*DIFFxx_(f,i-4,j) - 32*DIFFxx_(f,i-3,j) +168*DIFFxx_(f,i-2,j) - 672*DIFFxx_(f,i-1,j) + 672*DIFFxx_(f,i+1,j) - 168*DIFFxx_(f,i+2,j) + 32*DIFFxx_(f,i+3,j) -3*DIFFxx_(f,i+4,j))*C1_840)

#else
//#if 10
//#define DIFF(f)			((f[0] - 8*f[1] + 8*f[2] - f[3])*C1_12)
//#define DIFFx(f,i,j)	((f ARY(i,j-2) - 8*f ARY(i,j-1) + 8*f ARY(i,j+1) - f ARY(i,j+2))*C1_12)
//#define DIFFy(f,i,j)	((f ARY(i-2,j) - 8*f ARY(i-1,j) + 8*f ARY(i+1,j) - f ARY(i+2,j))*C1_12)
//
//#define DIFF_(f)		((f[0] - 8*f[1] + 8*f[2] - f[3])*C1_12)
//#define DIFFx_(f,i,j)	((f ARY(i,j-2) - 8*f ARY(i,j-1) + 8*f ARY(i,j+1) - f ARY(i,j+2))*C1_12)
//#define DIFFy_(f,i,j)	((f ARY(i-2,j) - 8*f ARY(i-1,j) + 8*f ARY(i+1,j) - f ARY(i+2,j))*C1_12)
//
//#define DIFFxy_(f,i,j)	((DIFFy_(f,i,j-2) - 8*DIFFy_(f,i,j-1) + 8*DIFFy_(f,i,j+1) - DIFFy_(f,i,j+2))*C1_12)
//
//#define DIFFxx_(f,i,j)	((- f ARY(i,j-2) + 16*f ARY(i,j-1) - 30*f ARY(i,j) + 16*f ARY(i,j+1) - f ARY(i,j+2))*C1_12)
//#define DIFFyy_(f,i,j)	((- f ARY(i-2,j) + 16*f ARY(i-1,j) - 30*f ARY(i,j) + 16*f ARY(i+1,j) - f ARY(i+2,j))*C1_12)
//
//#define DIFFxxx_(f,i,j)	((f ARY(i,j-3) - 8*f ARY(i,j-2) + 13*f ARY(i,j-1) - 13*f ARY(i,j+1) + 8*f ARY(i,j+2) -f ARY(i,j+3))*C1_08)
//#define DIFFyyy_(f,i,j)	((f ARY(i-3,j) - 8*f ARY(i-2,j) + 13*f ARY(i-1,j) - 13*f ARY(i+1,j) + 8*f ARY(i+2,j) -f ARY(i+3,j))*C1_08)
//
//#define DIFFxyy_(f,i,j)	((DIFFyy_(f,i,j-3) -8*DIFFyy_(f,i,j-2) + 13*DIFFyy_(f,i,j-1) -13*DIFFyy_(f,i,j+1) + 8*DIFFyy_(f,i,j+2) - DIFFyy_(f,i,j+3))*C1_08)
//#define DIFFxxy_(f,i,j)	((DIFFxx_(f,i-3,j) -8*DIFFxx_(f,i-2,j) + 13*DIFFxx_(f,i-1,j) -13*DIFFxx_(f,i+1,j) + 8*DIFFxx_(f,i+2,j) - DIFFxx_(f,i+3,j))*C1_08)
//
//#else

//1次精度１階微分
#define DIFF(f)			((-f[0] + f[1])*0.5)
#define DIFFx(f,i,j)	((- f ARY(i,j-1) + f ARY(i,j+1))*0.5)
#define DIFFy(f,i,j)	((- f ARY(i-1,j) + f ARY(i+1,j))*0.5)

#define DIFF_(f)		((-f[0] + f[1])*0.5)
#define DIFFx_(f,i,j)	((- f ARY(i,j-1) + f ARY(i,j+1))*0.5)
#define DIFFy_(f,i,j)	((- f ARY(i-1,j) + f ARY(i+1,j))*0.5)


//●●●●●●●●　1次精度差分　●●●●●●●●
//1次精度∂^2f/∂x∂x
#define DIFFxx_(f,i,j)	((f ARY(i,j-1) -2.0*f ARY(i,j) + f ARY(i,j+1)))
//1次精度∂^2f/∂y∂y
#define DIFFyy_(f,i,j)	((f ARY(i-1,j) -2.0*f ARY(i,j) + f ARY(i+1,j)))

//1次精度∂^2f/∂x∂y
//=(f(i+1,j+1)-f(i+1,j-1)-f(i-1,j+1)+f(i-1,j-1))/(4hk)
//#define DIFFxy_(f,i,j)	((f ARY(i+1,j+1) - f ARY(i+1,j-1) - f ARY(i-1,j+1) + f ARY(i-1,j-1))*0.25)
#define DIFFxy_(f,i,j)	((- DIFFx_(f,i-1,j) + DIFFx_(f,i+1,j))*0.5)

//2次精度∂^3f/∂x∂x∂x (1次精度では定義できない）
#define DIFFxxx_2(f,i,j)	(0.5*(-f ARY(i,j-2) + 2.0*(f ARY(i,j-1) -f ARY(i,j+1)) +f ARY(i,j+2)))
//2次精度∂^3f/∂y∂y∂y　(1次精度では定義できない）
#define DIFFyyy_2(f,i,j)	(0.5*(-f ARY(i-2,j) + 2.0*(f ARY(i-1,j) -f ARY(i+1,j)) +f ARY(i+2,j)))

//1次精度 ∂^3f/∂x∂x∂x
#define DIFFxxx_(f,i,j)	((- DIFFxx_(f,i,j-1) + DIFFxx_(f,i,j+1))*0.5)
//1次精度 ∂^3f/∂y∂y∂y
#define DIFFyyy_(f,i,j)	((- DIFFyy_(f,i-1,j) + DIFFyy_(f,i+1,j))*0.5)

//1次精度 ∂^3f/∂x∂y∂y
#define DIFFxyy_(f,i,j)	((- DIFFyy_(f,i,j-1) + DIFFyy_(f,i,j+1))*0.5)
//1次精度 ∂^3f/∂x∂x∂y
#define DIFFxxy_(f,i,j)	((- DIFFxx_(f,i-1,j) + DIFFxx_(f,i+1,j))*0.5)

//●●●●●●●●　2次精度差分　●●●●●●●●
//2次精度１階微分
//		//-2 -1  +1 +2
//		return (f[0] - 8*f[1] + 8*f[2] - f[3])*C1_12;
#define DIFF_2(f)	((f[0] - 8.0*(f[1] - f[2]) - f[3])*C1_12)
#define DIFFx_2(f,i,j)	((f ARY(i,j-2) - 8.0*(f ARY(i,j-1) - f ARY(i,j+1)) - f ARY(i,j+2))*C1_12)
#define DIFFy_2(f,i,j)	((f ARY(i-2,j) - 8.0*(f ARY(i-1,j) - f ARY(i+1,j)) - f ARY(i+2,j))*C1_12)

//2次精度∂^2f/∂x∂y
#define DIFFxy_2(f,i,j)	((DIFFy_2(f,i,j-2) - 8.0*(DIFFy_2(f,i,j-1) - DIFFy_2(f,i,j+1)) - DIFFy_2(f,i,j+2))*C1_12)

//2次精度∂^2f/∂x∂x
#define DIFF2(f)	((-f[0] + 16.0 * (f[1] + f[3] )- 30.0 * f[2] - f[4])*C1_12)
//-2 -1  0 +1 +2
//(-f[0] + 16*f[1] -30*f[2] +16*f[3] -f[4])*C1_12;
#define DIFFxx_2(f,i,j)	((-f ARY(i,j-2) +16.0*(f ARY(i,j-1)  + f ARY(i,j+1)) - 30.0*f ARY(i,j) - f ARY(i,j+2))*C1_12)
//2次精度∂^2f/∂y∂y
#define DIFFyy_2(f,i,j)	((-f ARY(i-2,j) +16.0*(f ARY(i-1,j)  + f ARY(i+1,j)) - 30.0*f ARY(i,j) - f ARY(i+2,j))*C1_12)

// -3 -2 -1   +1 +2 +3
//return (f[0] -8*f[1] + 13*f[2] - 13*f[3] + 8*f[4] - f[5])*C1_08;
//3次精度∂^3f/∂x∂x∂x 
#define DIFFxxx_3(f,i,j)	((f ARY(i,j-3) - 8.0*(f ARY(i,j-2)  -f ARY(i,j+2))+ 13.0*(f ARY(i,j-1) -f ARY(i,j+1)) -f ARY(i,j+3))*C1_08)
//3次精度∂^3f/∂x∂x∂x 
#define DIFFyyy_3(f,i,j)	((f ARY(i-3,j) - 8.0*(f ARY(i-2,j)  -f ARY(i+2,j))+ 13.0*(f ARY(i-1,j) -f ARY(i+1,j)) -f ARY(i+3,j))*C1_08)

//2次精度 ∂^3f/∂x∂y∂y
#define DIFFxyy_2(f,i,j)	((DIFFyy_2(f,i,j-2) - 8.0*(DIFFyy_2(f,i,j-1) -DIFFyy_2(f,i,j+1)) - DIFFyy_2(f,i,j+2))*C1_12)
//2次精度 ∂^3f/∂x∂x∂y
#define DIFFxxy_2(f,i,j)	((DIFFxx_2(f,i-2,j) - 8.0*(DIFFxx_2(f,i-1,j) -DIFFxx_2(f,i+1,j)) - DIFFxx_2(f,i+2,j))*C1_12)


//●●●●●●●●　4次精度差分　●●●●●●●●
//4次精度１階微分
//// -4 -3 -2 -1  +1 +2 +3 +4
//return (3*f[0] - 32*f[1] +168*f[2] - 672*f[3] + 672*f[4] - 168*f[5] + 32*f[6] -3*f[7])*C1_840;
#define DIFF_4(f)	    ((3.0*f[0] - 32.0*f[1] + 168.0*f[2] - 672.0*f[3] + 672.0*f[4]  - 168.0*f[5]  + 32.0*f[6]  - 3.0*f[7])*C1_840)
#define DIFFx_4(f,i,j)	((3.0*f ARY(i,j-4) - 32.0*f ARY(i,j-3) + 168.0*f ARY(i,j-2) - 672.0*f ARY(i,j-1) + 672.0*f ARY(i,j+1) - 168.0*f ARY(i,j+2) + 32.0*f ARY(i,j+3) - 3.0*f ARY(i,j+4))*C1_840)
#define DIFFy_4(f,i,j)	((3.0*f ARY(i-4,j) - 32.0*f ARY(i-3,j) + 168.0*f ARY(i-2,j) - 672.0*f ARY(i-1,j) + 672.0*f ARY(i+1,j) - 168.0*f ARY(i+2,j) + 32.0*f ARY(i+3,j) - 3.0*f ARY(i+4,j))*C1_840)


//4次精度∂^2f/∂x∂y
#define DIFFxy_4(f,i,j)	((3.0*DIFFy_4(f,i,j-4) - 32.0*DIFFy_4(f,i,j-3) + 168.0*DIFFy_4(f,i,j-2) - 672.0*DIFFy_4(f,i,j-1) + 672.0*DIFFy_4(f,i,j+1) - 168.0*DIFFy_4(f,i,j+2) + 32.0*DIFFy_4(f,i,j+3) - 3.0*DIFFy_4(f,i,j+4))*C1_840)

//// -4 -3 -2 -1 0 +1 +2 +3 +4
//return (-9*f[0] + 128*f[1] - 1008*f[2] + 8064*f[3] - 14350*f[4] + 8064*f[5] - 1008*f[6] + 128*f[7] - 9*f[8])*C1_5040;
//4次精度∂^2f/∂x∂x
#define DIFFxx_4(f,i,j)	((-9.0*f ARY(i,j-4) +128.0*f ARY(i,j-3) - 1008.0*f ARY(i,j-2) + 8064.0*f ARY(i,j-1) - 14350.0*f ARY(i,j) + 8064.0*f ARY(i,j+1) - 1008.0*f ARY(i,j+2) + 126.0*f ARY(i,j+3) - 9.0*f ARY(i,j+4))*C1_5040)
//2次精度∂^2f/∂y∂y
#define DIFFyy_4(f,i,j)	((-9.0*f ARY(i-4,j) +128.0*f ARY(i-3,j) - 1008.0*f ARY(i-2,j) + 8064.0*f ARY(i-1,j) - 14350.0*f ARY(i,j) + 8064.0*f ARY(i+1,j) - 1008.0*f ARY(i+2,j) + 126.0*f ARY(i+3,j) - 9.0*f ARY(i+4,j))*C1_5040)


//// -4 -3 -2 -1  +1 +2 +3 +4
//return (-7*f[0] + 72*f[1] - 338*f[2] + 488*f[3] - 488*f[4] + 338*f[5] - 72*f[6] + 7*f[7])*C1_240;
//4次精度∂^3f/∂x∂x∂x 
#define DIFFxxx_4(f,i,j)	((-7.0*f ARY(i,j-4) + 72.0*f ARY(i,j-3) - 338.0*f ARY(i,j-2) +488.0*f ARY(i,j-1) -488.0*f ARY(i,j+1) + 338.0*f ARY(i,j+2)- 72.0*f ARY(i,j+3) + 7.0*f ARY(i,j+4))*C1_240)
//4次精度∂^3f/∂y∂y∂y 
#define DIFFyyy_4(f,i,j)	((-7.0*f ARY(i-4,j) + 72.0*f ARY(i-3,j) - 338.0*f ARY(i-2,j) +488.0*f ARY(i-1,j) -488.0*f ARY(i+1,j) + 338.0*f ARY(i+2,j)- 72.0*f ARY(i+3,j) + 7.0*f ARY(i+4,j))*C1_240)

//4次精度 ∂^3f/∂x∂y∂y
#define DIFFxyy_4(f,i,j)	((3.0*DIFFyy_4(f,i,j-4) - 32.0*DIFFyy_4(f,i,j-3) + 168.0*DIFFyy_4(f,i,j-2) - 672.0*DIFFyy_4(f,i,j-1) + 672.0*DIFFyy_4(f,i,j+1) - 168.0*DIFFyy_4(f,i,j+2) + 32.0*DIFFyy_4(f,i,j+3) - 3.0*DIFFyy_4(f,i,j+4))*C1_840)
//4次精度 ∂^3f/∂x∂x∂y
#define DIFFxxy_4(f,i,j)	((3.0*DIFFxx_4(f,i-4,j) - 32.0*DIFFxx_4(f,i-3,j) + 168.0*DIFFxx_4(f,i-2,j) - 672.0*DIFFxx_4(f,i-1,j) + 672.0*DIFFxx_4(f,i+1,j) - 168.0*DIFFxx_4(f,i+2,j) + 32.0*DIFFxx_4(f,i+3,j) - 3.0*DIFFxx_4(f,i+4,j))*C1_840)


//一次精度風上差分
#define UPWIND1(mm,a)  mm[4]*DIFF(mm) - (a)*0.5*fabs(mm[4])*(mm[1] - 2.0*mm[3] + mm[0])

//二次精度風上差分
#define UPWIND2(mm,a)  mm[6]*(mm[0]+4.0*(-mm[1]+mm[2])-mm[3])*0.25 \
					+(a)*0.25*fabs(mm[6])*(mm[0] - 4.0*mm[1] + 6.0*mm[5] -4.0*mm[2] + mm[3])

//三次精度風上差分
#define UPWIND3(mm,a)  mm[6]*(mm[0]+8.0*(-mm[1]+mm[2])-mm[3])*C1_12 \
					+(a)*C1_12*fabs(mm[6])*(mm[0] - 4.0*mm[1] + 6.0*mm[5] -4.0*mm[2] + mm[3])

//#undef UPWIND3
//#define UPWIND3(mm,a) UPWIND2(mm,a)

//#endif
#endif

