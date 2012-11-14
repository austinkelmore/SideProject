
#include "OpenGLGraphics.h"

IGraphics* CreateGraphicsDriver()
{
	return new OpenGLGraphics();
}

OpenGLGraphics::OpenGLGraphics()
{
}

OpenGLGraphics::~OpenGLGraphics()
{
	Destroy();
}

bool OpenGLGraphics::Init()
{
	return true;
}

void OpenGLGraphics::Update()
{
}

void OpenGLGraphics::Destroy()
{

}
