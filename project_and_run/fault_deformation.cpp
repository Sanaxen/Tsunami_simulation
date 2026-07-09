#define _USE_MATH_DEFINES 
#define NOMINMAX

#include "fault_deformation.h"
#include <stdarg.h>
#include <Windows.h>
#include "utf8_printf.hpp"
//#define M_PI       3.14159265358979323846

//#define USE_GEODESIC_H

#ifdef USE_GEODESIC_H
#include "geodesic.h"
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

#define AREA_OFFSET		(30)
#define absorbingZone (25+AREA_OFFSET)
#define attenuation  0.03

double fault_z_scale = 1.0;

static FILE* logFile = 0;
void log_printf(char* format, ...)
{
	va_list	argp;
	char pszBuf[ 4096];
	va_start(argp, format);
	vsnprintf( pszBuf, 4096, format, argp);
	va_end(argp);

	if ( logFile != NULL )
	{
		fprintf(logFile, "%s", pszBuf); fflush(logFile);
	}
	::printf("%s", pszBuf);
}

//ビットマップ(x,y)の位置にマークを付ける
void BitmpMarke(char* fname, char* out, int x, int y, int w)
{
	BitMap bmp;

	bmp.Read(fname);

	int xxx = x;
	int yyy = bmp.H() - y;
#pragma omp parallel for
	for ( int i = -w; i <= w; i++ )
	{
		for ( int j = -w; j <= w; j++ )
		{
			if ( yyy+i >= bmp.H()-1 || yyy+i < 0 ) continue;
			if ( xxx+j >= bmp.W()-1 || xxx+j < 0 ) continue;
			bmp.cell(yyy+i,xxx+j).r = 255;
			bmp.cell(yyy+i,xxx+j).g = 0;
			bmp.cell(yyy+i,xxx+j).b = 0;
		}
	}
	bmp.Write(out);
}


void BitmpLine(BitMap& bmp, int x, int y, int w, double dx, double dy, double length, double th)
{
	const double lx = length*cos(-th*PI/180.0)/dx;
	const double ly = length*sin(-th*PI/180.0)/dy;


	double dt = 1.0/300.0;
	for ( int k = 0; k < 300; k++ )
	{
		const double t = (double)k*dt;
		const int sx = (int)(x*(1.0-t) + (x + lx)*t);
		const int sy = (int)(y*(1.0-t) + (y + ly)*t);

		const int xxx = sx;
		const int yyy = bmp.H() - sy;
		for ( int i = -w; i <= w; i++ )
		{
			for ( int j = -w; j <= w; j++ )
			{
				if ( yyy+i >= bmp.H() ) continue;
				if ( xxx+j >= bmp.W() ) continue;
				bmp.cell(yyy+i,xxx+j).r = 255;
				bmp.cell(yyy+i,xxx+j).g = 0;
				bmp.cell(yyy+i,xxx+j).b = 0;
			}
		}
	}
}


//ビットマップ(x,y)の位置から指定角度th(deg)で長さlengthの線を引く
// dx,dyは格子サイズ
void BitmpLine(char* fname, char* out, int x, int y, int w, double dx, double dy, double length, double th)
{
	BitMap bmp;

	bmp.Read(fname);

	const double lx = length*cos(-th*PI/180.0)/dx;
	const double ly = length*sin(-th*PI/180.0)/dy;

#ifdef _OPENMP
	const int th_n =  omp_get_max_threads() ;
	BitMap* bmp_tmp = new BitMap[th_n + 1];
#else
	const int th_n =  1;
	BitMap* bmp_tmp = new BitMap[1];
#endif

#pragma omp parallel for schedule(static)
	for ( int j = 0; j < th_n; j++ )
	{
		bmp_tmp[j].Create(bmp.W(), bmp.H());
	}

	double dt = 1.0/300.0;
#pragma omp parallel for schedule(static)
	for ( int k = 0; k < 300; k++ )
	{
		const double t = (double)k*dt;
		const int sx = (int)(x*(1.0-t) + (x + lx)*t);
		const int sy = (int)(y*(1.0-t) + (y + ly)*t);

		const int xxx = sx;
		const int yyy = bmp.H() - sy;
		for ( int i = -w; i <= w; i++ )
		{
			for ( int j = -w; j <= w; j++ )
			{
				if ( yyy+i >= bmp.H() ) continue;
				if ( xxx+j >= bmp.W() ) continue;
#ifdef _OPENMP
				const int th_id = omp_get_thread_num();
#else
				const int th_id = 0;
#endif
				bmp_tmp[th_id].cell(yyy+i,xxx+j).r = 255;
				bmp_tmp[th_id].cell(yyy+i,xxx+j).g = 0;
				bmp_tmp[th_id].cell(yyy+i,xxx+j).b = 0;
			}
		}
	}

#pragma omp parallel for schedule(static)
	for ( int i = 0; i < bmp.H()*bmp.W(); i++ )
	{
		for ( int j = 0; j < th_n; j++ )
		{
			if (  bmp_tmp[j].GetImage()->data[i].r == 0 )
			{
				continue;
			}
			bmp.GetImage()->data[i].r = bmp_tmp[j].GetImage()->data[i].r;
		}
	}
	delete [] bmp_tmp;

	bmp.Write(out);
}



//画像の四隅の経度緯度(longitude,latitude)を指定して経度緯度(lo,la)位置のビットマップ座標を計算する
// longitude:横, latitude:縦（上から下）
// longitude[0],latitude[0]    longitude[1],latitude[1]
//  +------------------------+
//  |                        |
//  |                        |
//  |                        |
//  |                        |
//  +------------------------+
//longitude[2],latitude[2]    longitude[3],latitude[3]
void latitudeLongitude_to_Bitmap(int w, int h, double longitude[4], double latitude[4], double lo, double la, int& xx, int& yy)
{
	xx = (int)((w-1)*(lo - longitude[0])/(longitude[1] - longitude[0])+0.5);
	yy = (int)((h-1)*(la - latitude[0])/(latitude[2] - latitude[0])+0.5);
}


//画像の四隅のピクセル座標を指定して断層パラメータの線を引く
// longitude:横, latitude:縦（上から下）
// longitude[0],latitude[0]    longitude[1],latitude[1]
//  +------------------------+
//  |                        |
//  |                        |
//  |                        |
//  |                        |
//  +------------------------+
//longitude[2],latitude[2]    longitude[3],latitude[3]
//w,hは画像サイズ
//dx,dyは格子サイズ

void BitmpFaultLine(int w, int h, double longitude[4], double latitude[4], std::vector<FaultLine>& fault, double dx, double dy, char* fname, char* out)
{
	int xx, yy;
	const int line_size = 0;

	latitudeLongitude_to_Bitmap( w,  h, longitude, latitude, fault[0].lo, fault[0].la, xx, yy);
	BitmpLine(fname, out, xx, yy, line_size, dx, dy, fault[0].length, 90.0 - fault[0].dir);

	BitMap bmp;

	bmp.Read(fname);


#ifdef _OPENMP
	const int th_n =  omp_get_max_threads() ;
	BitMap* bmp_tmp = new BitMap[th_n + 1];
#else
	const int th_n =  1;
	BitMap* bmp_tmp = new BitMap[1];
#endif

#pragma omp parallel
	{
#pragma omp	for schedule(guided)
		for (int j = 0; j < th_n; j++)
		{
			bmp_tmp[j].Create(bmp.W(), bmp.H());
		}

#pragma omp	for schedule(guided)
		for (int i = 1; i < fault.size(); i++)
		{
#ifdef _OPENMP
			const int th_id = omp_get_thread_num();
#else
			const int th_id = 0;
#endif
			int xx, yy;

			latitudeLongitude_to_Bitmap(w, h, longitude, latitude, fault[i].lo, fault[i].la, xx, yy);
			BitmpLine(bmp_tmp[th_id], xx, yy, line_size, dx, dy, fault[i].length, 90.0 - fault[i].dir);
		}
#pragma omp  for schedule(guided)
		for (int j = 0; j < th_n; j++)
		{
			for (int i = 0; i < bmp.H()*bmp.W(); i++)
			{
				if (bmp_tmp[j].GetImage()->data[i].r == 0)
				{
					continue;
				}
				bmp.GetImage()->data[i].r = bmp_tmp[j].GetImage()->data[i].r;
			}
		}
	}
	delete [] bmp_tmp;

	bmp.Write(out);

}

