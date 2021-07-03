#ifndef __BITMAP_H_INCLUDED__
#define __BITMAP_H_INCLUDED__

#define FILEHEADERSIZE 14
#define INFOHEADERSIZE 40
#define HEADERSIZE (FILEHEADERSIZE+INFOHEADERSIZE)

#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<math.h>

#include <omp.h>

#define LINELENGMAX	(4096*10)

typedef struct{
	unsigned char b;
	unsigned char g;
	unsigned char r;
}Rgb;

typedef struct{
	unsigned int height;
	unsigned int width;
	Rgb *data;
}Image;


class BitMap
{
	Image* data;
	Image *Read_Bmp(char *filename);
	void Free_Image(Image *img);
	int Write_Bmp(char *filename, Image *img);
	Image *Create_Image(int width, int height);
	int WriteText(char* filename, Image* img);
	Image* Read_Text(char *filename);
	Image* Read_Csv(char* filename, double min, double max);
	int Write_Csv(char* filename, Image* img, int rgb);

public:

	BitMap()
	{
		data = NULL;
	}

	~BitMap()
	{
		Clear();
	}

	void Clear()
	{
		if ( data ) Free_Image(data);
		data = NULL;
	}

	void Create(int width, int height)
	{
		data = Create_Image(width, height);
	}

	inline Image* GetImage() const
	{ 
		return data;
	}
	inline int W()
	{
		return data->width;
	}
	inline int H()
	{
		return data->height;
	}

	void Write(char *filename)
	{
		Write_Bmp(filename, data);
	}
	void Read(char *filename)
	{
		data = Read_Bmp( filename);
	}

	void WriteText(char *filename)
	{
		WriteText( filename, data);
	}

	void ReadText(char *filename)
	{
		Read_Text(filename);
	}

	inline Rgb& cell(int i, int j)
	{
		return data->data[(data->height-i-1)*data->width + j];
	}

	void ToGrayScale();

	void ReadCsv(char* filename, double min, double max)
	{
		if ( data ) Free_Image(data);
		data = Read_Csv(filename, min, max);
	}

	void WriteCsv(char* filename, int rgb)
	{
		Write_Csv( filename, data, rgb);
	}

	void Reverse();
	void ToGrayScale_and_Reverse();

	void Offset(int size);
	void convolve_smooth(int* mask, double mat[3][3]);
	
	void convolve_smooth(int* mask)
	{
		double tmp[3],conv[3][3]={{1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0}};
		convolve_smooth(mask, conv);
	}


	static int colortableNum;
	static unsigned char colorTbl[1024][3];
	void ColorTable();
	void ColorTable(BitMap& colormap);
	void ColorTable(int startIndex, int endIndex,unsigned char start[3], unsigned char end[3]);
	void ColorLevel( double min, double max, double* z, double zmask, unsigned char* maskcolor=NULL, int* top=NULL, double* elv=NULL);

};


#endif /*__BITMAP_H_INCLUDED__*/
