#include "std.h"
#include "OpenGL.h"

#include <CommCtrl.h>

#include "resource.h"

#include "JupiterWindow.h"
#include "JupiterControls.h"

BOOL GLWidget::InitGL( void )
{
	F_RET( _hWnd );

	_hDC = GetDC( _hWnd );
	if ( NULL == _hDC )
	{
		MessageBox( NULL, TEXT( "Create GL device context failed"), GL_WIDGET, MB_OK );
		return FALSE;
	}

	if ( !this->SetPixelFormat() )
		return FALSE;

	_hGLRC = wglCreateContext( _hDC );
	if ( NULL == _hGLRC )
	{
		MessageBox( NULL, TEXT( "Can't create a GL rendering context" ), GL_WIDGET, MB_OK );
		return FALSE;
	}

	return TRUE;
}

BOOL GLWidget::SetPixelFormat( void )
{
	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd, 0, sizeof( pfd ) );
	pfd.nSize =  sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER ;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cDepthBits = 32;
	pfd.cColorBits = 32;

	int pixelformat = ChoosePixelFormat( _hDC, &pfd );
	if ( NULL == pixelformat ) 
	{
		MessageBox( NULL, TEXT( "Can't find a suitable pixelformat" ), GL_WIDGET, MB_OK );
		return FALSE;
	}

	if ( !::SetPixelFormat( _hDC, pixelformat, &pfd ) )
	{
		MessageBox( NULL, TEXT( "Can't set the pixelformat" ), GL_WIDGET, MB_OK );
		return FALSE;
	}

	return TRUE;
}

BOOL GLWidget::MakeCurrent( void )
{
	T_RET( this->IsCurrent() );		

	if ( !wglMakeCurrent( _hDC, _hGLRC ) )
	{
		MessageBox( NULL, TEXT( "Can't activate the GL rendering context" ), GL_WIDGET, MB_OK );
		return FALSE;
	}	

	return TRUE;
}

GLWidget::GLWidget( HWND hParentWnd )
:CBaseWindow( GL_WIDGET, NULL, WS_VISIBLE|WS_CHILD, hParentWnd )
{
	InitGL();
}

GLWidget::~GLWidget( void )
{
	if ( NULL != _hGLRC )
	{
		if ( !wglDeleteContext( _hGLRC ) )
		{
			MessageBox(NULL, "Release RC Failed.", "SHUTDOWN ERROR", MB_OK);
		}
		_hGLRC = NULL;
	}

	if ( NULL != _hDC && !ReleaseDC( _hWnd, _hDC ) )
	{
		MessageBox(NULL,"Release DC Failed.","SHUTDOWN ERROR",MB_OK);
		_hDC = NULL;
	}	

	wglMakeCurrent( NULL, NULL );
}

BOOL GLWidget::OnResize( WPARAM wParam, LPARAM lParam )
{
	F_RET( this->MakeCurrent() );

	int width = LOWORD(lParam);
	int height = HIWORD(lParam);

	glViewport( 0, 0, width, height );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	int right = width/2;
	int left = -right;
	int bottom = height/2;
	int top = - bottom;
	glOrtho( left, right, bottom, top, -100, 100 );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	
	return FALSE;
}

BOOL GLWidget::OnPaint( WPARAM wParam, LPARAM lParam )
{
	F_RET( this->MakeCurrent() );
	glClear( GL_COLOR_BUFFER_BIT );

	SwapBuffers( _hDC );
	return FALSE;
}

void GLWidget::SetPosition( int x, int y, int width, int height )
{
	::MoveWindow( _hWnd, x, y, width, height, FALSE );
}

BOOL GLWidget::Initialize( void )
{
	F_RET( this->MakeCurrent() );

	float color = 160.f / 255.f;

	glClearColor( color, color, color, color );	

	return TRUE;
}