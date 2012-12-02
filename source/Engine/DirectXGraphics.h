
#pragma once

#include "IGraphics.h"

#include <string>

class DirectXGraphics : public IGraphics
{
public:
	DirectXGraphics();
	virtual ~DirectXGraphics();

	virtual bool Init();
	virtual void Update();
	virtual void Destroy();

private:
};
