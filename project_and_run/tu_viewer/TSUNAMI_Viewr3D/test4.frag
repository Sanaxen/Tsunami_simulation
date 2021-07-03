/*!
  @file phong.fs
	
  @brief GLSLフラグメントシェーダ
	- フォンシェーディング
 
  @author Makoto Fujisawa
  @date 2011
*/
#version 120

// バーテックスシェーダから受け取る変数
varying vec3 vPos;
varying vec3 vNrm;

void main(void)
{
	// 光源
	vec3 La = gl_LightSource[0].ambient.xyz;	// ライト環境光
	vec3 Ld = gl_LightSource[0].diffuse.xyz;	// ライト拡散反射光
	vec3 Ls = gl_LightSource[0].specular.xyz;	// ライト鏡面反射光
	vec3 Lp = gl_LightSource[0].position.xyz;	// ライト位置

	// 材質
	vec3 Ke = gl_FrontMaterial.emission.xyz;	// 放射色
	vec3 Ka = gl_FrontMaterial.ambient.xyz;		// 環境光
	vec3 Kd = gl_FrontMaterial.diffuse.xyz;		// 拡散反射
	vec3 Ks = gl_FrontMaterial.specular.xyz;	// 鏡面反射
	float shine = gl_FrontMaterial.shininess;

	vec3 V = normalize(-vPos.xyz);		// 視線ベクトル
	vec3 N = normalize(vNrm);			// 法線ベクトル
	vec3 L = normalize(Lp-vPos.xyz);	// ライトベクトル

	// 放射色の計算
	vec3 emissive = Ke;

	// 環境光の計算
	vec3 ambient = Ka*La;	// gl_FrontLightProduct[0].ambentで置き換え可能

	// 拡散反射の計算
	float diffuseLight = max(dot(L, N), 0.0);
	vec3 diffuse = Kd*Ld*diffuseLight;

	// 鏡面反射の計算
	vec3 specular = vec3(0.0);
	if(diffuseLight > 0.0){
		// フォン反射モデル
		vec3 R = reflect(-L, N);
		//vec3 R = -L+2*dot(N, L)*N;	// reflect関数を用いない場合
		float specularLight = pow(max(dot(R, N), 0.0), shine);

		// ハーフベクトルによる反射(Blinn-Phong)
		//vec3 H = normalize(L+V);
		//float specularLight = pow(max(dot(H, N), 0.0), shine);

		specular = Ks*Ls*specularLight;
	}

	gl_FragColor.xyz = emissive+ambient+diffuse+specular;
	gl_FragColor.w = 1.0;
}
