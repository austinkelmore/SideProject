
#pragma once

#include "IGraphicsDriver.h"

class OpenGLGraphicsDriver : public IGraphicsDriver
{
public:
	OpenGLGraphicsDriver();
	~OpenGLGraphicsDriver();

	virtual bool Init();
	virtual void Update();
	virtual void Destroy();
};

extern OpenGLGraphicsDriver* gGraphicsDriver;