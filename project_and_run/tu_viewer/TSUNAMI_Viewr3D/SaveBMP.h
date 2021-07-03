#if !defined(AFX_SAVEBMP_H__5C549B8C_451F_44BE_AF75_42C2534F0FC3__INCLUDED_)
#define AFX_SAVEBMP_H__5C549B8C_451F_44BE_AF75_42C2534F0FC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveBMP.h : ヘッダー ファイル
//
#include <stdio.h>
#  include "GL/freeglut.h"
#include "WinGDI.h"

/////////////////////////////////////////////////////////////////////////////
// CSaveBMP ウィンドウ

class CSaveBMP 
{
//openGLで取得した画像データをBMPファイル形式で保存する
//opnGLの色情報はRGBの順

// コンストラクション
public:
	CSaveBMP();

// アトリビュート
public:
	int width,height;
	int glByteWidth;	// 実際の横幅のバイト数
	unsigned char *pixel_data;

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。

	//{{AFX_VIRTUAL(CSaveBMP)
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	void SetSize(unsigned char *Pixeldata,int width, int height);
	void WriteInfoHeader(BITMAPINFOHEADER *info, FILE *fp);
	void WriteHeader(BITMAPFILEHEADER *header, FILE *fp);
	void InitHeader(BITMAPFILEHEADER *header, BITMAPINFOHEADER *info);
	void saveBMP(char* filename);
	virtual ~CSaveBMP();

	// 生成されたメッセージ マップ関数
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SAVEBMP_H__5C549B8C_451F_44BE_AF75_42C2534F0FC3__INCLUDED_)
