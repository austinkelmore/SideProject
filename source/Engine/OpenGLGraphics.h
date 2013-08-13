
#pragma once

#include "IGraphics.h"

#ifdef WIN32
#include "WindowsPlatform.h"
#endif

#include <gl\gl.h>
#include "opengl\glext.h"
#include "opengl\wglext.h"

#include <string>

class OpenGLGraphics : public IGraphics
{
public:
	OpenGLGraphics();
	virtual ~OpenGLGraphics();

	virtual bool Init();
	virtual void Update();
	virtual void Destroy();

	bool SetupShaders();
	bool CompileShader(const std::string& shader_path, const GLenum shader_type, unsigned int& o_shader);

#ifdef WIN32
	bool PreInitializeWindowsOpenGL(HWND window);
#endif // WIN32

private:
	bool LoadExtensions();
	int ChoosePixelFormat(HDC device_context);

#ifdef WIN32
	HDC _device_context;
#endif // WIN32
	HGLRC _rendering_context;

	// shader information
	unsigned int _shader_program;
	unsigned int _vertex_shader;
	unsigned int _fragment_shader;

	// vertex buffer information
	unsigned int _vertex_array;
	unsigned int _vertex_buffer;
	unsigned int _index_buffer;

	// extensions
	PFNWGLCHOOSEPIXELFORMATARBPROC _wglChoosePixelFormatARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC _wglCreateContextAttribsARB;
	PFNWGLSWAPINTERVALEXTPROC _wglSwapIntervalEXT;

	PFNGLATTACHSHADERPROC _glAttachShader;
	PFNGLBINDBUFFERPROC _glBindBuffer;
	PFNGLBINDVERTEXARRAYPROC _glBindVertexArray;
	PFNGLBUFFERDATAPROC _glBufferData;
	PFNGLCOMPILESHADERPROC _glCompileShader;
	PFNGLCREATEPROGRAMPROC _glCreateProgram;
	PFNGLCREATESHADERPROC _glCreateShader;
	PFNGLDEBUGMESSAGECALLBACKARBPROC _glDebugMessageCallbackARB;
	PFNGLDELETEBUFFERSPROC _glDeleteBuffers;
	PFNGLDELETEPROGRAMPROC _glDeleteProgram;
	PFNGLDELETESHADERPROC _glDeleteShader;
	PFNGLDELETEVERTEXARRAYSPROC _glDeleteVertexArrays;
	PFNGLDETACHSHADERPROC _glDetachShader;
	PFNGLENABLEVERTEXATTRIBARRAYPROC _glEnableVertexAttribArray;
	PFNGLGENBUFFERSPROC _glGenBuffers;
	PFNGLGENVERTEXARRAYSPROC _glGenVertexArrays;
	PFNGLGETATTRIBLOCATIONPROC _glGetAttribLocation;
	PFNGLGETPROGRAMINFOLOGPROC _glGetProgramInfoLog;
	PFNGLGETPROGRAMIVPROC _glGetProgramiv;
	PFNGLGETSHADERINFOLOGPROC _glGetShaderInfoLog;
	PFNGLGETSHADERIVPROC _glGetShaderiv;
	PFNGLLINKPROGRAMPROC _glLinkProgram;
	PFNGLSHADERSOURCEPROC _glShaderSource;
	PFNGLUSEPROGRAMPROC _glUseProgram;
	PFNGLVERTEXATTRIBPOINTERPROC _glVertexAttribPointer;
	PFNGLBINDATTRIBLOCATIONPROC _glBindAttribLocation;
	PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation;
	PFNGLUNIFORMMATRIX4FVPROC _glUniformMatrix4fv;
	PFNGLACTIVETEXTUREPROC _glActiveTexture;
	PFNGLUNIFORM1IPROC _glUniform1i;
	PFNGLGENERATEMIPMAPPROC _glGenerateMipmap;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC _glDisableVertexAttribArray;
	PFNGLUNIFORM3FVPROC _glUniform3fv;
	PFNGLUNIFORM4FVPROC _glUniform4fv;
};