//画像の四隅のピクセル座標を指定して断層変位コンタを画く
// longitude:横, latitude:縦（上から下）
// longitude[0],latitude[0]    longitude[1],latitude[1]
//  +------------------------+
//  |                        |
//  |                        |
//  |                        |
//  |                        |
//  +------------------------+
//longitude[2],latitude[2]    longitude[3],latitude[3]
//w,hは画像サイズ
//dx,dyは格子サイズ
void CalcFault(int w, int h, double longitude[4], double latitude[4], FaultLine& fault, double dx, double dy, double* depth_map, double* zval, double& uzmin, double& uzmax)
{
	doublereal du1 = fault.slip*cos(fault.rake*PI_180);
	doublereal du2 = fault.slip*sin(fault.rake*PI_180);

	//計算の原点(0,0)を求める
	int bx, by;
	latitudeLongitude_to_Bitmap( w,  h, longitude, latitude, fault.lo, fault.la, bx, by);

	doublereal depth_d = float_tt(0.0);

	if (depth_map)
	{
		depth_d = -depth_map[(h-by)*w + bx];
		//if (bx - 1 < 0 || bx+1 >= w || h - by + 1 >= h || h - by - 1 < 0)
		//{
		//	/* empty*/
		//}
		//else
		//{
		//	depth_d = -0.25*(depth_map[(h - by - 1)*w + bx] + depth_map[(h - by + 1)*w + bx] + depth_map[(h - by)*w + bx - 1] + depth_map[(h - by)*w + bx + 1]);
		//}
		if ( depth_d < 0.0 ) depth_d = 0.0;
		printf("depth_d %f\n", -depth_d);
	}

	double dlo = (longitude[1] - longitude[0])/(double)(w-1);
	double dla = (latitude[2] - latitude[0])/(double)(h-1);

	const int wh = w*h;

	doublereal* zz = new doublereal[wh];
	int* dupcalc = new int[h*w];
#pragma omp parallel for schedule(static)
	for (int i = 0; i < wh; i++)
	{
		dupcalc[i] = 0;
		zz[i] = float_tt(0.0);
	}

	const doublereal C = cos(-(-90.0 + fault.dir)*PI_180);
	const doublereal S = sin(-(-90.0 + fault.dir)*PI_180);
	const doublereal CC = C * C;
	const doublereal CS = C * S;
	const doublereal SS = S * S;

	const doublereal TNSN = tan(fault.dip*PI_180)*sin(fault.slip*PI_180);

#ifdef USE_CPPAMP
	concurrency::extent<1> e(wh);
	
	std::vector<doublereal> depth_map_;
	depth_map_.resize(wh);

	depth_map_[0] = FLT_MAX;
	if (depth_map)
	{
		for (int i = 0; i < wh; i++) depth_map_[i] = depth_map[i];
	}
	doublereal longitude_[4] = { longitude[0], longitude[1], longitude[2], longitude[3] };
	doublereal latitude_[4] = { latitude[0], latitude[1], latitude[2], latitude[3] };

	array_view<doublereal, 1> zz_f(e, zz);
	array_view<const doublereal, 1> depth_map_f(e, &depth_map_[0]);
	array_view<const doublereal, 1> longitude_f(4, longitude_);
	array_view<const doublereal, 1> latitude_f(4, latitude_);
	array_view<int, 1> dupcalc_(e, dupcalc);

	const int fault_horizontal_displacement = fault.horizontal_displacement;
	dupcalc_.discard_data();
	zz_f.discard_data();

	parallel_for_each(e, [=](index<1> idx) ACC_RESTRICTION
	{
		const int i = idx[0] / w;
		const int j = idx[0] % w;

		int xx = j, yy = h - i;

		//計算の原点(0,0)からの距離に換算する

		doublereal xxx = (doublereal)(j)*float_tt(dlo) + longitude_f[0];
		doublereal yyy = (doublereal)(i)*float_tt(dla) + latitude_f[0];

		doublereal x = distance_lambert_amp(float_tt(fault.la), float_tt(fault.lo), float_tt(fault.la), xxx);
		doublereal y = distance_lambert_amp(float_tt(fault.la), float_tt(fault.lo), yyy, float_tt(fault.lo));

		if (float_tt(fault.lo) > xxx) x *= float_tt(-1.0);
		if (float_tt(fault.la) < yyy) y *= float_tt(-1.0);
#if 10

		doublereal z = float_tt(0.0);
		doublereal depth = float_tt(fault.depth);
		doublereal dip = float_tt(fault.dip);
		doublereal al1 = float_tt(0.0);
		doublereal al2 = float_tt(fault.length);
		doublereal aw1 = float_tt(-fault.width);
		doublereal aw2 = float_tt(0.0);
		doublereal ux, uy, uz = float_tt(0.0);
		if (xx - 1 < 0 || xx + 1 >= w || yy + 1 >= h || yy - 1 < 0)
		{
			//continue;
		}
		else
		{
			//下端深さ
			//depth = depth + fault.width*TNSN/* + depth_d*/;

			//if (depth_map_f[0] != FLT_MAX)
			//{
			//	z = -depth_map_f[yy*w + xx] - depth_d;
			//	if (z >= float_tt(0.0)) z = float_tt(0.0);
			//}

			c012_t c012;
			int iret;
			dc3d_3(c012, C, S, CC, CS, SS,
				float_tt(-90.0) + float_tt(fault.dir), x, y, z, depth/*+depth_d*/, dip, al1, al2, aw1, aw2,
				du1, du2, float_tt(0.0), ux, uy, uz, iret);

			if (iret != 2)
			{
				//Tanioka and Satake (1995) 鉛直変位に加え，傾斜する海底面の水平変位に伴う水塊の運動を考慮
				doublereal horizontal_displacement = float_tt(0.0);
				if (fault_horizontal_displacement)
				{
					horizontal_displacement = ux*(depth_map_f[yy*w + xx - 1] - depth_map_f[yy*w + xx + 1])*0.5 / dx + uy*(depth_map_f[(yy - 1)*w + xx] - depth_map_f[(yy + 1)*w + xx])*0.5 / dy;
					if (i < absorbingZone || i > h - absorbingZone || j < absorbingZone || j > w - absorbingZone)
					{
						horizontal_displacement = float_tt(0.0);
					}
					horizontal_displacement *= float_tt(-1.0);
					if (ACC_MATH fabs(uz) < float_tt(0.01)) horizontal_displacement = float_tt(0.0);
				}

				if (!ACC_MATH isnan(uz) && !ACC_MATH isinf(uz))
				{
					uz *= float_tt(-1.0);
					uz = uz + horizontal_displacement;

					for (int i = -1; i <= 1; i++)
					{
						for (int j = -1; j <= 1; j++)
						{
							if (yy + i < 0 || yy + i >= h || xx + i < 0 || xx + i >= w)
							{
								continue;
							}
							zz_f[(yy + i)*w + (xx + j)] += uz;
							dupcalc_[(yy + i)*w + (xx + j)] += 1;
						}
					}
				}
			}
		}
#endif
	});
	zz_f.synchronize();
	dupcalc_.synchronize();
#else
	//計算領域全体の断層変位を計算する
#pragma omp parallel for  schedule(guided)
	for (int ii = 0; ii < wh; ii++)
	{
		const int i = ii / w;
		const int j = ii % w;

		int xx = j, yy = h - i;

		//計算の原点(0,0)からの距離に換算する
		double xxx = (double)(j)*dlo + longitude[0];
		double yyy = (double)(i)*dla + latitude[0];

		doublereal x = distance_lambert(fault.la, fault.lo, fault.la, xxx);
		doublereal y = distance_lambert(fault.la, fault.lo, yyy, fault.lo);
		if (fault.lo > xxx) x *= -1.0;
		if (fault.la < yyy) y *= -1.0;

		doublereal z = float_tt(0.0);
		doublereal depth = fault.depth;
		doublereal dip = fault.dip;
		doublereal al1 = float_tt(0.0);
		doublereal al2 = fault.length;
		doublereal aw1 = -fault.width;
		doublereal aw2 = float_tt(0.0);
		doublereal ux, uy, uz = float_tt(0.0);
		if (xx - 1 < 0 || xx + 1 >= w || yy + 1 >= h || yy - 1 < 0)
		{
			continue;
		}
		//下端深さ
		//depth = depth + fault.width*TNSN/* + depth_d*/;

		//if (depth_map)
		//{
		//	z = -depth_map[yy*w + xx] - depth_d;
		//	if ( z >= float_tt(0.0)) z = float_tt(0.0);
		//}

		c012_t c012;
		int iret;
		dc3d_3(c012, C, S, CC, CS, SS,
			float_tt(-90.0) + fault.dir, x, y, z, depth/*+depth_d*/, dip, al1, al2, aw1, aw2,
			du1, du2, float_tt(0.0), ux, uy, uz, iret);
		if (iret == 2)
		{
			//printf("error\n");
			continue;
		}
		//Tanioka and Satake (1995) 鉛直変位に加え，傾斜する海底面の水平変位に伴う水塊の運動を考慮
		doublereal horizontal_displacement = float_tt(0.0);
		if (fault.horizontal_displacement)
		{
			horizontal_displacement = ux*(depth_map[yy*w + xx - 1] - depth_map[yy*w + xx + 1])*0.5 / dx + uy*(depth_map[(yy - 1)*w + xx] - depth_map[(yy + 1)*w + xx])*0.5 / dy;
			if (i < absorbingZone || i > h - absorbingZone || j < absorbingZone || j > w - absorbingZone)
			{
				horizontal_displacement = float_tt(0.0);
			}
			horizontal_displacement *= float_tt(-1.0);
			if (fabs(uz) < float_tt(0.01)) horizontal_displacement = float_tt(0.0);
		}
#if _MSC_VER > 1700
		if (!isnan<doublereal>(uz) && !isinf<doublereal>(uz))
		{
			uz *= -1.0;
			uz = uz + horizontal_displacement;
			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					if (yy + i < 0 || yy + i >= h || xx + i < 0 || xx + i >= w)
					{
						continue;
					}
					zz[(yy + i)*w + (xx + j)] += uz;
					dupcalc[(yy + i)*w + (xx + j)] += 1;
				}
			}
		}
