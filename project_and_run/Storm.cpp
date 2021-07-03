#include "Storm.h"
#include "tsu.h"

void Storm_Line::SetSolver(void* solv)
{
	solver = solv;
	width = ((Solver*)solver)->iX;
	height = ((Solver*)solver)->jY;
}

bool Storm_Line::isAbsorbingZoneBounray(int i, int j)
{
	Solver* sol = (Solver*)solver;
	return sol->isAbsorbingZoneBounray(i,j);
}
double Storm_Line::absorbingZoneBounray(int i, int j, int flg)
{
	Solver* sol = (Solver*)solver;
	return sol->absorbingZoneBounray(i,j);
}


void Storm_Line::ChkArea()
{
	std::vector<Storm_Point> stormList2;

	AbeNoguera* prm = ((Solver*)solver)->Data;

	const int xsz = prm->iX;
	const int ysz = prm->jY;
	const int IX = prm->iX;

	for ( int i = 0; i < stormList.size(); i++ )
	{
		int dmy;
		Storm_Point a = stormList[i];
		double lon = a.lon;
		double lat = a.lat;
			
		int x = (lon - longitude[0])*(double)(xsz-1)/(longitude[1] - longitude[0]);
		int y = (lat - latitude[2])*(double)(ysz-1)/(latitude[0] - latitude[2]);
			
		bool hit = true;
		for ( int kk = -ABSZONE; kk < ABSZONE; kk++ )
		{
			if ( y+kk < 0 || y+kk >= ysz )
			{
				hit = false;
				continue;
			}
			for ( int ll = -ABSZONE; ll < ABSZONE; ll++ )
			{
				if ( x+ll < 0 || x+ll >=xsz )
				{
					hit = false;
					continue;
				}
				//if ( ((Solver*)solver)->isAbsorbingZoneBounray(y+kk, x+ll))
				//{
				//	hit = false;
				//	continue;
				//}
			}
		}
		if ( !hit )
		{
			printf("[%d] => 計算領域外\n", i);
			if ( stormList2.size() > 0 ) break;
		}
		else
		{
			stormList2.push_back(stormList[i]);
			printf("[%d] => 計算領域内\n", i);
		}
	}
	stormList = stormList2;
	double t = stormList[0].t;
	for ( int i = 0; i < stormList.size(); i++ )
	{
		stormList[i].t -= t;
	}
}

void Storm_Line::putLog()
{
	int num = stormList.size();
	for ( int i = 0; i < num; i++ )
	{
		((Solver*)solver)->log_printf("=============低気圧設定START===============\n");
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

		((Solver*)solver)->log_printf("[%d]時刻:%.3f[s](%.2f[h]) 経度[%d°%d′%d″]緯度[%d°%d′%d″]"
			"\n最大風速半径:%.2f[km]\n中心気圧:%.2f[hPa] 速度:%.2f[km/h]\n\n",
			i, stormList[i].t, stormList[i].t/3600.0, 
			(int)lon[0], 
			(int)lon[1], 
			(int)lon[2],
			(int)lat[0],
			(int)lat[1],
			(int)lat[2], stormList[i].rm/1000.0, stormList[i].pc, stormList[i].v/km_h);
	}
	((Solver*)solver)->log_printf("=============低気圧設定END===============\n\n");
}

