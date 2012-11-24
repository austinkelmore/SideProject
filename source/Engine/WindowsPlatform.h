
#pragma once

#include "IPlatform.h"
#include "JSONConfig.h"

class IGraphics;

class WindowsPlatform : public IPlatform
{
	typedef IPlatform SuperClass;

public:
	DEFINE_CONFIG(Platform)
	{
		Renderer = "opengl";
		Fullscreen = false;
		Width = 200;
		Height = 200;
	}
	std::string Renderer;
	bool Fullscreen;
	int Width;
	int Height;
	DEFINE_CONFIG_END(Platform)

	WindowsPlatform();
	virtual ~WindowsPlatform();

	virtual bool Init( int argc, char** argv );
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

	IGraphics* _graphics_renderer;
};
