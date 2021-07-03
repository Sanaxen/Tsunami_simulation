#ifndef _FAULT_DEFORMATION_H_

#define _FAULT_DEFORMATION_H_

#include <iostream>
#include <time.h>
#include <algorithm>

#include "DC3D.h"
#include "bitmap.h"
#include "CsvReadWrite.hpp"

using namespace std;

//#define Z	10
//#define L	85.0511278
#define PI		3.141592653589793
#define PI_180	0.01745329251994329

#define MU	(4.10e10)	//��Ղ̍�����[N/m^2]

//�f�w�p�����[�^
class FaultLine
{
public:
	double lo;		//�o�x
	double la;		//�ܓx
	double length;	//�f�w����
	double width;	//��
	double depth;	//�[��
	double dir;		//����(deg)
	double dip;		//�X�Ίp(deg)
	double rake;	//���ׂ�p(deg)
	double slip;	//���ׂ��(m)
	double t;		//�ŏ��̒f�w�j���0�Ƃ����Ƃ��̎��ԍ�
	double s;		//�j�󎞊�
	double a;		//�ʐ�
	double M0;		//�n�k���[�����g
	double Mw;		//���[�����g�}�O�j�`���[�h
	int horizontal_displacement;	//Tanioka and Satake (1995) �����ψʂɉ����C�X�΂���C��ʂ̐����ψʂɔ�������̉^�����l��
	
	bool operator <(const FaultLine& f) { // �召��r�p less ���Z�q
        return t < f.t;
    }
};


class Meteo
{
public:
	double lo;		//�o�x
	double la;		//�ܓx
	double D;		//�N���[�^���a
	double D0;		//�g�̗����a
	double depth;	//�[��
	double h;		//�����g����
};

//�p�x�\�� �u�x �� �b�v �� �u�x�v�ɕϊ�����
inline double to_360(double h, double m, double s)
{
	return (h+m/60.0 + s/3600.0);
}

//�p�x�\�� �u�x�v �� �u�x �� �b�v�ɕϊ�����
inline void to_60(double d, double& h, double& m, double& s)
{
	int x = (int)(d);
	h = x;

	int y = (int)((d - h)*60.0);
	m = y;

	s = (d - (h+m/60.0))*3600.0;
}

#define EPS5 0.001  /* DBL_EPSILON �� 1/5 ����x */
inline double arctanh(double x)  /* $\tanh^{-1} x$ */
{
	if (fabs(x) > EPS5)
		return 0.5 * log((1 + x) / (1 - x));
	return x * (1 + x * x / 3.0);
}


//�r�b�g�}�b�v(x,y)�̈ʒu�Ƀ}�[�N��t����
void BitmpMarke(char* fname, char* out, int x, int y, int w);

//�r�b�g�}�b�v(x,y)�̈ʒu����w��p�xth(deg)�Œ���length�̐�������
// dx,dy�͊i�q�T�C�Y
void BitmpLine(char* fname, char* out, int x, int y, int w, double dx, double dy, double length, double th);

//�s�N�Z�����W�n->���E���n���W�n
void coord_to_LatitudeLongitude( double p, double q, double& x, double& y);

//���E���n���W�n->�s�N�Z�����W�n
void coord_to_pixcel(double x, double y, double& p, double& q);

//�摜�̎l���̃s�N�Z�����W���w�肵�ĉ摜��(x,y)�ʒu�̈ܓx�o�x���v�Z����
// p:��, q:�c�i�ォ�牺�j
// p[0],q[0] ----------- p[1],q[1]
//  +                        +
//  |                        |
//  |                        |
//  |                        |
//  |                        |
// p[2],q[2] ----------- p[3],q[3]
void bitmap_to_LatitudeLongitude(int w, int h, double p[4], double q[4], int xx, int yy, double& lo, double& la);


//�摜�̎l���̃s�N�Z�����W���w�肵�Čo�x�ܓx(lo,la)�ʒu�̃r�b�g�}�b�v���W���v�Z����
// p:��, q:�c�i�ォ�牺�j
// p[0],q[0] ----------- p[1],q[1]
//  +                        +
//  |                        |
//  |                        |
//  |                        |
//  |                        |
// p[2],q[2] ----------- p[3],q[3]
void latitudeLongitude_to_Bitmap(int w, int h, double p[4], double q[4], double lo, double la, int& xx, int& yy);


//�摜�̎l���̃s�N�Z�����W���w�肵�Ēf�w�p�����[�^�̐�������
// p:��, q:�c�i�ォ�牺�j
// p[0],q[0] ----------- p[1],q[1]
//  +                        +
//  |                        |
//  |                        |
//  |                        |
//  |                        |
// p[2],q[2] ----------- p[3],q[3]
//w,h�͉摜�T�C�Y
//dx,dy�͊i�q�T�C�Y
void BitmpFaultLine(int w, int h, double p[4], double q[4], std::vector<FaultLine>& fault, double dx, double dy, char* fname, char* out);


//�摜�̎l���̃s�N�Z�����W���w�肵�Ēf�w�ψʃR���^���悭
// p:��, q:�c�i�ォ�牺�j
// p[0],q[0] ----------- p[1],q[1]
//  +                        +
//  |                        |
//  |                        |
//  |                        |
//  |                        |
// p[2],q[2] ----------- p[3],q[3]
//w,h�͉摜�T�C�Y
//dx,dy�͊i�q�T�C�Y
void CalcFault(int w, int h, double p[4], double q[4], FaultLine& fault, double dx, double dy, double* depth_map, double* zval, double& uzmin, double& uzmax);

void BitmpFaultWrite(int w, int h, double* zval, double zmin, double zmax, char* fname, char* out, char* Initial_wave_data_bmp );

double* CalcFaultListWrite(int w, int h, double p[4], double q[4], std::vector<FaultLine>& fault, Meteo* meteo, double dx, double dy, double* depth_map, char* fname, char* out, char* Initial_wave_data_bmp, int t_flag, char* drive2, char* dir2);

doublereal  distance_lambert(doublereal lat1, doublereal lon1, doublereal lat2, doublereal lon2);
doublereal  distance_lambert_amp(doublereal lat1, doublereal lon1, doublereal lat2, doublereal lon2)ACC_RESTRICTION;

//�f�w�ψʌv�Z���C��
int fault_deformation( char* parameterFile);

// Xor-Shift�ɂ�闐���W�F�l���[�^
class XorShift
{
	unsigned int seed_[4];
public:
	unsigned int next(void) ACC_RESTRICTION {
		const unsigned int t = seed_[0] ^ (seed_[0] << 11);
		seed_[0] = seed_[1];
		seed_[1] = seed_[2];
		seed_[2] = seed_[3];
		return seed_[3] = (seed_[3] ^ (seed_[3] >> 19)) ^ (t ^ (t >> 8));
	}

	doublereal next01(void) ACC_RESTRICTION {
		return (doublereal)next() / UINT_MAX;
	}
	doublereal Next01(void) ACC_RESTRICTION {
		return next01();
	}
	XorShift(const unsigned int initial_seed) ACC_RESTRICTION {
		seed(initial_seed);
	}
	XorShift(const int initial_seed) ACC_RESTRICTION {
		seed(initial_seed);
	}
	void seed(unsigned int initial_seed)ACC_RESTRICTION
	{
		unsigned int s = initial_seed;
		for (int i = 1; i <= 4; i++) {
			seed_[i - 1] = s = 1812433253U * (s ^ (s >> 30)) + i;
		}
	}
};

#endif