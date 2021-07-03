#ifndef _CSV2OBJ_H

#define _CSV2OBJ_H

#include <math.h>
#include "bitmap.h"

class VertexColor
{
public:
	int r;
	int g;
	int b;
	int id;
	VertexColor()
	{
		id = -1;
	}
};

class CsvToObj
{
	double* data;
	double* offset_height;
	int* offset_flg;
	int w;
	int h;

	static double normalize_scale;
	void minmaxUpdate(double x, double y, double z)
	{
		if (!scan) return;

		if ( x < min[0] ) min[0] = x;
		if ( x > max[0] ) max[0] = x;
		if ( y < min[1] ) min[1] = y;
		if ( y > max[1] ) max[1] = y;
		if ( z < min[2] ) min[2] = z;
		if ( z > max[2] ) max[2] = z;

		size[0] = max[0] - min[0];
		size[1] = max[1] - min[1];
		size[2] = max[2] - min[2];

		mid[0] = 0.5*(max[0] + min[0]);
		mid[1] = 0.5*(max[1] + min[1]);
		mid[2] = 0.5*(max[2] + min[2]);

		double s = size[0];
		if(size[1] > s)
		s = size[1];
		if(size[2] > s)
		s = size[2];
		normalize_scale = length/s;
	}

	void Normalize(double& x, double& y, double& z)
	{
		if ( scan || !normalization ) return;
		x = normalize_scale*(x - mid[0]);
		y = normalize_scale*(y - mid[1]);
		z = normalize_scale*(z - mid[2]);
	}

public:
	double dx;
	double dy;
	double zscale;
	double zscale2;
	double zscale3;
	int sign;
	int rgb[3];
	bool use_color;
	double minz;

	double* r_dx;
	double* latitude;
	double* longitude;
	int exp_scale;
	int smooth_limit;

	double globalScale;
	VertexColor* vertex_color;

	double offset;
	static double min[3];
	static double max[3];
	static double mid[3];
	static double size[3];
	static double length;
	
	bool scan;
	bool normalization;

	int** vectorIndex;
	double* alph;

	int vtk_export;
	BitMap* colormap;

	inline double& Vertex(int i, int j)
	{
		return data[i*w+j];
	}
	inline VertexColor& vertexColor(int i, int j)
	{
		return vertex_color[i*w+j];
	}

	CsvToObj() :alph(0), sign(1), r_dx(0), latitude(0), longitude(0), exp_scale(0), vectorIndex(0), globalScale(1.0), scan(false), normalization(false)
	{}

	CsvToObj(double* d, int xx, int yy, double* offsetv=0, int* offsetflg=0)
	{
		vtk_export = 0;
		alph = 0;
		normalization = false;
		scan = false;
		globalScale = 1.0;
		vectorIndex = 0;
		smooth_limit = 0;
		exp_scale = 0;
		latitude = 0;
		longitude = 0;
		r_dx = 0;
		offset_height = offsetv;
		offset_flg = offsetflg;
		data = d;
		w = xx;
		h = yy;
		dx = 1.0;
		dy = 1.0;
		sign = 1;
		minz = 0.0;
		offset = 0.0;
		use_color = false;
		vertex_color = NULL;
		minz = 0.0;
		zscale = 1.0;
		zscale2 = 1.0;
		zscale3 = 1.0;
	}

	void Color(int r, int g, int b)
	{
		rgb[0] = r;
		rgb[1] = g;
		rgb[2] = b;
		use_color = true;
	}

