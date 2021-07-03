#include "tsu.h"
#include "Storm.h"

//#define USE_OPENCL
#ifdef USE_OPENCL
#pragma comment(lib, "C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v5.0\\lib\\Win32\\OpenCL.lib")
#include "./mOpenCL/mOpenCL.hpp"
#endif

//int _matherr( struct _exception *except )
//{
//    /* Handle _DOMAIN errors for log or log10. */
//    if( except->type == _DOMAIN )
//    {
//		printf("%s (%f)\n", except->name, except->arg1);
//		exit(0);
//        return 1;
//    }
//    return 0;    /* Else use the default actions */
//}

extern float* debug_marker;

AbeNoguera::AbeNoguera(char* topography_data, char* water_depth_data, char* elevation_data, char* Initial_wave_dataUp, char* Initial_wave_dataDown, char* Initial_wave_data2, int impact)
{
	r_dx = NULL;
	wave_LinearInterpolation = true;
	isCopy = false;
	elevation = NULL;
	latitude = NULL;
	manning = NULL;
	h_max = 1.0;
	h_min = 0.0;
	e_min = 0.0;
	e_max = 0.0;
	w_scale[0] = 1.0;
	w_scale[1] = 1.0;
	w = ReadCsv(Initial_wave_dataUp, iX, jY);
	double* down_w = ReadCsv(Initial_wave_dataDown, iX, jY);


	if ( w && down_w )
	{
		for ( int i = 0; i < iX*jY; i++ )
		{
			if ( down_w[i] != 0.0 && w[i] == 0.0 )
			{
				w[i] = -down_w[i];
			}
		}
		delete [] down_w;
	}

	{
		FILE* fp = fopen(Initial_wave_data2, "r");
		if ( fp )
		{
			fclose(fp);
			if ( w ) delete [] w;

			w = ReadCsv(Initial_wave_data2, iX, jY);
			wave_LinearInterpolation = false;
			printf("[calculate deformation:fault model]\nMansinha and Smylie(1971)/ Okada [1985]\n");
		}
	}

	if (w == NULL )
	{
		double* tmp = ReadCsv(elevation_data, iX, jY);
		if (tmp == 0 )
		{
			printf("最低限必要なデータが見つかりません");
			exit(-1);
		}
		printf("初期波源はありませんでした\n");
		delete[] tmp;
		w = new double [iX*jY];
		memset(w, '\0', sizeof(double)*iX*jY);
	}
	w_start_org = new double [iX*jY];
	memcpy(w_start_org, w, sizeof(double)*iX*jY);

	h = ReadCsv(water_depth_data, iX, jY);
	if ( h == NULL )
	{
		h = new double[iX*jY];
		memset(h, '\0', sizeof(double)*iX*jY);
	}

	double* wrk = ReadCsv(topography_data, iX, jY);
	topog = new int[iX*jY];
	for ( int i = 0; i < iX*jY; i++ )
	{
		topog[i] = (int)wrk[i];
	}
	delete [] wrk;

	elevation = ReadCsv(elevation_data, iX, jY);

	if ( elevation == NULL )
	{
		elevation = new double[iX*jY];
		memset(elevation, '\0', sizeof(double)*iX*jY);
		//WriteCsv("aaaaaaa.csv", iX, jY, elevation,1.0);
	}

	m = new double[iX*jY];
	n = new double[iX*jY];

	memset(m, '\0', iX*jY*sizeof(double));
	memset(n, '\0', iX*jY*sizeof(double));

	right.create(iX, jY);

	e_min_org = 9999999.0;
	e_max_org = -99999999.0;

	h_min_org = 9999999.0;
	h_max_org = -99999999.0;
	for ( int i = 0; i < jY; i++ )
	{
		for ( int j = 0; j < iX; j++ )
		{
			//陸地
			if ( T(i,j) == 0 )
			{
				if ( impact && W(i,j) < 0.0)
				{
					h[i*iX+j] = -W(i,j);
					T(i,j) = 255;
					ELV(i,j) = 0;
				}else
				{
					h[i*iX+j] = 0;
					W(i,j) = 0;
					if (e_min_org > ELV(i,j)) e_min_org = ELV(i,j);
					if (e_max_org < ELV(i,j)) e_max_org = ELV(i,j);
				}
			}

			//陸地データの陸地以外をマスク
			if ( T(i,j) != 0 )
			{
				T(i,j) = 255;
				ELV(i,j) = 0;
			}
			if ( T(i,j) != 0 )
			{
				if (h_min_org > H(i,j)) h_min_org = H(i,j);
				if (h_max_org < H(i,j)) h_max_org = H(i,j);
			}
		}
	}
	printf("入力データ 水深　MIN:%f　MAX:%f\n", h_min_org, h_max_org);
	printf("入力標高データ 　MIN:%f　MAX:%f\n", e_min_org, e_max_org);


#if 0
	for ( int j = 0; j < iX; j++ )
	{
		H(0,j) = 0.0;
		U(0,j) = 0.0;
		V(0,j) = 0.0;
		W(0,j) = 0.0;

		H(jY-1,j) = 0.0;
		U(jY-1,j) = 0.0;
		V(jY-1,j) = 0.0;
		W(jY-1,j) = 0.0;
	}
	for ( int i = 0; i < jY; i++ )
	{
		H(i,0) = 0.0;
		U(i,0) = 0.0;
		V(i,0) = 0.0;
		W(i,0) = 0.0;

		H(i, iX-1) = 0.0;
		U(i, iX-1) = 0.0;
		V(i, iX-1) = 0.0;
		W(i, iX-1) = 0.0;
	}
#endif
}

void AbeNoguera::MakeManninig(double manning, double manning2, char* manninig_data)
{
	double* ma = new double[iX*jY];

#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < jY; i++ )
	{
		for ( int j = 0; j < iX; j++ )
		{
			ma[i*iX + j] = manning;
			if (T(i, j) != 0) ma[i*iX + j] *= manning2;
		}
	}
	WriteCsv(manninig_data, iX, jY, ma, 1.0);
	delete [] ma;
}

void AbeNoguera::Manning(double Manning, double Manning2, char* manninig_data, FILE* logfp =0)
{
	FILE* fp = NULL;

	fp = fopen(manninig_data, "r");
	if ( fp == NULL ) return;

	fclose(fp);

	double max, min;

	max = -999999.0;
	min = 9999999.0;
	int w, h;
	manning = ReadCsv(manninig_data, w, h);
	if ( w != iX || h != jY )
	{
		if (logfp) fprintf(logfp, "GRID SIZE ERRRO.\n");
		throw "GRID SIZE ERRRO.\n";
	}


	printf("Manning係数 %f 水域 %f x %f=%f\n", Manning, Manning, Manning2, Manning*Manning2);
	if (manning)
	{
		for ( int i = 0; i < jY; i++ )
		{
			for ( int j = 0; j < iX; j++ )
			{
				manning[i*iX + j] *= Manning;
				if (topog[i*iX + j] != 0) manning[i*iX + j] *= Manning2;
				//if ( manning[i*iX + j] > max ) max = manning[i*iX + j];
				//if ( manning[i*iX + j] < min ) min = manning[i*iX + j];
			}
		}

		//マニング係数の平滑化
		double* tmp = new double[iX*jY];
		for (int k = 0; k < 2; k++)
		{
			memcpy(tmp, manning, sizeof(double)*iX*jY);
			for (int i = 1; i < jY - 1; i++)
			{
				for (int j = 1; j < iX - 1; j++)
				{
					manning[i*iX + j] = 0.25*(tmp[(i - 1)*iX + j] + tmp[(i + 1)*iX + j] + tmp[i*iX + (j - 1)] + tmp[i*iX + (j + 1)]);
					if (manning[i*iX + j] > max) max = manning[i*iX + j];
					if (manning[i*iX + j] < min) min = manning[i*iX + j];
				}
			}
		}
		delete[] tmp;
	}
	if ( max > 0.1 || min < 0 )
	{
		printf("*********** Warning ***********\n");
		if (logfp)	fprintf(logfp, "*********** Warning ***********\n");

	}
	printf("マニング係数 MAX:%f MIN:%f\n", max, min);
	if ( logfp ) fprintf(logfp, "マニング係数 MAX:%f MIN:%f\n", max, min);
	if ( max > 0.1 || min < 0 )
	{
		printf("*******************************\n");
	}
}

void AbeNoguera::MakeLatitude(double box[4], char* latitude_data)
{
	double* la = new double[iX*jY];

	const double dlx = 1.0/(iX-1);
	const double dly = 1.0/(jY-1);

#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < jY; i++ )
	{
		for ( int j = 0; j < iX; j++ )
		{
			const double p = j*dlx;
			const double q = i*dly;
			la[i*iX + j] = ( 1-p)*(1-q)*box[0] + p*(1-q)*box[1] + (1-p)*q*box[2] + p*q*box[3];
		}
	}

	WriteCsv(latitude_data, iX, jY, la, 1.0);
	delete [] la;
}


void AbeNoguera::Latitude(char* latitude_data)
{
	FILE* fp = NULL;

	fp = fopen(latitude_data, "r");
	if ( fp == NULL ) return;

	fclose(fp);

	int w, h;
	latitude = ReadCsv(latitude_data, w, h);
}


void AbeNoguera::ToBitmap(BitMap& bmp)
{
	bmp.Create(iX, jY);

	const int ysz = jY;
	const int xsz = iX;

#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < ysz; i++ )
	{
		for ( int j = 0; j < xsz; j++ )
		{
			bmp.cell(i, j).r = (unsigned char)W(i,j);
		}
	}
}

void AbeNoguera::BitmapMaskTopog(BitMap& bmp)
{
	const int ysz = jY;
	const int xsz = iX;

#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < ysz; i++ )
	{
		for ( int j = 0; j < xsz; j++ )
		{
			if ( T(i,j) == 0 )
			{
				bmp.cell(i, j).r = bmp.cell(i, j).g = bmp.cell(i, j).b =0;
			}
		}
	}
}

void AbeNoguera::BitmapMask(BitMap& bmp)
{
	const int ysz = jY;
	const int xsz = iX;

	const Rgb white(255,255,255);
#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < ysz; i++ )
	{
		for ( int j = 0; j < xsz; j++ )
		{
			if ( T(i,j) != 0 )
			{
				bmp.cell(i, j) = white;
			}
		}
	}
}
void AbeNoguera::BitmapMask(BitMap& bmp, BitMap& maskbmp, double* z)
{
	const int ysz = jY;
	const int xsz = iX;

	const int IX = iX;
#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < ysz; i++ )
	{
		const int iIX = i*IX;
		for ( int j = 0; j < xsz; j++ )
		{
			if ( T(i,j) == 0 && fabs(z[iIX+j]) < 0.1  )
			{
				bmp.cell(i, j) = maskbmp.cell(i,j);
			}
		}
	}
}
void AbeNoguera::Output(char* filename, double scale)
{
	if ( OUTPUT_VALUE == 0 )
	{
		WriteCsv(filename, iX, jY, w, scale);
	}
	if ( OUTPUT_VALUE == 1 )
	{
		WriteCsv(filename, iX, jY, m, scale);
	}
	if ( OUTPUT_VALUE == 2 )
	{
		WriteCsv(filename, iX, jY, n, scale);
	}
	if ( OUTPUT_VALUE == 3 )
	{
		WriteCsv2(filename, iX, jY, m, n, scale);
	}
}

void AbeNoguera::Output(char* drive, char* dir, int index, double scale )
{
	char fname[512];
	sprintf(fname, "%s%soutput%06d.csv", drive, dir, index);
	WriteCsv(fname, iX, jY, w, scale);

	sprintf(fname, "%s%soutput_M%06d.csv", drive, dir, index);
	WriteCsv(fname, iX, jY, m, scale);

	sprintf(fname, "%s%soutput_N%06d.csv", drive, dir, index);
	WriteCsv(fname, iX, jY, n, scale);
}


