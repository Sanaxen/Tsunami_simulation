// SaveBMP.cpp : インプリメンテーション ファイル
//

#include "SaveBMP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaveBMP

CSaveBMP::CSaveBMP()
{
}

CSaveBMP::~CSaveBMP()
{
}




/////////////////////////////////////////////////////////////////////////////
// CSaveBMP メッセージ ハンドラ

void CSaveBMP::saveBMP(char* filename)
{
	FILE *fp;
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER info;

	int j=0;
	int x;
	int y;
	int alignmentParam;
	unsigned char zero=0;

	// ファイルオープン
	if( ( fp = fopen(filename, "wb") )==NULL){
		return ;
	}

	//ヘッダ構造体の初期化
	InitHeader(&header, &info);

	//Bitmapサイズ
	info.biWidth = width;
	info.biHeight = height;

	int writeWidth;
	
	///////////////////////////////////////////////////////////////////////////
	// BITMAP用の横幅の処理
	// データの幅のバイト数が4の倍数であるかをチェック
	if( width*3%4 == 0)
		writeWidth = width*3;
	else
		// そうでなければ，4の倍数にあわせた幅のバイトサイズにする
		writeWidth = width*3 + 4 - (width*3)%4;
	
	///////////////////////////////////////////////////////////////////////////
	//ファイル容量
	header.bfSize =
		writeWidth*height		//ビット情報量
		+ 14					//BitmapHeader構造体サイズ
		+ 40;					//BitmapInfoHeader構造体サイズ

	//ヘッダ書き込み
	WriteHeader(&header,fp);
	WriteInfoHeader(&info,fp);

	///////////////////////////////////////////////////////////////////////////
	// イメージデータ書き込み

	// 1画素３バイトとするとひとつの行は3*widthバイト＋アルファ
	// データの幅のバイト数がalignmentParamの倍数であるかをチェック
	glGetIntegerv( GL_PACK_ALIGNMENT,&alignmentParam);

	if( width * 3 % alignmentParam == 0)
		glByteWidth = width * 3;
	else
		// そうでなければ，alignmentParamの倍数にあわせた幅のバイトサイズにする
		glByteWidth = width * 3 + alignmentParam - (width*3)%alignmentParam;

	printf("image:width:%d  height:%d [Byte:%d]\n",width,height, glByteWidth);

	for( y=0 ; y < height ; y++ ){
		// データをBGRの順で書き込み
		for( x=0 ; x < width ; x++ ){
			j=fwrite((pixel_data+x*3+glByteWidth*y+2),sizeof(unsigned char),1,fp);
			j=fwrite((pixel_data+x*3+glByteWidth*y+1),sizeof(unsigned char),1,fp);
			j=fwrite((pixel_data+x*3+glByteWidth*y),sizeof(unsigned char),1,fp);
		}

		// 幅のバイト数が4の倍数でないときは０で埋める
		if( width*3%4 != 0)
			for( int j=0;j<4-(width*3)%4;j++)
				fwrite(&zero,sizeof(unsigned char),1,fp);
	}

	///////////////////////////////////////////////////////////////////////////
	//// メモリ開放
	//free(pixel_data);

	// ファイルクローズ
	fclose(fp);
}

void CSaveBMP::InitHeader(BITMAPFILEHEADER *header, BITMAPINFOHEADER *info)
{
	header->bfType = 'MB';
	header->bfSize = 0;
	header->bfReserved1 = 0;
	header->bfReserved2 = 0;
	header->bfOffBits = 54;

	info->biSize = 40;
	info->biWidth = 0;
	info->biHeight = 0;
	info->biPlanes = 1;
	info->biBitCount = 24;
	info->biCompression = 0;
	info->biSizeImage = 0;
	info->biXPelsPerMeter = 0;
	info->biYPelsPerMeter = 0;
	info->biClrUsed = 0;
	info->biClrImportant = 0;
}

void CSaveBMP::WriteHeader(BITMAPFILEHEADER *header, FILE *fp)
{
	//BMPファイルのファイル情報を保存

	fwrite(&(header->bfType), sizeof(WORD),1,fp);
	fwrite(&(header->bfSize), sizeof(DWORD),1,fp);
	fwrite(&(header->bfReserved1), sizeof(WORD),1,fp);
	fwrite(&(header->bfReserved2), sizeof(WORD),1,fp);
	fwrite(&(header->bfOffBits), sizeof(DWORD),1,fp);
}

void CSaveBMP::WriteInfoHeader(BITMAPINFOHEADER *info, FILE *fp)
{
	//BMPファイルのヘダ情報を保存

	fwrite(&(info->biSize), sizeof(int),1,fp);
	fwrite(&(info->biWidth), sizeof(int),1,fp);
	fwrite(&(info->biHeight), sizeof(int),1,fp);
	fwrite(&(info->biPlanes), sizeof(short),1,fp);
	fwrite(&(info->biBitCount), sizeof(short),1,fp);
	fwrite(&(info->biCompression), sizeof(int),1,fp);
	fwrite(&(info->biSizeImage), sizeof(int),1,fp);
	fwrite(&(info->biXPelsPerMeter), sizeof(int),1,fp);
	fwrite(&(info->biYPelsPerMeter), sizeof(int),1,fp);
	fwrite(&(info->biClrUsed), sizeof(int),1,fp);
	fwrite(&(info->biClrImportant), sizeof(int),1,fp);
}

void CSaveBMP::SetSize(unsigned char *Pixeldata,int width, int height)
{
  this->pixel_data = Pixeldata;
	this->width =width;
	this->height = height;

}