void Storm_Line::dumpBmp(char* drive, char* dir, char* IDname, BitMap& topography_data_mask_bmp, BitMap& colormap_default)
{
	AbeNoguera* prm = ((Solver*)solver)->Data;

	char fname[512];
	BitMap bmp3;
	bmp3.Copy(topography_data_mask_bmp);


	sprintf(fname, "%s%schk\\%s_AtmosphericPressure.bmp", drive, dir, IDname);
	printf("AtmosphericPressure=>%s\n", fname);

	const int xsz = prm->iX;
	const int ysz = prm->jY;
	const int IX = prm->iX;
	double pmax, pmin;

	int index = 0;
	int storm_id;
	double* pres = NULL;
	while( (pres = Pressure(NULL, prm->iX, prm->jY, pmin, pmax, storm_id, index)) != NULL )
	{
#pragma omp parallel for OMP_SCHEDULE
		for ( int i = zeroArea; i < ysz-zeroArea; i++ )
		{
			for ( int j = zeroArea; j < xsz-zeroArea; j++ )
			{
				double Pa = pres[i*xsz+j];
				//if ( Pa/hPa < 899.0) continue;
				int color = (colormap_default.W()-1)*(Pa-pmin)/(pmax - pmin);
				if ( color < 0 ) color = 0;
				if ( color >colormap_default.W()) color = colormap_default.W()-1;

				Rgb rgb = colormap_default.cell( colormap_default.H()/2, color);

				const double alp = 0.65;
				double r = (double)bmp3.cell(i,j).r*alp + (double)rgb.r*(1.0-alp);
				double g = (double)bmp3.cell(i,j).g*alp + (double)rgb.g*(1.0-alp);
				double b = (double)bmp3.cell(i,j).b*alp + (double)rgb.b*(1.0-alp);
						
				if ( r > 255.0 ) r = 255.0;
				else if ( r <0.0 ) r = 0.0;
				if ( g > 255.0 ) g = 255.0;
				else if ( g <0.0 ) g = 0.0;
				if ( b > 255.0 ) b = 255.0;
				else if ( b <0.0 ) b = 0.0;
				bmp3.cell(i,j) = Rgb(r, g, b);
			}
		}
		delete [] pres;
		index++;
	}
	bmp3.Write(fname);
}

void Storm_Line::dumpBmpWaterLevelRaising(char* drive, char* dir, char* IDname, BitMap& topography_data_mask_bmp, BitMap& colormap_default)
{
	AbeNoguera* prm = ((Solver*)solver)->Data;

	char fname[512];
	BitMap bmp3;
	bmp3.Copy(topography_data_mask_bmp);


	sprintf(fname, "%s%schk\\%s_WaterLevelRaising.bmp", drive, dir, IDname);
	printf("WaterLevelRaising=>%s\n", fname);

	const int xsz = prm->iX;
	const int ysz = prm->jY;
	const int IX = prm->iX;
	double pmax, pmin;

	int index = 0;
	int storm_id;
	double* waterheight = NULL;
	while( (waterheight = WaterLevelRaising(NULL, prm->iX, prm->jY, pmin, pmax, storm_id, index)) != NULL )
	{
#pragma omp parallel for OMP_SCHEDULE
		for ( int i = zeroArea; i < ysz-zeroArea; i++ )
		{
			for ( int j = zeroArea; j < xsz-zeroArea; j++ )
			{
				double wh = waterheight[i*xsz+j];
				int color = (colormap_default.W()-1)*(wh-pmin)/(pmax - pmin);
				if ( color < 0 ) color = 0;
				if ( color >colormap_default.W()) color = colormap_default.W()-1;

				Rgb rgb = colormap_default.cell( colormap_default.H()/2, color);

				const double alp = 0.65;
				double r = (double)bmp3.cell(i,j).r*alp + (double)rgb.r*(1.0-alp);
				double g = (double)bmp3.cell(i,j).g*alp + (double)rgb.g*(1.0-alp);
				double b = (double)bmp3.cell(i,j).b*alp + (double)rgb.b*(1.0-alp);
						
				if ( r > 255.0 ) r = 255.0;
				else if ( r <0.0 ) r = 0.0;
				if ( g > 255.0 ) g = 255.0;
				else if ( g <0.0 ) g = 0.0;
				if ( b > 255.0 ) b = 255.0;
				else if ( b <0.0 ) b = 0.0;
				bmp3.cell(i,j) = Rgb(r, g, b);
			}
		}
		delete [] waterheight;
		index++;
	}
	bmp3.Write(fname);
}

