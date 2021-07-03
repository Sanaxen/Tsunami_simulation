#include "konseki.h"
#include <Windows.h>

#include "bitmap.h"
#include "utf8_printf.hpp"

#define USE_STB_IMAGE

#ifdef USE_STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#endif

void getColumValue(char* line, int no, char* value)
{
	int n = 0;
	char* p;
	char* q;

	p = line;
	int i = 0;
	p++;
	while ( i != no )
	{
		while(*p != '\"' || ( *p == '\"' && *(p+1) != ',')) p++;
		p++;
		if (*p != ',' )
		{
			printf("error.\n");
			exit(1);
		}
		p++;
		if (*p != '\"' )
		{
			printf("error.\n");
			exit(1);
		}
		p++;
		i++;
	}

	char c = *p;
	while ( c != ',' && c != '\0' )
	{
		value[n] = '\0';
		if (*p == '\"'&& *(p+1) == ',') break;
		value[n] = c;
		p++;
		c = *p;
		n++;
	}
	value[n] = '\0';
}

//ビットマップ(x,y)の位置にマークを付ける
void BitmpMarke(BitMap& bmp, char* out, int x, int y, int w, int* rgb = 0)
{
	int xxx = x;
	int yyy = bmp.H() - y;
#pragma omp parallel for
	for ( int i = -w; i <= w; i++ )
	{
		for ( int j = -w; j <= w; j++ )
		{
			if ( yyy+i >= bmp.H()-1 || yyy+i < 0 ) continue;
			if ( xxx+j >= bmp.W()-1 || xxx+j < 0 ) continue;
			if ( rgb == 0 )
			{
				bmp.cell(yyy+i,xxx+j).r = 255;
				bmp.cell(yyy+i,xxx+j).g = 0;
				bmp.cell(yyy+i,xxx+j).b = 0;
			}else
			{
				bmp.cell(yyy+i,xxx+j).r = rgb[0];
				bmp.cell(yyy+i,xxx+j).g = rgb[1];
				bmp.cell(yyy+i,xxx+j).b = rgb[2];
			}
		}
	}
	//bmp.Write(out);
}

