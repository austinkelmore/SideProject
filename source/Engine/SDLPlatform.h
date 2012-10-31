
#pragma once

#include "IPlatform.h"
#include <windows.h>
#include "JSONConfig.h"

class SDLPlatform : public IPlatform
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

	SDLPlatform();
	virtual ~SDLPlatform();

	virtual bool Init( int argc, char** argv );
	virtual void Destroy();
	virtual void Update();
	virtual void Resize(int width, int height);
	virtual void Draw();

private:
};

extern SDLPlatform* g_platform;