#else
		//if (!_isnan(uz) && !_isinf(uz))
		{
			uz *= -1.0;
			uz = uz + horizontal_displacement;
			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					if (yy + i < 0 || yy + i >= h || xx + i < 0 || xx + i >= w)
					{
						continue;
					}
					zz[(yy + i)*w + (xx + j)] += uz;
					dupcalc[(yy + i)*w + (xx + j)] += 1;
				}
			}
		}
#endif
	}
#endif

	double* zz2 = new double[wh];
#pragma omp parallel
	{
#pragma omp for schedule(guided)
		for (int ii = 0; ii < wh; ii++)
		{
			if (dupcalc[ii])
			{
				zz[ii] = zz[ii] / dupcalc[ii];
				zz2[ii] = zz[ii];
			}
		}
#pragma omp for  schedule(guided)
		for (int i = 1; i < h - 1; i++)
		{
			for (int j = 1; j < w - 1; j++)
			{
				zz2[i*w + j] = (zz[(i - 1)*w + j] + zz[(i + 1)*w + j] + zz[i*w + (j - 1)] + zz[i*w + (j + 1)])*0.25;
			}
		}
#pragma omp  for  schedule(guided)
		for (int i = 0; i < wh; i++) zz[i] = zz2[i];
	}
	delete[] zz2;

	for (int ii = 0; ii < wh; ii++)
	{
		const int i = ii / w;
		const int j = ii % w;

		//if ( dupcalc[i*w + j] == 0.0 )
		//{
		//	continue;
		//}
		//double uz = zz[i*w + j] / dupcalc[i*w + j];
		double uz = zz[i*w + j] ;
		zval[i*w + j] += uz* fault_z_scale;
		depth_map[i*w + j] += uz* fault_z_scale;
		if (uzmax < uz) uzmax = zval[i*w + j];
		if (uzmin > uz) uzmin = zval[i*w + j];
	}
	delete[] zz;
	delete[] dupcalc;

	log_printf("min %f max %f\n", uzmin, uzmax);
}

//画像の四隅のピクセル座標を指定して断層変位コンタを画く
// longitude:横, latitude:縦（上から下）
// longitude[0],latitude[0]    longitude[1],latitude[1]
//  +------------------------+
//  |                        |
//  |                        |
//  |                        |
//  |                        |
//  +------------------------+
//longitude[2],latitude[2]    longitude[3],latitude[3]
//w,hは画像サイズ
//dx,dyは格子サイズ
#define SQR(x)	((x)*(x))
void CalcImpact(Meteo* meteo, int w, int h, double longitude[4], double latitude[4], double lo, double la, double dx, double dy, double* depth_map, double* zval, double& uzmin, double& uzmax)
{
	//計算の原点(0,0)を求める
	int bx, by;
	latitudeLongitude_to_Bitmap( w,  h, longitude, latitude, lo, la, bx, by);

	double depth_d = 0.0;

	if (depth_map)
	{
		depth_d = -depth_map[(h-by)*w + bx];
		if ( depth_d < 0.0 ) depth_d = 0.0;
		printf("depth_d %f\n", -depth_d);
	}

	double dlo = (longitude[1] - longitude[0])/(double)(w-1);
	double dla = (latitude[2] - latitude[0])/(double)(h-1);

	double rr = 9999999999.0;
	for ( int i = 0; i < h; i++ )
	{
		for ( int j = 0; j < w; j++ )
		{
			int xx = j, yy = h-i;

			//計算の原点(0,0)からの距離に換算する
			double xxx = (double)j*dlo + longitude[0];
			double yyy = (double)i*dla + latitude[0];

			double x = distance_lambert(la, lo, la, xxx);
			double y = distance_lambert(la, lo, yyy, lo);
			if ( lo > xxx ) x *= -1.0;
			if ( la < yyy ) y *= -1.0;

			double r = distance_lambert(la, lo, yyy, xxx);
			double z = 0.0;
			
			if ( xx >= w || yy >= h || yy < 0)
			{
				continue;
			}

			double uz = -(meteo->depth+depth_d)*(1.0- SQR(r)/SQR(meteo->D/2.0));
			if ( uz  > meteo->h )
			{
				if ( rr > r ) rr = r;
			}
		}
	}
	printf("rr %f\n", rr);

	//計算領域全体の変位を計算する
	for ( int i = 0; i < h; i++ )
	{
		for ( int j = 0; j < w; j++ )
		{
			int xx = j, yy = h-i;

			//計算の原点(0,0)からの距離に換算する
			double xxx = (double)j*dlo + longitude[0];
			double yyy = (double)i*dla + latitude[0];

			double x = distance_lambert(la, lo, la, xxx);
			double y = distance_lambert(la, lo, yyy, lo);
			if ( lo > xxx ) x *= -1.0;
			if ( la < yyy ) y *= -1.0;

			double r = distance_lambert(la, lo, yyy, xxx);
			double z = 0.0;
			
			if ( xx >= w || yy >= h || yy < 0)
			{
				continue;
			}


			double uz = -(meteo->depth+depth_d)*(1.0- SQR(r)/SQR(meteo->D/2.0));
			if ( r  >= rr)
			{
				//uz =meteo->h*exp(-0.0000001*SQR(r-rr));
				zval[yy*w + xx] = 0.0;
			}else
			{
				zval[yy*w + xx] = uz;
			}

			if ( uzmax < uz ) uzmax = zval[yy*w + xx];
			if ( uzmin > uz ) uzmin = zval[yy*w + xx];
		}
	}
	log_printf("クレータ深さ:%f (%f)[km]\n", meteo->depth*0.001, (meteo->depth+depth_d)*0.001);
	log_printf("min %f max %f\n", uzmin, uzmax);
}