	void Output(char* fname, int smooth_opt = 0, int* clipFlgP = 0, double* depth=0, double* height=0)
	{
		printf("h %d w %d\n", h, w);
		char* depthdown = getenv("DEPTH_DOWN");
		double depth_down = 0.0;
		if ( depthdown ) depth_down = atof(depthdown);

RESTART:	;
		double t = 1.0/(double)(h-1);
		double s = 1.0/(double)(w-1);

		if ( vertex_color ) use_color = true;

		double* data2 = NULL;

		double* data_org = 0;
		if (height)
		{
			data_org = data;
			data = height;
		}
		if ( smooth_opt == 1 ) data2 = smoothL();
		else if ( smooth_opt == 2 ) data2 = smooth();
		else if ( smooth_opt == 3 )
		{
			data2 = new double[w*h];
			memcpy(data2, data, sizeof(double)*w*h);
			printf("LaplacianSmoothing ");fflush(stdout);
			for ( int i = 0; i < 7; i++ )
			{
				double z = 1.5*(7.0 - (double)i)/7.0;
				if ( z < 0.3 ) z = 0.3;
				laplacianSmoothing(data2, 0, 0, 0.75, z);
				printf(" [%d/%d]",i+1, 7);fflush(stdout);
			}
			printf("\n");fflush(stdout);
		}
		else data2 = data;

		if (height)
		{
			data_org = data;
		}

		FILE* fp = NULL;
		
		if ( !scan ) fp = fopen(fname, "w");
		if ( fp && !vtk_export) fprintf(fp, "#ZSCALE:%f\n", zscale);

		if (vtk_export)
		{
			fprintf(fp, "# vtk DataFile Version 2.0\n");
			fprintf(fp, "Header\n");
			fprintf(fp, "ASCII\n");
			fprintf(fp, "DATASET UNSTRUCTURED_GRID\n");
			fprintf(fp, "POINTS %d float\n", h*w);
		}

		for ( int i = 0; i < h; i++ )
		{
			for ( int j = 0; j < w; j++ )
			{
				if ( r_dx ) dx = 1.0/r_dx[i];

				double z = data2[i*w+j];

				z += offset;
				if ( sign < 0 )
				{
					z = -z+depth_down;
				}

				if ( clipFlgP && clipFlgP[i*w+j] )
				{
					z = minz;
				}
				if ( clipFlgP && clipFlgP[i*w+j] == 0 && depth == 0 && height == 0)
				{
					if ( z < 0.0 ) z = 0.1;
				}

				double z2 = z;
				//if ( offset_height && offset_flg )
				//{
				//	if ( offset_flg[i*w+j] == 0 && fabs(data2[i*w+j]) > 0.1 )
				//	{
				//		z2 += zscale2*offset_height[i*w+j]/zscale;
				//	}
				//}

				double alp2 = 1.0;

				if (alph)
				{
					alp2 = alph[i*w + j];
				}
				if ( depth )
				{
					if ( z2 + depth[i*w+j] <= 3050.0 )
					{
						double x = 0.8;
						double y = 1.0 - x;
						alp2 = y*(z2 + depth[i*w+j])/3050.0 + x;
						if ( alp2 >= 1.0 ) alp2 = 1.0;
					}

					////äCíÍòIèo
					//if ( height && height[i*w+j] == 0.0 )
					//{
					//	if ( z2 + depth[i*w+j] < 0.3 ) z2 = -depth[i*w+j]-100.0;
					//}else
					//{
					//	if ( z2 < -depth[i*w+j] )
					//	{
					//		z2 = -depth[i*w+j]*zscale3/zscale + 0.1/zscale;
					//	}
					//}
				}


				//if (height && height[i*w + j] != 0)
				//{
				//	//z2 = z2 + height[i*w + j] / zscale;
				//	z2 = 1.0/zscale + height[i*w + j] / zscale;
				//}
				z2 = z2*zscale;
				//if ( height )
				//{
				//	z2 += height[i*w+j];
				//}
				//if ( exp_scale )
				//{
				//	double zzz = z;
				//	if ( zzz > 5.0 ) zzz = 5.0;
				//	if ( zzz < -5.0 ) zzz = -5.0;

				//	z2 = z2*(1.0+0.02*exp(zzz*zzz*0.25));
				//}

				if ( longitude && latitude )
				{
					double lat = latitude[2]*(1.0 - i*t) + latitude[0]*i*t;
					double lon = longitude[0]*(1.0 - j*s) + longitude[1]*j*s;

					const double f84 = 1.0/298.257223563;
					const double a = 6378137.000;
					const double e2 = f84*(2.0 - f84);
					
					double ee = 1.0 - e2*sin(lat)*sin(lat);
					if ( ee < 0.0 ) ee = 0.0;
					const double N = a/sqrt(ee);

					double x = (N+z2)*cos(lat)*cos(lon);
					double y = (N+z2)*cos(lat)*sin(lon);
					double zz = (N*(1.0 -e2)+z2)*sin(lat);

					if ( scan ) minmaxUpdate(x,y,zz);
					Normalize(x, y, zz);
					if (!use_color )
					{
						if ( fp && !vtk_export) fprintf(fp, "v %f %f %f\n", x*globalScale, y*globalScale, zz*globalScale);
					}else
					{
						if ( vertex_color == NULL )
						{
							if ( fp && !vtk_export) fprintf(fp, "v %f %f %f %d %d %d\n", x*globalScale, y*globalScale, zz*globalScale, rgb[0], rgb[1], rgb[2]);
							if (vtk_export && fp)
							{
								fprintf(fp, "%f %f %f\n", x*globalScale, y*globalScale, zz*globalScale);
							}
						}else
						{
							if ( fp )
							{
								if ( depth )
								{
									if (!vtk_export)fprintf(fp, "v %f %f %f %d %d %d %d %.2f\n", x*globalScale, y*globalScale, zz*globalScale, vertexColor(i,j).r, vertexColor(i,j).g, vertexColor(i,j).b, vertexColor(i,j).id, alp2);
									if (vtk_export && fp)
									{
										fprintf(fp, "%f %f %f\n", x*globalScale, y*globalScale, zz*globalScale);
									}
								}else
								{
									if (!vtk_export)fprintf(fp, "v %f %f %f %d %d %d %d\n", x*globalScale, y*globalScale, zz*globalScale, vertexColor(i,j).r, vertexColor(i,j).g, vertexColor(i,j).b, vertexColor(i,j).id);
									if (vtk_export && fp)
									{
										fprintf(fp, "%f %f %f\n", x*globalScale, y*globalScale, zz*globalScale);
									}
								}
							}
						}
					}
				}else
				{
					double x = dx*j;
					double y = dy*i;
					if ( scan ) minmaxUpdate(x,y,z2);
					Normalize(x, y, z2);
					if (!use_color )
					{
						if ( fp && !vtk_export) fprintf(fp, "v %f %f %f\n", x*globalScale, y*globalScale, z2*globalScale);
					}else
					{
						if ( vertex_color == NULL )
						{
							if ( fp && !vtk_export) fprintf(fp, "v %f %f %f %d %d %d\n", x*globalScale, y*globalScale, z2*globalScale, rgb[0], rgb[1], rgb[2]);
							if (vtk_export && fp)
							{
								fprintf(fp, "%f %f %f\n", x*globalScale, y*globalScale, z2*globalScale);
							}
						}else
						{
							if ( fp && !vtk_export) fprintf(fp, "v %f %f %f %d %d %d %d\n", x*globalScale, y*globalScale, z2*globalScale, vertexColor(i,j).r, vertexColor(i,j).g, vertexColor(i,j).b, vertexColor(i,j).id);
							if (vtk_export && fp)
							{
								fprintf(fp, "%f %f %f\n", x*globalScale, y*globalScale, z2*globalScale);
							}
						}
					}
				}
			}
		}

		if (vtk_export && fp)
		{
			fprintf(fp, "CELLS %d %d\n", (h-1)*(w-1)*2, 2*(h - 1)*(w - 1)*(3 + 1));
		}

		for ( int i = 0; i < h-1; i++ )
		{
			for ( int j = 0; j < w-1; j++ )
			{
				if (vtk_export && fp)
				{
					fprintf(fp, "%d %d %d %d\n", 3, i*w + j, i*w + j + 1, (i + 1)*w + j + 1);
					fprintf(fp, "%d %d %d %d\n", 3, i*w + j, (i + 1)*w + j + 1, (i + 1)*w + j);
				}
				if (clipFlgP && (clipFlgP[i*w + j] || clipFlgP[i*w + j + 1] || clipFlgP[(i + 1)*w + j] || clipFlgP[(i+1)*w + j+1]) )
				{
					continue;
				}

				if ( fp && !vtk_export) fprintf(fp, "f %d %d %d\n", i*w+j+1, i*w+j+1+1, (i+1)*w+j+1+1);
				if ( fp && !vtk_export) fprintf(fp, "f %d %d %d\n", i*w+j+1, (i+1)*w+j+1+1, (i+1)*w+j+1);
			}
		}

		if (vtk_export && fp)
		{
			fprintf(fp, "CELL_TYPES %d\n", 2*(h - 1)*(w - 1));
			for (int i = 0; i < h - 1; i++)
			{
				for (int j = 0; j < w - 1; j++)
				{
					fprintf(fp, "%d\n", 5);
					fprintf(fp, "%d\n", 5);
				}
			}

			if (vertex_color != NULL)
			{

				fprintf(fp, "POINT_DATA %d\n", w*h);
				fprintf(fp, "SCALARS scalar%d float\n", vtk_export);
				fprintf(fp, "LOOKUP_TABLE %s%d\n", "table", vtk_export);
				for (int i = 0; i < h; i++)
				{
					for (int j = 0; j < w; j++)
					{
						double z = data2[i*w + j];

						z += offset;
						if (sign < 0)
						{
							z = -z + depth_down;
						}

						if (clipFlgP && clipFlgP[i*w + j])
						{
							z = minz;
						}
						if (clipFlgP && clipFlgP[i*w + j] == 0 && depth == 0 && height == 0)
						{
							if (z < 0.0) z = 0.1;
						}

						if (vtk_export == 3)
						{
							fprintf(fp, "%f\n", z);
						}
						else
						{
							fprintf(fp, "%f\n", (float)vertexColor(i, j).id / colormap->W());
						}
					}
				}
				if (vtk_export < 3)
				{
					fprintf(fp, "LOOKUP_TABLE %s%d %d\n", "table", vtk_export, colormap->W() + 1);
					for (int i = 0; i < colormap->W(); i++)
					{
						fprintf(fp, "%f %f %f 1.0\n", (float)colormap->cell(colormap->H() / 2, i).r / 255.0f, (float)colormap->cell(colormap->H() / 2, i).g / 255.0f, (float)colormap->cell(colormap->H() / 2, i).b / 255.0f);
					}
					fprintf(fp, "%f %f %f 1.0\n", (float)19 / 255.0f, (float)22 / 255.0f, (float)43 / 255.0f);
				}
			}
		}
		if ( fp ) fclose(fp);
		if ( data2 != data ) delete [] data2;


		if ( vectorIndex )
		{
			char fname2[512];
			sprintf(fname2, "%s.vector", fname);
			FILE* fp = NULL;
			
			if ( !scan ) fp = fopen(fname2, "w");

			const double z2 = 4.0*zscale;
			for ( int i = 0; i < h; i++ )
			{
				for ( int j = 0; j < w; j++ )
				{
					if ( r_dx ) dx = 1.0/r_dx[i];

					if ( vectorIndex[i*w+j][0] < 0 || vectorIndex[i*w+j][1] < 0 )
					{
						continue;
					}

					if ( longitude && latitude )
					{
						double lat = latitude[2]*(1.0 - i*t) + latitude[0]*i*t;
						double lon = longitude[0]*(1.0 - j*s) + longitude[1]*j*s;

						const double f84 = 1.0/298.257223563;
						const double a = 6378137.000;
						const double e2 = f84*(2.0 - f84);
					
						double ee = 1.0 - e2*sin(lat)*sin(lat);
						if ( ee < 0.0 ) ee = 0.0;
						double N = a/sqrt(ee);

						double x = (N+z2)*cos(lat)*cos(lon);
						double y = (N+z2)*cos(lat)*sin(lon);
						double zz = (N*(1.0 -e2)+z2)*sin(lat);

						if ( scan ) minmaxUpdate(x,y,zz);
						Normalize(x, y, zz);
						if ( fp ) fprintf(fp, "%f %f %f", x*globalScale, y*globalScale, zz*globalScale);

						int ii = vectorIndex[i*w+j][0];
						int jj = vectorIndex[i*w+j][1];
						lat = latitude[2]*(1.0 - ii*t) + latitude[0]*ii*t;
						lon = longitude[0]*(1.0 - jj*s) + longitude[1]*jj*s;

						ee = 1.0 - e2*sin(lat)*sin(lat);
						if ( ee < 0.0 ) ee = 0.0;
						N = a/sqrt(ee);

						x = (N+z2)*cos(lat)*cos(lon);
						y = (N+z2)*cos(lat)*sin(lon);
						zz = (N*(1.0 -e2)+z2)*sin(lat);
						if ( scan ) minmaxUpdate(x,y,zz);
						Normalize(x, y, zz);
						if ( fp ) fprintf(fp, " %f %f %f\n", x*globalScale, y*globalScale, zz*globalScale);
					}
				}
			}
			if (fp) fclose(fp);
		}
		if (scan)
		{
			scan = false;
			goto RESTART;
		}
	}


