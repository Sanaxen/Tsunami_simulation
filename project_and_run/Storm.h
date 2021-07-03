#ifndef _STORM__H
#define _STORM__H

//#pragma warning ( disable : 4996 )の代わり
#define _CRT_SECURE_NO_DEPRECATE 

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "CsvReadWrite.hpp"
#include "bitmap.h"

//#define USE_GEODESIC_H

#ifdef USE_GEODESIC_H
#include "geodesic.h"
#endif

#define STORM_ACC_RESTRICTION
//#define STORM_USE_ACC
//#define STORM_USE_ACC2

//#define STORM_USE_GPU
//#define STORM_USE_GPU2

#include <amp.h>
#include <amp_math.h>
using namespace concurrency;
#ifdef STORM_USE_GPU
#define STORM_ACC_RESTRICTION restrict(amp)
#define ACC_TYPE "GPU"
#define FAST_MATH	fast_math::
#define FAST_MATH_RSQRT(x)	 fast_math::rsqrt((x))
#else if
#define STORM_ACC_RESTRICTION restrict(cpu)
#define ACC_TYPE "CPU"
#define FAST_MATH_RSQRT(x)	 1.0/sqrt((x))
#endif

#define ABSZONE	(40)

#define M_PI       3.1415926535897932384626433832795

#define M_PI_3		1.0471975511965977461542144610932	//π/3

#define hPa		(100.0)		// --> N/m^2
#define km_h	(0.277778)	// --> m/s

#define	MIN_DIST	(1.0*1000.0)	//3.0 [km]

#define OMP_STORM_SCHEDULE schedule(dynamic)

//Myers のモデルによる台風高潮計算

//傾度風の風向は，台風の等圧線に対して約30 度内向き
#define	CD0_CONDITION	(8.0)
#define REDUCTION_COEF1	(0.7)			//傾度風速の海上風への補正係数
#define REDUCTION_COEF2	(0.7)			//低減係数
#define CORIOLIS_COEF	(7.29e-5)		//Coriolis定数
#define STD_ATMO_PRESSURE (1013.0)		//基準気圧(hPa)

//空気密度
#define ROUA	1.22	//[kg/m^3]

//海水密度
#define ROU		1026.0	//[kg/m^3]

#define ROUA_ROU	(0.00118908382066276803118908382066)	//ROUA/ROU


#define VECTOT_MAXWIND		70.0f	//ベクトル最大ピクセル長をMAXとする最大風速
#define VECTOR_LENGTH_MAX	60		//ベクトル最大ピクセル長
#define VECTOR_STEP			20

/** * ２点間の直線距離を求める（Lambert-Andoyer）
* @param   float   lat1       始点緯度(十進度) 
* @param   float   lon1       始点経度(十進度) 
* @param   float   lat2       終点緯度(十進度) 
* @param   float   lon2       終点経度(十進度) 
* @return  float               距離（m） */
inline float  distance_lambert_f(float lat1, float lon1, float lat2, float lon2) STORM_ACC_RESTRICTION
{    
#ifdef USE_GEODESIC_H
	double alp1, alp2;
	double dist = solve_inverse_problem((double)lon1, (double)lat1, (double)lon2, (double)lat2, alp1, alp2);
	return (float)dist;
#else

	// WGS84    
	float A = 6378137.;   // 赤道半径    
	float F = 1.0 / 298.257222101;     // 扁平率 扁平率 F = (A - B) / A
	float B = A * (1.0 - F);
	// 極半径 
	//lat1 = lat1*M_PI/180.0;
	//lat2 = lat2*M_PI/180.0;
	//lon1 = lon1*M_PI/180.0;
	//lon2 = lon2*M_PI/180.0;
	
	float P1 = fast_math::atan(B/A * fast_math::tan(lat1));
	float P2 = fast_math::atan(B/A * fast_math::tan(lat2));
	// Spherical Distance    
	float th = fast_math::sin(P1)*fast_math::sin(P2) + fast_math::cos(P1)*fast_math::cos(P2)*fast_math::cos(lon1-lon2);
	if ( th < -1.0 ) th = -1.0;
	else if ( th > 1.0 ) th = 1.0;
	float sd = fast_math::acos(th);    
	// Lambert-Andoyer Correction    
	float cos_sd = fast_math::cos(sd/2);    
	float sin_sd = fast_math::sin(sd/2);    
	float cc = cos_sd*cos_sd;
	float ss = sin_sd*sin_sd;
	if ( fast_math::fabs(cc) < 1.0e-16 || fast_math::fabs(ss) < 1.0e-16 )
	{
		//printf("Correction:%f -> %f\n", sd*180.0/M_PI, (sd+0.000001)*180.0/M_PI);
		//printf("tan(lat1:%f):%f\n", lat1, tan(lat1));
		//printf("tan(lat2:%f):%f\n", lat2, tan(lat2));
		sd = fast_math::acos(th)+0.000001;    
		cos_sd = fast_math::cos(sd/2);    
		sin_sd = fast_math::sin(sd/2);    
		cc = cos_sd*cos_sd;
		ss = sin_sd*sin_sd;
	}
	//if ( fabs(cc) < 1.0e-16 || fabs(ss) < 1.0e-16 )
	//{
	//	printf("%f %f\n", cc, ss);
	//	abort();
	//}


	float c = (fast_math::sin(sd) - sd) * fast_math::pow(fast_math::sin(P1)+fast_math::sin(P2),2.0) / cc;    
	float s = (fast_math::sin(sd) + sd) * fast_math::pow(fast_math::sin(P1)-fast_math::sin(P2),2.0) / ss;    
	float delta = F / 8.0 * (c - s);
	// Geodetic Distance    
	float distance = A * (sd + delta);
	//if (_isnan(distance) || distance < 0.0)
	//{
	//	printf("c %f s %f\n", c, s);
	//	printf("delta %f\n", delta);
	//	printf("distance %f\n", distance);
	//	abort();
	//}
	return distance;
#endif
}


class Storm_Point
{
public:
	double lon;
	double lat;
	float rm;	//最大風速半径[m]
	double pc;	//中心気圧[hp]
	double v;	//進行速度[m/sec]
	double t;	//時刻(sec)
	double th;	//進行角度（真東をX軸として）
	time_t tm;	//観測の日付
	int	 flg1970;
	Storm_Point()
	{
		flg1970 = -1;
	}
};

class Storm_Line
{
	int statrtupTimeCount;
	double time;
	int zeroArea;
	void* solver;
	int width;
	int height;

public:
	double latitude[4];
	double longitude[4];
	std::vector<Storm_Point> stormList;
	float* ijTodistancsP;

