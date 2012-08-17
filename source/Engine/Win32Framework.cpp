
#include "Win32Framework.h"
#include <gl\glu.h>

Win32Framework* g_framework = NULL;

IFramework* CreateFramework()
{
	//ASSERT( g_framework = NULL );
	g_framework = new Win32Framework();
	return g_framework;
}

Win32Framework::Win32Framework()
	: IFramework()
{
	_app_instance = NULL;
	_window = NULL;
	_device_context = NULL;
	_fullscreen = false;
}

Win32Framework::~Win32Framework()
{
	Destroy();
	g_framework = NULL;
}

bool Win32Framework::Init()
{
	_app_instance = GetModuleHandle( NULL );
	_window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	_window_class.lpfnWndProc = &Win32Framework::WindowsMessageProc;
	_window_class.cbClsExtra = 0;
	_window_class.cbWndExtra = 0;
	_window_class.hInstance = _app_instance;
	_window_class.hIcon = LoadIcon( NULL, IDI_WINLOGO );
	_window_class.hCursor = LoadCursor( NULL, IDC_ARROW );
	_window_class.hbrBackground = NULL;
	_window_class.lpszMenuName = NULL;
	_window_class.lpszClassName = L"SideProject";

	if ( !RegisterClass( &_window_class ) )
	{
		MessageBox( NULL, L"Failed to register the window class.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	if ( _fullscreen )
	{
		memset( &_screen_settings, 0, sizeof(_screen_settings) );
		_screen_settings.dmSize = sizeof(_screen_settings);
		_screen_settings.dmPelsWidth = 640;
		_screen_settings.dmPelsHeight = 480;
		_screen_settings.dmBitsPerPel = 32;
		_screen_settings.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if ( ChangeDisplaySettings( &_screen_settings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
		{
			MessageBox( NULL, L"Can't switch to full screen mode.", L"Error", MB_OK|MB_ICONSTOP );
			return false;
		}
	}

	_window_rect.left = 0;			// Set Left Value To 0
	_window_rect.right = 640;		// Set Right Value To Requested Width
	_window_rect.top = 0;			// Set Top Value To 0
	_window_rect.bottom = 480;		// Set Bottom Value To Requested Height

	AdjustWindowRectEx( &_window_rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW|WS_EX_WINDOWEDGE );

	if ( !( _window = CreateWindow( L"SideProject",
									L"Did this work?",
									WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 
									0, 0, 640, 480,
									NULL, NULL,
									_app_instance,
									NULL ) ) )
	{
		MessageBox( NULL, L"Window creation error.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	ShowWindow( _window, SW_SHOW );

	if ( !( _device_context = GetDC( _window ) ) )
	{
		Destroy();
		MessageBox( NULL, L"Can't create a GL Device Context.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	PIXELFORMATDESCRIPTOR pixel_format_descriptor =	// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		32,											// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		32,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	_pixel_descriptor = pixel_format_descriptor;

	if ( !( _pixel_format = ChoosePixelFormat( _device_context, &_pixel_descriptor ) ) )	// Did Windows Find A Matching Pixel Format?
	{
		Destroy();
		MessageBox( NULL, L"Can't find a pixel format.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	if( !SetPixelFormat( _device_context, _pixel_format, &_pixel_descriptor) )		// Are We Able To Set The Pixel Format?
	{
		Destroy();
		MessageBox( NULL, L"Can't set the pixel format.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	if ( !( _render_context = wglCreateContext( _device_context ) ) )				// Are We Able To Get A Rendering Context?
	{
		Destroy();
		MessageBox( NULL, L"Can't create a GL rendering context.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	if( !wglMakeCurrent( _device_context, _render_context ) )					// Try To Activate The Rendering Context
	{
		Destroy();
		MessageBox( NULL, L"Can't activate the GL render context.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	ShowWindow( _window, SW_SHOW );						// Show The Window
	SetForegroundWindow( _window );						// Slightly Higher Priority
	SetFocus( _window );								// Sets Keyboard Focus To The Window

	// todo: amcgee - turn this into a function to resize the screen
	glViewport( 0, 0, 640, 480 );						// Reset The Current Viewport

	glMatrixMode( GL_PROJECTION );						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective( 45.f, (GLfloat)640/(GLfloat)480, 0.1f, 100.f );

	glMatrixMode( GL_MODELVIEW );						// Select The Modelview Matrix
	glLoadIdentity();

	glShadeModel( GL_SMOOTH );							// Enable Smooth Shading
	glClearColor( 0.f, 0.f, 0.f, 0.5f );				// Black Background
	glClearDepth( 1.f );								// Depth Buffer Setup
	glEnable( GL_DEPTH_TEST );							// Enables Depth Testing
	glDepthFunc( GL_LEQUAL );							// The Type Of Depth Testing To Do
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );// Really Nice Perspective Calculations
	/*
	if ( !InitGL() )									// Initialize Our Newly Created GL Window
	{
		Destroy();
		MessageBox( NULL, L"Initialization of OpenGL failed.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}
	*/
	return true;
}

void Win32Framework::Destroy()
{
	if ( _fullscreen )
	{
		ChangeDisplaySettings( NULL, 0 );
		ShowCursor( TRUE );
	}

	if ( _render_context )
	{
		if ( !wglMakeCurrent (NULL, NULL ) )
		{
			MessageBox( NULL, L"Release of device context and render context failed.", L"Shutdown Error", MB_OK|MB_ICONINFORMATION );
		}

		if ( !wglDeleteContext( _render_context ) )
		{
			MessageBox( NULL, L"Releasing the render context failed.", L"Shutdown Error", MB_OK|MB_ICONINFORMATION );
		}
		_render_context = NULL;
	}

	if ( _device_context && !ReleaseDC( _window, _device_context ) )
	{
		MessageBox( NULL, L"Releasing the device context failed.", L"Shutdown Error", MB_OK|MB_ICONINFORMATION );
		_device_context = NULL;
	}

	if ( _window && !DestroyWindow( _window ) )
	{
		MessageBox( NULL, L"Could not release the handle to the window.", L"Shutdown Error", MB_OK|MB_ICONINFORMATION );
		_window = NULL;
	}

	if ( _app_instance && !UnregisterClass( L"SideProject", _app_instance ) )
	{
		MessageBox( NULL, L"Could not unregister class.", L"Shutdown Error", MB_OK|MB_ICONEXCLAMATION );
		_app_instance = NULL;
	}
}

LRESULT CALLBACK Win32Framework::WindowsMessageProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_CLOSE:
		{
			PostQuitMessage( 0 );
			return 0;
		}
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

void Win32Framework::Update()
{
	// handle the messages
	MSG msg;
	if ( PeekMessage( &msg, NULL, 0, PM_NOREMOVE, PM_REMOVE ) )
	{
		if ( msg.message == WM_QUIT )
		{
			_exit_framework = true;
		}
		else
		{
			TranslateMessage(&msg);				// Translate The Message
			DispatchMessage(&msg);				// Dispatch The Message
		}
	}

	// draw the screen
	glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();
	SwapBuffers( _device_context );
}