	void OutputEarth(char* fname)
	{
		if ( !(longitude && latitude) ) return;

		double pai = 3.14159265358979323;
		int hh = 60;
		int ww = 60;
		double t = 1.0/(double)(hh-1);
		double s = 1.0/(double)(ww-1);

		double longi[2];
		double latit[2];

		longi[0] = 0.0;
		longi[1] = 2.0*pai;

		latit[0] = -pai*0.5;
		latit[1] = pai*0.5;

		double z2 = 0.0;
		FILE* fp = NULL;
		if (!scan ) fp = fopen(fname, "w");
		for ( int i = 0; i < hh; i++ )
		{
			for ( int j = 0; j < ww; j++ )
			{
				double lat = latit[0]*(1.0 - i*t) + latit[1]*i*t;
				double lon = longi[0]*(1.0 - j*s) + longi[1]*j*s;

				const double f84 = 1.0/298.257223563;
				const double a = 6378137.000;
				const double e2 = f84*(2.0 - f84);
					
				double ee = 1.0 - e2*sin(lat)*sin(lat);
				if ( ee < 0.0 ) ee = 0.0;
				const double N = a/sqrt(ee);

				double x = (N+z2)*cos(lat)*cos(lon);
				double y = (N+z2)*cos(lat)*sin(lon);
				double zz = (N*(1.0 -e2)+z2)*sin(lat);

				if ( scan ) minmaxUpdate(x,y,zz);
				Normalize(x, y, zz);
				if ( fp ) fprintf(fp, "v %f %f %f\n", x*globalScale, y*globalScale, zz*globalScale);
			}
		}
		for ( int i = 0; i < hh-1; i++ )
		{
			for ( int j = 0; j < ww-1; j++ )
			{
				if ( fp ) fprintf(fp, "f %d %d %d\n", i*ww+j+1, i*ww+j+1+1, (i+1)*ww+j+1+1);
				if ( fp ) fprintf(fp, "f %d %d %d\n", i*ww+j+1, (i+1)*ww+j+1+1, (i+1)*ww+j+1);
			}
		}
		if ( fp ) fclose(fp);
	}