	double cd_coef;		//海面抵抗係数の係数
	double cd_coef2;	//海面抵抗係数の係数
	double typhoon_radius_coef;

	int statrtupTimeNum;

	int JST;
	inline Storm_Line()
	{
		JST = 0;
		ijTodistancsP = 0;
		zeroArea = 0;
		time = 0;
		stormList.clear();

		//台風第 9918 号による瀬戸内海の高潮の再現計算(水圏環境工学研究室 鷺野豊)では3.0倍している。
		cd_coef = 1.0;
		cd_coef2 = 1.0;
		typhoon_radius_coef = 1.0;
	}
	inline ~Storm_Line()
	{
		stormList.clear();
	}

	void SetSolver(void* solv);

	inline void SetZeroArea(int n)
	{
		zeroArea = n;
	}

	inline double GetTime() const
	{
		return time;
	}
	inline void SetTime(double t)
	{
		time = t;
		if ( isSetup() ) time = 0.0;
	}
	inline void TimeReflesh(double t)
	{
		const int sz = stormList.size();
		for ( int i = 1; i < sz; i++ )
		{
			stormList[i].t += t;
		}
	}

	inline void Load(char* fname)
	{
		stormList.clear();

		FILE* fp = NULL;
		char buf[256];
		
		fp = fopen(fname, "r");
		if ( fp == NULL ) return;

		int flg1970 = -1;
		struct tm tm;
		time_t begintime=0;

		int UTCtoJST = 0;
		if ( JST ) UTCtoJST = 9*3600;

		bool flg = false;
		while( fgets(buf,256,fp)!= NULL )
		{
			if ( strcmp(buf, "LAT\n") == 0 )
			{
				printf("緯度\n");
				double h,m,s;
				fgets(buf, 256, fp);
				sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); latitude[0] = (h+m/60.0 + s/3600.0)*M_PI/180.0;
				printf("北東:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, latitude[0]);
				fgets(buf, 256, fp);
				sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); latitude[1] = (h+m/60.0 + s/3600.0)*M_PI/180.0;
				printf("北西:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, latitude[1]);
				fgets(buf, 256, fp);
				sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); latitude[2] = (h+m/60.0 + s/3600.0)*M_PI/180.0;
				printf("南東:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, latitude[2]);
				fgets(buf, 256, fp);
				sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); latitude[3] = (h+m/60.0 + s/3600.0)*M_PI/180.0;
				printf("南西:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, latitude[3]);
				continue;
			}
			if ( strcmp(buf, "LON\n") == 0 )
			{
				printf("経度\n");
				double h,m,s;
				fgets(buf, 256, fp);
				sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); longitude[0] = (h+m/60.0 + s/3600.0)*M_PI/180.0;
				printf("北東:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, longitude[0]);
				fgets(buf, 256, fp);
				sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); longitude[1] = (h+m/60.0 + s/3600.0)*M_PI/180.0;
				printf("北西:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, longitude[1]);
				fgets(buf, 256, fp);
				sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); longitude[2] = (h+m/60.0 + s/3600.0)*M_PI/180.0;
				printf("南東:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, longitude[2]);
				fgets(buf, 256, fp);
				sscanf(buf, "%lf,%lf,%lf", &h, &m, &s); longitude[3] = (h+m/60.0 + s/3600.0)*M_PI/180.0;
				printf("南西:%d°%d′%.3f″ -> %f\n", (int)h, (int)m, s, longitude[3]);
				continue;
			}
			if ( strncmp(buf,"TYPHOON_BEGIN", 13) == 0 )
			{
				flg = true;
				int y;
				int m;
				int d;
				int h;
				int min;
				int sec;

				int n = sscanf(buf, "TYPHOON_BEGIN %d/%d/%d %d:%d:%d", &y, &m, &d, &h, &min, &sec);
				if ( n == 6 )
				{
					if ( y < 1970 )
					{
						flg1970 = 1;
					}else
					{
						flg1970 = 0;
					}
					tm.tm_sec = sec;
					tm.tm_min = min;
					tm.tm_hour = h;
					tm.tm_mday = d;
					tm.tm_mon = m -1;
					tm.tm_year = y - 1900;
					if ( flg1970 == 1 ) tm.tm_year = y - 1900 + 20;
					tm.tm_isdst = -1;
					tm.tm_yday = 0;
					tm.tm_wday = 0;
					begintime = mktime(&tm)+UTCtoJST;
				}
				continue;
			}
			if ( flg && strstr(buf,"TYPHOON_END") )
			{
				flg = false;
				continue;
			}
			if ( flg )
			{
				if (buf[0] == '@' )
				{
					char fname[512];
					char name[80];
					int id;
					double rm = 100.0;

					strcpy(fname, buf+1);
					char* p = strchr(fname, ' ');
					if ( p ) *p = '\0';
					strcpy(name, p+1);
					p = strchr(name, ' ');
					if ( p ) *p = '\0';
					id = atoi(p+1);
					p = strchr(p+1, ' ');
					if ( p == NULL ) printf("最大風速半径が未設定\n");
					rm = atof(p+1);
					LoadBesttrack( fname, name, id, rm );
					continue;
				}

				Storm_Point a;
				double lon[3], lat[3];
				double wrk;
				sscanf(buf, "%lf %lf,%lf,%lf %lf,%lf,%lf %lf %lf %lf", &(a.t), 
					lon, lon+1, lon+2,
					lat, lat+1, lat+2, &wrk, &(a.pc), &(a.v));
				a.rm = wrk;
				a.lon = (lon[0]+lon[1]/60.0 + lon[2]/3600.0)*M_PI/180.0;
				a.lat = (lat[0]+lat[1]/60.0 + lat[2]/3600.0)*M_PI/180.0;

				printf("時刻:%.3f[s](%.2f[h]) (経度,緯度)=(%f,%f)\n最大風速半径:%.2f[m]\n中心気圧:%.2f[hPa] 速度:%.2f[km/h]\n\n",
					a.t, a.t/3600.0, a.lon, a.lat, a.rm, a.pc, a.v);
				
				if ( stormList.size() == 0 )
				{
					a.flg1970 = flg1970;
					a.tm = begintime;
				}
				if ( stormList.size() >= 1 )
				{
					int i = stormList.size()-1;
					double dist = distance_lambert(stormList[i].lat, stormList[i].lon,  a.lat, a.lon);
					dist /= 1000.0;
					printf("移動距離:%d -> %d ==> %f[km]\n", i, i+1, dist);
					if ( a.t < 0.0 )
					{
						a.t = stormList[i].t + 3600.0*dist/stormList[i].v;
						a.flg1970 = flg1970;
						a.tm = begintime + a.t;
					}else
					{
						a.flg1970 = flg1970;
						a.tm = begintime + a.t;
					}
					if ( stormList[i].v < 0.0 )
					{
						stormList[i].v = dist/((a.t-stormList[i].t)/3600.0);
					}
				}
				if ( a.rm <= 0.0 )
				{
					if ( a.pc < 950.0 ) a.rm = 0.77*a.pc - 651.0;
					else a.rm = 1.63*a.pc - 1471.0;
#if 10
					//後でQuiringら（2011）の経験式を使う
					a.rm *= -1.0;
#endif
					a.rm *= 1000.0;
				}
				printf("\n");
				stormList.push_back(a);
			}
		}
		fclose(fp);

		const int n = stormList.size();
		if ( n == 0 ) return;

		//単位変換
		printf("============================================\n");
		for ( int i = 0; i < n; i++ )
		{
			Storm_Point& a = stormList[i];
			a.rm *= typhoon_radius_coef;
			printf("[%d]時刻:%.3f[s](%.2f[h]) (経度,緯度)=(%f,%f)\n最大風速半径:%.2f[km]\n中心気圧:%.2f[hPa] 速度:%.2f[km/h]\n\n",
				i, a.t, a.t/3600.0, a.lon, a.lat, a.rm*0.001, a.pc, a.v);

			a.v *= km_h;	//km/h -> m/s]

			if ( a.rm <= 0 )
			{
				//、Quiringら（2011）の経験式
				a.rm = 1852.0*(49.67 - 0.47 * a.v);
			}
		}
		printf("============================================\n");

		for ( int i = 0; i < n-1; i++ )
		{
			//double dx = distance_lambert(stormList[i].lat, stormList[i+1].lon,  stormList[i].lat, stormList[i].lon);
			//double dy = distance_lambert(stormList[i].lat, stormList[i].lon,  stormList[i+1].lat, stormList[i].lon);
			double dx = stormList[i+1].lon - stormList[i].lon;
			double dy = stormList[i+1].lat - stormList[i].lat;
#if 0
			if ( fabs(dx) > 1.0e-16 )
			{
				if ( stormList[i+1].lon - stormList[i].lon >= 0.0 )
				{
					stormList[i].th = atan(dy/dx);
				}else
				{
					stormList[i].th = atan(dy/dx);
				}
			}else
			{
				if ( stormList[i+1].lat - stormList[i].lat >= 0.0 )
				{
					stormList[i].th = M_PI/2.0;
				}else
				{
					stormList[i].th = 3.0*M_PI/2.0;
				}
			}
#else
			stormList[i].th = atan2(dy, dx);
#endif
		}
		stormList[n-1].th = stormList[n-2].th;

		if ( !flg )
		{
			return;
		}
		return;
	}