double Solver::absorbingZoneBounray(int i, int j, int flg)
{
	double dw1 = 1.0;
	double dw2 = 1.0;
	double dw = 1.0;

	const int absorbingZone_ = absorbingZone;
	const int IX = iX;
	const int JY = jY;

	//吸収境界条件 Crejan(1985)
	if ( IsAbsorbingZoneBounray(i,j) )
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

class QH_
{
public:
	double q;
	double h;

	QH_()
	{
		q = 0.0;
		h = 0.0;
	}

	inline void Update(double q_, double h_)
	{
		if ( q < q_ )
		{
			q = q_;
			h = h_;
		}
	}
};

#define QH_SIZE_MAX	(3600)
#ifdef FLOW_LIMIT_DEBUG

QH_ *QH = 0;
#endif

static int debug_index_i = 300;
static int debug_index_j = 400;

//#define DEBUG_LOG(i,j) (i == debug_index_i && j == debug_index_j)
#define DEBUG_LOG(i,j) 0
//#define DEBUG_LOG(i,j) (W(i, j) > 0.01 && timeIndex % 31 == 0)
//#define DEBUG_LOG(i,j) (W(i, j) > 0.1 && w_init == -1)
//#define DEBUG_LOG(i,j) (H(i, j) >= 59 && w_init == -1)

int Solver::MotionEquation2(double delta_t, double delta_x, double delta_y)
{
	const double r_dy = 1.0/delta_y;
	const double r_dt = 1.0/delta_t;
	const double r_dyy = r_dy*r_dy;
	const double r_dyyy = r_dy*r_dy*r_dy;

	const int soltype = this->solver;
	const int soliton = this->Soliton;
	const int coriolis = this->Coriolis_force;
	const double omega_val = 2.0*this->omega;

	const int ysz = jY-ZERO_AREA_WD;
	const int xsz = iX-ZERO_AREA_WD;

#ifdef FLOW_LIMIT_DEBUG
	if ( QH == 0 ) QH = new QH_[ QH_SIZE_MAX];
#endif

#include "array_expand.h"


	//コリオリ因子の計算
	if ( coriolis && !coriolisFactor)
	{
		if ( !coriolisFactor) coriolisFactor = new double[ Data->jY* Data->iX];

#pragma omp parallel for OMP_SCHEDULE
		for (int ii__ = 0; ii__ < iX*jY; ii__++)
		//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
		{
			int i = ii__ / iX;
			int j = ii__ % iX;
			if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
			{
				continue;
			}
			//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
			{
				coriolisFactor[i*IX+j] = omega_val*sin(LAT(i,j));
			}
		}
	}


	//高潮計算に必要な付加項の計算
	Storm_Point* stm_p = NULL;
	if ( Storm_p)
	{
		double p_min, p_max;
		int storm_id;
		stm_p = &(Storm_p->get());

		Storm_p->ijTodistancs(stm_p, iX, jY);

		//吸い上げ効果
#if 10
		//(#1)
		double* waterLevelRaising = Storm_p->WaterLevelRaising( stm_p, iX, jY, p_min, p_max, storm_id);
#else
		//(#2)
		double* waterLevelRaising = Storm_p->Pressure( stm_p, iX, jY, p_min, p_max, storm_id);
#endif
		//printf("Storm[%d]-->Strom[%d] time %.4f 吸い上げ効果:%.2f\n", storm_id, storm_id+1, Storm_p->get().t, p_max);

		if ( shearForceX == 0 )
		{
			shearForceX = new double[iX*jY];
			shearForceY = new double[iX*jY];
		}
		memset(shearForceX, '\0', sizeof(double)*iX*jY);
		memset(shearForceY, '\0', sizeof(double)*iX*jY);

		//海面せん断力＋吸い上げ効果の計算
#pragma omp parallel for OMP_SCHEDULE
		for (int ii__ = 0; ii__ < iX*jY; ii__++)
		//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
		{
			int i = ii__ / iX;
			int j = ii__ % iX;
			if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
			{
				continue;
			}
			const double r_dx = R_DX(i); //1.0/delta_x;
			//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
			{

				const double d2 = D2__(i,j);
				const double gd2 = g_*d2;
				double tau_x, tau_y;
				double r;

				if (  T__(i,j) == 0 || D2__(i, j) < 1.0 )
				{
					continue;
				}
				if ( IsAbsorbingZoneBounray(i,j) )
				{
					continue;
				}

				double press = Storm_p->ShearForce( stm_p, IX, JY, i, j,  tau_x, tau_y, r);

				//吸い上げ効果の計算
#if 10
				//(#1)
				double dpx = gd2*DIFFx(waterLevelRaising, i,j)*r_dx;
				double dpy = gd2*DIFFy(waterLevelRaising, i,j)*r_dy;

				//  吸い上げ効果 + 海面せん断力(吹き寄せ効果)
				shearForceX[i*iX + j] = dpx + tau_x/ROU;
				shearForceY[i*iX + j] = dpy + tau_y/ROU;
#else
				//(#2)
				double dpx = d2*DIFFx(waterLevelRaising, i,j)*r_dx/ROU;
				double dpy = d2*DIFFy(waterLevelRaising, i,j)*r_dy/ROU;

				//  吸い上げ効果 + 海面せん断力(吹き寄せ効果)
				shearForceX[i*iX + j] = -hPa*dpx + tau_x/ROU;
				shearForceY[i*iX + j] = -hPa*dpy + tau_y/ROU;
#endif
			}
		}
		delete [] waterLevelRaising;


		//Radiation応力項の計算
		float* r_dx = new float[jY];
#pragma omp parallel for OMP_SCHEDULE
		for ( int i = 0; i < jY; i++ )
		{
			r_dx[i] = R_DX(i); //1.0/delta_x;
		}
		Radiation radi(iX, jY, Data->topog, Data->m, Data->n, Data->w, r_dx, (float)r_dy);

#pragma omp parallel for OMP_SCHEDULE
		for (int ii__ = 0; ii__ < iX*jY; ii__++)
		//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
		{
			int i = ii__ / iX;
			int j = ii__ % iX;
			if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
			{
				continue;
			}
			//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
			{
				if (  T__(i,j) == 0 || D2__(i, j) < 1.0 )
				{
					continue;
				}
				if ( IsAbsorbingZoneBounray(i,j) )
				{
					continue;
				}
				shearForceX[i*iX+j] +=  -radi.radiation_x[i*iX+j];
				shearForceY[i*iX+j] +=  -radi.radiation_y[i*iX+j];
			}
		}
		delete [] r_dx;
		Storm_p->ijTodistancsClear();
	}


	int	use_upwindow_method = use_upwindow;


	int skipp_advection_terms = 0;
	int eval_advection_terms = 0;
	//運動方程式
#pragma omp parallel for OMP_SCHEDULE reduction(+:skipp_advection_terms) reduction(+:eval_advection_terms)
	for ( int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}

		const double r_dx = R_DX(i); //1.0/delta_x;
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			//圧力項の計算
			double local_termM;
			double local_termN;

			const double d2 = D2__(i,j);
			const double gd2 = g_*d2;
			double MM = M__(i,j);
			double NN = N__(i,j);

			local_termM = gd2*DIFFx(w__, i, j);
			local_termN = gd2*DIFFy(w__, i, j);



			if (DEBUG_LOG(i,j))
			{
				log_printf("\n[timeIndex %d]\nDEBUG:local_termM %.16f local_termN %.16f\n", timeIndex, local_termM, local_termN);
				log_printf("DEBUG:M %.16f N %.16f W %.16f\n", MM, NN, W(i,j));
			}

			//圧力項の境界条件
			//∂w/∂n = ∂w/∂x * nx + ∂w/∂y * ny = 0
			if ( RunUp_ )
			{
				if (RUNUPFLG(i,j) != IS_DRY )
				{
					if (RUNUPFLG(i-1,j) == IS_DRY || RUNUPFLG(i+1,j) == IS_DRY )
					{
						// n = (0, 1)
						//∂w/∂y * ny = 0 => w(i+1,j) - w(i-1,j) = 0 => w(i-1,j) = w(i+1,j)
						// ∴ ∂w/∂y = 0
						local_termN = 0.0;
					}
					if (RUNUPFLG(i,j-1) == IS_DRY || RUNUPFLG(i,j+1) == IS_DRY)
					{
						local_termM = 0.0;
					}
				}
			}else
			{
				if ( T__(i,j) != 0 )
				{
					if ( T__(i-1,j) == 0 || T__(i+1,j) == 0 )
					{
						local_termN = 0.0;
					}
					if ( T__(i,j-1) == 0 || T__(i,j+1) == 0 )
					{
						local_termM = 0.0;
					}
				}
			}

			//遡上計算ロジックを使っているため地盤高の考慮は要らない
			/*
			  河川では方程式上は座標系は鉛直方向を+として local_termM = g*h(∂(h+z)/∂x)  --(1)。

			  陸地では標高側はマイナスになるので local_termM = g*D(∂η/∂x) = g*(η-標高)(∂η/∂x)　となる。
			  遡上計算ロジックでは全水深Dが陸地での水深としているのでD=陸地での水深
			  また、陸地での水深=遡上計算ロジックによって水位+標高にあるとしている。
			  従って遡上計算ロジックではg*h(∂(h+z)/∂x)= g*D(∂η/∂x)と考えてよい(?)

			 η->陸地での水深+標高　と変換すると g*(陸地での水深+標高-標高)(∂(陸地での水深+標高)/∂x)=g*(陸地での水深)(∂(陸地での水深+標高)/∂x)
			 したがって、 河川では陸地での水深をh,標高（地盤高)zと書くとlocal_termM = g*h(∂(h+z)/∂x)	==(1)と一致する。
			*/
			if (RunUp && T__(i, j) == 0 && W__(i, j) > 0.05)
			{
				if (RUNUPFLG(i - 1, j) == IS_WET && T__(i - 1, j) == 0 &&
					RUNUPFLG(i + 1, j) == IS_WET && T__(i + 1, j) == 0 &&
					RUNUPFLG(i, j - 1) == IS_WET && T__(i, j - 1) == 0 &&
					RUNUPFLG(i, j + 1) == IS_WET && T__(i, j + 1) == 0)
				{
					double dxE = DIFFx(elv__, i, j);
					double dyE = DIFFy(elv__, i, j);

					local_termM += gd2*dxE;
					local_termN += gd2*dyE;
				}
			}

			FM__(i,j) = - local_termM*r_dx;
			FN__(i,j) = - local_termN*r_dy;
			if (  DEBUG_LOG(i,j) )
			{
				log_printf("\n\nDEBUG:local_termM %f local_termN %f\n", local_termM, local_termN);
				log_printf("DEBUG:M %f N %f\n", MM, NN);
				log_printf("DEBUG:FM %f FN %f\n", FM__(i,j), FN__(i,j));
			}

			//非線形項の考慮
			if ( soltype >= 2 )
			{
				double kk_scm = 1.0;
				unsigned char gap = 0;
				//if ( 
				//	GAP(i+3, j-3) & GAP_1 || GAP(i+3, j-2) & GAP_1 || GAP(i+3, j-1) & GAP_1 || GAP(i+3, j) & GAP_1 || GAP(i+3, j+1) & GAP_1 || GAP(i+3, j+2) & GAP_1 || GAP(i+3, j+3) & GAP_1 ||
				//	GAP(i+2, j-3) & GAP_1 || GAP(i+2, j-2) & GAP_1 || GAP(i+2, j-1) & GAP_1 || GAP(i+2, j) & GAP_1 || GAP(i+2, j+1) & GAP_1 || GAP(i+2, j+2) & GAP_1 || GAP(i+2, j+3) & GAP_1 ||
				//	GAP(i+1, j-3) & GAP_1 || GAP(i+1, j-2) & GAP_1 || GAP(i+1, j-1) & GAP_1 || GAP(i+1, j) & GAP_1 || GAP(i+1, j+1) & GAP_1 || GAP(i+1, j+2) & GAP_1 || GAP(i+1, j+3) & GAP_1 ||
				//	GAP(i  , j-3) & GAP_1 || GAP(i  , j-2) & GAP_1 || GAP(i  , j-1) & GAP_1 || GAP(i  , j) & GAP_1 || GAP(i  , j+1) & GAP_1 || GAP(i  , j+2) & GAP_1 || GAP(i  , j+3) & GAP_1 ||
				//	GAP(i-1, j-3) & GAP_1 || GAP(i-1, j-2) & GAP_1 || GAP(i-1, j-1) & GAP_1 || GAP(i-1, j) & GAP_1 || GAP(i-1, j+1) & GAP_1 || GAP(i-1, j+2) & GAP_1 || GAP(i-1, j+3) & GAP_1 ||
				//	GAP(i-2, j-3) & GAP_1 || GAP(i-2, j-2) & GAP_1 || GAP(i-2, j-1) & GAP_1 || GAP(i-2, j) & GAP_1 || GAP(i-2, j+1) & GAP_1 || GAP(i-2, j+2) & GAP_1 || GAP(i-2, j+3) & GAP_1 ||
				//	GAP(i-3, j-3) & GAP_1 || GAP(i-3, j-2) & GAP_1 || GAP(i-3, j-1) & GAP_1 || GAP(i-3, j) & GAP_1 || GAP(i-3, j+1) & GAP_1 || GAP(i-3, j+2) & GAP_1 || GAP(i-3, j+3) & GAP_1 )
				//{
				//	gap = true;
				//}

				//if ( 
				//	GAP(i+2, j-2) & GAP_1 || GAP(i+2, j-1) & GAP_1 || GAP(i+2, j) & GAP_1 || GAP(i+2, j+1) & GAP_1 || GAP(i+2, j+2) & GAP_1 ||
				//	GAP(i+1, j-2) & GAP_1 || GAP(i+1, j-1) & GAP_1 || GAP(i+1, j) & GAP_1 || GAP(i+1, j+1) & GAP_1 || GAP(i+1, j+2) & GAP_1 ||
				//	GAP(i  , j-2) & GAP_1 || GAP(i  , j-1) & GAP_1 || GAP(i  , j) & GAP_1 || GAP(i  , j+1) & GAP_1 || GAP(i  , j+2) & GAP_1 ||
				//	GAP(i-1, j-2) & GAP_1 || GAP(i-1, j-1) & GAP_1 || GAP(i-1, j) & GAP_1 || GAP(i-1, j+1) & GAP_1 || GAP(i-1, j+2) & GAP_1 ||
				//	GAP(i-2, j-2) & GAP_1 || GAP(i-2, j-1) & GAP_1 || GAP(i-2, j) & GAP_1 || GAP(i-2, j+1) & GAP_1 || GAP(i-2, j+2) & GAP_1 )
				//{
				//	gap = true;
				//}

				//if ( 
				//	GAP(i+1, j-1) & GAP_1 || GAP(i+1, j) & GAP_1 || GAP(i+1, j+1) & GAP_1 ||
				//	GAP(i  , j-1) & GAP_1 || GAP(i  , j) & GAP_1 || GAP(i  , j+1) & GAP_1 ||
				//	GAP(i-1, j-1) & GAP_1 || GAP(i-1, j) & GAP_1 || GAP(i-1, j+1) & GAP_1 )
				//{
				//	gap = true;
				//}

				if ( GAP(i,j) & (GAP_1|GAP_4) )
				{
					gap = GAP(i,j);
				}
				//if ( 
				//	T__(i+3, j-3) != 0 && T__(i+3, j-2) != 0 && T__(i+3, j-1) != 0 && T__(i+3, j) != 0 && T__(i+3, j+1) != 0 && T__(i+3, j+2) != 0 && T__(i+3, j+3) != 0 &&
				//	T__(i+2, j-3) != 0 && T__(i+2, j-2) != 0 && T__(i+2, j-1) != 0 && T__(i+2, j) != 0 && T__(i+2, j+1) != 0 && T__(i+2, j+2) != 0 && T__(i+2, j+3) != 0 &&
				//	T__(i+1, j-3) != 0 && T__(i+1, j-2) != 0 && T__(i+1, j-1) != 0 && T__(i+1, j) != 0 && T__(i+1, j+1) != 0 && T__(i+1, j+2) != 0 && T__(i+1, j+3) != 0 &&
				//	T__(i  , j-3) != 0 && T__(i  , j-2) != 0 && T__(i  , j-1) != 0 && T__(i  , j) != 0 && T__(i  , j+1) != 0 && T__(i  , j+2) != 0 && T__(i  , j+3) != 0 &&
				//	T__(i-1, j-3) != 0 && T__(i-1, j-2) != 0 && T__(i-1, j-1) != 0 && T__(i-1, j) != 0 && T__(i-1, j+1) != 0 && T__(i-1, j+2) != 0 && T__(i-1, j+3) != 0 &&
				//	T__(i-2, j-3) != 0 && T__(i-2, j-2) != 0 && T__(i-2, j-1) != 0 && T__(i-2, j) != 0 && T__(i-2, j+1) != 0 && T__(i-2, j+2) != 0 && T__(i-2, j+3) != 0 &&
				//	T__(i-3, j-3) != 0 && T__(i-3, j-2) != 0 && T__(i-3, j-1) != 0 && T__(i-3, j) != 0 && T__(i-3, j+1) != 0 && T__(i-3, j+2) != 0 && T__(i-3, j+3) != 0 )
				//{
				//	/* empty*/;
				//}else
				//{
				//	gap = true;
				//}

				//非線形項の計算
				double ddu1 = 0.0;
				double ddv1 = 0.0;
				double ddu2 = 0.0;
				double ddv2 = 0.0;

				double dd1 = 0.0;
				double dd2 = 0.0;

				double RM = 0.0, RN = 0.0;

				double coef = 1.0;
				//Dがゼロまたはある下限値より小さくなった場合には，その全水深を分母として持つ項のみを省略(小谷ら(1998))
				if ( !IsAbsorbingZoneBounray(i,j) &&  d2 >= D_UNDER_LIMIT )
				{

					int use_upwindow_method_wrk = use_upwindow_method;

					kk_scm = 1.0;
					//use_upwindow_method_wrk = 1;
					if ( use_upwindow_method_wrk != 1 )
					{
						double d_max = -9999999999.0;
						double d_min =  9999999999.0;
						double d_gap_wrk;
						//汀線境界と陸地では振動しやすいので
						bool water_area = true;


						for (int kki = -3; kki <= 3; kki++)
						{
							for (int kkj = -3; kkj <= 3; kkj++)
							{
								d_gap_wrk = D2__(i + kki, j + kkj);
								if (d_max < d_gap_wrk) d_max = d_gap_wrk;
								if (d_min > d_gap_wrk) d_min = d_gap_wrk;
								if (T(i + kki, j + kkj) == 0) d_min = 0.0;

								if (water_area )
								{
									water_area = water_area && (T(i + kki, j + kkj) != 0);
									water_area = water_area && (H(i + kki, j + kkj) > 0.0);
								}
							}
						}

						if (fabs(d_max - d_min)  > 0.1*dx || fabs(d_max - d_min) > 0.1 *dy|| d_min < 0.3)
						{
							kk_scm = 1.0;
							use_upwindow_method_wrk = 1;
							//printf("===================\n");
						}
						else
						{
							//printf("===================\n");
						}
				
						//if (fabs(d_max - d_min) > 50.0 && d_min < 5.0)
						if (fabs(d_max - d_min) > steep_slope1[1] && d_min < steep_slope1[0]/* || fabs(d_max - d_min) > 50.0*/)
						{
							double tt = fabs(d_max - d_min) / 100.0;
							debug_marker[iX*i + j] = tt;
							{
								kk_scm = 1.0;
								use_upwindow_method_wrk = 1;
							}
							GAP(i, j) |= GAP_7;
						}

						//周辺含めて水域にはなって居ない＝＞陸地境界が含まれる。
						if (!water_area)
						{
							kk_scm = 1.0;
							use_upwindow_method_wrk = 1;
						}
					}

					const double A = C7_03;

					double MA = MAN__(i,j);

					double R = 0.0;
					double d1 = d2;
					if (  DEBUG_LOG(i,j) )
					{
						log_printf("DEBUG:d2 %f MA %f\n", d2, MA);
					}

					//摩擦項（式(3.2）の第5）に含まれている水深は10-2 より小さければ、それを10-2 で置き換える。
					//宮古湾における津波防災対策検討調査業務委託報 告 書
					double min_limit = 0.01;
					if (min_limit < Dmin) min_limit = Dmin;

#if 10
					if (T__(i-1,j-1)==0||T__(i,j-1)==0||T__(i+1,j-1)==0||
						T__(i-1,j  )==0||T__(i,j  )==0||T__(i+1,j  )==0||
						T__(i-1,j+1)==0||T__(i,j+1)==0||T__(i+1,j+1)==0)
					{
						min_limit = 0.1;
						//kk_scm = 1.0;
						//use_upwindow_method_wrk = 1;
					}
#endif
					//min_limit = 0.1;


					//if ( gap & GAP_1 )
					//{
					//	use_upwindow_method_wrk = 1;
					//}
					//if ( gap & GAP_4 ) coef = 0.7;
					//if ( gap & GAP_1 ) coef = 0.2;

					//double depth = ADVECTION_TERM_DEPTH;
					//if ( 
					//	H__(i+2, j-2) > depth || H__(i+2, j-1) > depth || H__(i+2, j) > depth || H__(i+2, j+1) > depth || H__(i+2, j+2) > depth ||
					//	H__(i+1, j-2) > depth || H__(i+1, j-1) > depth || H__(i+1, j) > depth || H__(i+1, j+1) > depth || H__(i+1, j+2) > depth ||
					//	H__(i  , j-2) > depth || H__(i  , j-1) > depth || H__(i  , j) > depth || H__(i  , j+1) > depth || H__(i  , j+2) > depth ||
					//	H__(i-1, j-2) > depth || H__(i-1, j-1) > depth || H__(i-1, j) > depth || H__(i-1, j+1) > depth || H__(i-1, j+2) > depth ||
					//	H__(i-2, j-2) > depth || H__(i-2, j-1) > depth || H__(i-2, j) > depth || H__(i-2, j+1) > depth || H__(i-2, j+2) > depth )
					//{
					//	//if ( gap /*& GAP_1*/ ) coef = 0.1;
					//}


					//移流項
					double mm[7];
					double dx[6];
					double dy[6];
					
					if ( use_upwindow_method_wrk == 0 || use_upwindow_method_wrk == 1 )
					{
						dx[1] = D2__(i,j-1);
						dx[2] = D2__(i,j+1);
						dx[3] = D2__(i,j);

						dy[1] = D2__(i-1,j);
						dy[2] = D2__(i+1,j);
						dy[3] = D2__(i,j);

						//dx[1] = ((fabs(dx[1])<dmin)?dmin:dx[1]);
						//dx[1] = (dx[1]<dmin && dx[1]>-dmin)?dmin:dx[1];
						//dx[2] = (dx[2]<dmin && dx[2]>-dmin)?dmin:dx[2];
						//dx[3] = (dx[3]<dmin && dx[3]>-dmin)?dmin:dx[3];

						//dy[1] = (dy[1]<dmin && dy[1]>-dmin)?dmin:dy[1];
						//dy[2] = (dy[2]<dmin && dy[2]>-dmin)?dmin:dy[2];
						//dy[3] = (dy[3]<dmin && dy[3]>-dmin)?dmin:dy[3];
					}else	
					if ( use_upwindow_method_wrk == 3 )
					{
						dx[1] = D2__(i,j-2);
						dx[2] = D2__(i,j-1);
						dx[3] = D2__(i,j+1);
						dx[4] = D2__(i,j+2);
						dx[5] = D2__(i,j);

						dy[1] = D2__(i-2,j);
						dy[2] = D2__(i-1,j);
						dy[3] = D2__(i+1,j);
						dy[4] = D2__(i+2,j);
						dy[5] = D2__(i,j);

						//dx[1] = (dx[1]<dmin && dx[1]>-dmin)?dmin:dx[1];
						//dx[2] = (dx[2]<dmin && dx[2]>-dmin)?dmin:dx[2];
						//dx[3] = (dx[3]<dmin && dx[3]>-dmin)?dmin:dx[3];
						//dx[4] = (dx[4]<dmin && dx[4]>-dmin)?dmin:dx[4];
						//dx[5] = (dx[5]<dmin && dx[5]>-dmin)?dmin:dx[5];

						//dy[1] = (dy[1]<dmin && dy[1]>-dmin)?dmin:dy[1];
						//dy[2] = (dy[2]<dmin && dy[2]>-dmin)?dmin:dy[2];
						//dy[3] = (dy[3]<dmin && dy[3]>-dmin)?dmin:dy[3];
						//dy[4] = (dy[4]<dmin && dy[4]>-dmin)?dmin:dy[4];
						//dy[5] = (dy[5]<dmin && dy[5]>-dmin)?dmin:dy[5];

						//if (gap & GAP_4 ) kk_scm = 1.1;
					}

					if (d1 < min_limit) d1 = min_limit;

					double DD = pow(d1, A);

					R = g_*MA*MA*sqrt(MM*MM + NN*NN) / DD;

					//摩擦項
					RM = R*MM;
					RN = R*NN;

					if (DEBUG_LOG(i, j))
					{
						log_printf("DEBUG:sqrt(MM*MM + NN*NN) %f 1/pow(d1, A) %f\n", sqrt(MM*MM + NN*NN), 1.0 / pow(d1, A));
						log_printf("DEBUG:d1 %f R %f 摩擦項 %f %f\n", d1, R, RM, RN);
					}

					eval_advection_terms++;
					double advection_term;

					int skipp = 0;
					if ( use_upwindow_method_wrk == 3)
					{
						mm[0] =  (fabs(dx[1]) > min_limit )?M__(i,j-2)/dx[1]:0;
						mm[1] =  (fabs(dx[2]) > min_limit )?M__(i,j-1)/dx[2]:0;
						mm[2] =  (fabs(dx[3]) > min_limit )?M__(i,j+1)/dx[3]:0;
						mm[3] =  (fabs(dx[4]) > min_limit )?M__(i,j+2)/dx[4]:0;
						mm[5] =  (fabs(dx[5]) > min_limit )?M__(i,j  )/dx[5]:0;
						mm[6] =  M__(i,j  );
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term =  UPWIND3(mm,kk_scm);
						mm[0] =  M__(i,j-2);
						mm[1] =  M__(i,j-1);
						mm[2] =  M__(i,j+1);
						mm[3] =  M__(i,j+2);
						mm[5] =  M__(i,j  );
						mm[6] =  (fabs(dx[5]) > min_limit )?M__(i,j  )/dx[5]:0;
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term += UPWIND3(mm,kk_scm);
					}else
					if ( use_upwindow_method_wrk == 1)
					{
						mm[0] =  (fabs(dx[1]) > min_limit )?M__(i,j-1)/dx[1]:0;
						mm[1] =  (fabs(dx[2]) > min_limit )?M__(i,j+1)/dx[2]:0;
						mm[3] =  (fabs(dx[3]) > min_limit )?M__(i,j  )/dx[3]:0;
						mm[4] =  M__(i,j  );
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term =  UPWIND1(mm,kk_scm);
						mm[0] =  M__(i,j-1);
						mm[1] =  M__(i,j+1);
						mm[3] =  M__(i,j  );
						mm[4] =  (fabs(dx[3]) > min_limit )?M__(i,j  )/dx[3]:0;
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term +=  UPWIND1(mm,kk_scm);
					}else
					{
						mm[0] =  M__(i,j-1)* M__(i,j-1)/dx[1];
						mm[1] =  M__(i,j+1)* M__(i,j+1)/dx[2];
						advection_term =  DIFF(mm);
					}
					ddu1 = advection_term*r_dx;

					//if ( (dx[1] < 100.0 || dx[2] < 100.0 ) && fabs(advection_term) > 0.001 && fabs(local_termM+advection_term) > fabs(local_termM)*ADVECTION_TERM_LIMIT  )
					//{
					//	GAP(i,j) |= GAP_2;
					//	skipp++;
					//}
					//if (  DEBUG_LOG(i,j) )
					//{
					//	log_printf("DEBUG:ddu1 mm[0]%f mm[1]%f dx[1] %f dx[2] %f\n", mm[0], mm[1], dx[1], dx[2]);
					//}

					if ( use_upwindow_method_wrk == 3)
					{
						mm[0] =  (fabs(dy[1]) > min_limit )?M__(i-2,j)/dy[1]:0;
						mm[1] =  (fabs(dy[2]) > min_limit )?M__(i-1,j)/dy[2]:0;
						mm[2] =  (fabs(dy[3]) > min_limit )?M__(i+1,j)/dy[3]:0;
						mm[3] =  (fabs(dy[4]) > min_limit )?M__(i+2,j)/dy[4]:0;
						mm[5] =  (fabs(dy[5]) > min_limit )?M__(i,j  )/dy[5]:0;
						mm[6] =  N__(i,j  );
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term =  UPWIND3(mm,kk_scm);
						mm[0] =  N__(i-2,j);
						mm[1] =  N__(i-1,j);
						mm[2] =  N__(i+1,j);
						mm[3] =  N__(i+2,j);
						mm[5] =  N__(i,j  );
						mm[6] =  (fabs(dy[5]) > min_limit )?M__(i,j  )/dy[5]:0;
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term +=  UPWIND3(mm,kk_scm);
					}else
					if ( use_upwindow_method_wrk == 1)
					{
						mm[0] =  (fabs(dy[1]) > min_limit )?M__(i-1,j)/dy[1]:0;
						mm[1] =  (fabs(dy[2]) > min_limit )?M__(i+1,j)/dy[2]:0;
						mm[3] =  (fabs(dy[3]) > min_limit )?M__(i  ,j)/dy[3]:0;
						mm[4] =  N__(i  ,j);
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term =  UPWIND1(mm,kk_scm);
						mm[0] =  N__(i-1,j);
						mm[1] =  N__(i+1,j);
						mm[3] =  N__(i  ,j);
						mm[4] =  (fabs(dy[3]) > min_limit )?M__(i  ,j)/dy[3]:0;
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term +=  UPWIND1(mm,kk_scm);
					}else
					{
						mm[0] =  M__(i-1,j)* N__(i-1,j)/dy[1];
						mm[1] =  M__(i+1,j)* N__(i+1,j)/dy[2];
						advection_term =  DIFF(mm);
					}
					ddv1 = advection_term*r_dy;

					//if ( (dy[1] < 100.0 || dy[2] < 100.0 ) && fabs(advection_term) > 0.001 && fabs(local_termM+advection_term) > fabs(local_termM)*ADVECTION_TERM_LIMIT )
					//{
					//	GAP(i,j) |= GAP_2;
					//	skipp++;
					//}
					//if (  DEBUG_LOG(i,j) )
					//{
					//	log_printf("DEBUG:ddv1 mm[0]%f mm[1]%f dx[1] %f dx[2] %f\n", mm[0], mm[1], dy[1], dy[2]);
					//}


					if ( use_upwindow_method_wrk == 3)
					{
						mm[0] =  (fabs(dx[1]) > min_limit )?M__(i,j-2)/dx[1]:0;
						mm[1] =  (fabs(dx[2]) > min_limit )?M__(i,j-1)/dx[2]:0;
						mm[2] =  (fabs(dx[3]) > min_limit )?M__(i,j+1)/dx[3]:0;
						mm[3] =  (fabs(dx[4]) > min_limit )?M__(i,j+2)/dx[4]:0;
						mm[5] =  (fabs(dx[5]) > min_limit )?M__(i,j  )/dx[5]:0;
						mm[6] =  N__(i,j  );
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term =  UPWIND3(mm,kk_scm);
						mm[0] =  N__(i,j-2);
						mm[1] =  N__(i,j-1);
						mm[2] =  N__(i,j+1);
						mm[3] =  N__(i,j+2);
						mm[5] =  N__(i,j  );
						mm[6] =  (fabs(dx[5]) > min_limit )?M__(i,j  )/dx[5]:0;
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term +=  UPWIND3(mm,kk_scm);
					}else
					if ( use_upwindow_method_wrk == 1)
					{
						mm[0] =  (fabs(dx[1]) > min_limit )?M__(i,j-1)/dx[1]:0;
						mm[1] =  (fabs(dx[2]) > min_limit )?M__(i,j+1)/dx[2]:0;
						mm[3] =  (fabs(dx[3]) > min_limit )?M__(i,j  )/dx[3]:0;
						mm[4] =  N__(i,j  );
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term =  UPWIND1(mm,kk_scm);
						mm[0] =  N__(i,j-1);
						mm[1] =  N__(i,j+1);
						mm[3] =  N__(i,j  );
						mm[4] =  (fabs(dx[3]) > min_limit )?M__(i,j  )/dx[3]:0;
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term +=  UPWIND1(mm,kk_scm);
					}else
					{
						mm[0] =  M__(i,j-1)* N__(i,j-1)/dx[1];
						mm[1] =  M__(i,j+1)* N__(i,j+1)/dx[2];
						advection_term =  DIFF(mm);
					}
					ddu2 = advection_term*r_dx;

					//if ( (dx[1] < 100.0 || dx[2] < 100.0 ) && fabs(advection_term) > 0.001 && fabs(local_termN+advection_term) > fabs(local_termN)*ADVECTION_TERM_LIMIT )
					//{
					//	GAP(i,j) |= GAP_2;
					//	skipp++;
					//}
					//if (  DEBUG_LOG(i,j) )
					//{
					//	log_printf("DEBUG:ddu2 mm[0]%f mm[1]%f dx[1] %f dx[2] %f\n", mm[0], mm[1], dx[1], dx[2]);
					//}

					if ( use_upwindow_method_wrk == 3 )
					{
						mm[0] =  (fabs(dy[1]) > min_limit )?N__(i-2,j)/dy[1]:0;
						mm[1] =  (fabs(dy[2]) > min_limit )?N__(i-1,j)/dy[2]:0;
						mm[2] =  (fabs(dy[3]) > min_limit )?N__(i+1,j)/dy[3]:0;
						mm[3] =  (fabs(dy[4]) > min_limit )?N__(i+2,j)/dy[4]:0;
						mm[5] =  (fabs(dy[5]) > min_limit )?N__(i,j  )/dy[5]:0;
						mm[6] =  N__(i,j  );
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term =  UPWIND3(mm,kk_scm);
						mm[0] =  N__(i-2,j);
						mm[1] =  N__(i-1,j);
						mm[2] =  N__(i+1,j);
						mm[3] =  N__(i+2,j);
						mm[5] =  N__(i,j  );
						mm[6] =  (fabs(dy[5]) > min_limit )?N__(i,j  )/dy[5]:0;
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term =  UPWIND3(mm,kk_scm);
					}else
					if ( use_upwindow_method_wrk == 1 )
					{
						mm[0] =  (fabs(dy[1]) > min_limit )?N__(i-1,j)/dy[1]:0;
						mm[1] =  (fabs(dy[2]) > min_limit )?N__(i+1,j)/dy[2]:0;
						mm[3] =  (fabs(dy[3]) > min_limit )?N__(i  ,j)/dy[3]:0;
						mm[4] =  N__(i  ,j);
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term =  UPWIND1(mm,kk_scm);
						mm[0] =  N__(i-1,j);
						mm[1] =  N__(i+1,j);
						mm[3] =  N__(i  ,j);
						mm[4] =  (fabs(dy[3]) > min_limit )?N__(i  ,j)/dy[3]:0;
						advection_term_coef(i,j,mm,use_upwindow_method_wrk);
						advection_term +=  UPWIND1(mm,kk_scm);
					}else
					{
						mm[0] =  N__(i-1,j)* N__(i-1,j)/dy[1];
						mm[1] =  N__(i+1,j)* N__(i+1,j)/dy[2];
						advection_term =  DIFF(mm);
					}
					ddv2 = advection_term*r_dy;

					//if ( (dy[1] < 100.0 || dy[2] < 100.0 ) && fabs(advection_term) > 0.001 && fabs(local_termN+advection_term) > fabs(local_termN)*ADVECTION_TERM_LIMIT )
					//{
					//	GAP(i,j) |= GAP_2;
					//	skipp++;
					//}
					//if (  DEBUG_LOG(i,j) )
					//{
					//	log_printf("DEBUG:ddv2 mm[0]%f mm[1]%f dx[1] %f dx[2] %f\n", mm[0], mm[1], dy[1], dy[2]);
					//}
					//if ( skipp ) skipp_advection_terms++;

					if (  DEBUG_LOG(i,j) )
					{
						log_printf("DEBUG:ddu1 %f ddv1 %f ddu2 %f ddv2 %f\n", ddu1, ddv1, ddu2, ddv2);
					}
				}

				//運動方程式に
				FM__(i, j) += -coef*(ddu1 + ddv1) - coef*RM;
				FN__(i, j) += -coef*(ddu2 + ddv2) - coef*RN;

				if (  DEBUG_LOG(i,j) )
				{
					log_printf("DEBUG:FM %f FN %f\n", FM__(i,j), FN__(i,j));
				}
			}
			//コリオリ因子の考慮
			if ( coriolis )
			{
				const double co = coriolisFactor[i*IX+j];

				FM__(i,j) +=  co*NN;
				FN__(i,j) +=  -co*MM;
				if (  DEBUG_LOG(i,j) )
				{
					log_printf("DEBUG: コリオリ因子の考慮 FM %f FN %f\n", FM__(i,j), FN__(i,j));
				}
			}
			if ( Storm_p)
			{
				//せん断力と吸い上げ効果の計算
				double d = 0.25*(D2__(i-1,j)+D2__(i+1,j)+D2__(i,j-1)+D2__(i,j+1));
				if ( d > 0.01 )
				{
					FM__(i,j) +=  shearForceX[i*iX+j];
					FN__(i,j) +=  shearForceY[i*iX+j];
				}
			}
		}
	}
	//if ( (double)skipp_advection_terms/(double)(eval_advection_terms) > 0.05 )
	//{
	//	log_printf("Warning:abnormal advection term:%.2f%%\n", 100.0*(double)skipp_advection_terms/(double)(eval_advection_terms)); 
	//}

	//goto end;
	if (!soliton || soltype < 3 || w_init == 1)
	{
		//log_printf("soliton %d soltype %d w_init %d\n", soliton, soltype, w_init);
		goto end;
	}

	if ( tdiff.Fm == 0 )
	{
		//log_printf("tdiff.Fm == 0\n");
		goto end;
	}

	//補正係数(Boussinesq dipersion coefficient)
	double B = 1.0/15.0;
	

	//Peregrine -> 1
	int Peregrine = 0;
	if ( soltype == 4 )
	{
		Peregrine = 1;
	}

	if ( Peregrine )
	{
		B = 0.0;
	}

	if (  DEBUG_LOG(debug_index_i,debug_index_j) )
	{
		log_printf("\n\n非線形分散項計算\n");
	}

