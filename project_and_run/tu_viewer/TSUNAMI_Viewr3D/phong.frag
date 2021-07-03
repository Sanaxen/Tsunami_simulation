// fragment shader of phong shader

#define USE_REFLECTION (0)      /* スペキュラー計算に反射ベクトルを使用 */

varying vec3 position;
varying vec3 normal;

void main (void)
{
	vec3 fnormal = normalize(normal);
	vec3 lightVec = gl_LightSource[0].position.xyz - position;
	
	/* 光源までの距離 */
	float dis = length(lightVec);
	
	lightVec = normalize(lightVec);
	
	/* 減衰係数 */
	float attenuation = 1.0 / (gl_LightSource[0].constantAttenuation +
	                           gl_LightSource[0].linearAttenuation * dis +
	                           gl_LightSource[0].quadraticAttenuation * dis * dis);

	/* ディフューズ */
	float diffuse = dot(lightVec, fnormal);

	/* アンビエント */
	gl_FragColor = gl_FrontLightProduct[0].ambient;

	if (diffuse > 0.0)
	{
#if USE_REFLECTION
		/* 反射ベクトル */
		vec3 viewVec = normalize(-position);
		vec3 reflectVec = reflect(-lightVec, fnormal);
		float specular = pow(max(dot(viewVec, reflectVec), 0.0), gl_FrontMaterial.shininess);
#else
	    /* ハーフベクトル */
		vec3 viewVec = normalize(-position);
		vec3 halfVec = normalize(lightVec + viewVec);
		float specular = pow(max(dot(fnormal, halfVec), 0.0), gl_FrontMaterial.shininess);
#endif
		gl_FragColor += gl_FrontLightProduct[0].diffuse * diffuse * attenuation
					  + gl_FrontLightProduct[0].specular * specular * attenuation;
	}
}
