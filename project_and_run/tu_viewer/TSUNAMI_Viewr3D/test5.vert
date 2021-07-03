/*!
  @file phong.vs
	
  @brief GLSLフラグメントシェーダ
	- フォンシェーディング
 
  @author Makoto Fujisawa
  @date 2011
*/
#version 120

// フラグメントシェーダに値を渡すための変数
varying vec3 vPos;
varying vec3 vNrm;

void main(void)
{
	// 頂点位置と法線
	vPos = (gl_ModelViewMatrix*gl_Vertex).xyz;
	vNrm = gl_NormalMatrix*gl_Normal;

	// 描画頂点位置
	gl_Position = ftransform();
}
