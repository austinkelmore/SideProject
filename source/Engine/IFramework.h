
#pragma once

class IFramework
{
public:
	IFramework() { _exit_framework = false; }
	virtual ~IFramework() {}

	// functions that need to be overwritten
	virtual bool Init() = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;

	bool IsDone() { return _exit_framework; }

protected:
	bool _exit_framework;
};