#define TSU_H__
#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<math.h>

#include "../bitmap.h"

#define LINELENGMAX	(4096*100)

class LonLat
{
public:
	double lon;
	double lat;
	double hi;
};

LonLat* TextPointRawRead( char* filename, int& w, int& h)
{
	int i, j;
	int real_width;
	unsigned int width, height;
	FILE *fp;
	double *line_data;
	LonLat* data = NULL;

	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error: %s could not read.", filename);
		return NULL;
	}

	char buf[LINELENGMAX];

	int linenum = 0;
	while( fgets(buf, LINELENGMAX, fp ) != NULL ) linenum++;

	printf("line %d\n", linenum);
	fclose(fp);
	fp = fopen(filename, "r");

	width = 1;
	double lon = 0.0;
	fgets(buf, LINELENGMAX, fp );
	sscanf(buf, "%lf", &lon);

	while( fgets(buf, LINELENGMAX, fp ) != NULL )
	{
		double lon2 = 0.0;
		sscanf(buf, "%lf", &lon2);

		if ( lon2 < lon ) break;
		lon = lon2;
		width++;
	}
	height = linenum/width;
	printf("w %d h %d\n", width, height);

	fclose(fp);

	fp = fopen(filename, "r");
	data = new LonLat[width*height];
	for ( int i = 0; i < height; i++ )
	{
		for ( int j = 0; j < width; j++ )
		{
			LonLat a;
			fgets(buf, LINELENGMAX, fp );
			sscanf(buf, "%lf %lf %lf", &a.lon, &a.lat, &a.hi);
			data[width*i + j] = a;
		}
	}
	fclose(fp);

	w = width;
	h = height;
	return data;
}


double* TextRawRead( char* filename, int& w, int& h)
{
	int i, j;
	int real_width;
	unsigned int width, height;
	FILE *fp;
	double *line_data;
	double* data = NULL;

	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error: %s could not read.", filename);
		return NULL;
	}

	char buf[LINELENGMAX];
	fgets(buf, LINELENGMAX, fp);

	int sep = 0;

	char* p = buf;
	width = 0;
	do{
		p = strchr(p, ' ');
		if ( p )
		{
			width++;
			p++;
		}
	}while( p );

	if ( width == 0 )
	{
		sep = 1;
		p = buf;
		do{
			p = strchr(p, ',');
			if ( p )
			{
				width++;
				p++;
			}
		}while( p );
	}
	width++;

	w = width;

	height = 1;
	while( fgets(buf, LINELENGMAX, fp) ) height++;

	h = height;

	printf("w:%d h:%d\n", w, h);

	fclose(fp);
	fp = fopen(filename, "r");

	if((line_data = (double *)malloc(sizeof(double)*width)) == NULL){
		fprintf(stderr, "Error: Allocation error.\n");
		return NULL;
	}

	data = new double[w*h];
	memset(data, '\0', w*h*sizeof(double));

	for(i=0; i<height; i++){
		fgets(buf, LINELENGMAX, fp);
		p = buf;

		if ( sep == 0 )
		{
			for ( int k = 0; k < width; k++ )
			{
				double dmy;
				sscanf(p, "%lf",  &dmy);
				line_data[k] = dmy;
				p = strchr(p, ' ');
				if ( p ) p++;
			}
		}else
		{
			for ( int k = 0; k < width; k++ )
			{
				double dmy;
				sscanf(p, "%lf",  &dmy);
				line_data[k] = dmy;
				p = strchr(p, ',');
				if ( p ) p++;
			}
		}
		for(j=0; j<width; j++){
			data[w*(height-i-1) + j] = line_data[j];
		}
	}

	free(line_data);

	fclose(fp);


	return data;
}

int TextWriteCsv(char* filename, int w, int h, double* data, double scale)
{
	int i, j;
	FILE *fp;
	int real_width;

	if((fp = fopen(filename, "w")) == NULL){
		fprintf(stderr, "Error: %s could not open.", filename);
		return 1;
	}


	for(i=0; i<h; i++){
		for(j=0; j<w-1; j++){
			fprintf(fp, "%f,", (data[w*(h-i-1) + j])*scale);
		}
		fprintf(fp, "%f\n", (data[w*(h-i-1)+j])*scale);
	}
	fclose(fp);

	return 0;
}

