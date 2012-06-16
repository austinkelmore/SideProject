
#include "OpenGLGraphicsDriver.h"

OpenGLGraphicsDriver* gGraphicsDriver = 0x0;

IGraphicsDriver* CreateGraphicsDriver()
{
	//ASSERT( gGraphicsDriver == NULL );
	gGraphicsDriver = new OpenGLGraphicsDriver();
	return gGraphicsDriver;
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
