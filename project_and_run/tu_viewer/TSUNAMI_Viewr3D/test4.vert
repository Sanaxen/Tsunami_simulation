/*!
  @file phong.vs
	
  @brief GLSL�t���O�����g�V�F�[�_
	- �t�H���V�F�[�f�B���O
 
  @author Makoto Fujisawa
  @date 2011
*/
#version 120

// �t���O�����g�V�F�[�_�ɒl��n�����߂̕ϐ�
varying vec3 vPos;
varying vec3 vNrm;

void main(void)
{
	// ���_�ʒu�Ɩ@��
	vPos = (gl_ModelViewMatrix*gl_Vertex).xyz;
	vNrm = gl_NormalMatrix*gl_Normal;

	// �`�撸�_�ʒu
	gl_Position = ftransform();
}