#if 10
	const int bw = 5;
	if ( !Breaking_factor )
	{
		Breaking_factor = new float[Data->iX*Data->jY];
	}
	if ( Breaking_point == 0 )
	{
		Breaking_point = new char[Data->iX*Data->jY];
		memset(Breaking_point, '\0', sizeof(char)*Data->iX*Data->jY);
	}
	memset(Breaking_factor, '\0', sizeof(float)*Data->iX*Data->jY);


	//砕波・波峰を中心に渦動粘性係数として減衰パラメータを空間分布を与える
	float* Breaking_factorDistr = new float[Data->iX*Data->jY];
	memset(Breaking_factorDistr, '\0', sizeof(float)*Data->iX*Data->jY);
	

	//砕波・波峰を中心
#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			char* breaking_factor = &(Breaking_point[i*IX+j]);

			const double h = H2__(i,j);

			if ( h < 0.000001 )
			{
				*breaking_factor = 0;
				continue;
			}

			bool top = false;
			//波高水深比(の近似値・・・本来は振幅)
			double eps = Amplitude(bw, i,j, top)/h;

			//砕波開始
#if 0
			if ( eps > 0.827 && h > 0.001 && top)
			{
				*breaking_factor = 1;
				//log_printf("砕波・波峰を中心\n");
			}
#else
			if ( Water_particle_velocity(i,j) > SQR(0.59) )
			{
				*breaking_factor = 1;
			}
#endif
			//砕波中
			if ( *breaking_factor == 1 && eps > 0.55)
			{
				*breaking_factor = 1;
			}
			//砕波終了
			if ( *breaking_factor == 1 && eps < 0.55 && h > 0.001)
			{
				*breaking_factor = 0;
			}
		}
	}

	const int bw2 = 10;
	const float cdist = sqrtf(2.0f)*(float)bw;
#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{


			//砕波・波峰を中心に空間分布を与える
			if ( Breaking_point[i*IX+j] == 1 )
			{
#pragma omp critical
				{
					for ( int ki = -bw2; ki <= bw2; ki++ )
					{
						for ( int kj = -bw2; kj <= bw2; kj++ )
						{
							if ( i+ki < 0 || i+ki >= JY || j+kj < 0 || j+kj >= IX )
							{
								continue;
							}
							const double h = D2__(i+ki,j+kj);
							const double w = W__(i+ki,j+kj);
							if ( Breaking_point[(i+ki)*IX+(j+kj)] == 2 || h < 0.0 || w < 0.0)
							{
								continue;
							}

							float sdist = sqrtf(SQR( ki) + SQR( kj));
							//0.37 => [砕波形態を考慮した津波の砕波減衰モデル]
							double nu_b = 0.37*sqrtf(g*h)*w;

							Breaking_factor[(i+ki)*IX+(j+kj)] += nu_b*(1.0f - sdist/cdist);
							Breaking_factorDistr[(i+ki)*IX+(j+kj)] += 1.0f;
						}
					}
				}
			}
		}
	}

	//渦動粘性係数の重複加算を平均化する。
#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			if ( Breaking_factorDistr[i*IX+j] > 1.0f )
			{
				Breaking_factor[i*IX+j] /= Breaking_factorDistr[i*IX+j];
			}
			if ( Breaking_factor[i*IX+j] <= 0.0 )
			{
				Breaking_factor[i*IX+j] = 0.0;
				if ( nu < 0.0 )
				{
					Breaking_factor[i*IX+j] = -nu;
				}
			}
		}
	}
	delete [] Breaking_factorDistr;
#endif
	

	const double ds = (delta_x < delta_y) ? delta_x:delta_y;

	//リチャードソン則
	double hv = 0.01*pow( (delta_x > delta_y) ? delta_x:delta_y, C4_03);

	int skipp_dispersion_terms = 0;
	int eval_dispersion_terms = 0;

