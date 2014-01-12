
#include "OpenGLGraphics.h"
#include "FileIO.h"
#include "Logging.h"

void APIENTRY OpenGLDebugFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
									const GLchar* message, GLvoid* user_param)
{
	UNUSED_VAR(user_param);
	UNUSED_VAR(length);
	UNUSED_VAR(id);

	std::string src_name;
	switch(source)
	{
	case GL_DEBUG_SOURCE_API_ARB: src_name = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: src_name = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: src_name = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: src_name = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB: src_name = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER_ARB: src_name = "Other"; break;
	}

	std::string error_type;
	switch(type)
	{
	case GL_DEBUG_TYPE_ERROR_ARB: error_type = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: error_type = "Deprecated Functionality"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: error_type = "Undefined Behavior"; break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB: error_type = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB: error_type = "Performance"; break;
	case GL_DEBUG_TYPE_OTHER_ARB: error_type = "Other"; break;
	}

	std::string type_severity;
	switch(severity)
	{
	case GL_DEBUG_SEVERITY_HIGH_ARB: type_severity = "High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB: type_severity = "Medium"; break;
	case GL_DEBUG_SEVERITY_LOW_ARB: type_severity = "Low"; break;
	}

	g_log->Log(LOG_Graphics, "%s from %s, %s priority Message: %s",
		error_type.c_str(), src_name.c_str(), type_severity.c_str(), message);
}

OpenGLGraphics::OpenGLGraphics()
{
#ifdef WIN32
	_device_context = NULL;
#endif // WIN32
	_rendering_context = NULL;

	_shader_program = 0;
	_vertex_shader = 0;
	_fragment_shader = 0;

	_vertex_array = 0;
	_vertex_buffer = 0;
	_index_buffer = 0;
}

OpenGLGraphics::~OpenGLGraphics()
{
	for (unsigned i = 0; i < _graphics_components.size(); ++i)
		delete _graphics_components[i];

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
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		0,
	};

	_rendering_context = _wglCreateContextAttribsARB(_device_context, 0, attribute_list);
	if (!_rendering_context)
		return false;

	ret_val = wglMakeCurrent(_device_context, _rendering_context);
	if (!ret_val)
		return false;

#endif // WIN32
	
#ifdef GL_ARB_debug_output
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	_glDebugMessageCallbackARB(OpenGLDebugFunction, NULL);
#endif // GL_ARB_debug_output
	
	if (!SetupShaders())
		return false;

	// create a dummy graphics component
	GraphicsComponent::Create();
	std::vector<float> new_vertex_array;
	std::vector<float> new_colors;
	int vertex_count = 0;
	for (unsigned i = 0; i < _graphics_components.size(); ++i)
	{
		std::vector<float> vertices = _graphics_components[i]->GetVertices();
		std::vector<float> colors = _graphics_components[i]->GetColors();
		vertex_count += vertices.size() * 2;
		new_vertex_array.insert(new_vertex_array.end(), vertices.begin(), vertices.end());
		new_colors.insert(new_colors.end(), colors.begin(), colors.end());
	}
	new_vertex_array.insert(new_vertex_array.end(), new_colors.begin(), new_colors.end());

	// dummy perspective matrix
	float aspectRatio = float(g_platform->GetWidth())/g_platform->GetHeight();
	_perspective_matrix.CreatePerspectiveMatrix(aspectRatio, 0.5f, 3.f);
	_perspective_matrix_ogl = _glGetUniformLocation(_shader_program, "perspectiveMatrix");

	_glUseProgram(_shader_program);
	_glUniformMatrix4fv(_perspective_matrix_ogl, 1, GL_FALSE, _perspective_matrix.GetData());
	_glUseProgram(0);

	_glGenBuffers(1, &_vertex_buffer);
	_glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	_glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(float), &new_vertex_array[0], GL_STATIC_DRAW);
	_glBindBuffer(GL_ARRAY_BUFFER, 0);

	_glGenVertexArrays(1, &_vertex_array);
	_glBindVertexArray(_vertex_array);

	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);

	// Set the depth buffer to be entirely cleared to 1.0 values.
	glClearDepth(1.0f);

	// Set the polygon winding to front facing for the left handed system.
	glFrontFace(GL_CW);

	// Enable back face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	return true;
}

