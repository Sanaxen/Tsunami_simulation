#include"bitmap.h"

unsigned char BitMap::colorTbl[1024][3];
int BitMap::colortableNum;


Image *BitMap::Read_Bmp(char *filename)
{
	int i, j;
	int real_width;
	unsigned int width, height;
	unsigned int color;
	FILE *fp;
	unsigned char header_buf[HEADERSIZE];
	unsigned char *bmp_line_data;
	Image *img;

	if((fp = fopen(filename, "rb")) == NULL){
		fprintf(stderr, "Error: %s could not read.\n", filename);
		return NULL;
	}

	fread(header_buf, sizeof(unsigned char), HEADERSIZE, fp);
	if(strncmp((char*)header_buf, "BM", 2)){
		fprintf(stderr, "Error: %s is not Bitmap file.\n", filename);
		return NULL;
	}

	memcpy(&width, header_buf + 18, sizeof(width));
	memcpy(&height, header_buf + 22, sizeof(height));
	memcpy(&color, header_buf + 28, sizeof(unsigned int));

	if(color != 24){
		fprintf(stderr, "Error: %s is not 24bit color image\n", filename);
		return NULL;
	}

	real_width = width*3 + width%4;

	if((bmp_line_data = (unsigned char *)malloc(sizeof(unsigned char)*real_width)) == NULL){
		fprintf(stderr, "Error: Allocation error.\n");
		return NULL;
	}

	if((img = Create_Image(width, height)) == NULL){
		free(bmp_line_data);
		fclose(fp);
		return NULL;
	}

	for(i=0; i<height; i++){
		fread(bmp_line_data, 1, real_width, fp);
		for(j=0; j<width; j++){
			img->data[(height-i-1)*width + j].b = bmp_line_data[j*3];
			img->data[(height-i-1)*width + j].g = bmp_line_data[j*3 + 1];
			img->data[(height-i-1)*width + j].r = bmp_line_data[j*3 + 2];
		}
	}

	free(bmp_line_data);

	fclose(fp);

	return img;
}


void BitMap::Offset( int size )
{
	if ( size == 0 ) return;

	Image* d = Create_Image( data->width+size*2, data->height+size*2);

	if ( size > 0 )
	{
		int ii = 0;
		int jj = 0;
		for(int i=size; i< d->height-size; i++, ii++){
			jj = 0;
			for(int j=size; j<d->width-size; j++, jj++){
				d->data[i*d->width + j].b = data->data[ii*data->width + jj].b;
				d->data[i*d->width + j].g = data->data[ii*data->width + jj].g;
				d->data[i*d->width + j].r = data->data[ii*data->width + jj].r;
			}
		}
	}else
	{
		size = -size;
		int ii = size;
		int jj = 0;
		for(int i=0; i< d->height; i++, ii++){
			jj = 0;
			for(int j=0; j<d->width; j++, jj++){
				d->data[i*d->width + j].b = data->data[ii*data->width + jj+size].b;
				d->data[i*d->width + j].g = data->data[ii*data->width + jj+size].g;
				d->data[i*d->width + j].r = data->data[ii*data->width + jj+size].r;
			}
		}
	}
	free(data->data);
	data = d;
}