int main( int argc, char** argv)
{
#if 0
	{
		BitMap bmp;
		BitMap wrk;

		bmp.Read( argv[1] );

		wrk.Create(bmp.W(), bmp.H());
		for ( int i = 0; i < bmp.H(); i++ )
		{
			for ( int j = 0; j < bmp.W(); j++)
			{
				wrk.cell(i,j) = bmp.cell(i,j);
				if ( wrk.cell(i,j).r == 200 && wrk.cell(i,j).r == wrk.cell(i,j).g && wrk.cell(i,j).r == wrk.cell(i,j).b )
				{
					continue;
				}
				if ( wrk.cell(i,j).r > 250 )
				{
					wrk.cell(i,j).r = 255 - wrk.cell(i,j).b;
					wrk.cell(i,j).g = 0;
					wrk.cell(i,j).b = 0;
					continue;
				}
				if ( wrk.cell(i,j).b > 250 )
				{
					wrk.cell(i,j).b = 255 - wrk.cell(i,j).r;
					wrk.cell(i,j).r = 0;
					wrk.cell(i,j).g = 0;
					continue;
				}
				wrk.cell(i,j).r = 255;
				wrk.cell(i,j).g = 255;
				wrk.cell(i,j).b = 255;
			}
		}
		wrk.Write("aaaa.bmp");
		exit(0);
	}
#endif

	BitMap bmp;

	if ( argc < 3 )
	{
		goto err;
	}

	int X[2], Y[2];
	X[0] = X[1] = -1;
	Y[0] = Y[1] = -1;
	if ( strcmp(argv[1], "-clip") == 0 )
	{
		if ( argc != 7 )
		{
			goto err;
		}
		X[0] = atoi(argv[2]);
		X[1] = atoi(argv[3]);
		Y[0] = atoi(argv[4]);
		Y[1] = atoi(argv[5]);

		bmp.Read(argv[6]);

		BitMap clipedbmp;

		int w = X[1]-X[0]+1;
		int h = Y[1]-Y[0]+1;
		clipedbmp.Create(w, h);
		printf("%d %d\n", w, h);
		int ii, jj;
		for ( int i = bmp.H()-Y[1] , ii = 0; i <= bmp.H()-Y[0]; i++, ii++ )
		{
			for ( int j = X[0], jj = 0; j <= X[1]; j++, jj++ )
			{
				clipedbmp.cell(ii, jj).r = bmp.cell(i,j).r;
				clipedbmp.cell(ii, jj).g = bmp.cell(i,j).g;
				clipedbmp.cell(ii, jj).b = bmp.cell(i,j).b;
			}
		}
		printf("%d %d\n", ii, jj);
		clipedbmp.Write("cliped.bmp");
	}
	if ( strcmp(argv[1], "-clip_t") == 0 )
	{
		if ( argc != 7 )
		{
			goto err;
		}
		X[0] = atoi(argv[2]);
		X[1] = atoi(argv[3]);
		Y[0] = atoi(argv[4]);
		Y[1] = atoi(argv[5]);

		int W, H;
		double* bmp = TextRawRead(argv[6], W, H);


		int w = X[1]-X[0]+1;
		int h = Y[1]-Y[0]+1;

		double* clipedbmp = new double[w*h];
		memset(clipedbmp, '\0', sizeof(double)*w*h);
		printf("%d %d\n", w, h);
		int ii, jj;
		for ( int i = H-Y[1] , ii = 0; i <= H-Y[0]; i++, ii++ )
		{
			for ( int j = X[0], jj = 0; j <= X[1]; j++, jj++ )
			{
				clipedbmp[w*ii + jj] = bmp[W*i+j];
			}
		}
		printf("%d %d\n", ii, jj);
		TextWriteCsv("cliped.csv", w, h, clipedbmp, 1.0);
		delete [] bmp;
		delete [] clipedbmp;
	}

	if ( strcmp(argv[1], "-clip_tr") == 0 )
	{
		if ( argc < 7 )
		{
			goto err;
		}
		X[0] = atoi(argv[2]);
		X[1] = atoi(argv[3]);
		Y[0] = atoi(argv[4]);
		Y[1] = atoi(argv[5]);

		int W, H;
		double* bmp = TextRawRead(argv[6], W, H);


		int w = X[1]-X[0]+1;
		int h = Y[1]-Y[0]+1;

		double* clipedbmp = new double[w*h];
		memset(clipedbmp, '\0', sizeof(double)*w*h);
		printf("%d %d\n", w, h);
		int ii, jj;
		for ( int i = H-Y[1] , ii = 0; i <= H-Y[0]; i++, ii++ )
		{
			for ( int j = X[0], jj = 0; j <= X[1]; j++, jj++ )
			{
				clipedbmp[w*(h-ii-1) + jj] = bmp[W*i+j];
			}
		}
		printf("%d %d\n", ii, jj);
		TextWriteCsv("cliped.csv", w, h, clipedbmp, 1.0);

		int hh = h;
		int ww = w;
		if ( argc == 8 )
		{
			FILE* fp = fopen("Parameter.txt", "w");
			printf("%s\n", argv[7]);
			fprintf(fp, "%s\n", argv[7]);
			double a = clipedbmp[0];
			double h = (int)a;
			double m = (int)((a-h)*60.0);
			double s = ((a-h)*60.0 -m)*60.0;
			printf("%.1f, %.1f, %.1f\n",  h, m, s);
			fprintf(fp, "%.1f, %.1f, %.1f\n",  h, m, s);

			a = clipedbmp[(ww-1)];
			h = (int)a;
			m = (int)((a-h)*60.0);
			s = ((a-h)*60.0 -m)*60.0;
			printf("%.1f, %.1f, %.1f\n",  h, m, s);
			fprintf(fp, "%.1f, %.1f, %.1f\n",  h, m, s);

			a = clipedbmp[(hh-1)*ww];
			h = (int)a;
			m = (int)((a-h)*60.0);
			s = ((a-h)*60.0 -m)*60.0;
			printf("%.1f, %.1f, %.1f\n",  h, m, s);
			fprintf(fp, "%.1f, %.1f, %.1f\n",  h, m, s);

			a = clipedbmp[(hh-1)*ww+(ww-1)];
			h = (int)a;
			m = (int)((a-h)*60.0);
			s = ((a-h)*60.0 -m)*60.0;
			printf("%.1f, %.1f, %.1f\n",  h, m, s);
			fprintf(fp, "%.1f, %.1f, %.1f\n\n",  h, m, s);
		}
		delete [] bmp;
		delete [] clipedbmp;
	}

	if ( strcmp(argv[1], "-e1") == 0 )
	{
		if ( argc < 3 ) goto err;

		bmp.Read(argv[2]);
		BitMap reversebmp;

		reversebmp.Create(bmp.W(), bmp.H());
		for ( int i = 0; i < bmp.H(); i++ )
		{
			for ( int j = 0; j < bmp.W(); j++)
			{
				if ( bmp.cell(i,j).r == 255 && bmp.cell(i,j).g == 255 && bmp.cell(i,j).b == 255 )
				{
					reversebmp.cell(i, j) =  bmp.cell(i,j);
					continue;
				}else
				{
					reversebmp.cell(i, j).r = 0;
					reversebmp.cell(i, j).g = 0;
					reversebmp.cell(i, j).b = 0;
				}
			}
		}
		reversebmp.Write("e1.bmp");
	}

	if ( strcmp(argv[1], "-e2") == 0 )
	{
		if ( argc < 4 ) goto err;
		BitMap wrk;
		BitMap reversebmp;

		bmp.Read(argv[2]);
		wrk.Read(argv[3]);

		reversebmp.Create(bmp.W(), bmp.H());
		for ( int i = 0; i < wrk.H(); i++ )
		{
			for ( int j = 0; j < bmp.W(); j++)
			{
				if ( bmp.cell(i,j).r != 0 && bmp.cell(i,j).g != 0 && bmp.cell(i,j).b != 0 )
				{
					reversebmp.cell(i, j) =  wrk.cell(i,j);
					continue;
				}else
				{
					reversebmp.cell(i, j).r = 0;
					reversebmp.cell(i, j).g = 0;
					reversebmp.cell(i, j).b = 0;
				}
			}
		}
		reversebmp.Write("e2.bmp");
	}

	if ( strcmp(argv[1], "-e3") == 0 )
	{
		if ( argc < 4 ) goto err;
		BitMap wrk;
		BitMap reversebmp;

		bmp.Read(argv[2]);
		wrk.Read(argv[3]);

		reversebmp.Create(bmp.W(), bmp.H());
		for ( int i = 0; i < bmp.H(); i++ )
		{
			for ( int j = 0; j < bmp.W(); j++)
			{
				if ( bmp.cell(i,j).r != 0 && bmp.cell(i,j).g != 0 && bmp.cell(i,j).b != 0 )
				{
					reversebmp.cell(i, j) =  bmp.cell(i,j);
					continue;
				}else
				{
					reversebmp.cell(i, j) =  wrk.cell(i,j);
				}
			}
		}
		reversebmp.Write("e3.bmp");
	}

	if ( strcmp(argv[1], "-e4") == 0 )
	{
		if ( argc < 4 ) goto err;
		BitMap wrk;
		BitMap reversebmp;

		bmp.Read(argv[2]);
		wrk.Read(argv[3]);

		reversebmp.Create(bmp.W(), bmp.H());
		for ( int i = 0; i < bmp.H(); i++ )
		{
			for ( int j = 0; j < bmp.W(); j++)
			{
				if ( bmp.cell(i,j).r == 0 && bmp.cell(i,j).g == 0 && bmp.cell(i,j).b == 0 )
				{
					reversebmp.cell(i, j) =  wrk.cell(i,j);
					continue;
				}else
				{
					reversebmp.cell(i, j).r = 0;
					reversebmp.cell(i, j).g = 0;
					reversebmp.cell(i, j).b = 0;
				}
			}
		}
		reversebmp.Write("e4.bmp");
	}

	if ( strcmp(argv[1], "-e5") == 0 )
	{
		if ( argc < 3 ) goto err;
		BitMap wrk;
		BitMap reversebmp;

		bmp.Read(argv[2]);

		reversebmp.Create(bmp.W(), bmp.H());
		for ( int i = 0; i < bmp.H(); i++ )
		{
			for ( int j = 1; j < bmp.W()-1; j++)
			{
				if ( bmp.cell(i,j).r < 43 && bmp.cell(i,j).g < 43 && bmp.cell(i,j).b < 43 )
				{
					if ( bmp.cell(i,j-1).r > 200 && bmp.cell(i,j-1).g > 200 && bmp.cell(i,j-1).b > 200 )
					{
						reversebmp.cell(i, j-1).r = 0;
						reversebmp.cell(i, j-1).g = 0;
						reversebmp.cell(i, j-1).b = 0;
						continue;
					}
					if ( bmp.cell(i,j+1).r > 200 && bmp.cell(i,j+1).g > 200 && bmp.cell(i,j+1).b > 200 )
					{
						reversebmp.cell(i, j+1).r = 0;
						reversebmp.cell(i, j+1).g = 0;
						reversebmp.cell(i, j+1).b = 0;
						continue;
					}
				}
				reversebmp.cell(i, j) =  bmp.cell(i,j);
			}
		}
		reversebmp.Write("e5.bmp");
	}


	if ( strcmp(argv[1], "-e6") == 0 )
	{
		if ( argc < 4 ) goto err;
		BitMap wrk;
		BitMap bmp2;

		bmp.Read(argv[2]);
		bmp2.Read(argv[3]);

		wrk.Create(bmp.W(), bmp.H()+bmp2.H());
		for ( int i = 0; i < bmp2.H(); i++ )
		{
			for ( int j = 0; j < bmp2.W(); j++)
			{
				wrk.cell(i,j) = bmp2.cell(i,j);
			}
		}
		int k = bmp2.H();
		for ( int i = 0; i < bmp.H(); i++ )
		{
			for ( int j = 0; j < bmp.W(); j++)
			{
				wrk.cell(i+k,j) = bmp.cell(i,j);
			}
		}
		wrk.Write("e6.bmp");
	}
	if ( strcmp(argv[1], "-e6_t") == 0 )
	{
		if ( argc < 3 ) goto err;

		int W1, H1;
		int W2, H2;
		double* bmp = TextRawRead(argv[2], W1, H1);
		double* bmp2 = TextRawRead(argv[3], W2, H2);

		double* wrk = new double[W1*(H1+H2)];
		memset(wrk, '\0', sizeof(double)*W1*(H1+H2));

		for ( int i = 0; i < H2; i++ )
		{
			for ( int j = 0; j < W2; j++)
			{
				wrk[W1*i + j] = bmp2[W2*i+j];
			}
		}
		int k = H2;
		for ( int i = 0; i < H1; i++ )
		{
			for ( int j = 0; j < W1; j++)
			{
				wrk[W1*(i+k) + j] = bmp[W1*i+j];
			}
		}
		TextWriteCsv("e6_t.csv", W1, H1+H2, wrk, 1.0);

		int hh = H1+H2;
		int ww = W1;
		if ( argc == 5 )
		{
			FILE* fp = fopen("Parameter.txt", "w");
			printf("%s\n", argv[4]);
			fprintf(fp, "%s\n", argv[4]);
			double a = wrk[0];
			double h = (int)a;
			double m = (int)((a-h)*60.0);
			double s = ((a-h)*60.0 -m)*60.0;
			printf("%.1f, %.1f, %.1f\n",  h, m, s);
			fprintf(fp, "%.1f, %.1f, %.1f\n",  h, m, s);

			a = wrk[(ww-1)];
			h = (int)a;
			m = (int)((a-h)*60.0);
			s = ((a-h)*60.0 -m)*60.0;
			printf("%.1f %.1f %.1f\n",  h, m, s);
			fprintf(fp, "%.1f %.1f %.1f\n",  h, m, s);

			a = wrk[(hh-1)*ww];
			h = (int)a;
			m = (int)((a-h)*60.0);
			s = ((a-h)*60.0 -m)*60.0;
			printf("%.1f, %.1f, %.1f\n",  h, m, s);
			fprintf(fp, "%.1f, %.1f, %.1f\n",  h, m, s);

			a = wrk[(hh-1)*ww+(ww-1)];
			h = (int)a;
			m = (int)((a-h)*60.0);
			s = ((a-h)*60.0 -m)*60.0;
			printf("%.1f, %.1f, %.1f\n",  h, m, s);
			fprintf(fp, "%.1f, %.1f, %.1f\n\n",  h, m, s);
		}

		delete [] bmp;
		delete [] bmp2;
		delete [] wrk;
	}
	if ( strcmp(argv[1], "-e7_t") == 0 )
	{
		int w, h;
		LonLat* data = TextPointRawRead(argv[2], w, h);

		FILE* fp;
		if((fp = fopen(argv[3], "w")) == NULL){
			fprintf(stderr, "Error: %s could not open.", argv[3]);
			return 1;
		}

		for(int i=0; i<h; i++){
			for(int j=0; j<w-1; j++){
				fprintf(fp, "%f ", (data[w*i + j].lon));
			}
			fprintf(fp, "%f\n", (data[w*i+w-1].lon));
		}
		fclose(fp);

		if((fp = fopen(argv[4], "w")) == NULL){
			fprintf(stderr, "Error: %s could not open.", argv[4]);
			return 1;
		}

		for(int i=0; i<h; i++){
			for(int j=0; j<w-1; j++){
				fprintf(fp, "%f ", (data[w*i + j].lat));
			}
			fprintf(fp, "%f\n", (data[w*i+w-1].lat));
		}
		fclose(fp);

		if((fp = fopen(argv[5], "w")) == NULL){
			fprintf(stderr, "Error: %s could not open.", argv[5]);
			return 1;
		}

		for(int i=0; i<h; i++){
			for(int j=0; j<w-1; j++){
				fprintf(fp, "%.1f ", (data[w*i + j].hi));
			}
			fprintf(fp, "%.1f\n", (data[w*i+w-1].hi));
		}
		fclose(fp);
	}
	

	return 0;

err:	;
	printf("画像のクリップ\nbitmapedit.exe -clip startX endX startY endY bitmapfile\n");
	printf("白以外の箇所を黒に変更\nbitmapedit.exe -e1 bitmapfile\n");
	printf("bitmapfile1の黒の箇所 + 黒以外の箇所はbitmapfile2 =>合成\nbitmapedit.exe -e2 bitmapfile1 bitmapfile2\n");
	printf("bitmapfile1の黒の箇所はbitmapfile2と同じ =>合成\nbitmapedit.exe -e3 bitmapfile1 bitmapfile2\n");
	printf("bitmapfile1の黒の箇所はbitmapfile2と同じ　それ以外は黒 =>合成\nbitmapedit.exe -e4 bitmapfile1 bitmapfile2\n");
	printf("bitmapfile1の黒の境界の白を除去する\nbitmapedit.exe -e5 bitmapfile1\n");
}

