
#pragma once

#include "Vector2.h"
#include "BaseConfig.h"

DEFINE_CONFIG(PlatformConfig, Platform)
	std::string Renderer;
	bool Fullscreen;
	int Width;
	int Height;
DEFINE_CONFIG_END

class IPlatform
{
public:
	IPlatform();
	virtual ~IPlatform();

	// functions that need to be overwritten
	virtual bool Init(int argc, char** argv) = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
	virtual void Resize(int width, int height) = 0;

	bool IsDone() { return _exit_framework; }
	void Exit() { _exit_framework = true; }

	void SetScreenDimensions(const int width, const int height);
	int GetWidth() const { return _window_width; }
	int GetHeight() const { return _window_height; }

	PlatformConfig* _platform_config;

private:
	bool _exit_framework;
	int _window_width;
	int _window_height;
};

namespace Platform
{
	// todo: akelmore - i don't like this code here
	void CreateBasePlatformFunctionality();
	void DestroyBasePlatformFunctionality();
}

extern IPlatform* g_platform;
