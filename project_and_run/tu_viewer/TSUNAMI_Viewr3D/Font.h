//-----------------------------------------------------------------------
// File : Font.h
// Desc : Font System
// Date : Jan. 15, 2010
// Version : 1.0
// Author : Pocol
//-----------------------------------------------------------------------

#ifndef __FONT_H__
#define __FONT_H__

//
// Includes
//
#include <iostream>
#include <windows.h>
#include <GL/glut.h>
#include <cstdarg>

//
// Constant Variables
//
const unsigned int FONT_BUFFER_SIZE = 256;

//
// BitmapFont class
//
class BitmapFont
{
private:
	int m_listBaseA;	
	int m_listBaseU;
	int m_StrLength;
	HFONT m_FontA;
	HFONT m_FontU;
	HDC m_hDC;
public:
	BitmapFont();
	~BitmapFont();
	bool CreateA(char* fontname, int size);
	bool CreateW(wchar_t* fontname, int size);
	void ReleaseA();
	void ReleaseW();
	void DrawStringA(char* format, ...);
	void DrawStringW(wchar_t* format, ...);
};

#endif //__FONT_H__