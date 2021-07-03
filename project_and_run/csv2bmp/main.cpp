#include <iostream>
#include <time.h>
#include <Windows.h>

using namespace std;

#include "bitmap.h"
#include "DirectryTool.h"
#include "CsvReadWrite.hpp"

int main(int argc, char** argv)
{

	if ( argc < 1 )
	{
		printf("csv2bmp.exe csvfileName [csvfilenName2]\n");
		return -1;
	}

	char drive[_MAX_DRIVE];	// ドライブ名
    char dir[_MAX_DIR];		// ディレクトリ名
    char fname[_MAX_FNAME];	// ファイル名
    char ext[_MAX_EXT];		// 拡張子

    _splitpath( argv[1], drive, dir, fname, ext );

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

	int w, h;


	BitMap bmp;

	double min, max;

	min = 99999999.0;
	max = -99999999.0;

	double* wrk = ReadCsv(argv[1], w, h);
	printf("%d %d\n", w, h);

	if (argc == 3)
	{
		_splitpath(argv[2], drive, dir, fname, ext);

		double* wrk2 = ReadCsv(argv[2], w, h);
		double* wrk3 = new double[w*h];
		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{
				wrk3[i*w + j] = wrk2[i*w + j] - wrk[i*w + j];
				if (min > wrk3[i*w + j]) min = wrk3[i*w + j];
				if (max < wrk3[i*w + j]) max = wrk3[i*w + j];
			}
		}
		char output[512];
		sprintf(output, "%s%s%s.csv", drive, dir, fname);
		WriteCsv(output, w, h, wrk3, 1.0);

		bmp.ReadCsv(output, min, max);
		sprintf(output, "%s%s%s.bmp", drive, dir, fname);
		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{
				if (wrk3[i*w + j] >= 0.0)
				{
					bmp.cell(i, j).r = 0.0;
					if (fabs(max) > 0.00001)
					{
						bmp.cell(i, j).r = 255.0*wrk3[i*w + j] / max;
					}
					bmp.cell(i, j).g = 0.0;
					bmp.cell(i, j).b = 0.0;
				}
				else
				{
					bmp.cell(i, j).r = 0.0;
					bmp.cell(i, j).g = 0.0;
					bmp.cell(i, j).b = 0.0;
					if (fabs(min) > 0.00001)
					{
						bmp.cell(i, j).b = 255.0*wrk3[i*w + j] / min;
					}
				}
			}
		}
		bmp.Write(output);

		delete[] wrk;
		delete[] wrk2;
		delete[] wrk3;
		bmp.Clear();
		return 0;
	}

	for ( int i = 0; i < h; i++ )
	{
		for ( int j = 0; j < w; j++ )
		{
			if ( min > wrk[i*w +j] ) min = wrk[i*w +j];
			if ( max < wrk[i*w +j] ) max = wrk[i*w +j];
		}
	}
	printf("min %f max %f\n0.0メートルのピクセル値は=%dになります\n", min, max, (int)(255.0*(0.0 - min)/(max - min)));


	for ( int i = 0; i < h; i++ )
	{
		for ( int j = 0; j < w; j++ )
		{
			wrk[i*w +j] = 255.0*(wrk[i*w +j] - min)/(max - min);
		}
	}
	char output[512];
	sprintf(output, "%s%s%s.tmp", drive, dir, fname);
	WriteCsv(output, w, h, wrk, 1.0);

	bmp.ReadCsv(output, 0, 255.0);
	sprintf(output, "%s%s%s.bmp", drive, dir, fname);
	bmp.Write(output);

	delete [] wrk;

	bmp.Clear();

err:	;
	return 0;
}
