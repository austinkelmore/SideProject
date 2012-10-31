
#pragma once

class IPlatform
{
public:
	IPlatform() { _exit_framework = false; }
	virtual ~IPlatform() {}

	// functions that need to be overwritten
	virtual bool Init( int argc, char** argv ) = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
	virtual void Resize(int width, int height) = 0;
	virtual void Draw() = 0;

	bool IsDone() { return _exit_framework; }
	void SetDone() { _exit_framework = true; }

private:
	bool _exit_framework;
};