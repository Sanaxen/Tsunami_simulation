varying vec3 normal;

void main()
{
	vec3 color = vec3(normalize(normal));
	color = color * 0.5 + 0.5;
	gl_FragColor = vec4(color, 1.0);
}