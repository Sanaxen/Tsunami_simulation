// ConvertOBJtoVRML.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

// �����̐��x
//  0 : float
//  1 : double
#define PRECISION	(1)

//--------------------------------------------------------------------------
// �w�b�_�t�@�C��
//--------------------------------------------------------------------------
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include <stdlib.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <vector>
#include <string>
#include <codecvt>

#if PRECISION
#define	DOUBLE	double
#else
#define	DOUBLE	float
#endif

const unsigned int  CP_SJIS =   932;

//--------------------------------------------------------------------------
// �\����
//--------------------------------------------------------------------------
// RGB�J���[
struct Color_rgb {
	DOUBLE	r;
	DOUBLE	g;
	DOUBLE	b;
} ;

#include "Vector3D.h"
#include "Vertex3D.h"
#include "Face.h"
#include "Material.h"

using namespace std;

//--------------------------------------------------------------------------
// �O���[�o���ϐ�
//--------------------------------------------------------------------------
const string g_logfile = "convert.log";			// ���O�t�@�C����
ofstream g_ofs;									// ���O�t�@�C���X�g���[��
clock_t g_Start;								// �����J�n����
const string g_newmtlname = "cnvmaterial_";		// �V�K�쐬����ގ���
const DOUBLE tol_color = 1.0 / 255.0;

//--------------------------------------------------------------------------
// �����֐�
//--------------------------------------------------------------------------
// Usage
void Usage()
{
	cout << "ConvFormat.exe [-nrev] [-tr ����] [-vcol|-fcol] [-vnrm|-fnrm|-nonrm] infile outfile" << endl;
	cout << "	infile  : ���͌`��t�@�C���t���p�X" << endl;
	cout << "				���Ή��t�H�[�}�b�g�F*.obj,*.wrl" << endl;
	cout << "	outfile : �o�͌`��t�@�C���t���p�X" << endl;
	cout << "				���Ή��t�H�[�}�b�g�F*.obj,*.wrl" << endl;
	cout << "	<�I�v�V����>" << endl;
	cout << "		-nrev		: �@���x�N�g���𔽓]����" << endl;
	cout << "		-tr ����	: �����x(0-1)��ݒ肷��" << endl;
	cout << "					:   0 = �s����" << endl;
	cout << "					:   1 = ����" << endl;
	cout << "		-vcol		: ���_�J���[�ŏo�͂���" << endl;
	cout << "		-fcol		: �ʃJ���[�ŏo�͂���" << endl;
	cout << "		-vnrm		: ���_�@���x�N�g���ŏo�͂���" << endl;
	cout << "		-fnrm		: �ʖ@���x�N�g���ŏo�͂���" << endl;
	cout << "		-nonrm		: �@���x�N�g�����o�͂��Ȃ�" << endl;
	//system("pause");
	return;
}

////�x�N�g������
//inline double DotProduct(CVector3D v1, CVector3D v2) {
//	return v1.m_x * v2.m_x + v1.m_y * v2.m_y + v1.m_z * v2.m_z;
//}

// �擪�����̋�(�X�y�[�X�A�^�u�A���s)���폜
inline void TrimString(string &buf)
{
    string::size_type pos;
    while((pos = buf.find_first_of(" �@\t")) == 0){
        buf.erase(buf.begin());
        if(buf.empty()) break;
    }
    while((pos = buf.find_last_of(" �@\t\r\n")) == string::npos-1){
        buf.erase(buf.end());
        if(buf.empty()) break;
    }
}

// ���������؂蕶���ŕ���
inline vector<string> split(string str, string delim) {
	vector<string> items;
	string::size_type dlm_idx;
	if(string::npos == (dlm_idx = str.find_first_of(delim))) {
		items.push_back(str.substr(0, dlm_idx));
	}
	while(string::npos != (dlm_idx = str.find_first_of(delim))) {
		if(string::npos == str.find_first_not_of(delim)) {
			break;
		}
		items.push_back(str.substr(0, dlm_idx));
		dlm_idx++;
		str = str.erase(0, dlm_idx);
		if(string::npos == str.find_first_of(delim) && "" != str) {
			items.push_back(str);
			break;
		}
	}
	return items;
}

// �����񂪎����l��\���Ă��邩�𒲂ׂ�
inline bool IsNumeric(const string &str)
{
    if(str.find_first_not_of("-0123456789. Ee\t") != string::npos) {
        return false;
    }
 
    return true;
}

// ���C�h������֕ϊ��B
inline wstring ToWideString( const string& String, unsigned int CodePage )
{
    int Length( ::MultiByteToWideChar( CodePage, MB_PRECOMPOSED, &String[ 0 ], -1, NULL, 0 ) );

    vector<wchar_t>  Result( Length + 1, 0 );

    ::MultiByteToWideChar( CodePage, MB_PRECOMPOSED, &String[ 0 ], -1, &Result[ 0 ], Length );

    return &Result[ 0 ];
}

// �_A���_B�̃x�N�g�����쐬
inline CVector3D CreVector( const CVertex3D& A, const CVertex3D& B ) {
	CVector3D vec;
	vec.m_x = B.m_x - A.m_x;
	vec.m_y = B.m_y - A.m_y;
	vec.m_z = B.m_z - A.m_z;
	return vec;
}

/*
 *	���_ABC�ō��ꂽ�ʂ���@�����v�Z����B
 */
CVector3D CreatePolygonNormal( const CVertex3D& A, const CVertex3D& B, const CVertex3D& C ) {

	CVector3D AB,AC;

	AB = CreVector( A, B );
	AC = CreVector( A, C );

	CVector3D normal = AB * AC;	//AB AC�̊O��
	normal.Normalize();//�P�ʃx�N�g���ɂ���

	return normal;
}

/*
 *	���_�@���x�N�g�����v�Z
 */
bool CalcNormalOfVtx(
		vector<CVertex3D>&			ioVtxList,	// ���_���X�g
		const vector<CTriangle>&	iFaceList	// �ʃ��X�g
)
{
	size_t ic, jc;

	// ���_�����[�v
	size_t size1 = ioVtxList.size();
	bool bRet;
	CVector3D normal;
	size_t fidx;
	for ( ic = 0; ic < size1; ++ic ) {
		// ������
		ioVtxList[ic].m_nrm.m_x = 0.0; ioVtxList[ic].m_nrm.m_y = 0.0; ioVtxList[ic].m_nrm.m_z = 0.0;

		// �����ʐ����[�v
		size_t size2 = ioVtxList[ic].m_FidxList.size();
		for ( jc = 0; jc < size2; ++jc ) {
			// �ʃC���f�b�N�X
			fidx = ioVtxList[ic].m_FidxList[jc];

			// �ʖ@���x�N�g�������Z
//			ioVtxList[ic].m_nrm += normal;
			ioVtxList[ic].m_nrm = ioVtxList[ic].m_nrm + iFaceList[fidx].m_nrm;
		}

		if ( size2 == 0) ioVtxList[ic].m_nrm = normal;

		// ���_�@���x�N�g����P�ʉ�
		bRet = ioVtxList[ic].m_nrm.Normalize();
		if (bRet != true) {
			g_ofs << "[Warning] ���_�@���x�N�g�����O�x�N�g�� : " << ic << " : " 
				  << ioVtxList[ic].m_nrm.m_x << "," << ioVtxList[ic].m_nrm.m_y << "," << ioVtxList[ic].m_nrm.m_z << " :";
			for ( jc = 0; jc < size2; ++jc ) {
				g_ofs << " " << ioVtxList[ic].m_FidxList[jc];
			}
			g_ofs << endl;
			continue;
			// �b��Ƃ��āA�擪�̏����ʂ̖@���x�N�g�����Z�b�g
			fidx = ioVtxList[ic].m_FidxList[0];
			ioVtxList[ic].m_nrm = iFaceList[fidx].m_nrm;
			ioVtxList[ic].m_nrm.Normalize();
		}
	}

	return true;
}

/*
 *	MTL�t�@�C���ǂݍ���
 */
