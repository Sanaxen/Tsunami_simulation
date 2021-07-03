#include "..\bitmap.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	char drive[_MAX_DRIVE];	// ドライブ名
	char dir[_MAX_DIR];		// ディレクトリ名
	char fname[_MAX_FNAME];	// ファイル名
	char ext[_MAX_EXT];		// 拡張子

	_splitpath(argv[1], drive, dir, fname, ext);

	cout << "Drive=" << drive << endl;
	cout << "Dir  =" << dir << endl;
	cout << "Fname=" << fname << endl;
	cout << "Ext  =" << ext << endl;

	char fname[_MAX_FNAME];	// ファイル名
	BitMap bmp2;
	sprintf(fname, "%s%soutput\\output_W%06d.csv", drive, dir, i);
	bmp2.ReadCsv(fname, min, max);

	BitMap samplingline;
	sprintf(fname, "%s%ssamplingline_data.bmp", drive, dir);
	samplingline.Read(fname);

	if (samplingline.GetImage())
	{
		sprintf(fname, "%s%ssampling\\sampling_point%06d.dat", drive, dir, i);
		FILE* fp = fopen(fname, "w");
		for (int ii = 0; ii < samplingline.H(); ii++)
		{
			for (int jj = 0; jj < samplingline.W(); jj++)
			{
				if (samplingline.cell(ii, jj).r == 0 && samplingline.cell(ii, jj).g == 0 && samplingline.cell(ii, jj).b == 0)
				{
					fprintf(fp, "%.4f\n", grid.W(ii, jj));
					break;
				}
			}
		}
		fclose(fp);
	}
	return 0;
}