#pragma omp parallel for OMP_SCHEDULE  /*reduction(+:skipp_dispersion_terms) reduction(+:eval_dispersion_terms)*/
	for (int ii = 0; ii < iX*jY; ii++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii / iX;
		int j = ii % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		const double r_dx = R_DX(i);	//1.0/delta_x;
		const double r_dxx = r_dx*r_dx;
		const double r_dxy = r_dx*r_dy;
		const double r_dxxx = r_dx*r_dx*r_dx;
		const double r_dxxy = r_dx*r_dx*r_dy;
		const double r_dxyy = r_dx*r_dy*r_dy;

		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			bool water_area = false;
			//陸上と水深が負(海域だが露出している=陸地))の場所は計算できない
			//差分計算のため参照位置も含む
			if ( 
				T__(i+3, j-3) != 0 && T__(i+3, j-2) != 0 && T__(i+3, j-1) != 0 && T__(i+3, j) != 0 && T__(i+3, j+1) != 0 && T__(i+3, j+2) != 0 && T__(i+3, j+3) != 0 &&
				T__(i+2, j-3) != 0 && T__(i+2, j-2) != 0 && T__(i+2, j-1) != 0 && T__(i+2, j) != 0 && T__(i+2, j+1) != 0 && T__(i+2, j+2) != 0 && T__(i+2, j+3) != 0 &&
				T__(i+1, j-3) != 0 && T__(i+1, j-2) != 0 && T__(i+1, j-1) != 0 && T__(i+1, j) != 0 && T__(i+1, j+1) != 0 && T__(i+1, j+2) != 0 && T__(i+1, j+3) != 0 &&
				T__(i  , j-3) != 0 && T__(i  , j-2) != 0 && T__(i  , j-1) != 0 && T__(i  , j) != 0 && T__(i  , j+1) != 0 && T__(i  , j+2) != 0 && T__(i  , j+3) != 0 &&
				T__(i-1, j-3) != 0 && T__(i-1, j-2) != 0 && T__(i-1, j-1) != 0 && T__(i-1, j) != 0 && T__(i-1, j+1) != 0 && T__(i-1, j+2) != 0 && T__(i-1, j+3) != 0 &&
				T__(i-2, j-3) != 0 && T__(i-2, j-2) != 0 && T__(i-2, j-1) != 0 && T__(i-2, j) != 0 && T__(i-2, j+1) != 0 && T__(i-2, j+2) != 0 && T__(i-2, j+3) != 0 &&
				T__(i-3, j-3) != 0 && T__(i-3, j-2) != 0 && T__(i-3, j-1) != 0 && T__(i-3, j) != 0 && T__(i-3, j+1) != 0 && T__(i-3, j+2) != 0 && T__(i-3, j+3) != 0 )
			{
				/* empty*/;
				water_area = true;
			}else
			{
				//continue;
			}
			if ( 
				H__(i+3, j-3) > 0.05 && H__(i+3, j-2) > 0.05 && H__(i+3, j-1) > 0.05 && H__(i+3, j) > 0.05 && H__(i+3, j+1) > 0.05 && H__(i+3, j+2) > 0.05 && H__(i+3, j+3) > 0.05 &&
				H__(i+2, j-3) > 0.05 && H__(i+2, j-2) > 0.05 && H__(i+2, j-1) > 0.05 && H__(i+2, j) > 0.05 && H__(i+2, j+1) > 0.05 && H__(i+2, j+2) > 0.05 && H__(i+2, j+3) > 0.05 &&
				H__(i+1, j-3) > 0.05 && H__(i+1, j-2) > 0.05 && H__(i+1, j-1) > 0.05 && H__(i+1, j) > 0.05 && H__(i+1, j+1) > 0.05 && H__(i+1, j+2) > 0.05 && H__(i+1, j+3) > 0.05 &&
				H__(i  , j-3) > 0.05 && H__(i  , j-2) > 0.05 && H__(i  , j-1) > 0.05 && H__(i  , j) > 0.05 && H__(i  , j+1) > 0.05 && H__(i  , j+2) > 0.05 && H__(i  , j+3) > 0.05 &&
				H__(i-1, j-3) > 0.05 && H__(i-1, j-2) > 0.05 && H__(i-1, j-1) > 0.05 && H__(i-1, j) > 0.05 && H__(i-1, j+1) > 0.05 && H__(i-1, j+2) > 0.05 && H__(i-1, j+3) > 0.05 &&
				H__(i-2, j-3) > 0.05 && H__(i-2, j-2) > 0.05 && H__(i-2, j-1) > 0.05 && H__(i-2, j) > 0.05 && H__(i-2, j+1) > 0.05 && H__(i-2, j+2) > 0.05 && H__(i-2, j+3) > 0.05 &&
				H__(i-3, j-3) > 0.05 && H__(i-3, j-2) > 0.05 && H__(i-3, j-1) > 0.05 && H__(i-3, j) > 0.05 && H__(i-3, j+1) > 0.05 && H__(i-3, j+2) > 0.05 && H__(i-3, j+3) > 0.05 )
			{
				/* empty */;
				water_area = true;
			}else
			{
				//continue;
			}
			if ( 
				riverInfo.isRiver(i+3, j-3) && riverInfo.isRiver(i+3, j-2) && riverInfo.isRiver(i+3, j-1) && riverInfo.isRiver(i+3, j) && riverInfo.isRiver(i+3, j+1) && riverInfo.isRiver(i+3, j+2) && riverInfo.isRiver(i+3, j+3) &&
				riverInfo.isRiver(i+2, j-3) && riverInfo.isRiver(i+2, j-2) && riverInfo.isRiver(i+2, j-1) && riverInfo.isRiver(i+2, j) && riverInfo.isRiver(i+2, j+1) && riverInfo.isRiver(i+2, j+2) && riverInfo.isRiver(i+2, j+3) &&
				riverInfo.isRiver(i+1, j-3) && riverInfo.isRiver(i+1, j-2) && riverInfo.isRiver(i+1, j-1) && riverInfo.isRiver(i+1, j) && riverInfo.isRiver(i+1, j+1) && riverInfo.isRiver(i+1, j+2) && riverInfo.isRiver(i+1, j+3) &&
				riverInfo.isRiver(i  , j-3) && riverInfo.isRiver(i  , j-2) && riverInfo.isRiver(i  , j-1) && riverInfo.isRiver(i  , j) && riverInfo.isRiver(i  , j+1) && riverInfo.isRiver(i  , j+2) && riverInfo.isRiver(i  , j+3) &&
				riverInfo.isRiver(i-1, j-3) && riverInfo.isRiver(i-1, j-2) && riverInfo.isRiver(i-1, j-1) && riverInfo.isRiver(i-1, j) && riverInfo.isRiver(i-1, j+1) && riverInfo.isRiver(i-1, j+2) && riverInfo.isRiver(i-1, j+3) &&
				riverInfo.isRiver(i-2, j-3) && riverInfo.isRiver(i-2, j-2) && riverInfo.isRiver(i-2, j-1) && riverInfo.isRiver(i-2, j) && riverInfo.isRiver(i-2, j+1) && riverInfo.isRiver(i-2, j+2) && riverInfo.isRiver(i-2, j+3) &&
				riverInfo.isRiver(i-3, j-3) && riverInfo.isRiver(i-3, j-2) && riverInfo.isRiver(i-3, j-1) && riverInfo.isRiver(i-3, j) && riverInfo.isRiver(i-3, j+1) && riverInfo.isRiver(i-3, j+2) && riverInfo.isRiver(i-3, j+3) )
			{
				/* empty */;
				water_area = true;
			}else
			{
				//continue;
			}
			if ( 
				W__(i+3, j-3) > 0.1 && W__(i+3, j-2) > 0.1 && W__(i+3, j-1) > 0.1 && W__(i+3, j) > 0.1 && W__(i+3, j+1) > 0.1 && W__(i+3, j+2) > 0.1 && W__(i+3, j+3) > 0.1 &&
				W__(i+2, j-3) > 0.1 && W__(i+2, j-2) > 0.1 && W__(i+2, j-1) > 0.1 && W__(i+2, j) > 0.1 && W__(i+2, j+1) > 0.1 && W__(i+2, j+2) > 0.1 && W__(i+2, j+3) > 0.1 &&
				W__(i+1, j-3) > 0.1 && W__(i+1, j-2) > 0.1 && W__(i+1, j-1) > 0.1 && W__(i+1, j) > 0.1 && W__(i+1, j+1) > 0.1 && W__(i+1, j+2) > 0.1 && W__(i+1, j+3) > 0.1 &&
				W__(i  , j-3) > 0.1 && W__(i  , j-2) > 0.1 && W__(i  , j-1) > 0.1 && W__(i  , j) > 0.1 && W__(i  , j+1) > 0.1 && W__(i  , j+2) > 0.1 && W__(i  , j+3) > 0.1 &&
				W__(i-1, j-3) > 0.1 && W__(i-1, j-2) > 0.1 && W__(i-1, j-1) > 0.1 && W__(i-1, j) > 0.1 && W__(i-1, j+1) > 0.1 && W__(i-1, j+2) > 0.1 && W__(i-1, j+3) > 0.1 &&
				W__(i-2, j-3) > 0.1 && W__(i-2, j-2) > 0.1 && W__(i-2, j-1) > 0.1 && W__(i-2, j) > 0.1 && W__(i-2, j+1) > 0.1 && W__(i-2, j+2) > 0.1 && W__(i-2, j+3) > 0.1 &&
				W__(i-3, j-3) > 0.1 && W__(i-3, j-2) > 0.1 && W__(i-3, j-1) > 0.1 && W__(i-3, j) > 0.1 && W__(i-3, j+1) > 0.1 && W__(i-3, j+2) > 0.1 && W__(i-3, j+3) > 0.1 )
			{
				/* empty */;
				water_area = water_area && true;
			}else
			{
				//continue;
			}

			//const double r = 1.0;
			//if ( 
			//	D2__(i+3, j-3)/ds < r && D2__(i+3, j-2)/ds < r && D2__(i+3, j-1)/ds < r && D2__(i+3, j)/ds < r && D2__(i+3, j+1)/ds < r && D2__(i+3, j+2)/ds < r && D2__(i+3, j+3)/ds < r &&
			//	D2__(i+2, j-3)/ds < r && D2__(i+2, j-2)/ds < r && D2__(i+2, j-1)/ds < r && D2__(i+2, j)/ds < r && D2__(i+2, j+1)/ds < r && D2__(i+2, j+2)/ds < r && D2__(i+2, j+3)/ds < r &&
			//	D2__(i+1, j-3)/ds < r && D2__(i+1, j-2)/ds < r && D2__(i+1, j-1)/ds < r && D2__(i+1, j)/ds < r && D2__(i+1, j+1)/ds < r && D2__(i+1, j+2)/ds < r && D2__(i+1, j+3)/ds < r &&
			//	D2__(i  , j-3)/ds < r && D2__(i  , j-2)/ds < r && D2__(i  , j-1)/ds < r && D2__(i  , j)/ds < r && D2__(i  , j+1)/ds < r && D2__(i  , j+2)/ds < r && D2__(i  , j+3)/ds < r &&
			//	D2__(i-1, j-3)/ds < r && D2__(i-1, j-2)/ds < r && D2__(i-1, j-1)/ds < r && D2__(i-1, j)/ds < r && D2__(i-1, j+1)/ds < r && D2__(i-1, j+2)/ds < r && D2__(i-1, j+3)/ds < r &&
			//	D2__(i-2, j-3)/ds < r && D2__(i-2, j-2)/ds < r && D2__(i-2, j-1)/ds < r && D2__(i-2, j)/ds < r && D2__(i-2, j+1)/ds < r && D2__(i-2, j+2)/ds < r && D2__(i-2, j+3)/ds < r &&
			//	D2__(i-3, j-3)/ds < r && D2__(i-3, j-2)/ds < r && D2__(i-3, j-1)/ds < r && D2__(i-3, j)/ds < r && D2__(i-3, j+1)/ds < r && D2__(i-3, j+2)/ds < r && D2__(i-3, j+3)/ds < r )
			//{
			//	/* empty */;
			//	water_area = true;
			//}else
			//{
			//	continue;
			//}

			//bool gap = false;
			//for ( int ki = -6; ki <= 6; ki++ )
			//{
			//	if ( i+ki < 0 || i+ki >= JY ) continue;
			//	for ( int kj = -6; kj <= 6; kj++ )
			//	{
			//		if ( j+kj < 0 || j+kj >= IX ) continue;

			//		if ( GAP(i+ki, j+kj) & GAP_1 )
			//		{
			//			gap = true;
			//			break;
			//		}
			//	}
			//	if ( gap ) break;
			//}
			//if ( gap )
			//{
			//	continue;
			//}


			//if ( 
			//	GAP(i+3, j-3) & GAP_1 || GAP(i+3, j-2) & GAP_1 || GAP(i+3, j-1) & GAP_1 || GAP(i+3, j) & GAP_1 || GAP(i+3, j+1) & GAP_1 || GAP(i+3, j+2) & GAP_1 || GAP(i+3, j+3) & GAP_1 ||
			//	GAP(i+2, j-3) & GAP_1 || GAP(i+2, j-2) & GAP_1 || GAP(i+2, j-1) & GAP_1 || GAP(i+2, j) & GAP_1 || GAP(i+2, j+1) & GAP_1 || GAP(i+2, j+2) & GAP_1 || GAP(i+2, j+3) & GAP_1 ||
			//	GAP(i+1, j-3) & GAP_1 || GAP(i+1, j-2) & GAP_1 || GAP(i+1, j-1) & GAP_1 || GAP(i+1, j) & GAP_1 || GAP(i+1, j+1) & GAP_1 || GAP(i+1, j+2) & GAP_1 || GAP(i+1, j+3) & GAP_1 ||
			//	GAP(i  , j-3) & GAP_1 || GAP(i  , j-2) & GAP_1 || GAP(i  , j-1) & GAP_1 || GAP(i  , j) & GAP_1 || GAP(i  , j+1) & GAP_1 || GAP(i  , j+2) & GAP_1 || GAP(i  , j+3) & GAP_1 ||
			//	GAP(i-1, j-3) & GAP_1 || GAP(i-1, j-2) & GAP_1 || GAP(i-1, j-1) & GAP_1 || GAP(i-1, j) & GAP_1 || GAP(i-1, j+1) & GAP_1 || GAP(i-1, j+2) & GAP_1 || GAP(i-1, j+3) & GAP_1 ||
			//	GAP(i-2, j-3) & GAP_1 || GAP(i-2, j-2) & GAP_1 || GAP(i-2, j-1) & GAP_1 || GAP(i-2, j) & GAP_1 || GAP(i-2, j+1) & GAP_1 || GAP(i-2, j+2) & GAP_1 || GAP(i-2, j+3) & GAP_1 ||
			//	GAP(i-3, j-3) & GAP_1 || GAP(i-3, j-2) & GAP_1 || GAP(i-3, j-1) & GAP_1 || GAP(i-3, j) & GAP_1 || GAP(i-3, j+1) & GAP_1 || GAP(i-3, j+2) & GAP_1 || GAP(i-3, j+3) & GAP_1 )
			//{
			//	continue;
			//}


			//最低水深(0.05)未満では無視する
			//if ( 0.25*(H__(i-1,j)+H__(i+1,j)+H__(i,j-1)+H__(i,j+1)) < 0.05 )
			//{
			//	continue;
			//}


			////局所項（比較用で再計算）
			double local_termM = fabs(g_*D2__(i,j)*DIFFx(w__, i,j))*r_dx;
			double local_termN = fabs(g_*D2__(i,j)*DIFFy(w__, i,j))*r_dy;

			//double local_termM = fabs(FM__(i,j));
			//double local_termN = fabs(FN__(i,j));

			if ( fabs(local_termM) < 0.001 || fabs(local_termN) < 0.001)
			{
				continue;
			}

			if (  DEBUG_LOG(i,j) )
			{
				log_printf("\nDEBUG:r_dx:%f r_dy %f\n", r_dx, r_dy);
				log_printf("DEBUG:H2 %f\n",  H2__(i,j));
				log_printf("DEBUG:非線形項 %f %f\n\n", FM__(i,j), FN__(i,j));
			}

			int skipp = 0;

			//非線形分散項の計算
			double dd1 = 0.0;
			double dd2 = 0.0;


			//const double d2ij =  D2__(i,j);
			if ( !IsAbsorbingZoneBounray(i,j) )
			{
				//ソリトン分裂項の計算
				if ( soliton >= 3 )
				{
					//eval_dispersion_terms++;

					const double C1 = C1_03;
					const double C2 = C1_06;

					const double h = H2__(i,j);
					const double hh = SQR(h);

					const double dxxM = DIFFxx_2(m__, i, j)*r_dxx;
					const double dyyM = DIFFyy_2(m__, i, j)*r_dyy;
					const double dxxN = DIFFxx_2(n__, i, j)*r_dxx;
					const double dyyN = DIFFyy_2(n__, i, j)*r_dyy;

					const double dtxxM = (DIFFxx_2(tdiff.Fm, i, j))*r_dxx;
					const double dtxyM = (DIFFxy_2(tdiff.Fm, i, j))*r_dxy;

					const double dtyyN = (DIFFyy_2(tdiff.Fn, i, j))*r_dyy;
					const double dtxyN = (DIFFxy_2(tdiff.Fn, i, j))*r_dxy;

					const double dxxxW = DIFFxxx_3(w__,i,j)*r_dxxx;
					const double dxyyW = DIFFxyy_2(w__,i,j)*r_dxyy;
					const double dyyyW = DIFFyyy_3(w__,i,j)*r_dyyy;
					const double dxxyW = DIFFxxy_2(w__,i,j)*r_dxxy;


					//砕波項の計算					
					//波高水深比
					//double eps = fabs(Wh[i*IX+j].w)/H2__(i,j);
					double nu_b/* = nu_beta(i,j)*/;
					//nu_b = 0.0;
					//if ( eps > 0.83 && W__(i,j) > 0.001 && d2ij > 0.001 )
					//{
					//	nu_b = 0.37*sqrt(g*d2ij)*W__(i,j); 
					//	if (  DEBUG_LOG(i,j) )
					//	{
					//		log_printf("DEBUG:h2 %f W %f\n", h2, W__(i,j));
					//	}
					//}
					//if ( W__(i,j) < 0.0 ) nu_b = 0.0;

					nu_b = Breaking_factor[i*IX+j];
					//if ( W__(i,j) < 0.0 ) nu_b = 0.0;
					
					if ( nu_b > hv )
					{
						//printf("%f %f\n", nu_b, hv);
						nu_b = hv;
					}

					/*
					     dd1_1 = νb*(∂^2M/∂x^2 + ∂^2M/∂y^2)
					     dd2_1 = νb*(∂^2N/∂x^2 + ∂^2N/∂y^2)
					*/
					double dd1_1 = nu_b*(dxxM + dyyM);
					double dd2_1 = nu_b*(dxxN + dyyN);
					if (  DEBUG_LOG(i,j) )
					{
						//0.37*sqrtf(g*h)*w;
						log_printf("DEBUG:H %f W %f\n", H__(i,j), W__(i,j));
						log_printf("DEBUG:nu_b %f\n", nu_b);
						log_printf("DEBUG:砕波現象による減衰項 dd1_1 %f dd2_1 %f\n", dd1_1, dd2_1);
					}

					//Madsen-Sφrensen(1998) ( Boussinesq分散項)
					double d2 = (B+C1)*hh;
					double d3 = B*g_*hh*h;
					if (  DEBUG_LOG(i,j) )
					{
						log_printf("DEBUG:Boussinesq分散項 係数d2 %f d3 %f\n", d2, d3);
					}
					if (!water_area)
					{
						d2 = 0.0;
						d3 = 0.0;
					}

					/*
						dd1_0 = ∂^3M/∂x^2∂t + ∂^3N/∂x∂y∂t
						dd1_2 = ∂^3W/∂x^3 + ∂^3W/∂x∂y^2
					*/
					const double dd1_0 = dtxxM + dtxyN;
					const double dd1_2 = dxxxW + dxyyW;
					if (  DEBUG_LOG(i,j) )
					{
						log_printf("DEBUG:Boussinesq分散項 dd1_0(%f)=dtxxM(%f) + dtxyN(%f)\n", dd1_0, dtxxM, dtxyN);
						log_printf("DEBUG:Boussinesq分散項 dd1_2(%f)=dxxxW(%f) + dxyyW(%f)\n", dd1_2, dxxxW, dxyyW);
					}
					
					/*
						dd2_0 = ∂^3N/∂y^2∂t + ∂^3M/∂x∂y∂t
						dd2_2 = ∂^3W/∂y^3 + ∂^3W/∂x^2∂y
					*/
					const double dd2_0 = dtyyN + dtxyM;
					const double dd2_2 = dyyyW + dxxyW;

					if (  DEBUG_LOG(i,j) )
					{
						log_printf("DEBUG:Boussinesq分散項 dd2_0(%f)=dtyyN(%f) + dtxyM(%f)\n", dd2_0, dtyyN, dtxyM);
						log_printf("DEBUG:Boussinesq分散項 dd2_2(%f)=dyyyW(%f) + dxxyW(%f)\n", dd2_2, dyyyW, dxxyW);
					}

					/*
						dd1_0 = ∂^3M/∂x^2∂t + ∂^3N/∂x∂y∂t
						dd1_2 = ∂^3W/∂x^3 + ∂^3W/∂x∂y^2
						dd1_1 = (∂^2M/∂x^2 + ∂^2M/∂y^2)
						d2 = (B+C1)*h^2;
						d3 = B*g*pow(H(i,j),3.0)

						addterm1 = (B+C1)*h^2*(∂^3M/∂x^2∂t + ∂^3N/∂x∂y∂t) 
						           + B*g*pow(H(i,j),3.0)*(∂^3W/∂x^3 + ∂^3W/∂x∂y^2)
								   + νb*(∂^2M/∂x^2 + ∂^2M/∂y^2)

					*/
					double gamma = 0.3*dispersion_correction_coefficient1;
					if ( W__(i,j)/H2__(i,j) > 0.60 )
					{
						//波高水深比H/h=0.8程度となう静水深以浅いで分散項は省略
						//※ブシネスクモデルによる砕波帯内波・流れ場の推定精度の控除に関する検討
						gamma = 0.0;
					}

					double xx1 = d2*dd1_0;
					double xx2 = d3*dd1_2;
					if ( fabs(xx1) > 0.0 && gamma*(local_termM) < fabs(xx1))
					{
						xx1  *= gamma*(local_termM)/fabs(xx1);
					}
					if ( fabs(xx2) > 0.0 && gamma*(local_termM) < fabs(xx2))
					{
						xx2 *= gamma*(local_termM)/fabs(xx2);
					}
					//if ( gamma*(local_termM) < fabs(dd1_1))
					//{
					//	//dd1_1 = 0.0;
					//}

					double addterm1 = (xx1 + xx2 + dd1_1);

					/*
						dd2_0 = ∂^3N/∂y^2∂t + ∂^3M/∂x∂y∂t
						dd2_2 = ∂^3W/∂y^3 + ∂^3W/∂x^2∂y
						dd2_1 = (∂^2N/∂x^2 + ∂^2N/∂y^2)
						d2 = (B+C1)*h^2;
						d3 = B*g*pow(H(i,j),3.0)

						addterm2 = (B+C1)*h^2*(∂^3N/∂y^2∂t + ∂^3M/∂x∂y∂t) 
						           + B*g*pow(H(i,j),3.0)*(∂^3W/∂y^3 + ∂^3W/∂x^2∂y)
								   + νb*(∂^2N/∂x^2 + ∂^2N/∂y^2)

					*/
					double yy1 = d2*dd2_0;
					double yy2 = d3*dd2_2;
					if ( fabs(yy1) > 0.0 && gamma*(local_termN) < fabs(yy1))
					{
						yy1  *= gamma*(local_termN)/fabs(yy1);
					}
					if ( fabs(yy2) > 0.0 && gamma*(local_termN) < fabs(yy2))
					{
						yy2 *= gamma*(local_termN)/fabs(yy2);
					}

					double addterm2 = (yy1 + yy2 + dd2_1);


					if ( soliton == 3 )
					{
						addterm1 = dd1_1;
						addterm2 = dd2_1;
					}

					if (  DEBUG_LOG(i,j) )
					{
						log_printf("\nDEBUG:addterm1 %f addterm2 %f\n", addterm1, addterm2);
					}

					dd1 = addterm1;
					dd2 = addterm2;

					if ( soliton == 4 && water_area && !Peregrine )
					{
						double dh[8];
#ifdef USE_HIGH_ORDER
						dh[0] = H2__(i,j-4);
						dh[1] = H2__(i,j-3);
						dh[2] = H2__(i,j-2);
						dh[3] = H2__(i,j-1);
						dh[4] = H2__(i,j+1);
						dh[5] = H2__(i,j+2);
						dh[6] = H2__(i,j+3);
						dh[7] = H2__(i,j+4);
#else
						//1次精度
						//dh[0] = H__(i,j-1);
						//dh[1] = H__(i,j+1);

						//4次精度
						//dh[0] = H__(i,j-4);
						//dh[1] = H__(i,j-3);
						//dh[2] = H__(i,j-2);
						//dh[3] = H__(i,j-1);
						//dh[4] = H__(i,j+1);
						//dh[5] = H__(i,j+2);
						//dh[6] = H__(i,j+3);
						//dh[7] = H__(i,j+4);

						//2次精度
						dh[0] = H2__(i,j-2);
						dh[1] = H2__(i,j-1);
						dh[2] = H2__(i,j+1);
						dh[3] = H2__(i,j+2);
						const double dxH = DIFF_2(dh)*r_dx;

						//dh[0] = H2__(i, j - 1);
						//dh[1] = H2__(i, j + 1);
						//const double dxH = DIFF_(dh)*r_dx;
#endif
						/*
							dxHH1 = ∂H/∂x
						*/

#ifdef USE_HIGH_ORDER
						dh[0] = H2__(i-4,j);
						dh[1] = H2__(i-3,j);
						dh[2] = H2__(i-2,j);
						dh[3] = H2__(i-1,j);
						dh[4] = H2__(i+1,j);
						dh[5] = H2__(i+2,j);
						dh[6] = H2__(i+3,j);
						dh[7] = H2__(i+4,j);
#else
						//1次精度
						//dh[0] = H__(i-1,j);
						//dh[1] = H__(i+1,j);

						//4次精度
						//dh[0] = H__(i-4,j);
						//dh[1] = H__(i-3,j);
						//dh[2] = H__(i-2,j);
						//dh[3] = H__(i-1,j);
						//dh[4] = H__(i+1,j);
						//dh[5] = H__(i+2,j);
						//dh[6] = H__(i+3,j);
						//dh[7] = H__(i+4,j);

						//2次精度
						dh[0] = H2__(i-2,j);
						dh[1] = H2__(i-1,j);
						dh[2] = H2__(i+1,j);
						dh[3] = H2__(i+2,j);
						const double dyH = DIFF_2(dh)*r_dy;

						//dh[0] = H2__(i - 1, j);
						//dh[1] = H2__(i + 1, j);
						//const double dyH = DIFF_(dh)*r_dy;
#endif
						/*
							dxHH1 = ∂H/∂y
						*/

						const double dtdxM = (DIFFx_2(tdiff.Fm,i,j))*r_dx;
						const double dtdyM = (DIFFy_2(tdiff.Fm,i,j))*r_dy;
						const double dtdxN = (DIFFx_2(tdiff.Fn,i,j))*r_dx;
						const double dtdyN = (DIFFy_2(tdiff.Fn,i,j))*r_dy;
						const double dxxW = DIFFxx_2(w__,i,j)*r_dxx;
						const double dyyW = DIFFyy_2(w__,i,j)*r_dyy;
						const double dxyW = DIFFxy_2(w__,i,j)*r_dxy;

						//修正ブシネスク方程式で追加される項(Madsen and Sorensen、1992)
						/*
							a1 =  h*∂H/∂x*(C1*∂^2M/∂x∂t + C2*∂^2N/∂y∂t)
							b1 =  h*∂H/∂y*(C2*∂^2N/∂x∂t)
							c1 = B*g*h^2*(∂H/∂x*(2*∂^2W/∂x^2 + ∂^2W/∂y^2) + ∂H/∂y*∂^2W/∂x∂y)
						*/
						double a1 = h*dxH*(C1*dtdxM + C2*dtdyN);
						double b1 = h*dyH*(C2*dtdxN);
						double c1 = B*g_*hh*(dxH*(2.0*dxxW+dyyW)+dyH*dxyW);
						if (  DEBUG_LOG(i,j) )
						{
							log_printf("DEBUG:a1 %f b1 %f c1 %f\n", a1, b1, c1);
						}
						
						double gamma2 = 0.03*dispersion_correction_coefficient2;
						if ( W__(i,j)/H2__(i,j) > 0.60 )
						{
							//波高水深比H/h=0.8程度となう静水深以浅いで分散項は省略
							//※ブシネスクモデルによる砕波帯内波・流れ場の推定精度の控除に関する検討
							gamma2 = 0.0;
						}

						if ( fabs(a1) > 0.0 && gamma2*(local_termM) < fabs(a1))
						{
							a1  *= gamma2*(local_termM)/fabs(a1);
						}
						if ( fabs(b1) > 0.0 && gamma2*(local_termM) < fabs(b1))
						{
							b1  *= gamma2*(local_termM)/fabs(b1);
						}
						if ( fabs(c1) > 0.0 && gamma2*(local_termM) < fabs(c1))
						{
							c1  *= gamma2*(local_termM)/fabs(c1);
						}



						/*
							a2 =  h*∂H/∂y*(C1*∂^2N/∂y∂t + C2*∂^2M/∂x∂t)
							b2 =  h*∂H/∂x*(C2*∂^2M/∂y∂t)
							c2 = B*g*h^2*(∂H/∂y*(2*∂^2W/∂y^2 + ∂^2W/∂x^2) + ∂H/∂x*∂^2W/∂x∂y)
						*/
						double a2 = h*dyH*(C1*dtdyN + C2*dtdxM);
						double b2 = h*dxH*(C2*dtdyM);
						double c2 = B*g_*hh*(dyH*(2.0*dyyW+dxxW)+dxH*dxyW);
						if (  DEBUG_LOG(i,j) )
						{
							log_printf("DEBUG:a2 %f b2 %f c2 %f\n", a2, b2, c2);
						}
						if ( fabs(a2) > 0.0 && gamma2*(local_termN) < fabs(a2))
						{
							a2  *= gamma2*(local_termN)/fabs(a2);
						}
						if ( fabs(b2) > 0.0 && gamma2*(local_termN) < fabs(b2))
						{
							b2  *= gamma2*(local_termN)/fabs(b2);
						}
						if ( fabs(c2) > 0.0 && gamma2*(local_termN) < fabs(c2))
						{
							c2  *= gamma2*(local_termN)/fabs(c2);
						}
				
						double addterm1 = (a1 + b1) + c1;
						double addterm2 = (a2 + b2) + c2;

						dd1 += addterm1;
						dd2 += addterm2;
					}
				}
				//printf("砕波項 %3.8g %3.8g  分散項 %3.8g %3.8g\n", dd1_1, dd2_1, dd1_0, dd2_0);


				//if ( fabs(local_termM) > 0.001 && fabs(local_termM+dd1) > fabs(FM__(i,j))*dispersin_lim)
				//{
				//	//log_printf("local_termM %f => %f  %f\n", local_termM, dd1, dd1/local_termM);
				//	skipp++;
				//}
				//if ( fabs(local_termN) > 0.001 && fabs(local_termN+dd2) > fabs(FN__(i,j))*dispersin_lim)
				//{
				//	//log_printf("local_termN %f => %f  %f\n", local_termN, dd2, dd2/local_termN);
				//	skipp++;
				//}
				//if ( skipp )
				//{
				//	skipp_dispersion_terms++;
				//}
				//printf("%f %f\n", dd1, dd2);
				
				//double gamma = 1.0;
				//if (gamma*(local_termM) < fabs(dd1) || gamma*(local_termN) < fabs(dd2))
				//{
				//	dd1 = 0.0;
				//	dd2 = 0.0;
				//}
				FM__(i,j) += dd1;
				FN__(i,j) += dd2;
				if (  DEBUG_LOG(i,j) )
				{
					log_printf("DEBUG:非線形分散項 %f %f\n\n", dd1, dd2);
				}

			}
		}
	}
	if (  DEBUG_LOG(debug_index_i,debug_index_j) )
	{
		log_printf("\n\n非線形分散項計算 END\n");
	}
	//if ( (double)skipp_dispersion_terms/(double)(eval_dispersion_terms) > 0.05 )
	//{
	//	log_printf("Warning:abnormal dispersion term:%.2f%%\n", 100.0*(double)skipp_dispersion_terms/(double)(eval_dispersion_terms)); 
	//}

