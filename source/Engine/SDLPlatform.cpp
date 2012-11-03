
#include "SDLPlatform.h"
#include "Logging.h"
#include "BasicMacros.h"

#include <SDL/include/SDL.h>
#include <SDL/include/SDL_video.h>
#include <gl/gl.h>
#include <gl/glu.h>

IMPLEMENT_CONFIG(Platform, SDLPlatform)
{
	ADD_PROPS(bool, Fullscreen);
	ADD_PROPS(int, Width);
	ADD_PROPS(int, Height);
}

SDLPlatform* g_platform = NULL;

IPlatform* CreateFramework()
{
	//ASSERT( g_platform = NULL );
	g_platform = new SDLPlatform();
	return g_platform;
}

SDLPlatform::SDLPlatform()
	: IPlatform()
{
	_screen = NULL;
}

SDLPlatform::~SDLPlatform()
{
	Destroy();
	g_platform = NULL;
}

bool SDLPlatform::Init(int argc, char** argv)
{
	UNUSED_VAR(argc);
	UNUSED_VAR(argv);
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		g_log->Log(LOG_Platform, "Unable to initialize SDL Video: %s\n", SDL_GetError());
		return false;
	}

	SDL_WM_SetCaption( "Side Project!", "Side Project!" );

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	Uint32 flags = SDL_OPENGL|SDL_HWSURFACE|SDL_DOUBLEBUF;

	if (GetProps()->Fullscreen)
		flags |= SDL_FULLSCREEN;

	_screen = SDL_SetVideoMode(GetProps()->Width, GetProps()->Height, 32, flags);
	if (!_screen)
	{
		g_log->Log(LOG_Platform, "Unable to set SDL video mode: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

void SDLPlatform::Destroy()
{
	SDL_Quit();
}

void SDLPlatform::Resize(int width, int height)
{
	UNUSED_VAR(width);
	UNUSED_VAR(height);
}

void SDLPlatform::Update()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_QUIT:
			Exit();
			break;
		}
	}

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapBuffers();
}