	void OutputEarth2(char* fname)
	{
		if ( !(longitude && latitude) ) return;
		double pai = 3.14159265358979323;
		int hh = 60;
		int ww = 60;
		double t = 1.0/(double)(hh-1);
		double s = 1.0/(double)(ww-1);

		double longi[2];
		double latit[2];

		longi[0] = 0.0;
		longi[1] = 2.0*pai;

		latit[0] = -pai*0.5;
		latit[1] = pai*0.5;

		double z2 = 0.0;
		FILE* fp = NULL;
		if ( !scan ) fp = fopen(fname, "w");
		for ( int i = 0; i < hh; i++ )
		{
			for ( int j = 0; j < ww; j++ )
			{
				double lat = latit[0]*(1.0 - i*t) + latit[1]*i*t;
				double lon = longi[0]*(1.0 - j*s) + longi[1]*j*s;

				const double f84 = 1.0/298.257223563;
				const double a = 6378137.000;
				const double e2 = f84*(2.0 - f84);
					
				double ee = 1.0 - e2*sin(lat)*sin(lat);
				if ( ee < 0.0 ) ee = 0.0;
				const double N = a/sqrt(ee);

				double x = (N+z2)*cos(lat)*cos(lon);
				double y = (N+z2)*cos(lat)*sin(lon);
				double zz = (N*(1.0 -e2)+z2)*sin(lat);

				if ( scan ) minmaxUpdate(x,y,zz);
				Normalize(x, y, zz);
				if ( fp ) fprintf(fp, "v %f %f %f\n", x*globalScale, y*globalScale, zz*globalScale);
			}
		}
		for ( int i = 0; i < hh-1; i++ )
		{
			for ( int j = 0; j < ww-1; j++ )
			{
				if ( fp ) fprintf(fp, "f %d %d %d %d\n", i*ww+j+1, i*ww+j+1+1, (i+1)*ww+j+1+1, (i+1)*ww+j+1);
			}
		}
		if ( fp ) fclose(fp);
	}

