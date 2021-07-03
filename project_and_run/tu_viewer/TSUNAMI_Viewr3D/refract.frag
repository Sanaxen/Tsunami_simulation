// refract.frag
uniform samplerCube cubemap;
 
varying vec3 r;  // 視線の反射ベクトル
varying vec3 s;  // 視線の屈折ベクトル
varying float t; // 境界面での反射率
 
void main(void)
{
  gl_FragColor = mix(textureCube(cubemap, s), textureCube(cubemap, r), t);
}