int BitMap::Write_Bmp(char *filename, Image *img)
{
	int i, j;
	FILE *fp;
	int real_width;
	unsigned char *bmp_line_data;
	unsigned char header_buf[HEADERSIZE];
	unsigned int file_size;
	unsigned int offset_to_data;
	unsigned long info_header_size;
	unsigned int planes;
	unsigned int color;
	unsigned long compress;
	unsigned long data_size;
	long xppm;
	long yppm;

	if((fp = fopen(filename, "wb")) == NULL){
		fprintf(stderr, "Error: %s could not open.\n", filename);
		return 1;
	}

	real_width = img->width*3 + img->width%4;

	file_size = img->height * real_width + HEADERSIZE;
	offset_to_data = HEADERSIZE;
	info_header_size = INFOHEADERSIZE;
	planes = 1;
	color = 24;
	compress = 0;
	data_size = img->height * real_width;
	xppm = 1;
	yppm = 1;
	
	header_buf[0] = 'B';
	header_buf[1] = 'M';
	memcpy(header_buf + 2, &file_size, sizeof(file_size));
	header_buf[6] = 0;
	header_buf[7] = 0;
	header_buf[8] = 0;
	header_buf[9] = 0;
	memcpy(header_buf + 10, &offset_to_data, sizeof(file_size));
	header_buf[11] = 0;
	header_buf[12] = 0;
	header_buf[13] = 0;

	memcpy(header_buf + 14, &info_header_size, sizeof(info_header_size));
	header_buf[15] = 0;
	header_buf[16] = 0;
	header_buf[17] = 0;
	memcpy(header_buf + 18, &img->width, sizeof(img->width));
	memcpy(header_buf + 22, &img->height, sizeof(img->height));
	memcpy(header_buf + 26, &planes, sizeof(planes));
	memcpy(header_buf + 28, &color, sizeof(color));
	memcpy(header_buf + 30, &compress, sizeof(compress));
	memcpy(header_buf + 34, &data_size, sizeof(data_size));
	memcpy(header_buf + 38, &xppm, sizeof(xppm));
	memcpy(header_buf + 42, &yppm, sizeof(yppm));
	header_buf[46] = 0;
	header_buf[47] = 0;
	header_buf[48] = 0;
	header_buf[49] = 0;
	header_buf[50] = 0;
	header_buf[51] = 0;
	header_buf[52] = 0;
	header_buf[53] = 0;

	fwrite(header_buf, sizeof(unsigned char), HEADERSIZE, fp);
	
	if((bmp_line_data = (unsigned char *)malloc(sizeof(unsigned char)*real_width)) == NULL){
		fprintf(stderr, "Error: Allocation error.\n");
		fclose(fp);
		return 1;
	}

	for(i=0; i<img->height; i++){
		for(j=0; j<img->width; j++){
			bmp_line_data[j*3]			=	img->data[(img->height - i - 1)*img->width + j].b;
			bmp_line_data[j*3 + 1]	=	img->data[(img->height - i - 1)*img->width + j].g;
			bmp_line_data[j*3 + 2]			=	img->data[(img->height - i - 1)*img->width + j].r;
		}
		for(j=img->width*3; j<real_width; j++){
			bmp_line_data[j] = 0;
		}
		fwrite(bmp_line_data, sizeof(unsigned char), real_width, fp);
	}

	free(bmp_line_data);

	fclose(fp);

	return 0;
}

Image *BitMap::Create_Image(int width, int height)
{
	Image *img;

	if((img = (Image *)malloc(sizeof(Image))) == NULL){
		fprintf(stderr, "Allocation error\n");
		return NULL;
	}

	if((img->data = (Rgb*)malloc(sizeof(Rgb)*width*height)) == NULL){
		fprintf(stderr, "Allocation error\n");
		free(img);
		return NULL;
	}

	memset(img->data, '\0', sizeof(Rgb)*width*height);
	img->width = width;
	img->height = height;

	return img;
}

void BitMap::Free_Image(Image *img)
{
	if ( img->data ) free(img->data);
	if ( img ) free(img);
}


int BitMap::WriteText(char* filename, Image* img)
{
	int i, j;
	FILE *fp;
	int real_width;
	unsigned char *bmp_line_data;

	if((fp = fopen(filename, "w")) == NULL){
		fprintf(stderr, "Error: %s could not open.\n", filename);
		return 1;
	}

	real_width = img->width*3 + img->width%4;

	
	fprintf(fp, "%d %d\n", img->width, img->height);
	if((bmp_line_data = (unsigned char *)malloc(sizeof(unsigned char)*real_width)) == NULL){
		fprintf(stderr, "Error: Allocation error.\n");
		fclose(fp);
		return 1;
	}

	for(i=0; i<img->height; i++){
		for(j=0; j<img->width; j++){
			bmp_line_data[j*3]		=	img->data[(img->height - i - 1)*img->width + j].b;
			bmp_line_data[j*3 + 1]	=	img->data[(img->height - i - 1)*img->width + j].g;
			bmp_line_data[j*3 + 2]	=	img->data[(img->height - i - 1)*img->width + j].r;
		}
		for(j=img->width*3; j<real_width; j++){
			bmp_line_data[j] = 0;
		}

		for ( int k = 0; k < real_width; k++ )
		{
			fprintf(fp, "%d\n", bmp_line_data[k]);
		}
	}

	free(bmp_line_data);

	fclose(fp);

	return 0;
}

