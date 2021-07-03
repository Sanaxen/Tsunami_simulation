// マテリアル情報
class CMaterial{
public:
	// マテリアル名
	std::string	m_mtlname;

	// アンビエントカラー
	Color_rgb	m_Ka;
	// Kaセット済みフラグ
	bool		m_setKa;

	// ディフューズカラー（物体の色）
	Color_rgb	m_Kd;
	// Kdセット済みフラグ
	bool		m_setKd;

	// スペキュラーカラー（ハイライトの色）
	Color_rgb	m_Ks;
	// Ksセット済みフラグ
	bool		m_setKs;

	// 反射の強さ
	DOUBLE		m_Ns;
	// Nsセット済みフラグ
	bool		m_setNs;

	// アルファ値（透明度）
	DOUBLE		m_Tr;
	// Trセット済みフラグ
	bool		m_setTr;

	// コンストラクタ
	CMaterial(){}
	CMaterial( TCHAR* mtlname ) {
		InitParam();
		m_mtlname = mtlname;
	}
	CMaterial( std::string mtlname ) {
		InitParam();
		m_mtlname = mtlname;
	}

	// デストラクタ
	~CMaterial(){}

	// コピーコンストラクタ
	CMaterial( const CMaterial& m ) {
		m_mtlname = m.m_mtlname;
		m_Ka = m.m_Ka;
		m_setKa = m.m_setKa;
		m_Kd = m.m_Kd;
		m_setKd = m.m_setKd;
		m_Ks = m.m_Ks;
		m_setKs = m.m_setKs;
		m_Ns = m.m_Ns;
		m_setNs = m.m_setNs;
		m_Tr = m.m_Tr;
		m_setTr = m.m_setTr;
	}

	void InitParam() {
		m_mtlname = "";
		m_Ka.r = 0.0; m_Ka.g = 0.0; m_Ka.b = 0.0;
		m_setKa = false;
		m_Kd.r = 0.0; m_Kd.g = 0.0; m_Kd.b = 0.0;
		m_setKd = false;
		m_Ks.r = 0.0; m_Ks.g = 0.0; m_Ks.b = 0.0;
		m_setKs = false;
		m_Ns = 0.0;
		m_setNs = false;
		m_Tr = 0.0;
		m_setTr = false;
	}
};