bool ReadMTLFile(
		string&				iMTLFile,	// �ǂݍ���MTL�t�@�C����
		vector<CMaterial>&	ioMtlList	// �}�e���A�����X�g
)
{
	// ���̓t�@�C���I�[�v��
	ifstream ifs(iMTLFile);
	if(!ifs || !ifs.is_open() || ifs.bad() || ifs.fail()){
		g_ofs << "[ERROR] MTL�t�@�C���I�[�v���G���[" << endl;
		return false;
	}

	string buf;
	string::size_type comment_start = 0;
	TCHAR mtlname[256];
	memset(mtlname, 0x00, sizeof(mtlname));

	while(!ifs.eof()){
		TCHAR dmy[256];
		DOUBLE r,g,b,dval;

		getline(ifs, buf);

		// '#'�ȍ~�̓R�����g�Ƃ��Ė���
		if( (comment_start = buf.find('#')) != string::size_type(-1) )
			buf = buf.substr(0, comment_start);

		// �s���̃X�y�[�X�C�^�u���폜
		TrimString(buf);

		// ��s�͖���
		if(buf.empty())
			continue;

		// KEY���擾
		TCHAR key[256];
		_stscanf(buf.c_str(), "%s", key);

		// �}�e���A����
		if ( _tcsicmp(key, _T("newmtl")) == 0 ) {
			_stscanf(buf.c_str(), "%s %s", dmy, mtlname);

			// �}�e���A�����i�[
			CMaterial mat(mtlname);
			ioMtlList.push_back(mat);
		}
		// �A���r�G���g�J���[
		else if ( _tcsicmp(key, _T("Ka")) == 0 ){
			r = -1.0; g = -1.0; b = -1.0;
#if PRECISION
			_stscanf(buf.c_str(), "%s %lf %lf %lf", dmy, &r, &g, &b);
#else
			_stscanf(buf.c_str(), "%s %f %f %f", dmy, &r, &g, &b);
#endif
			if ( !ioMtlList.empty() ) {
				ioMtlList.back().m_Ka.r = r;
				ioMtlList.back().m_Ka.g = g;
				ioMtlList.back().m_Ka.b = b;
				ioMtlList.back().m_setKa = true;
			}
		}
		// �f�B�t���[�Y�J���[�i���̂̐F�j
		else if ( _tcsicmp(key, _T("Kd")) == 0 ){
			r = -1.0; g = -1.0; b = -1.0;
#if PRECISION
			_stscanf(buf.c_str(), "%s %lf %lf %lf", dmy, &r, &g, &b);
#else
			_stscanf(buf.c_str(), "%s %f %f %f", dmy, &r, &g, &b);
#endif
			if ( !ioMtlList.empty() ) {
				ioMtlList.back().m_Kd.r = r;
				ioMtlList.back().m_Kd.g = g;
				ioMtlList.back().m_Kd.b = b;
				ioMtlList.back().m_setKd = true;
			}
		}
		// �X�y�L�����[�J���[�i�n�C���C�g�̐F�j
		else if ( _tcsicmp(key, _T("Ks")) == 0 ){
			r = -1.0; g = -1.0; b = -1.0;
#if PRECISION
			_stscanf(buf.c_str(), "%s %lf %lf %lf", dmy, &r, &g, &b);
#else
			_stscanf(buf.c_str(), "%s %f %f %f", dmy, &r, &g, &b);
#endif
			if ( !ioMtlList.empty() ) {
				ioMtlList.back().m_Ks.r = r;
				ioMtlList.back().m_Ks.g = g;
				ioMtlList.back().m_Ks.b = b;
				ioMtlList.back().m_setKs = true;
			}
		}
		// ���˂̋���
		else if ( _tcsicmp(key, _T("Ns")) == 0 ){
			dval = -1.0;
#if PRECISION
			_stscanf(buf.c_str(), "%s %lf", dmy, &dval);
#else
			_stscanf(buf.c_str(), "%s %f", dmy, &dval);
#endif
			if ( !ioMtlList.empty() ) {
				ioMtlList.back().m_Ns = dval;
				ioMtlList.back().m_setNs = true;
			}
		}
		// �A���t�@�l�i�����x�j
		else if ( _tcsicmp(key, _T("Tr")) == 0 ||
				  _tcsicmp(key, _T("d") ) == 0){
			dval = -1.0;
#if PRECISION
			_stscanf(buf.c_str(), "%s %lf", dmy, &dval);
#else
			_stscanf(buf.c_str(), "%s %f", dmy, &dval);
#endif
			if ( !ioMtlList.empty() ) {
				ioMtlList.back().m_Tr = dval;
				ioMtlList.back().m_setTr = true;
			}
		}
	}
	ifs.close();

	g_ofs << "�}�e���A���t�@�C���ǂݍ��ݏI��[sec]  : " << (clock() - g_Start)/CLOCKS_PER_SEC << endl;
	g_ofs << "\t�}�e���A����: " << ioMtlList.size() << endl;

	return true;
}

/*
 *	OBJ�t�@�C���ǂݍ���
 *	���@���x�N�g�����A�e�N�X�`�����͖�������
 */