void BitmpFaultWrite(char* drive, char* dir, int w, int h, double* zval, double zmin, double zmax, char* fname, char* out, char* Initial_wave_data_bmp)
{
	BitMap fault_colormap_bmp1;
	BitMap fault_colormap_bmp2;
	BitMap bmp;
	bmp.Read(fname);

	BitMap bmp2;
	bmp2.Create(w, h);

	char colormap_fname[512];
	sprintf(colormap_fname, "%s%s/../colormap/%s", drive, dir, "fault_colormap1.bmp");
	fault_colormap_bmp1.Read(colormap_fname);

	sprintf(colormap_fname, "%s%s/../colormap/%s", drive, dir, "fault_colormap2.bmp");
	fault_colormap_bmp2.Read(colormap_fname);

	//printf("w%d h%d\n", fault_colormap_bmp.W(), fault_colormap_bmp.H());
	const double alp = 0.85;

	const int wh = w*h;
	//#pragma omp parallel for  schedule(dynamic, 10)
#pragma omp parallel for  schedule(guided)
	for ( int ii = 0; ii < w*h; ii++ )
	{
		const int i = ii / w;
		const int j = ii % w;

		const int xx = j, yy = h-i;
		if ( xx >= w || yy >= h || yy < 0)
		{
			continue;
		}

		double z = zval[yy*w + xx];

		//if ( fabs(z) < 0.001 ) continue;
		//if (z > 0.0 && z < 0.00001) z = 0.0;


		if ( z > 0.0 )
		{
			int x = fault_colormap_bmp1.W()/2;
			double t = (double)fault_colormap_bmp1.H()*z/5.0;

			int s = (int)t;
			if ( s < 0 ) s = 0;
			if ( s >= fault_colormap_bmp1.H() ) s = fault_colormap_bmp1.H()-1;

			t = (double)s/(double)fault_colormap_bmp1.H();
			double a = alp*(1.0 - exp(-t*3.0));
			//a = 0.45;
			bmp.cell(yy,xx).r = (int)(a*(double)fault_colormap_bmp1.cell(s,x).r + (double)bmp.cell(yy,xx).r*(1.0 - a));
			bmp.cell(yy,xx).g = (int)(a*(double)fault_colormap_bmp1.cell(s,x).g + (double)bmp.cell(yy,xx).g*(1.0 - a));
			bmp.cell(yy,xx).b = (int)(a*(double)fault_colormap_bmp1.cell(s,x).b + (double)bmp.cell(yy,xx).b*(1.0 - a));
		}else
		{
			int x = fault_colormap_bmp2.W()/2;
			double t = fault_colormap_bmp2.H()*fabs(z)/1.5;

			int s = (int)t;
			if ( s < 0 ) s = 0;
			if ( s >= fault_colormap_bmp2.H() ) s = fault_colormap_bmp2.H()-1;

			t = (double)s/(double)fault_colormap_bmp1.H();
			double a = alp*(1.0 - exp(-t*3.0));
			//a = 0.45;
			bmp.cell(yy,xx).r = (int)(a*(double)fault_colormap_bmp2.cell(s,x).r + (double)bmp.cell(yy,xx).r*(1.0 - a));
			bmp.cell(yy,xx).g = (int)(a*(double)fault_colormap_bmp2.cell(s,x).g + (double)bmp.cell(yy,xx).g*(1.0 - a));
			bmp.cell(yy,xx).b = (int)(a*(double)fault_colormap_bmp2.cell(s,x).b + (double)bmp.cell(yy,xx).b*(1.0 - a));
		}
	}
	bmp.Write(out);

	//#pragma omp parallel for  schedule(dynamic, 10)
#pragma omp parallel for  schedule(guided)
	for ( int ii = 0; ii < wh; ii++ )
	{
		const int i = ii / w;
		const int j = ii % w;

		const int xx = j, yy = h-i;
		if ( xx >= w || yy >= h || yy < 0)
		{
			continue;
		}

		double z = zval[yy*w + xx];

		if ( fabs(z) < 0.1 ) continue;
		if ( z >= 0.0 )
		{
			int s = (int)((255.0*z/zmax)+0.5);
			if ( s < 0 ) s = 0;
			else if ( s > 255 ) s = 255;
			bmp2.cell(yy,xx).r = s;
		}else
		{
			int s = (int)((255.0*z/zmin)+0.5);
			if ( s < 0 ) s = 0;
			else if ( s > 255 ) s = 255;
			bmp2.cell(yy,xx).b = s;
		}
	}
	bmp2.Write(Initial_wave_data_bmp);
}

/** * ２点間の直線距離を求める（Lambert-Andoyer）
* @param   float   lat1       始点緯度(十進度) 
* @param   float   lon1       始点経度(十進度) 
* @param   float   lat2       終点緯度(十進度) 
* @param   float   lon2       終点経度(十進度) 
* @return  float               距離（m） */
doublereal  distance_lambert(doublereal lat1, doublereal lon1, doublereal lat2, doublereal lon2)
{    
	lat1 = lat1*float_tt(PI_180);
	lat2 = lat2*float_tt(PI_180);
	lon1 = lon1*float_tt(PI_180);
	lon2 = lon2*float_tt(PI_180);

#ifdef USE_GEODESIC_H
	double alp1, alp2;
	double dist = solve_inverse_problem(lon1, lat1, lon2, lat2, alp1, alp2);
	return dist;
#endif

	// WGS84    
	doublereal A = float_tt(6378137.);   // 赤道半径    
	doublereal F = float_tt(1.0) / float_tt(298.257222101);     // 扁平率 扁平率 F = (A - B) / A
	doublereal B = A * (float_tt(1.0) - F);	// 極半径 
	
	doublereal BA = B / A;

	doublereal P1 = ACC_MATH atan(BA * ACC_MATH tan(lat1));
	doublereal P2 = ACC_MATH atan(BA * ACC_MATH tan(lat2));
	// Spherical Distance    
	doublereal th = ACC_MATH sin(P1)*ACC_MATH sin(P2) + ACC_MATH cos(P1)*ACC_MATH cos(P2)*ACC_MATH cos(lon1-lon2);
	if ( th < float_tt(-1.0) ) th = float_tt(-1.0);
	else if ( th > float_tt(1.0) ) th = float_tt(1.0);
	doublereal sd = ACC_MATH acos(th);
	// Lambert-Andoyer Correction    
	doublereal cos_sd = ACC_MATH cos(sd/ float_tt(2));
	doublereal sin_sd = ACC_MATH sin(sd/ float_tt(2));
	doublereal cc = cos_sd*cos_sd;
	doublereal ss = sin_sd*sin_sd;

#if 0
	if (ACC_MATH fabs(cc) < float_tt(1.0e-16) || ACC_MATH fabs(ss) < float_tt(1.0e-16) )
	{
		printf("Correction:%f -> %f\n", sd*180.0/M_PI, (sd+ 0.00000001)*180.0/M_PI);
		//printf("tan(lat1:%f):%f\n", lat1, tan(lat1));
		//printf("tan(lat2:%f):%f\n", lat2, tan(lat2));

		sd = ACC_MATH acos(th)+0.00000001;
		cos_sd = ACC_MATH cos(sd/2);
		sin_sd = ACC_MATH sin(sd/2);
		cc = cos_sd*cos_sd;
		ss = sin_sd*sin_sd;
	}
#endif
	//if ( fabs(cc) < 1.0e-16 || fabs(ss) < 1.0e-16 )
	//{
	//	printf("%f %f\n", cc, ss);
	//	abort();
	//}

	doublereal sinsd = ACC_MATH sin(sd);
	doublereal sin_P1 = ACC_MATH sin(P1);
	doublereal sin_P2 = ACC_MATH sin(P2);

	doublereal c;
	doublereal s;

	if (ACC_MATH fabs(cc) < 1.0e-10)
	{
		c = float_tt(0.0);
	}
	else
	{
		c = (sinsd - sd) * ACC_MATH pow(sin_P1 + sin_P2, float_tt(2.0)) / cc;
	}

	if (ACC_MATH fabs(ss) < 1.0e-10)
	{
		s = float_tt(0.0);
	}
	else
	{
		s = (sinsd + sd) * ACC_MATH pow(sin_P1 - sin_P2, float_tt(2.0)) / ss;
	}

	doublereal delta = F / float_tt(8.0) * (c - s);
	// Geodetic Distance    
	double distance = A * (sd + delta);
	//if (_isnan(distance) || distance < 0.0)
	//{
	//	printf("c %f s %f\n", c, s);
	//	printf("delta %f\n", delta);
	//	printf("distance %f\n", distance);
	//	abort();
	//}
#if 0
	double alp1, alp2;
	double dist = solve_inverse_problem(lon1, lat1, lon2, lat2, alp1, alp2);
	
	printf("%f | %f %f\n", fabs(distance - dist), distance, dist);
	return dist;
#endif
	return distance;
}

