
#include "IGraphics.h"

IGraphics* g_graphics = NULL;

IGraphics::IGraphics()
{
	g_graphics = this;
}

IGraphics::~IGraphics()
{
	g_graphics = NULL;
}