bool ReadOBJFile(
		string&				iObjDir,	// �ǂݍ���OBJ�t�@�C���f�B���N�g���p�X 
		string&				iObjFile,	// �ǂݍ���OBJ�t�@�C����
		int&				iNormalF,	// �@���x�N�g���o�̓t���O ��1:���_�@�� 2: �ʖ@�� -1:�o�͂��Ȃ�
		vector<CVertex3D>&	oVtxList,	// ���_���X�g
		vector<CTriangle>&	oFaceList,	// �ʃ��X�g
		vector<CMaterial>&	ioMtlList,	// �}�e���A�����X�g
		vector<CVector3D>&	oNrmList	// �@���x�N�g�����X�g
)
{
	size_t ic;

	// ������
	oVtxList.clear();
	oFaceList.clear();
	oNrmList.clear();

	// ���̓t�@�C���I�[�v��
	ifstream ifs(iObjDir+iObjFile);
	if(!ifs || !ifs.is_open() || ifs.bad() || ifs.fail()){
		g_ofs << "[ERROR] OBJ�t�@�C���I�[�v���G���[" << endl;
		return false;
	}

	string buf;
	string::size_type comment_start = 0;
	TCHAR mtlfile[256];
	TCHAR mtlname[256];
	memset(mtlfile, 0x00, sizeof(mtlfile));
	memset(mtlname, 0x00, sizeof(mtlname));
	long v1,v2,v3,n1,n2,n3;
	DOUBLE x,y,z,r,g,b;
#if PRECISION
	DOUBLE minv = -DBL_MAX;
#else
	DOUBLE minv = -FLT_MAX;
#endif

	while(!ifs.eof()){
		TCHAR dmy[256];

		getline(ifs, buf);

		// '#'�ȍ~�̓R�����g�Ƃ��Ė���
		if( (comment_start = buf.find('#')) != string::size_type(-1) )
			buf = buf.substr(0, comment_start);

		// �s���̃X�y�[�X�C�^�u���폜
		TrimString(buf);

		// ��s�͖���
		if(buf.empty())
			continue;

		// KEY���擾
		TCHAR key[256];
		_stscanf(buf.c_str(), "%s", key);

		// ���_���W
		if ( _tcsicmp(key, _T("v")) == 0 ) {
			x = minv; y = minv; z = minv;
			r = -1.0; g = -1.0; b = -1.0;
#if PRECISION
			_stscanf(buf.c_str(), "v %lf %lf %lf %lf %lf %lf",  &x, &y, &z, &r, &g, &b);
#else
			_stscanf(buf.c_str(), "v %f %f %f %f %f %f",  &x, &y, &z, &r, &g, &b);
#endif
			if ( x > minv && y > minv && z > minv ) {
				CVertex3D vtx(oVtxList.size(), x, y, z);
				if ( r >= 0 && g >= 0 && b >= 0 ) {
					vtx.SetColor(r, g, b);
				}
				oVtxList.push_back(vtx);
			}
		}
		// �@���x�N�g��
		else if ( _tcsicmp(key, _T("vn")) == 0 ) {
			x = minv; y = minv; z = minv;
#if PRECISION
			_stscanf(buf.c_str(), "vn %lf %lf %lf",  &x, &y, &z);
#else
			_stscanf(buf.c_str(), "vn %f %f %f",  &x, &y, &z);
#endif
			if ( iNormalF > 0 && x > minv && y > minv && z > minv ) {
				CVector3D vec(x, y, z);
				oNrmList.push_back(vec);
			}
		}
		// ��
		else if ( _tcsicmp(key, _T("f")) == 0 ){
			v1 = -1; v2 = -1; v3 = -1;
			n1 = -1; n2 = -1; n3 = -1;
			if ( _tcsstr(buf.c_str(), _T("//")) == NULL ) {
				_stscanf(buf.c_str(), "f %ld %ld %ld",  &v1, &v2, &v3);
			} else {
				_stscanf(buf.c_str(), "f %ld//%ld %ld//%ld %ld//%ld",  &v1, &n1, &v2, &n2, &v3, &n3);
			}
			if ( v1 > 0 && v2 > 0 && v3 > 0 ) {
				if ( n1 > 0 && n2 > 0 && n3 > 0 ) {
					CTriangle tri(oFaceList.size(), v1-1, v2-1, v3-1, n1-1, n2-1, n3-1, mtlname);
					oFaceList.push_back(tri);
				} else {
					CTriangle tri(oFaceList.size(), v1-1, v2-1, v3-1, mtlname);
					oFaceList.push_back(tri);
				}
			}
		}
		// �}�e���A���t�@�C����
		else if ( _tcsicmp(key, _T("mtllib")) == 0 ){
			_stscanf(buf.c_str(), "%s %s", dmy, mtlfile);
		}
		// �g�p�}�e���A����
		else if ( _tcsicmp(key, _T("usemtl")) == 0 ){
			_stscanf(buf.c_str(), "%s %s", dmy, mtlname);
		}
	}
	ifs.close();

	// �}�e���A���t�@�C���̓ǂݍ���
	if ( _tcslen(mtlfile) > 0 ) {
		string matfile(iObjDir+mtlfile);
		if ( ReadMTLFile(matfile, ioMtlList) != true ) {
			g_ofs << "[ERROR] �}�e���A���t�@�C���̓ǂݍ��݃G���[" << endl; 
			return false;
		}
	}

	// ���̓t�@�C���ɖ@���x�N�g���f�[�^������ꍇ
	if ( oNrmList.size() > 0 ) {
		for ( ic = 0; ic < oFaceList.size(); ++ic ) {
			// �ʂ̒��_�C���f�b�N�X���擾
			v1 = oFaceList[ic].m_vidx[0];
			v2 = oFaceList[ic].m_vidx[1];
			v3 = oFaceList[ic].m_vidx[2];
			// �ʂ̒��_�̖@���x�N�g���C���f�b�N�X���擾
			n1 = oFaceList[ic].m_nidx[0];
			n2 = oFaceList[ic].m_nidx[1];
			n3 = oFaceList[ic].m_nidx[2];

			// ���_�̖@���x�N�g���ɉ��Z
			oVtxList[v1].m_nrm = oVtxList[v1].m_nrm + oNrmList[n1];
			oVtxList[v2].m_nrm = oVtxList[v2].m_nrm + oNrmList[n2];
			oVtxList[v3].m_nrm = oVtxList[v3].m_nrm + oNrmList[n3];

			// �ʂ̖@���x�N�g�����i�[
			if ( n1 == n2 == n3 ) {
				oFaceList[ic].m_nrm = oNrmList[n1];
			} else {
				oFaceList[ic].m_nrm = oNrmList[n1] + oNrmList[n2] + oNrmList[n3];
			}
			oFaceList[ic].m_nrm.Normalize();
		}

		// ���_�̖@���x�N�g���̒P�ʉ�
		for ( ic = 0; ic < oVtxList.size(); ++ic ) {
			oVtxList[ic].m_nrm.Normalize();
		}
	}

	if(oVtxList.empty() || oFaceList.empty()) return false;

	return true;
}

/*
 *	VRML�t�@�C���ǂݍ���
 *	��Transform,children�ɖ��Ή�
 */
