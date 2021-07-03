#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "CsvReadWrite.hpp"

double* ReadCsv( char* filename, int& w, int& h)
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

	char* buf = new char[LINELENGMAX];
	fgets(buf, LINELENGMAX, fp);

	char* p = buf;
	width = 0;
	do{
		p = strchr(p, ',');
		if ( p )
		{
			width++;
			p++;
		}
	}while( p );
	width++;


	height = 1;
	while( fgets(buf, LINELENGMAX, fp) ) height++;



	fclose(fp);
	fp = fopen(filename, "r");

	if((line_data = (double *)malloc(sizeof(double)*width)) == NULL){
		fprintf(stderr, "Error: Allocation error.\n");
		delete [] buf;
		return NULL;
	}
	memset(line_data, '\0', sizeof(double)*width);

	w = width;
	h = height;
	data = new double[w*h];
	memset(data, '\0', w*h*sizeof(double));

	for(i=0; i<height; i++){
		memset(line_data, '\0', sizeof(double)*width);

		fgets(buf, LINELENGMAX, fp);
		p = buf;
		for ( int k = 0; k < width; k++ )
		{
			double dmy;
			if (sscanf(p, "%lf", &dmy) == 1)
			{
				line_data[k] = dmy;
			}
			p = strchr(p, ',');
			if (p) p++;
		}

#pragma omp parallel for
		for(j=0; j<width; j++){
			data[w*i + j] = line_data[j];
		}
	}

	free(line_data);

	fclose(fp);
	delete [] buf;


	return data;
}

int WriteCsv(char* filename, int w, int h, double* data, double scale)
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
			fprintf(fp, "%f ,", data[w*i + j]*scale);
		}
		fprintf(fp, "%f\n", data[w*i+j]*scale);
	}
	fclose(fp);

	return 0;
}

int WriteCsv2(char* filename, int w, int h, double* data1, double* data2, double scale)
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
			double vx = data1[w*i + j];
			double vy = data2[w*i + j];
			fprintf(fp, "%f ,", sqrt(vx*vx + vy*vy)*scale);
		}	
		double vx = data1[w*i + j];
		double vy = data2[w*i + j];
		fprintf(fp, "%f\n", sqrt(vx*vx + vy*vy)*scale);
	}
	fclose(fp);

	return 0;
}

int WriteCsv(char* filename, int w, int h, int* data, double scale)
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
			fprintf(fp, "%f ,", data[w*i + j]*scale);
		}
		fprintf(fp, "%f\n", data[w*i+j]*scale);
	}
	fclose(fp);

	return 0;
}
