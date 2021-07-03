// ConvertOBJtoVRML.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

// 実数の精度
//  0 : float
//  1 : double
#define PRECISION	(1)

//--------------------------------------------------------------------------
// ヘッダファイル
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
// 構造体
//--------------------------------------------------------------------------
// RGBカラー
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
// グローバル変数
//--------------------------------------------------------------------------
const string g_logfile = "convert.log";			// ログファイル名
ofstream g_ofs;									// ログファイルストリーム
clock_t g_Start;								// 処理開始時間
const string g_newmtlname = "cnvmaterial_";		// 新規作成する材質名
const DOUBLE tol_color = 1.0 / 255.0;

//--------------------------------------------------------------------------
// 内部関数
//--------------------------------------------------------------------------
// Usage
void Usage()
{
	cout << "ConvFormat.exe [-nrev] [-tr 実数] [-vcol|-fcol] [-vnrm|-fnrm|-nonrm] infile outfile" << endl;
	cout << "	infile  : 入力形状ファイルフルパス" << endl;
	cout << "				※対応フォーマット：*.obj,*.wrl" << endl;
	cout << "	outfile : 出力形状ファイルフルパス" << endl;
	cout << "				※対応フォーマット：*.obj,*.wrl" << endl;
	cout << "	<オプション>" << endl;
	cout << "		-nrev		: 法線ベクトルを反転する" << endl;
	cout << "		-tr 実数	: 透明度(0-1)を設定する" << endl;
	cout << "					:   0 = 不透明" << endl;
	cout << "					:   1 = 透明" << endl;
	cout << "		-vcol		: 頂点カラーで出力する" << endl;
	cout << "		-fcol		: 面カラーで出力する" << endl;
	cout << "		-vnrm		: 頂点法線ベクトルで出力する" << endl;
	cout << "		-fnrm		: 面法線ベクトルで出力する" << endl;
	cout << "		-nonrm		: 法線ベクトルを出力しない" << endl;
	//system("pause");
	return;
}

////ベクトル内積
//inline double DotProduct(CVector3D v1, CVector3D v2) {
//	return v1.m_x * v2.m_x + v1.m_y * v2.m_y + v1.m_z * v2.m_z;
//}

// 先頭末尾の空白(スペース、タブ、改行)を削除
inline void TrimString(string &buf)
{
    string::size_type pos;
    while((pos = buf.find_first_of(" 　\t")) == 0){
        buf.erase(buf.begin());
        if(buf.empty()) break;
    }
    while((pos = buf.find_last_of(" 　\t\r\n")) == string::npos-1){
        buf.erase(buf.end());
        if(buf.empty()) break;
    }
}

// 文字列を区切り文字で分割
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

// 文字列が実数値を表しているかを調べる
inline bool IsNumeric(const string &str)
{
    if(str.find_first_not_of("-0123456789. Ee\t") != string::npos) {
        return false;
    }
 
    return true;
}

// ワイド文字列へ変換。
inline wstring ToWideString( const string& String, unsigned int CodePage )
{
    int Length( ::MultiByteToWideChar( CodePage, MB_PRECOMPOSED, &String[ 0 ], -1, NULL, 0 ) );

    vector<wchar_t>  Result( Length + 1, 0 );

    ::MultiByteToWideChar( CodePage, MB_PRECOMPOSED, &String[ 0 ], -1, &Result[ 0 ], Length );

    return &Result[ 0 ];
}

// 点A→点Bのベクトルを作成
inline CVector3D CreVector( const CVertex3D& A, const CVertex3D& B ) {
	CVector3D vec;
	vec.m_x = B.m_x - A.m_x;
	vec.m_y = B.m_y - A.m_y;
	vec.m_z = B.m_z - A.m_z;
	return vec;
}

/*
 *	頂点ABCで作られた面から法線を計算する。
 */
CVector3D CreatePolygonNormal( const CVertex3D& A, const CVertex3D& B, const CVertex3D& C ) {

	CVector3D AB,AC;

	AB = CreVector( A, B );
	AC = CreVector( A, C );

	CVector3D normal = AB * AC;	//AB ACの外積
	normal.Normalize();//単位ベクトルにする

	return normal;
}

/*
 *	頂点法線ベクトルを計算
 */
