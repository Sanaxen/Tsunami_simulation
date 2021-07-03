// phong.vert

varying vec3 position;
varying vec3 normal;

void main(void)
{
	position = vec3(gl_ModelViewMatrix * gl_Vertex);
	normal = normalize(gl_NormalMatrix * gl_Normal);

	gl_Position = ftransform();
}
