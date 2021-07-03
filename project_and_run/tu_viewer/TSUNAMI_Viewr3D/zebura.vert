varying vec3 r;  // 視線の反射ベクトル
 
void main(void)
{
  vec4 p = gl_ModelViewMatrix * gl_Vertex;  // 頂点位置
  vec3 v = p.xyz / p.w;                     // 視線ベクトル
  vec3 n = gl_NormalMatrix * gl_Normal;     // 法線ベクトル
  r = vec3(gl_TextureMatrix[0] * vec4(reflect(v, n), 1.0));
  gl_Position = ftransform();
}
