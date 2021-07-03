#ifndef _GLSLOBJ_HPP
#define _GLSLOBJ_HPP

#include "GL/glut.h"
#include "GL/glext.h"

#include "glsl.h"
#include <stdio.h>

class GLSLObj
{
private:
	/* シェーダプログラムのコンパイル／リンク結果を得る変数 */
	GLint compiled, linked;

public:
	// シェーダオブジェクト
	GLuint vertShader;
	GLuint fragShader;
	GLuint gl2Program;


	GLSLObj()
	{
		compiled = 0;
		linked = 0;

		/* GLSL の初期化 */
		if (glslInit())
		{
			printf("GLSLを使用できる環境ではありません\n");
			exit(1);
		}

		/* シェーダオブジェクトの作成 */
		vertShader = glCreateShader(GL_VERTEX_SHADER);
		fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	}

	~GLSLObj()
	{
		glDeleteProgram(gl2Program);
	}

	/* シェーダのソースプログラムの読み込み */
	int LoadShader(char* ShaderSource);

	/* シェーダのソースプログラムのコンパイル */
	int CompileShader();

	/* シェーダオブジェクトのシェーダプログラムへの登録 */
	void AttachShader();

	/* シェーダオブジェクトの削除 */
	void DeleteShader()
	{
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
	}

	/* シェーダプログラムのリンク */
	int LinkProgram();

	/* シェーダプログラムの適用 */
	void UseProgram()
	{
		glUseProgram(gl2Program);
		//printf("シェーダプログラムのON\n");
	}
	void UseProgramOff()
	{
		glUseProgram(0);
		//printf("シェーダプログラムのOFF\n");
	}
	void SetFloatVar(char* varname, float value)
	{
		glUniform1f(glGetUniformLocation(gl2Program, varname), value);
	}

	int var;
	void MakeVar(char* varname)
	{
		var = glGetUniformLocation(gl2Program, varname);
	}
	void SetVar(int val)
	{
		glUniform1i(var, 0);
	}
};

#endif
