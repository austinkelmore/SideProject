
#pragma once

#include "IGraphics.h"

#include <string>

class OpenGLGraphics : public IGraphics
{
public:
	OpenGLGraphics();
	~OpenGLGraphics();

	virtual bool Init();
	virtual void Update();
	virtual void Destroy();

private:
};
