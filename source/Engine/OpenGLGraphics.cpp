
#include "OpenGLGraphics.h"

OpenGLGraphics::OpenGLGraphics()
{
#ifdef WIN32
	_device_context = NULL;
#endif // WIN32
	_rendering_context = NULL;
}

OpenGLGraphics::~OpenGLGraphics()
{
	Destroy();
}

bool OpenGLGraphics::Init()
{
#ifdef WIN32
	_device_context = static_cast<WindowsPlatform*>(g_platform)->GetDC();
	if (!_device_context)
		return false;

	int chosen_pixel_format = ChoosePixelFormat(_device_context);

	PIXELFORMATDESCRIPTOR pixel_format_descriptor;
	int ret_val = SetPixelFormat(_device_context, chosen_pixel_format, &pixel_format_descriptor);
	if (!ret_val)
		return false;

	wglCreateContext(_device_context);

	// set us up to use opengl 4.0
	int attribute_list[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		0,
	};

	_rendering_context = _wglCreateContextAttribsARB(_device_context, 0, attribute_list);
	if (!_rendering_context)
		return false;

	ret_val = wglMakeCurrent(_device_context, _rendering_context);
	if (!ret_val)
		return false;

#endif // WIN32

	// Set the depth buffer to be entirely cleared to 1.0 values.
	glClearDepth(1.0f);

	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);

	// Set the polygon winding to front facing for the left handed system.
	glFrontFace(GL_CW);

	// Enable back face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	return true;
}

#ifdef WIN32
bool OpenGLGraphics::PreInitializeWindowsOpenGL(HWND window)
{
	HDC device_context = GetDC(window);
	if (!device_context)
		return false;

	// set a dummy pixel format
	PIXELFORMATDESCRIPTOR pixel_format;
	int ret_val = SetPixelFormat(device_context, 1, &pixel_format);
	if (!ret_val)
		return false;

	HGLRC render_context = wglCreateContext(device_context);
	if (!render_context)
		return false;

	ret_val = wglMakeCurrent(device_context, render_context);
	if (!ret_val)
		return false;

	// we unfortunately need this temp window to load all of these extensions, so do that now
	if (!LoadExtensions())
		return false;

	// now clean up this temporary window since we've loaded the extensions
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(render_context);
	ReleaseDC(window, device_context);

	return true;
}
#endif

bool OpenGLGraphics::LoadExtensions()
{
	// Load the OpenGL extensions that this application will be using.
	_wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if(!_wglChoosePixelFormatARB)
		return false;

	_wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if(!_wglCreateContextAttribsARB)
		return false;

	_wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if(!_wglSwapIntervalEXT)
		return false;

	_glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	if(!_glAttachShader)
		return false;

	_glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	if(!_glBindBuffer)
		return false;

	_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
	if(!_glBindVertexArray)
		return false;

	_glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	if(!_glBufferData)
		return false;

	_glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	if(!_glCompileShader)
		return false;

	_glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	if(!_glCreateProgram)
		return false;

	_glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	if(!_glCreateShader)
		return false;

	_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	if(!_glDeleteBuffers)
		return false;

	_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	if(!_glDeleteProgram)
		return false;

	_glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	if(!_glDeleteShader)
		return false;

	_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
	if(!_glDeleteVertexArrays)
		return false;

	_glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	if(!_glDetachShader)
		return false;

	_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	if(!_glEnableVertexAttribArray)
		return false;

	_glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	if(!_glGenBuffers)
		return false;

	_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	if(!_glGenVertexArrays)
		return false;

	_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	if(!_glGetAttribLocation)
		return false;

	_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	if(!_glGetProgramInfoLog)
		return false;

	_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	if(!_glGetProgramiv)
		return false;

	_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	if(!_glGetShaderInfoLog)
		return false;

	_glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	if(!_glGetShaderiv)
		return false;

	_glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	if(!_glLinkProgram)
		return false;

	_glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	if(!_glShaderSource)
		return false;

	_glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	if(!_glUseProgram)
		return false;

	_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	if(!_glVertexAttribPointer)
		return false;

	_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	if(!_glBindAttribLocation)
		return false;

	_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	if(!_glGetUniformLocation)
		return false;

	_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	if(!_glUniformMatrix4fv)
		return false;

	_glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	if(!_glActiveTexture)
		return false;

	_glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	if(!_glUniform1i)
		return false;

	_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	if(!_glGenerateMipmap)
		return false;

	_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	if(!_glDisableVertexAttribArray)
		return false;

	_glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	if(!_glUniform3fv)
		return false;

	_glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	if(!_glUniform4fv)
		return false;

	return true;
}

int OpenGLGraphics::ChoosePixelFormat(HDC device_context)
{
	int attribute_list[] =
	{
		// Support for OpenGL rendering.
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,

		// Support for rendering to a window.
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,

		// Support for hardware acceleration.
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,

		// Support for 24bit color.
		WGL_COLOR_BITS_ARB, 24,

		// Support for 24 bit depth buffer.
		WGL_DEPTH_BITS_ARB, 24,

		// Support for double buffer.
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,

		// Support for swapping front and back buffer.
		WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB,

		// Support for the RGBA pixel type.
		 WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,

		// Support for a 8 bit stencil buffer.
		WGL_STENCIL_BITS_ARB, 8,

		// Null terminate the attribute list.
		0,
	};

	int pixel_format;
	UINT format_count;

	int result = _wglChoosePixelFormatARB(device_context, attribute_list, NULL, 1, &pixel_format, &format_count);
	if (!result)
	{
		DBG_ASSERT_FAIL("Can't find appropriate pixel format.");
	}

	return pixel_format;
}

void OpenGLGraphics::Update()
{
	SwapBuffers(_device_context);
}

void OpenGLGraphics::Destroy()
{
	if (_rendering_context)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(_rendering_context);
		_rendering_context = NULL;
	}

	if (_device_context)
	{
		HWND window = static_cast<WindowsPlatform*>(g_platform)->GetWindow();
		ReleaseDC(window, _device_context);
		_device_context = NULL;
	}
}