	void Read(char* fname, double zscale, int w, int h, double** depth, double** elv, int** tflg, double& zmin)
	{
		double* data2 = NULL;


		FILE* fp = fopen(fname, "r");
		if ( fp == NULL )
		{
			return ;
		}

		char buf[512];

		fgets(buf, 512, fp);
		if ( buf[0] != '#' )
		{
			fclose(fp);
			fp = fopen(fname, "r");
		}else
		{
			sscanf(buf, "#ZSCALE:%lf", &zscale);
		}

		//int n = 0;
		double x1, y1, z1;
		double x2, y2, z2;
		//fgets(buf, 512, fp);
		//sscanf(buf, "v %lf %lf %lf", &x1, &y1, &z1);
		//fgets(buf, 512, fp);
		//sscanf(buf, "v %lf %lf %lf", &x2, &y2, &z2);

		//n = 2;
		//dx = (x2 - x1);
		//while( fgets(buf, 512, fp) != NULL )
		//{
		//	sscanf(buf, "v %lf %lf %lf", &x1, &y1, &z1);
		//	if ( (fabs(x2-x1) -dx) > 0.001 )
		//	{
		//		break;
		//	}
		//	x2 = x1;
		//	y2 = y1;
		//	z2 = z1;
		//	n++;
		//}
		//w = n;
		//fclose(fp);

		//fp = fopen(fname, "r");
		//fgets(buf, 512, fp);
		//if ( buf[0] != '#' )
		//{
		//	fclose(fp);
		//	fp = fopen(fname, "r");
		//}

		//n = 1;
		//while( fgets(buf, 512, fp) != NULL ) if ( buf[0] == 'v' ) n++;
		//fclose(fp);

		//h = n/w;

		fp = fopen(fname, "r");
		fgets(buf, 512, fp);
		if ( buf[0] != '#' )
		{
			fclose(fp);
			fp = fopen(fname, "r");
		}

		zmin = 999999.0;
		for ( int i = 0; i < h; i++ )
		{
			for ( int j = 0; j < w; j++ )
			{
				fgets(buf, 512, fp);
				sscanf(buf, "v %lf %lf %lf", &x1, &y1, &z1);
				if ( zmin > z1 ) zmin = z1;
			}
		}
		fclose(fp);

		//printf("zmin %f\n", zmin);

		fp = fopen(fname, "r");
		fgets(buf, 512, fp);
		if ( buf[0] != '#' )
		{
			fclose(fp);
			fp = fopen(fname, "r");
		}
		*depth = new double[w*h];
		*elv = new double[w*h];
		*tflg = new int[w*h];
		for ( int i = 0; i < h; i++ )
		{
			for ( int j = 0; j < w; j++ )
			{
				fgets(buf, 512, fp);
				sscanf(buf, "v %lf %lf %lf", &x1, &y1, &z1);
				

				(*depth)[i*w+j] = 0.0;
				(*elv)[i*w+j] = zmin;
				if ( z1 < 0.0 )
				{
					(*depth)[i*w+j] = -z1/zscale;
					(*tflg)[i*w+j] = 1;
				}else
				{
					(*elv)[i*w+j] = z1/zscale;
					(*tflg)[i*w+j] = 0;
				}
			}
		}
		fclose(fp);

		return ;
	}


