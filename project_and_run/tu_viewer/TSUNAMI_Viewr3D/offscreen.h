#ifndef _OFFSCREEN

#define _OFFSCREEN

#include "GL/glew.h"
#include "GL/gl.h"

class OffScreenRender
{
	GLuint	texture_name;
	GLuint	renderbuffer_name;
	GLuint	framebuffer_name;
	GLint	width;
	GLint	height;


	void InitTexture( void )
	{
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glGenTextures( 1, &texture_name );
		glBindTexture( GL_TEXTURE_2D, texture_name );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height,  0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
	}

	void InitRenderbuffer( void )
	{
		glGenRenderbuffersEXT( 1, &renderbuffer_name );
		glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, renderbuffer_name );
		glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,  width, height );
	}

	void InitFramebuffer( void )
	{
		glGenFramebuffersEXT( 1, &framebuffer_name );
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, framebuffer_name );

		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
								   GL_TEXTURE_2D, texture_name, 0 );
		glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
									  GL_RENDERBUFFER_EXT, renderbuffer_name );

		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	}
public:

	OffScreenRender(void (* callback)( void ), int viewport[4])
	{
		width = viewport[2];
		height = viewport[3];
		InitTexture();
		InitRenderbuffer();
		InitFramebuffer();


		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, framebuffer_name );

		if ( glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT )
			 != GL_FRAMEBUFFER_COMPLETE_EXT ) {
		  printf("framebuffer is not complete\n");
		}
		if ( callback ) callback();
	}

	~OffScreenRender()
	{
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
		glDeleteTextures(1, &texture_name);
		glDeleteFramebuffersEXT(1, &framebuffer_name);
		glDeleteRenderbuffersEXT(1, &renderbuffer_name);
	}

};

#endif