/** * ２点間の直線距離を求める（Lambert-Andoyer）
* @param   float   lat1       始点緯度(十進度)
* @param   float   lon1       始点経度(十進度)
* @param   float   lat2       終点緯度(十進度)
* @param   float   lon2       終点経度(十進度)
* @return  float               距離（m） */
doublereal  distance_lambert_amp(doublereal lat1, doublereal lon1, doublereal lat2, doublereal lon2) ACC_RESTRICTION
{
	lat1 = lat1*float_tt(PI_180);
	lat2 = lat2*float_tt(PI_180);
	lon1 = lon1*float_tt(PI_180);
	lon2 = lon2*float_tt(PI_180);
#if 10
#ifdef USE_GEODESIC_H
	double alp1, alp2;
	double dist = solve_inverse_problem(lon1, lat1, lon2, lat2, alp1, alp2);
	return dist;
#endif

	// WGS84    
	doublereal A = float_tt(6378137.);   // 赤道半径    
	doublereal F = float_tt(1.0) / float_tt(298.257222101);     // 扁平率 扁平率 F = (A - B) / A
	doublereal B = A * (float_tt(1.0) - F);	// 極半径 

	doublereal BA = B / A;

	doublereal P1 = ACC_MATH atan(BA * ACC_MATH tan(lat1));
	doublereal P2 = ACC_MATH atan(BA * ACC_MATH tan(lat2));
	// Spherical Distance    
	doublereal th = ACC_MATH sin(P1)*ACC_MATH sin(P2) + ACC_MATH cos(P1)*ACC_MATH cos(P2)*ACC_MATH cos(lon1 - lon2);
	if (th < float_tt(-1.0)) th = float_tt(-1.0);
	else if (th > float_tt(1.0)) th = float_tt(1.0);
	doublereal sd = ACC_MATH acos(th);
	// Lambert-Andoyer Correction    
	doublereal cos_sd = ACC_MATH cos(sd / float_tt(2));
	doublereal sin_sd = ACC_MATH sin(sd / float_tt(2));
	doublereal cc = cos_sd*cos_sd;
	doublereal ss = sin_sd*sin_sd;
#if 10
	if (ACC_MATH fabs(cc) < float_tt(1.0e-9) || ACC_MATH fabs(ss) < float_tt(1.0e-9))
	{
		return FLT_MIN;
		//printf("Correction:%f -> %f\n", sd*180.0 / M_PI, (sd + 0.000001)*180.0 / M_PI);
		//printf("tan(lat1:%f):%f\n", lat1, tan(lat1));
		//printf("tan(lat2:%f):%f\n", lat2, tan(lat2));

		sd = ACC_MATH acos(th) + float_tt(0.000001);
		cos_sd = ACC_MATH cos(sd / 2);
		sin_sd = ACC_MATH sin(sd / 2);
		cc = cos_sd*cos_sd;
		ss = sin_sd*sin_sd;
	}
#endif
	//if ( fabs(cc) < 1.0e-16 || fabs(ss) < 1.0e-16 )
	//{
	//	printf("%f %f\n", cc, ss);
	//	abort();
	//}

	doublereal sinsd = ACC_MATH sin(sd);
	doublereal sin_P1 = ACC_MATH sin(P1);
	doublereal sin_P2 = ACC_MATH sin(P2);

	doublereal c;
	doublereal s;

	if (ACC_MATH fabs(cc) < 1.0e-7)
	{
		c = float_tt(0.0);
	}
	else
	{
		c = (sinsd - sd) * ACC_MATH pow(sin_P1 + sin_P2, double_tt(2.0)) / cc;
	}

	if (ACC_MATH fabs(ss) < 1.0e-7)
	{
		s = float_tt(0.0);
	}
	else
	{
		s = (sinsd + sd) * ACC_MATH pow(sin_P1 - sin_P2, double_tt(2.0)) / ss;
	}

	doublereal delta = F / float_tt(8.0) * (c - s);
	// Geodetic Distance    
	double distance = A * (sd + delta);
	//if (_isnan(distance) || distance < 0.0)
	//{
	//	printf("c %f s %f\n", c, s);
	//	printf("delta %f\n", delta);
	//	printf("distance %f\n", distance);
	//	abort();
	//}
#if 0
	double alp1, alp2;
	double dist = solve_inverse_problem(lon1, lat1, lon2, lat2, alp1, alp2);

	printf("%f | %f %f\n", fabs(distance - dist), distance, dist);
	return dist;
#endif
	return distance;
#endif
	return float_tt(0.0);
}

double absorbingZoneBounray(int w, int h, int i, int j, int flg)
{
	double dw1 = 1.0;
	double dw2 = 1.0;
	double dw = 1.0;

	const int absorbingZone_ = absorbingZone;
	const int IX = w;
	const int JY = h;

	//吸収境界条件 Crejan(1985)
	if ( i < absorbingZone || i > h-absorbingZone || j < absorbingZone || j > w-absorbingZone )
	{
		int yy;
		int xx;
		int d;

		if ( i < absorbingZone_ ) yy = (absorbingZone_ - i);
		else if ( i > JY-absorbingZone_ ) yy =  JY-absorbingZone_ - i;
		if ( j < absorbingZone_ ) xx = (absorbingZone_ - j);
		else if ( j > IX-absorbingZone_ ) xx =  IX-absorbingZone_ - j;

		if ( j < absorbingZone_ || j > IX-absorbingZone_ )
		{
			d = xx*xx;
			dw1 = exp(-attenuation*(double)d);
			if ( flg == 0 )
			{
				return dw1;
			}
		}

		if ( i < absorbingZone_ || i > JY-absorbingZone_ )
		{
			d = yy*yy;
			dw2 = exp(-attenuation*(double)d);
			if ( flg == 1 )
			{
				return dw2;
			}
		}

		if ( dw1 < dw2 ) return dw1;
		return dw2;
	}
	return dw;
}

double Attenuation(double d)
{
	if ( fabs(d) < 1.0e-14 ) return 0.0;
	
	if (d < 0.0)
	{
		return exp(-0.002*pow(1.0 / fabs(d), 3.0));
	}
	return exp(-0.05*pow(1.0 / fabs(d), 3.0));
}

void sommthing(double* zval, int w, int h, double zmin= 0.0, double zmax=0.0);
static double _uzmin, _uzmax;
double* CalcFaultListWrite(char* drive, char* dir, int w, int h, double longitude[4], double latitude[4], std::vector<FaultLine>& fault, Meteo* meteo, double dx, double dy, double* depth_map, char* fname, char* out, char* Initial_wave_data_bmp, int t_flag, char* drive2, char* dir2)
{
	double uzmin, uzmax;
	double uzmin_all, uzmax_all;
	double zmin = -0.5;
	double zmax = 2.0;

	uzmin = uzmin_all = 99999999999.0;
	uzmax = uzmax_all = -9999999999.0;
	double* zval_all = NULL;
	double* zval = new double[w*h];
	memset(zval, '\0', sizeof(double)*w*h);

	char Initial_wave_data3[512];
	char FaultParameters[512];
	char Initial_wave_data_bmp3[512];

	if ( t_flag )
	{
		zval_all = new double[w*h];
		memset(zval_all, '\0', sizeof(double)*w*h);

		{
			char fname[256];
			HANDLE hFind;
			WIN32_FIND_DATAA win32fd;

			sprintf(fname, "%s%sInitial_wave_data3_*.*", drive2, dir2);
			hFind = FindFirstFileA(fname, &win32fd);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				do {
					if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						//printf("%s (DIR)\n", win32fd.cFileName);
					}
					else {
						printf("delete %s\n", win32fd.cFileName);
						_unlink(win32fd.cFileName);
					}
				} while (FindNextFileA(hFind, &win32fd));
			}
			FindClose(hFind);

			sprintf(fname, "%s%sFaultParameters_*.bmp", drive2, dir2);
			hFind = FindFirstFileA(fname, &win32fd);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				do {
					if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						//printf("%s (DIR)\n", win32fd.cFileName);
					}
					else {
						printf("delete %s\n", win32fd.cFileName);
						_unlink(win32fd.cFileName);
					}
				} while (FindNextFileA(hFind, &win32fd));
			}
			FindClose(hFind);
		}
	}

	if ( meteo )
	{
		CalcImpact( meteo, w, h, longitude, latitude, meteo->lo, meteo->la, dx, dy, depth_map, zval, uzmin, uzmax);
		double* zval_new = new double [h*w];

		for ( int ii = 0; ii < h; ii++ )
		{
			for ( int jj = 0; jj < w; jj++ )
			{
				//if ( ii < absorbingZone || ii > h-absorbingZone || jj < absorbingZone || jj > w-absorbingZone )
				//{
				//	zval[ii*w+jj] = 0.0;
				//}
				double dw = absorbingZoneBounray( w,  h,  ii,  jj, -1);
				zval[ii*w+jj] *= dw;
				if ( fabs(zval[ii*w+jj]) < 0.001 ) zval_new[ii*w+jj] = 0.0;
			}
		}
		BitmpFaultWrite(drive,dir, w, h, zval, uzmin, uzmax, fname,  out, Initial_wave_data_bmp);
		_uzmin = uzmin;
		_uzmax = uzmax;
		return zval;
	}

	int amp_gpu_device = -1;
