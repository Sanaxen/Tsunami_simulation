#include "vob.hpp"


PFNGLGENBUFFERSPROC		glGenBuffers;
PFNGLISBUFFERPROC		glIsBuffer;
PFNGLBINDBUFFERPROC		glBindBuffer;
PFNGLBUFFERDATAPROC		glBufferData;
PFNGLBUFFERSUBDATAPROC	glBufferSubData;
PFNGLMAPBUFFERPROC		glMapBuffer;
PFNGLUNMAPBUFFERPROC	glUnmapBuffer;
PFNGLDELETEBUFFERSPROC	glDeleteBuffers;

VOBobject::VOBobject()
{
	glGenBuffers    = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glIsBuffer      =(PFNGLISBUFFERPROC)wglGetProcAddress("glIsBuffer");
	glBindBuffer    = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData    =(PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
	glMapBuffer     =	(PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer");
	glUnmapBuffer   = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");
	glDeleteBuffers =	(PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
}
