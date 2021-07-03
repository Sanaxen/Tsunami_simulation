#ifndef _BITMAPTEXTURE_H

#define _BITMAPTEXTURE_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GL/freeglut.h"

class BitMapTexture
{
public:
	unsigned long sizeX;  //横
	unsigned long sizeY; //縦
	unsigned char *Data;  //画像データ格納
	GLuint texture;

	BitMapTexture(char *FileName){
		Load(FileName);
		//TexSet();
		glGenTextures(1, &texture);
	}

	bool Load(char *FileName) {
		FILE *File;
		unsigned long size;// イメージのバイトサイズ
		unsigned long i;// カウンタ
		unsigned short int planes;        //デバイス面数
		unsigned short int bpp;            // ピクセル数
		unsigned char temp;                         // カラー関係作業用
		//ファイルオープン
		if ((File = fopen(FileName, "rb"))==NULL){
			printf("ファイルがありません");
			return false;
		}
		//ビットマップの幅データ部分へ移動
		fseek(File, 18, SEEK_CUR);
		//横幅を読み込む
		if ((i = fread(&sizeX, 4, 1, File)) != 1) {
			printf("読み込みエラー");
			return false;
		}
		//縦幅を読み込む
		if ((i = fread(&sizeY, 4, 1, File)) != 1) {
			printf("読み込みエラー");
			return false;
		}
		//画像サイズの計算
		size = sizeX * sizeY * 3;//プレーン数を読み込む
		if ((fread(&planes, 2, 1, File)) != 1) {   //bmpは「1」になる
			printf("プレーン数が読み込めません");
			return false;
		}
		if (planes != 1) {
			printf("プレーン数が1以外です");
			return false;
		}
		//ピクセル値を読み込む
		if ((i = fread(&bpp, 2, 1, File)) != 1) {
			printf("ビット数が読めません");
			return false;
		}
		if (bpp != 24) {//24bppでなければエラー
			printf("24ビット画像ではありません");
			return false;
		}
		//24ビット飛ばして、カラーデータ(RGB)部分へ
		fseek(File, 24, SEEK_CUR);    //データ読み込み
		Data = (unsigned char *) malloc(size);
		if (Data == NULL) {
			printf("メモリが確保できません");
			return false;
		}
		if ((i = fread(Data, size, 1, File)) != 1) {
			printf("データが読めません");
			return false;
		}
		for (i=0;i<size;i+=3) { //bgr -> rgb
			temp = Data[i];
			Data[i] = Data[i+2];
			Data[i+2] = temp;
		}
		return true;
	}

	void TexSet()
	{
		 //glEnable( GL_TEXTURE_2D );
		 //glBindTexture( GL_TEXTURE_2D, texture );
		 glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		 glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		 glTexImage2D( GL_TEXTURE_2D, 0, 3, sizeX, sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, Data );
		 //glBindTexture( GL_TEXTURE_2D, 0 );
	}
};

#endif
