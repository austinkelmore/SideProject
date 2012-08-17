
#include "OpenGLGraphicsDriver.h"

OpenGLGraphicsDriver* g_graphics_driver = 0x0;

IGraphicsDriver* CreateGraphicsDriver()
{
	//ASSERT( g_graphics_driver == NULL );
	g_graphics_driver = new OpenGLGraphicsDriver();
	return g_graphics_driver;
}

OpenGLGraphicsDriver::OpenGLGraphicsDriver()
{

}

OpenGLGraphicsDriver::~OpenGLGraphicsDriver()
{
	Destroy();
}

bool OpenGLGraphicsDriver::Init()
{
	return true;
}

void OpenGLGraphicsDriver::Update()
{

}

void OpenGLGraphicsDriver::Destroy()
{

}
