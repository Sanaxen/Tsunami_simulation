
// 面（三角形）情報
class CTriangle{
public:
	// 面インデックス
	size_t		m_idx;
	// 無効フラグ
	bool		m_inv;

	// 頂点インデックス
	long		m_vidx[3];

	// 法線ベクトルインデックス
	long		m_nidx[3];

	// 法線ベクトル
	CVector3D	m_nrm;

	// マテリアル名
	std::string	m_mtlname;

	// 面の色
	Color_rgb	m_color;
	// 色セット済みフラグ
	bool m_setrgb;

	// コンストラクタ
	CTriangle(){}
	CTriangle( size_t idx, long v1, long v2, long v3 ) {
		InitParam();
		m_idx = idx;
		m_vidx[0] = v1; m_vidx[1] = v2; m_vidx[2] = v3;
	}
	CTriangle( size_t idx, long v1, long v2, long v3, std::string mtlname ) {
		InitParam();
		m_idx = idx;
		m_vidx[0] = v1; m_vidx[1] = v2; m_vidx[2] = v3;
		m_mtlname = mtlname;
	}
	CTriangle( size_t idx, long v1, long v2, long v3, long n1, long n2, long n3, std::string mtlname ) {
		InitParam();
		m_idx = idx;
		m_vidx[0] = v1; m_vidx[1] = v2; m_vidx[2] = v3;
		m_nidx[0] = n1; m_nidx[1] = n2; m_nidx[2] = n3;
		m_mtlname = mtlname;
	}
	CTriangle( size_t idx, long v1, long v2, long v3, DOUBLE r, DOUBLE g, DOUBLE b )	{
		InitParam();
		m_idx = idx;
		m_vidx[0] = v1; m_vidx[1] = v2; m_vidx[2] = v3;
		SetColor( r, g, b );
	}

	// デストラクタ
	~CTriangle(){}

	// コピーコンストラクタ
	CTriangle( const CTriangle& f ) {
		m_idx = f.m_idx;
		m_inv = f.m_inv;
		m_vidx[0] = f.m_vidx[0]; m_vidx[1] = f.m_vidx[1]; m_vidx[2] = f.m_vidx[2];
		m_nidx[0] = f.m_nidx[0]; m_nidx[1] = f.m_nidx[1]; m_nidx[2] = f.m_nidx[2];
		m_nrm = f.m_nrm;
		m_mtlname = f.m_mtlname;
		m_color = f.m_color;
		m_setrgb = f.m_setrgb;
	}

	void InitParam() {
		m_idx = 0;
		m_inv = false;
		m_vidx[0] = -1; m_vidx[1] = -1; m_vidx[2] = -1;
		m_nidx[0] = -1; m_nidx[1] = -1; m_nidx[2] = -1;
		m_nrm.m_x = 0.0; m_nrm.m_y = 0.0; m_nrm.m_z = 0.0;
		m_mtlname = "";
		m_color.r = 0.0; m_color.g = 0.0; m_color.b = 0.0;
		m_setrgb = false;
	}

	void SetColor( DOUBLE r, DOUBLE g, DOUBLE b ) {
		if ( r > 1.0 || g > 1.0 || b > 1.0 ) {
			r /= MAXRGB; g /= MAXRGB; b /= MAXRGB;
		}
		m_color.r = r; m_color.g = g; m_color.b = b;
		m_setrgb = true;
	}

};
