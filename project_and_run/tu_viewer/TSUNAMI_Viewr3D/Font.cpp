//-----------------------------------------------------------------------
// File : Font.cpp
// Desc : Font System
// Date : Jan. 15, 2010
// Version : 1.0
// Author : Pocol
//-----------------------------------------------------------------------

//
// Includes
//
#include "Font.h"


//-----------------------------------------------------------------------
// Name : BitmapFont()
// Desc : �R���X�g���N�^.
//-----------------------------------------------------------------------
BitmapFont::BitmapFont()
{
	m_listBaseA = 0;
	m_listBaseU = 0;
	m_FontA = NULL;
	m_FontU = NULL;
}

//-----------------------------------------------------------------------
// Name : ~BitmapFont()
// Desc : �f�X�g���N�^.
//-----------------------------------------------------------------------
BitmapFont::~BitmapFont()
{
	if ( m_listBaseA != 0 )
	{
		glDeleteLists(m_listBaseA, 96);
		m_listBaseA = 0;
	}
	if ( m_listBaseU != 0 )
	{
		glDeleteLists(m_listBaseU, m_StrLength);
		m_listBaseU = 0;
		m_StrLength = 0;
	}
	if ( m_FontA )
	{
		DeleteObject( m_FontA );
		m_FontA = NULL;
	}
	if ( m_FontU )
	{
		DeleteObject( m_FontU );
		m_FontU = NULL;
	}
}

//------------------------------------------------------------------------
// Name : CreateA()
// Desc : �t�H���g�̍쐬.
//------------------------------------------------------------------------
bool BitmapFont::CreateA(char *fontname, int size)
{
	HINSTANCE hInst = NULL;
	m_FontA = CreateFontA(
		size,						//�t�H���g����.
        0,							//������.
        0,							//�e�L�X�g�̊p�x.
        0,							//�x�[�X���C���Ƃ����Ƃ̊p�x.
        FW_REGULAR,					//�t�H���g�̏d���i�����j.
        FALSE,						//�C�^���b�N��.
        FALSE,						//�A���_�[���C��.
        FALSE,						//�ł�������.
        ANSI_CHARSET,				//�����Z�b�g.
        OUT_DEFAULT_PRECIS,			//�o�͐��x.
        CLIP_DEFAULT_PRECIS,		//�N���b�s���O���x.
        ANTIALIASED_QUALITY,		//�o�͕i��.
        FIXED_PITCH | FF_MODERN,	//�s�b�`�ƃt�@�~���[.
        fontname);					//���̖�.

	if ( m_FontA == NULL )
	{
		printf("CreateFontA error.\n");
		return false;
	}

	m_hDC = wglGetCurrentDC();
	SelectObject(m_hDC, m_FontA);

	m_listBaseA = glGenLists(96);
	wglUseFontBitmapsA(m_hDC, 32, 96, m_listBaseA);
	
	return true;
}

//------------------------------------------------------------------------
// Name : CreateW()
// Desc : �t�H���g�̍쐬.
//------------------------------------------------------------------------
bool BitmapFont::CreateW(wchar_t *fontname, int size)
{
	m_FontU = CreateFontW(
		size,						//�t�H���g����.
        0,							//������.
        0,							//�e�L�X�g�̊p�x.
        0,							//�x�[�X���C���Ƃ����Ƃ̊p�x.
        FW_REGULAR,					//�t�H���g�̏d���i�����j.
        FALSE,						//�C�^���b�N��.
        FALSE,						//�A���_�[���C��.
        FALSE,						//�ł�������.
        SHIFTJIS_CHARSET,			//�����Z�b�g.
        OUT_DEFAULT_PRECIS,			//�o�͐��x.
        CLIP_DEFAULT_PRECIS,		//�N���b�s���O���x.
        ANTIALIASED_QUALITY,		//�o�͕i��.
        FIXED_PITCH | FF_MODERN,	//�s�b�`�ƃt�@�~���[.
        fontname);					//���̖�.

	if ( m_FontU == NULL )
	{
		printf("CreateFontW error.\n");
		return false;
	}
	m_hDC = wglGetCurrentDC();
	SelectObject(m_hDC, m_FontU);

	return true;
}

