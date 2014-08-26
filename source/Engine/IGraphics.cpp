
#include "IGraphics.h"

IGraphics* g_graphics = nullptr;

IGraphics::IGraphics()
{
	g_graphics = this;
}

IGraphics::~IGraphics()
{
	g_graphics = nullptr;
}