	inline void LoadBesttrack(char* fname, char* name, int id, double rm )
	{
		FILE* fp = fopen( fname, "r");
		if ( fp == NULL )
		{
			printf("[%s]open error.\n", fname);
			exit(0);
			return;
		}

		int UTCtoJST = 0;
		if ( JST ) UTCtoJST = 9*3600;

		int dmy[3];
		char buf[512];
		bool lockup = false;
		while ( fgets(buf, 512, fp) != NULL )
		{
			if ( strstr(buf, name) )
			{
				sscanf(buf, "%d %d %d", dmy, dmy+1, dmy+2 );
				if ( dmy[1] == id )
				{
					lockup = true;
					break;
				}
			}
		}
		if (!lockup )
		{
			printf("not data [%s --> %s]\n", fname, name);
			exit(0);
		}

		int num = 0;
		sscanf(buf, "%d %d %d", dmy, dmy+1, &num );
		printf("データ数:%d\n", num);

		int y_1970 = 0;
		for ( int i = 0; i < num; i++ )
		{
			fgets(buf, 512, fp);
			int utc;
			int sg;
			int lev;
			int la;
			int lo;
			int pa;
			sscanf(buf, "%d %d %d %d %d %d", &utc, &sg, &lev, &la, &lo, &pa);

			Storm_Point a;
			a.lat = (double)la*0.1*M_PI/180.0;
			a.lon = (double)lo*0.1*M_PI/180.0;
			a.pc = (double)pa;
			a.rm = rm;
			if ( a.rm <= 0.0 )
			{
				if (a.pc < 950.0) a.rm = 0.77*a.pc - 651.0;
				else a.rm = 1.63*a.pc - 1471.0;
#if 10
				//後でQuiringら（2011）の経験式を適用する場合
				a.rm *= -1.0;
#endif
			}
			a.t = 0.0;

			char tmp[32];
			sprintf(tmp, "%d", utc);
			int by = tmp[0] - '0';
			int y = by*10 + tmp[1] - '0';

			if ( y  > 30 ) y += 1900;
			else y += 2000;

			int m = tmp[2] - '0';
			m = 10*m + tmp[3] - '0';

			int d = tmp[4] - '0';
			d = d*10 + tmp[5]-'0';

			int h = tmp[6] - '0';
			h = 10*h + tmp[7] - '0';

			int flg1970 = 0;
			if ( y < 1970 )
			{
				flg1970 = 1;
			}
			struct tm tm;
			tm.tm_sec = 0;
			tm.tm_min = 0;
			tm.tm_hour = h;
			tm.tm_mday = d;
			tm.tm_mon = m -1;
			tm.tm_year = y - 1900;
			if ( flg1970 ) tm.tm_year = y - 1900 + 20;
			tm.tm_isdst = -1;
			tm.tm_yday = 0;
			tm.tm_wday = 0;
			a.tm = mktime(&tm)+UTCtoJST;
			a.flg1970 = flg1970;
			a.v = -1;
			stormList.push_back(a);
		}
		fclose(fp);

		for ( int i = 0; i < num-1; i++ )
		{
			stormList[i+1].t = stormList[i].t + difftime(stormList[i+1].tm, stormList[i].tm);
		}
		stormList[0].t = 0;

		for ( int i = 0; i < num-1; i++ )
		{
			double dist = distance_lambert(stormList[i].lat, stormList[i].lon,  stormList[i+1].lat, stormList[i+1].lon);
			dist /= 1000.0;
			printf("移動距離:%d -> %d ==> %f[km]\n", i, i+1, dist);
			stormList[i].v = dist/((stormList[i+1].t-stormList[i].t)/3600.0);
#if 10
			if (stormList[i].rm <= 0.0)
			{
				//Quiringら（2011）の経験式
				stormList[i].rm = 1852.0*(49.67 - 0.47 * stormList[i].v*km_h);
			}
#endif
		}
		stormList[num-1].v = stormList[num-2].v; 

		put("TyphoonParameter_all.txt");
	}


