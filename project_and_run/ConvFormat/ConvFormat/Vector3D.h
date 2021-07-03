class CVector3D{
public:
	DOUBLE	m_x;
	DOUBLE	m_y;
	DOUBLE	m_z;

	// コンストラクタ
	CVector3D(){}
	CVector3D( DOUBLE x, DOUBLE y, DOUBLE z) {this->m_x = x; this->m_y = y; this->m_z = z; }

	// デストラクタ
	~CVector3D(){}

	//コピーコンストラクタ
	CVector3D( const CVector3D& v ) { m_x = v.m_x; m_y = v.m_y; m_z = v.m_z; }

	//ベクトル足し算( this + v )
	CVector3D operator + ( const CVector3D& v ) const { return CVector3D( m_x + v.m_x, m_y + v.m_y, m_z + v.m_z ); }

	//ベクトル引き算( this - v )
	CVector3D operator - ( const CVector3D& v ) const { return CVector3D( m_x - v.m_x, m_y - v.m_y, m_z - v.m_z ); }
	
	//ベクトル外積( this × vr )
	CVector3D operator * ( const CVector3D& vr ) const {
		return CVector3D( (m_y * vr.m_z) - (m_z * vr.m_y), (m_z * vr.m_x) - (m_x * vr.m_z), (m_x * vr.m_y) - (m_y * vr.m_x) );
	}
	
	//自身を単位ベクトルにする
	bool Normalize() {
		DOUBLE length = pow( (DOUBLE)(( m_x * m_x ) + ( m_y * m_y ) + ( m_z * m_z )), (DOUBLE)0.5 );//ベクトルの長さ
		if ( length > 0.0 ) {
			m_x /= length;
			m_y /= length;
			m_z /= length;
			return true;
		}
		else {
			return false;
		}
	}

	//反転する
	void Reverse() {
		m_x = -m_x;
		m_y = -m_y;
		m_z = -m_z;
	}

	// 長さを取得
	DOUBLE GetLength() {
		return pow( ( m_x * m_x ) + ( m_y * m_y ) + ( m_z * m_z ), (DOUBLE)0.5 );
	}

	// 内積を計算
	DOUBLE DotProduct(CVector3D v) {
		return m_x * v.m_x + m_y * v.m_y + m_z * v.m_z;
	}

	// COSを計算
	DOUBLE GetCOS(CVector3D v) {
		return DotProduct(v) / (GetLength()*v.GetLength());
	}

};