//------------------------------------------------------------------------
// Name : ReleaseA()
// Desc : �t�H���g�ƃf�B�X�v���C���X�g�̔j��.
//------------------------------------------------------------------------
void BitmapFont::ReleaseA()
{
	if ( m_listBaseA != 0 )
	{
		glDeleteLists(m_listBaseA, 96);
		m_listBaseA = 0;
	}
	if ( m_FontA )
	{
		DeleteObject( m_FontA );
		m_FontA = NULL;
	}
}

//------------------------------------------------------------------------
// Name : ReleaseW()
// Desc : �t�H���g�ƃf�B�X�v���C���X�g�̔j��.
//------------------------------------------------------------------------
void BitmapFont::ReleaseW()
{
	if ( m_listBaseU != 0 )
	{
		glDeleteLists(m_listBaseU, m_StrLength);
		m_listBaseU = 0;
		m_StrLength = 0;
	}
	if ( m_FontU )
	{
		DeleteObject( m_FontU );
		m_FontU = NULL;
	}
}

//------------------------------------------------------------------------
// Name : DrawStringA()
// Desc : ������̕`��.
//------------------------------------------------------------------------
void BitmapFont::DrawStringA(char *format, ...)
{
	char buf[FONT_BUFFER_SIZE];
	va_list ap;

	//�|�C���^��NULL�̏ꍇ�͏I��.
	if ( format == NULL )
		return;

	//������ϊ�.
	va_start(ap, format);
	vsprintf_s(buf, format, ap);
	va_end(ap);

	//�f�B�X�v���C���X�g�ŕ`��.
	glPushAttrib(GL_LIST_BIT);
	glListBase(m_listBaseA - 32);
	glCallLists(strlen(buf), GL_UNSIGNED_BYTE, buf);
	glPopAttrib();
}

//------------------------------------------------------------------------
// Name : DrawStringW()
// Desc : ���C�h������̕`��.
//------------------------------------------------------------------------
void BitmapFont::DrawStringW(wchar_t *format, ...)
{
	wchar_t buf[FONT_BUFFER_SIZE];
	va_list ap;
	BOOL result = FALSE;
	
	//�|�C���^��NULL�̏ꍇ�͏I��.
	if ( format == NULL )
		return;

	//������ϊ�.
	va_start(ap, format);
	vswprintf_s(buf, format, ap);
	va_end(ap);

	//�ȑO�̃f�B�X�v���C���X�g������ꍇ�͍폜.
	if ( m_listBaseU != 0 )
	{
		glDeleteLists(m_listBaseU, m_StrLength);
		m_listBaseU = 0;
		m_StrLength = 0;
	}

	//�f�B�X�v���C���X�g�쐬.
	m_StrLength = wcslen(buf);
	m_listBaseU = glGenLists(m_StrLength);
	SelectObject(m_hDC, m_FontU);

	for( int i=0; i<m_StrLength; i++ )
	{
		//�f�B�X�v���C���X�g�Ɋi�[.
		result = wglUseFontBitmapsW(m_hDC, buf[i], 1, m_listBaseU + (DWORD)i);

		//�i�[���s��.
		if ( !result )
		{
			glDeleteLists(m_listBaseU, m_StrLength);
			m_listBaseU = 0;
			m_StrLength = 0;
			return;
		}
	}

	//�f�B�X�v���C���X�g�ŕ`��.
	for( int i=0; i<m_StrLength; i++ )
	{
		glCallList(m_listBaseU + i);
	}

	//�f�B�X�v���C���X�g�j��.
	glDeleteLists(m_listBaseU, m_StrLength);
	m_listBaseU = 0;
	m_StrLength = 0;
}