bool OpenGLGraphics::SetupShaders()
{
	// set up the shaders
	_shader_program = _glCreateProgram();

	if (!CompileShader("shaders/Texture.frag", GL_FRAGMENT_SHADER, _fragment_shader))
		return false;
	if (!CompileShader("shaders/Texture.vert", GL_VERTEX_SHADER, _vertex_shader))
		return false;

	_glLinkProgram(_shader_program);

	int program_status = 0;
	_glGetProgramiv(_shader_program, GL_LINK_STATUS, &program_status);
	if (program_status != GL_TRUE)
	{
		int log_size = 0;
		_glGetShaderiv(_shader_program, GL_INFO_LOG_LENGTH, &log_size);
		char* log_buffer = new char[log_size];
		_glGetShaderInfoLog(_shader_program, log_size, NULL, log_buffer);

		g_log->Log(LOG_Graphics, "Error linking shader program: %s", log_buffer);
		delete[] log_buffer;

		return false;
	}

	_glDetachShader(_shader_program, _fragment_shader);
	_glDetachShader(_shader_program, _vertex_shader);

	_offset_location = _glGetUniformLocation(_shader_program, "offset");

	return true;
}

bool OpenGLGraphics::CompileShader(const std::string& shader_path, const GLenum shader_type, unsigned int& o_shader)
{
	char* shader_file = NULL;
	int file_size = 0;
	GLint shader_status = 0;

	// load the shader file into a buffer
	if (!LoadFileToBuffer(shader_path.c_str(), &shader_file, file_size))
	{
		g_log->Log(LOG_Graphics, "Couldn't load Shader file %s", shader_path.c_str());
		return false;
	}

	// create the shader
	o_shader = _glCreateShader(shader_type);
	if (o_shader == GL_FALSE)
	{
		g_log->Log(LOG_Graphics, "Couldn't create OpenGL Shader %s", shader_path.c_str());
		DeleteFileBuffer(&shader_file);
		return false;
	}

	_glShaderSource(o_shader, 1, &shader_file, &file_size);
	DeleteFileBuffer(&shader_file);

	_glCompileShader(o_shader);
	_glGetShaderiv(o_shader, GL_COMPILE_STATUS, &shader_status);
	if (shader_status != GL_TRUE)
	{
		int log_size = 0;
		_glGetShaderiv(o_shader, GL_INFO_LOG_LENGTH, &log_size);
		char* log_buffer = new char[log_size];
		_glGetShaderInfoLog(o_shader, log_size, NULL, log_buffer);

		g_log->Log(LOG_Graphics, "Error Compiling shader %s: %s", shader_path.c_str(), log_buffer);
		delete[] log_buffer;

		return false;
	}

	_glAttachShader(_shader_program, o_shader);

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
	if (!_wglChoosePixelFormatARB)
		return false;

	_wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (!_wglCreateContextAttribsARB)
		return false;

	_wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if (!_wglSwapIntervalEXT)
		return false;

	_glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	if (!_glAttachShader)
		return false;

	_glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	if (!_glBindBuffer)
		return false;

	_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
	if (!_glBindVertexArray)
		return false;

	_glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	if (!_glBufferData)
		return false;

	_glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	if (!_glCompileShader)
		return false;

	_glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	if (!_glCreateProgram)
		return false;

	_glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	if (!_glCreateShader)
		return false;

	_glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)wglGetProcAddress("glDebugMessageCallbackARB");
	if (!_glDebugMessageCallbackARB)
		return false;

	_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	if (!_glDeleteBuffers)
		return false;

	_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	if (!_glDeleteProgram)
		return false;

	_glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	if (!_glDeleteShader)
		return false;

	_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
	if (!_glDeleteVertexArrays)
		return false;

	_glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	if (!_glDetachShader)
		return false;

	_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	if (!_glEnableVertexAttribArray)
		return false;

	_glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	if (!_glGenBuffers)
		return false;

	_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	if (!_glGenVertexArrays)
		return false;

	_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	if (!_glGetAttribLocation)
		return false;

	_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	if (!_glGetProgramInfoLog)
		return false;

	_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	if (!_glGetProgramiv)
		return false;

	_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	if (!_glGetShaderInfoLog)
		return false;

	_glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	if (!_glGetShaderiv)
		return false;

	_glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	if (!_glLinkProgram)
		return false;

	_glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	if (!_glShaderSource)
		return false;

	_glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	if (!_glUseProgram)
		return false;

	_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	if (!_glVertexAttribPointer)
		return false;

	_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	if (!_glBindAttribLocation)
		return false;

	_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	if (!_glGetUniformLocation)
		return false;

	_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	if (!_glUniformMatrix4fv)
		return false;

	_glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	if (!_glActiveTexture)
		return false;

	_glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	if (!_glUniform1i)
		return false;

	_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	if (!_glGenerateMipmap)
		return false;

	_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	if (!_glDisableVertexAttribArray)
		return false;

	_glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	if (!_glUniform3fv)
		return false;

	_glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	if (!_glUniform4fv)
		return false;

	_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
	if (!_glBufferSubData)
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

