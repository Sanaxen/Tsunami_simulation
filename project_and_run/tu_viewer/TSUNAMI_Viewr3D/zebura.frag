varying vec3 r;  // �����̔��˃x�N�g��
 
void main(void)
{
  gl_FragColor = vec4(step(0.5, fract(2.0 * r.z / r.y)));
}
