
#include "WindowsPlatform.h"
#include "Logging.h"
#include "BasicMacros.h"
#include "OpenGLGraphics.h"
#include "JSONConfig.h"

static LRESULT CALLBACK WindowsMessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	UNUSED_VAR(hwnd);
	UNUSED_VAR(wparam);
	UNUSED_VAR(lparam);
	DBG_ASSERT(g_platform);
	switch(umessage)
	{
		// Check if the window is being closed or destroyed.
		case WM_DESTROY:
		case WM_CLOSE:
		{
			if (!static_cast<WindowsPlatform*>(g_platform)->_ignore_window_messages)
			{
				g_platform->Exit();
				return 0;
			}
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	}
}


IPlatform* CreatePlatform()
{
	Platform::CreateBasePlatformFunctionality();

	DBG_ASSERT(g_platform == nullptr);
	g_platform = new WindowsPlatform();

	// set up the logging second so that we have a report mechanism in case other things fail
	g_log = new Logging();

	//g_config->DebugPrintJSONConfigs();

	return g_platform;
}

WindowsPlatform::WindowsPlatform()
	: IPlatform()
{
	_graphics_renderer = nullptr;
	_instance = nullptr;
	_window = nullptr;
	_ignore_window_messages = false;
}

WindowsPlatform::~WindowsPlatform()
{
	Destroy();
	g_platform = nullptr;
}

// todo: akelmore - separate out the init with the command line versus regular init
bool WindowsPlatform::Init(int argc, char** argv)
{
	SuperClass::Init(argc, argv);

	_instance = GetModuleHandle(nullptr);

	LPCWSTR app_name = L"Side Project";
	// Setup the windows class with default settings.
	_window_class.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	_window_class.lpfnWndProc   = WindowsMessageHandler;
	_window_class.cbClsExtra    = 0;
	_window_class.cbWndExtra    = 0;
	_window_class.hInstance     = _instance;
	_window_class.hIcon			= LoadIcon(nullptr, IDI_WINLOGO);
	_window_class.hIconSm       = _window_class.hIcon;
	_window_class.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	_window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	_window_class.lpszMenuName  = nullptr;
	_window_class.lpszClassName = app_name;
	_window_class.cbSize        = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&_window_class);

	// do the correct initialization based off of what we're doing (defaulting to OpenGL)
	if (_platform_config->Renderer == "directx")
	{
		DBG_ASSERT_FAIL("I haven't created the DirectX part of the renderer yet.");
	}
	else
	{
		_ignore_window_messages = true;
		_window = CreateWindowEx(WS_EX_APPWINDOW, app_name, app_name,
			WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			200, 200, _platform_config->Width, _platform_config->Height,
			nullptr, nullptr, _instance, nullptr);

		if (_window == nullptr)
			return false;

		// hide the window because it's only a temp one
		ShowWindow(_window, SW_HIDE);

		OpenGLGraphics* opengl_graphics = new OpenGLGraphics();
		_graphics_renderer = opengl_graphics;

		opengl_graphics->PreInitializeWindowsOpenGL(_window);

		// destroy the temp window
		DestroyWindow(_window);
		_window = nullptr;
		_ignore_window_messages = false;
	}

	DEVMODE dmScreenSettings;
	if (_platform_config->Fullscreen)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)_platform_config->Width;
		dmScreenSettings.dmPelsHeight = (unsigned long)_platform_config->Height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}

	// create the real window
	// 200s are x/y position on screen
	_window = CreateWindowEx(WS_EX_APPWINDOW, app_name, app_name,
							WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
							200, 200, _platform_config->Width, _platform_config->Height,
							nullptr, nullptr, _instance, nullptr);

	if (_window == nullptr)
		return false;

	SetScreenDimensions(_platform_config->Width, _platform_config->Height);

	if(!_graphics_renderer->Init())
		return false;

	ShowWindow(_window, SW_SHOW);
	SetForegroundWindow(_window);
	SetFocus(_window);

	return true;
}

void WindowsPlatform::Destroy()
{
	if (_platform_config->Fullscreen)
		ChangeDisplaySettings(nullptr, 0);

	// delete the rendering system first
	delete _graphics_renderer;

	DestroyWindow(_window);

	// Remove the application instance.
	UnregisterClass(_window_class.lpszClassName, _instance);

	// delete all of the subsystems
	delete g_log;

	SuperClass::Destroy();
}

void WindowsPlatform::Resize(int width, int height)
{
	SetScreenDimensions(width, height);

	// todo: akelmore - handle resizing of the window
	//Destroy();
}

void WindowsPlatform::Update()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	_graphics_renderer->Update();
}

HDC WindowsPlatform::GetDC() const
{
	return ::GetDC(_window);
}

HWND WindowsPlatform::GetWindow() const
{
	return _window;
}