	double* smoothL()
	{
		double* data2 = new double[h*w];

		memcpy(data2, data, sizeof(double)*h*w);
		#pragma omp parallel for
		for ( int i = 1; i < h-1; i++ )
		{
			for ( int j = 1; j < w-1; j++ )
			{
				 data2[w*i+j]= 0.25*(data[w*i+(j+1)]+data[w*i+(j-1)]+data[w*(i+1)+j]+data[w*(i-1)+j]);
			}
		}
		return data2;
	}

	double* smooth()
	{
		double* data2 = new double[h*w];
		double s = 1.0/(0.01*0.01);
		double w1, w2, w3, w4;

		memcpy(data2, data, sizeof(double)*h*w);
		#pragma omp parallel for
		for ( int i = 1; i < h-1; i++ )
		{
			for ( int j = 1; j < w-1; j++ )
			{
				w1 = 1.0/(1.0 + s*pow(data[w*i+(j+1)]-data[i*w+j],2));
				w2 = 1.0/(1.0 + s*pow(data[w*i+(j-1)]-data[i*w+j],2));
				w3 = 1.0/(1.0 + s*pow(data[w*(i+1)+j]-data[i*w+j],2));
				w4 = 1.0/(1.0 + s*pow(data[w*(i-1)+j]-data[i*w+j],2));
				data2[i*w+j] = (w1*data[i*w+(j+1)]+w2*data[i*w+(j-1)]+w3*data[w*(i+1)+j]+w4*data[w*(i-1)+j])/(w1+w2+w3+w4);
			}
		}
		return data2;
	}