bool CalcNormalOfVtx(
		vector<CVertex3D>&			ioVtxList,	// 頂点リスト
		const vector<CTriangle>&	iFaceList	// 面リスト
)
{
	size_t ic, jc;

	// 頂点数ループ
	size_t size1 = ioVtxList.size();
	bool bRet;
	CVector3D normal;
	size_t fidx;
	for ( ic = 0; ic < size1; ++ic ) {
		// 初期化
		ioVtxList[ic].m_nrm.m_x = 0.0; ioVtxList[ic].m_nrm.m_y = 0.0; ioVtxList[ic].m_nrm.m_z = 0.0;

		// 所属面数ループ
		size_t size2 = ioVtxList[ic].m_FidxList.size();
		for ( jc = 0; jc < size2; ++jc ) {
			// 面インデックス
			fidx = ioVtxList[ic].m_FidxList[jc];

			// 面法線ベクトルを加算
//			ioVtxList[ic].m_nrm += normal;
			ioVtxList[ic].m_nrm = ioVtxList[ic].m_nrm + iFaceList[fidx].m_nrm;
		}

		if ( size2 == 0) ioVtxList[ic].m_nrm = normal;

		// 頂点法線ベクトルを単位化
		bRet = ioVtxList[ic].m_nrm.Normalize();
		if (bRet != true) {
			g_ofs << "[Warning] 頂点法線ベクトルが０ベクトル : " << ic << " : " 
				  << ioVtxList[ic].m_nrm.m_x << "," << ioVtxList[ic].m_nrm.m_y << "," << ioVtxList[ic].m_nrm.m_z << " :";
			for ( jc = 0; jc < size2; ++jc ) {
				g_ofs << " " << ioVtxList[ic].m_FidxList[jc];
			}
			g_ofs << endl;
			continue;
			// 暫定として、先頭の所属面の法線ベクトルをセット
			fidx = ioVtxList[ic].m_FidxList[0];
			ioVtxList[ic].m_nrm = iFaceList[fidx].m_nrm;
			ioVtxList[ic].m_nrm.Normalize();
		}
	}

	return true;
}

/*
 *	MTLファイル読み込み
 */