bool ReadVRMLFile(
		string&				iVRMLFile,	// �ǂݍ���OBJ�t�@�C���p�X 
		int&				iNormalF,	// �@���x�N�g���o�̓t���O ��1:���_�@�� 2: �ʖ@�� -1:�o�͂��Ȃ�
		vector<CVertex3D>&	oVtxList,	// ���_���X�g
		vector<CTriangle>&	oFaceList,	// �ʃ��X�g
		vector<CMaterial>&	ioMtlList,	// �}�e���A�����X�g
		vector<CVector3D>&	oNrmList	// �@���x�N�g�����X�g
)
{
	size_t ic;

	// ������
	oVtxList.clear();
	oFaceList.clear();
	oNrmList.clear();

	// ���̓t�@�C���I�[�v��
	ifstream ifs(iVRMLFile);
	if(!ifs || !ifs.is_open() || ifs.bad() || ifs.fail()){
		g_ofs << "[ERROR] VRML�t�@�C���I�[�v���G���[" << endl;
		return false;
	}

	string buf;
	string::size_type comment_start = 0;
	bool shapeNode = false;
	int shape_field = 0;		// Shape�t�B�[���h	1:IndexedFaceSet 2:Appearance
	int geom_field = 0;			// IndexedFaceSet�t�B�[���h	1:Coordinate 2:Normal 3:Color 4:coordIndex
	bool nrmvF = true;
	bool colvF = true;
	bool updateVtxF = false;
	bool updateFaceF = false;
	size_t vcnt = 0;
	size_t fcnt = 0;
#if PRECISION
	DOUBLE minv = -DBL_MAX;
#else
	DOUBLE minv = -FLT_MAX;
#endif

	DOUBLE x,y,z,r,g,b;
	long v1,v2,v3;

	while(!ifs.eof()){
		TCHAR dmy[256];
		DOUBLE val;

		getline(ifs, buf);

		// '#'�ȍ~�̓R�����g�Ƃ��Ė���
		if( (comment_start = buf.find('#')) != string::size_type(-1) )
			buf = buf.substr(0, comment_start);

		// �s���̃X�y�[�X�C�^�u���폜
		TrimString(buf);

		// ��s�͖���
		if(buf.empty()) continue;

		// �������ɕϊ�
		transform( buf.begin(), buf.end(), buf.begin(), ::tolower );

		// �L�[���[�h����
		if ( _tcsstr(buf.c_str(), _T("shape")) != NULL ) {
			shapeNode = true;
		}
		else if ( shapeNode == true && _tcsstr(buf.c_str(), _T("indexedfaceset")) != NULL ) {
			shape_field = 1;
		}
		else if ( shape_field == 1 && _tcsstr(buf.c_str(), _T("normalpervertex")) != NULL ) {
			if ( _tcsstr( buf.c_str(), _T("false")) != NULL ) {
				nrmvF = false;
			}
		}
		else if ( shape_field == 1 && _tcsstr(buf.c_str(), _T("colorpervertex")) != NULL ) {
			if ( _tcsstr(buf.c_str(), _T("false")) != NULL ) {
				colvF = false;
			}
		}
		else if ( shape_field == 1 && _tcsstr(buf.c_str(), _T("coordinate")) != NULL ) {
			geom_field = 1;
			if ( oVtxList.size() > 0 ) {
				updateVtxF = true;
			}
		}
		else if ( shape_field == 1 && _tcsstr(buf.c_str(), _T("normal")) != NULL ) {
			geom_field = 2;
		}
		else if ( shape_field == 1 && _tcsstr(buf.c_str(), _T("color")) != NULL ) {
			geom_field = 3;
			if ( oVtxList.size() > 0 ) {
				updateVtxF = true;
			}
			if ( oFaceList.size() > 0 ) {
				updateFaceF = true;
			}
		}
		else if ( shape_field == 1 && _tcsstr(buf.c_str(), _T("coordindex")) != NULL ) {
			geom_field = 4;
			if ( oFaceList.size() > 0 ) {
				updateFaceF = true;
			}
		}
		else if ( shapeNode == true && _tcsstr(buf.c_str(), _T("appearance")) != NULL ) {
			shape_field = 2;
			geom_field = 0;
		}
		else if ( shape_field == 2 && _tcsstr(buf.c_str(), _T("transparency")) != NULL ) {
#if PRECISION
			_stscanf(buf.c_str(), "%s %lf", dmy, &val);
#else
			_stscanf(buf.c_str(), "%s %f", dmy, &val);
#endif
			// �擪�̃}�e���A���Ɋi�[
			ioMtlList[0].m_Tr = val;
			ioMtlList[0].m_setTr = true;
		}
		else if ( shape_field == 2 && _tcsstr(buf.c_str(), _T("shininess")) != NULL ) {
#if PRECISION
			_stscanf(buf.c_str(), "%s %lf", dmy, &val);
#else
			_stscanf(buf.c_str(), "%s %f", dmy, &val);
#endif
			// �擪�̃}�e���A���Ɋi�[
			ioMtlList[0].m_Ns = val;
			ioMtlList[0].m_setNs = true;
		}
		else {

			// �J���}�ŕ���
			string delim = ",";
			vector<string> strlist = split( buf, delim );

			/*
			 *	�_���W�l���擾
			 */
			if ( geom_field == 1 ) {
				for ( ic = 0; ic < strlist.size(); ++ic ) {
					x = minv; y = minv; z = minv;
#if PRECISION
					_stscanf(strlist[ic].c_str(), "%lf %lf %lf", &x, &y, &z);
#else
					_stscanf(strlist[ic].c_str(), "%f %f %f", &x, &y, &z);
#endif
					if ( x > minv && y > minv && z > minv ) {
						// �_���X�g���i�[�̏ꍇ
						if ( updateVtxF == false ) {
							CVertex3D vtx(oVtxList.size(), x, y, z);
							oVtxList.push_back(vtx);
						}
						else {
							if ( vcnt >= oVtxList.size() ) {
								g_ofs << "[ERROR] �_�X�V�G���[" << endl;
								return false;
							}
							oVtxList[vcnt].m_x = x;
							oVtxList[vcnt].m_y = y;
							oVtxList[vcnt].m_z = z;
							++vcnt;
						}
					}
				}
			}

			/*
			 *	�@���x�N�g�����擾
			 */
			if ( geom_field == 2 && iNormalF > 0 ) {
				for ( ic = 0; ic < strlist.size(); ++ic ) {
					x = minv; y = minv; z = minv;
#if PRECISION
					_stscanf(strlist[ic].c_str(), "%lf %lf %lf", &x, &y, &z);
#else
					_stscanf(strlist[ic].c_str(), "%f %f %f", &x, &y, &z);
#endif
					if ( x > minv && y > minv && z > minv ) {
						CVector3D vec(x, y, z);
						oNrmList.push_back(vec);
					}
				}
			}

			/*
			 *	�F���擾
			 */
			if ( geom_field == 3 ) {
				for ( ic = 0; ic < strlist.size(); ++ic ) {
					r = -1; g = -1; b = -1;
#if PRECISION
					_stscanf(strlist[ic].c_str(), "%lf %lf %lf", &r, &g, &b);
#else
					_stscanf(strlist[ic].c_str(), "%f %f %f", &r, &g, &b);
#endif
					if ( r >= 0 && g >= 0 && b >= 0 ) {
						if ( colvF == true ) {
							// �_���X�g���i�[�̏ꍇ
							if ( updateVtxF == false ) {
								CVertex3D vtx(oVtxList.size(), minv, minv, minv, r, g, b);
								oVtxList.push_back(vtx);
							}
							else {
								if ( vcnt >= oVtxList.size() ) {
									g_ofs << "[ERROR] �F�X�V�G���[" << endl;
									return false;
								}
								oVtxList[vcnt].m_color.r = r;
								oVtxList[vcnt].m_color.g = g;
								oVtxList[vcnt].m_color.b = b;
								oVtxList[vcnt].m_setrgb = true;
								++vcnt;
							}
						}
						else {
							// �ʃ��X�g���i�[�̏ꍇ
							if ( updateFaceF == false ) {
								CTriangle tri(oFaceList.size(), -1, -1, -1, r, g, b);
								oFaceList.push_back(tri);
							}
							else {
								if ( fcnt >= oFaceList.size() ) {
									g_ofs << "[ERROR] �F�X�V�G���[" << endl;
									return false;
								}
								oFaceList[fcnt].m_color.r = r;
								oFaceList[fcnt].m_color.g = g;
								oFaceList[fcnt].m_color.b = b;
								oFaceList[fcnt].m_setrgb = true;
								++fcnt;
							}
						}
					}
				}
			}

			/*
			 *	�ʂ��擾
			 */
			if ( geom_field == 4 ) {
				v1 = -1; v2 = -1; v3 = -1;
				for ( ic = 0; ic < strlist.size(); ++ic ) {
					long vidx = -999;
					_stscanf(strlist[ic].c_str(), "%ld", &vidx);
					// �\���_�C���f�b�N�X�̏ꍇ
					if ( vidx >= 0 ) {
						if ( v1 < 0 )
							v1 = vidx;
						else if ( v2 < 0 )
							v2 = vidx;
						else if ( v3 < 0 ) 
							v3 = vidx;
						else {
							g_ofs << "[ERROR] �\���_���G���[" << endl;
							return false;
						}
					}
					// ��؂蕶���̏ꍇ
					else if ( vidx == -1 ) {
						// �ʃ��X�g���X�V

						// �ʃ��X�g���i�[�̏ꍇ
						if ( updateFaceF == false ) {
							CTriangle tri(oFaceList.size(), v1, v2, v3 );
							oFaceList.push_back(tri);
						}
						else {
							if ( fcnt >= oFaceList.size() ) {
								g_ofs << "[ERROR] �\���_�X�V�G���[" << endl;
								return false;
							}
							oFaceList[fcnt].m_vidx[0] = v1;
							oFaceList[fcnt].m_vidx[1] = v2;
							oFaceList[fcnt].m_vidx[2] = v3;
							++fcnt;
						}
						v1 = -1; v2 = -1; v3 = -1;
					}
				}
			}
		}
	}
	ifs.close();

	/*
	 *	�ʂ̖@���x�N�g������ݒ�
	 */
	// �@���x�N�g����񂪂������ꍇ
	if ( oNrmList.size() > 0 ) {
		// ���_�@���x�N�g���̏ꍇ
		if ( nrmvF ) {
			// ���_�@���x�N�g�����i�[
			for ( ic = 0; ic < oVtxList.size(); ++ic ) {
				oVtxList[ic].m_nrm = oNrmList[ic];
				oVtxList[ic].m_nrm.Normalize();
			}
			// �ʂ̒��_�@���x�N�g�����i�[
			for ( ic = 0; ic < oFaceList.size(); ++ic ) {
				// ���_�C���f�b�N�X���擾
				v1 = oFaceList[ic].m_vidx[0];
				v2 = oFaceList[ic].m_vidx[1];
				v3 = oFaceList[ic].m_vidx[2];

				// �ʂ̒��_�@�������i�[
				oFaceList[ic].m_nrm = oVtxList[v1].m_nrm + oVtxList[v2].m_nrm + oVtxList[v3].m_nrm;
				oFaceList[ic].m_nrm.Normalize();
			}
		}
		// �ʖ@���x�N�g���̏ꍇ
		else {
			// �ʖ@���x�N�g�����i�[
			for ( ic = 0; ic < oFaceList.size(); ++ic ) {
				// ���_�C���f�b�N�X���擾
				v1 = oFaceList[ic].m_vidx[0];
				v2 = oFaceList[ic].m_vidx[1];
				v3 = oFaceList[ic].m_vidx[2];

				// �ʂ̒��_�@�������i�[
				oFaceList[ic].m_nrm = oNrmList[ic];
				oFaceList[ic].m_nrm.Normalize();

				// ���_�@���x�N�g���ɉ��Z
				oVtxList[v1].m_nrm = oVtxList[v1].m_nrm + oNrmList[ic];
				oVtxList[v2].m_nrm = oVtxList[v2].m_nrm + oNrmList[ic];
				oVtxList[v3].m_nrm = oVtxList[v3].m_nrm + oNrmList[ic];
			}
			// ���_�@���x�N�g����P�ʉ�
			for ( ic = 0; ic < oVtxList.size(); ++ic ) {
				oVtxList[ic].m_nrm.Normalize();
			}
		}
	}

	if(oVtxList.empty() || oFaceList.empty()) return false;

	return true;
}

/*
 *	�����l���X�V����
 */
