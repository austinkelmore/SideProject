
#include "DirectXGraphics.h"

DirectXGraphics::DirectXGraphics()
{
}

DirectXGraphics::~DirectXGraphics()
{
	Destroy();
}

bool DirectXGraphics::Init()
{
	return true;
}

void DirectXGraphics::Update()
{
}

void DirectXGraphics::Destroy()
{

}

GraphicsComponent* DirectXGraphics::CreateGraphicsComponent()
{
	return NULL;
}