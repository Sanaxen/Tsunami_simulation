#ifndef _GLSLOBJ_HPP
#define _GLSLOBJ_HPP

#include "GL/glut.h"
#include "GL/glext.h"

#include "glsl.h"
#include <stdio.h>

class GLSLObj
{
private:
	/* �V�F�[�_�v���O�����̃R���p�C���^�����N���ʂ𓾂�ϐ� */
	GLint compiled, linked;

public:
	// �V�F�[�_�I�u�W�F�N�g
	GLuint vertShader;
	GLuint fragShader;
	GLuint gl2Program;


	GLSLObj()
	{
		compiled = 0;
		linked = 0;

		/* GLSL �̏����� */
		if (glslInit())
		{
			printf("GLSL���g�p�ł�����ł͂���܂���\n");
			exit(1);
		}

		/* �V�F�[�_�I�u�W�F�N�g�̍쐬 */
		vertShader = glCreateShader(GL_VERTEX_SHADER);
		fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	}

	~GLSLObj()
	{
		glDeleteProgram(gl2Program);
	}

	/* �V�F�[�_�̃\�[�X�v���O�����̓ǂݍ��� */
	int LoadShader(char* ShaderSource);

	/* �V�F�[�_�̃\�[�X�v���O�����̃R���p�C�� */
	int CompileShader();

	/* �V�F�[�_�I�u�W�F�N�g�̃V�F�[�_�v���O�����ւ̓o�^ */
	void AttachShader();

	/* �V�F�[�_�I�u�W�F�N�g�̍폜 */
	void DeleteShader()
	{
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
	}

	/* �V�F�[�_�v���O�����̃����N */
	int LinkProgram();

	/* �V�F�[�_�v���O�����̓K�p */
	void UseProgram()
	{
		glUseProgram(gl2Program);
		//printf("�V�F�[�_�v���O������ON\n");
	}
	void UseProgramOff()
	{
		glUseProgram(0);
		//printf("�V�F�[�_�v���O������OFF\n");
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
