/*!
  @file cook_torrance.fs
	
  @brief GLSLtOgVF[_
	- Cook-TorranceΎΚ½Λf
 
  @author Makoto Fujisawa
  @date 2011
*/
#version 120

uniform float m;
uniform float refrac;

// o[ebNXVF[_©ησ―ζιΟ
varying vec3 vPos;
varying vec3 vNrm;

void main(void)
{
	// υΉ
	vec3 La = gl_LightSource[0].ambient.xyz;	// CgΒ«υ
	vec3 Ld = gl_LightSource[0].diffuse.xyz;	// CggU½Λυ
	vec3 Ls = gl_LightSource[0].specular.xyz;	// CgΎΚ½Λυ
	vec3 Lp = gl_LightSource[0].position.xyz;	// CgΚu

	// ήΏ
	vec3 Ke = gl_FrontMaterial.emission.xyz;	// ϊΛF
	vec3 Ka = gl_FrontMaterial.ambient.xyz;		// Β«υ
	vec3 Kd = gl_FrontMaterial.diffuse.xyz;		// gU½Λ
	vec3 Ks = gl_FrontMaterial.specular.xyz;	// ΎΚ½Λ
	float shine = gl_FrontMaterial.shininess;

	vec3 V = normalize(-vPos.xyz);		// όxNg
	vec3 N = normalize(vNrm);			// @όxNg
	vec3 L = normalize(Lp-vPos.xyz);	// CgxNg
	vec3 H = normalize(L+V);			// n[txNg


	// ϊΛFΜvZ
	vec3 emissive = Ke;

	// Β«υΜvZ
	vec3 ambient = Ka*La;	// gl_FrontLightProduct[0].ambentΕu«·¦Β\

	// gU½ΛΜvZ
	float diffuseLight = max(dot(L, N), 0.0);
	vec3 diffuse = Kd*Ld*diffuseLight;

	// ΎΚ½ΛΜvZ
	vec3 specular = vec3(0.0);
	if(diffuseLight > 0.0){
		// Cook-Torrance½ΛfΜvZ

		float NH = dot(N, H);
		float VH = dot(V, H);
		float NV = dot(N, V);
		float NL = dot(N, L);

		float alpha = acos(NH);

		// D:xbN}ͺzΦ
		float D = (1.0/(4*m*m*NH*NH*NH*NH))*exp((NH*NH-1)/(m*m*NH*NH));

		// G:τ½Έ
		float G = min(1, min((2*NH*NV)/VH, (2*NH*NL)/VH));

		// F:tl
		float c = VH;
		float g = sqrt(refrac*refrac+c*c-1);
		float F = ((g-c)*(g-c)/((g+c)*(g+c)))*(1+(c*(g+c)-1)*(c*(g+c)-1)/((c*(g-c)-1)*(c*(g-c)-1)));

		float specularLight = D*G*F/NV;
		specular = Ks*Ls*specularLight;
	}

	gl_FragColor.xyz = emissive+ambient+diffuse+specular;
	gl_FragColor.w = 1.0;
}
