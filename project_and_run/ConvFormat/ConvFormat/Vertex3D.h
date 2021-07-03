
#define MAXRGB	(255)

class CVertex3D{
public:
	// ���_�C���f�b�N�X
	size_t		m_idx;
	// �����t���O
	bool		m_inv;

	// ���W�l
	DOUBLE		m_x;
	DOUBLE		m_y;
	DOUBLE		m_z;

	// �_�̐F
	Color_rgb	m_color;

	// �F�Z�b�g�ς݃t���O
	bool		m_setrgb;

	// ���_�@���x�N�g��
	CVector3D	m_nrm;

	// �����ʃ��X�g
	std::vector<size_t> m_FidxList;

	// �R���X�g���N�^
	CVertex3D(){}
	CVertex3D( size_t idx, DOUBLE x, DOUBLE y, DOUBLE z) {
		InitParam();
		m_idx = idx;
		m_x = x; m_y = y; m_z = z;
	}
	CVertex3D( size_t idx, DOUBLE x, DOUBLE y, DOUBLE z, DOUBLE r, DOUBLE g, DOUBLE b )	{
		InitParam();
		m_idx = idx;
		m_x = x; m_y = y; m_z = z;
		SetColor( r, g, b );
	}

	// �f�X�g���N�^
	~CVertex3D(){}

	// �R�s�[�R���X�g���N�^
	CVertex3D( const CVertex3D& v ) {
		m_idx = v.m_idx;
		m_inv = v.m_inv;
		m_x = v.m_x; m_y = v.m_y; m_z = v.m_z;
		m_color = v.m_color;
		m_setrgb = v.m_setrgb;
		m_nrm = v.m_nrm;
		m_FidxList = v.m_FidxList;
	}

	void InitParam() {
		m_idx = 0;
		m_inv = false;
		m_x = 0.0; m_y = 0.0; m_z = 0.0;
		m_color.r = 0.0; m_color.g = 0.0; m_color.b = 0.0;
		m_setrgb = false;
		m_nrm.m_x = 0.0; m_nrm.m_y = 0.0; m_nrm.m_z = 0.0;
		m_FidxList.clear();
	}

	void SetColor( DOUBLE r, DOUBLE g, DOUBLE b ) {
		if ( r > 1.0 || g > 1.0 || b > 1.0 ) {
			r /= MAXRGB; g /= MAXRGB; b /= MAXRGB;
		}
		m_color.r = r; m_color.g = g; m_color.b = b;
		m_setrgb = true;
	}

};
