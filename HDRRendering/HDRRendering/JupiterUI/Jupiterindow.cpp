#include "../std.h"
#include "../OpenGL/OpenGL.h"
#include "JupiterWindow.h"
#include "JupiterControls.h"

#define WND_HANDLE _hWnd

#define W_CHECK1 \
	if ( NULL == WND_HANDLE ) return TRUE;

#define W_CHECK2 \
	if ( NULL == WND_HANDLE ) return FALSE;

#define W_CHECK3 \
	if ( NULL == WND_HANDLE ) return;

#define CHECK_WND(pWnd) \
	if ( NULL == (pWnd) ) break;

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{		
	CBaseWindow* window = CBaseWindow::FindWindow( hWnd );
	if ( NULL == CBaseWindow::s_CurrentWnd && NULL == window )
		return 0;
	// Handle some specific messages:
	switch( msg )
	{
	case WM_ERASEBKGND:
		window = NULL;
		break;
	case WM_PAINT:
		CHECK_WND( window );		
		C_BREAK_R( window->OnPaint( wParam, lParam ) );	

	case WM_CREATE:	
		CHECK_WND( CBaseWindow::s_CurrentWnd );
		C_BREAK_R( CBaseWindow::s_CurrentWnd->OnCreate( hWnd, wParam, lParam ) );

	case WM_KEYDOWN:
		CHECK_WND( window );
		C_BREAK_R( window->OnKeyDown( wParam, lParam ) );

	case WM_KEYUP:	
		CHECK_WND( window );
		C_BREAK_R( window->OnKeyUp( wParam, lParam ) );

	case WM_LBUTTONDOWN:
		CHECK_WND( window );
		C_BREAK_R( window->OnLButtonDown( wParam, lParam ) );

	case WM_LBUTTONUP:
		CHECK_WND( window );
		C_BREAK_R( window->OnLButtonUp( wParam, lParam ) );

	case WM_RBUTTONDOWN:
		CHECK_WND( window );
		C_BREAK_R( window->OnRButtonDown( wParam, lParam ) );

	case WM_RBUTTONUP:
		CHECK_WND( window );
		C_BREAK_R( window->OnRButtonUp( wParam, lParam ) );

	case WM_MOUSELEAVE:
		CHECK_WND( window );
		C_BREAK_R( window->OnMouseLeave( wParam, lParam ) );		

	case WM_SIZE:	
		CHECK_WND( window );
		C_BREAK_R( window->OnResize( wParam, lParam ) );	

	case  WM_COMMAND:	
		CHECK_WND( window );
		C_BREAK_R( window->OnCommand( wParam, lParam ) );		

	case WM_DESTROY:		
		break;	

	case WM_CLOSE:
		::PostQuitMessage(0); 
		return 0;

	default:
		break;
	}
	
	return ::DefWindowProc( hWnd, msg, wParam, lParam );
}

CBaseWindow::~CBaseWindow(void)
{	
	s_WindowMap.erase( _hWnd );
	DestroyWindow( _hWnd );
	_hWnd = NULL;
}

