
#include "OpenGLGraphicsDriver.h"

IGraphicsDriver* CreateGraphicsDriver()
{
	return new OpenGLGraphicsDriver();
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