//ビットマップ(x,y)の位置にマークを付ける
void BitmpMarke2(BitMap& bmp, char* out, int x, int y, int w, int* rgb, ValueCell* grid, colorGrid* cgrid, int* rgb2 = 0)
{
	int xxx = x;
	int yyy = bmp.H() - y;
	double z = grid[y*bmp.W() + x].max;

	if (rgb2 && bmp.cell(yyy, xxx).g != 0 || bmp.cell(yyy, xxx).b != 0)
	{
		rgb2[0] = bmp.cell(yyy, xxx).r;
		rgb2[1] = bmp.cell(yyy, xxx).g;
		rgb2[2] = bmp.cell(yyy, xxx).b;

		cgrid[(yyy)*bmp.W() + (xxx)].id = &(grid[y*bmp.W() + x]);
		cgrid[(yyy)*bmp.W() + (xxx)].rgb[0] = rgb2[0];
		cgrid[(yyy)*bmp.W() + (xxx)].rgb[1] = rgb2[1];
		cgrid[(yyy)*bmp.W() + (xxx)].rgb[2] = rgb2[2];

		return;
	}

	if (rgb2 && bmp.cell(yyy, xxx).g == 0 && bmp.cell(yyy, xxx).b == 0)
	{
		bmp.cell(yyy, xxx).r = rgb[0];
		bmp.cell(yyy, xxx).g = rgb[1];
		bmp.cell(yyy, xxx).b = rgb[2];
		rgb2[0] = rgb[0];
		rgb2[1] = rgb[1];
		rgb2[2] = rgb[2];

		cgrid[(yyy)*bmp.W() + (xxx)].id = &(grid[y*bmp.W() + x]);
		cgrid[(yyy)*bmp.W() + (xxx)].rgb[0] = rgb2[0];
		cgrid[(yyy)*bmp.W() + (xxx)].rgb[1] = rgb2[1];
		cgrid[(yyy)*bmp.W() + (xxx)].rgb[2] = rgb2[2];
		return;
	}
	printf("============================%d %d %d\n", bmp.cell(yyy, xxx).r, bmp.cell(yyy, xxx).g, bmp.cell(yyy, xxx).b);
	abort();

#pragma omp parallel for
	for ( int i = -w; i <= w; i++ )
	{
		for ( int j = -w; j <= w; j++ )
		{
			if ( yyy+i >= bmp.H()-1 || yyy+i < 0 ) continue;
			if ( xxx+j >= bmp.W()-1 || xxx+j < 0 ) continue;
			if ( rgb2 && bmp.cell(yyy+i,xxx+j).r == 255 && bmp.cell(yyy+i,xxx+j).g != 0 && bmp.cell(yyy+i,xxx+j).b != 0)
			{
				rgb2[0] = bmp.cell(yyy+i,xxx+j).r;
				rgb2[1] = bmp.cell(yyy+i,xxx+j).g;
				rgb2[2] = bmp.cell(yyy+i,xxx+j).b;
			}
			if (rgb2 && bmp.cell(yyy + i, xxx + j).r == 255 && bmp.cell(yyy + i, xxx + j).g == 0 && bmp.cell(yyy + i, xxx + j).b == 0)
			{
				bmp.cell(yyy + i, xxx + j).r = rgb[0];
				bmp.cell(yyy + i, xxx + j).g = rgb[1];
				bmp.cell(yyy + i, xxx + j).b = rgb[2];
				rgb2[0] = rgb[0];
				rgb2[1] = rgb[1];
				rgb2[2] = rgb[2];
			}

			cgrid[(yyy+i)*bmp.W() + (xxx+j)].id = &(grid[y*bmp.W() + x]);
			cgrid[(yyy+i)*bmp.W() + (xxx+j)].rgb[0] = rgb[0];
			cgrid[(yyy+i)*bmp.W() + (xxx+j)].rgb[1] = rgb[1];
			cgrid[(yyy+i)*bmp.W() + (xxx+j)].rgb[2] = rgb[2];
		}
	}
	//bmp.Write(out);
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


int Konseki( char* parameterFile)
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


	double latitude[4];
	double longitude[4];

	FILE* fp = fopen( parameterFile, "r");
	if ( fp == NULL )
	{
		return -1;
	}
	char IDname[256];

	char* buf = new char[LINEMAX2];

	while( fgets(buf, LINEMAX, fp ) != NULL )
	{
		if ( strcmp(buf, "ID\n") == 0 )
		{
			fgets(IDname, LINEMAX, fp);
			IDname[strlen(IDname)-1] = '\0';
			{
				FILE* fp = fopen(IDname, "w");
				if ( fp == NULL )
				{
					printf("ERROR:IDで利用できない文字が使われています");
					strcpy(IDname, "untitled");
					printf("=>%sを利用します\n",IDname);
				}else
				{
					fclose(fp);
					remove(IDname);
				}
			}
			continue;
		}
		if ( strcmp(buf, "LAT\n") == 0 )
		{
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
	}
	fclose(fp);

	sprintf(fname, "%s%s%s_KONSEKI.csv", drive, dir, IDname);

	fp = fopen( fname, "r");
	if ( fp == NULL )
	{
		printf("[%s]を開くことが出来ません\n", fname);
		return -1;
	}
	std::vector<konsekiValue> konsekiList;

	csv_getline csv;

	char topography_data[512];
	char chkpoint_data[512];

	sprintf(topography_data, "%s%stopography_data.bmp", drive, dir);
	sprintf(chkpoint_data, "%s%schkpoint_data_all.bmp", drive, dir);

	BitMap bmp;
	bmp.Read(topography_data);

	int w = bmp.W();
	int h = bmp.H();

	char* value = new char[LINEMAX2];
	int linenum = 1;
	fgets(buf, LINEMAX2, fp);
	while( csv.get(fp) != NULL )
	{
		konsekiValue ko;
		//printf("%s", buf);

		getColumValue(csv.buffer(), 0, value);
		ko.id = atoi(value);

		//痕跡IDは重複しているIDなので使わない
		//getColumValue(csv.buffer(), 6, value);
		//ko.kid = atoi(value);
		ko.kid = 0;

		getColumValue(csv.buffer(), 11, value);
		ko.name = value;
		getColumValue(csv.buffer(), 12, value);
		ko.name += value;
		getColumValue(csv.buffer(), 8, value);
		ko.name += "(";
		ko.name += value;
		ko.name += ")";

		double h, m, s;

		getColumValue(csv.buffer(), 18, value);
		h = atof(value);
		getColumValue(csv.buffer(), 19, value);
		m = atof(value);
		getColumValue(csv.buffer(), 20, value);
		s = atof(value);

		ko.lat = h + m/60.0 + s/3600.0;
		getColumValue(csv.buffer(), 21, value);
		h = atof(value);
		getColumValue(csv.buffer(), 22, value);
		m = atof(value);
		getColumValue(csv.buffer(), 23, value);
		s = atof(value);

		ko.lon = h + m/60.0 + s/3600.0;

				//tp_痕跡高_上限
		//getColumValue(csv.buffer(), 42, value);
		
		//文献記載_痕跡高_上限
		getColumValue(csv.buffer(), 35, value);
		if ( value[0] == '\0' )
		{
			printf("記載無し[%d]\n", ko.kid);
		}
		ko.hight = atof(value);
		
		//文献記載_痕跡パターン
		getColumValue(csv.buffer(), 37, value);

		ko.type = 999;
		if ( strcmp(value, "遡上高") == 0 ) ko.type = 1;
		if ( strcmp(value, "浸水高") == 0 ) ko.type = 2;
		if ( strcmp(value, "浸水深") == 0 ) ko.type = 3;
		if ( strcmp(value, "不明") == 0 )
		{
			ko.type = 4;
			printf("不明:%f[m]\n", ko.hight );
		}
		if ( ko.type == 999 )
		{
			printf("文献記載_痕跡パターン：%s\n", value);
		}

		//文献記載_高さ定義_信頼度
		getColumValue(csv.buffer(), 38, value);
		if ( strcmp(value, "◎") == 0 ) ko.h_confidence = 'A';
		if ( strcmp(value, "○") == 0 ) ko.h_confidence = 'B';
		if ( strcmp(value, "△") == 0 ) ko.h_confidence = 'C';

		//文献信頼度
		getColumValue(csv.buffer(), 54, value);
		if ( strcmp(value, "◎") == 0 ) ko.confidence = 'A';
		if ( strcmp(value, "○") == 0 ) ko.confidence = 'B';
		if ( strcmp(value, "△") == 0 ) ko.confidence = 'C';

		//printf("%d\n", linenum);
		linenum++;

		printf("id %d kid %d %s 緯度:%f 経度:%f ランク:%c\n", ko.id, ko.kid, ko.name.c_str(), ko.lat, ko.lon,ko.rank);
		if ( ko.hight <= 0.0 )
		{
			printf("浸水無し[%d]\n", ko.kid);
			continue;
		}

		getColumValue(csv.buffer(), 46, value);
		ko.rank = value[0];
		if ( !isalpha(ko.rank) ) ko.rank = '?';


		//格子上では重複してしまう痕跡データを精査する
		int xxx, yyy;
		bool skipp = false;

#if 10
		switch (ko.rank)
		{
		case 'A': break;
		case 'B': break;
		case 'C': break;
		case 'D': break;
		default:
			skipp = true;
			break;
		}
#endif

#if 10
		if (!skipp)
		{
			//格子上で重複するケース
			latitudeLongitude_to_Bitmap(w, h, longitude, latitude, ko.lon, ko.lat, xxx, yyy);

			double lon = (xxx - 0.5)*(longitude[1] - longitude[0]) / (double)(w - 1) + longitude[0];
			double lat = (yyy - 0.5)*(latitude[2] - latitude[0]) / (double)(h - 1) + latitude[0];

			double min = 99999999999999.0;
			int id = -1;
			for (int kk = 0; kk < konsekiList.size(); kk++)
			{
				int xx, yy;
				latitudeLongitude_to_Bitmap(w, h, longitude, latitude, konsekiList[kk].lon, konsekiList[kk].lat, xx, yy);

				//格子上では重複
				if (xx == xxx && yy == yyy)
				{
					if (konsekiList[kk].max < ko.hight)
					{
						konsekiList[kk].max = ko.hight;
					}
					if (konsekiList[kk].min > ko.hight)
					{
						konsekiList[kk].min = ko.hight;
					}
					double diff = (konsekiList[kk].lon - lon)*(konsekiList[kk].lon - lon) + (konsekiList[kk].lat - lat)*(konsekiList[kk].lat - lat);
					if (diff < min)
					{
						min = diff;
						id = kk;
					}
				}
			}
			if (id >= 0 && min < konsekiList[id].diff)
			{
				konsekiList[id] = ko;
				konsekiList[id].diff = min;
				skipp = true;
			}
		}
#endif
		if ( !skipp ) konsekiList.push_back(ko);
	}
	fclose(fp);
	delete [] value;
	delete [] buf;



	ValueCell* grid = new ValueCell[w*h];
	colorGrid* cgrid = new colorGrid[w*h];

	const int sz = konsekiList.size();

	int skip = 0;
	for ( int i = 0; i < sz; i++ )
	{
		konsekiValue ko = konsekiList[i];
		//if ( ko.rank > 'C' )
		//{
		//	printf("Dランク以降は却下\n");
		//	continue;
		//}
		//if ( ko.rank >= 'C' )
		//{
		//	if ( ! (ko.confidence <= 'B' && ko.h_confidence <= 'B'))
		//	{
		//		printf("信頼出来ないCランクは却下\n");
		//	}
		//}
		if ( ko.type > 4 )
		{
			skip++;
			continue;
		}

		//printf("[%d/%d] id %d kid %d %s 緯度:%f 経度:%f ランク:%c\n", i+1, sz, ko.id, ko.kid, ko.name.c_str(), ko.lat, ko.lon,ko.rank);
		
#if 10
		int xx, yy;
		latitudeLongitude_to_Bitmap(w, h, longitude, latitude, ko.lon, ko.lat, xx, yy);

		const int bw = 0;
		for ( int ki = -bw; ki <= bw; ki++ )
		{
			for ( int kj = -bw; kj <= bw; kj++ )
			{
				if ( xx-kj < 0 || xx+kj >= w-1 ) continue;
				if ( yy-ki < 0 || yy+kj >= h-1 ) continue;

				grid[w*(yy+ki)+xx+kj].dup++;
				grid[w*(yy+ki)+xx+kj].sum += ko.hight;
				if ( grid[w*(yy+ki)+xx+kj].max < ko.hight )
				{
					grid[w*(yy+ki)+xx+kj].max = ko.hight;
				}
				if ( grid[w*(yy+ki)+xx+kj].min > ko.hight )
				{
					grid[w*(yy+ki)+xx+kj].min = ko.hight;
				}
				grid[w*(yy+ki)+xx+kj].konseki_list.push_back(ko);
				BitmpMarke(bmp, chkpoint_data, xx, yy, 0);
			}
		}
#else
		double dxx = (longitude[1] - longitude[0])/((double)w-1.0);
		double dyy = (latitude[2] - latitude[0])/((double)h-1.0);

		std::vector<int> xxx;
		std::vector<int> yyy;
		for ( int ki = -1; ki <= 1; ki++ )
		{
			for ( int kj = -1; kj <= 1; kj++ )
			{

				int xx, yy;
				latitudeLongitude_to_Bitmap(w, h, longitude, latitude, 
					ko.lon+0.5*dxx*(double)kj, 
					ko.lat+0.5*dyy*(double)ki,
					xx, yy);

				if ( xx < 0 || xx >= w-1 ) continue;
				if ( yy < 0 || yy >= h-1 ) continue;

				bool dup = false;
				for ( int kk = 0; kk < xxx.size(); kk++ )
				{
					if ( xxx[kk] = xx && yyy[kk] == yy )
					{
						dup = true;
						break;
					}
				}
				if ( dup ) continue;

				xxx.push_back(xx);
				yyy.push_back(yy);

				grid[w*(yy)+xx].dup++;
				grid[w*(yy)+xx].sum += ko.hight;
				if ( grid[w*(yy)+xx].max < ko.hight )
				{
					grid[w*(yy)+xx].type = ko.type;
					grid[w*(yy)+xx].kid = ko.kid;
					grid[w*(yy)+xx].lon = ko.lon;
					grid[w*(yy)+xx].lat = ko.lat;
					grid[w*(yy)+xx].rank = ko.rank;
					grid[w*(yy)+xx].name = ko.name;
					grid[w*(yy)+xx].max = ko.hight;
				}
				grid[w*(yy)+xx].konseki_list.push_back(ko);
				if ( grid[w*(yy)+xx].min > ko.hight ) grid[w*(yy)+xx].min = ko.hight;
				BitmpMarke(bmp, chkpoint_data, xx, yy, 0);
			}
		}
#endif
	}
	bmp.Write(chkpoint_data);
	printf("痕跡データ数:%d スキップ:%d\n", sz, skip);


	std::vector<rgbrow> rgbtabel;
	for ( int i = 0; i < 255; i++ )
	{
		for ( int j = 0; j < 255; j++ )
		{
			rgbrow a;
			a.rgb[0] = 255;
			a.rgb[1] = i;
			a.rgb[2] = j;
			rgbtabel.push_back(a);
		}
	}

	char parametertable[512];
	sprintf(parametertable, "%s%s%s_chkpoint.txt", drive, dir, IDname);

	fp = fopen(parametertable, "w");
	fprintf(fp, "CHKPOINT_BEGIN\n");

	int sponjiArea = 30;
	int num =0;

	sprintf(chkpoint_data, "%s%schkpoint_data.bmp", drive, dir);
	bmp.Read(topography_data);

	for (int i = 0; i < konsekiList.size(); i++)
	{
		int id = i;
		int rgb2[3] = { 0, 0, 0 };
		int xx, yy;
		latitudeLongitude_to_Bitmap(w, h, longitude, latitude, konsekiList[id].lon, konsekiList[id].lat, xx, yy);
		if (xx >= w || xx < 0 || yy >= h || yy < 0)
		{
			continue;
		}

		BitmpMarke2(bmp, chkpoint_data, xx, yy, 1, rgbtabel[num].rgb, grid, cgrid, rgb2);
		fprintf(fp, "%s[(%d %d)]\n", konsekiList[id].name.c_str(), yy, xx);
		fprintf(fp, "%d %d %d\n", rgb2[0], rgb2[1], rgb2[2]);
		fprintf(fp, "%c%.2f\n", (konsekiList[id].rank <= 'Z') ? konsekiList[id].rank : '?', konsekiList[id].hight);
		fprintf(fp, "LON:%f LAT:%f\n", konsekiList[id].lon, konsekiList[id].lat);
		fprintf(fp, "PTN:%d\n", konsekiList[id].type);
		fprintf(fp, "\n");
		num++;
	}

	fprintf(fp, "CHKPOINT_END\n\n");
	fclose(fp);

	bmp.Write(chkpoint_data);
	printf("チェック痕跡数:%d\n", num);


	for ( int i = 0; i < h; i++ )
	{
		for ( int j = 0; j < w; j++ )
		{
			if ( cgrid[i*w+j].id == 0 )
			{
				bmp.cell(i,j).r = 0;
				bmp.cell(i,j).g = 0;
				bmp.cell(i,j).b = 0;
			}else
			{
				bmp.cell(i,j).r = cgrid[i*w+j].rgb[0];
				bmp.cell(i,j).g = cgrid[i*w+j].rgb[1];
				bmp.cell(i,j).b = cgrid[i*w+j].rgb[2];
			}
		}
	}
	sprintf(fname, "%s%sgoogle_earth\\%s_chkpoint_data.bmp", drive, dir, IDname);
	bmp.Write(fname);
#ifdef USE_STB_IMAGE
	{
		unsigned char* pixels;
		int width;
		int height;
		int bpp;
		pixels = stbi_load(fname, &width, &height, &bpp, 0);

		char fname2[_MAX_FNAME];
		sprintf(fname2, "%s%sgoogle_earth\\%s_chkpoint_data.png", drive, dir, IDname);
		unsigned char* pixels2 = stbi__convert_format(pixels, bpp, 4, width, height);
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int pos = (i*width + j);
				if (pixels2[pos * 4 + 0] == 0 && pixels2[pos * 4 + 1] == 0 && pixels2[pos * 4 + 2] == 0)
				{
					pixels2[pos * 4 + 3] = 0;
				}
				else
				{
					pixels2[pos * 4 + 3] = 255;
				}
			}
		}
		stbi_write_png(fname2, width, height, 4, pixels2, width * 4);
		if (pixels == pixels2)
		{
			stbi_image_free(pixels);
		}
		else
		{
			stbi_image_free(pixels2);
		}

		_unlink(fname);
	}