Image* BitMap::Read_Text(char *filename)
{
	int i, j;
	int real_width;
	unsigned int width, height;
	unsigned int color;
	FILE *fp;
	unsigned char* header_buf = new unsigned char[HEADERSIZE];
	unsigned char *bmp_line_data;
	Image *img;

	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error: %s could not read.\n", filename);
		delete[] header_buf;
		return NULL;
	}

	char buf[LINELENGMAX];
	fgets(buf, LINELENGMAX, fp);

	sscanf(buf, "%d %d", &width, &height);
	color = 24;

	real_width = width*3 + width%4;

	if((bmp_line_data = (unsigned char *)malloc(sizeof(unsigned char)*real_width)) == NULL){
		fprintf(stderr, "Error: Allocation error.\n");
		delete[] header_buf;
		return NULL;
	}

	if((img = Create_Image(width, height)) == NULL){
		free(bmp_line_data);
		fclose(fp);
		delete[] header_buf;
		return NULL;
	}

	for(i=0; i<height; i++){
		for ( int k = 0; k < real_width; k++ )
		{
			int dmy;
			fgets(buf, LINELENGMAX, fp);
			sscanf(buf, "%d",  &dmy);
			bmp_line_data[k] = (unsigned char)dmy;
		}
		for(j=0; j<width; j++){
			img->data[(height-i-1)*width + j].b = bmp_line_data[j*3];
			img->data[(height-i-1)*width + j].g = bmp_line_data[j*3 + 1];
			img->data[(height-i-1)*width + j].r = bmp_line_data[j*3 + 2];
		}
	}

	delete[] header_buf;
	free(bmp_line_data);

	fclose(fp);

	return img;
}


void BitMap::ToGrayScale()
{

#pragma omp parallel for
	for(int i=0; i<data->height; i++)
	{
		for(int j=0; j<data->width; j++)
		{
			Rgb& rgb = cell(i,j);
			rgb.r = (77*rgb.r +150*rgb.g + 29*rgb.b) >> 8;
			rgb.b = rgb.r;
			rgb.g = rgb.r;
		}
	}
}

void BitMap::ToGrayScale_and_Reverse()
{

#pragma omp parallel for
	for(int i=0; i<data->height; i++)
	{
		for(int j=0; j<data->width; j++)
		{
			Rgb& rgb = cell(i,j);
			rgb.r = 255 - (77*rgb.r +150*rgb.g + 29*rgb.b) >> 8;
			rgb.b = rgb.r;
			rgb.g = rgb.r;
		}
	}
}