end:	;

	return 0;
}


#ifdef USE_OPENCL
mOpenCL_common cm;
mOpenCL_program* pg = 0;
mOpenCL_kernel* ker = 0;
#endif

QH_ *QHorg = 0; 
QH_ *QHnew = 0; 

int Solver::MotionEquation3(AbeNoguera* p, AbeNoguera* cur, double delta_t, double delta_x, double delta_y)
{
	const double dt = delta_t;

	const int ysz = jY-ZERO_AREA_WD;
	const int xsz = iX-ZERO_AREA_WD;
#include "array_expand.h"
	double* pm__ = p->m;
	double* pn__ = p->n;

#ifdef FLOW_LIMIT_DEBUG
	if(QHorg==0) QHorg = new QH_[QH_SIZE_MAX]; 
	if(QHnew==0) QHnew = new QH_[QH_SIZE_MAX]; 
#endif


#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j <xsz; j++ )
		{
			pM__(i, j) = M__(i, j) + dt*(FM__(i, j));
			pN__(i, j) = N__(i, j) + dt*(FN__(i, j));

			if (  DEBUG_LOG(i,j) )
			{
				log_printf("DEBUG:dt %f FM %f  FN %f\n", dt, FM__(i,j), FN__(i,j));
				log_printf("DEBUG:M %f N %f -> M %f N %f\n", M__(i,j), N__(i,j), pM__(i,j), pN__(i,j));
				double q = sqrt(pM__(i,j)*pM__(i,j) + pN__(i,j)*pN__(i,j));
				double q2 = sqrt(M__(i,j)*M__(i,j) + N__(i,j)*N__(i,j));
				log_printf("DEBUG:Q %f  -> %f\n\n", q2, q);
			}
		}
	}


	if(0)
	{
		const int ysz = jY-ZERO_AREA_WD;
		const int xsz = iX-ZERO_AREA_WD;
		const int absorbingZone_ = absorbingZone;
		const int IX = iX;
		const int JY = jY;

		double a, b, c, d, e, f;

		a = 1.0/2.0;
		b = 1.0/2.0;
		c = 0.0;
		d = 0.0;
		e = 0.0;
		f = 0.0;

		//a = 5.0/8.0;
		//b = 1.0/2.0;
		//c = -1.0/8.0;
		//d = 0.0;
		//e = 0.0;
		//f = 0.0;


		//a = 193.0/256.0;
		//b = 105.0/256.0;
		//c = -15.0/64.0;
		//d = 45.0/512.0;
		//e = -5.0/256.0;
		//f = 1.0/512.0;

		b *= 0.5;
		c *= 0.5;
		d *= 0.5;
		e *= 0.5;
		f *= 0.5;

		const double aa = a*a;
		const double bb = b*b;
		const double cc = c*c;
		const double dd = d*d;
		const double ee = e*e;
		const double ff = f*f;

	
		double* arry1_wrk = new double[IX*JY];
		double* arry2_wrk = new double[IX*JY];

		memcpy(arry1_wrk, pm__, sizeof(double)*IX*JY);
		memcpy(arry2_wrk, pn__, sizeof(double)*IX*JY);

		tmpX = pm__;
		tmpY = pn__;

		#pragma omp parallel for OMP_SCHEDULE
		for (int ii__ = 0; ii__ < iX*jY; ii__++)
		//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
		{
			int i = ii__ / iX;
			int j = ii__ % iX;
			if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
			{
				continue;
			}
			//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
			{
				if ( IsAbsorbingZoneBounray(i,j) || !(GAP(i,j) & GAP_7))
				{
					continue;
				}
				//ARY_wrk(arry1_wrk, i,j) = LELEFILTER_2(ARY_wrkX,i,j);
				//ARY_wrk(arry2_wrk, i,j) = LELEFILTER_2(ARY_wrkY,i,j);
				ARY_wrk(arry1_wrk, i,j) = 0.25*(pM__(i-1,j)+pM__(i+1,j)+pM__(i,j-1)+pM__(i,j+1));
				ARY_wrk(arry2_wrk, i,j) = 0.25*(pN__(i-1,j)+pN__(i+1,j)+pN__(i,j-1)+pN__(i,j+1));
			}
		}
		memcpy(pm__, arry1_wrk, sizeof(double)*IX*JY);
		memcpy(pn__, arry2_wrk, sizeof(double)*IX*JY);
		delete [] arry1_wrk;
		delete [] arry2_wrk;
	}

	float* z_f = 0;