void Storm_Line::dumpBmpCenter(char* drive, char* dir, char* IDname, BitMap& topography_data_mask_bmp, BitMap& colormap_default)
{
	AbeNoguera* prm = ((Solver*)solver)->Data;

	char fname[512];
	BitMap bmp3;
	bmp3.Copy(topography_data_mask_bmp);


	sprintf(fname, "%s%schk\\%s_Center.bmp", drive, dir, IDname);
	printf("WaterLevelRaising=>%s\n", fname);

	const int xsz = prm->iX;
	const int ysz = prm->jY;
	const int IX = prm->iX;
	double pmax, pmin;

	for ( int i = 0; i < stormList.size(); i++ )
	{
		int dmy;
		Storm_Point a = stormList[i];
		double lon = a.lon;
		double lat = a.lat;
			
		int x = (lon - longitude[0])*(double)(xsz-1)/(longitude[1] - longitude[0]);
		int y = (lat - latitude[2])*(double)(ysz-1)/(latitude[0] - latitude[2]);
			
		bool hit = true;
		for ( int kk = -3; kk < 3; kk++ )
		{
			if ( y+kk < 0 || y+kk >= bmp3.H() )
			{
				hit = false;
				continue;
			}
			for ( int ll = -3; ll < 3; ll++ )
			{
				if ( x+ll < 0 || x+ll >= bmp3.W() )
				{
					hit = false;
					continue;
				}
				bmp3.cell(y+kk, x+ll) = Rgb(255, 255, 255);
			}
		}
		if ( !hit ) printf("[%d] => 計算領域外\n", i);
		else printf("[%d] => 計算領域内\n", i);
	}
	bmp3.Write(fname);
}

void Storm_Line:: windVector(BitMap& bmp)
{
	AbeNoguera* prm = ((Solver*)solver)->Data;
	if ( prm == 0 ) return;

	int storm_id;
	Storm_Point a = get(time, storm_id);

	float wmax = -9999999.0f;
	const int xsz = prm->iX;
	const int ysz = prm->jY;
	const int IX = prm->iX;
	const int JY = prm->jY;

	const double dlo = (longitude[1] - longitude[0])/(double)(width-1);
	const double dla = (latitude[0] - latitude[2])/(double)(height-1);

	const float maxwind = VECTOT_MAXWIND;				//ベクトル最大ピクセル長をMAXとする最大風速
	const float vector_length_max = VECTOR_LENGTH_MAX;	//ベクトル最大ピクセル長

	BitMap copyBmp;
	copyBmp.Copy(bmp);
	{
		ijTodistancs(&a, IX, JY);
		int step = VECTOR_STEP;

#pragma omp parallel for OMP_SCHEDULE
		for ( int i = zeroArea+step; i < ysz-zeroArea; i += step )
		{
			for ( int j = zeroArea+step; j < xsz-zeroArea; j += step)
			{
				double lon = j*dlo + longitude[0];
				double lat = i*dla + latitude[2];

				//風ベクトルの計算
				double wx, wy, r;
				Wind(a, width, height, i, j, lon, lat, wx, wy, r);
				float ww = sqrt(wx*wx + wy*wy);
				if ( ww < 10.0f )
				{
					continue;
				}
				//if ( r > a.rm*5.0 )
				//{
				//	continue;
				//}

				//風ベクトルの単位化
				float xx = wx/ww;
				float yy = wy/ww;

				//風ベクトルのピクセル長
				float length = vector_length_max*ww/maxwind;
				if ( length > vector_length_max ) length = vector_length_max;

				float dt = 1.0/length;
				float t = 0.0f;

				while( /*length > 16.0f &&*/ t <= length)
				{
					int x = (int)(j + xx*t);
					int y = (int)(i + yy*t);
					if ( x < zeroArea || x > IX-zeroArea || y < zeroArea || y > JY-zeroArea )
					{
						t += dt;
						continue;
					}
					bmp.cell(y,x).r = 255;
					bmp.cell(y,x).g = 255;
					bmp.cell(y,x).b = 255;

					float alp = 0.8*t/length;
					float r = (float)copyBmp.cell(y,x).r*alp + (float)bmp.cell(i,j).r*(1.0-alp);
					float g = (float)copyBmp.cell(y,x).g*alp + (float)bmp.cell(i,j).g*(1.0-alp);
					float b = (float)copyBmp.cell(y,x).b*alp + (float)bmp.cell(i,j).b*(1.0-alp);
					if ( r < 0 ) r = 0.0f;
					if ( g < 0 ) g = 0.0f;
					if ( b < 0 ) b = 0.0f;
					if ( r > 255 ) r = 255.0f;
					if ( g > 255 ) g = 255.0f;
					if ( b > 255 ) b = 255.0f;
					bmp.cell(y,x).r = r;
					bmp.cell(y,x).g = g;
					bmp.cell(y,x).b = b;
					t += dt;

#if 0
					for ( int ki = -1; ki <= 1; ki++ )
					{
						for ( int kj = -1; kj <= 1; kj++ )
						{
							if ( j+kj < zeroArea || j+kj > IX-zeroArea || i+ki < zeroArea || i+ki > JY-zeroArea )
							{
								continue;
							}
							bmp.cell(i+ki,j+kj).r = 255;
							bmp.cell(i+ki,j+kj).g = 206;
							bmp.cell(i+ki,j+kj).b = 231;

							float alp = 0.5f;
							float r = (float)copyBmp.cell(i+ki,j+kj).r*alp + (float)bmp.cell(i+ki,j+kj).r*(1.0-alp);
							float g = (float)copyBmp.cell(i+ki,j+kj).g*alp + (float)bmp.cell(i+ki,j+kj).g*(1.0-alp);
							float b = (float)copyBmp.cell(i+ki,j+kj).b*alp + (float)bmp.cell(i+ki,j+kj).b*(1.0-alp);
							if ( r < 0 ) r = 0.0f;
							if ( g < 0 ) g = 0.0f;
							if ( b < 0 ) b = 0.0f;
							if ( r > 255 ) r = 255.0f;
							if ( g > 255 ) g = 255.0f;
							if ( b > 255 ) b = 255.0f;
							bmp.cell(i+ki,j+kj).r = r;
							bmp.cell(i+ki,j+kj).g = g;
							bmp.cell(i+ki,j+kj).b = b;
						}
					}
#endif
				}
			}
		}
		ijTodistancsClear();
	}
}

