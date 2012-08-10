
#pragma once

class IFramework
{
public:
	IFramework() { mExitFramework = false; }
	virtual ~IFramework() {}

	// functions that need to be overwritten
	virtual bool Init() = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;

	bool IsDone() { return mExitFramework; }

protected:
	bool mExitFramework;
};