bool UpdateAttribute(
		vector<CVertex3D>&	ioVtxList,	// ���_���X�g
		vector<CTriangle>&	ioFaceList,	// �ʃ��X�g
		vector<CMaterial>&	ioMtlList,	// �}�e���A�����X�g
		vector<CVector3D>&	iNrmList,	// �@���x�N�g�����X�g
		bool&				iNrmRevF,	// �@�����]�t���O
		DOUBLE&				iTr,		// �����x
		int&				iColorF,	// �ΏېF ��1:���_ 2:��
		int&				iNormalF,	// �@���x�N�g���o�̓t���O ��1:���_�@�� 2: �ʖ@�� -1:�o�͂��Ȃ�
		int&				iOutFtype	// �o�̓t�@�C���^�C�v
										//	1: OBJ�`��
										//	2: VRML�`��
)
{
	size_t ic, jc;
	long v1,v2,v3;

	if ( ioVtxList.empty() || ioFaceList.empty() ) return true;
	if ( iColorF != 1 && iColorF != 2 ) return false;
	if ( iNormalF != 1 && iNormalF != 2 && iNormalF != -1 ) return false;

	/*
	 *	���_�̏����ʃ��X�g���i�[
	 */
	for ( ic = 0; ic < ioFaceList.size(); ++ic ) {
		// �ʂ̒��_�C���f�b�N�X���擾
		v1 = ioFaceList[ic].m_vidx[0];
		v2 = ioFaceList[ic].m_vidx[1];
		v3 = ioFaceList[ic].m_vidx[2];

		// ���_���|�����ʂɖʃC���f�b�N�X���i�[
		ioVtxList[v1].m_FidxList.push_back(ic);
		ioVtxList[v2].m_FidxList.push_back(ic);
		ioVtxList[v3].m_FidxList.push_back(ic);
	}

	// ���̓t�@�C���ɖ@���x�N�g���f�[�^���Ȃ��ꍇ
	if ( iNormalF > 0 && iNrmList.empty() ) {
		// �ʖ@���x�N�g�����v�Z
		for ( ic = 0; ic < ioFaceList.size(); ++ic ) {
			// �ʂ̒��_�C���f�b�N�X���擾
			v1 = ioFaceList[ic].m_vidx[0];
			v2 = ioFaceList[ic].m_vidx[1];
			v3 = ioFaceList[ic].m_vidx[2];

			// �ʖ@���x�N�g�����v�Z
			CVector3D nrm = CreatePolygonNormal( ioVtxList[v1], ioVtxList[v2], ioVtxList[v3] );

			// �ʖ@���x�N�g�����i�[
			ioFaceList[ic].m_nrm = nrm;
		}

		// ���_�@���x�N�g�����v�Z
		if ( CalcNormalOfVtx(ioVtxList,ioFaceList) != true ) {
			g_ofs << "[ERROR] ���_�@���x�N�g���̌v�Z�G���[" << endl; 
			return false;
		}
	}

	/*
	 *	�ʂ̐F��ݒ�
	 */
	for ( ic = 0; ic < ioFaceList.size(); ++ic ) {
		// �ʂ̐F���ݒ�ς݂̏ꍇ�X�L�b�v
		if ( ioFaceList[ic].m_setrgb == true ) continue;

		// �g�p�}�e���A�������w�肳��Ă���ꍇ
		if ( ioFaceList[ic].m_mtlname.length() > 0 ) {
			// �g�p�}�e���A����������
			bool findF = false;
			for ( jc = 0; jc < ioMtlList.size(); ++jc ) {
				if ( _tcsicmp(ioMtlList[jc].m_mtlname.c_str(), ioFaceList[ic].m_mtlname.c_str()) == 0 ) {
					ioFaceList[ic].m_color = ioMtlList[jc].m_Kd;
					findF = true;
					break;
				}
			}
			// ������Ȃ������ꍇ
			if ( !findF ) {
				g_ofs << "[ERROR] �g�p�}�e���A�������݂��Ȃ� : " << ioFaceList[ic].m_mtlname << endl; 
				return false;
			}
		}
		// �g�p�}�e���A�������w�肳��Ă��Ȃ��ꍇ
		else {
			// �\���_�C���f�b�N�X
			long v1,v2,v3;
			v1 = ioFaceList[ic].m_vidx[0];
			v2 = ioFaceList[ic].m_vidx[1];
			v3 = ioFaceList[ic].m_vidx[2];

			// �\���_�̐F����ʂ̐F���v�Z�i�\���_�̐F�̕��ρj
			ioFaceList[ic].m_color.r = (ioVtxList[v1].m_color.r + ioVtxList[v2].m_color.r + ioVtxList[v3].m_color.r) / 3.0;
			ioFaceList[ic].m_color.g = (ioVtxList[v1].m_color.g + ioVtxList[v2].m_color.g + ioVtxList[v3].m_color.g) / 3.0;
			ioFaceList[ic].m_color.b = (ioVtxList[v1].m_color.b + ioVtxList[v2].m_color.b + ioVtxList[v3].m_color.b) / 3.0;
		}
	}

	time_t now_time = time(NULL);
	/*
	 *	�}�e���A�����X�g���X�V
	 */
	// �o�͂�OBJ�`�� ���� �ʖ��ɐF�w��̏ꍇ
	if ( iOutFtype == 1 && iColorF == 2 ) {
		size_t mcnt = 0;	// �V�K�쐬�����ގ���
		for ( ic = 0; ic < ioFaceList.size(); ++ic ) {
			// �g�p�}�e���A�������w�肳��Ă���ꍇ�̓X�L�b�v
			if ( ioFaceList[ic].m_mtlname.length() > 0 ) continue;

			// �V�K�}�e���A�����i�[
			stringstream ss;
			ss << g_newmtlname << now_time << "_" << mcnt++;
			CMaterial mat( ss.str() );
			mat.m_Kd.r = ioFaceList[ic].m_color.r;
			mat.m_Kd.g = ioFaceList[ic].m_color.g;
			mat.m_Kd.b = ioFaceList[ic].m_color.b;
			mat.m_setKd = true;
			// ���͂�VRML�`����Material�ݒ肪����ꍇ
			if ( ioMtlList.size() > 0 && ioMtlList[0].m_mtlname.length() == 0 &&
				 (ioMtlList[0].m_setTr == true || ioMtlList[0].m_setNs == true) ) {
				mat.m_Tr = ioMtlList[0].m_Tr;
				mat.m_setTr = ioMtlList[0].m_setTr;
				mat.m_Ns = ioMtlList[0].m_Ns;
				mat.m_setNs = ioMtlList[0].m_setNs;
			}
			ioMtlList.push_back(mat);
			ioFaceList[ic].m_mtlname = ss.str();

			// ����F�̖ʂɓ���}�e���A������ݒ�
			for ( jc = ic+1; jc < ioFaceList.size(); ++jc ) {
				if ( fabs(ioFaceList[jc].m_color.r - mat.m_Kd.r) < tol_color &&
					 fabs(ioFaceList[jc].m_color.g - mat.m_Kd.g) < tol_color &&
					 fabs(ioFaceList[jc].m_color.b - mat.m_Kd.b) < tol_color ) {
						 ioFaceList[jc].m_mtlname = ss.str();
				}
			}
		}
	}

	/*
	 *	�_�̐F��ݒ�
	 */
	if ( iColorF == 1 ) {
		for ( ic = 0; ic < ioVtxList.size(); ++ic ) {
			// �_�̐F���ݒ�ς݂̏ꍇ�X�L�b�v
			if ( ioVtxList[ic].m_setrgb == true ) continue;

			DOUBLE r = 0.0;
			DOUBLE g = 0.0;
			DOUBLE b = 0.0;

			// �����ʐ����[�v
			for ( jc = 0; jc < ioVtxList[ic].m_FidxList.size(); ++jc ) {
				size_t fidx = ioVtxList[ic].m_FidxList[jc];
				r += ioFaceList[fidx].m_color.r;
				g += ioFaceList[fidx].m_color.g;
				b += ioFaceList[fidx].m_color.b;
			}

			// �_�̐F���X�V
			ioVtxList[ic].m_color.r = r / ioVtxList[ic].m_FidxList.size();
			ioVtxList[ic].m_color.g = g / ioVtxList[ic].m_FidxList.size();
			ioVtxList[ic].m_color.b = b / ioVtxList[ic].m_FidxList.size();
		}
	}

	/*
	 *	�@���x�N�g���𔽓]����
	 */
	if ( iNrmRevF && iNormalF > 0 ) {
		for ( ic = 0; ic < ioVtxList.size(); ++ic ) {
			ioVtxList[ic].m_nrm.Reverse();
		}
		for ( ic = 0; ic < ioFaceList.size(); ++ic ) {
			ioFaceList[ic].m_nrm.Reverse();
		}
	}

	/*
	 *	�����x��ݒ肷��
	 */
	if ( iTr >= 0.0 ) {
		for ( ic = 0; ic < ioMtlList.size(); ++ic ) {
			ioMtlList[ic].m_Tr = iTr;
			ioMtlList[ic].m_setTr = true;
		}
	}

	return true;
}

