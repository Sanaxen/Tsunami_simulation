// refract.vert

varying vec3 r;  // 視線の反射ベクトル
varying vec3 s;  // 視線の屈折ベクトル
varying float t; // 境界面での反射率
 
const float eta = 0.67;  // 屈折率の比
const float f = (1.0 - eta) * (1.0 - eta) / ((1.0 + eta) * (1.0 + eta));
 
void main(void)
{
  vec4 p = gl_ModelViewMatrix * gl_Vertex;  // 頂点位置
  vec3 v = normalize(p.xyz / p.w);          // 視線ベクトル
  vec3 n = gl_NormalMatrix * gl_Normal;     // 法線ベクトル
  r = vec3(gl_TextureMatrix[0] * vec4(reflect(v, n), 1.0));
  s = vec3(gl_TextureMatrix[0] * vec4(refract(v, n, eta), 1.0));
  t = f + (1.0 - f) * pow(1.0 - dot(-v, n), 5.0);
  gl_Position = ftransform();
}
