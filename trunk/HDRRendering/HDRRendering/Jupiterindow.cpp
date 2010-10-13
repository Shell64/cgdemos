#include "std.h"
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
		::PostQuitMessage(0); 
		return 0;	

	case WM_CLOSE:		
		SAFE_RELEASE( window );
		break;

	default:
		break;
	}
	
	return ::DefWindowProc( hWnd, msg, wParam, lParam );
}

CBaseWindow::~CBaseWindow(void)
{
	DestroyWindow( _hWnd );
	s_WindowMap.erase( _hWnd );
}

CBaseWindow::CBaseWindow(LPCSTR lpszClassName, 
						 LPCSTR lpszWindowName /*= NULL*/, 	
						 DWORD dwStyle /*= WS_OVERLAPPEDWINDOW*/,
						 HWND hParentWnd /*= NULL*/,
						 HICON hIcon /*= NULL*/, 
						 HCURSOR hCursor /*= NULL*/, 
						 HBRUSH hBackground /*= NULL*/, 
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
	wndClass.hCursor = hCursor == NULL ? LoadCursor( NULL, IDC_ARROW ) : hCursor;
	wndClass.hbrBackground = hBackground == NULL ? (HBRUSH)( COLOR_WINDOW ) : hBackground ;
	wndClass.lpszMenuName = lpszMenuName;
	wndClass.lpszClassName = lpszClassName;

	if ( !RegisterClass( &wndClass ) ) 
	{
		DWORD errCode = GetLastError();
		MessageBox( NULL, TEXT( "RegisterClass failed!" ), lpszClassName, MB_ICONERROR );
		return;
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

CBaseWindow* CBaseWindow::FindWindow( HWND hWnd )
{
	WindowMap::iterator it = s_WindowMap.find( hWnd );
	return s_WindowMap.end() == it ? NULL : it->second;
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
}

void CBaseWindow::Show( void )
{
	W_CHECK3;
	::ShowWindow( _hWnd, SW_SHOW );
}

void CBaseWindow::Hide( void )
{
	W_CHECK3;
	::ShowWindow( _hWnd, SW_HIDE );
}

CBaseWindow::WindowMap CBaseWindow::s_WindowMap;
CBaseWindow* CBaseWindow::s_CurrentWnd = NULL;

MainWindow::MainWindow( LPCTSTR lpszClassName, LPCTSTR lpszWindowName )
:CBaseWindow( lpszClassName, lpszWindowName ),
_glWidget( NULL )
{
	RET( _glWidget = new GLWidget( _hWnd ) );
	_glWidget->Initialize();

	_glWidget->SetPosition( 10, 10, 400, 200 );
}


BOOL MainWindow::OnCreate( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}


BOOL MainWindow::OnResize( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}
