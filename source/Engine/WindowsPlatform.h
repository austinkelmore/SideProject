
#pragma once

#include "IPlatform.h"
#include "JSONConfig.h"

class WindowsPlatform : public IPlatform
{
	typedef IPlatform SuperClass;

public:
	DEFINE_CONFIG(Platform)
	{
		Fullscreen = false;
		Width = 200;
		Height = 200;
	}
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

private:

	HINSTANCE _instance;
	HWND _window;
	WNDCLASSEX _window_class;
};
