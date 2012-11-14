
#include "WindowsPlatform.h"
#include "Logging.h"
#include "BasicMacros.h"

static LRESULT CALLBACK WindowsMessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	UNUSED_VAR(hwnd);
	UNUSED_VAR(wparam);
	UNUSED_VAR(lparam);
	// ASSERT(g_platform);
	switch(umessage)
	{
		// Check if the window is being closed or destroyed.
		case WM_DESTROY:
		case WM_CLOSE:
			g_platform->Exit();
			return 0;

		// All other messages pass to the message handler in the system class.
		default:
		{
			//return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	}
}

IMPLEMENT_CONFIG(Platform, WindowsPlatform)
{
	ADD_PROPS(bool, Fullscreen);
	ADD_PROPS(int, Width);
	ADD_PROPS(int, Height);
}

IPlatform* CreatePlatform()
{
	// set up the logging first so that we have a report mechanism in case other things fail
	g_log = new Logging();

	// g_config is guaranteed to be initialized because of the static props initializing it
	g_config->ReadConfigFolder("configs");
	g_config->Initialize();
	g_config->DebugPrintJSONConfigs();

	//ASSERT( g_platform = NULL );
	g_platform = new WindowsPlatform();
	return g_platform;
}

WindowsPlatform::WindowsPlatform()
	: IPlatform()
{
	_instance = NULL;
	_window = NULL;
}

WindowsPlatform::~WindowsPlatform()
{
	Destroy();
	g_platform = NULL;
}

bool WindowsPlatform::Init(int argc, char** argv)
{
	UNUSED_VAR(argc);
	UNUSED_VAR(argv);
	
	_instance = GetModuleHandle(NULL);

	LPCWSTR app_name = L"Side Project";
	// Setup the windows class with default settings.
	_window_class.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	_window_class.lpfnWndProc   = WindowsMessageHandler;
	_window_class.cbClsExtra    = 0;
	_window_class.cbWndExtra    = 0;
	_window_class.hInstance     = _instance;
	_window_class.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	_window_class.hIconSm       = _window_class.hIcon;
	_window_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
	_window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	_window_class.lpszMenuName  = NULL;
	_window_class.lpszClassName = app_name;
	_window_class.cbSize        = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&_window_class);

	DEVMODE dmScreenSettings;
	if (GetProps()->Fullscreen)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)GetProps()->Width;
		dmScreenSettings.dmPelsHeight = (unsigned long)GetProps()->Height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}

	_window = CreateWindowEx(WS_EX_CLIENTEDGE, app_name, app_name,
							 WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
							 200, 200, GetProps()->Width, GetProps()->Height,
							 NULL, NULL, _instance, NULL);

	if (_window == NULL)
	{
	}

	ShowWindow(_window, SW_SHOW);
	SetForegroundWindow(_window);
	SetFocus(_window);

	// hide the mouse cursor
	//ShowCursor(false);

	return true;
}

void WindowsPlatform::Destroy()
{
	if(GetProps()->Fullscreen)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(_window);

	// Remove the application instance.
	UnregisterClass(_window_class.lpszClassName, _instance);

	// delete all of the subsystems
	delete g_log;
	delete g_config;
}

void WindowsPlatform::Resize(int width, int height)
{
	UNUSED_VAR(width);
	UNUSED_VAR(height);


}

void WindowsPlatform::Update()
{
	g_config->CheckForConfigFolderChanges();
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
