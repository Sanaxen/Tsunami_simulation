#if !defined(AFX_SAVEBMP_H__5C549B8C_451F_44BE_AF75_42C2534F0FC3__INCLUDED_)
#define AFX_SAVEBMP_H__5C549B8C_451F_44BE_AF75_42C2534F0FC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveBMP.h : �w�b�_�[ �t�@�C��
//
#include <stdio.h>
#  include "GL/freeglut.h"
#include "WinGDI.h"

/////////////////////////////////////////////////////////////////////////////
// CSaveBMP �E�B���h�E

class CSaveBMP 
{
//openGL�Ŏ擾�����摜�f�[�^��BMP�t�@�C���`���ŕۑ�����
//opnGL�̐F����RGB�̏�

// �R���X�g���N�V����
public:
	CSaveBMP();

// �A�g���r���[�g
public:
	int width,height;
	int glByteWidth;	// ���ۂ̉����̃o�C�g��
	unsigned char *pixel_data;

// �I�y���[�V����
public:

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B

	//{{AFX_VIRTUAL(CSaveBMP)
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	void SetSize(unsigned char *Pixeldata,int width, int height);
	void WriteInfoHeader(BITMAPINFOHEADER *info, FILE *fp);
	void WriteHeader(BITMAPFILEHEADER *header, FILE *fp);
	void InitHeader(BITMAPFILEHEADER *header, BITMAPINFOHEADER *info);
	void saveBMP(char* filename);
	virtual ~CSaveBMP();

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SAVEBMP_H__5C549B8C_451F_44BE_AF75_42C2534F0FC3__INCLUDED_)