/*
 *	�}�e���A���t�@�C����������
 */
bool WriteMTLFile(
		string&				iOutMATFile,	// �o�̓}�e���A���t�@�C���p�X 
		vector<CMaterial>&	iMtlList		// �}�e���A�����X�g
)
{
	size_t ic;

	if( iMtlList.empty() ) return true;

	// �o�̓t�@�C���I�[�v��
	ofstream ofs(iOutMATFile);
	if(!ofs || !ofs.is_open() || ofs.bad() || ofs.fail()){
		g_ofs << "[ERROR] �o�̓t�@�C���I�[�v���G���[" << endl;
		return false;
	}

	ofs << "### MAT File Generated by ConvFormat.exe" << endl << endl;

	/*
	 *	�}�e���A���̏o��
	 */
	for ( ic = 0; ic < iMtlList.size(); ++ic ) {
		// �}�e���A�������Ȃ��ꍇ�A�X�L�b�v
		if ( iMtlList[ic].m_mtlname.length() == 0 ) continue;

		// �}�e���A����
		ofs << "newmtl " << iMtlList[ic].m_mtlname << endl;

		// �A���r�G���g�J���[
		if ( iMtlList[ic].m_setKa ) {
			ofs << "Ka " << iMtlList[ic].m_Ka.r << " " << iMtlList[ic].m_Ka.g << " " << iMtlList[ic].m_Ka.b << endl;
		}

		// �f�B�t���[�Y�J���[�i���̂̐F�j
		if ( iMtlList[ic].m_setKd ) {
			ofs << "Kd " << iMtlList[ic].m_Kd.r << " " << iMtlList[ic].m_Kd.g << " " << iMtlList[ic].m_Kd.b << endl;
		}

		// �X�y�L�����[�J���[�i�n�C���C�g�̐F�j
		if ( iMtlList[ic].m_setKs ) {
			ofs << "Ks " << iMtlList[ic].m_Ks.r << " " << iMtlList[ic].m_Ks.g << " " << iMtlList[ic].m_Ks.b << endl;
		}

		// ���˂̋���
		if ( iMtlList[ic].m_setNs ) {
			ofs << "Ns " << iMtlList[ic].m_Ns << endl;
		}

		// �A���t�@�l�i�����x�j
		if ( iMtlList[ic].m_setTr ) {
			ofs << "Tr " << iMtlList[ic].m_Tr << endl;
		}

		ofs << endl;
	}
	ofs.close();

	return true;
}

/*
 *	OBJ�t�@�C����������
 */
bool WriteOBJFile(
		string&				iOutObjDir,		// �o��OBJ�t�@�C���f�B���N�g���p�X 
		string&				iOutObjFile,	// �o��OBJ�t�@�C����
		string&				iOutObjExt,		// �o��OBJ�t�@�C���g���q
		vector<CVertex3D>&	iVtxList,		// ���_���X�g
		vector<CTriangle>&	iFaceList,		// �ʃ��X�g
		vector<CMaterial>&	iMtlList,		// �}�e���A�����X�g
		int&				iColorF,		// �ΏېF ��1:���_ 2:��
		int&				iNormalF		// �@���x�N�g���o�̓t���O ��1:���_�@�� 2: �ʖ@�� -1:�o�͂��Ȃ�
)
{
	size_t ic;

	if ( iVtxList.empty() || iFaceList.empty() ) return true;
	if ( iColorF != 1 && iColorF != 2 ) return false;
	if ( iNormalF != 1 && iNormalF != 2 && iNormalF != -1 ) return false;

	// �o�̓t�@�C���I�[�v��
	ofstream ofs(iOutObjDir+iOutObjFile+iOutObjExt);
	if(!ofs || !ofs.is_open() || ofs.bad() || ofs.fail()){
		g_ofs << "[ERROR] �o�̓t�@�C���I�[�v���G���[" << endl;
		return false;
	}

	ofs << "### OBJ File Generated by ConvFormat.exe" << endl;

	// �}�e���A���t�@�C�������o��
	if ( iMtlList.size() > 1 ) {
		ofs << "mtllib ./" << iOutObjFile << ".mtl" << endl;
		ofs << endl;
	}

	/*
	 *	���_���W�̏o��
	 */
	for ( vector<CVertex3D>::iterator vit = iVtxList.begin(); vit != iVtxList.end(); ++vit) {
		// �����̏ꍇ�A�X�L�b�v����
		if ( (*vit).m_inv == true ) continue;

		ofs << "v " << (*vit).m_x << " " << (*vit).m_y << " " << (*vit).m_z;
		if ( iColorF == 1 ) {
			if ( (*vit).m_setrgb == true ) {
				ofs << " " << (*vit).m_color.r << " " << (*vit).m_color.g << " " << (*vit).m_color.b << endl;
			} else {
				ofs << endl;
			}
		}
		else {
			ofs << endl;
		}
	}
	ofs << endl;

	/*
	 *	�@���x�N�g���̏o��
	 */
	if ( iNormalF == 1 ) {
		for ( ic = 0; ic < iVtxList.size(); ++ic ) {
			// �����̏ꍇ�A�X�L�b�v����
			if ( iVtxList[ic].m_inv == true ) continue;

			ofs << "vn " << iVtxList[ic].m_nrm.m_x << " " << iVtxList[ic].m_nrm.m_y << " " << iVtxList[ic].m_nrm.m_z << endl;
		}
		ofs << endl;
	}
	else if ( iNormalF == 2 ) {
		for ( ic = 0; ic < iFaceList.size(); ++ic ) {
			// �����̏ꍇ�A�X�L�b�v����
			if ( iFaceList[ic].m_inv == true ) continue;

			ofs << "vn " << iFaceList[ic].m_nrm.m_x << " " << iFaceList[ic].m_nrm.m_y << " " << iFaceList[ic].m_nrm.m_z << endl;
		}
		ofs << endl;
	}

	/*
	 *	�ʏ��̏o��
	 */
	string mtlname = "";
	long v1,v2,v3,ov1,ov2,ov3,of1;
	for ( vector<CTriangle>::iterator fit = iFaceList.begin(); fit != iFaceList.end(); ++fit) {
		// �����̏ꍇ�A�X�L�b�v����
		if ( (*fit).m_inv == true ) continue;

		// �}�e���A�����g�p����ꍇ
		if ( (*fit).m_mtlname.length() > 0 && (*fit).m_mtlname != mtlname ) {
			ofs << endl << "usemtl " << (*fit).m_mtlname << endl;
			mtlname = (*fit).m_mtlname;
		}

		// �\���_�C���f�b�N�X
		v1 = (*fit).m_vidx[0];
		v2 = (*fit).m_vidx[1];
		v3 = (*fit).m_vidx[2];
		// �o�͂���\���_�ԍ�
		ov1 = iVtxList[v1].m_idx+1;
		ov2 = iVtxList[v2].m_idx+1;
		ov3 = iVtxList[v3].m_idx+1;
		// �o�͂���ʔԍ�
		of1 = (*fit).m_idx + 1;
		if ( iNormalF == -1 ) {
			ofs << "f " << ov1 << " " << ov2 << " " << ov3 << endl;
		} else if ( iNormalF == 1 ) {
			ofs << "f " << ov1 << "//" << ov1 << " " << ov2 << "//" << ov2 << " " << ov3 << "//" << ov3 << endl;
		} else {
			ofs << "f " << ov1 << "//" << of1 << " " << ov2 << "//" << of1 << " " << ov3 << "//" << of1 << endl;
		}
	}
	ofs.close();

	// �}�e���A���t�@�C���̏o��
	if ( iMtlList.size() > 1 ) {
		if ( WriteMTLFile( iOutObjDir+iOutObjFile+".mtl", iMtlList ) != true ) {
			g_ofs << "[ERROR] OBJ�t�@�C���̏o�̓G���[" << endl; 
			return false;
		}
	}

	return true;
}