#ifdef TSU_USE_ACC
	{
		z_f = new float[iX*jY];
		float* m_f = new float[iX*jY];
		float* n_f = new float[iX*jY];
		float* d2_f = new float[iX*jY];

		memset(z_f, '\0', sizeof(float)*iX*jY);
		memset(m_f, '\0', sizeof(float)*iX*jY);
		memset(n_f, '\0', sizeof(float)*iX*jY);
		memset(d2_f, '\0', sizeof(float)*iX*jY);

#pragma omp parallel for OMP_SCHEDULE
		for ( int i = ZERO_AREA_WD; i < ysz; i++ )
		{
			for ( int j = ZERO_AREA_WD; j < xsz; j++ )
			{
				d2_f[i*iX+j] = D2__(i,j);
				m_f[i*iX+j]  = pM__(i,j);
				n_f[i*iX+j]  = pN__(i,j);
			}
		}

		{
			extent<2> e(jY, iX);
			array_view<float, 2> r(e, z_f);
			array_view<const float, 2> d2_f_(e, d2_f);
			array_view<const float, 2> m_f_(e, m_f);
			array_view<const float, 2> n_f_(e, n_f);

			const float flow_res2_f = flow_res2;
			const float FLOW_DEPTH_LIMIT_COND_COEF1_f = (float)FLOW_DEPTH_LIMIT_COND_COEF1;
			const float FLOW_DEPTH_LIMIT_COND_COEF2_f = (float)FLOW_DEPTH_LIMIT_COND_COEF2;

			parallel_for_each(e, [=](index<2> idx) restrict(amp)
			{
				const int i = idx[0];
				const int j = idx[1];

				float av = av = 0.25*(d2_f_(i-1,j) + d2_f_(i+1,j) + d2_f_(i,j-1) + d2_f_(i,j-1));
				if ( fast_math::fabs(d2_f_[idx]) < fast_math::fabs(av) ) av = fast_math::fabs(d2_f_[idx]);

				float q = fast_math::sqrt(m_f_[idx]*m_f_[idx] + n_f_[idx]*n_f_[idx]);

				//許容最大値
				float qmax = flow_res2_f*FLOW_DEPTH_LIMIT_COND_COEF1_f*fast_math::pow(av, FLOW_DEPTH_LIMIT_COND_COEF2_f);

				r[idx] = -1.0f;
				if ( q > 0.1 && qmax > 0.1f && q > qmax)
				{
					r[idx] = qmax/q;
				}

			});
		}
		delete [] m_f;
		delete [] n_f;
		delete [] d2_f;
	}
#else
	{
		z_f = new float[iX*jY];
		memset(z_f, '\0', sizeof(float)*iX*jY);
		{
#pragma omp parallel for OMP_SCHEDULE
			for (int ii__ = 0; ii__ < iX*jY; ii__++)
			//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
			{
				int i = ii__ / iX;
				int j = ii__ % iX;
				if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
				{
					continue;
				}
				//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
				{
					double av = 0.25*(D2__(i-1,j) + D2__(i+1,j) + D2__(i,j-1) + D2__(i,j-1));
					if ( fabs(D2__(i,j)) < fabs(av) ) av = fabs(D2__(i,j));

					double q = sqrt(pM__(i, j)*pM__(i, j) + pN__(i, j)*pN__(i, j));

					//許容最大値
					double qmax = flow_res2*FLOW_DEPTH_LIMIT_COND_COEF1*pow(av, FLOW_DEPTH_LIMIT_COND_COEF2);

					z_f[i*iX+j] = -1.0f;
					if ( q > 0.1 && qmax > 0.1f && q > qmax)
					{
						z_f[i*iX+j] = qmax/q;
					}
				}
			}
		}
	}

#endif

	int cutoff_tgt = 0;
	int cutoff = 0;
#pragma omp parallel for OMP_SCHEDULE reduction(+:cutoff)
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j <xsz; j++ )
		{
			//異常値検査
			cutoff_tgt++;

			double z = -1.0;
			if (z_f)
			{
				z = z_f[i*iX+j];
			}else
			{
				double av = av = 0.25*(D2__(i-1,j) + D2__(i+1,j) + D2__(i,j-1) + D2__(i,j-1));
				if ( fabs(D2__(i,j)) < fabs(av) ) av = fabs(D2__(i,j));

				double q = sqrt(pM__(i,j)*pM__(i,j) + pN__(i,j)*pN__(i,j));

				//許容最大値
				double qmax = flow_res2*FLOW_DEPTH_LIMIT_COND_COEF1*pow(av, FLOW_DEPTH_LIMIT_COND_COEF2);
				if ( q > 0.1 && qmax > 0.1 && q > qmax)
				{
					z = qmax/q;
				}
			}

			if ( z > 0.0 && z < 1.0)
			{
				pM__(i,j) *= z;
				pN__(i,j) *= z;
				GAP(i,j) |= GAP_6;	//異常マーク
				cutoff++;
			}
		}
	}
	delete [] z_f;

	if ( (double)cutoff/(double)(cutoff_tgt) > 0.005 )
	{
		log_printf("Warning:abnormal flow rate:%.4f%% (flowres2:%.3f)\n", 100.0*(double)cutoff/(double)(cutoff_tgt), flow_res2); 
	}



	//異常値があった場合
	{
		AbnormalFilter(p);

#ifdef FLOW_LIMIT_DEBUG
#pragma omp parallel for OMP_SCHEDULE reduction(+:cutoff)
		for ( int i = ZERO_AREA_WD; i < ysz; i++ )
		{
			for ( int j = ZERO_AREA_WD; j <xsz; j++ )
			{
				double q = sqrt(pM__(i,j)*pM__(i,j) + pN__(i,j)*pN__(i,j));

				int index = (int)D2__(i,j);
				if ( index < 0 ) index = 0;
				if ( index >= QH_SIZE_MAX ) index = QH_SIZE_MAX-1;
				QHnew[index].Update(q, H__(i,j));
			}
		}
		FILE* fp = fopen("QH.csv", "w");
		if ( fp )
		{
			fprintf(fp, "全水深,流量org,流量new\n");
			for ( int i = 0; i < QH_SIZE_MAX; i++ )
			{
				fprintf(fp, "%d,%f,%f\n", i, QHorg[i].q+0.00001, QHnew[i].q+0.00001);
			}
			fclose(fp);
		}
#endif
	}


#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j <xsz; j++ )
		{
			if (  DEBUG_LOG(i,j) )
			{
				log_printf("DEBUG:(IN)M %f N %f\n", p->M(i,j), p->N(i,j));
				double q2 = sqrt(p->M(i,j)*p->M(i,j) + p->N(i,j)*p->N(i,j));
				log_printf("DEBUG:(IN)Q %f\n\n", q2);
			}

			//境界の条件
			TBoundaryConditionMain(p, i, j);


#if 0
			if ( Revetment_h >= 0.5 && RUNUPFLG(i,j) == IS_OVERFLOW )
			{
				double dh;
				if ( RunupConditionFlagCondition(p, i, j, dh) == IS_OVERFLOW )
				{
					double qq = sqrt(SQR(p->M(i,j)) + SQR(p->N(i,j)));
					if ( dh >= Revetment_h && qq > 0.001 )
					{
						//if ( dh > 3.0 ) printf("---------護岸越流[%f]--------\n", dh);
						p->M(i,j) /= qq;
						p->N(i,j) /= qq;

						//相田の公式
						double q = 0.6*dh*sqrt(g*dh);

						if ( q < 0.0 ) q = 0.0;
						p->M(i,j) *= q;
						p->N(i,j) *= q;
						if (  DEBUG_LOG(i,j) )
						{
							log_printf("DEBUG:(OUT1)M %f N %f\n", p->M(i,j), p->N(i,j));
							double q2 = sqrt(p->M(i,j)*p->M(i,j) + p->N(i,j)*p->N(i,j));
							log_printf("DEBUG:(OUT1)Q %f\n\n", q2);
						}
					}
				}
			}
#endif
			//吸収境界条件 Crejan(1985)
			const double dw1 = absorbingZoneBounray(i, j, 0);
			const double dw2 = absorbingZoneBounray(i, j, 1);
			p->M(i,j) *= dw1;
			p->N(i,j) *= dw2;
			if (  DEBUG_LOG(i,j) )
			{
				log_printf("DEBUG:(OUT)M %f N %f\n", p->M(i,j), p->N(i,j));
				double q2 = sqrt(p->M(i,j)*p->M(i,j) + p->N(i,j)*p->N(i,j));
				log_printf("DEBUG:(OUT)Q %f\n\n", q2);
			}
		}
	}

#if 10
	int target_num = 0;
	int mdfy_num = 0;
	//数値発散を抑える処理(水位先端境界での制限)
#pragma omp parallel for OMP_SCHEDULE reduction(+:target_num) reduction(+:mdfy_num)
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j <xsz; j++ )
		{

			const double d2 = D2(i,j);
			const double fabs_d2 = fabs(d2);
			if ( fabs_d2 >= 500.0 )
			{
				continue;
			}

			const double m = p->M(i,j);
			const double n = p->N(i,j);
			const double Q = sqrt(m*m + n*n);

			if ( Q > 0.001)
			{
				target_num++;
				double HH = fabs_d2;
				double cof1 = MN_UPPER_LIMIT1;
				double cof2 = MN_UPPER_LIMIT2;

				cof1 = cof2;

#if 10
				bool elv = false;
				double d_gap_wrk;
				double d_min = 99999999.0;
				double d_max = -99999999.0;

				for (int kki = -2; kki <= 2; kki++)
				{
					for (int kkj = -2; kkj <= 2; kkj++)
					{
						d_gap_wrk = D2__(i + kki, j + kkj);
						if (d_max < d_gap_wrk) d_max = d_gap_wrk;
						if (d_min > d_gap_wrk) d_min = d_gap_wrk;
						if (T__(i + kki, j + kkj) == 0) elv = true;
					}
				}
				
				//if (fabs(d_max - d_min) > steep_slope2[1] && d_min < steep_slope2[0])
				//if (fabs(d_min) > 0.000001 && fabs(d_max - d_min) / fabs(d_min) > 1.0 && d_min < steep_slope2[0])			
				
				if (fabs(d_max - d_min) > steep_slope2[1] && d_min < steep_slope2[0])
				{
					//cof1 = 17.0;
					cof1 *= MN_FLOW_RESTRICTION_COEF2;
				}
				if (RunUp && T__(i,j) != 0 && elv)
				{
					cof1 = cof2;
					cof1 *= MN_FLOW_RESTRICTION_COEF2;
				}

				if (RunUp && T__(i,j) == 0 )
				{
					cof1 = cof2;
					cof1 *= MN_FLOW_RESTRICTION_COEF3;
				}
#endif
				if (HH > 0.0 && Q > cof1*HH )
				{
					//log_printf("\nCAUTION!! M(%d,%d):%f N(%d,%d):%f H:%f W:%f\n", i, j,  p->M(i,j), i, j, p->N(i,j), HH, W(i,j) );
					const double a = (flow_res*MN_FLOW_RESTRICTION_COEF*cof1*HH)/Q;
					GAP(i,j) |= GAP_3;

					p->M(i,j) *= a;
					p->N(i,j) *= a;

					mdfy_num++;
					//log_printf("	====> M(%d,%d):%f N(%d,%d):%f\n", i, j,  p->M(i,j), i, j, p->N(i,j));
					//exit(1);
				}
			}
			if (  DEBUG_LOG(i,j) )
			{
				log_printf("DEBUG:(OUT2)M %f N %f\n", p->M(i,j), p->N(i,j));
				double q2 = sqrt(p->M(i,j)*p->M(i,j) + p->N(i,j)*p->N(i,j));
				log_printf("DEBUG:(OUT2)Q %f\n\n", q2);
			}

		}
	}

	if (100.0*(double)mdfy_num / (double)target_num > 5.0) log_printf("%d %d %f%%\n", target_num, mdfy_num, 100.0*(double)mdfy_num / (double)target_num);
#endif

	return 0;
}

void Solver::AbnormalFilter(AbeNoguera* p)
{
	const int ysz = jY-ZERO_AREA_WD;
	const int xsz = iX-ZERO_AREA_WD;
#include "array_expand.h"
	double* pm__ = p->m;
	double* pn__ = p->n;

	int abnormal_flow = 0;
	//異常値があったか？
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j <xsz; j++ )
		{
			if ( GAP(i,j) & GAP_6 )
			{
				abnormal_flow = 1;
				break;
			}
		}
		if ( abnormal_flow ) break;
	}

	if ( abnormal_flow == 0 ) return;

	//異常値があった場合
	double a, b, c, d, e, f;

	int _filterOrder = 10;

	if ( _filterOrder == 2 )
	{
		a = 1.0/2.0;
		b = 1.0/2.0;
		c = 0.0;
		d = 0.0;
		e = 0.0;
		f = 0.0;
	}
	if ( _filterOrder == 4 )
	{
		a = 5.0/8.0;
		b = 1.0/2.0;
		c = -1.0/8.0;
		d = 0.0;
		e = 0.0;
		f = 0.0;
	}
	if ( _filterOrder == 6 )
	{
		a = 11.0/16.0;
		b = 15.0/32.0;
		c = -3.0/16.0;
		d = 1.0/32.0;
		e = 0.0;
		f = 0.0;
	}
	if ( _filterOrder == 8 )
	{
		a = 93.0/128.0;
		b = 7.0/16.0;
		c = -7.0/32.0;
		d = 1.0/16.0;
		e = -1.0/128.0;
		f = 0.0;
	}
	if ( _filterOrder == 10 )
	{
		a = 193.0/256.0;
		b = 105.0/256.0;
		c = -15.0/64.0;
		d = 45.0/512.0;
		e = -5.0/256.0;
		f = 1.0/512.0;
	}
	b *= 0.5;
	c *= 0.5;
	d *= 0.5;
	e *= 0.5;
	f *= 0.5;

	const double aa = a*a;
	const double bb = b*b;
	const double cc = c*c;
	const double dd = d*d;
	const double ee = e*e;
	const double ff = f*f;

	double* Mnew = new double[iX*jY];
	double* Nnew = new double[iX*jY];
	memcpy(Mnew, p->m, sizeof(double)*iX*jY);
	memcpy(Nnew, p->n, sizeof(double)*iX*jY);

	const int kmax = 3;
	const int range = 0;
	for ( int k = 0; k < kmax; k++ )
	{
		//減衰処理で上限をカットしたので平滑化を行う
#pragma omp parallel for OMP_SCHEDULE
		for (int ii__ = 0; ii__ < iX*jY; ii__++)
		//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
		{
			int i = ii__ / iX;
			int j = ii__ % iX;
			if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
			{
				continue;
			}
			//for ( int j = ZERO_AREA_WD; j <xsz; j++ )
			{
				if ( IsAbsorbingZoneBounray(i,j))
				{
					continue;
				}

				unsigned char x = (GAP(i,j) & GAP_6);
				for ( int ki = -range; ki <= range; ki++ )
				for ( int kj = -range; kj <= range; kj++ ) x |= ((GAP(i+ki,j+kj) & GAP_6));

				if ( x )
				{
					switch(_filterOrder)
					{
					case 10:
						Mnew[i*iX+j] = LELEFILTER_10(pM__, i, j);
						Nnew[i*iX+j] = LELEFILTER_10(pN__, i, j);
						break;
					case 8:
						Mnew[i*iX+j] = LELEFILTER_8(pM__, i, j);
						Nnew[i*iX+j] = LELEFILTER_8(pN__, i, j);
						break;
					case 6:
						Mnew[i*iX+j] = LELEFILTER_6(pM__, i, j);
						Nnew[i*iX+j] = LELEFILTER_6(pN__, i, j);
						break;
					case 4:
						Mnew[i*iX+j] = LELEFILTER_4(pM__, i, j);
						Nnew[i*iX+j] = LELEFILTER_4(pN__, i, j);
						break;
					case 2:
						Mnew[i*iX+j] = LELEFILTER_2(pM__, i, j);
						Nnew[i*iX+j] = LELEFILTER_2(pN__, i, j);
						break;
					}
				}
			}
		}
		memcpy(p->m, Mnew, sizeof(double)*iX*jY);
		memcpy(p->n, Nnew, sizeof(double)*iX*jY);
	}
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j <xsz; j++ )
		{
			if (GAP(i,j) & GAP_6 )
			{
				GAP(i,j) &= ~GAP_6;
			}
		}
	}
	delete [] Mnew;
	delete [] Nnew;
}


int Solver::ContinuityEquation2(double delta_t, double delta_x, double delta_y)
{
	const double r_dy = 1.0/delta_y;
	const double r_dt = 1.0/delta_t;

	const int ysz = jY-ZERO_AREA_WD;
	const int xsz = iX-ZERO_AREA_WD;

#include "array_expand.h"

	//連続の式
#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		const double r_dx = R_DX(i);	//1.0/delta_x;

		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			//連続の式
			FW__(i,j) =  -DIFFx(m__, i,j)*r_dx -DIFFy(n__, i,j)*r_dy;
		}
	}
	return 0;
}

int Solver::WaveHeightBounrayCondition(AbeNoguera* p, int i, int j)
{
#include "array_expand.h"

	double* qm__ = p->m;
	double* qn__ = p->n;
	double* qw__ = p->w;
	//if ( fabs(p->W(i,j)) > 25.0 )
	//{
	//	log_printf("%d %d %f <- %f TOP:%d DEPTH:%f ELV:%f\n", i, j, p->W(i,j), W(i,j), T(i,j), H(i,j), ELV(i,j));
	//}

	double& waveHeight =  qW__(i,j);

	if ( waveHeight > 2.0e4 ||  waveHeight < -2.0e4)
	{
		log_printf("エラー終了（数値発散）(%d,%d) %f <- %f\n", i, j, p->W(i,j), W(i,j));
		log_printf("%f <- %f TOP:%d DEPTH:%f ELV:%f\n", p->W(i,j), W(i,j), T(i,j), H(i,j), ELV(i,j));
		return -1;
	}
	if ( _isnan(waveHeight) )
	{
		log_printf("エラー終了（数値計算不正）%d-----------------------------%d %d p->W(i,j) %f\n", __LINE__, i,j,p->W(i,j));
		log_printf("%f <- %f TOP:%d DEPTH:%f ELV:%f\n", p->W(i,j), W(i,j), T(i,j), H(i,j), ELV(i,j));
		return -2;
	}
	//境界の条件

	if ( T__(i,j) == 0 && !RunUp_ )
	{
		qM__(i,j) = 0.0;
		qN__(i,j) = 0.0;
		waveHeight = 0;
	}

	if ( RunUp_ && T__(i,j) == 0 )
	{
		//本来マイナスにはならない。急激に変更すると解が振動するため減衰させる
		if ( qW__(i,j) < 0.0 )
		{
			qM__(i,j) *= 0.001;
			qN__(i,j) *= 0.001;
			waveHeight *= 0.001;
		}

		//「遡上しない」という判定箇所
		if (waveHeight < DRYDEPTH2 || RUNUPFLG(i, j) == IS_DRY)
		{
			qM__(i,j) = 0.0;
			qN__(i,j) = 0.0;
			waveHeight = 0.0;
			RUNUPFLG(i, j) = IS_DRY;
		}
	}

	//解の発散を防止する方法
	{
		//水深より波高が下がることは無い
#if 10
		//const double h1 = H__(i,j);
		//const double h2 = (H__(i-1,j)+H__(i+1,j)+H__(i,j-1)+H__(i,j+1))*0.25;
		//double h = h1;
		//if ( h1 > h2 ) h = h2;
		//if ( T__(i,j) != 0 && waveHeight < -h2 )
		//{
		//	//printf("海底露出　水深:%f 波高:%f\n", h2, waveHeight);
		//	waveHeight = -h2*0.999;
		//}
#else
		double h1 = H(i,j);
		if ( T(i,j) != 0 && p->W(i,j) < -h1 )
		{
			p->W(i,j) = -h1*0.99;
			if ( p->W(i,j) + h1 < Dmin ) p->W(i,j) = -Dmin;
		}
#endif
	}
	return 0;

}

