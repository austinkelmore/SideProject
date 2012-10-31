
#pragma once

#include "IPlatform.h"
#include "JSONConfig.h"

#include <SDL/include/SDL.h>
#include <SDL/include/SDL_video.h>

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

private:

	SDL_Surface* _screen;
};

extern SDLPlatform* g_platform;