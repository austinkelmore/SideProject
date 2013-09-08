
#pragma once

#include "IPlatform.h"
#include "JSONConfig.h"

DEFINE_CONFIG(PlatformConfig, Platform)
	std::string Renderer;
	bool Fullscreen;
	int Width;
	int Height;
DEFINE_CONFIG_END

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

	PlatformConfig* _platform_config;

	HINSTANCE _instance;
	HWND _window;
	WNDCLASSEX _window_class;

	class IGraphics* _graphics_renderer;
};
