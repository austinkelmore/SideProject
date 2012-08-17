
#pragma once

#include "IFramework.h"
#include <windows.h>
#include <gl\gl.h>

class Win32Framework : public IFramework
{
	typedef IFramework SUPER;

public:

	Win32Framework();
	virtual ~Win32Framework();

	virtual bool Init();
	virtual void Destroy();
	virtual void Update();

	static LRESULT CALLBACK WindowsMessageProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:

	// windows related stuff
	HINSTANCE _app_instance;
	WNDCLASS _window_class;
	HWND _window;
	DEVMODE _screen_settings;
	HDC _device_context;
	PIXELFORMATDESCRIPTOR _pixel_descriptor;
	GLuint _pixel_format;
	HGLRC _render_context;

	// config vars
	bool _fullscreen;
	RECT _window_rect;
};

extern Win32Framework* g_framework;