// todo: akelmore - rename from update to something more descriptive (like draw), or separate out the two
void OpenGLGraphics::Update()
{
	// doing this the slow way at first
	// get the vertices
	std::vector<float> new_vertex_array;
	std::vector<float> new_colors;
	int vertex_count = 0;
	for (unsigned i = 0; i < _graphics_components.size(); ++i)
	{
		std::vector<float> vertices = _graphics_components[i]->GetVertices();
		std::vector<float> colors = _graphics_components[i]->GetColors();
		vertex_count += _graphics_components[i]->GetVertexCount() * 2;
		new_vertex_array.insert(new_vertex_array.end(), vertices.begin(), vertices.end());
		new_colors.insert(new_colors.end(), colors.begin(), colors.end());
	}
	new_vertex_array.insert(new_vertex_array.end(), new_colors.begin(), new_colors.end());

	//_glBindBuffer(GL_ARRAY_BUFFER, _vertex_array);
	//_glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * sizeof(float), &new_vertex_array[0]);
	//_glBindBuffer(GL_ARRAY_BUFFER, 0);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_glUseProgram(_shader_program);
	
	//GLuint _perspective_matrix_ogl = _glGetUniformLocation(_shader_program, "perspectiveMatrix");
	//_glUniformMatrix4fv(_perspective_matrix_ogl, 1, GL_FALSE, _perspective_matrix.GetData());

	// calc the position offsets and tell the shader what they are so we only have to do it once and not per object
	Vector4 offset(1.5f, 0.5f, 0.f, 0.f);
	//static float move = 0.f;
	//move += 0.00005f;
	//offset[0] += move;
	
	_glUniform4fv(_offset_location, sizeof(offset), &offset[0]);

	_glBindBuffer(GL_ARRAY_BUFFER, _vertex_array);
	_glEnableVertexAttribArray(0);
	_glEnableVertexAttribArray(1);
	size_t colorData = new_vertex_array.size() * sizeof(new_vertex_array[0]) / 2;
	_glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	_glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorData);

	glDrawArrays(GL_TRIANGLES, 0, vertex_count / 2);

	_glDisableVertexAttribArray(0);
	_glDisableVertexAttribArray(1);
	_glUseProgram(0);

	SwapBuffers(_device_context);
}

void OpenGLGraphics::Destroy()
{
	// hacky hack hack
	_glBindBuffer(GL_ARRAY_BUFFER, 0);
	_glDeleteBuffers(1, &_vertex_buffer);

	_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	_glDeleteBuffers(1, &_index_buffer);

	_glBindVertexArray(0);
	_glDeleteVertexArrays(1, &_vertex_array);

	// get rid of the shaders
	_glDeleteShader(_vertex_shader);
	_glDeleteShader(_fragment_shader);
	_glDeleteProgram(_shader_program);

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

GraphicsComponent* OpenGLGraphics::CreateGraphicsComponent()
{
	GraphicsComponent* component = new GraphicsComponent();
	_graphics_components.push_back(component);
	return component;
}