	inline void put(char* fname) const
	{
		int num = stormList.size();
		FILE* fp = fopen(fname, "w");
		for ( int i = 0; i < num; i++ )
		{
			double lon[3];
			double lat[3];

			double w = 180.0*stormList[i].lon/M_PI;

			lon[0] = (int)w;
			lon[1] = (int)((w - lon[0])*60.0);
			lon[2] = (w - lon[0] - lon[1]/60.0)*3600.0;

			w = 180.0*stormList[i].lat/M_PI;

			lat[0] = (int)w;
			lat[1] = (int)((w - lat[0])*60.0);
			lat[2] = (w - lat[0] - lat[1]/60.0)*3600.0;

			fprintf(fp, "%d %d,%d,%.2f %d,%d,%.2f %.2f %.2f %.2f\n", (int)stormList[i].t, 
				(int)lon[0], (int)lon[1], lon[2],
				(int)lat[0], (int)lat[1], lat[2], stormList[i].rm, stormList[i].pc, stormList[i].v);
		}
		fclose(fp);
	}
	void putLog();

	inline void Interpolation(int divNum)
	{
		std::vector<Storm_Point> stormList2;

		const int n = stormList.size();
		for ( int i = 0; i < n-1; i++ )
		{
			stormList2.push_back(stormList[i]);
			double pre_t = stormList2[stormList2.size()-1].t;
			for ( int k = 1; k < divNum; k++ )
			{
				Storm_Point a;
				double s = (double)k/divNum;

				a.lon = stormList[i].lon + s*( stormList[i+1].lon- stormList[i].lon);
				a.lat = stormList[i].lat + s*( stormList[i+1].lat- stormList[i].lat);
				a.v = stormList[i].v + s*( stormList[i+1].v- stormList[i].v);
				a.rm = stormList[i].rm + s*( stormList[i+1].rm- stormList[i].rm);
				a.pc = stormList[i].pc + s*( stormList[i+1].pc- stormList[i].pc);
				a.th = stormList[i].th;
				a.t = pre_t + s*(double)(stormList[i+1].t-stormList[i].t);
				a.tm = stormList[i].tm + s*(double)(stormList[i+1].t-stormList[i].t);
				a.flg1970 = stormList[i].flg1970;
				stormList2.push_back(a);
			}
		}
		stormList2.push_back(stormList[n-1]);
		stormList = stormList2;
	}

	inline Storm_Point get() const
	{
		int dmy;
		return get(time, dmy);
	}

	inline Storm_Point get(double t, int& id) const
	{
		const int n = stormList.size();
		if ( t < stormList[0].t )
		{
			id = 0;
			if ( isSetup() ) id = -1;
			return stormList[0];
		}
		if ( t > stormList[n-1].t )
		{
			id = n-1;
			Storm_Point a;

			int i = n -1;
			double s = (t - stormList[i].t)/(stormList[i].t-stormList[i-1].t);

			a = stormList[i];
			a.lon = stormList[i].lon + s*( stormList[i].lon- stormList[i-1].lon);
			a.lat = stormList[i].lat + s*( stormList[i].lat- stormList[i-1].lat);
			a.v = stormList[i].v;
			a.pc = stormList[i].pc;
			if ( STD_ATMO_PRESSURE - a.pc <= 0.0 )
			{
				a.pc = STD_ATMO_PRESSURE;
			}
			a.th = stormList[i].th;
			a.t = t;
			
			a.tm = stormList[i].tm + s*( stormList[i].tm- stormList[i-1].tm);
			a.flg1970 = stormList[i].flg1970;
			return a;
		}

		for ( int i = 0; i < n-1; i++ )
		{
			if ( t >= stormList[i].t && t <= stormList[i+1].t)
			{
				id = i;
				Storm_Point a;

				double s = (t - stormList[i].t)/(stormList[i+1].t-stormList[i].t);

				a.lon = stormList[i].lon + s*( stormList[i+1].lon- stormList[i].lon);
				a.lat = stormList[i].lat + s*( stormList[i+1].lat- stormList[i].lat);
				a.v = stormList[i].v + s*( stormList[i+1].v- stormList[i].v);
				a.rm = stormList[i].rm + s*( stormList[i+1].rm- stormList[i].rm);
				a.pc = stormList[i].pc + s*( stormList[i+1].pc- stormList[i].pc);
				a.th = stormList[i].th;
				a.t = t;
				a.tm = stormList[i].tm + s*( stormList[i+1].tm- stormList[i].tm);
				a.flg1970 = stormList[i].flg1970;
				return a;
			}
		}
		id = -999;
		printf("時間範囲エラー\n");
		return stormList[0];
	}

	inline bool isSetup() const
	{
		if ( statrtupTimeCount < statrtupTimeNum )
		{
			return true;
		}
		return false;
	}

	/** * ２点間の直線距離を求める（Lambert-Andoyer）
	* @param   float   lat1       始点緯度(十進度) 
	* @param   float   lon1       始点経度(十進度) 
	* @param   float   lat2       終点緯度(十進度) 
	* @param   float   lon2       終点経度(十進度) 
	* @return  float               距離（m） */
	inline double  distance_lambert(double lat1, double lon1, double lat2, double lon2) const
	{    
#ifdef USE_GEODESIC_H
		double alp1, alp2;
		double dist = solve_inverse_problem(lon1, lat1, lon2, lat2, alp1, alp2);
		return dist;
#else
		// WGS84    
		double A = 6378137.;   // 赤道半径    
		double F = 1.0 / 298.257222101;     // 扁平率 扁平率 F = (A - B) / A
		double B = A * (1.0 - F);
		// 極半径 
		//lat1 = lat1*M_PI/180.0;
		//lat2 = lat2*M_PI/180.0;
		//lon1 = lon1*M_PI/180.0;
		//lon2 = lon2*M_PI/180.0;
	
		double P1 = atan(B/A * tan(lat1));
		double P2 = atan(B/A * tan(lat2));
		// Spherical Distance    
		double th = sin(P1)*sin(P2) + cos(P1)*cos(P2)*cos(lon1-lon2);
		if ( th < -1.0 ) th = -1.0;
		else if ( th > 1.0 ) th = 1.0;
		double sd = acos(th);    
		// Lambert-Andoyer Correction    
		double cos_sd = cos(sd/2);    
		double sin_sd = sin(sd/2);    
		double cc = cos_sd*cos_sd;
		double ss = sin_sd*sin_sd;
		if ( fabs(cc) < 1.0e-16 || fabs(ss) < 1.0e-16 )
		{
			printf("Correction:%f -> %f\n", sd*180.0/M_PI, (sd+0.000001)*180.0/M_PI);
			//printf("tan(lat1:%f):%f\n", lat1, tan(lat1));
			//printf("tan(lat2:%f):%f\n", lat2, tan(lat2));
			sd = acos(th)+0.000001;    
			cos_sd = cos(sd/2);    
			sin_sd = sin(sd/2);    
			cc = cos_sd*cos_sd;
			ss = sin_sd*sin_sd;
		}
		//if ( fabs(cc) < 1.0e-16 || fabs(ss) < 1.0e-16 )
		//{
		//	printf("%f %f\n", cc, ss);
		//	abort();
		//}


		double c = (sin(sd) - sd) * pow(sin(P1)+sin(P2),2.0) / cc;    
		double s = (sin(sd) + sd) * pow(sin(P1)-sin(P2),2.0) / ss;    
		double delta = F / 8.0 * (c - s);
		// Geodetic Distance    
		double distance = A * (sd + delta);
		//if (_isnan(distance) || distance < 0.0)
		//{
		//	printf("c %f s %f\n", c, s);
		//	printf("delta %f\n", delta);
		//	printf("distance %f\n", distance);
		//	abort();
		//}
		return distance;
#endif
	}

