/*!
  @file cook_torrance.fs
	
  @brief GLSLフラグメントシェーダ
	- Cook-Torrance鏡面反射モデル
 
  @author Makoto Fujisawa
  @date 2011
*/
#version 120

uniform float m;
uniform float refrac;

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
	vec3 H = normalize(L+V);			// ハーフベクトル


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
		// Cook-Torrance反射モデルの計算

		float NH = dot(N, H);
		float VH = dot(V, H);
		float NV = dot(N, V);
		float NL = dot(N, L);

		float alpha = acos(NH);

		// D:ベックマン分布関数
		float D = (1.0/(4*m*m*NH*NH*NH*NH))*exp((NH*NH-1)/(m*m*NH*NH));

		// G:幾何減衰
		float G = min(1, min((2*NH*NV)/VH, (2*NH*NL)/VH));

		// F:フレネル項
		float c = VH;
		float g = sqrt(refrac*refrac+c*c-1);
		float F = ((g-c)*(g-c)/((g+c)*(g+c)))*(1+(c*(g+c)-1)*(c*(g+c)-1)/((c*(g-c)-1)*(c*(g-c)-1)));

		float specularLight = D*G*F/NV;
		specular = Ks*Ls*specularLight;
	}

	gl_FragColor.xyz = emissive+ambient+diffuse+specular;
	gl_FragColor.w = 1.0;
}
