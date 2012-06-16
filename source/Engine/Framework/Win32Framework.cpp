
#include "Win32Framework.h"

Win32Framework* gFramework = NULL;

IFramework* CreateFramework()
{
	//ASSERT( gFramework = NULL );
	gFramework = new Win32Framework();
	return gFramework;
}

Win32Framework::Win32Framework()
{
	mAppInstance = NULL;
	mWindow = NULL;
	mDeviceContext = NULL;
	mFullscreen = false;
}

Win32Framework::~Win32Framework()
{
	Destroy();
}

bool Win32Framework::Init()
{
	mAppInstance = GetModuleHandle( NULL );
	mWindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	mWindowClass.lpfnWndProc = &Win32Framework::WindowsMessageProc;
	mWindowClass.cbClsExtra = 0;
	mWindowClass.cbWndExtra = 0;
	mWindowClass.hInstance = mAppInstance;
	mWindowClass.hIcon = LoadIcon( NULL, IDI_WINLOGO );
	mWindowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	mWindowClass.hbrBackground = NULL;
	mWindowClass.lpszMenuName = NULL;
	mWindowClass.lpszClassName = L"SideProject";

	if ( !RegisterClass( &mWindowClass ) )
	{
		MessageBox( NULL, L"Failed to register the window class.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	if ( mFullscreen )
	{
		memset( &mScreenSettings, 0, sizeof(mScreenSettings) );
		mScreenSettings.dmSize = sizeof(mScreenSettings);
		mScreenSettings.dmPelsWidth = 640;
		mScreenSettings.dmPelsHeight = 480;
		mScreenSettings.dmBitsPerPel = 32;
		mScreenSettings.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if ( ChangeDisplaySettings( &mScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
		{
			MessageBox( NULL, L"Can't switch to full screen mode.", L"Error", MB_OK|MB_ICONSTOP );
			return false;
		}
	}

	mWindowRect.left=(long)0;			// Set Left Value To 0
	mWindowRect.right=(long)640;		// Set Right Value To Requested Width
	mWindowRect.top=(long)0;			// Set Top Value To 0
	mWindowRect.bottom=(long)480;		// Set Bottom Value To Requested Height

	AdjustWindowRect( &mWindowRect, WS_OVERLAPPEDWINDOW, FALSE );

	if ( !( mWindow = CreateWindow( L"SideProject",
									L"Did this work?",
									WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 
									0, 0, 640, 480,
									NULL, NULL,
									mAppInstance,
									NULL ) ) )
	{
		MessageBox( NULL, L"Window creation error.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	ShowWindow( mWindow,SW_SHOW );

	if ( !( mDeviceContext = GetDC( mWindow ) ) )
	{
		Destroy();
		MessageBox( NULL, L"Can't create a GL Device Context.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd =						// pfd Tells Windows How We Want Things To Be
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
	mPixelDescriptor = pfd;

	if ( !( mPixelFormat = ChoosePixelFormat( mDeviceContext, &mPixelDescriptor ) ) )	// Did Windows Find A Matching Pixel Format?
	{
		Destroy();
		MessageBox( NULL, L"Can't find a pixel format.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	if( !SetPixelFormat( mDeviceContext, mPixelFormat, &mPixelDescriptor) )		// Are We Able To Set The Pixel Format?
	{
		Destroy();
		MessageBox( NULL, L"Can't set the pixel format.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	if ( !( mRenderContext = wglCreateContext( mDeviceContext ) ) )				// Are We Able To Get A Rendering Context?
	{
		Destroy();
		MessageBox( NULL, L"Can't create a GL rendering context.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	if( !wglMakeCurrent( mDeviceContext, mRenderContext ) )					// Try To Activate The Rendering Context
	{
		Destroy();
		MessageBox( NULL, L"Can't activate the GL render context.", L"Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	return true;
}

void Win32Framework::Destroy()
{
	if ( mFullscreen )
	{
		ChangeDisplaySettings( NULL, 0 );
		ShowCursor( TRUE );
	}

	if ( mRenderContext )
	{
		if ( !wglMakeCurrent (NULL, NULL ) )
		{
			MessageBox( NULL, L"Release of device context and render context failed.", L"Shutdown Error", MB_OK|MB_ICONINFORMATION );
		}

		if ( !wglDeleteContext( mRenderContext ) )
		{
			MessageBox( NULL, L"Releasing the render context failed.", L"Shutdown Error", MB_OK|MB_ICONINFORMATION );
		}
		mRenderContext = NULL;
	}

	if ( mDeviceContext && !ReleaseDC( mWindow, mDeviceContext ) )
	{
		MessageBox( NULL, L"Releasing the device context failed.", L"Shutdown Error", MB_OK|MB_ICONINFORMATION );
		mDeviceContext = NULL;
	}

	if ( mWindow && !DestroyWindow( mWindow ) )
	{
		MessageBox( NULL, L"Could not release the handle to the window.", L"Shutdown Error", MB_OK|MB_ICONINFORMATION );
		mWindow = NULL;
	}

	if ( mAppInstance && !UnregisterClass( L"SideProject", mAppInstance ) )
	{
		MessageBox( NULL, L"Could not unregister class.", L"Shutdown Error", MB_OK|MB_ICONEXCLAMATION );
		mAppInstance = NULL;
	}
}

LRESULT CALLBACK Win32Framework::WindowsMessageProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

void Win32Framework::Update()
{
	// do nothing
}
