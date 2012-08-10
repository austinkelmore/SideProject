
#pragma once

#include "../IFramework.h"
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
	HINSTANCE mAppInstance;
	WNDCLASS mWindowClass;
	HWND mWindow;
	DEVMODE mScreenSettings;
	HDC mDeviceContext;
	PIXELFORMATDESCRIPTOR mPixelDescriptor;
	GLuint mPixelFormat;
	HGLRC mRenderContext;

	// config vars
	bool mFullscreen;
	RECT mWindowRect;
};

extern Win32Framework* gFramework;