bool ReadMTLFile(
		string&				iMTLFile,	// 読み込みMTLファイル名
		vector<CMaterial>&	ioMtlList	// マテリアルリスト
)
{
	// 入力ファイルオープン
	ifstream ifs(iMTLFile);
	if(!ifs || !ifs.is_open() || ifs.bad() || ifs.fail()){
		g_ofs << "[ERROR] MTLファイルオープンエラー" << endl;
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

		// '#'以降はコメントとして無視
		if( (comment_start = buf.find('#')) != string::size_type(-1) )
			buf = buf.substr(0, comment_start);

		// 行頭のスペース，タブを削除
		TrimString(buf);

		// 空行は無視
		if(buf.empty())
			continue;

		// KEYを取得
		TCHAR key[256];
		_stscanf(buf.c_str(), "%s", key);

		// マテリアル名
		if ( _tcsicmp(key, _T("newmtl")) == 0 ) {
			_stscanf(buf.c_str(), "%s %s", dmy, mtlname);

			// マテリアルを格納
			CMaterial mat(mtlname);
			ioMtlList.push_back(mat);
		}
		// アンビエントカラー
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
		// ディフューズカラー（物体の色）
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
		// スペキュラーカラー（ハイライトの色）
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
		// 反射の強さ
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
		// アルファ値（透明度）
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

	g_ofs << "マテリアルファイル読み込み終了[sec]  : " << (clock() - g_Start)/CLOCKS_PER_SEC << endl;
	g_ofs << "\tマテリアル数: " << ioMtlList.size() << endl;

	return true;
}

/*
 *	OBJファイル読み込み
 *	※法線ベクトル情報、テクスチャ情報は無視する
 */
bool ReadOBJFile(
		string&				iObjDir,	// 読み込みOBJファイルディレクトリパス 
		string&				iObjFile,	// 読み込みOBJファイル名
		int&				iNormalF,	// 法線ベクトル出力フラグ ※1:頂点法線 2: 面法線 -1:出力しない
		vector<CVertex3D>&	oVtxList,	// 頂点リスト
		vector<CTriangle>&	oFaceList,	// 面リスト
		vector<CMaterial>&	ioMtlList,	// マテリアルリスト
		vector<CVector3D>&	oNrmList	// 法線ベクトルリスト
)
{
	size_t ic;

	// 初期化
	oVtxList.clear();
	oFaceList.clear();
	oNrmList.clear();

	// 入力ファイルオープン
	ifstream ifs(iObjDir+iObjFile);
	if(!ifs || !ifs.is_open() || ifs.bad() || ifs.fail()){
		g_ofs << "[ERROR] OBJファイルオープンエラー" << endl;
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

		// '#'以降はコメントとして無視
		if( (comment_start = buf.find('#')) != string::size_type(-1) )
			buf = buf.substr(0, comment_start);

		// 行頭のスペース，タブを削除
		TrimString(buf);

		// 空行は無視
		if(buf.empty())
			continue;

		// KEYを取得
		TCHAR key[256];
		_stscanf(buf.c_str(), "%s", key);

		// 頂点座標
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
		// 法線ベクトル
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
		// 面
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
		// マテリアルファイル名
		else if ( _tcsicmp(key, _T("mtllib")) == 0 ){
			_stscanf(buf.c_str(), "%s %s", dmy, mtlfile);
		}
		// 使用マテリアル名
		else if ( _tcsicmp(key, _T("usemtl")) == 0 ){
			_stscanf(buf.c_str(), "%s %s", dmy, mtlname);
		}
	}
	ifs.close();

	// マテリアルファイルの読み込み
	if ( _tcslen(mtlfile) > 0 ) {
		string matfile(iObjDir+mtlfile);
		if ( ReadMTLFile(matfile, ioMtlList) != true ) {
			g_ofs << "[ERROR] マテリアルファイルの読み込みエラー" << endl; 
			return false;
		}
	}

	// 入力ファイルに法線ベクトルデータがある場合
	if ( oNrmList.size() > 0 ) {
		for ( ic = 0; ic < oFaceList.size(); ++ic ) {
			// 面の頂点インデックスを取得
			v1 = oFaceList[ic].m_vidx[0];
			v2 = oFaceList[ic].m_vidx[1];
			v3 = oFaceList[ic].m_vidx[2];
			// 面の頂点の法線ベクトルインデックスを取得
			n1 = oFaceList[ic].m_nidx[0];
			n2 = oFaceList[ic].m_nidx[1];
			n3 = oFaceList[ic].m_nidx[2];

			// 頂点の法線ベクトルに加算
			oVtxList[v1].m_nrm = oVtxList[v1].m_nrm + oNrmList[n1];
			oVtxList[v2].m_nrm = oVtxList[v2].m_nrm + oNrmList[n2];
			oVtxList[v3].m_nrm = oVtxList[v3].m_nrm + oNrmList[n3];

			// 面の法線ベクトルを格納
			if ( n1 == n2 == n3 ) {
				oFaceList[ic].m_nrm = oNrmList[n1];
			} else {
				oFaceList[ic].m_nrm = oNrmList[n1] + oNrmList[n2] + oNrmList[n3];
			}
			oFaceList[ic].m_nrm.Normalize();
		}

		// 頂点の法線ベクトルの単位化
		for ( ic = 0; ic < oVtxList.size(); ++ic ) {
			oVtxList[ic].m_nrm.Normalize();
		}
	}

	if(oVtxList.empty() || oFaceList.empty()) return false;

	return true;
}

/*
 *	VRMLファイル読み込み
 *	※Transform,childrenに未対応
 */
bool ReadVRMLFile(
		string&				iVRMLFile,	// 読み込みOBJファイルパス 
		int&				iNormalF,	// 法線ベクトル出力フラグ ※1:頂点法線 2: 面法線 -1:出力しない
		vector<CVertex3D>&	oVtxList,	// 頂点リスト
		vector<CTriangle>&	oFaceList,	// 面リスト
		vector<CMaterial>&	ioMtlList,	// マテリアルリスト
		vector<CVector3D>&	oNrmList	// 法線ベクトルリスト
)
{
	size_t ic;

	// 初期化
	oVtxList.clear();
	oFaceList.clear();
	oNrmList.clear();

	// 入力ファイルオープン
	ifstream ifs(iVRMLFile);
	if(!ifs || !ifs.is_open() || ifs.bad() || ifs.fail()){
		g_ofs << "[ERROR] VRMLファイルオープンエラー" << endl;
		return false;
	}

	string buf;
	string::size_type comment_start = 0;
	bool shapeNode = false;
	int shape_field = 0;		// Shapeフィールド	1:IndexedFaceSet 2:Appearance
	int geom_field = 0;			// IndexedFaceSetフィールド	1:Coordinate 2:Normal 3:Color 4:coordIndex
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

		// '#'以降はコメントとして無視
		if( (comment_start = buf.find('#')) != string::size_type(-1) )
			buf = buf.substr(0, comment_start);

		// 行頭のスペース，タブを削除
		TrimString(buf);

		// 空行は無視
		if(buf.empty()) continue;

		// 小文字に変換
		transform( buf.begin(), buf.end(), buf.begin(), ::tolower );

		// キーワード判定
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
			// 先頭のマテリアルに格納
			ioMtlList[0].m_Tr = val;
			ioMtlList[0].m_setTr = true;
		}
		else if ( shape_field == 2 && _tcsstr(buf.c_str(), _T("shininess")) != NULL ) {
#if PRECISION
			_stscanf(buf.c_str(), "%s %lf", dmy, &val);
#else
			_stscanf(buf.c_str(), "%s %f", dmy, &val);
#endif
			// 先頭のマテリアルに格納
			ioMtlList[0].m_Ns = val;
			ioMtlList[0].m_setNs = true;
		}
		else {

			// カンマで分割
			string delim = ",";
			vector<string> strlist = split( buf, delim );

			/*
			 *	点座標値を取得
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
						// 点リスト未格納の場合
						if ( updateVtxF == false ) {
							CVertex3D vtx(oVtxList.size(), x, y, z);
							oVtxList.push_back(vtx);
						}
						else {
							if ( vcnt >= oVtxList.size() ) {
								g_ofs << "[ERROR] 点更新エラー" << endl;
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
			 *	法線ベクトルを取得
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
			 *	色を取得
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
							// 点リスト未格納の場合
							if ( updateVtxF == false ) {
								CVertex3D vtx(oVtxList.size(), minv, minv, minv, r, g, b);
								oVtxList.push_back(vtx);
							}
							else {
								if ( vcnt >= oVtxList.size() ) {
									g_ofs << "[ERROR] 色更新エラー" << endl;
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
							// 面リスト未格納の場合
							if ( updateFaceF == false ) {
								CTriangle tri(oFaceList.size(), -1, -1, -1, r, g, b);
								oFaceList.push_back(tri);
							}
							else {
								if ( fcnt >= oFaceList.size() ) {
									g_ofs << "[ERROR] 色更新エラー" << endl;
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
			 *	面を取得
			 */
			if ( geom_field == 4 ) {
				v1 = -1; v2 = -1; v3 = -1;
				for ( ic = 0; ic < strlist.size(); ++ic ) {
					long vidx = -999;
					_stscanf(strlist[ic].c_str(), "%ld", &vidx);
					// 構成点インデックスの場合
					if ( vidx >= 0 ) {
						if ( v1 < 0 )
							v1 = vidx;
						else if ( v2 < 0 )
							v2 = vidx;
						else if ( v3 < 0 ) 
							v3 = vidx;
						else {
							g_ofs << "[ERROR] 構成点数エラー" << endl;
							return false;
						}
					}
					// 区切り文字の場合
					else if ( vidx == -1 ) {
						// 面リストを更新

						// 面リスト未格納の場合
						if ( updateFaceF == false ) {
							CTriangle tri(oFaceList.size(), v1, v2, v3 );
							oFaceList.push_back(tri);
						}
						else {
							if ( fcnt >= oFaceList.size() ) {
								g_ofs << "[ERROR] 構成点更新エラー" << endl;
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
	 *	面の法線ベクトル情報を設定
	 */
	// 法線ベクトル情報があった場合
	if ( oNrmList.size() > 0 ) {
		// 頂点法線ベクトルの場合
		if ( nrmvF ) {
			// 頂点法線ベクトルを格納
			for ( ic = 0; ic < oVtxList.size(); ++ic ) {
				oVtxList[ic].m_nrm = oNrmList[ic];
				oVtxList[ic].m_nrm.Normalize();
			}
			// 面の頂点法線ベクトルを格納
			for ( ic = 0; ic < oFaceList.size(); ++ic ) {
				// 頂点インデックスを取得
				v1 = oFaceList[ic].m_vidx[0];
				v2 = oFaceList[ic].m_vidx[1];
				v3 = oFaceList[ic].m_vidx[2];

				// 面の頂点法線情報を格納
				oFaceList[ic].m_nrm = oVtxList[v1].m_nrm + oVtxList[v2].m_nrm + oVtxList[v3].m_nrm;
				oFaceList[ic].m_nrm.Normalize();
			}
		}
		// 面法線ベクトルの場合
		else {
			// 面法線ベクトルを格納
			for ( ic = 0; ic < oFaceList.size(); ++ic ) {
				// 頂点インデックスを取得
				v1 = oFaceList[ic].m_vidx[0];
				v2 = oFaceList[ic].m_vidx[1];
				v3 = oFaceList[ic].m_vidx[2];

				// 面の頂点法線情報を格納
				oFaceList[ic].m_nrm = oNrmList[ic];
				oFaceList[ic].m_nrm.Normalize();

				// 頂点法線ベクトルに加算
				oVtxList[v1].m_nrm = oVtxList[v1].m_nrm + oNrmList[ic];
				oVtxList[v2].m_nrm = oVtxList[v2].m_nrm + oNrmList[ic];
				oVtxList[v3].m_nrm = oVtxList[v3].m_nrm + oNrmList[ic];
			}
			// 頂点法線ベクトルを単位化
			for ( ic = 0; ic < oVtxList.size(); ++ic ) {
				oVtxList[ic].m_nrm.Normalize();
			}
		}
	}

	if(oVtxList.empty() || oFaceList.empty()) return false;

	return true;
}

/*
 *	属性値を更新する
 */
bool UpdateAttribute(
		vector<CVertex3D>&	ioVtxList,	// 頂点リスト
		vector<CTriangle>&	ioFaceList,	// 面リスト
		vector<CMaterial>&	ioMtlList,	// マテリアルリスト
		vector<CVector3D>&	iNrmList,	// 法線ベクトルリスト
		bool&				iNrmRevF,	// 法線反転フラグ
		DOUBLE&				iTr,		// 透明度
		int&				iColorF,	// 対象色 ※1:頂点 2:面
		int&				iNormalF,	// 法線ベクトル出力フラグ ※1:頂点法線 2: 面法線 -1:出力しない
		int&				iOutFtype	// 出力ファイルタイプ
										//	1: OBJ形式
										//	2: VRML形式
)
{
	size_t ic, jc;
	long v1,v2,v3;

	if ( ioVtxList.empty() || ioFaceList.empty() ) return true;
	if ( iColorF != 1 && iColorF != 2 ) return false;
	if ( iNormalF != 1 && iNormalF != 2 && iNormalF != -1 ) return false;

	/*
	 *	頂点の所属面リストを格納
	 */
	for ( ic = 0; ic < ioFaceList.size(); ++ic ) {
		// 面の頂点インデックスを取得
		v1 = ioFaceList[ic].m_vidx[0];
		v2 = ioFaceList[ic].m_vidx[1];
		v3 = ioFaceList[ic].m_vidx[2];

		// 頂点情報－所属面に面インデックスを格納
		ioVtxList[v1].m_FidxList.push_back(ic);
		ioVtxList[v2].m_FidxList.push_back(ic);
		ioVtxList[v3].m_FidxList.push_back(ic);
	}

	// 入力ファイルに法線ベクトルデータがない場合
	if ( iNormalF > 0 && iNrmList.empty() ) {
		// 面法線ベクトルを計算
		for ( ic = 0; ic < ioFaceList.size(); ++ic ) {
			// 面の頂点インデックスを取得
			v1 = ioFaceList[ic].m_vidx[0];
			v2 = ioFaceList[ic].m_vidx[1];
			v3 = ioFaceList[ic].m_vidx[2];

			// 面法線ベクトルを計算
			CVector3D nrm = CreatePolygonNormal( ioVtxList[v1], ioVtxList[v2], ioVtxList[v3] );

			// 面法線ベクトルを格納
			ioFaceList[ic].m_nrm = nrm;
		}

		// 頂点法線ベクトルを計算
		if ( CalcNormalOfVtx(ioVtxList,ioFaceList) != true ) {
			g_ofs << "[ERROR] 頂点法線ベクトルの計算エラー" << endl; 
			return false;
		}
	}

	/*
	 *	面の色を設定
	 */
	for ( ic = 0; ic < ioFaceList.size(); ++ic ) {
		// 面の色が設定済みの場合スキップ
		if ( ioFaceList[ic].m_setrgb == true ) continue;

		// 使用マテリアル名が指定されている場合
		if ( ioFaceList[ic].m_mtlname.length() > 0 ) {
			// 使用マテリアル名を検索
			bool findF = false;
			for ( jc = 0; jc < ioMtlList.size(); ++jc ) {
				if ( _tcsicmp(ioMtlList[jc].m_mtlname.c_str(), ioFaceList[ic].m_mtlname.c_str()) == 0 ) {
					ioFaceList[ic].m_color = ioMtlList[jc].m_Kd;
					findF = true;
					break;
				}
			}
			// 見つからなかった場合
			if ( !findF ) {
				g_ofs << "[ERROR] 使用マテリアルが存在しない : " << ioFaceList[ic].m_mtlname << endl; 
				return false;
			}
		}
		// 使用マテリアル名が指定されていない場合
		else {
			// 構成点インデックス
			long v1,v2,v3;
			v1 = ioFaceList[ic].m_vidx[0];
			v2 = ioFaceList[ic].m_vidx[1];
			v3 = ioFaceList[ic].m_vidx[2];

			// 構成点の色から面の色を計算（構成点の色の平均）
			ioFaceList[ic].m_color.r = (ioVtxList[v1].m_color.r + ioVtxList[v2].m_color.r + ioVtxList[v3].m_color.r) / 3.0;
			ioFaceList[ic].m_color.g = (ioVtxList[v1].m_color.g + ioVtxList[v2].m_color.g + ioVtxList[v3].m_color.g) / 3.0;
			ioFaceList[ic].m_color.b = (ioVtxList[v1].m_color.b + ioVtxList[v2].m_color.b + ioVtxList[v3].m_color.b) / 3.0;
		}
	}

	time_t now_time = time(NULL);
	/*
	 *	マテリアルリストを更新
	 */
	// 出力がOBJ形式 かつ 面毎に色指定の場合
	if ( iOutFtype == 1 && iColorF == 2 ) {
		size_t mcnt = 0;	// 新規作成した材質名
		for ( ic = 0; ic < ioFaceList.size(); ++ic ) {
			// 使用マテリアル名が指定されている場合はスキップ
			if ( ioFaceList[ic].m_mtlname.length() > 0 ) continue;

			// 新規マテリアルを格納
			stringstream ss;
			ss << g_newmtlname << now_time << "_" << mcnt++;
			CMaterial mat( ss.str() );
			mat.m_Kd.r = ioFaceList[ic].m_color.r;
			mat.m_Kd.g = ioFaceList[ic].m_color.g;
			mat.m_Kd.b = ioFaceList[ic].m_color.b;
			mat.m_setKd = true;
			// 入力がVRML形式でMaterial設定がある場合
			if ( ioMtlList.size() > 0 && ioMtlList[0].m_mtlname.length() == 0 &&
				 (ioMtlList[0].m_setTr == true || ioMtlList[0].m_setNs == true) ) {
				mat.m_Tr = ioMtlList[0].m_Tr;
				mat.m_setTr = ioMtlList[0].m_setTr;
				mat.m_Ns = ioMtlList[0].m_Ns;
				mat.m_setNs = ioMtlList[0].m_setNs;
			}
			ioMtlList.push_back(mat);
			ioFaceList[ic].m_mtlname = ss.str();

			// 同一色の面に同一マテリアル名を設定
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
	 *	点の色を設定
	 */
	if ( iColorF == 1 ) {
		for ( ic = 0; ic < ioVtxList.size(); ++ic ) {
			// 点の色が設定済みの場合スキップ
			if ( ioVtxList[ic].m_setrgb == true ) continue;

			DOUBLE r = 0.0;
			DOUBLE g = 0.0;
			DOUBLE b = 0.0;

			// 所属面数ループ
			for ( jc = 0; jc < ioVtxList[ic].m_FidxList.size(); ++jc ) {
				size_t fidx = ioVtxList[ic].m_FidxList[jc];
				r += ioFaceList[fidx].m_color.r;
				g += ioFaceList[fidx].m_color.g;
				b += ioFaceList[fidx].m_color.b;
			}

			// 点の色を更新
			ioVtxList[ic].m_color.r = r / ioVtxList[ic].m_FidxList.size();
			ioVtxList[ic].m_color.g = g / ioVtxList[ic].m_FidxList.size();
			ioVtxList[ic].m_color.b = b / ioVtxList[ic].m_FidxList.size();
		}
	}

	/*
	 *	法線ベクトルを反転する
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
	 *	透明度を設定する
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
 *	マテリアルファイル書き込み
 */
bool WriteMTLFile(
		string&				iOutMATFile,	// 出力マテリアルファイルパス 
		vector<CMaterial>&	iMtlList		// マテリアルリスト
)
{
	size_t ic;

	if( iMtlList.empty() ) return true;

	// 出力ファイルオープン
	ofstream ofs(iOutMATFile);
	if(!ofs || !ofs.is_open() || ofs.bad() || ofs.fail()){
		g_ofs << "[ERROR] 出力ファイルオープンエラー" << endl;
		return false;
	}

	ofs << "### MAT File Generated by ConvFormat.exe" << endl << endl;

	/*
	 *	マテリアルの出力
	 */
	for ( ic = 0; ic < iMtlList.size(); ++ic ) {
		// マテリアル名がない場合、スキップ
		if ( iMtlList[ic].m_mtlname.length() == 0 ) continue;

		// マテリアル名
		ofs << "newmtl " << iMtlList[ic].m_mtlname << endl;

		// アンビエントカラー
		if ( iMtlList[ic].m_setKa ) {
			ofs << "Ka " << iMtlList[ic].m_Ka.r << " " << iMtlList[ic].m_Ka.g << " " << iMtlList[ic].m_Ka.b << endl;
		}

		// ディフューズカラー（物体の色）
		if ( iMtlList[ic].m_setKd ) {
			ofs << "Kd " << iMtlList[ic].m_Kd.r << " " << iMtlList[ic].m_Kd.g << " " << iMtlList[ic].m_Kd.b << endl;
		}

		// スペキュラーカラー（ハイライトの色）
		if ( iMtlList[ic].m_setKs ) {
			ofs << "Ks " << iMtlList[ic].m_Ks.r << " " << iMtlList[ic].m_Ks.g << " " << iMtlList[ic].m_Ks.b << endl;
		}

		// 反射の強さ
		if ( iMtlList[ic].m_setNs ) {
			ofs << "Ns " << iMtlList[ic].m_Ns << endl;
		}

		// アルファ値（透明度）
		if ( iMtlList[ic].m_setTr ) {
			ofs << "Tr " << iMtlList[ic].m_Tr << endl;
		}

		ofs << endl;
	}
	ofs.close();

	return true;
}

/*
 *	OBJファイル書き込み
 */
bool WriteOBJFile(
		string&				iOutObjDir,		// 出力OBJファイルディレクトリパス 
		string&				iOutObjFile,	// 出力OBJファイル名
		string&				iOutObjExt,		// 出力OBJファイル拡張子
		vector<CVertex3D>&	iVtxList,		// 頂点リスト
		vector<CTriangle>&	iFaceList,		// 面リスト
		vector<CMaterial>&	iMtlList,		// マテリアルリスト
		int&				iColorF,		// 対象色 ※1:頂点 2:面
		int&				iNormalF		// 法線ベクトル出力フラグ ※1:頂点法線 2: 面法線 -1:出力しない
)
{
	size_t ic;

	if ( iVtxList.empty() || iFaceList.empty() ) return true;
	if ( iColorF != 1 && iColorF != 2 ) return false;
	if ( iNormalF != 1 && iNormalF != 2 && iNormalF != -1 ) return false;

	// 出力ファイルオープン
	ofstream ofs(iOutObjDir+iOutObjFile+iOutObjExt);
	if(!ofs || !ofs.is_open() || ofs.bad() || ofs.fail()){
		g_ofs << "[ERROR] 出力ファイルオープンエラー" << endl;
		return false;
	}

	ofs << "### OBJ File Generated by ConvFormat.exe" << endl;

	// マテリアルファイル名を出力
	if ( iMtlList.size() > 1 ) {
		ofs << "mtllib ./" << iOutObjFile << ".mtl" << endl;
		ofs << endl;
	}

	/*
	 *	頂点座標の出力
	 */
	for ( vector<CVertex3D>::iterator vit = iVtxList.begin(); vit != iVtxList.end(); ++vit) {
		// 無効の場合、スキップする
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
	 *	法線ベクトルの出力
	 */
	if ( iNormalF == 1 ) {
		for ( ic = 0; ic < iVtxList.size(); ++ic ) {
			// 無効の場合、スキップする
			if ( iVtxList[ic].m_inv == true ) continue;

			ofs << "vn " << iVtxList[ic].m_nrm.m_x << " " << iVtxList[ic].m_nrm.m_y << " " << iVtxList[ic].m_nrm.m_z << endl;
		}
		ofs << endl;
	}
	else if ( iNormalF == 2 ) {
		for ( ic = 0; ic < iFaceList.size(); ++ic ) {
			// 無効の場合、スキップする
			if ( iFaceList[ic].m_inv == true ) continue;

			ofs << "vn " << iFaceList[ic].m_nrm.m_x << " " << iFaceList[ic].m_nrm.m_y << " " << iFaceList[ic].m_nrm.m_z << endl;
		}
		ofs << endl;
	}

	/*
	 *	面情報の出力
	 */
	string mtlname = "";
	long v1,v2,v3,ov1,ov2,ov3,of1;
	for ( vector<CTriangle>::iterator fit = iFaceList.begin(); fit != iFaceList.end(); ++fit) {
		// 無効の場合、スキップする
		if ( (*fit).m_inv == true ) continue;

		// マテリアルを使用する場合
		if ( (*fit).m_mtlname.length() > 0 && (*fit).m_mtlname != mtlname ) {
			ofs << endl << "usemtl " << (*fit).m_mtlname << endl;
			mtlname = (*fit).m_mtlname;
		}

		// 構成点インデックス
		v1 = (*fit).m_vidx[0];
		v2 = (*fit).m_vidx[1];
		v3 = (*fit).m_vidx[2];
		// 出力する構成点番号
		ov1 = iVtxList[v1].m_idx+1;
		ov2 = iVtxList[v2].m_idx+1;
		ov3 = iVtxList[v3].m_idx+1;
		// 出力する面番号
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

	// マテリアルファイルの出力
	if ( iMtlList.size() > 1 ) {
		if ( WriteMTLFile( iOutObjDir+iOutObjFile+".mtl", iMtlList ) != true ) {
			g_ofs << "[ERROR] OBJファイルの出力エラー" << endl; 
			return false;
		}
	}

	return true;
}

/*
 *	VRMLファイル書き込み
 */
bool WriteVRMLFile(
		string&				iVRMLFile,	// 書き込みVRMLファイルパス 
		vector<CVertex3D>&	iVtxList,	// 頂点リスト
		vector<CTriangle>&	iFaceList,	// 面リスト
		vector<CMaterial>&	iMtlList,	// マテリアルリスト
		int&				iColorF,	// 対象色 ※1:頂点 2:面
		int&				iNormalF	// 法線ベクトル出力フラグ ※1:頂点法線 2: 面法線 -1:出力しない
)
{
	size_t ic;

	if ( iVtxList.empty() || iFaceList.empty() ) return true;
	if ( iColorF != 1 && iColorF != 2 ) return false;
	if ( iNormalF != 1 && iNormalF != 2 && iNormalF != -1 ) return false;

	// 出力ファイルオープン
	//ofstream ofs(iVRMLFile);
	wofstream ofs(iVRMLFile);
	ofs.imbue(locale(locale(), new codecvt_utf8_utf16<wchar_t>()));
	if(!ofs || !ofs.is_open() || ofs.bad() || ofs.fail()){
		g_ofs << "[ERROR] 出力ファイルオープンエラー" << endl;
		return false;
	}

	// VRMLバージョン
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
	// 面毎に色指定の場合
	if ( iColorF == 2 ) {
		ofs << "        colorPerVertex FALSE\n";
	} else {
		ofs << "        colorPerVertex TRUE\n";
	}
	// 面法線ベクトルの場合
	if ( iNormalF == 2 ) {
		ofs << "        normalPerVertex FALSE\n";
	} else if ( iNormalF == 1 ) {
		ofs << "        normalPerVertex TRUE\n";
	}

	// 頂点座標の出力
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

	// 頂点法線ベクトルの出力
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

	// 色情報の出力
	ofs << "        color Color\n";
	ofs << "        {\n";
	ofs << "          color\n";
	ofs << "          [\n"; 
	// 面毎に色指定の場合
	if ( iColorF == 2 ) {
		n = iFaceList.size();
		for(ic = 0; ic < n; ++ic){
			ofs << "            " << iFaceList[ic].m_color.r << " " << iFaceList[ic].m_color.g << " " << iFaceList[ic].m_color.b;
			ofs << ((ic == n-1) ? "\n" : ",\n");
		}
	}
	// 点毎に色指定の場合
	else {
		n = iVtxList.size();
		for(ic = 0; ic < n; ++ic){
			ofs << "            " << iVtxList[ic].m_color.r << " " << iVtxList[ic].m_color.g << " " << iVtxList[ic].m_color.b;
			ofs << ((ic == n-1) ? "\n" : ",\n");
		}
	}
	ofs << "          ]\n";
	ofs << "        }" << endl;

	// 位相情報の出力
	ofs << "        coordIndex\n";
	ofs << "        [\n";
	n = iFaceList.size();
	for(ic = 0; ic < n; ++ic){
		ofs << "            " << iFaceList[ic].m_vidx[0] << "," << iFaceList[ic].m_vidx[1] << "," << iFaceList[ic].m_vidx[2];
		ofs << ((ic == n-1) ? ",-1\n" : ",-1,\n");
	}
	ofs << "        ]\n";
	ofs << "      }\n";

	// 属性情報の出力
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
// メイン関数
//--------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	int status = -1;	// ステータス
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
	int colF = 0;	// 1:頂点カラー 2:面カラー
	int nrmF = 0;	// 1:頂点法線ベクトル 2:面法線ベクトル -1:出力しない

	g_Start = clock();

	// 実行モジュールのパスを分解
	_tsplitpath_s( argv[0], driveA, _MAX_DRIVE, dirA, _MAX_DIR, NULL, 0, NULL, 0 );

	// ログファイルをオープン
	string logfile = driveA;
	logfile += dirA;
	logfile += g_logfile;
	g_ofs.open(logfile);
	if( !g_ofs.is_open() || g_ofs.bad() || g_ofs.fail() ){
		cout << "[ERROR] ログファイルのオープンエラー" << endl;
		return -1;
	}

	if ( argc < 3 || 8 < argc ) {
		goto USAGE;
	}

	// 入力引数を判別
	arg_infile = argv[argc-2];
	arg_outfile = argv[argc-1];
	for ( ic = 0; ic < optsize; ++ic ) opt[ic] = NULL;
	for ( ic = 1; ic < (size_t)(argc-2); ++ic ) {
		opt[ic-1] = argv[ic];
	}

	// オプションを判定
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
	g_ofs << "入力ファイル : " << arg_infile << endl;
	g_ofs << "出力ファイル : " << arg_outfile << endl;
	g_ofs << "オプション   :";
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

	// 入力ファイルパスを分解
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
		g_ofs << "[ERROR] 入力ファイルの拡張子が未対応" << endl; 
		goto RTN;
	}
	indir = driveA;
	indir += dirA;
	infile = fnameA;
	infile += extA;

	// 出力ファイルパスを分解
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
		g_ofs << "[ERROR] 出力ファイルの拡張子が未対応" << endl; 
		goto RTN;
	}
	outdir = driveA;
	outdir += dirA;
	outfile = fnameA;
	outext = extA;

	// 先頭にダミー用マテリアルを格納
	mtlList.push_back(dmyMtl);

	// 入力ファイルの読み込み
	g_ofs << "入力ファイル読み込み開始" << endl;
	if ( inftype == 1 ) {
		// OBJファイルの読み込み
		if ( ReadOBJFile(indir, infile, nrmF, vertList, faceList, mtlList, nrmList) != true ) {
			g_ofs << "[ERROR] OBJファイルの読み込みエラー" << endl; 
			goto RTN;
		}
	}
	else if ( inftype == 2 ) {
		// VRMLファイルの読み込み
		if ( ReadVRMLFile(indir+infile, nrmF, vertList, faceList, mtlList, nrmList) != true ) {
			g_ofs << "[ERROR] VRMLファイルの読み込みエラー" << endl; 
			goto RTN;
		}
	}
	g_ofs << "ファイル読み込み終了[sec]  : " << (clock() - g_Start)/CLOCKS_PER_SEC << endl;
	g_ofs << "\t頂点数: " << vertList.size() << endl;
	g_ofs << "\t面数　: " << faceList.size() << endl;
	if ( nrmF != -1 ) {
		g_ofs << "\t法線数: " << nrmList.size() << endl;
	}

	// 色がオプションで指定されていない場合
	if ( colF == 0 ) {
		if ( vertList[0].m_setrgb == true ) {
			colF = 1;
		} else {
			colF = 2;
		}
	}
	// 法線がオプションで指定されていない場合
	if ( nrmF == 0 ) {
		if ( nrmList.size() == faceList.size() ) {
			// ファイルから入力された法線情報が面法線ベクトルの場合、面法線を出力
			nrmF = 2;
		} else {
			// 上記以外の場合、頂点法線を出力
			nrmF = 1;
		}
	}

	// 属性値を更新
	if ( UpdateAttribute( vertList, faceList, mtlList, nrmList, nrmRevF, tr, colF, nrmF, outftype ) != true ) {
		g_ofs << "[ERROR] 属性値更新エラー" << endl; 
		goto RTN;
	}
	g_ofs << "属性値更新終了[sec]  : " << (clock() - g_Start)/CLOCKS_PER_SEC << endl;

	// 出力ファイルの書き出し
	if ( outftype == 1 ) {
		// OBJファイルの出力
		if ( WriteOBJFile( outdir, outfile, outext, vertList, faceList, mtlList, colF, nrmF ) != true ) {
			g_ofs << "[ERROR] OBJファイルの出力エラー" << endl; 
			goto RTN;
		}
	}
	else if ( outftype == 2 ) {
		// VRMLファイルの出力
		string outpath = arg_outfile;
		if ( WriteVRMLFile( outpath, vertList, faceList, mtlList, colF, nrmF) != true ) {
			g_ofs << "[ERROR] VRMLファイルの出力エラー" << endl; 
			goto RTN;
		}
	}
	g_ofs << "ファイル出力終了[sec]     : " << (clock() - g_Start)/CLOCKS_PER_SEC << endl;
	cout << "処理正常終了" << endl;

	status = 0;

	goto RTN;

USAGE:
	g_ofs << "[ERROR] 入力引数が不正です。" << endl;
	Usage();
	goto RTN;

RTN:
	g_ofs.close();

	return status;
}