	inline double AtmosphericPressure(Storm_Point& a, double r) const
	{
#if 10
		// [N/m^2]
		const double Km = 1000.0;
		double dp = STD_ATMO_PRESSURE - a.pc;
		//if (r < 5.0*Km) return a.pc + dp* exp(-a.rm / 5.0*Km);
		return a.pc + dp* exp(-a.rm/r);
#else
		// [N/m^2]
		double dp = STD_ATMO_PRESSURE - a.pc;
	
		//中心付近で現実の気圧分布とよく一致させるために, 次のような重みwを掛ける.
		const double Km = 1000.0;
		double w = 1.0;
		if (r <= 10.0*Km) w = 10.0;
		else w = 100.0 / (r / Km);

		return w * (a.pc + dp* exp(-a.rm / r));
#endif
	}

	inline double Vgr(Storm_Point& a, double r) const
	{
		// [N/m^2]
		double dp = STD_ATMO_PRESSURE - a.pc;

		double f = 2.0*CORIOLIS_COEF*sin(a.lat);

		double w = 0.0;
		
		if ( r > 0.1 ) w = f*f/4.0 + hPa*(a.rm*dp/(ROUA*r*r*r))*exp(-a.rm/r);
		if ( w < 0.0 ) w = 0.0;
		double vgr = r*(sqrt(w)-f/2.0);
		return vgr;

	}
	inline double Vtf(Storm_Point& a, double r) const
	{
		const double C2 = REDUCTION_COEF2;
		double vgr1 = Vgr(a, r);
		double vgr2 = Vgr(a, a.rm);
		
		if ( fabs( vgr2) < 1.0e-16 ) return 0.0;
		return C2*(vgr1/vgr2)*a.v;
	}


	//傾度風速
	inline void Wgr(Storm_Point& a, double lon, double lat, double r, double& wx_gr, double& wy_gr) const
	{
		double vgr = Vgr(a, r);

		double C1 = REDUCTION_COEF1;
		
		//傾度風の風向は，台風の等圧線に対して約30 度内向きに
		double th2 = 0.0;
		double dx = lon - a.lon;
		double dy = lat - a.lat;
#if 0
		if ( fabs(dx) > 1.0e-16 )
		{
			if ( dx >= 0.0 )
			{
				th2 = atan(dy / dx) + 2.0*M_PI_3;
			}
			if ( dx < 0.0 )
			{
				th2 = atan(dy / dx) + 5.0*M_PI_3;
			}
		}else
		{
			if ( dy >= 0.0 )
			{
				th2 = 2.0*M_PI_3;
			}
			if ( dy < 0.0 )
			{
				th2 = 5.0*M_PI_3;
			}
		}
#else
		th2 = atan2(dy, dx) + 2.0*M_PI_3;
#endif

		const double dp = STD_ATMO_PRESSURE - a.pc;

#if 10
		//藤井・光田(1986)

	#if 10
		const double C1_max = 1.2;//非現実的な風速になってしまうので制限！！
		const double C_inf = 0.666666666666666667;
		double s = 0.0231*dp  - 1.96;
		const double C_rm = min(max(C_inf*(1.0 + pow(10.0, s)), 1.1), C1_max);
		const double k = 2.5;
		const double sp = 0.5;
		const double x = r / a.rm;

		//printf("%f dp %f s %f pow()%f\n", a.pc/hPa, dp/hPa, s, pow(10.0,s));
		C1 = C_inf + (C_rm - C_inf)* pow(x/sp,k-1.0)*exp((1.0-1.0/k)*(1.0-pow(x/sp,k)));
	#else
		//台風23号による高潮の再現計算[http://www.nilim.go.jp/lab/bcg/siryou/tnn/tnn0273pdf/ks0273006.pdf]
		const double C_inf = 0.666666666666666667;
		const double k = 2.5;
		const double xp = 0.5;
		const double C1_xp = 1.2;
		const double x = r / a.rm;

		C1 = C_inf + (C1_xp - C_inf)* pow(x/xp, k - 1.0)*exp((1.0 - 1.0 / k)*(1.0 - pow(x/xp, k)));
	#endif

#else
		//藤井・光田(1987)
		C1 = (2.0 / 3.0)*(1.0 + pow(10.0, (0.0231*dp/hPa - 1.95)));
		if (C1 > 1.4) C1 = 1.4;	//非現実的な風速になってしまうので制限！！
#endif
		wx_gr = C1*vgr*cos(th2);
		wy_gr = C1*vgr*sin(th2);
	}

	//台風の移動による風
	inline void Wtf(Storm_Point& a, double r, double& wx_tf, double& wy_tf) const
	{
		double vtf = Vtf(a, r);
		wx_tf = vtf*cos(a.th);
		wy_tf = vtf*sin(a.th);
		//printf("wx_tf %f[m] wy_tf[m] %f\n", wx_tf, wy_tf);
	}