#ifdef USE_CPPAMP
	{
		char* env = getenv("AMP_GPU_DEVICE_NO");
		if (env)
		{
			log_printf("set AMP_GPU_DEVICE_NO=%d\n", amp_gpu_device);
			amp_gpu_device = atoi(env);
		}
		else
		{
			log_printf("default AMP_GPU_DEVICE_NO=%d\n", amp_gpu_device);
		}
		std::vector<accelerator> accs = accelerator::get_all();
		for (int i = 0; i < accs.size(); i++)
		{
			log_printf("[%d]description:%s\n", i, WStringToString(accs[i].description).c_str());
			log_printf("[%d]device_path:%s\n", i, WStringToString(accs[i].device_path).c_str());
			log_printf("[%d]dedicated_memory:%d\n", i, accs[i].dedicated_memory);
			log_printf("[%d]supports_double_precision:%s\n\n",
				i, accs[i].supports_double_precision ? "true" : "false");
		}

#ifdef DC3D_ORG_TYPE
		for (int i = 0; i < accs.size(); i++)
		{
			if (accs[i].supports_double_precision)
			{
				if (amp_gpu_device == i)
				{
					break;
				}
				else
				{
					log_printf("H/W double_precision->false!!\n");
					amp_gpu_device = 0;
					exit(1);
				}
			}
		}
#endif
#ifdef DC3D_DOUBLE_TYPE
		for (int i = 0; i < accs.size(); i++)
		{
			if (accs[i].supports_double_precision)
			{
				if (amp_gpu_device == i)
				{
					break;
				}
				else
				{
					log_printf("H/W double_precision->false!!\n");
					amp_gpu_device = 0;
					exit(1);
				}
			}
		}
#endif

		if (amp_gpu_device >= 0 && amp_gpu_device < accs.size())
		{
			accelerator::set_default(accs[amp_gpu_device].device_path);
		}
		accelerator default_acc;
		log_printf("default_acc.description:%s\n", WStringToString(default_acc.description).c_str());
		log_printf("default_acc.device_path:%s\n", WStringToString(default_acc.device_path).c_str());
		log_printf("default_acc.dedicated_memory:%d\n", default_acc.dedicated_memory);
		log_printf("default_acc.supports_double_precision:%s\n\n",
			default_acc.supports_double_precision ? "true" : "false");
	}
#endif

	const int sz = fault.size();

	const int wh = w*h;
	for ( int i = 0; i < sz; i++ )
	{
		log_printf("[%d/%d]\n", i+1, sz);
		CalcFault( w,  h,  longitude,  latitude,  fault[i], dx, dy, depth_map, zval, uzmin, uzmax);

		//#pragma omp parallel for  schedule(dynamic, 10)
#pragma omp parallel for  schedule(guided)
		for (int k = 0; k < wh; k++)
		{			
			const int ii = k / w;
			const int jj = k % w;

			zval[ii * w + jj] = zval[ii * w + jj];

			double dw = absorbingZoneBounray(w, h, ii, jj, -1);
			zval[ii*w + jj] *= dw;
			//zval[ii*w+jj] *= Attenuation(zval[ii*w+jj]);

			if (dw < 1.0)
			{
				if (fabs(zval[ii*w + jj]) < 0.00001) zval[ii*w + jj] = 0.0;
			}
		}
		if ( i+1 <= sz-1 )
		{
			if (fabs(fault[i + 1].t - fault[i].t) < 0.01)
			{
				continue;
			}
		}



		if ( t_flag )
		{
			sommthing(zval, w, h, uzmin, uzmax);

			log_printf("min:%f max:%f\n", uzmin, uzmax);
#pragma omp parallel for
			for ( int ii = 0; ii < wh; ii++ )
			{
				zval_all[ii] += zval[ii];
			}



			sprintf(Initial_wave_data3, "%s%sInitial_wave_data3_%.3f.csv", drive2, dir2, fault[i].t);
			if (fault[i].s > 0.0)
			{
				sprintf(Initial_wave_data3, "%s%sInitial_wave_data3_%.3f_%.3f.csv", drive2, dir2, fault[i].t, fault[i].s);
			}
			WriteCsv(Initial_wave_data3, w, h, zval, 1.0);
			

			sprintf(Initial_wave_data_bmp3, "%s%sInitial_wave_data3_%.3f.bmp", drive2, dir2, fault[i].t);
			if (fault[i].s > 0.0)
			{
				sprintf(Initial_wave_data_bmp3, "%s%sInitial_wave_data3_%.3f_%.3f.bmp", drive2, dir2, fault[i].t, fault[i].s);
			}
			sprintf(FaultParameters, "%s%sFaultParameters_%.3f.bmp", drive2, dir2, fault[i].t);
			if (fault[i].s > 0.0)
			{
				sprintf(FaultParameters, "%s%sFaultParameters_%.3f_%.3f.bmp", drive2, dir2, fault[i].t, fault[i].s);
			}
			//BitmpFaultWrite(drive, dir, w, h, zval, uzmin, uzmax, fname, FaultParameters, Initial_wave_data_bmp3);
			BitmpFaultWrite(drive, dir, w, h, zval_all, uzmin, uzmax, fname, FaultParameters, Initial_wave_data_bmp3);
			
			memset(zval, '\0', sizeof(double)*w*h);
			if ( uzmin < uzmin_all ) uzmin_all = uzmin;
			if ( uzmax > uzmax_all ) uzmax_all = uzmax;
			uzmin = 99999999999.0;
			uzmax = -9999999999.0;
		}
	}

	if ( !t_flag )
	{
		sommthing(zval, w, h );
	}


	if ( !t_flag ) log_printf("min:%f max:%f\n", uzmin, uzmax);
	else log_printf("min:%f max:%f\n", uzmin_all, uzmax_all);

	if ( t_flag )
	{
		BitmpFaultWrite(drive,dir, w, h, zval_all, uzmin_all, uzmax_all, fname,  out, Initial_wave_data_bmp);
		delete [] zval;
		zval = zval_all;
	}else
	{
		BitmpFaultWrite(drive,dir, w, h, zval, uzmin, uzmax, fname,  out, Initial_wave_data_bmp);
	}
	return zval;
}

void sommthing(double* zval, int w, int h, double zmin, double zmax)
{
	const int wh = w*h;

	for ( int k = 0; k < 3; k++ )
	{
		double* zval_new = new double [h*w];
		memcpy(zval_new, zval, w*h*sizeof(double));

		if (zmin != zmax)
		{
			const double d = std::max(fabs(zmin), fabs(zmax))*0.01;
			//#pragma omp parallel for  schedule(dynamic, 10)
#pragma omp parallel for  schedule(guided)
			for (int ii = 0; ii < wh; ii++)
			{
				const int i = ii / w;
				const int j = ii % w;
				if (i - 1 < 0 || i + 1 >= h || j - 1 < 0 || j + 1 >= w)
				{
					continue;
				}
				if (fabs(zval[i*w + j]) < d/(double)(k+1))
				{
					zval_new[i*w + j] = 0.25*(zval[(i - 1)*w + j] + zval[(i + 1)*w + j] + zval[i*w + (j - 1)] + zval[i*w + (j + 1)]);
				}
			}
		}
		else
		{
#pragma omp parallel for
			for (int ii = 0; ii < wh; ii++)
			{
				const int i = ii / w;
				const int j = ii % w;
				if (i - 1 < 0 || i + 1 >= h || j - 1 < 0 || j + 1 >= w)
				{
					continue;
				}
				if (fabs(zval[i*w + j]) < 0.4)
				{
					zval_new[i*w + j] = 0.25*(zval[(i - 1)*w + j] + zval[(i + 1)*w + j] + zval[i*w + (j - 1)] + zval[i*w + (j + 1)]);
				}
			}
		}
		memcpy(zval, zval_new, w*h*sizeof(double));
		delete[] zval_new;
	}

	double s = 1.0;
	if (zmin != zmax)
	{
		//小断層で重ねわせする場合は個々の小断層の起伏は小さいため
		//そのまま減衰させると起伏がゼロになってしまって重ね合わせても
		//起伏無しになってしまう。
		const double d = std::max(fabs(zmin), fabs(zmax));
		if (d < 1.0)
		{
			s = 1.0 / d;
		}
	}
#pragma omp parallel for
	for (int ii = 0; ii < wh; ii++)
	{
		const int i = ii / w;
		const int j = ii % w;
		double dw = absorbingZoneBounray(w, h, i, j, -1);
		zval[i*w + j] *= dw*Attenuation(zval[i*w + j]*s);
	}
}

