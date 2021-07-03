// refract.frag
uniform samplerCube cubemap;
 
varying vec3 r;  // �����̔��˃x�N�g��
varying vec3 s;  // �����̋��܃x�N�g��
varying float t; // ���E�ʂł̔��˗�
 
void main(void)
{
  gl_FragColor = mix(textureCube(cubemap, s), textureCube(cubemap, r), t);
}