	inline void Wind(Storm_Point& a, int w, int h, int i, int j, double lon, double lat, double& wx, double& wy, double& r) const
	{

		double dlo = (longitude[1] - longitude[0])/(double)(w-1);
		double dla = (latitude[0] - latitude[2])/(double)(h-1);

		lon = j*dlo + longitude[0];
		lat = i*dla + latitude[2];

		if (!ijTodistancsP)
		{
			r = distance_lambert(a.lat, a.lon, lat, lon);
		}else
		{
			r = ijTodistancsP[i*w + j];
		}
		if (r < MIN_DIST) r = MIN_DIST;

		double wgr[2];
		double wtf[2];

		Wgr(a, lon, lat, r, wgr[0], wgr[1]);
		Wtf(a, r, wtf[0], wtf[1]);

		wx = wgr[0] + wtf[0];
		wy = wgr[1] + wtf[1];

		//printf("wx %f[m] wy[m] %f\n", wx, wy);
	}

	inline double ShearForce(Storm_Point& a, int w, int h, int i, int j, double& lon, double& lat, double& tx, double& ty, double& r) const
	{
		double ww[2];

		Wind(a, w, h, i, j, lon, lat, ww[0], ww[1], r);

		double www = sqrt(ww[0]*ww[0] + ww[1]*ww[1]);
		//if ( fabs(ww) > 5.0 )
		//{
		//	printf("WIndoPower %f[m/s]\n", ww);
		//}

		double Cd = 0.0026;

#if 0
		//水表面と風との抵抗係数(海面抵抗係数)
		if ( www < CD0_CONDITION ) Cd = 0.001*(1.290-0.024*www);
		else  Cd = 0.001*(0.5811+0.063*www);
#else
		//Wu(1982)	---- CD0_CONDITION*0.9375=7.5
		if (www < CD0_CONDITION*0.9375) Cd = 0.001*(1.2875);
		else  Cd = 0.001*(0.8 + 0.065*www);
#endif
		////本多・光易
		//if ( www < CD0_CONDITION ) Cd = 0.001*(1.0 -1.890*www*0.01)*1.28;
		//else Cd = 0.0001*(1.0+1.078*www*0.1)*5.81;

		//台風第 9918 号による瀬戸内海の高潮の再現計算(水圏環境工学研究室 鷺野豊)
		if ( r < a.rm*2.0 ) Cd *= cd_coef;
		Cd *= cd_coef2;

		tx = ROUA*Cd*ww[0]*www;
		ty = ROUA*Cd*ww[1]*www;

		//気圧
		double press = AtmosphericPressure(a, r);
		return press;
	}

	inline double ShearForce(Storm_Point* stm, int w, int h, int i, int j, double& tx, double& ty, double& r) const
	{

		double lon;
		double lat;

		int storm_id;
		Storm_Point a;
		
		if ( stm )
		{
			a = *stm;
		}else
		{
			a = get(time, storm_id);
		}

		double press = ShearForce(a, w, h, i, j, lon, lat,tx, ty, r);
		if ( isSetup() ) 
		{
			double s = (double)statrtupTimeCount/(double)statrtupTimeNum;
			tx *= s;
			ty *= s;
		}
		return press;
	}

	inline float MaxBreeze() const
	{
		int storm_id;
		Storm_Point a = get(time, storm_id);

		float wmax = -9999999.0f;

		const double dlo = (longitude[1] - longitude[0])/(double)(width-1);
		const double dla = (latitude[0] - latitude[2])/(double)(height-1);


		const int num_threads  = omp_get_max_threads()+1;
		float* wmax_th = new float[num_threads];
		for ( int i = 0; i < num_threads; i++ )
		{
			wmax_th[i] = wmax;
		}

#pragma omp parallel for OMP_STORM_SCHEDULE
		for ( int i = zeroArea; i < height-zeroArea; i++ )
		{
			for ( int j = zeroArea; j < width-zeroArea; j++ )
			{
				const int id = omp_get_thread_num();
				
				double lon = j*dlo + longitude[0];
				double lat = i*dla + latitude[2];

				double wx, wy, r;
				Wind(a, width, height, i, j, lon, lat, wx, wy, r);

				float ww = (wx*wx + wy*wy);
				if ( ww > wmax_th[id] ) wmax_th[id] = ww;
			}
		}
		//スレッド毎に持ち帰った値を比較して本当の最大値となった情報を取得する
		wmax = wmax_th[0];
		for ( int jj = 0; jj < num_threads-1; jj++ )
		{
			if ( wmax <= wmax_th[jj+1] )
			{
				wmax = wmax_th[jj+1];
			}
		}
		delete [] wmax_th;

		return sqrt(wmax);
	}

	inline double StormArea(double wind) const
	{
		int storm_id;
		Storm_Point a = get(time, storm_id);

		float rrr = -9999999.0f;

		const double dlo = (longitude[1] - longitude[0]) / (double)(width - 1);
		const double dla = (latitude[0] - latitude[2]) / (double)(height - 1);


		const int num_threads = omp_get_max_threads() + 1;
		float* rrr_th = new float[num_threads];
		for (int i = 0; i < num_threads; i++)
		{
			rrr_th[i] = -99999.0;
		}
		wind = wind*wind;

#pragma omp parallel for OMP_STORM_SCHEDULE
		for (int i = zeroArea; i < height - zeroArea; i++)
		{
			for (int j = zeroArea; j < width - zeroArea; j++)
			{
				const int id = omp_get_thread_num();

				double lon = j*dlo + longitude[0];
				double lat = i*dla + latitude[2];

				double wx, wy, r;
				Wind(a, width, height, i, j, lon, lat, wx, wy, r);

				float ww = (wx*wx + wy*wy);
				if (ww >= wind && r > rrr_th[id])
				{
					rrr_th[id] = r;
				}
			}
		}
		//スレッド毎に持ち帰った値を比較して本当の最大値となった情報を取得する
		double rr = rrr_th[0];
		for (int jj = 0; jj < num_threads - 1; jj++)
		{
			if (rr <= rrr_th[jj + 1])
			{
				rr = rrr_th[jj + 1];
			}
		}
		delete[] rrr_th;

		return rr;
	}
	
 
	void windVector(BitMap& bmp);
	int** windVector();
	void windVectorDelete(int** index);



	inline void dump(int index,  int w, int h) const
	{
		double* p = new double[w*h];
		memset(p, '\0', sizeof(double)*w*h);
		int storm_id;
		Storm_Point a = get(time, storm_id);
		
		const double dlo = (longitude[1] - longitude[0])/(double)(w-1);
		const double dla = (latitude[0] - latitude[2])/(double)(h-1);

		double max = -9999999999.0;
		double min = 9999999999.0;
		for ( int i = zeroArea; i < h-zeroArea; i++ )
		{
			for ( int j = zeroArea; j < w-zeroArea; j++ )
			{

				double lon = j*dlo + longitude[0];
				double lat = i*dla + latitude[2];
				double r = distance_lambert(a.lat, a.lon, lat, lon);

				p[i*w+j] = AtmosphericPressure(a, r);
				if ( p[i*w+j] > max ) max =  p[i*w+j];
				if ( p[i*w+j] < min ) min =  p[i*w+j];
			}
		}
		for ( int i = zeroArea; i < h-zeroArea; i++ )
		{
			for ( int j = zeroArea; j < w-zeroArea; j++ )
			{
				p[i*w+j] = 255*(int)(p[i*w+j]-min)/(max-min);
			}
		}
		char fname[512];
		sprintf(fname, "AtmosphericPressure%06d.csv", index);
		WriteCsv(fname, w, h, p, 1.0);
	}

