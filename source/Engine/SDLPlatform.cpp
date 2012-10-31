
#include "SDLPlatform.h"

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
}

SDLPlatform::~SDLPlatform()
{
	Destroy();
	g_platform = NULL;
}

bool SDLPlatform::Init( int argc, char** argv )
{
	return true;
}

void SDLPlatform::Destroy()
{
	
}

void SDLPlatform::Resize(int width, int height)
{
}

void SDLPlatform::Update()
{
}

void SDLPlatform::Draw()
{
	
}