int fault_deformation( char* parameterFile)
{
	char drive[_MAX_DRIVE];	// ドライブ名
	char dir[_MAX_DIR];		// ディレクトリ名
	char fname[_MAX_FNAME];	// ファイル名
	char ext[_MAX_EXT];		// 拡張子

	_splitpath( parameterFile, drive, dir, fname, ext );

	cout << "Drive=" << drive << endl;
	cout << "Dir  =" << dir   << endl;
	cout << "Fname=" << fname << endl;
	cout << "Ext  =" << ext   << endl;

	char thisModuleFileName[_MAX_PATH];
	//実行中のプロセスのフルパス名を取得する
	GetModuleFileNameA(NULL, thisModuleFileName, sizeof(thisModuleFileName));

	char exe_drive[_MAX_DRIVE];	// ドライブ名
	char exe_dir[_MAX_DIR];		// ディレクトリ名
	char exe_fname[_MAX_FNAME];	// ファイル名
	char exe_ext[_MAX_EXT];		// 拡張子

	_splitpath( thisModuleFileName, exe_drive, exe_dir, exe_fname, exe_ext );

	bool set_latitude = false;
	bool set_longitude = false;
	double latitude[4];
	double longitude[4];
	double dx, dy;
	std::vector<FaultLine> fault;

	Meteo meteo;
	int impact = 0;

	FILE* fp = fopen( parameterFile, "r");
	if ( fp == NULL )
	{
		printf("file open error.\n");
		return -1;
	}

	char IDname[256];
	int horizontal_displacement = 0;

	int t_flag = 0;
	char buf[256];

	if (getenv("FAULT_Z_SCALE"))
	{
		//fault_z_scale = atof(getenv("FAULT_Z_SCALE"));
		//if (fault_z_scale <= 0) fault_z_scale = 1.0;
	}
	if (getenv("XY_DISPLACEMENT"))
	{
		horizontal_displacement = atoi(getenv("XY_DISPLACEMENT"));
	}
	while( fgets(buf, 256, fp ) != NULL )
	{
		printf("=>%s\n", buf);
		//if (strncmp(buf, "FAULT_Z_SCALE", 13) == 0) printf("@@@\n");

		if ( strcmp(buf, "ID\n") == 0 )
		{
			fgets(IDname, 256, fp);
			IDname[strlen(IDname)-1] = '\0';
			{
				FILE* fp = fopen(IDname, "w");
				if ( fp == NULL )
				{
					printf("ERROR:IDで利用できない文字が使われています");
					strcpy(IDname, "untitled");
					printf("=>%sを利用します\n", IDname);
				}else
				{
					fclose(fp);
					remove(IDname);
				}
				char logfile[512];
				sprintf(logfile, "%s%s%s_fault_deformation.log", drive, dir, IDname);
				logFile = fopen(logfile, "w");
			}
			continue;
		}

		if ( strcmp(buf, "DX\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &dx);
			continue;
		}
		if ( strcmp(buf, "DY\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &dy);
			continue;
		}
		if ( strcmp(buf, "LAT\n") == 0 )
		{
			set_latitude = true;
			printf("緯度\n");
			double h,m,s;
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); latitude[0] = (h+m/60.0 + s/3600.0);
			printf("北東:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, latitude[0]);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); latitude[1] = (h+m/60.0 + s/3600.0);
			printf("北西:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, latitude[1]);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); latitude[2] = (h+m/60.0 + s/3600.0);
			printf("南東:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, latitude[2]);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); latitude[3] = (h+m/60.0 + s/3600.0);
			printf("南西:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, latitude[3]);
			continue;
		}
		if ( strcmp(buf, "LON\n") == 0 )
		{
			set_longitude = true;
			printf("経度\n");
			double h,m,s;
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); longitude[0] = (h+m/60.0 + s/3600.0);
			printf("北東:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, longitude[0]);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); longitude[1] = (h+m/60.0 + s/3600.0);
			printf("北西:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, longitude[1]);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); longitude[2] = (h+m/60.0 + s/3600.0);
			printf("南東:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, longitude[2]);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); longitude[3] = (h+m/60.0 + s/3600.0);
			printf("南西:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, longitude[3]);
			continue;
		}
		if ( !impact && strncmp(buf, "FAULT_PARAMETERS", 16) == 0 )
		{
			log_printf("\n■断層パラメータ[%03d]\n", fault.size()+1);
			FaultLine f;

			f.t = 0.0;
			f.s = -1.0 - fault.size();

			if ( strncmp(buf, "FAULT_PARAMETERS T", 18) == 0 )
			{
				int r = sscanf(buf, "FAULT_PARAMETERS T%lf S%lf", &f.t, &f.s);
				if (r != 2)
				{
					sscanf(buf, "FAULT_PARAMETERS T%lf", &f.t);
					f.s = -1.0 - fault.size();
				}
				if (f.t < 0.0) f.t = 0.0;
				t_flag = 1;
			}
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &f.lo);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &f.la);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &f.length);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &f.dir);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &f.width);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &f.depth);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &f.dip);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &f.rake);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &f.slip);

			f.a = f.length*f.width;
			f.M0 = MU*f.slip*f.a;
			f.Mw = (log(f.M0)/log(10.0)-9.1)/1.5;

			f.horizontal_displacement = horizontal_displacement;
			fault.push_back(f);

			log_printf("時刻      :%8.3f(sec)\n", f.t);
			if ( f.s > 0.0 ) log_printf("破壊時間  :%8.3f(sec)\n", f.s);
			log_printf("経度      :%8.3f(deg)\n", f.lo);
			log_printf("緯度      :%8.3f(deg)\n", f.la);
			log_printf("断層長さ  :%8.3f(km)\n", f.length/1000.0);
			log_printf("走向      :%8.3f(deg)\n", f.dir);
			log_printf("幅        :%8.3f(km)\n", f.width/1000.0);
			log_printf("深さ      :%8.3f(km)\n", f.depth/1000.0);
			log_printf("傾斜角    :%8.3f(deg)\n", f.dip);
			log_printf("すべり角  :%8.3f(deg)\n", f.rake);
			log_printf("すべり量  :%8.3f(m)\n", f.slip);
			log_printf("断層面積: :%f[km^2]\n", f.a/1000000.0);
			log_printf("地震モーメントM0         :%.3g[N・m]\n", f.M0);
			log_printf("モーメントマグニチュードMw:%.2f\n", f.Mw);
			log_printf("水平方向効果・考慮:%d\n", f.horizontal_displacement);
			log_printf("--------------------------------------\n");
			continue;
		}

		if ( strcmp(buf, "IMPACT\n") == 0 )
		{
			fgets(buf, 256, fp);
			sscanf(buf, "%d", &impact);
			continue;
		}
		if ( strncmp(buf, "METEORITE_PARAMETER", 19) == 0 )
		{
			log_printf("\n■隕石パラメータ]\n");
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &meteo.lo);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &meteo.la);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &meteo.D);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &meteo.depth);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &meteo.D0);
			fgets(buf, 256, fp);
			sscanf(buf, "%lf", &meteo.h);
			log_printf("経度      :%8.3f(deg)\n", meteo.lo);
			log_printf("緯度      :%8.3f(deg)\n", meteo.la);
			log_printf("クレータ直径  :%8.3f(km)\n", meteo.D/1000.0);
			log_printf("クレータ深さ  :%8.3f(m)\n", meteo.depth);
			log_printf("初期波の立ち上がり    :%8.3f(km)\n", meteo.D0/1000.0);
			log_printf("初期波高さ    :%8.3f(m)\n", meteo.h);
			log_printf("--------------------------------------\n");
			continue;
		}
		if (strncmp(buf, "XY_DISPLACEMENT", 15) == 0)
		{
			fgets(buf, 256, fp);
			horizontal_displacement = atoi(buf);
			continue;
		}

		//fault_z_scale
		if (0)
		{
			if (strncmp(buf, "FAULT_Z_SCALE", 13) == 0)
			{
				fgets(buf, 256, fp);
				fault_z_scale = atof(buf);
				if (fault_z_scale <= 0) fault_z_scale = 1.0;
				continue;
			}
		}
	}
	fclose(fp);



	if (!set_latitude || !set_longitude)
	{
		if (logFile) fclose(logFile);
		return 1;
	}

	//四隅の世界測地座標（経度・緯度）
	double *x, *y;

	x = longitude;
	y = latitude;

	char filename[512];
	int h, w;
	sprintf(filename, "%s%swater_depth_data3.csv", drive, dir);

	double* map = ReadCsv(filename, w, h);
	
	if ( map == NULL )
	{
		sprintf(filename, "%s%swater_depth_data2.csv", drive, dir);
		map = ReadCsv(filename, w, h);
		if ( map == NULL )
		{
			sprintf(filename, "%s%swater_depth_data.csv", drive, dir);
			map = ReadCsv(filename, w, h);
		}
	}

	if ( map == NULL )
	{
		printf("Error: water_depth_data not found\n");
	}else
	{
		double* org_map = new double[w*h];

		for ( int k = 0; k < 3; k++ )
		{
			memcpy(org_map, map, sizeof(double)*w*h);
#pragma omp parallel for schedule(static)
			for ( int i = 1; i < h-1; i++ )
			{
				for ( int j = 1; j < w-1; j++ )
				{
					map[i*w + j] = (org_map[(i-1)*w+j]+org_map[(i+1)*w+j]+org_map[i*w+j-1]+org_map[i*w+j+1])*0.25;
				}
			}
		}
		delete [] org_map;
	}

	printf("w %d h %d\n", w, h);

	// p:横, q:縦（上から下）
	// p[0],q[0] ----------- p[1],q[1]
	//  +                        +
	//  |                        |
	//  |                        |
	//  |                        |
	//  |                        |
	// p[2],q[2] ----------- p[3],q[3]
	double xdist1 = distance_lambert(y[0], x[0], y[1], x[1]);
	double xdist2 = distance_lambert(y[2], x[2], y[3], x[3]);
	double ydist1 = distance_lambert(y[0], x[0], y[2], x[2]);
	double ydist2 = distance_lambert(y[1], x[1], y[3], x[3]);

	log_printf("X %f %f\n", xdist1, xdist2);
	log_printf("Y %f %f\n", ydist1, ydist2);

	log_printf("input DX %f ==> calc DX N:%f S:%f AV:%f\n", dx, xdist1/(double)w, xdist2/(double)w, 0.5*(xdist1+xdist2)/(double)w);
	log_printf("input DY %f ==> calc DY N:%f S:%f AV:%f\n", dy, ydist1/(double)h, ydist2/(double)h, 0.5*(ydist1+ydist2)/(double)h);

	{
		FILE* fp = fopen("dx_distance.txt", "w");
		if (fp)
		{
			double dt = 1.0 / (double)(h - 1);
			for (int i = h - 1; i >= 0; i--)
			{
				double t = (double)i*dt;
				double yy = y[0] * (1.0 - t) + y[2] * t;
				double d = distance_lambert(yy, x[0], yy, x[1]);
				fprintf(fp, "%f\n", d / w);
			}
			printf("h:%d\n", h);
			fclose(fp);
		}
		else
		{
			printf("dx_distance.txt [open error!]\n");
			return -1;
		}
	}

	if ( !impact && fault.size() == 0 )
	{
		if (logFile) fclose(logFile);
		return 1;
	}

	char topography_data_mask[512];
	char faultLines[512];
	char FaultParameters[512];
	char Initial_wave_data[512];
	char Initial_wave_data_bmp[512];

	sprintf(topography_data_mask, "%s%stopography_data_mask.bmp", drive, dir);
	sprintf(faultLines, "%s%sfaultLines.bmp", drive, dir);
	sprintf(FaultParameters, "%s%sFaultParameters.bmp", drive, dir);
	sprintf(Initial_wave_data, "%s%sInitial_wave_data2.csv", drive, dir);
	sprintf(Initial_wave_data_bmp, "%s%sInitial_wave_data2.bmp", drive, dir);

	Meteo* meteo_p = NULL;
	if ( !impact )
	{
		log_printf("[calculate deformation due to a fault model]\nMansinha and Smylie(1971)/ Okada [1985]\n");
		BitmpFaultLine( w,  h,  longitude, latitude, fault,  dx,  dy, topography_data_mask, faultLines);
	}else
	{
		meteo_p = &meteo;
	}
	
	if ( impact ) goto END;

	printf("pre sort\n");
	for ( int i = 0; i < fault.size(); i++ )
	{
		printf("[%d]t:%f\n", i, fault[i].t);
	}
	printf("post sort\n");
	std::sort(fault.begin(), fault.end());
	for ( int i = 0; i < fault.size(); i++ )
	{
		printf("[%d]t:%f\n", i, fault[i].t);
	}
	if ( fault[0].t != 0.0 )
	{
		printf("fault model ERROR:start time != 0.0\n");
		if (logFile) fclose(logFile);
		return -1;
	}

	double a = 0.0;
	double d = 0.0;
	for ( int i = 0; i < fault.size(); i++ )
	{
		a += fault[i].a;
		d += fault[i].slip;
	}

	double dd = d/(double)fault.size();

	double M0 = MU*dd*a;
	double Mw = (log(M0)/log(10.0)-9.1)/1.5;

	log_printf("※断層総面積:%f[km^2] 平均すべり量:%f[m]\n", a/1000000.0, dd);
	log_printf("※地震モーメントM0 %.2g[N・m]\n", M0);
	log_printf("※モーメントマグニチュードMw:%.2f\n", Mw);