	void Geometry(double* hh, double* ee, int* tt, int& numFace, int& numVertex, int** Face, double** vertex, int** id)
	{
		numFace = 0;
		numVertex = 0;
		*vertex = new double[3*w*h];
		*id = new int[w*h];
		
		for ( int i = 0; i < h; i++ )
		{
			for ( int j = 0; j < w; j++ )
			{
				(*vertex)[3*numVertex] = dx*j;
				(*vertex)[3*numVertex+1] = dy*i;
				if ( hh && ee && tt )
				{
					if ( tt[i*w+j] == 0 )
					{
						(*vertex)[3*numVertex+2] = ee[i*w+j];
					}else
					{
						(*vertex)[3*numVertex+2] = -hh[i*w+j];
					}
				}else
				{
					(*vertex)[3*numVertex+2] = hh[i*w+j];
				}
				(*id)[numVertex] = i*w+j;
				numVertex++;
			}
		}

		*Face = new int[3*2*(h-1)*(w-1)];
		for ( int i = 0; i < h-1; i++ )
		{
			for ( int j = 0; j < w-1; j++ )
			{
				(*Face)[3*numFace  ] =i*w+j;
				(*Face)[3*numFace+1] =i*w+j+1;
				(*Face)[3*numFace+2] =(i+1)*w+j+1;
				numFace++;
				(*Face)[3*numFace  ] =i*w+j;
				(*Face)[3*numFace+1] =(i+1)*w+j+1;
				(*Face)[3*numFace+2] =(i+1)*w+j;
				numFace++;
			}
		}
	}
	//
	// ÉâÉvÉâÉVÉAÉìÇÃåvéZ
	//
	double* computeLaplacians(double* hh, double* ee, int* tt, int& numFace, int& numVertex, int** Face, double** vertex, int** id)
	{
		Geometry( hh, ee, tt, numFace, numVertex, Face, vertex, id);

		double*lap = new double[3*numVertex];
		int *nei = new int[numVertex];

		memset(lap, '\0', sizeof(double)*3*numVertex);
		memset(nei, '\0', sizeof(int)*numVertex);

		for(int i=0; i< numFace; i++)
		{
			int* t = &((*Face)[3*i]);
			double* A = &((*vertex)[3*t[0]]);
			double* B = &((*vertex)[3*t[1]]);
			double* C = &((*vertex)[3*t[2]]);

			lap[3*t[0]+0] += B[0] - A[0];
			lap[3*t[0]+1] += B[1] - A[1];
			lap[3*t[0]+2] += B[2] - A[2];
			nei[t[0]] = nei[t[0]] + 1;

			lap[3*t[1]+0] += C[0] - B[0];
			lap[3*t[1]+1] += C[1] - B[1];
			lap[3*t[1]+2] += C[2] - B[2];
			nei[t[1]] = nei[t[1]] + 1;

			lap[3*t[2]+0] += A[0] - C[0];
			lap[3*t[2]+1] += A[1] - C[1];
			lap[3*t[2]+2] += A[2] - C[2];
			nei[t[2]] = nei[t[2]] + 1;
		}

#pragma omp parallel for schedule(dynamic)
		for(int i=0; i< numVertex; i++)
		{
			lap[3*i+0] = lap[3*i+0] / nei[i];
			lap[3*i+1] = lap[3*i+1] / nei[i];
			lap[3*i+2] = lap[3*i+2] / nei[i];
		}

		delete []nei;
		return lap;
	}