int Solver::ContinuityEquation3(AbeNoguera* p, AbeNoguera* cur, double delta_t, double delta_x, double delta_y)
{

	const double dt = delta_t;
	//連続の式
	const int ysz =  jY-ZERO_AREA_WD;
	const int xsz =  iX-ZERO_AREA_WD;
#include "array_expand.h"
	double* pw__ = p->w;
	double* pm__ = p->m;
	double* pn__ = p->n;

	double* reeduction_coefficient = new double[iX*jY];
#pragma omp parallel for OMP_SCHEDULE
	for ( int i = 0; i < jY; i++ )
	{
		for ( int j = 0; j < iX; j++ )
		{
			reeduction_coefficient[i*iX+j] = 1.0;
		}
	}


#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			pW__(i, j) = W__(i, j) + dt*(FW__(i, j));

			const double h1 = H__(i,j);
			const double h2 = (H__(i-1,j)+H__(i+1,j)+H__(i,j-1)+H__(i,j+1))*0.25;
			double h = h1;
			if ( h1 > h2 ) h = h2;

			if ( T__(i,j) != 0 && pW__(i,j) < -h2 )
			{
				pW__(i,j) = -h2*0.999;

				//質量保存を極力満たすように流量を調整しておく
#if 0
				{
					double alp = 1.0;
					if ( fabs( FW__(i,j)) > 1.0e-10 ) alp = ((-W__(i,j)+0.999*h2)/dt)/FW__(i,j);
					
					if ( fabs(h2) > 0.000001 )
					{
						log_printf("海底露出　水深:%f 波高:%f => 補正係数:%.8f\n", h2, pW__(i,j), alp);
					}
					//if ( fabs(alp) < 1.0 )
					//{
						reeduction_coefficient[i*iX+j] = alp;
					//}else
					//{
					//	reeduction_coefficient[i*iX+j] = 0.7;
					//}
					/*
					pW__(i,j)+h2 = 0;
					W__(i,j) + dt*(alp*FW__(i,j)) + h2 = 0;
					dt*(alp*FW__(i,j)) = -W__(i,j)-h2;
					(alp*FW__(i,j)) = -(W__(i,j)+h2)/dt;
					alp = (-(W__(i,j)+h2)/dt)/FW__(i,j);
					*/
				}
#endif
			}
		}
	}
#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			double a = 0.5*(reeduction_coefficient[(i)*iX+j-1]+reeduction_coefficient[(i)*iX+j+1]);
			double b = 0.5*(reeduction_coefficient[(i-1)*iX+j]+reeduction_coefficient[(i+1)*iX+j]);
			pM__(i,j) *= a;
			pN__(i,j) *= b;
		}
	}
	delete [] reeduction_coefficient;


#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			//吸収境界条件 Crejan(1985)
			const double dw = absorbingZoneBounray(i, j);
			FW__(i,j) *= dw;
			pW__(i,j) *= dw;

			int stat = WaveHeightBounrayCondition(p, i, j);
			if ( stat < 0 )
			{
				exit(stat);
			}
		}
	}

	this->maxheight_cur = 0.0;
	this->maxheight_w_cur2 = 0.0;
	this->maxheight_cur2 = 0.0;

#ifdef USE_OPENMP_3

	const int num_threads       = omp_get_max_threads();
	double* wmax_th             = new double[num_threads];
	double* wmin_th             = new double[num_threads];
	double* maxheight_cur_th    = new double[num_threads];
	double* maxheight_th        = new double[num_threads];
	int* maxheightPos1_th       = new int[num_threads];
	int* maxheightPos2_th       = new int[num_threads];
	double* maxheight_w_cur2_th = new double[num_threads];
	double* maxheight_w2_th     = new double[num_threads];
	double* maxheight_cur2_th   = new double[num_threads];
	double* maxheight2_th       = new double[num_threads];

	for ( int i = 0; i < num_threads; i++ )
	{
		wmax_th[i]             = wmax;
		wmin_th[i]             = wmin;
		maxheight_cur_th[i]    = maxheight_cur;
		maxheight_th[i]        = maxheight;
		maxheightPos1_th[i]    = maxheightPos[0];
		maxheightPos2_th[i]    = maxheightPos[1];
		maxheight_w_cur2_th[i] = maxheight_w_cur2;
		maxheight_w2_th[i]     = maxheight_w2;
		maxheight_cur2_th[i]   = maxheight_cur2;
		maxheight2_th[i]       = maxheight2;
	}

	const bool exist_river = riverInfo.existRiver();

#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			if (exist_river && (riverInfo.isInflow(i, j) || riverInfo.isRiver(i,j)))
			{
				continue;
			}
			const int id = omp_get_thread_num();

			const double pWij = pW__(i,j);
			const double Wij  = W__(i,j);

			if ( wmax_th[id] < pWij ) wmax_th[id] = pWij;
			if ( wmin_th[id] > pWij ) wmin_th[id] = pWij;

			if ( IsAbsorbingZoneBounray(i,j)) continue;
			if (maxheight_cur_th[id] < Wij)
			{
				maxheight_cur_th[id] = Wij;
			}
			if (maxheight_th[id] < Wij)
			{
				maxheight_th[id] = Wij;
				maxheightPos1_th[id] = i;
				maxheightPos2_th[id] = j;
			}

			if ( T__(i,j) == 0 )
			{
				if ( maxheight_w_cur2_th[id] < Wij)
				{
					maxheight_w_cur2_th[id] = Wij;
				}
				if ( maxheight_w2_th[id] < Wij )
				{
					maxheight_w2_th[id] = Wij;
				}

				if ( isWet(i,j) )
				{
					if ( maxheight_cur2_th[id] < ELV__(i,j))
					{
						maxheight_cur2_th[id] = ELV__(i,j);
					}
					if ( maxheight2_th[id] < ELV__(i,j) )
					{
						maxheight2_th[id] = ELV__(i,j);
					}
				}
			}
		}
	}
	for ( int i = 0; i < num_threads; i++ )
	{
		if ( wmax          < wmax_th[i])          wmax = wmax_th[i];
		if ( wmin          > wmin_th[i])          wmin = wmin_th[i];
		if ( maxheight_cur < maxheight_cur_th[i]) maxheight_cur = maxheight_cur_th[i];
		if ( maxheight     < maxheight_th[i])
		{
			maxheight = maxheight_th[i];
			maxheightPos[0] = maxheightPos1_th[i];
			maxheightPos[1] = maxheightPos2_th[i];
		}
		if ( maxheight_w_cur2 < maxheight_w_cur2_th[i]) maxheight_w_cur2 = maxheight_w_cur2_th[i];
		if ( maxheight_w2     < maxheight_w2_th[i])     maxheight_w2     = maxheight_w2_th[i];
		if ( maxheight_cur2   < maxheight_cur2_th[i])   maxheight_cur2   = maxheight_cur2_th[i];
		if ( maxheight2       < maxheight2_th[i])       maxheight2       = maxheight2_th[i];
	}

	delete [] wmax_th;
	delete [] wmin_th;
	delete [] maxheight_cur_th;
	delete [] maxheight_th;
	delete [] maxheightPos1_th;
	delete [] maxheightPos2_th;
	delete [] maxheight_w_cur2_th;
	delete [] maxheight_w2_th;
	delete [] maxheight_cur2_th;
	delete [] maxheight2_th;
#else
	//double wave_min = 99999999999.0;
	for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			if ( wmax < p->W(i,j) ) wmax = p->W(i,j);
			if ( wmin > p->W(i,j) ) wmin = p->W(i,j);

			MaxHeightUpdate(i,j);
			MaxHeight2Update(i,j);

			//if (RunUp && T(i,j) == 0 )
			//{
			//	if ( wave_min > p->W(i,j) ) wave_min = p->W(i,j);
			//}
		}
	}
#endif	
	//if (RunUp) printf("----%.9f\n", wave_min);
	return 0;
}

void Solver::eval(AbeNoguera* nxttime, AbeNoguera* cur, double delta_t)
{
	double DT = delta_t;	//leap-frog (互い違いに解くのだが 0.5delta_t が正しいのか？

	MakeD2();

#if 0
	for (int i = 1; i < jY-1; i++)
	{
#pragma omp parallel for OMP_SCHEDULE
		for (int j = 1; j < iX - 1; j++)
		{
			if (!RunUp)
			{
				if (T(i, j) == 0)
				{
					double n = 0.0;
					cur->W(i, j) = 0.0;
					if (T(i - 1, j) != 0 || T(i + 1, j) != 0)
					{
						if (T(i - 1, j) != 0)
						{
							cur->W(i, j) += cur->W(i - 1, j);
							n++;
						}
						if (T(i + 1, j) != 0)
						{
							cur->W(i, j) += cur->W(i + 1, j);
							n++;
						}
					}
					if (T(i, j - 1) != 0 || T(i, j + 1) != 0)
					{
						if (T(i, j - 1) != 0)
						{
							cur->W(i, j) += cur->W(i, j - 1);
							n++;
						}
						if (T(i, j + 1) != 0)
						{
							cur->W(i, j) += cur->W(i, j + 1);
							n++;
						}
					}
					if (n >= 1.0) cur->W(i, j) /= n;
				}
			}
			else
			{
				if (RUNUPFLG(i, j) == IS_DRY)
				{
					double n = 0.0;
					cur->W(i, j) = 0.0;
					if (RUNUPFLG(i - 1, j) != IS_DRY || RUNUPFLG(i + 1, j) != IS_DRY)
					{
						if (RUNUPFLG(i - 1, j) != IS_DRY)
						{
							cur->W(i, j) += cur->W(i - 1, j);
							n++;
						}
						if (RUNUPFLG(i + 1, j) != IS_DRY)
						{
							cur->W(i, j) += cur->W(i + 1, j);
							n++;
						}
					}
					if (RUNUPFLG(i, j - 1) != IS_DRY || RUNUPFLG(i, j + 1) != IS_DRY)
					{
						if (RUNUPFLG(i, j - 1) != IS_DRY)
						{
							cur->W(i, j) += cur->W(i, j - 1);
							n++;
						}
						if (RUNUPFLG(i, j + 1) != IS_DRY)
						{
							cur->W(i, j) += cur->W(i, j + 1);
							n++;
						}
					}
					if (n >= 1.0) cur->W(i, j) /= n;
				}
			}
		}
	}
#endif

	//格子間のギャップが大きい場合の差分が異常になるのを抑制
	//チェッカーボード振動抑制

	// dM/dt = Fm, dN/dt = Fn, dW/dt = Fw を解く　以下まとめて dX/dt = F(M(t),N(t),W(t)) と書く
	// Δt だけ進んだM,N,W値を求める
	//運動方程式
	//F(M(t),N(t),---) の計算(引数のdtは特別な場合以外は参照しない：現在のF(---)を計算する)
	MotionEquation2( DT, dx, dy);

	//F(M(t),N(t),---)から差分法で M(t+Δt),N(t+Δt)を求める
	MotionEquation3(nxttime, cur, DT, dx, dy);
	Data->Updata(nxttime);

	//AbnormalGapFilter();

	const int ysz = jY-ZERO_AREA_WD;
	const int xsz = iX-ZERO_AREA_WD;
	if ( timeIndex % filterCycle4 == 0 )
	{
		CheckerBordCheckSetup();
		//#pragma omp parallel for OMP_SCHEDULE
		for (int ii__ = 0; ii__ < iX*jY; ii__++)
		//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
		{
			int i = ii__ / iX;
			int j = ii__ % iX;
			if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
			{
				continue;
			}
			//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
			{
				//OpenMPを有効にするならこの関数の中でcritical制御が必要！！
				CheckerBordCheck(i,j, 0);
			}
		}
	}
	//Filter(0);


	//求めた流入量M,Nから連続の式で波高を求める
	//F(---,---,W(t)) の計算(引数のdtは特別な場合以外は参照しない：現在のF(---)を計算する)
	ContinuityEquation2(DT, dx, dy);

	//F(---,---,W(t))から差分法で W(t+Δt)を求める
	ContinuityEquation3(nxttime, cur, DT, dx, dy);
	Data->Updata(nxttime);
	//この時点でΔt だけ進んだM(t+Δt),N(t+Δt),W(t+Δt)値が計算された。使ったF(M(t),N(t),W(t))がセットされた状態

	if ( timeIndex % filterCycle4 == 0 )
	{
		CheckerBordCheckSetup();
		//#pragma omp parallel for OMP_SCHEDULE
		for (int ii__ = 0; ii__ < iX*jY; ii__++)
		//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
		{
			int i = ii__ / iX;
			int j = ii__ % iX;
			if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
			{
				continue;
			}
			//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
			{
				//OpenMPを有効にするならこの関数の中でcritical制御が必要！！
				CheckerBordCheck(i,j, 1);
			}
		}
	}
	Filter(1);
}

//流量制限で上端をカットしている場合に滑らかに修正する
int Solver::AbnormalGapFilter()
{
	const int ysz = jY-ZERO_AREA_WD;
	const int xsz = iX-ZERO_AREA_WD;
	const int absorbingZone_ = absorbingZone;
	const int IX = iX;
	const int JY = jY;

	int smooth_peek = filterCycle5;
	if ( timeIndex % smooth_peek != 0) return 0;

	double a, b, c, d, e, f;


	const int _filterOrder = 10;

	if ( _filterOrder == 2 )
	{
		a = 1.0/2.0;
		b = 1.0/2.0;
		c = 0.0;
		d = 0.0;
		e = 0.0;
		f = 0.0;
	}
	if ( _filterOrder == 4 )
	{
		a = 5.0/8.0;
		b = 1.0/2.0;
		c = -1.0/8.0;
		d = 0.0;
		e = 0.0;
		f = 0.0;
	}
	if ( _filterOrder == 6 )
	{
		a = 11.0/16.0;
		b = 15.0/32.0;
		c = -3.0/16.0;
		d = 1.0/32.0;
		e = 0.0;
		f = 0.0;
	}
	if ( _filterOrder == 8 )
	{
		a = 93.0/128.0;
		b = 7.0/16.0;
		c = -7.0/32.0;
		d = 1.0/16.0;
		e = -1.0/128.0;
		f = 0.0;
	}
	if ( _filterOrder == 10 )
	{
		a = 193.0/256.0;
		b = 105.0/256.0;
		c = -15.0/64.0;
		d = 45.0/512.0;
		e = -5.0/256.0;
		f = 1.0/512.0;
	}
	b *= 0.5;
	c *= 0.5;
	d *= 0.5;
	e *= 0.5;
	f *= 0.5;

	const double aa = a*a;
	const double bb = b*b;
	const double cc = c*c;
	const double dd = d*d;
	const double ee = e*e;
	const double ff = f*f;

	
	AbeNoguera* smooth = Data->Copy();

	int timeIndex_ = timeIndex;
	const int RunUp_     = RunUp;

	#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			if ( IsAbsorbingZoneBounray(i,j))
			{
				continue;
			}
		
			bool gap = false;
			//unsigned char gaptype = GAP_3;

			//gap = 
			//	GAP(i-2,j-2) & gaptype ||GAP(i-2,j-1) & gaptype || GAP(i-2,j) & gaptype || GAP(i-2,j+1) & gaptype || GAP(i-2,j+2) & gaptype || 
			//	GAP(i-1,j-2) & gaptype ||GAP(i-1,j-1) & gaptype || GAP(i-1,j) & gaptype || GAP(i-1,j+1) & gaptype || GAP(i-1,j+2) & gaptype || 
			//	GAP(i  ,j-2) & gaptype ||GAP(i  ,j-1) & gaptype || GAP(i  ,j) & gaptype || GAP(i  ,j+1) & gaptype || GAP(i  ,j+2) & gaptype || 
			//	GAP(i+1,j-2) & gaptype ||GAP(i+1,j-1) & gaptype || GAP(i+1,j) & gaptype || GAP(i+1,j+1) & gaptype || GAP(i+1,j+2) & gaptype || 
			//	GAP(i+2,j-2) & gaptype ||GAP(i+2,j-1) & gaptype || GAP(i+2,j) & gaptype || GAP(i+2,j+1) & gaptype || GAP(i+2,j+2) & gaptype;

			//gap =
			//	GAP(i-1,j-1) & gaptype || GAP(i-1,j) & gaptype || GAP(i-1,j+1) & gaptype|| 
			//	GAP(i,  j-1) & gaptype || GAP(i,  j) & gaptype || GAP(i,  j+1) & gaptype|| 
			//	GAP(i+1,j-1) & gaptype || GAP(i+1,j) & gaptype || GAP(i+1,j+1) & gaptype;

			//gap = gap ||
			//	GAP(i-2,j-2) & GAP_1 ||GAP(i-2,j-1) & GAP_1 || GAP(i-2,j) & GAP_1 || GAP(i-2,j+1) & GAP_1 || GAP(i-2,j+2) & GAP_1 || 
			//	GAP(i-1,j-2) & GAP_1 ||GAP(i-1,j-1) & GAP_1 || GAP(i-1,j) & GAP_1 || GAP(i-1,j+1) & GAP_1 || GAP(i-1,j+2) & GAP_1 || 
			//	GAP(i  ,j-2) & GAP_1 ||GAP(i  ,j-1) & GAP_1 || GAP(i  ,j) & GAP_1 || GAP(i  ,j+1) & GAP_1 || GAP(i  ,j+2) & GAP_1 || 
			//	GAP(i+1,j-2) & GAP_1 ||GAP(i+1,j-1) & GAP_1 || GAP(i+1,j) & GAP_1 || GAP(i+1,j+1) & GAP_1 || GAP(i+1,j+2) & GAP_1 || 
			//	GAP(i+2,j-2) & GAP_1 ||GAP(i+2,j-1) & GAP_1 || GAP(i+2,j) & GAP_1 || GAP(i+2,j+1) & GAP_1 || GAP(i+2,j+2) == 1;

			if ( gap )
			{
				if(_filterOrder == 10)
				{
					smooth->M(i,j) = LELEFILTER_10(M,i,j);
					smooth->N(i,j) = LELEFILTER_10(N,i,j);
				}else if (_filterOrder == 8 )
				{
					smooth->M(i,j) = LELEFILTER_8(M,i,j);
					smooth->N(i,j) = LELEFILTER_8(N,i,j);
				}else if (_filterOrder == 6 )
				{
					smooth->M(i,j) = LELEFILTER_6(M,i,j);
					smooth->N(i,j) = LELEFILTER_6(N,i,j);
				}else if (_filterOrder == 4 )
				{
					smooth->M(i,j) = LELEFILTER_4(M,i,j);
					smooth->N(i,j) = LELEFILTER_4(N,i,j);
				}else if (_filterOrder == 2 )
				{
					smooth->M(i,j) = LELEFILTER_2(M,i,j);
					smooth->N(i,j) = LELEFILTER_2(N,i,j);
				}
			}
		}
	}
	Data->Updata(smooth);
	delete smooth;

	GapFlgReste(GAP_3|GAP_2);
	return 0;
}

