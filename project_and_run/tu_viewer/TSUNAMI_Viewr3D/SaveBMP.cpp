// SaveBMP.cpp : �C���v�������e�[�V���� �t�@�C��
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
// CSaveBMP ���b�Z�[�W �n���h��

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

	// �t�@�C���I�[�v��
	if( ( fp = fopen(filename, "wb") )==NULL){
		return ;
	}

	//�w�b�_�\���̂̏�����
	InitHeader(&header, &info);

	//Bitmap�T�C�Y
	info.biWidth = width;
	info.biHeight = height;

	int writeWidth;
	
	///////////////////////////////////////////////////////////////////////////
	// BITMAP�p�̉����̏���
	// �f�[�^�̕��̃o�C�g����4�̔{���ł��邩���`�F�b�N
	if( width*3%4 == 0)
		writeWidth = width*3;
	else
		// �����łȂ���΁C4�̔{���ɂ��킹�����̃o�C�g�T�C�Y�ɂ���
		writeWidth = width*3 + 4 - (width*3)%4;
	
	///////////////////////////////////////////////////////////////////////////
	//�t�@�C���e��
	header.bfSize =
		writeWidth*height		//�r�b�g����
		+ 14					//BitmapHeader�\���̃T�C�Y
		+ 40;					//BitmapInfoHeader�\���̃T�C�Y

	//�w�b�_��������
	WriteHeader(&header,fp);
	WriteInfoHeader(&info,fp);

	///////////////////////////////////////////////////////////////////////////
	// �C���[�W�f�[�^��������

	// 1��f�R�o�C�g�Ƃ���ƂЂƂ̍s��3*width�o�C�g�{�A���t�@
	// �f�[�^�̕��̃o�C�g����alignmentParam�̔{���ł��邩���`�F�b�N
	glGetIntegerv( GL_PACK_ALIGNMENT,&alignmentParam);

	if( width * 3 % alignmentParam == 0)
		glByteWidth = width * 3;
	else
		// �����łȂ���΁CalignmentParam�̔{���ɂ��킹�����̃o�C�g�T�C�Y�ɂ���
		glByteWidth = width * 3 + alignmentParam - (width*3)%alignmentParam;

	printf("image:width:%d  height:%d [Byte:%d]\n",width,height, glByteWidth);

	for( y=0 ; y < height ; y++ ){
		// �f�[�^��BGR�̏��ŏ�������
		for( x=0 ; x < width ; x++ ){
			j=fwrite((pixel_data+x*3+glByteWidth*y+2),sizeof(unsigned char),1,fp);
			j=fwrite((pixel_data+x*3+glByteWidth*y+1),sizeof(unsigned char),1,fp);
			j=fwrite((pixel_data+x*3+glByteWidth*y),sizeof(unsigned char),1,fp);
		}

		// ���̃o�C�g����4�̔{���łȂ��Ƃ��͂O�Ŗ��߂�
		if( width*3%4 != 0)
			for( int j=0;j<4-(width*3)%4;j++)
				fwrite(&zero,sizeof(unsigned char),1,fp);
	}

	///////////////////////////////////////////////////////////////////////////
	//// �������J��
	//free(pixel_data);

	// �t�@�C���N���[�Y
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
	//BMP�t�@�C���̃t�@�C������ۑ�

	fwrite(&(header->bfType), sizeof(WORD),1,fp);
	fwrite(&(header->bfSize), sizeof(DWORD),1,fp);
	fwrite(&(header->bfReserved1), sizeof(WORD),1,fp);
	fwrite(&(header->bfReserved2), sizeof(WORD),1,fp);
	fwrite(&(header->bfOffBits), sizeof(DWORD),1,fp);
}

void CSaveBMP::WriteInfoHeader(BITMAPINFOHEADER *info, FILE *fp)
{
	//BMP�t�@�C���̃w�_����ۑ�

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
