
#pragma once

#include "IPlatform.h"
#include <windows.h>

class WindowsPlatform : public IPlatform
{
	typedef IPlatform SuperClass;

public:

	WindowsPlatform();
	virtual ~WindowsPlatform();

	virtual bool Init(int argc, char** argv);
	virtual void Destroy();
	virtual void Update();
	virtual void Resize(int width, int height);

	HDC GetDC() const;
	HWND GetWindow() const;

	bool _ignore_window_messages;

private:

	HINSTANCE _instance;
	HWND _window;
	WNDCLASSEX _window_class;

	class IGraphics* _graphics_renderer;
};
