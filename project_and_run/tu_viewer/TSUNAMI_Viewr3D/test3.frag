uniform samplerCube cubemap;
 
varying vec3 r;  // ‹ü‚Ì”½ËƒxƒNƒgƒ‹
 
void main(void)
{
  gl_FragColor = textureCube(cubemap, r);
}