	inline double* Pressure(  Storm_Point* stm, int w, int h, double& min, double& max, int& id, int index = -1) const
	{
		double* p = new double[w*h];
		//memset(p, '\0', sizeof(double)*w*h);

#pragma omp parallel for OMP_STORM_SCHEDULE
		for ( int i = 0; i < h; i++ )
		{
			for ( int j = 0; j < w; j++ )
			{
				p[i*w+j] = STD_ATMO_PRESSURE;
			}
		}

		Storm_Point a;
		
		if ( stm )
		{
			a = *stm;
		}else
		{
			a= get(time, id);
		}

		if ( index >= 0 )
		{
			if ( index >= stormList.size())
			{
				return NULL;
			}
			a = this->stormList[index];
		}

		const double dlo = (longitude[1] - longitude[0])/(double)(w-1);
		const double dla = (latitude[0] - latitude[2])/(double)(h-1);

		double wmax = -999999.0;
		max = -9999999999.0;
		min = 9999999999.0;

#pragma omp parallel for OMP_STORM_SCHEDULE
		for ( int i = 0; i < h-0; i++ )
		{
			for ( int j = 0; j < w-0; j++ )
			{

				double lon = j*dlo + longitude[0];
				double lat = i*dla + latitude[2];

				double r;
				if (!ijTodistancsP)
				{
					r = distance_lambert(a.lat, a.lon, lat, lon);
				}else
				{
					r = ijTodistancsP[i*w + j];
				}

				if (r < MIN_DIST) r = MIN_DIST;
				p[i*w+j] = AtmosphericPressure(a, r);
			}
		}

		for ( int i = 0; i < h-0; i++ )
		{
			for ( int j = 0; j < w-0; j++ )
			{
				if ( p[i*w+j] > max ) max =  p[i*w+j];
				if ( p[i*w+j] < min ) min =  p[i*w+j];
			}
		}
		//if ( index < 0 ) printf("\t大気圧 Max %f[hPa] Min %f[hPa]\n", max/hPa, min/hPa);
		if ( isSetup() ) 
		{
			double s = (double)statrtupTimeCount/(double)statrtupTimeNum;

			for ( int i = 0; i < h-0; i++ )
			{
				for ( int j = 0; j < w-0; j++ )
				{
					p[i*w+j] = (1.0-s)*STD_ATMO_PRESSURE + s*p[i*w+j];
				}
			}
		}
		return p;
	}

	inline double* WaterLevelRaising( Storm_Point* stm, int w, int h, double& min, double& max, int& id, int index = -1) const
	{
		double* p = Pressure( stm,  w,  h, min, max, id, index);
		if ( p == NULL )
		{
			return NULL;
		}
		max = -999999.0;
		min = 999999.0;


#pragma omp parallel for OMP_STORM_SCHEDULE
		for ( int i = 0; i < h-0; i++ )
		{
			for ( int j = 0; j < w-0; j++ )
			{
				//気圧による水位上昇
				double d = STD_ATMO_PRESSURE - p[i*w+j];
				p[i*w+j] = d*hPa/(ROU*9.8);

				if ( p[i*w+j] < 0.0 ) p[i*w+j] = 0.0;
				if ( p[i*w+j] > 0.0 && p[i*w+j] < 0.01 ) p[i*w+j] = p[i*w+j]*exp(-0.01/p[i*w+j])/exp(-1.0);
				if ( (i < zeroArea || i >= h-zeroArea) || ( j < zeroArea || j >= w-zeroArea) )
				{
					p[i*w+j] = 0.0;
				}
			}
		}
		for ( int i = zeroArea; i < h-zeroArea; i++ )
		{
			for ( int j = zeroArea; j < w-zeroArea; j++ )
			{
				if ( p[i*w+j] > max )
				{
					max = p[i*w+j];
				}
				if ( p[i*w+j] < min )
				{
					min = p[i*w+j];
				}
			}
		}
		if ( isSetup() ) printf("セットアップ中[%d/%d]\n", statrtupTimeCount+1, statrtupTimeNum);
		return p;
	}

	double absorbingZoneBounray(int i, int j, int flg =  -1);//吸収境界条件 Crejan(1985)
	bool isAbsorbingZoneBounray(int i, int j);

	void dumpBmp( char* drive, char* dir, char* IDname, BitMap& topography_data_mask_bmp, BitMap& colormap_default);
	void dumpBmpWaterLevelRaising( char* drive, char* dir, char* IDname, BitMap& topography_data_mask_bmp, BitMap& colormap_default);
	void dumpBmpCenter( char* drive, char* dir, char* IDname, BitMap& topography_data_mask_bmp, BitMap& colormap_default);

	inline void StartUpTime(int n)
	{
		statrtupTimeCount = 0;
		statrtupTimeNum = n;
	}
	inline void StartUpCount()
	{
		statrtupTimeCount++;
	}
	void Storm_Line::ChkArea();

	void ijTodistancsClear()
	{
		if ( ijTodistancsP )
		{
			delete [] ijTodistancsP;
		}
		ijTodistancsP = 0;
	}

