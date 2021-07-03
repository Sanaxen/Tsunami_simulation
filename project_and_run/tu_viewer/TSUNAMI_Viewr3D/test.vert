//普通のシェーディング

void main(void)
{
  vec3 position = vec3(gl_ModelViewMatrix * gl_Vertex);
  vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
  vec3 light = normalize(gl_LightSource[0].position.xyz - position);
   float diffuse = dot(light, normal);
  
  gl_FrontColor = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * diffuse;
  
  gl_Position = ftransform();
}
 