CBaseWindow::CBaseWindow(LPCSTR lpszClassName, 
						 LPCSTR lpszWindowName /*= NULL*/, 	
						 DWORD dwStyle /*= WS_OVERLAPPEDWINDOW*/,
						 HWND hParentWnd /*= NULL*/,
						 HBRUSH hBackground /*= NULL*/, 
						 HICON hIcon /*= NULL*/, 
						 HCURSOR hCursor /*= NULL*/, 						
						 LPCSTR lpszMenuName /*= NULL */ )
						 : _hInstance( NULL ),
						 _hWnd( NULL )
{
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle( NULL );
	RET( hInstance );
	_hInstance = hInstance;

	WNDCLASS wndClass = { 0 };

	wndClass.style = 0;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = hIcon ;
	wndClass.hCursor = hCursor;
	wndClass.hbrBackground = hBackground ;
	wndClass.lpszMenuName = lpszMenuName;
	wndClass.lpszClassName = lpszClassName;

	if ( !RegisterClass( &wndClass ) ) 
	{	
		DWORD code = GetLastError();
		if ( 1410 != code ) {
			MessageBox( NULL, TEXT( "RegisterClass failed!" ), lpszClassName, MB_ICONERROR );
			return;
		}
	}

	//s_CurrentWnd is used only when creating the window
	s_CurrentWnd = this;
	_hWnd = CreateWindow( lpszClassName, 
		lpszWindowName,
		dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hParentWnd, NULL, hInstance, NULL );
	s_CurrentWnd = NULL;

	if ( NULL == _hWnd ) {		
		MessageBox( NULL, TEXT( "CreateWindow failed" ), lpszWindowName, MB_ICONERROR );		
	}
	else {
		s_WindowMap[ _hWnd ] = this;
	}
}

/************************************************************************
 * the following are window message handle
 * return value: TRUE if need further process, otherwise FALSE
 * parameters: Windows message parameters
 ************************************************************************/
BOOL CBaseWindow::OnLButtonDown( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL CBaseWindow::OnLButtonUp( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL CBaseWindow::OnRButtonDown( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL CBaseWindow::OnRButtonUp( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL CBaseWindow::OnMouseMove( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL CBaseWindow::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL CBaseWindow::OnKeyDown( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL CBaseWindow::OnKeyUp( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL CBaseWindow::OnResize( WPARAM wParam, LPARAM lParam )
{	
	return TRUE;
}

BOOL CBaseWindow::OnPaint( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL CBaseWindow::OnCreate( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL CBaseWindow::OnCommand( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

void CBaseWindow::Show( int nCmdShow )
{
	W_CHECK3;
	::ShowWindow( _hWnd,  nCmdShow );
	::UpdateWindow( _hWnd );
}

void CBaseWindow::Show( void )
{
	this->Show( SW_SHOW );	
}

void CBaseWindow::Hide( void )
{
	W_CHECK3;
	::ShowWindow( _hWnd, SW_HIDE );
}

/************************************************************************
 * the following are static members
 ************************************************************************/
CBaseWindow::WindowMap CBaseWindow::s_WindowMap;
CBaseWindow* CBaseWindow::s_CurrentWnd = NULL;

CBaseWindow* CBaseWindow::FindWindow( HWND hWnd )
{
	WindowMap::iterator it = s_WindowMap.find( hWnd );
	return s_WindowMap.end() == it ? NULL : it->second;
}

/************************************************************************
 * class MainWindow
 ************************************************************************/
MainWindow::MainWindow( LPCTSTR lpszClassName, LPCTSTR lpszWindowName )
:CBaseWindow( lpszClassName, lpszWindowName ), _current( 0 )
{
	for ( int n = 0; n < s_nWN; ++n )
		_glWidgets[ n ] = NULL;

	int r = 0, c = 0, width = 200;
	for ( int n = 0; n < s_nWN; ++n )
	{
		RET( _glWidgets[n] = new GLWidget( _hWnd ) );
		RET( _glWidgets[n]->Initialize() );
		_glWidgets[n]->SetPosition( c, r, width, width );
		if ( (n+1) % 3 )
		{
			c += width + 2;
		}
		else
		{
			r += width + 2;
			c = 0;
		}
	}	
}

MainWindow::~MainWindow( void )
{
	for ( int n = 0; n < s_nWN; ++n )
	{
		SAFE_RELEASE( _glWidgets[ n ] );
	}
}

BOOL MainWindow::OnResize( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

int MainWindow::Run( void )
{
	this->Show();
	MSG msg = { 0 };
	while( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else 
		{
			if ( NULL != _glWidgets[ _current ] )
			{
				_glWidgets[_current]->OnPaint( NULL, NULL );
				_current = ( _current + 1 ) % s_nWN;
			}
		}
	}
	return 0;
}