#endif
	sprintf(fname, "%s%sgoogle_earth\\%s_checkpoint.kml", drive, dir, IDname);
	fp = fopen(fname, "w");

	utf8str utf8;

	utf8.fprintf(fp, 
		"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
		"<kml xmlns=\"http://earth.google.com/kml/2.2\">\n"
		"<Folder><name>%s_CheckPoint</name>\n"
		"<GroundOverlay>\n"
		"   <name>image</name>\n"
		"   <Icon>\n"
		"       <href>%s_chkpoint_data.png</href>\n"
		"   </Icon>\n"
		"   <LatLonBox>\n"
		"      <north>%f</north>\n"
        "      <south>%f</south>\n"
		"      <east>%f</east>\n"
		"      <west>%f</west>\n"
		"   </LatLonBox>\n"
		"</GroundOverlay>\n",
		IDname, IDname, latitude[0], latitude[2], longitude[0], longitude[1]
	);
	for ( int i = 0; i < h; i++ )
	{
		for ( int j = 0; j < w; j++ )
		{
			if ( cgrid[i*w+j].id == 0 )
			{
				continue;
			}

			for ( int k = 0; k < (cgrid[i*w+j].id)->konseki_list.size(); k++ )
			{
				//if ( (cgrid[i*w+j].id)->konseki_list[k].AlreadyOutput ) continue;
				utf8.fprintf(fp,
					"<Placemark><name>%c%.3f</name>\n"
					"  <description>%s[%fm]</description>\n"
					"  <Point><coordinates>%f,%f</coordinates></Point>\n"
					"</Placemark>\n",
					((cgrid[i*w+j].id)->konseki_list[k].rank <= 'Z')?(cgrid[i*w+j].id)->konseki_list[k].rank:'?',
					(cgrid[i*w+j].id)->konseki_list[k].hight,
					(cgrid[i*w+j].id)->konseki_list[k].name.c_str(),
					(cgrid[i*w+j].id)->konseki_list[k].hight,
					(cgrid[i*w+j].id)->konseki_list[k].lon,
					(cgrid[i*w+j].id)->konseki_list[k].lat
					);
				//(cgrid[i*w+j].id)->konseki_list[k].AlreadyOutput = true;
			}
		}
	}

	utf8.fprintf(fp,	"</Folder>\n");
	utf8.fprintf(fp,	"</kml>\n");

	fclose(fp);

	if ( num > 255*255 )
	{
		printf("痕跡数が多すぎます[MAX:%d]\n", 255*255);
		fp = fopen(parametertable, "w");
		fprintf(fp, "痕跡数が多すぎます[MAX:%d]\n", 255*255);
		fclose(fp);
	}

	return 0;
}

int main(int argc, char** argv)
{
	Konseki( argv[1] );
	return 0;
}
