#ifndef _FAULT_DEFORMATION_H_

#define _FAULT_DEFORMATION_H_

#include <iostream>
#include <time.h>
#include <algorithm>

#include "DC3D.h"
#include "bitmap.h"
#include "CsvReadWrite.hpp"

using namespace std;

//#define Z	10
//#define L	85.0511278
#define PI		3.141592653589793
#define PI_180	0.01745329251994329

#define MU	(4.10e10)	//岩盤の剛性率[N/m^2]

//断層パラメータ
class FaultLine
{
public:
	double lo;		//経度
	double la;		//緯度
	double length;	//断層長さ
	double width;	//幅
	double depth;	//深さ
	double dir;		//走向(deg)
	double dip;		//傾斜角(deg)
	double rake;	//すべり角(deg)
	double slip;	//すべり量(m)
	double t;		//最初の断層破壊を0としたときの時間差
	double s;		//破壊時間
	double a;		//面積
	double M0;		//地震モーメント
	double Mw;		//モーメントマグニチュード
	int horizontal_displacement;	//Tanioka and Satake (1995) 鉛直変位に加え，傾斜する海底面の水平変位に伴う水塊の運動を考慮
	
	bool operator <(const FaultLine& f) { // 大小比較用 less 演算子
        return t < f.t;
    }
};


class Meteo
{
public:
	double lo;		//経度
	double la;		//緯度
	double D;		//クレータ直径
	double D0;		//波の立つ直径
	double depth;	//深さ
	double h;		//初期波高さ
};

//角度表現 「度 分 秒」 を 「度」に変換する
inline double to_360(double h, double m, double s)
{
	return (h+m/60.0 + s/3600.0);
}

//角度表現 「度」 を 「度 分 秒」に変換する
inline void to_60(double d, double& h, double& m, double& s)
{
	int x = (int)(d);
	h = x;

	int y = (int)((d - h)*60.0);
	m = y;

	s = (d - (h+m/60.0))*3600.0;
}

#define EPS5 0.001  /* DBL_EPSILON の 1/5 乗程度 */
inline double arctanh(double x)  /* $\tanh^{-1} x$ */
{
	if (fabs(x) > EPS5)
		return 0.5 * log((1 + x) / (1 - x));
	return x * (1 + x * x / 3.0);
}


//ビットマップ(x,y)の位置にマークを付ける
void BitmpMarke(char* fname, char* out, int x, int y, int w);

//ビットマップ(x,y)の位置から指定角度th(deg)で長さlengthの線を引く
// dx,dyは格子サイズ
void BitmpLine(char* fname, char* out, int x, int y, int w, double dx, double dy, double length, double th);

//ピクセル座標系->世界測地座標系
void coord_to_LatitudeLongitude( double p, double q, double& x, double& y);

//世界測地座標系->ピクセル座標系
void coord_to_pixcel(double x, double y, double& p, double& q);

//画像の四隅のピクセル座標を指定して画像の(x,y)位置の緯度経度を計算する
// p:横, q:縦（上から下）
// p[0],q[0] ----------- p[1],q[1]
//  +                        +
//  |                        |
//  |                        |
//  |                        |
//  |                        |
// p[2],q[2] ----------- p[3],q[3]
void bitmap_to_LatitudeLongitude(int w, int h, double p[4], double q[4], int xx, int yy, double& lo, double& la);


//画像の四隅のピクセル座標を指定して経度緯度(lo,la)位置のビットマップ座標を計算する
// p:横, q:縦（上から下）
// p[0],q[0] ----------- p[1],q[1]
//  +                        +
//  |                        |
//  |                        |
//  |                        |
//  |                        |
// p[2],q[2] ----------- p[3],q[3]
void latitudeLongitude_to_Bitmap(int w, int h, double p[4], double q[4], double lo, double la, int& xx, int& yy);


//画像の四隅のピクセル座標を指定して断層パラメータの線を引く
// p:横, q:縦（上から下）
// p[0],q[0] ----------- p[1],q[1]
//  +                        +
//  |                        |
//  |                        |
//  |                        |
//  |                        |
// p[2],q[2] ----------- p[3],q[3]
//w,hは画像サイズ
//dx,dyは格子サイズ
void BitmpFaultLine(int w, int h, double p[4], double q[4], std::vector<FaultLine>& fault, double dx, double dy, char* fname, char* out);


//画像の四隅のピクセル座標を指定して断層変位コンタを画く
// p:横, q:縦（上から下）
// p[0],q[0] ----------- p[1],q[1]
//  +                        +
//  |                        |
//  |                        |
//  |                        |
//  |                        |
// p[2],q[2] ----------- p[3],q[3]
//w,hは画像サイズ
//dx,dyは格子サイズ
void CalcFault(int w, int h, double p[4], double q[4], FaultLine& fault, double dx, double dy, double* depth_map, double* zval, double& uzmin, double& uzmax);

void BitmpFaultWrite(int w, int h, double* zval, double zmin, double zmax, char* fname, char* out, char* Initial_wave_data_bmp );

double* CalcFaultListWrite(int w, int h, double p[4], double q[4], std::vector<FaultLine>& fault, Meteo* meteo, double dx, double dy, double* depth_map, char* fname, char* out, char* Initial_wave_data_bmp, int t_flag, char* drive2, char* dir2);

doublereal  distance_lambert(doublereal lat1, doublereal lon1, doublereal lat2, doublereal lon2);
doublereal  distance_lambert_amp(doublereal lat1, doublereal lon1, doublereal lat2, doublereal lon2)ACC_RESTRICTION;

//断層変位計算メイン
int fault_deformation( char* parameterFile);

// Xor-Shiftによる乱数ジェネレータ
class XorShift
{
	unsigned int seed_[4];
public:
	unsigned int next(void) ACC_RESTRICTION {
		const unsigned int t = seed_[0] ^ (seed_[0] << 11);
		seed_[0] = seed_[1];
		seed_[1] = seed_[2];
		seed_[2] = seed_[3];
		return seed_[3] = (seed_[3] ^ (seed_[3] >> 19)) ^ (t ^ (t >> 8));
	}

	doublereal next01(void) ACC_RESTRICTION {
		return (doublereal)next() / UINT_MAX;
	}
	doublereal Next01(void) ACC_RESTRICTION {
		return next01();
	}
	XorShift(const unsigned int initial_seed) ACC_RESTRICTION {
		seed(initial_seed);
	}
	XorShift(const int initial_seed) ACC_RESTRICTION {
		seed(initial_seed);
	}
	void seed(unsigned int initial_seed)ACC_RESTRICTION
	{
		unsigned int s = initial_seed;
		for (int i = 1; i <= 4; i++) {
			seed_[i - 1] = s = 1812433253U * (s ^ (s >> 30)) + i;
		}
	}
};

#endif