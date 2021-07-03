// vertex shader of phong shader

varying vec3 position;
varying vec3 normal;

void main(void)
{
	/* ‹“_À•W‚ÅŒõŒ¹ŒvZ */
	position = vec3(gl_ModelViewMatrix * gl_Vertex);
	normal = gl_NormalMatrix * gl_Normal;
  
	gl_Position = ftransform();
}