int** Storm_Line:: windVector()
{
	AbeNoguera* prm = ((Solver*)solver)->Data;
	if ( prm == 0 ) return 0;

	int storm_id;
	Storm_Point a = get(time, storm_id);

	float wmax = -9999999.0f;
	const int xsz = prm->iX;
	const int ysz = prm->jY;
	const int IX = prm->iX;
	const int JY = prm->jY;

	const double dlo = (longitude[1] - longitude[0])/(double)(width-1);
	const double dla = (latitude[0] - latitude[2])/(double)(height-1);

	const float maxwind = VECTOT_MAXWIND;				//ベクトル最大ピクセル長をMAXとする最大風速
	const float vector_length_max = VECTOR_LENGTH_MAX;	//ベクトル最大ピクセル長

	int** index = new int*[IX*JY];
	memset(index, '\0', sizeof(int*)*IX*JY);

	for ( int i = 0; i < IX*JY; i++ )
	{
		index[i] = new int[2];
		index[i][0] = -1;
		index[i][1] = -1;
	}
	{
		ijTodistancs(&a, IX, JY);
		int step = VECTOR_STEP;

#pragma omp parallel for OMP_SCHEDULE
		for ( int i = zeroArea+step; i < ysz-zeroArea; i += step )
		{
			for ( int j = zeroArea+step; j < xsz-zeroArea; j += step)
			{
				double lon = j*dlo + longitude[0];
				double lat = i*dla + latitude[2];

				//風ベクトルの計算
				double wx, wy, r;
				Wind(a, width, height, i, j, lon, lat, wx, wy, r);
				float ww = sqrt(wx*wx + wy*wy);
				if ( ww < 6.0f )
				{
					continue;
				}
				//if ( r > a.rm*5.0 )
				//{
				//	continue;
				//}

				//風ベクトルの単位化
				float xx = wx/ww;
				float yy = wy/ww;

				//風ベクトルのピクセル長
				float length = vector_length_max*ww/maxwind;
				if ( length > vector_length_max ) length = vector_length_max;

				float dt = 1.0/length;
				float t = 0.0f;

				while( /*length > 16.0f &&*/ t <= length)
				{
					int x = (int)(j + xx*t);
					int y = (int)(i + yy*t);
					if ( x < zeroArea || x > IX-zeroArea || y < zeroArea || y > JY-zeroArea )
					{
						t += dt;
						continue;
					}
					index[i*IX+j][0] = y; 
					index[i*IX+j][1] = x; 
					t += dt;
				}
			}
		}
		ijTodistancsClear();
	}
	return index;
}

void Storm_Line::windVectorDelete(int** index)
{
	AbeNoguera* prm = ((Solver*)solver)->Data;
	if ( prm == 0 ) return;
	const int IX = prm->iX;
	const int JY = prm->jY;
	for ( int i = 0; i <IX*JY; i++ )
	{
		delete [] index[i];
	}
	delete [] index;
}