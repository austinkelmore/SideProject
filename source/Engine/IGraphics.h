
#pragma once

class IGraphics
{
public:
	IGraphics() {}
	virtual ~IGraphics() {}

	virtual bool Init() = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;
};