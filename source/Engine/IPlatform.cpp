
#include "IPlatform.h"
#include "JSONConfig.h"
#include <stdlib.h>

IPlatform* g_platform = nullptr;

IMPLEMENT_CONFIG(PlatformConfig)
{
	ADD_CONFIG_VAR(std::string, Renderer, "opengl");
	ADD_CONFIG_VAR(bool, Fullscreen, false);
	ADD_CONFIG_VAR(int, Width, 200);
	ADD_CONFIG_VAR(int, Height, 200);
}

void Platform::CreateBasePlatformFunctionality()
{
	DBG_ASSERT(g_config == nullptr);
	g_config = new JSONConfigMgr();
	g_config->ReadConfigFolder("configs");
}

void Platform::DestroyBasePlatformFunctionality()
{
}

IPlatform::IPlatform()
	: _exit_framework(false), _window_width(0), _window_height(0), _platform_config(nullptr)
{
	// do nothing
}

IPlatform::~IPlatform()
{
}

bool IPlatform::Init(int argc, char** argv)
{
	UNUSED_VAR(argc);
	UNUSED_VAR(argv);

	_platform_config = PlatformConfig::StaticNew();

	return true;
}

void IPlatform::Update()
{
	g_config->CheckForConfigFolderChanges();

	if (_platform_config->Width != GetWidth() || _platform_config->Height != GetHeight())
	{
		Resize(_platform_config->Width, _platform_config->Height);
	}
}

void IPlatform::Destroy()
{
	delete _platform_config;
	delete g_config;
	g_config = nullptr;
}

void IPlatform::SetScreenDimensions(const int width, const int height)
{
	DBG_ASSERT(width > 0 && height > 0);
	_window_width = width;
	_window_height = height;
}