	//
	//
	//
	void laplacianSmoothing(double* hh, double* ee, int* tt, float delta, double z)
	{
		int numFace;
		int numVertex;
		int* Face;
		double* vertex;
		int* id;

		double* lap = computeLaplacians( hh, ee, tt, numFace, numVertex, &Face, &vertex, &id);
		float dt = delta;

#pragma omp parallel for schedule(dynamic)
		for(int i=0; i< numVertex; i++)
		{
			if ( smooth_limit)
			{
				if ( fabs(vertex[3*i+2]) < z )
				{
					vertex[3*i+0] = vertex[3*i+0] + dt*lap[3*i+0];
					vertex[3*i+1] = vertex[3*i+1] + dt*lap[3*i+1];
					vertex[3*i+2] = vertex[3*i+2] + dt*lap[3*i+2];
				}
			}else
			{
				vertex[3*i+0] = vertex[3*i+0] + dt*lap[3*i+0];
				vertex[3*i+1] = vertex[3*i+1] + dt*lap[3*i+1];
				vertex[3*i+2] = vertex[3*i+2] + dt*lap[3*i+2];
			}
			if ( hh && ee && tt )
			{
				if ( tt[id[i]] == 0 )
				{
					ee[id[i]] = vertex[3*i+2];
				}else
				{
					hh[id[i]] = -vertex[3*i+2];
				}
			}else
			{
					hh[id[i]] = vertex[3*i+2];
			}
		}

		delete [] lap;
		delete [] vertex;
		delete [] Face;
		delete [] id;
	}

};



#endif