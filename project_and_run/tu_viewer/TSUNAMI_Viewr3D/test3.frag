uniform samplerCube cubemap;
 
varying vec3 r;  // �����̔��˃x�N�g��
 
void main(void)
{
  gl_FragColor = textureCube(cubemap, r);
}