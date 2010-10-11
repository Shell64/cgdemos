#ifndef OEMRESOURCE
#define OEMRESOURCE
#endif

#include <windows.h>
//#include <GdiPlus.h>
#include <cstdio>

#include "resource.h"
#include "Controls.h"

#define APP_WINDOW_NAME "Jupiter"
#define APP_CLASS_NMAE "Jupiter_Class"

//using namespace Gdiplus;
HWND g_hToolbar = NULL;
DisplayWidget* g_pDisplayWidget = NULL;
HWND g_hMainWnd = NULL;
HINSTANCE g_hInstance = NULL;

BOOL InitApplication( HINSTANCE hInstance );
BOOL InitInstance( HINSTANCE hInstance, INT nShowCmd );
LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

void ShutDown( void );

void OnFileOpen( void );

int WINAPI WinMain( HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nShowCmd )
{	
	BOOL bSuccess = TRUE;
	MSG msg = { 0 };
	BOOL bRet;

	/*GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken = NULL;*/

	// only one instance is allowed
	HANDLE appMutex = CreateMutex( NULL, FALSE, "Jupiter" );
	if ( ERROR_ALREADY_EXISTS == GetLastError() )
	{
		bSuccess = FALSE;
		goto exit_func;
	}
	
	//GdiplusStartup( &gdiplusToken, &gdiplusStartupInput, NULL );

	if ( !InitApplication( hInstance ) ) 
	{
		bSuccess = FALSE;
		goto exit_func;
	}

	if ( !InitInstance( hInstance, nShowCmd ) )
	{
		bSuccess = FALSE;
		goto exit_func;
	}	

	g_hInstance = hInstance;

	while ( 0 != ( bRet = GetMessage( &msg, NULL, 0, 0 ) ) ) 
	{
		if ( -1 == bRet ) 
		{
			bSuccess = FALSE;
			goto exit_func;
		}

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

exit_func:

	/*if ( gdiplusToken ) 
	{
		GdiplusShutdown( gdiplusToken );
	}*/

	CloseHandle( appMutex );

	if ( bSuccess ) 
	{
		return int( msg.wParam );
	}
	return 0;
}

BOOL InitApplication( HINSTANCE hInstance )
{
	BOOL bSuccess = TRUE;
	WNDCLASS wndClass = { 0 };

	wndClass.style = 0;
	wndClass.lpfnWndProc = MainWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon( hInstance, TEXT( "BlackLeaf" ) );
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	wndClass.hbrBackground = (HBRUSH)( COLOR_WINDOW  );
	wndClass.lpszMenuName = TEXT( "MainMenu" );
	wndClass.lpszClassName = TEXT( APP_CLASS_NMAE );

	if ( !RegisterClass( &wndClass ) ) 
	{
		MessageBox( NULL, TEXT( "RegisterClass failed!" ), TEXT( APP_CLASS_NMAE ), MB_ICONERROR );
		bSuccess = FALSE;
	}

	if ( !InitControls( hInstance ) )
	{
		bSuccess = FALSE;
	}

	return bSuccess;
}

BOOL InitInstance( HINSTANCE hInstance, INT nShowCmd )
{
	g_hMainWnd = CreateWindow( TEXT( APP_CLASS_NMAE ), 
		TEXT( APP_WINDOW_NAME ),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL );

	if ( NULL == g_hMainWnd ) 
	{
		MessageBox( NULL, "CreateWindow failed", TEXT( APP_CLASS_NMAE ), MB_ICONERROR );
		return FALSE;
	}

	ShowWindow( g_hMainWnd, nShowCmd );
	UpdateWindow( g_hMainWnd );

	return TRUE;
}

void UpdateLayout( HWND hMainWnd )
{
	RECT MWndRect;	
	GetClientRect( hMainWnd, &MWndRect );

	RECT DWRect;
	DWRect.left = 200;
	DWRect.top = 0;
	DWRect.right = MWndRect.right;
	DWRect.bottom = MWndRect.bottom;

	g_pDisplayWidget->SetPosition( DWRect.left, DWRect.top, 
		DWRect.right - DWRect.left, DWRect.bottom - DWRect.top );
}

LRESULT CALLBACK ProcessCommand( WPARAM wParam )
{
	switch ( wParam )
	{
	case IDM_FILE_OPEN:
		OnFileOpen( );
		break;
	case  IDM_FILE_SAVE:
		break;
	case IDM_FILE_EXIT:
		ShutDown();
		break;
	case IDM_TOOLS_MOVE:
		//g_pDisplayWidget->SetState( DWS_MOVE );
		break;
	case  IDM_TOOLS_ZOOM:
		//g_pDisplayWidget->SetState( DWS_ZOOM );
		break;
	case ID_GEODESICMATTING_FORE:
		break;
	case ID_GEODESICMATTING_BACK:
		break;
	case ID_GEODESICMATTING_DONE:
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{	
	static bool init = false;
	// Handle some specific messages:
	switch( msg )
	{

	case WM_PAINT:
		break;

	case WM_CREATE:	
		g_pDisplayWidget = new SceneRender( hWnd );
		if ( NULL == g_pDisplayWidget ) 
		{
			::PostMessage( hWnd, WM_CLOSE, 0, 0 );
		}
		else
		{
			g_pDisplayWidget->Initialize();
		}
		return 0;

	case WM_KEYDOWN:
		g_pDisplayWidget->KeyDownProc( wParam, lParam );
		return 0;

	case WM_KEYUP:
		g_pDisplayWidget->KeyUpProc( wParam, lParam );
		return 0;

	case WM_DESTROY: 		
		::PostQuitMessage(0); 
		return 0;

	case WM_SIZE:		
		UpdateLayout( hWnd );
		return 0;

	case WM_CLOSE:	
		ShutDown();
		return 0;

	case  WM_COMMAND:
		ProcessCommand( wParam );
		return 0;

	default:
		break;
	}

	// Forward any other messages we didn't handle
	// above to the default window procedure.
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void ShutDown( void )
{	
	if ( NULL != g_pDisplayWidget )
	{
		delete g_pDisplayWidget;
		g_pDisplayWidget = NULL;
	}

	DestroyWindow( g_hMainWnd );
	g_hMainWnd = NULL;

	if ( NULL != g_hToolbar )
	{
		DestroyWindow( g_hToolbar );
		g_hToolbar = NULL;
	}
}

void OnFileOpen( void )
{
	TCHAR szFile[ MAX_PATH ] = { 0 };
	TCHAR szPath[ MAX_PATH ] = { 0 };
	OPENFILENAME ofn = 
	{
		sizeof ( OPENFILENAME ),
		NULL,
		NULL,
		TEXT("JPG files(*.jpg,*jpeg)\0*.jpg;*.jpeg\0Bitmap Files(*.bmp)\0*.bmp\0Png Files(.png)\0*.png\0All files(*.*)\0*.*\0"),
		NULL,
		0,
		1,
		szFile, MAX_PATH,
		NULL,
		0,
		szPath,
		TEXT("Open Image File"),
		OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
		0,
		0,
		TEXT(".jpg"),
		0,
		NULL,
		NULL		
	};
	if ( TRUE != GetOpenFileName( &ofn ) )
	{
		return;
	}	

	//Image* hdrImg = ImageManager::get()->getImage( szFile );
	
	//code comes here

	return;
}