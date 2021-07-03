#include "glslobj.hpp"

#include <string.h>

int GLSLObj::LoadShader(char* ShaderSource)
{
	char vert[512];
	char frag[512];

	strcpy(vert, ShaderSource);
	strcat(vert, ".vert");

	strcpy(frag, ShaderSource);
	strcat(frag, ".frag");

	/* シェーダのソースプログラムの読み込み */
	if (readShaderSource(vertShader, vert)) return -1;
	if (readShaderSource(fragShader, frag)) return -1;
	
	printf("LoadShader OK!!\n");
	return 0;
}


int GLSLObj::CompileShader()
{
	/* バーテックスシェーダのソースプログラムのコンパイル */
	glCompileShader(vertShader);
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
	printShaderInfoLog(vertShader);
	
	if (compiled == GL_FALSE)
	{
		return -1;
	}
  
	/* フラグメントシェーダのソースプログラムのコンパイル */
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
	printShaderInfoLog(fragShader);

	if (compiled == GL_FALSE) 
	{
		return -1;
	}
	printf("CompileShader OK!!\n");

	return 0;
}


void GLSLObj::AttachShader()
{
	gl2Program = glCreateProgram();
	glAttachShader(gl2Program, vertShader);
	glAttachShader(gl2Program, fragShader);
}

int GLSLObj::LinkProgram()
{
	glLinkProgram(gl2Program);
	glGetProgramiv(gl2Program, GL_LINK_STATUS, &linked);
	printProgramInfoLog(gl2Program);
	
	if (linked == GL_FALSE)
	{
		printf("シェーダのリンクエラー\n");
		return -1;
	}
	printf("LinkProgram OK!!\n");

	return 0;  
}
