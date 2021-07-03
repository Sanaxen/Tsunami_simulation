varying vec3 r;  // �����̔��˃x�N�g��
 
void main(void)
{
  vec4 p = gl_ModelViewMatrix * gl_Vertex;  // ���_�ʒu
  vec3 v = p.xyz / p.w;                     // �����x�N�g��
  vec3 n = gl_NormalMatrix * gl_Normal;     // �@���x�N�g��
  r = vec3(gl_TextureMatrix[0] * vec4(reflect(v, n), 1.0));
  gl_Position = ftransform();
}