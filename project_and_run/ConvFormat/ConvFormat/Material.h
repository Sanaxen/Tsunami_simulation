// �}�e���A�����
class CMaterial{
public:
	// �}�e���A����
	std::string	m_mtlname;

	// �A���r�G���g�J���[
	Color_rgb	m_Ka;
	// Ka�Z�b�g�ς݃t���O
	bool		m_setKa;

	// �f�B�t���[�Y�J���[�i���̂̐F�j
	Color_rgb	m_Kd;
	// Kd�Z�b�g�ς݃t���O
	bool		m_setKd;

	// �X�y�L�����[�J���[�i�n�C���C�g�̐F�j
	Color_rgb	m_Ks;
	// Ks�Z�b�g�ς݃t���O
	bool		m_setKs;

	// ���˂̋���
	DOUBLE		m_Ns;
	// Ns�Z�b�g�ς݃t���O
	bool		m_setNs;

	// �A���t�@�l�i�����x�j
	DOUBLE		m_Tr;
	// Tr�Z�b�g�ς݃t���O
	bool		m_setTr;

	// �R���X�g���N�^
	CMaterial(){}
	CMaterial( TCHAR* mtlname ) {
		InitParam();
		m_mtlname = mtlname;
	}
	CMaterial( std::string mtlname ) {
		InitParam();
		m_mtlname = mtlname;
	}

	// �f�X�g���N�^
	~CMaterial(){}

	// �R�s�[�R���X�g���N�^
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
