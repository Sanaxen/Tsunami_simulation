#define _CRT_SECURE_NO_WARNINGS

#ifdef	WIN32
#include	<windows.h>
#pragma		warning(disable:4267)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>

#if defined(WIN32)
#  include "GL/glut.h"
#elif defined(__APPLE__) || defined(MACOSX)
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/glut.h>
#endif

#include "SaveBMP.h"

/* �摜�𑜓x 2835 pixel/meter �� 72dpi */
#define XRESOLUTION 2835
#define YRESOLUTION 2835


static size_t PixelsImageSize = 0;
static GLubyte* PixelsImageBuffer = 0;

void DeletePixelsImageBuffer()
{
	if ( PixelsImageBuffer ) free(PixelsImageBuffer);
	PixelsImageBuffer = 0;
	PixelsImageSize = 0;
}


/*
** �E�B���h�E�̓��e�� BMP �`���ŕۑ�
*/
int SavePixelsImage(char* fileName, int viewport[4], int offscreen)
{
	/* BMP/DIB �t�@�C���̃w�b�_��` */
	struct biHeader {
		unsigned char bfType[2];
		unsigned char bfSize[4];
		unsigned char bfReserved1[2];
		unsigned char bfReserved2[2];
		unsigned char bfOffBits[4];
		unsigned char biSize[4];
		unsigned char biWidth[4];
		unsigned char biHeight[4];
		unsigned char biPlanes[2];
		unsigned char biBitCount[2];
		unsigned char biCompression[4];
		unsigned char biSizeImage[4];
		unsigned char biXPelsPerMeter[4];
		unsigned char biYPelsPerMeter[4];
		unsigned char biClrUsed[4];
		unsigned char biClrImportant[4];
	} header;
  
	GLubyte *image;
	size_t size;

	/* ��ʕ\���̊�����҂� */
	glFinish();

	/* �r���[�|�[�g�̃T�C�Y���̃��������m�ۂ��� */
	size = viewport[2] * viewport[3] * 3;


	int alignmentParam;
	// �f�[�^�i�[�̉����Ɏ��܂镝�̃o�C�g�̔{�����擾
	glGetIntegerv( GL_PACK_ALIGNMENT,&alignmentParam);
	
	int glByteWidth;

	// 1��f�R�o�C�g�Ƃ���ƂЂƂ̍s��3*width�o�C�g�{�A���t�@
	// �f�[�^�̕��̃o�C�g����alignmentParam�̔{���ł��邩���`�F�b�N
	if( viewport[2] * 3 % alignmentParam == 0)
		glByteWidth = viewport[2] * 3;
	else
		// �����łȂ���΁CalignmentParam�̔{���ɂ��킹�����̃o�C�g�T�C�Y�ɂ���
		glByteWidth = viewport[2] * 3 + alignmentParam - (viewport[2]*3)%alignmentParam;

	size = (glByteWidth)*(viewport[3]);
	///////////////////////////////////////////////////////////////////////////

	if ( PixelsImageBuffer == 0 )
	{
		PixelsImageBuffer = (GLubyte *)malloc(size);
		PixelsImageSize = size;
	}else
	{
		if ( PixelsImageSize < size )
		{
			free(PixelsImageBuffer);
			PixelsImageBuffer = (GLubyte *)malloc(size);
			PixelsImageSize = size;
		}
	}
	image = PixelsImageBuffer;
  
	if (image)
	{
		glReadPixels(0,0,viewport[2],viewport[3],GL_RGB,GL_UNSIGNED_BYTE,image);
		CSaveBMP mp_saveBitmap;
		mp_saveBitmap.SetSize(image,viewport[2],viewport[3]);//�t�@�C���ۑ��f�[�^��ݒ�
		mp_saveBitmap.saveBMP(fileName);//�t�@�C�����w�肵�ĕۑ�
		return 0;

		FILE *fp;

		fp = fopen(fileName, "wb");
		if (fp) 
		{
			long temp;

			glReadBuffer(GL_FRONT);
			//if ( offscreen )
			//{
			//	glGetTexImage( GL_TEXTURE_2D, 1, GL_RGB, GL_UNSIGNED_BYTE, image);
			//}else
			//{
				glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_RGB, GL_UNSIGNED_BYTE, image);
			//}
			/*
			** BMP �� Little Endian �Ȃ̂�
			** �g�p�}�V���� Endian �Ɉˑ����Ȃ��悤��
			** 1 byte ���ݒ肷��
			*/

			header.bfType[0] = 'B';
			header.bfType[1] = 'M';

			temp = size + 54;
			header.bfSize[0] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.bfSize[1] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.bfSize[2] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.bfSize[3] = (unsigned char)(temp & 0xff);

			header.bfReserved1[0] = 0;
			header.bfReserved1[1] = 0;
			header.bfReserved2[0] = 0;
			header.bfReserved2[1] = 0;

			header.bfOffBits[0] = 54;
			header.bfOffBits[1] = 0;
			header.bfOffBits[2] = 0;
			header.bfOffBits[3] = 0;

			header.biSize[0] = 40;
			header.biSize[1] = 0;
			header.biSize[2] = 0;
			header.biSize[3] = 0;

			temp = viewport[2];
			header.biWidth[0] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biWidth[1] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biWidth[2] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biWidth[3] = (unsigned char)(temp & 0xff);

			temp = viewport[3];
			header.biHeight[0] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biHeight[1] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biHeight[2] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biHeight[3] = (unsigned char)(temp & 0xff);

			header.biPlanes[0] = 1;
			header.biPlanes[1] = 0;

			header.biBitCount[0] = 24;
			header.biBitCount[1] = 0;

			header.biCompression[0] = 0;
			header.biCompression[1] = 0;
			header.biCompression[2] = 0;
			header.biCompression[3] = 0;

			temp = size;
			header.biSizeImage[0] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biSizeImage[1] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biSizeImage[2] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biSizeImage[3] = (unsigned char)(temp & 0xff);

			temp = XRESOLUTION;
			header.biXPelsPerMeter[0] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biXPelsPerMeter[1] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biXPelsPerMeter[2] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biXPelsPerMeter[3] = (unsigned char)(temp & 0xff);

			temp = YRESOLUTION;
			header.biYPelsPerMeter[0] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biYPelsPerMeter[1] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biYPelsPerMeter[2] = (unsigned char)(temp & 0xff);
			temp >>= 8;
			header.biYPelsPerMeter[3] = (unsigned char)(temp & 0xff);

			header.biClrUsed[0] = 0;
			header.biClrUsed[1] = 0;
			header.biClrUsed[2] = 0;
			header.biClrUsed[3] = 0;

			header.biClrImportant[0] = 0;
			header.biClrImportant[1] = 0;
			header.biClrImportant[2] = 0;
			header.biClrImportant[3] = 0;

			fwrite(&header, sizeof(header), 1, fp);

			// BGR�Ȃ炱��ŗǂ��B
			//fwrite(image, size, 1, fp);
			for (int y = 0; y < viewport[3]; y++) 
			{
				int x;
				for (x = 0; x < viewport[2]; x++) 
				{
					putc(image[(y*viewport[2]+x)*3+2], fp); /* blue */
					putc(image[(y*viewport[2]+x)*3+1], fp); /* green */
					putc(image[(y*viewport[2]+x)*3+0], fp); /* red */
				}
				while ((x--)%4 != 0) { putc('\0', fp); } /* padding */
			}
			fclose(fp);
		}
		return 1;
	}
  
	return 0;
}

////////////////////////////////////////////////////////////////