Image* BitMap::Read_Csv(char* filename, double min, double max)
{
	int i, j;
	int real_width;
	unsigned int width, height;
	unsigned int color;
	FILE *fp;
	unsigned char *header_buf = new unsigned char[HEADERSIZE];
	unsigned char *bmp_line_data;
	Image *img;

	if((fp = fopen(filename, "r")) == NULL){
		delete[] header_buf;
		fprintf(stderr, "Error: %s could not read.\n", filename);
		return NULL;
	}

	char buf[LINELENGMAX];
	fgets(buf, LINELENGMAX, fp);
	color = 24;

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

	real_width = width*3 + width%4;

	if((bmp_line_data = (unsigned char *)malloc(sizeof(unsigned char)*real_width)) == NULL){
		fprintf(stderr, "Error: Allocation error.\n");
		delete[] header_buf;
		return NULL;
	}

	if((img = Create_Image(width, height)) == NULL){
		free(bmp_line_data);
		fclose(fp);
		delete[] header_buf;
		return NULL;
	}

	double w = 255.0/(max - min);
	for(i=0; i<height; i++){
		fgets(buf, LINELENGMAX, fp);
		p = buf;
		for ( int k = 0; k < width; k++ )
		{
			double dmy;
			sscanf(p, "%lf",  &dmy);
			dmy = (dmy - min)*w;
			if ( dmy < 0.0 ) dmy = 0.0;
			if ( dmy > 255.0 ) dmy = 255.0;

			bmp_line_data[3*k] = (unsigned char)dmy;
			bmp_line_data[3*k+1] = (unsigned char)dmy;
			bmp_line_data[3*k+2] = (unsigned char)dmy;
			p = strchr(p, ',');
			if ( p ) p++;
		}
		for(j=img->width*3; j<real_width; j++){
			bmp_line_data[j] = 0;
		}

		for(j=0; j<width; j++){
			img->data[(height-i-1)*width + j].b = bmp_line_data[j*3];
			img->data[(height-i-1)*width + j].g = bmp_line_data[j*3 + 1];
			img->data[(height-i-1)*width + j].r = bmp_line_data[j*3 + 2];
		}
	}

	delete[] header_buf;
	free(bmp_line_data);

	fclose(fp);


	return img;
}


int BitMap::Write_Csv(char* filename, Image* img, int rgb)
{
	int i, j;
	FILE *fp;
	int real_width;

	if((fp = fopen(filename, "w")) == NULL){
		fprintf(stderr, "Error: %s could not open.\n", filename);
		return 1;
	}

	real_width = img->width*3 + img->width%4;

	
	for(i=0; i<img->height; i++){
		switch(rgb)
		{
		case 0:
			for(j=0; j<img->width-1; j++){
				fprintf(fp, "%.1f ,", (float)img->data[(img->height - i - 1)*img->width + j].r);
			}
			fprintf(fp, "%.1f\n", (float)img->data[(img->height - i - 1)*img->width + j].r);
		break;
		case 1:
			for(j=0; j<img->width-1; j++){
				fprintf(fp, "%.1f ,", (float)img->data[(img->height - i - 1)*img->width + j].g);
			}
			fprintf(fp, "%.1f\n", (float)img->data[(img->height - i - 1)*img->width + j].g);
		break;
		case 2:
			for(j=0; j<img->width-1; j++){
				fprintf(fp, "%.1f ,", (float)img->data[(img->height - i - 1)*img->width + j].b);
			}
			fprintf(fp, "%.1f\n", (float)img->data[(img->height - i - 1)*img->width + j].b);
		break;
		}

	}
	fclose(fp);

	return 0;
}


void BitMap::Reverse()
{
#pragma omp parallel for
	for(int i=0; i<data->height; i++)
	{
		for(int j=0; j<data->width; j++)
		{
			Rgb& rgb = cell(i,j);
			rgb.r = 255 - rgb.r;
			rgb.b = rgb.r;
			rgb.g = rgb.r;
		}
	}
}

