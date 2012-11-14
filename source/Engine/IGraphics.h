
#pragma once

class IGraphics
{
public:
	IGraphics() {}
	~IGraphics() {}

	virtual bool Init() = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;
};