class CVector3D{
public:
	DOUBLE	m_x;
	DOUBLE	m_y;
	DOUBLE	m_z;

	// �R���X�g���N�^
	CVector3D(){}
	CVector3D( DOUBLE x, DOUBLE y, DOUBLE z) {this->m_x = x; this->m_y = y; this->m_z = z; }

	// �f�X�g���N�^
	~CVector3D(){}

	//�R�s�[�R���X�g���N�^
	CVector3D( const CVector3D& v ) { m_x = v.m_x; m_y = v.m_y; m_z = v.m_z; }

	//�x�N�g�������Z( this + v )
	CVector3D operator + ( const CVector3D& v ) const { return CVector3D( m_x + v.m_x, m_y + v.m_y, m_z + v.m_z ); }

	//�x�N�g�������Z( this - v )
	CVector3D operator - ( const CVector3D& v ) const { return CVector3D( m_x - v.m_x, m_y - v.m_y, m_z - v.m_z ); }
	
	//�x�N�g���O��( this �~ vr )
	CVector3D operator * ( const CVector3D& vr ) const {
		return CVector3D( (m_y * vr.m_z) - (m_z * vr.m_y), (m_z * vr.m_x) - (m_x * vr.m_z), (m_x * vr.m_y) - (m_y * vr.m_x) );
	}
	
	//���g��P�ʃx�N�g���ɂ���
	bool Normalize() {
		DOUBLE length = pow( (DOUBLE)(( m_x * m_x ) + ( m_y * m_y ) + ( m_z * m_z )), (DOUBLE)0.5 );//�x�N�g���̒���
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

	//���]����
	void Reverse() {
		m_x = -m_x;
		m_y = -m_y;
		m_z = -m_z;
	}

	// �������擾
	DOUBLE GetLength() {
		return pow( ( m_x * m_x ) + ( m_y * m_y ) + ( m_z * m_z ), (DOUBLE)0.5 );
	}

	// ���ς��v�Z
	DOUBLE DotProduct(CVector3D v) {
		return m_x * v.m_x + m_y * v.m_y + m_z * v.m_z;
	}

	// COS���v�Z
	DOUBLE GetCOS(CVector3D v) {
		return DotProduct(v) / (GetLength()*v.GetLength());
	}

};