END:	;

	double* zval = CalcFaultListWrite( exe_drive, exe_dir, w,  h, longitude, latitude, fault, meteo_p, dx, dy,  map, topography_data_mask, FaultParameters, Initial_wave_data_bmp,t_flag, drive, dir);
	WriteCsv(Initial_wave_data, w, h, zval, 1.0);


	if ( impact )
	{
		for ( int i = 0; i < h; i++ )
		{
			for ( int j = 0; j < w; j++ )
			{
				if ( i < absorbingZone || i > h-absorbingZone || j < absorbingZone || j > w-absorbingZone )
				{
					zval[i*w+j] = 0.0;
				}
			}
		}
		double* zval_cur = new double [h*w];
		memcpy(zval_cur, zval, w*h*sizeof(double));

		//衝突時の海水の凹みを生成
		sprintf(fname, "%s%sInitial_wave_data3_%d.csv", drive, dir, 0);
		WriteCsv(fname, w, h, zval, 1.0);

		//立ち上がり部分（水しぶきの上がった部分）が落ちて元の海水面に戻る動作
		for ( int i = 0; i < h; i++ )
		{
			for ( int j = 0; j < w; j++ )
			{
				//上がった部分だけ下げる
				if ( zval[i*w+j] > 0.0 ) zval[i*w+j] *= -1.0;
				else zval[i*w+j] = 0.0;	//その他は加算無し
				if ( i < absorbingZone || i > h-absorbingZone || j < absorbingZone || j > w-absorbingZone )
				{
					zval[i*w+j] = 0.0;
				}
				zval_cur[i*w+j] += zval[i*w+j];
			}
		}
		sprintf(fname, "%s%sInitial_wave_data3_%d.csv", drive, dir, 1);
		WriteCsv(fname, w, h, zval, 1.0);

		double* zval2 = new double [h*w];
		memcpy(zval2, zval_cur, w*h*sizeof(double));

		for ( int k = 0; k < 40; k++ )
		{
			double* zval_new = new double [h*w];

			for ( int ii = 1; ii < h-1; ii++ )
			{
				for ( int jj = 1; jj < w-1; jj++ )
				{
					zval_new[ii*w+jj] = 0.25*(zval2[(ii-1)*w+jj] + zval2[(ii+1)*w+jj] + zval2[ii*w+(jj-1)] + zval2[ii*w+(jj+1)]);
					if ( ii < absorbingZone || ii > h-absorbingZone || jj < absorbingZone || jj > w-absorbingZone )
					{
						zval_new[ii*w+jj] = 0.0;
					}
					if ( zval_new[ii*w+jj] > zval2[ii*w+jj] ) zval_new[ii*w+jj] =zval2[ii*w+jj];
				}
			}
			memcpy(zval2, zval_new, w*h*sizeof(double));
			delete [] zval_new;
		}

		double* zval3 = new double [h*w];
		memcpy(zval3, zval_cur, w*h*sizeof(double));

		for ( int i = 0; i < h; i++ )
		{
			for ( int j = 0; j < w; j++ )
			{
				//滑らかにして下がった分の補正分
				if ( zval3[i*w+j] > zval2[i*w+j] ) zval3[i*w+j] = -(zval3[i*w+j]-zval2[i*w+j]);
				else zval3[i*w+j] = 0.0;//その他はそのまま
				if ( i < absorbingZone || i > h-absorbingZone || j < absorbingZone || j > w-absorbingZone )
				{
					zval3[i*w+j] = 0.0;
				}
				zval_cur[i*w+j] += zval3[i*w+j];
			}
		}

		sprintf(fname, "%s%sInitial_wave_data3_%d.csv", drive, dir, 2);
		WriteCsv(fname, w, h, zval3, 1.0);
		delete [] zval2;
		delete [] zval3;

		BitmpFaultWrite(exe_drive, exe_dir, w, h, zval_cur, _uzmin, _uzmax, topography_data_mask, FaultParameters, Initial_wave_data_bmp);
		WriteCsv(Initial_wave_data, w, h, zval_cur, 1.0);
		delete [] zval_cur;
	}

	delete [] zval;

	delete [] map;

	if ( logFile ) fclose(logFile);
	return 0;
}