void BitMap::convolve_smooth(int *mask, double conv[3][3])
{
	double tmp[3];


	Image* img = Create_Image(data->width, data->height);

	/*èÙÇ›çûÇﬁï™ïzÇÃêÈåæÅBÇ±Ç±ÇïœÇ¶ÇÈÇ∆ÉXÉÄÅ[ÉWÉìÉìÉOÇ≈Ç»Ç≠Ç»ÇÈ
	Ç±ÇÃèÍçáÇÕÇRxÇRâÊëfÇÃïΩãœÇ™ê^ÇÒíÜÇ…ì¸ÇÈÇÊÇ§Ç…Ç¬Ç≠Ç¡ÇƒÇ†ÇÈÅB*/

	for(int j=1;j<data->height-1;j++)
	{
		for(int i=1;i<data->width-1;i++)
		{
			if ( mask )
			{
				if ( mask[(data->height-1-j)*data->width+i] == 0 )
				{
					img->data[j*data->width+i].r = data->data[j*data->width+i].r;
					img->data[j*data->width+i].g = data->data[j*data->width+i].g;
					img->data[j*data->width+i].b = data->data[j*data->width+i].b;
					continue;
				}
			}

			/*Ç‹Ç∏ÇÕÅAÇXÇ¬ÇÃílÇÇÊÇ—ÅAÇªÇÍÇºÇÍÇ…èÙÇ›çûÇﬁï™ïzÇä|ÇØ
			ÇªÇÃÇ∑Ç◊ÇƒÇÃílÇâ¡éZÇµÇΩåãâ Ç tmpÇ…ï€ë∂Ç∑ÇÈ*/

    
			tmp[0] =
			data->data[(j-1)*data->width+(i-1)].r*conv[0][0]+
			data->data[(j-1)*data->width+i    ].r*conv[0][1]+
			data->data[(j-1)*data->width+(i+1)].r*conv[0][2]+

			data->data[j  *data->width+(i-1)].r*conv[1][0]+
			data->data[j  *data->width+i    ].r*conv[1][1]+
			data->data[j  *data->width+(i+1)].r*conv[1][2]+

			data->data[(j+1)*data->width+(i-1)].r*conv[2][0]+
			data->data[(j+1)*data->width+i    ].r*conv[2][1]+
			data->data[(j+1)*data->width+(i+1)].r*conv[2][2];

			tmp[1] =
			data->data[(j-1)*data->width+(i-1)].g*conv[0][0]+
			data->data[(j-1)*data->width+i    ].g*conv[0][1]+
			data->data[(j-1)*data->width+(i+1)].g*conv[0][2]+

			data->data[j  *data->width+(i-1)].g*conv[1][0]+
			data->data[j  *data->width+i    ].g*conv[1][1]+
			data->data[j  *data->width+(i+1)].g*conv[1][2]+

			data->data[(j+1)*data->width+(i-1)].g*conv[2][0]+
			data->data[(j+1)*data->width+i    ].g*conv[2][1]+
			data->data[(j+1)*data->width+(i+1)].g*conv[2][2];

			tmp[2] =
			data->data[(j-1)*data->width+(i-1)].g*conv[0][0]+
			data->data[(j-1)*data->width+i    ].g*conv[0][1]+
			data->data[(j-1)*data->width+(i+1)].g*conv[0][2]+

			data->data[j  *data->width+(i-1)].b*conv[1][0]+
			data->data[j  *data->width+i    ].b*conv[1][1]+
			data->data[j  *data->width+(i+1)].b*conv[1][2]+

			data->data[(j+1)*data->width+(i-1)].b*conv[2][0]+
			data->data[(j+1)*data->width+i    ].b*conv[2][1]+
			data->data[(j+1)*data->width+(i+1)].b*conv[2][2];

			///*ÇXâÊëfÇÃïΩãœÇ»ÇÃÇ≈,ÇXÇ≈äÑÇÈ*/
			//tmp[0] /= 9.0;
			//tmp[1] /= 9.0;
			//tmp[2] /= 9.0;

			if ( tmp[0] < 0 ) tmp[0] = 0;
			if ( tmp[0] > 255 ) tmp[0] = 255;
			if ( tmp[1] < 0 ) tmp[1] = 0;
			if ( tmp[1] > 255 ) tmp[1] = 255;
			if ( tmp[2] < 0 ) tmp[2] = 0;
			if ( tmp[2] > 255 ) tmp[2] = 255;

			/*äÑÇ¡ÇΩílÇë„ì¸*/
			img->data[j*data->width+i].r = (unsigned char)tmp[0];
			img->data[j*data->width+i].g = (unsigned char)tmp[1];
			img->data[j*data->width+i].b = (unsigned char)tmp[2];
		}
	}

	free( data->data);
	data->data = img->data;

	img->data = NULL;
	Free_Image( img );
}

