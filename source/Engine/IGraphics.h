
#pragma once

#include "GraphicsComponent.h"

class IGraphics
{
public:
	IGraphics();
	virtual ~IGraphics();

	virtual bool Init() = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;
	virtual GraphicsComponent* CreateGraphicsComponent() = 0;
};

extern IGraphics* g_graphics;