//数値振動を除去する
int Solver::Filter(int flag)
{
	const int ysz = jY-ZERO_AREA_WD;
	const int xsz = iX-ZERO_AREA_WD;
	const int absorbingZone_ = absorbingZone;
	const int IX = iX;
	const int JY = jY;


//#pragma omp parallel for OMP_SCHEDULE
//	for ( int i = 0; i < JY; i++ )
//	{
//		for ( int j = 0; j < IX; j++ )
//		{
//			if ( fabs(Data->W(i,j)) < 0.0000001 )
//			{
//				Data->W(i,j) *= 0.001;
//			}
//			double q = ( SQR(Data->M(i,j)) + SQR(Data->N(i,j)));
//			if ( q < 1.0e-16 )
//			{
//				Data->M(i,j) *= 0.001;
//				Data->N(i,j) *= 0.001;
//				Data->W(i,j) *= 0.001;
//			}
//		}
//	}

	int smooth_peek1 = filterCycle1;
	int smooth_peek5 = filterCycle4;
	int smooth_peek2 = filterCycle2;


	if ( timeIndex % smooth_peek5 != 0 && timeIndex % smooth_peek1 != 0 && timeIndex % smooth_peek2 != 0)
	{
		return 0;
	}

	double a, b, c, d, e, f;


	int _filterOrder = filterOrder;

	if (  timeIndex % smooth_peek5 == 0 ) _filterOrder = 4;

	if ( _filterOrder == 2 )
	{
		a = 1.0/2.0;
		b = 1.0/2.0;
		c = 0.0;
		d = 0.0;
		e = 0.0;
		f = 0.0;
	}
	if ( _filterOrder == 4 )
	{
		a = 5.0/8.0;
		b = 1.0/2.0;
		c = -1.0/8.0;
		d = 0.0;
		e = 0.0;
		f = 0.0;
	}
	if ( _filterOrder == 6 )
	{
		a = 11.0/16.0;
		b = 15.0/32.0;
		c = -3.0/16.0;
		d = 1.0/32.0;
		e = 0.0;
		f = 0.0;
	}
	if ( _filterOrder == 8 )
	{
		a = 93.0/128.0;
		b = 7.0/16.0;
		c = -7.0/32.0;
		d = 1.0/16.0;
		e = -1.0/128.0;
		f = 0.0;
	}
	if ( _filterOrder == 10 )
	{
		a = 193.0/256.0;
		b = 105.0/256.0;
		c = -15.0/64.0;
		d = 45.0/512.0;
		e = -5.0/256.0;
		f = 1.0/512.0;
	}
	b *= 0.5;
	c *= 0.5;
	d *= 0.5;
	e *= 0.5;
	f *= 0.5;

	const double aa = a*a;
	const double bb = b*b;
	const double cc = c*c;
	const double dd = d*d;
	const double ee = e*e;
	const double ff = f*f;

	
	AbeNoguera* smooth = Data->Copy();

	int timeIndex_ = timeIndex;
	int RunUp_     = RunUp;

	//if ( timeIndex_ % smooth_peek5 == 0)
	//{
	//	CheckerBordCheckSetup();
	//	//#pragma omp parallel for OMP_SCHEDULE
	//	for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	//	{
	//		for ( int j = ZERO_AREA_WD; j < xsz; j++ )
	//		{
	//			//OpenMPを有効にするならこの関数の中でcritical制御が必要！！
	//			CheckerBordCheck(i,j);
	//		}
	//	}
	//	//WriteCsv("chk\\圧力振動.csv", IX, JY, CheckerBord,1.0);
	//}

	#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			if ( IsAbsorbingZoneBounray(i,j))
			{
				continue;
			}

			if ( RunUp_ && (RUNUPFLG2(i,j) == IS_DRY || RUNUPFLG(i, j) == IS_DRY))
			{
				smooth->W(i, j) = 0.0;
				smooth->M(i, j) = 0.0;
				smooth->N(i, j) = 0.0;
				continue;
			}
			if (!RunUp_ && T(i,j) == 0 )
			{
				continue;
			}
		
			bool checkerBordCheck[3] = {false, false, false};

			if ( timeIndex_ % smooth_peek5 == 0)
			{
				checkerBordCheck[0] = isCheckerBord(0, i,j);
				checkerBordCheck[1] = isCheckerBord(1, i,j);
				checkerBordCheck[2] = isCheckerBord(2, i,j);
				if ( !checkerBordCheck[0] && !checkerBordCheck[1] && !checkerBordCheck[2]) continue;
#ifdef CHACKER_BORD_DEBUG
				continue;
#endif
			}
			if ( timeIndex_ % smooth_peek1 == 0)
			{
				checkerBordCheck[1] = true;
				checkerBordCheck[2] = true;
			}
			if ( timeIndex_ % smooth_peek2 == 0)
			{
				checkerBordCheck[0] = true;
			}

			if(_filterOrder == 10)
			{
				if ( flag == 1 && checkerBordCheck[0] ) smooth->W(i,j) = LELEFILTER_10(W,i,j);
				if ( flag == 0 && checkerBordCheck[1] ) smooth->M(i,j) = LELEFILTER_10(M,i,j);
				if ( flag == 0 && checkerBordCheck[2] ) smooth->N(i,j) = LELEFILTER_10(N,i,j);
			}else if (_filterOrder == 8 )
			{
				if ( flag == 1 && checkerBordCheck[0] ) smooth->W(i,j) = LELEFILTER_8(W,i,j);
				if ( flag == 0 && checkerBordCheck[1] ) smooth->M(i,j) = LELEFILTER_8(M,i,j);
				if ( flag == 0 && checkerBordCheck[2] ) smooth->N(i,j) = LELEFILTER_8(N,i,j);
			}else if (_filterOrder == 6 )
			{
				if ( flag == 1 && checkerBordCheck[0] ) smooth->W(i,j) = LELEFILTER_6(W,i,j);
				if ( flag == 0 && checkerBordCheck[1] ) smooth->M(i,j) = LELEFILTER_6(M,i,j);
				if ( flag == 0 && checkerBordCheck[2] ) smooth->N(i,j) = LELEFILTER_6(N,i,j);
			}else if (_filterOrder == 4 )
			{
				if ( flag == 1 && checkerBordCheck[0] ) smooth->W(i,j) = LELEFILTER_4(W,i,j);
				if ( flag == 0 && checkerBordCheck[1] ) smooth->M(i,j) = LELEFILTER_4(M,i,j);
				if ( flag == 0 && checkerBordCheck[2] ) smooth->N(i,j) = LELEFILTER_4(N,i,j);
			}else if (_filterOrder == 2 )
			{
				if ( flag == 1 && checkerBordCheck[0] ) smooth->W(i,j) = LELEFILTER_2(W,i,j);
				if ( flag == 0 && checkerBordCheck[1] ) smooth->M(i,j) = LELEFILTER_2(M,i,j);
				if ( flag == 0 && checkerBordCheck[2] ) smooth->N(i,j) = LELEFILTER_2(N,i,j);
			}

			if ( RunUp_ && (smooth->W(i,j) < 0.0 && (RUNUPFLG2(i, j) == IS_DRY || RUNUPFLG(i, j) == IS_DRY)))
			{
				//if ( W(i,j) >= 0.0 ) printf("^^^^^^^^^^^^^^^^^^^^^^^^^ %.8f %.8f\n", smooth->W(i,j), W(i,j));
				smooth->W(i,j) = 0.0;
				smooth->M(i,j) = 0.0;
				smooth->N(i,j) = 0.0;
			}
		}
	}

#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			const double h1 = H(i,j);
			const double h2 = (H(i-1,j)+H(i+1,j)+H(i,j-1)+H(i,j+1))*0.25;
			double h = h1;
			if ( h1 > h2 ) h = h2;

			if ( T(i,j) != 0 && smooth->W(i,j) < -h2 )
			{
				smooth->W(i,j) = -h2*0.999;
			}
		}
	}

	//if ( !RunUp_ )
	//{
	//	//境界の値を元に戻す
	//	#pragma omp parallel for OMP_SCHEDULE
	//	for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	//	{
	//		for ( int j = ZERO_AREA_WD; j < xsz; j++ )
	//		{
	//			if ( IsAbsorbingZoneBounray(i,j))
	//			{
	//				continue;
	//			}
	//			if ( T(i,j) == 0 )
	//			{
	//				smooth->W(i,j) = 0.0;
	//				smooth->M(i,j) = 0.0;
	//				smooth->N(i,j) = 0.0;
	//			}
	//		}
	//	}
	//}

	Data->Updata(smooth);
	delete smooth;
	return 0;
}


int Solver::Iterat(int method)
{
	if ( !setup) return -99;

	AbeNoguera* nxttime = Data->Copy();


	if ( method == 1 )
	{
		AbeNoguera* pre = 0;

		if ( solver < 3 )
		{
			eval(nxttime, Data, dt);
		}

		if ( solver >= 3 )
		{
			const int IX = iX;
			const int JY = jY;
			const double ds = (dx < dy) ? dx:dy;


			tdiff.Updata(Data->right);
			const double inv_dt = 1.0/dt;
			const double r_dy = 1.0/dy;

			#pragma omp parallel for
			for (int ii__ = 0; ii__ < iX*jY; ii__++)
			//for ( int i = ZERO_AREA_WD; i < jY-ZERO_AREA_WD; i++ )
			{
				int i = ii__ / iX;
				int j = ii__ % iX;
				if (i < ZERO_AREA_WD || i >= jY - ZERO_AREA_WD || j < ZERO_AREA_WD || j >= iX - ZERO_AREA_WD)
				{
					continue;
				}
				const double r_dx = R_DX(i);
				//for ( int j = ZERO_AREA_WD; j < iX-ZERO_AREA_WD; j++ )
				{
					nxttime->M(i,j) = Data->M(i,j) - g*D2(i,j)*DIFFx(Data->w, i, j)*r_dx*dt;
					nxttime->N(i,j) = Data->N(i,j) - g*D2(i,j)*DIFFy(Data->w, i, j)*r_dy*dt;
				}
			}

			#pragma omp parallel for
			for (int ii__ = 0; ii__ < iX*jY; ii__++)
			//for ( int i = ZERO_AREA_WD; i < jY-ZERO_AREA_WD; i++ )
			{
				int i = ii__ / iX;
				int j = ii__ % iX;
				if (i < ZERO_AREA_WD || i >= jY-ZERO_AREA_WD || j < ZERO_AREA_WD || j >= iX - ZERO_AREA_WD)
				{
					continue;
				}
				const double r_dx = R_DX(i);
				//for ( int j = ZERO_AREA_WD; j < iX-ZERO_AREA_WD; j++ )
				{
					nxttime->W(i,j) =  Data->W(i,j) + (-DIFFx(nxttime->m, i,j)*r_dx -DIFFy(nxttime->n, i,j)*r_dy)*dt;
				}
			}

			#pragma omp parallel for
			for (int ii__ = 0; ii__ < iX*jY; ii__++)
			//for ( int i = ZERO_AREA_WD; i < jY-ZERO_AREA_WD; i++ )
			{
				int i = ii__ / iX;
				int j = ii__ % iX;
				if (i < ZERO_AREA_WD || i >= jY - ZERO_AREA_WD || j < ZERO_AREA_WD || j >= iX - ZERO_AREA_WD)
				{
					continue;
				}
				//for ( int j = ZERO_AREA_WD; j < iX-ZERO_AREA_WD; j++ )
				{
					double cf3 = 1.0;

					tdiff.FM(i,j) = cf3*(nxttime->M(i,j) - Data->M(i,j))*inv_dt;
					tdiff.FN(i,j) = cf3*(nxttime->N(i,j) - Data->N(i,j))*inv_dt;
					tdiff.FW(i,j) = cf3*(nxttime->W(i,j) - Data->W(i,j))*inv_dt;
				}
			}
			delete nxttime;
			nxttime = Data->Copy();
			eval(nxttime, Data, dt);
		}

		delete nxttime;
	}
END:	;
	
	timeIndex++;
	if ( wmax > 2.0e4 )
	{
		return -1;
	}
	return 0;
}


void Solver::generalFilter(double* arry)
{
	const int ysz = jY-ZERO_AREA_WD;
	const int xsz = iX-ZERO_AREA_WD;
	const int absorbingZone_ = absorbingZone;
	const int IX = iX;
	const int JY = jY;

	double a, b, c, d, e, f;


	a = 193.0/256.0;
	b = 105.0/256.0;
	c = -15.0/64.0;
	d = 45.0/512.0;
	e = -5.0/256.0;
	f = 1.0/512.0;

	b *= 0.5;
	c *= 0.5;
	d *= 0.5;
	e *= 0.5;
	f *= 0.5;

	const double aa = a*a;
	const double bb = b*b;
	const double cc = c*c;
	const double dd = d*d;
	const double ee = e*e;
	const double ff = f*f;

	
	double* arry_wrk = new double[IX*JY];

	memcpy(arry_wrk, arry, sizeof(double)*IX*JY);

	tmpX = arry;

	#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			if ( IsAbsorbingZoneBounray(i,j))
			{
				continue;
			}
			ARY_wrk(arry_wrk, i,j) = LELEFILTER_10(ARY_wrkX,i,j);
		}
	}
	memcpy(arry, arry_wrk, sizeof(double)*IX*JY);
	delete [] arry_wrk;
}

void Solver::generalFilter(double* arry1, double* arry2)
{
	const int ysz = jY-ZERO_AREA_WD;
	const int xsz = iX-ZERO_AREA_WD;
	const int absorbingZone_ = absorbingZone;
	const int IX = iX;
	const int JY = jY;

	double a, b, c, d, e, f;


	a = 193.0/256.0;
	b = 105.0/256.0;
	c = -15.0/64.0;
	d = 45.0/512.0;
	e = -5.0/256.0;
	f = 1.0/512.0;

	b *= 0.5;
	c *= 0.5;
	d *= 0.5;
	e *= 0.5;
	f *= 0.5;

	const double aa = a*a;
	const double bb = b*b;
	const double cc = c*c;
	const double dd = d*d;
	const double ee = e*e;
	const double ff = f*f;

	
	double* arry1_wrk = new double[IX*JY];
	double* arry2_wrk = new double[IX*JY];

	memcpy(arry1_wrk, arry1, sizeof(double)*IX*JY);
	memcpy(arry2_wrk, arry2, sizeof(double)*IX*JY);

	tmpX = arry1;
	tmpY = arry2;

	#pragma omp parallel for OMP_SCHEDULE
	for (int ii__ = 0; ii__ < iX*jY; ii__++)
	//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
	{
		int i = ii__ / iX;
		int j = ii__ % iX;
		if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
		{
			continue;
		}
		//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
		{
			if ( IsAbsorbingZoneBounray(i,j))
			{
				continue;
			}
			ARY_wrk(arry1_wrk, i,j) = LELEFILTER_10(ARY_wrkX,i,j);
			ARY_wrk(arry2_wrk, i,j) = LELEFILTER_10(ARY_wrkY,i,j);
		}
	}
	memcpy(arry1, arry1_wrk, sizeof(double)*IX*JY);
	memcpy(arry2, arry2_wrk, sizeof(double)*IX*JY);
	delete [] arry1_wrk;
	delete [] arry2_wrk;
}


// leng:活断層の長さ
// delta:断層までの距離
double Solver::InitialHeght(double leng, double delta)
{
	const double mu = 3.50e10;	//[N/m^2] 剛性率
	const double C = 0.2;

	double Mw;		//モーメントマグニチュード
	double M0;		//地震モーメント
	double Ht;

	leng *= 0.001;
	delta *= 0.001;

	double W = leng/1.5;	//断層幅（武村(1998)
	printf("断層幅:%f[m]\n", W);

	if ( leng < 22.5 )
	{
		double Dt;				//すべり量

		Mw = (log10(leng)+3.77)/0.75;
		M0 = pow(10.0, 1.5*Mw+9.1); //[N/m]
		Dt = 0.000001*M0/(mu*leng*W);

		printf("断層すべり量:%f[m]\n", Dt);

		
	}else
	{
		printf("断層すべり量:%s[m]\n", "------");

		//武村(1998)
		M0 = pow(10.0, 2.0*log10(leng)+16.64);

		//Kanamori(1977)
		Mw = (log10(M0)-9.1)/1.5;
	}

	printf("モーメントマグニチュードMw:%g\n", Mw);
	printf("地震モーメントM0:%g[Nm]\n", M0);

	//阿部(1989)の予測式から津波高さを算出
	double r0 = pow(10.0, 0.5*Mw-2.25);

	double h = 1.5*pow(10.0, 0.5*Mw-3.30+C);
	printf("最大波高さ:%f\n", h);

	if ( delta <= r0 )
	{
		Ht = pow(10.0, 0.5*Mw-3.30+C);
	}else{
		Ht = pow(10.0, Mw - log10(delta)-5.55+C);
	}
	printf("推定津波高さ:%f[m]\n", Ht);
	return h;
}