void BitMap::ColorTable(BitMap& colormap)
{
	printf("%d %d\n", colormap.W(), colormap.H());

	int n = colormap.H();
	if ( n >= 1024 ) n = 1024;
	int w = colormap.W()/2;

	colortableNum = n-1;
	printf("colortableNum %d\n", colortableNum);
	for ( int i = 0; i < n; i++ )
	{
		colorTbl[i][0] = colormap.cell(i, w).r;
		colorTbl[i][1] = colormap.cell(i, w).g;
		colorTbl[i][2] = colormap.cell(i, w).b;
		//printf("(%d) %d %d %d\n", i, colorTbl[i][0], colorTbl[i][1], colorTbl[i][2]);
	}
}

void BitMap::ColorTable()
{
	colortableNum = 255;
	// ê¬Å®óŒ 
	for (int i = 0; i < 64; i++) 
	{ 
		int green = i * 4;
		colorTbl[i][0] = 0;
		colorTbl[i][1] = green;
		colorTbl[i][2] = 255 - green;
	} 
	// óŒÅ®â© 
	for (int i = 0; i < 64; i++) 
	{ 
		int red = i * 4;
		colorTbl[i+64][0] = red;
		colorTbl[i+64][1] = 255;
		colorTbl[i+64][2] = 0;
	} 
	// â©Å®ê‘ 
	for (int i = 0; i < 128; i++) 
	{ 
		int green = 255 - i * 2; 
		colorTbl[i+128][0] = 255;
		colorTbl[i+128][1] = green;
		colorTbl[i+128][2] = 0;
	}
}
void BitMap::ColorTable(int startIndex, int endIndex, unsigned char start[3], unsigned char end[3])
{
	colortableNum = 255;
	double dr = ((double)(end[0] - start[0])/(double)(endIndex-startIndex));
	double dg = ((double)(end[1] - start[1])/(double)(endIndex-startIndex));
	double db = ((double)(end[2] - start[2])/(double)(endIndex-startIndex));

	for ( int  i = startIndex; i <= endIndex; i++ )
	{
		int ir = (int)floor( start[0] + i*dr );
		int ig = (int)floor( start[1] + i*dg );
		int ib = (int)floor( start[2] + i*db );

		colorTbl[i][0] = ir;
		colorTbl[i][1] = ig;
		colorTbl[i][2] = ib;
	}
}

void BitMap::ColorLevel( double min, double max, double* z, double zmask, unsigned char* maskcolor, int* top, double* elv)
{
	double b = 50;
	double w = colortableNum/(max - min);
	
	int zero = (0.0-min)/(max-min);
#pragma omp parallel for
	for ( int i = 0; i < data->height; i++ )
	{
		for ( int j = 0; j < data->width; j++ )
		{
			double zz = z[(data->height-i-1)*data->width + j];
			//if ( top && top[(data->height-i-1)*data->width + j] == 0 && elv )
			//{
			//	zz = zz - elv[(data->height-i-1)*data->width + j];
			//}
			if ( fabs(zz) < zmask )
			{
				if ( maskcolor )
				{
					data->data[i*data->width + j].r = maskcolor[0];
					data->data[i*data->width + j].g = maskcolor[1];
					data->data[i*data->width + j].b = maskcolor[2];
				}
				continue;
			}

			double dmy  = (zz-min)*w;

			if ( dmy <= zero )
			{
				dmy = ((zz - min)/(0.0 - min))*b;
			}else
			{
				dmy = b+((zz - 0.0)/(max - 0.0))*(colortableNum - b-1);
			}

			if ( dmy < 0.0 ) dmy = 0.0;
			if ( dmy > colortableNum ) dmy = colortableNum;

			int index = (int)dmy;
			data->data[i*data->width + j].r = colorTbl[index][0];
			data->data[i*data->width + j].g = colorTbl[index][1];
			data->data[i*data->width + j].b = colorTbl[index][2];
		}
	}
}