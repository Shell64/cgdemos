#include "../std.h"
#include "../Win32.h"
#include "../3D/3d.h"
#include "../OpenGL/OpenGL.h"

#include <CommCtrl.h>

#include "resource.h"

#include "../OpenGL/glErrorUtil.h"
#include "JupiterWindow.h"
#include "JupiterControls.h"

bool GLWidget::InitGL( void )
{
	V_RET( _hWnd );

	_hDC = GetDC( _hWnd );
	if ( NULL == _hDC )
	{
		MessageBox( NULL, TEXT( "Create GL device context failed"), GL_WIDGET, MB_OK );
		return false;
	}
	
	if ( !this->SetPixelFormat() )
		return false;

	_hGLRC = wglCreateContext( _hDC );
	if ( NULL == _hGLRC )
	{
		MessageBox( NULL, TEXT( "Can't create a GL rendering context" ), GL_WIDGET, MB_OK );
		return false;
	}

	V_RET( this->MakeCurrent() );	
	glewInit();

	return true;
}

bool GLWidget::SetPixelFormat( void )
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
		return false;
	}

	if ( !::SetPixelFormat( _hDC, pixelformat, &pfd ) )
	{
		MessageBox( NULL, TEXT( "Can't set the pixelformat" ), GL_WIDGET, MB_OK );
		return false;
	}

	return true;
}

bool GLWidget::MakeCurrent( void )
{
	T_RET( this->IsCurrent() );		

	if ( !wglMakeCurrent( _hDC, _hGLRC ) )
	{
		MessageBox( NULL, TEXT( "Can't activate the GL rendering context" ), GL_WIDGET, MB_OK );
		return false;
	}	

	return true;
}

//must set window background as NULL, otherwise the rendered
//scene may be erased with background color
GLWidget::GLWidget( HWND hParentWnd )
:CBaseWindow( GL_WIDGET, NULL, WS_VISIBLE|WS_CHILD, hParentWnd, NULL ), 
_bInitialized( false ), _hGLRC( NULL ), _hDC( NULL )
{
	InitGL();
}

GLWidget::~GLWidget( void )
{	
	if ( NULL != _hGLRC )
	{
		if ( !wglDeleteContext( _hGLRC ) )
		{
			MessageBox( NULL, "Release RC Failed.", "SHUTDOWN ERROR", MB_OK );
		}
		_hGLRC = NULL;
	}
	wglMakeCurrent( NULL, NULL );

	if ( NULL != _hDC && !ReleaseDC( _hWnd, _hDC ) )
	{
		DWORD err = GetLastError();
		MessageBox( NULL, "Release DC Failed.", "SHUTDOWN ERROR", MB_OK );
		_hDC = NULL;
	}		
}

bool GLWidget::OnResize( WPARAM wParam, LPARAM lParam )
{
	V_RET( this->MakeCurrent() );

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
	
	return false;
}

bool GLWidget::OnPaint( WPARAM wParam, LPARAM lParam )
{
	V_RET( this->MakeCurrent() );
	glClear( GL_COLOR_BUFFER_BIT );

	SwapBuffers( _hDC );
	
	return false;
}

void GLWidget::SetPosition( int x, int y, int width, int height )
{
	::MoveWindow( _hWnd, x, y, width, height, false );
}

bool GLWidget::BasicInitialize( void )
{
	V_RET( this->MakeCurrent() );

	float color = 0.f;

	glClearColor( color, color, color, color );	

	return true;
}

bool GLWidget::Initialize( void )
{
	V_RET( this->BasicInitialize() );

	_bInitialized = true;

	return true;
}

void GLWidget::SaveMVPMatrices( void )
{
	glGetIntegerv( GL_VIEWPORT, s_Viewport );
	glGetDoublev( GL_MODELVIEW_MATRIX, s_ModelView );
	glGetDoublev( GL_PROJECTION_MATRIX, s_Projection );	
}

void GLWidget::RestoreMVPMatrices( void )
{
	glViewport( s_Viewport[0], s_Viewport[1],
		s_Viewport[2], s_Viewport[3] );

	glMatrixMode( GL_PROJECTION );
	glLoadMatrixd( s_Projection );

	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixd( s_ModelView );
}

GLint GLWidget::s_Viewport[4];

GLdouble GLWidget::s_Projection[16];

GLdouble GLWidget::s_ModelView[16];
