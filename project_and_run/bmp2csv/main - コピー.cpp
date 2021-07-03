#include <iostream>
#include <time.h>
#include <Windows.h>

using namespace std;

#include "bitmap.h"
#include "DirectryTool.h"
#include "CsvReadWrite.hpp"

int main(int argc, char** argv)
{

	if ( argc < 3 )
	{
		printf("地形データ生成:bmp2csv.exe -e bitmapfileName [min_value [max_value]]\n");
		printf("初期波源生成:bmp2csv.exe -i bitmapfileName [min_value [max_value]]\n");
		printf("(汎用)bmp2csv.exe -x bitmapfileName [min_value [max_value]]\n");
		return -1;
	}

	char drive[_MAX_DRIVE];	// ドライブ名
    char dir[_MAX_DIR];		// ディレクトリ名
    char fname[_MAX_FNAME];	// ファイル名
    char ext[_MAX_EXT];		// 拡張子

    _splitpath( argv[2], drive, dir, fname, ext );

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

    cout << "Drive=" << exe_drive << endl;
    cout << "Dir  =" << exe_dir   << endl;
    cout << "Fname=" << exe_fname << endl;
    cout << "Ext  =" << exe_ext   << endl;

	BitMap bmp;

	double min, max;

	min = -1.0;
	max = 1.0;

	if ( argc >= 4 ) min = atof(argv[3]);
	if ( argc == 5 ) max = atof(argv[4]);
	if ( min >= max )
	{
		goto err;
	}

	bmp.Read(argv[2]);

	char output[512];

	if ( strcmp(argv[1], "-x" ) == 0 )
	{
		sprintf(output, "%s%s%s.csv", drive, dir, fname);
		bmp.WriteCsv(output, 0, min, max);
	}

	if ( strcmp(argv[1], "-e" ) == 0 )
	{
		sprintf(output, "%s%s%s.csv", drive, dir, "water_depth_data3");
		bmp.WriteCsv(output, 0, min, max);

		printf("w:%d h:%d\n", bmp.W(), bmp.H()); 

		int minpix = 255;
		int maxpix = 0;
		for (int i = 0; i < bmp.H(); i++)
		{
			for (int j = 0; j < bmp.W(); j++)
			{
				if ( minpix > bmp.cell(i,j).r ) minpix = bmp.cell(i,j).r;
				if ( maxpix < bmp.cell(i,j).r ) maxpix = bmp.cell(i,j).r;
			}
		}
		int zero = (int)(-min*(float)(maxpix-minpix)/(max -min)+(float)minpix);
		for (int i = 0; i < bmp.H(); i++)
		{
			for (int j = 0; j < bmp.W(); j++)
			{
				if ( zero > bmp.cell(i,j).r ) bmp.cell(i,j).r =  bmp.cell(i,j).g =  bmp.cell(i,j).b = 255;
				else bmp.cell(i,j).r = bmp.cell(i,j).g =  bmp.cell(i,j).b = 0;
			}
		}
		sprintf(output, "%s%s%s.bmp", drive, dir, "topography_data");
		bmp.Write(output);

		double* tmp = new double[bmp.H()*bmp.W()];
		for (int i = 0; i < bmp.H(); i++)
		{
			for (int j = 0; j < bmp.W(); j++)
			{
				tmp[i*bmp.W() +j] = bmp.cell(i,j).r;
			}
		}

		sprintf(output, "%s%s%s.csv", drive, dir, "topography_data");
		WriteCsv(output, bmp.W(), bmp.H(), tmp, 1.0);
		delete [] tmp;

		for (int i = 0; i < bmp.H(); i++)
		{
			for (int j = 0; j < bmp.W(); j++)
			{
				if ( bmp.cell(i,j).r == 0)
				{
					bmp.cell(i,j).r = 0;
					bmp.cell(i,j).g = 255;
					bmp.cell(i,j).b = 0;
				}else
				{
					bmp.cell(i,j).r = 0;
					bmp.cell(i,j).g = 0;
					bmp.cell(i,j).b = 0;
				}
			}
		}
		sprintf(output, "%s%s%s.bmp", drive, dir, "topography_data_mask");
		bmp.Write(output);
	}

	if ( strcmp(argv[1], "-i" ) == 0 )
	{
		sprintf(output, "%s%s%s.csv", drive, dir, "Initial_wave_dataUp");
		bmp.WriteCsv(output, 0, 0.0, max);
		sprintf(output, "%s%s%s.csv", drive, dir, "Initial_wave_dataDown");
		bmp.WriteCsv(output, 2, 0.0, -min);
		sprintf(output, "%s%s%s.csv", drive, dir, "Initial_wave_data2");
		bmp.WriteCsv(output, 0, min, max);
	}

	if ( strcmp(argv[1], "-red" ) == 0 || strcmp(argv[1], "-blue" ) == 0 || strcmp(argv[1], "-red_blue" ) == 0)
	{
		for (int i = 0; i < bmp.H(); i++)
		{
			for (int j = 0; j < bmp.W(); j++)
			{
				int d = 1;
				double max = (double)bmp.cell(i,j).r;
				double min = (double)bmp.cell(i,j).r;
				if ( (double)bmp.cell(i,j).g > max )
				{
					d = 2;
					max = (double)bmp.cell(i,j).g;
				}
				if ( (double)bmp.cell(i,j).b > max )
				{
					d = 3;
					max = (double)bmp.cell(i,j).b;
				}
				if ( (double)bmp.cell(i,j).g < min )
				{
					min = (double)bmp.cell(i,j).g;
				}
				if ( (double)bmp.cell(i,j).b < min )
				{
					min = (double)bmp.cell(i,j).b;
				}
				if ( (max - min) <= 1.0e-8 || max == 0.0)
				{
					bmp.cell(i,j).r = 255;
					bmp.cell(i,j).g = 255;
					bmp.cell(i,j).b = 255;
					continue;
				}

				double h = 0.0;
				if ( d == 1 && (max - min) > 1.0e-8)
				{
					h = 60.0*( (bmp.cell(i,j).g-bmp.cell(i,j).b)/(max - min));
				}
				if ( d == 2 && (max - min) > 1.0e-8)
				{
					h = 60.0*( (bmp.cell(i,j).b-bmp.cell(i,j).r)/(max - min))+120.0;
				}
				if ( d == 3 && (max - min) > 1.0e-8)
				{
					h = 60.0*( (bmp.cell(i,j).r-bmp.cell(i,j).g)/(max - min))+240.0;
				}
				if ( h < 0.0 ) h += 360.0;

				double s = (max - min)/max;
				double v = max/255.0;

				bool t = false;
				if ( strcmp(argv[1], "-red" ) == 0 || strcmp(argv[1], "-red_blue" ) == 0)
				{
					if ( (h > 350.0 || h < 50.0) && s > 0.05 && v > 0.5)
					{
						/* empty */
						t = true;
						if (  bmp.cell(i,j).r > bmp.cell(i,j).b )
						{
							bmp.cell(i,j).r = bmp.cell(i,j).r-bmp.cell(i,j).b;
						}else
						{
							bmp.cell(i,j).r = 0;
						}
						bmp.cell(i,j).g = 0;
						bmp.cell(i,j).b = 0;
					}
				}
				if ( strcmp(argv[1], "-blue" ) == 0 || strcmp(argv[1], "-red_blue" ) == 0)
				{
					if ( (h < 270.0 && h > 180.0) && s > 0.05 && v > 0.6)
					{
						/* empty */
						t = true;
						if (  bmp.cell(i,j).b > bmp.cell(i,j).r )
						{
							bmp.cell(i,j).b = bmp.cell(i,j).b-bmp.cell(i,j).r;
						}else
						{
							bmp.cell(i,j).b = 0;
						}
						bmp.cell(i,j).r = 0;
						bmp.cell(i,j).g = 0;
					}
				}
				if ( !t )
				{
					bmp.cell(i,j).r = 255;
					bmp.cell(i,j).g = 255;
					bmp.cell(i,j).b = 255;
				}
			}
		}

		if ( strcmp(argv[1], "-red" ) == 0 || strcmp(argv[1], "-red_blue" ) == 0)
		{
			for ( int n = 0; n < 5; n++ )
			{
				for ( int i = 0; i < bmp.H(); i++ )
				{
					for ( int j = 0; j < bmp.W(); j++)
					{
						if ( bmp.cell(i,j).g == 255 && bmp.cell(i,j).b == 255 )
						{
							int s = 0;
							int d = 0;
							for ( int k = 1; k <= 5; k++ )
							{
								if ( j+k < 0 || j+k >= bmp.W() ) break;
								if ( bmp.cell(i,j+k).g == 0 && bmp.cell(i,j+k).b == 0 )
								{
									s++;
									d = bmp.cell(i,j+k).r;
									break;
								}
							}
							for ( int k = 1; k >= -5; k-- )
							{
								if ( j+k < 0 || j+k >= bmp.W() ) break;
								if ( bmp.cell(i,j+k).g == 0 && bmp.cell(i,j+k).b == 0 )
								{
									s++;
									d = bmp.cell(i,j+k).r;
									break;
								}
							}
							if ( s == 2 )
							{
								bmp.cell(i,j).r = d;
								bmp.cell(i,j).g = 0;
								bmp.cell(i,j).b = 0;
							}
						}
						if ( bmp.cell(i,j).g == 255 && bmp.cell(i,j).b == 255 )
						{
							int s = 0;
							int d = 0;
							for ( int k = 1; k <= 5; k++ )
							{
								if ( i+k < 0 || i+k >= bmp.H() ) break;
								if ( bmp.cell(i+k,j).g == 0 && bmp.cell(i+k,j).b == 0 )
								{
									s++;
									d = bmp.cell(i+k,j).r;
									break;
								}
							}
							for ( int k = 1; k >= -5; k-- )
							{
								if ( i+k < 0 || i+k >= bmp.H() ) break;
								if ( bmp.cell(i+k,j).g == 0 && bmp.cell(i+k,j).b == 0 )
								{
									s++;
									d = bmp.cell(i+k,j).r;
									break;
								}
							}
							if ( s == 2 )
							{
								bmp.cell(i,j).r = d;
								bmp.cell(i,j).g = 0;
								bmp.cell(i,j).b = 0;
							}
						}
					}
				}
			}
		}
		if ( strcmp(argv[1], "-blue" ) == 0 || strcmp(argv[1], "-red_blue" ) == 0)
		{
			for ( int n = 0; n < 5; n++ )
			{
				for ( int i = 0; i < bmp.H(); i++ )
				{
					for ( int j = 0; j < bmp.W(); j++)
					{
						if ( bmp.cell(i,j).g == 255 && bmp.cell(i,j).r == 255 )
						{
							int s = 0;
							int d = 0;
							for ( int k = 1; k <= 5; k++ )
							{
								if ( j+k < 0 || j+k >= bmp.W() ) break;
								if ( bmp.cell(i,j+k).g == 0 && bmp.cell(i,j+k).r == 0 )
								{
									s++;
									d = bmp.cell(i,j+k).b;
									break;
								}
							}
							for ( int k = 1; k >= -5; k-- )
							{
								if ( j+k < 0 || j+k >= bmp.W() ) break;
								if ( bmp.cell(i,j+k).g == 0 && bmp.cell(i,j+k).r == 0 )
								{
									s++;
									d = bmp.cell(i,j+k).b;
									break;
								}
							}
							if ( s == 2 )
							{
								bmp.cell(i,j).b = d;
								bmp.cell(i,j).g = 0;
								bmp.cell(i,j).r = 0;
							}
						}
						if ( bmp.cell(i,j).g == 255 || bmp.cell(i,j).r == 255 )
						{
							int s = 0;
							int d = 0;
							for ( int k = 1; k <= 5; k++ )
							{
								if ( i+k < 0 || i+k >= bmp.H() ) break;
								if ( bmp.cell(i+k,j).g == 0 && bmp.cell(i+k,j).r == 0 )
								{
									s++;
									d = bmp.cell(i+k,j).b;
									break;
								}
							}
							for ( int k = 1; k >= -5; k-- )
							{
								if ( i+k < 0 || i+k >= bmp.H() ) break;
								if ( bmp.cell(i+k,j).g == 0 && bmp.cell(i+k,j).r == 0 )
								{
									s++;
									d = bmp.cell(i+k,j).b;
									break;
								}
							}
							if ( s == 2 )
							{
								bmp.cell(i,j).b = d;
								bmp.cell(i,j).g = 0;
								bmp.cell(i,j).r = 0;
							}
						}
					}
				}
			}
		}
		if ( strcmp(argv[1], "-red" ) == 0 )sprintf(output, "%s%s%s.bmp", drive, dir, "red");
		if ( strcmp(argv[1], "-blue" ) == 0 )sprintf(output, "%s%s%s.bmp", drive, dir, "blue");
		if ( strcmp(argv[1], "-red_blue" ) == 0 )sprintf(output, "%s%s%s.bmp", drive, dir, "red_blue");
		bmp.Write(output);

		if ( strcmp(argv[1], "-red_blue" ) == 0 )
		{
			sprintf(output, "%s%s%s.csv", drive, dir, "Initial_wave_data2");
			bmp.WriteCsv(output, 0, min, max);
		}
	}
	bmp.Clear();

err:	;
	return 0;
}