/*
 *	VRML�t�@�C����������
 */
bool WriteVRMLFile(
		string&				iVRMLFile,	// ��������VRML�t�@�C���p�X 
		vector<CVertex3D>&	iVtxList,	// ���_���X�g
		vector<CTriangle>&	iFaceList,	// �ʃ��X�g
		vector<CMaterial>&	iMtlList,	// �}�e���A�����X�g
		int&				iColorF,	// �ΏېF ��1:���_ 2:��
		int&				iNormalF	// �@���x�N�g���o�̓t���O ��1:���_�@�� 2: �ʖ@�� -1:�o�͂��Ȃ�
)
{
	size_t ic;

	if ( iVtxList.empty() || iFaceList.empty() ) return true;
	if ( iColorF != 1 && iColorF != 2 ) return false;
	if ( iNormalF != 1 && iNormalF != 2 && iNormalF != -1 ) return false;

	// �o�̓t�@�C���I�[�v��
	//ofstream ofs(iVRMLFile);
	wofstream ofs(iVRMLFile);
	ofs.imbue(locale(locale(), new codecvt_utf8_utf16<wchar_t>()));
	if(!ofs || !ofs.is_open() || ofs.bad() || ofs.fail()){
		g_ofs << "[ERROR] �o�̓t�@�C���I�[�v���G���[" << endl;
		return false;
	}

	// VRML�o�[�W����
	ofs << "#VRML V2.0 utf8\n";
	ofs << "Transform\n"; 
	ofs << "{\n";
	ofs << "  scale 1 1 1\n";
	ofs << "  translation 0 0 0\n";
	ofs << "  children\n";
	ofs << "  [\n";
	ofs << "    Shape\n";
	ofs << "    {\n"; 
	ofs << "      geometry IndexedFaceSet\n"; 
	ofs << "      {\n";
	ofs << "        convex FALSE\n";
	ofs << "        solid FALSE\n";
	// �ʖ��ɐF�w��̏ꍇ
	if ( iColorF == 2 ) {
		ofs << "        colorPerVertex FALSE\n";
	} else {
		ofs << "        colorPerVertex TRUE\n";
	}
	// �ʖ@���x�N�g���̏ꍇ
	if ( iNormalF == 2 ) {
		ofs << "        normalPerVertex FALSE\n";
	} else if ( iNormalF == 1 ) {
		ofs << "        normalPerVertex TRUE\n";
	}

	// ���_���W�̏o��
	ofs << "        coord Coordinate\n";
	ofs << "        {\n"; 
	ofs << "          point\n";
	ofs << "          [\n"; 
	size_t n = iVtxList.size();
	for(ic = 0; ic < n; ++ic){
		ofs << "            " << iVtxList[ic].m_x << " " << iVtxList[ic].m_y << " " << iVtxList[ic].m_z;
		ofs << ((ic == n-1) ? "\n" : ",\n");
	}
	ofs << "          ]\n";
	ofs << "        }" << endl;

	// ���_�@���x�N�g���̏o��
	if ( iNormalF > 0 ) {
		ofs << "        normal Normal\n";
		ofs << "        {\n"; 
		ofs << "          vector\n";
		ofs << "          [\n"; 
		if ( iNormalF == 1 ) {
			n = iVtxList.size();
			for(ic = 0; ic < n; ++ic){
				ofs << "            " << iVtxList[ic].m_nrm.m_x << " " << iVtxList[ic].m_nrm.m_y << " " << iVtxList[ic].m_nrm.m_z;
				ofs << ((ic == n-1) ? "\n" : ",\n");
			}
		} else {
			n = iFaceList.size();
			for(ic = 0; ic < n; ++ic){
				ofs << "            " << iFaceList[ic].m_nrm.m_x << " " << iFaceList[ic].m_nrm.m_y << " " << iFaceList[ic].m_nrm.m_z;
				ofs << ((ic == n-1) ? "\n" : ",\n");
			}
		}
		ofs << "          ]\n";
		ofs << "        }" << endl;
	}

	// �F���̏o��
	ofs << "        color Color\n";
	ofs << "        {\n";
	ofs << "          color\n";
	ofs << "          [\n"; 
	// �ʖ��ɐF�w��̏ꍇ
	if ( iColorF == 2 ) {
		n = iFaceList.size();
		for(ic = 0; ic < n; ++ic){
			ofs << "            " << iFaceList[ic].m_color.r << " " << iFaceList[ic].m_color.g << " " << iFaceList[ic].m_color.b;
			ofs << ((ic == n-1) ? "\n" : ",\n");
		}
	}
	// �_���ɐF�w��̏ꍇ
	else {
		n = iVtxList.size();
		for(ic = 0; ic < n; ++ic){
			ofs << "            " << iVtxList[ic].m_color.r << " " << iVtxList[ic].m_color.g << " " << iVtxList[ic].m_color.b;
			ofs << ((ic == n-1) ? "\n" : ",\n");
		}
	}
	ofs << "          ]\n";
	ofs << "        }" << endl;

	// �ʑ����̏o��
	ofs << "        coordIndex\n";
	ofs << "        [\n";
	n = iFaceList.size();
	for(ic = 0; ic < n; ++ic){
		ofs << "            " << iFaceList[ic].m_vidx[0] << "," << iFaceList[ic].m_vidx[1] << "," << iFaceList[ic].m_vidx[2];
		ofs << ((ic == n-1) ? ",-1\n" : ",-1,\n");
	}
	ofs << "        ]\n";
	ofs << "      }\n";

	// �������̏o��
	if ( iMtlList.size() > 0 && (iMtlList[0].m_setTr == true || iMtlList[0].m_setNs == true) ) {
		ofs << "      appearance Appearance\n"; 
		ofs << "      {\n";
		ofs << "        material Material\n";
		ofs << "        {\n";
		if ( iMtlList[0].m_setTr == true ) {
			ofs << "          transparency " << iMtlList[0].m_Tr << endl;
		}
		if ( iMtlList[0].m_setNs == true ) {
			ofs << "          shininess " << iMtlList[0].m_Ns << endl;
		}
		ofs << "        }\n";
		ofs << "      }\n";
	}

	ofs << "    }\n"; 
	ofs << "  ]\n";
	ofs << "}\n";

	ofs.close();

	return true;
}