	void ijTodistancs(Storm_Point* a, int w, int h)
	{
#ifndef STORM_USE_ACC
		const float dlo = (longitude[1] - longitude[0])/(float)(w-1);
		const float dla = (latitude[0] - latitude[2])/(float)(h-1);

		float org_lon = a->lon;
		float org_lat = a->lat;
		float lon_offset = longitude[0];
		float lat_offset = latitude[2];

		if ( ! ijTodistancsP ) ijTodistancsP = new float[w*h];

		for ( int i = 0; i < h; i++ )
		{
			for ( int j = 0; j < w; j++ )
			{
				float lon = j*dlo + lon_offset;
				float lat = i*dla + lat_offset;
				ijTodistancsP[i*w+j] = distance_lambert_f(org_lat, org_lon, lat, lon);
			}
		}
#endif

#ifdef STORM_USE_ACC
		const float dlo = (longitude[1] - longitude[0])/(float)(w-1);
		const float dla = (latitude[0] - latitude[2])/(float)(h-1);

		float org_lon = a->lon;
		float org_lat = a->lat;
		float lon_offset = longitude[0];
		float lat_offset = latitude[2];

		if ( ! ijTodistancsP ) ijTodistancsP = new float[w*h];

		extent<2> e(h,w);
		array_view<float, 2> r(e, ijTodistancsP);

		parallel_for_each(e, [=](index<2> idx) STORM_ACC_RESTRICTION
		{
			const int i = idx[0];
			const int j = idx[1];
			float lon = j*dlo + lon_offset;
			float lat = i*dla + lat_offset;
			r[idx] = distance_lambert_f(org_lat, org_lon, lat, lon);
		});

#if 0
		printf("%d %d  %d %d\n", w, e[0], h, e[1] );
		for ( int i = 0; i < h; i++ )
		{
			for ( int j = 0; j < w; j++ )
			{
				index<2> idx;
				idx[0] = i;
				idx[1] = j;
				printf("%f %f %f\n", r[idx], ijTodistancsP[i*w+j], fabs(r[idx]-ijTodistancsP[i*w+j]));
			}
		}
#endif
#endif

	}
};


//ラディエーション応力
#define ARY_f(i,j)		[(i)*IX+(j)]
#define DIFFx_f(a,i,j)	((- a ARY_f(i,j-1) + a ARY_f(i,j+1))*0.5f)
#define DIFFy_f(a,i,j)	((- a ARY_f(i-1,j) + a ARY_f(i+1,j))*0.5f)


class Radiation
{
	float* sxx;
	float* syy;
	float* sxy;

public:

	float* radiation_x;
	float* radiation_y;

	~Radiation()
	{
		delete [] radiation_x;
		delete [] radiation_y;
	}

	Radiation(int w, int h, int* topg, double* m, double*n, double* w_height, float* r_dx, float r_dy)
	{
		sxx = new float[w*h];
		syy = new float[w*h];
		sxy = new float[w*h];

		const float coef = ROU*9.8f/8.0f;
#ifdef STORM_USE_ACC2
		extent<2> e(h,w);
		array_view<float, 2> sxx_(e, sxx);
		array_view<float, 2> syy_(e, syy);
		array_view<float, 2> sxy_(e, sxy);
		array_view<const int, 2> topg_(e, topg);

		float* m_f = new float[w*h];
		float* n_f = new float[w*h];
		float* H_f = new float[w*h];

#pragma omp parallel for OMP_STORM_SCHEDULE
		for ( int i = 0; i < h-0; i++ )
		{
			for ( int j = 0; j < w-0; j++ )
			{
				m_f[i*w+j] = m[i*w+j];
				n_f[i*w+j] = n[i*w+j];

				H_f[i*w+j] = 0.0f;

				//1.65倍として推移する(本来は波高=発生した波の頂上から谷までの高さの差)
				H_f[i*w+j] = 1.65f*fabs(w_height[i*w+j]);
			}
		}
		array_view<const float, 2> m_(e, m_f);
		array_view<const float, 2> n_(e, n_f);
		array_view<const float, 2> H_f_(e, H_f);


		parallel_for_each(e, [=](index<2> idx) STORM_ACC_RESTRICTION
		{
			const int i = idx[0];
			const int j = idx[1];
			sxx_[idx] = 0.0f;
			syy_[idx] = 0.0f;
			sxy_[idx] = 0.0f;
			if ( topg_[idx] != 0 )
			{
				const float H = H_f_[idx];
				const float mm = m_[idx];
				const float nn = n_[idx];
				const float x = mm*mm + nn*nn;
				if ( FAST_MATH fabs(x) > 0.001f )
				{
					float rr = FAST_MATH_RSQRT(x);
					float E = coef*(H*H);
					float costh = mm*rr;
					float sinth = nn*rr;

					float n = 1.0f;
					sxx_[idx] = E*(n*(1.0f + costh*costh)-0.5f); 
					sxy_[idx] = E*(n*costh*sinth); 
					syy_[idx] = E*(n*(1.0f + sinth*sinth)-0.5f);
				}
			}
		});
		delete [] m_f;
		delete [] n_f;
		delete [] H_f;
#else
#pragma omp parallel for OMP_STORM_SCHEDULE
		for ( int i = 0; i < h-0; i++ )
		{
			for ( int j = 0; j < w-0; j++ )
			{
				sxx[i*w+j] = 0.0f;
				syy[i*w+j] = 0.0f;
				sxy[i*w+j] = 0.0f;
				if ( topg[i*w+j] != 0 )
				{
					//(本来は波高=発生した波の頂上から谷までの高さの差)
					double H = 1.65*fabs(w_height[i*w+j]);
					double mm = m[i*w+j];
					double nn = n[i*w+j];
					double x = mm*mm + nn*nn;
					if ( fabs(x) > 0.001 )
					{
						double rr = 1.0/sqrt(x);
						double E = coef*(H*H);
						double costh = mm*rr;
						double sinth = nn*rr;

						//長波（浅海波）近似をしている場合 n=群速度/波速 => 群速度c0=√(gh), 波速c=√(gh) => n = 1.0
						//深海波（沖波）で n = 0.5 ，極浅海波（長波）で n = 1.0 となり，群速度は波速以下である						
						double n = 1.0f;
						sxx[i*w+j] = E*(n*(1.0f + costh*costh)-0.5f); 
						sxy[i*w+j] = E*(n*costh*sinth); 
						syy[i*w+j] = E*(n*(1.0f + sinth*sinth)-0.5f);
					}
				}
			}
		}
#endif
		radiation_x = new float[w*h];
		radiation_y = new float[w*h];
		const int IX = w;
		const float r_rou = 1.0f/(float)ROU;

		
#pragma omp parallel for OMP_STORM_SCHEDULE
		for ( int i = 0; i < h-0; i++ )
		{
			for ( int j = 0; j < w-0; j++ )
			{
				radiation_x[i*w+j] = 0.0f;
				radiation_y[i*w+j] = 0.0f;
				if ( i-1 < 0 || i+1 >= h || j-1 < 0 || j+1 <= w)
				{
					continue;
				}
				if (  topg[i*w+j] == 0 )
				{
					continue;
				}
				radiation_x[i*w+j] = (DIFFx_f(sxx, i,j)*r_dx[i] + DIFFy_f(sxy, i,j)*r_dy)*r_rou;
				radiation_y[i*w+j] = (DIFFx_f(sxy, i,j)*r_dx[i] + DIFFy_f(syy, i,j)*r_dy)*r_rou;
			}
		}
		delete [] sxx;
		delete [] syy;
		delete [] sxy;
	}

};

#endif
