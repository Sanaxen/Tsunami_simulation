/*!
  @file phong.fs
	
  @brief GLSL�t���O�����g�V�F�[�_
	- �t�H���V�F�[�f�B���O
 
  @author Makoto Fujisawa
  @date 2011
*/
#version 120

// �o�[�e�b�N�X�V�F�[�_����󂯎��ϐ�
varying vec3 vPos;
varying vec3 vNrm;

void main(void)
{
	// ����
	vec3 La = gl_LightSource[0].ambient.xyz;	// ���C�g����
	vec3 Ld = gl_LightSource[0].diffuse.xyz;	// ���C�g�g�U���ˌ�
	vec3 Ls = gl_LightSource[0].specular.xyz;	// ���C�g���ʔ��ˌ�
	vec3 Lp = gl_LightSource[0].position.xyz;	// ���C�g�ʒu

	// �ގ�
	vec3 Ke = gl_FrontMaterial.emission.xyz;	// ���ːF
	vec3 Ka = gl_FrontMaterial.ambient.xyz;		// ����
	vec3 Kd = gl_FrontMaterial.diffuse.xyz;		// �g�U����
	vec3 Ks = gl_FrontMaterial.specular.xyz;	// ���ʔ���
	float shine = gl_FrontMaterial.shininess;

	vec3 V = normalize(-vPos.xyz);		// �����x�N�g��
	vec3 N = normalize(vNrm);			// �@���x�N�g��
	vec3 L = normalize(Lp-vPos.xyz);	// ���C�g�x�N�g��

	// ���ːF�̌v�Z
	vec3 emissive = Ke;

	// �����̌v�Z
	vec3 ambient = Ka*La;	// gl_FrontLightProduct[0].ambent�Œu�������\

	// �g�U���˂̌v�Z
	float diffuseLight = max(dot(L, N), 0.0);
	vec3 diffuse = Kd*Ld*diffuseLight;

	// ���ʔ��˂̌v�Z
	vec3 specular = vec3(0.0);
	if(diffuseLight > 0.0){
		// �t�H�����˃��f��
		vec3 R = reflect(-L, N);
		//vec3 R = -L+2*dot(N, L)*N;	// reflect�֐���p���Ȃ��ꍇ
		float specularLight = pow(max(dot(R, N), 0.0), shine);

		// �n�[�t�x�N�g���ɂ�锽��(Blinn-Phong)
		//vec3 H = normalize(L+V);
		//float specularLight = pow(max(dot(H, N), 0.0), shine);

		specular = Ks*Ls*specularLight;
	}

	gl_FragColor.xyz = emissive+ambient+diffuse+specular;
	gl_FragColor.w = 1.0;
}