//--------------------------------------------------------------------------
// ���C���֐�
//--------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	int status = -1;	// �X�e�[�^�X
	size_t ic;

	TCHAR driveA[_MAX_DRIVE];
	TCHAR dirA[_MAX_DIR];
	TCHAR fnameA[_MAX_FNAME];
	TCHAR extA[_MAX_EXT];

	TCHAR* opt[5];
	const int optsize = 5;
	TCHAR* arg_infile = NULL;
	TCHAR* arg_outfile = NULL;
	int inftype = 0;
	int outftype = 0;

	string indir;
	string infile;
	string outdir;
	string outfile;
	string outext;

	vector<CVertex3D> vertList;
	vector<CTriangle> faceList;
	vector<CMaterial> mtlList;
	vector<CVector3D> nrmList;

	CMaterial dmyMtl("");

	bool nrmRevF = false;
	DOUBLE tr = -1.0;
	int colF = 0;	// 1:���_�J���[ 2:�ʃJ���[
	int nrmF = 0;	// 1:���_�@���x�N�g�� 2:�ʖ@���x�N�g�� -1:�o�͂��Ȃ�

	g_Start = clock();

	// ���s���W���[���̃p�X�𕪉�
	_tsplitpath_s( argv[0], driveA, _MAX_DRIVE, dirA, _MAX_DIR, NULL, 0, NULL, 0 );

	// ���O�t�@�C�����I�[�v��
	string logfile = driveA;
	logfile += dirA;
	logfile += g_logfile;
	g_ofs.open(logfile);
	if( !g_ofs.is_open() || g_ofs.bad() || g_ofs.fail() ){
		cout << "[ERROR] ���O�t�@�C���̃I�[�v���G���[" << endl;
		return -1;
	}

	if ( argc < 3 || 8 < argc ) {
		goto USAGE;
	}

	// ���͈����𔻕�
	arg_infile = argv[argc-2];
	arg_outfile = argv[argc-1];
	for ( ic = 0; ic < optsize; ++ic ) opt[ic] = NULL;
	for ( ic = 1; ic < (size_t)(argc-2); ++ic ) {
		opt[ic-1] = argv[ic];
	}

	// �I�v�V�����𔻒�
	nrmRevF = false;
	tr = -1.0;
	colF = 0;
	nrmF = 0;
	for ( ic = 0; ic < optsize; ic++ ) {
		if ( opt[ic] != NULL ) {
			if ( _tcsicmp(opt[ic], _T("-nrev")) == 0 ) {
				nrmRevF = true;
			}
			else if ( _tcsicmp(opt[ic], _T("-tr")) == 0 ) {
				if ( ic+1 < optsize ) {
					if ( opt[ic+1] != NULL ) {
						string str = opt[ic+1];
						if ( IsNumeric(str) == true ) {
							tr = _tstof(opt[ic+1]);
						}
					}
				}
				if ( tr < 0 ) goto USAGE;
			}
			else if ( _tcsicmp(opt[ic], _T("-vcol")) == 0 ) {
				if ( colF != 0 ) goto USAGE;
				colF = 1;
			}
			else if ( _tcsicmp(opt[ic], _T("-fcol")) == 0 ) {
				if ( colF != 0 ) goto USAGE;
				colF = 2;
			}
			else if ( _tcsicmp(opt[ic], _T("-vnrm")) == 0 ) {
				if ( nrmF != 0 ) goto USAGE;
				nrmF = 1;
			}
			else if ( _tcsicmp(opt[ic], _T("-fnrm")) == 0 ) {
				if ( nrmF != 0 ) goto USAGE;
				nrmF = 2;
			}
			else if ( _tcsicmp(opt[ic], _T("-nonrm")) == 0 ) {
				if ( nrmF != 0 ) goto USAGE;
				nrmF = -1;
			}
		}
	}
	g_ofs << "���̓t�@�C�� : " << arg_infile << endl;
	g_ofs << "�o�̓t�@�C�� : " << arg_outfile << endl;
	g_ofs << "�I�v�V����   :";
	if ( nrmRevF ) g_ofs << " -nrev";
	if ( tr >= 0 ) g_ofs << " -tr " << tr;
	if ( colF == 1 )
		g_ofs << " -vcol" <<endl;
	else if ( colF == 2 )
		g_ofs << " -fcol" <<endl;
	if ( nrmF == 1 )
		g_ofs << " -vnrm" <<endl;
	else if ( nrmF == 2 )
		g_ofs << " -fnrm" <<endl;
	else if ( nrmF == -1 )
		g_ofs << " -nonrm" <<endl;
	g_ofs << endl;

	// ���̓t�@�C���p�X�𕪉�
	memset(driveA,0,sizeof(driveA));
	memset(dirA,0,sizeof(dirA));
	memset(fnameA,0,sizeof(fnameA));
	memset(extA,0,sizeof(extA));
	_tsplitpath_s( arg_infile, driveA, _MAX_DRIVE, dirA, _MAX_DIR, fnameA, _MAX_FNAME, extA, _MAX_EXT );
	if ( _tcsicmp( extA, _T(".obj") ) == 0 ) {
		inftype = 1;
	} else if ( _tcsicmp( extA, _T(".wrl") ) == 0 ) {
		inftype = 2;
	} else {
		g_ofs << "[ERROR] ���̓t�@�C���̊g���q�����Ή�" << endl; 
		goto RTN;
	}
	indir = driveA;
	indir += dirA;
	infile = fnameA;
	infile += extA;

	// �o�̓t�@�C���p�X�𕪉�
	memset(driveA,0,sizeof(driveA));
	memset(dirA,0,sizeof(dirA));
	memset(fnameA,0,sizeof(fnameA));
	memset(extA,0,sizeof(extA));
	_tsplitpath_s( arg_outfile, driveA, _MAX_DRIVE, dirA, _MAX_DIR, fnameA, _MAX_FNAME, extA, _MAX_EXT );
	if ( _tcsicmp( extA, _T(".obj") ) == 0 ) {
		outftype = 1;
	} else if ( _tcsicmp( extA, _T(".wrl") ) == 0 ) {
		outftype = 2;
	} else {
		g_ofs << "[ERROR] �o�̓t�@�C���̊g���q�����Ή�" << endl; 
		goto RTN;
	}
	outdir = driveA;
	outdir += dirA;
	outfile = fnameA;
	outext = extA;

	// �擪�Ƀ_�~�[�p�}�e���A�����i�[
	mtlList.push_back(dmyMtl);

	// ���̓t�@�C���̓ǂݍ���
	g_ofs << "���̓t�@�C���ǂݍ��݊J�n" << endl;
	if ( inftype == 1 ) {
		// OBJ�t�@�C���̓ǂݍ���
		if ( ReadOBJFile(indir, infile, nrmF, vertList, faceList, mtlList, nrmList) != true ) {
			g_ofs << "[ERROR] OBJ�t�@�C���̓ǂݍ��݃G���[" << endl; 
			goto RTN;
		}
	}
	else if ( inftype == 2 ) {
		// VRML�t�@�C���̓ǂݍ���
		if ( ReadVRMLFile(indir+infile, nrmF, vertList, faceList, mtlList, nrmList) != true ) {
			g_ofs << "[ERROR] VRML�t�@�C���̓ǂݍ��݃G���[" << endl; 
			goto RTN;
		}
	}
	g_ofs << "�t�@�C���ǂݍ��ݏI��[sec]  : " << (clock() - g_Start)/CLOCKS_PER_SEC << endl;
	g_ofs << "\t���_��: " << vertList.size() << endl;
	g_ofs << "\t�ʐ��@: " << faceList.size() << endl;
	if ( nrmF != -1 ) {
		g_ofs << "\t�@����: " << nrmList.size() << endl;
	}

	// �F���I�v�V�����Ŏw�肳��Ă��Ȃ��ꍇ
	if ( colF == 0 ) {
		if ( vertList[0].m_setrgb == true ) {
			colF = 1;
		} else {
			colF = 2;
		}
	}
	// �@�����I�v�V�����Ŏw�肳��Ă��Ȃ��ꍇ
	if ( nrmF == 0 ) {
		if ( nrmList.size() == faceList.size() ) {
			// �t�@�C��������͂��ꂽ�@����񂪖ʖ@���x�N�g���̏ꍇ�A�ʖ@�����o��
			nrmF = 2;
		} else {
			// ��L�ȊO�̏ꍇ�A���_�@�����o��
			nrmF = 1;
		}
	}

	// �����l���X�V
	if ( UpdateAttribute( vertList, faceList, mtlList, nrmList, nrmRevF, tr, colF, nrmF, outftype ) != true ) {
		g_ofs << "[ERROR] �����l�X�V�G���[" << endl; 
		goto RTN;
	}
	g_ofs << "�����l�X�V�I��[sec]  : " << (clock() - g_Start)/CLOCKS_PER_SEC << endl;

	// �o�̓t�@�C���̏����o��
	if ( outftype == 1 ) {
		// OBJ�t�@�C���̏o��
		if ( WriteOBJFile( outdir, outfile, outext, vertList, faceList, mtlList, colF, nrmF ) != true ) {
			g_ofs << "[ERROR] OBJ�t�@�C���̏o�̓G���[" << endl; 
			goto RTN;
		}
	}
	else if ( outftype == 2 ) {
		// VRML�t�@�C���̏o��
		string outpath = arg_outfile;
		if ( WriteVRMLFile( outpath, vertList, faceList, mtlList, colF, nrmF) != true ) {
			g_ofs << "[ERROR] VRML�t�@�C���̏o�̓G���[" << endl; 
			goto RTN;
		}
	}
	g_ofs << "�t�@�C���o�͏I��[sec]     : " << (clock() - g_Start)/CLOCKS_PER_SEC << endl;
	cout << "��������I��" << endl;

	status = 0;

	goto RTN;

USAGE:
	g_ofs << "[ERROR] ���͈������s���ł��B" << endl;
	Usage();
	goto RTN;

RTN:
	g_ofs.close();

	return status;
}

