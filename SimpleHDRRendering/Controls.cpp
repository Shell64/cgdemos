#ifndef OEMRESOURCE
#define OEMRESOURCE
#endif
#include <Windows.h>

#include <GL/gl.h>
#include <CommCtrl.h>
#include <assert.h>
#include <exception>

#include "resource.h"
#include "cv.h"
#include "rgbe.h"
#include "ShaderObject.h"
#include "ProgramGLSL.h"
#include "GLTexImage.h"

#include "Controls.h"

LRESULT CALLBACK DisplayWidgetProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	static bool bMouseEntered = false;

	DisplayWidget* pDisplayWidget = DisplayWidget::GetWidget( hWnd );
	// Handle some specific messages:
	switch( msg )
	{
	case WM_PAINT:
		pDisplayWidget->Display();
		//there're other operations need to be done by default
		break;
		
	case WM_RBUTTONUP:
		pDisplayWidget->RButtonUpProc( wParam, lParam );
		return 0;

	case WM_LBUTTONDOWN:	
		pDisplayWidget->LButtonDownProc( wParam, lParam );
		return 0;

	case WM_LBUTTONUP:
		pDisplayWidget->LButtonUpProc( wParam, lParam );
		return 0;

	case WM_MOUSEMOVE:
		pDisplayWidget->MouseMoveProc( wParam, lParam );
		return 0;

	case WM_MOUSELEAVE:
		pDisplayWidget->MouseLeaveProc( wParam, lParam );
		return 0;
		
	case WM_DESTROY: 
		::PostQuitMessage(0); 
		return 0;

	case WM_SIZE:	
		pDisplayWidget->Reshape( LOWORD(lParam), HIWORD(lParam) );
		//if the following sentence is eliminated, there'll be no
		//WM_PAINT received when shrink the size of the control,
		//though it works normally when expand the size of the control
		InvalidateRect( hWnd, NULL, TRUE );

		return 0;
	}

	// Forward any other messages we didn't handle
	// above to the default window procedure.
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL InitControls( HINSTANCE hInstance )
{
	WNDCLASS wndClass = { 0 };
	BOOL bSuccess = TRUE;

	wndClass.style = 0;
	wndClass.lpfnWndProc = DisplayWidgetProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = NULL;
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = WC_DISPLAYWIDGET;

	if ( !RegisterClass( &wndClass ) ) 
	{
		MessageBox( NULL, TEXT( "RegisterClass failed!" ), WC_DISPLAYWIDGET, MB_OK|MB_ICONERROR );
		bSuccess = FALSE;
	}

	return bSuccess;
}


DisplayWidget::DisplayWidgetMap DisplayWidget::s_DWM;

DisplayWidget::DisplayWidget( HWND hWndParent )
: m_hWnd( NULL ), m_hDC( NULL ), 
m_hGLRC( NULL )
{	
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle( NULL );
	m_hWnd = CreateWindow( 
		WC_DISPLAYWIDGET, NULL,
		WS_CHILD | WS_VISIBLE, 
		0, 0, 0, 0, hWndParent, 
		NULL, hInstance, NULL );

	if ( NULL == m_hWnd ) 
	{
		MessageBox( NULL, TEXT( "Create Window failed" ), WC_DISPLAYWIDGET, MB_OK );
	}

	if ( this->InitGL() ) 
	{		
		s_DWM[ m_hWnd ] = this;		
	}
}

DisplayWidget::~DisplayWidget( void )
{
	s_DWM.erase( m_hWnd );

	if ( NULL != m_hGLRC )
	{
		if ( !wglDeleteContext( m_hGLRC ) )
		{
			MessageBox(NULL, "Release RC Failed.", "SHUTDOWN ERROR", MB_OK);
		}
		m_hGLRC = NULL;
	}

	if ( NULL != m_hDC && !ReleaseDC( m_hWnd, m_hDC ) )
	{
		MessageBox(NULL,"Release DC Failed.","SHUTDOWN ERROR",MB_OK);
		m_hDC = NULL;
	}

	if ( NULL != m_hWnd && !DestroyWindow( m_hWnd ) )
	{
		MessageBox( NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK );
		m_hWnd = NULL;
	}

	wglMakeCurrent( NULL, NULL );
}

BOOL DisplayWidget::InitGL( void )
{
	if ( NULL == m_hWnd )
		return FALSE;

	m_hDC = GetDC( m_hWnd );
	if ( NULL == m_hDC )
	{
		MessageBox( NULL, TEXT( "Create GL device context failed"), WC_DISPLAYWIDGET, MB_OK );
		return FALSE;
	}

	if ( !this->SetPixelFormat() )
		return FALSE;

	m_hGLRC = wglCreateContext( m_hDC );
	if ( NULL == m_hGLRC )
	{
		MessageBox( NULL, TEXT( "Can't create a GL rendering context" ), WC_DISPLAYWIDGET, MB_OK );
		return FALSE;
	}

	return TRUE;
}

BOOL DisplayWidget::SetPixelFormat( void )
{
	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd, 0, sizeof( pfd ) );
	pfd.nSize =  sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER ;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cDepthBits = 32;
	pfd.cColorBits = 32;

	int pixelformat = ChoosePixelFormat( m_hDC, &pfd );
	if ( NULL == pixelformat ) 
	{
		MessageBox( NULL, TEXT( "Can't find a suitable pixelformat" ), WC_DISPLAYWIDGET, MB_OK );
		return FALSE;
	}

	if ( !::SetPixelFormat( m_hDC, pixelformat, &pfd ) )
	{
		MessageBox( NULL, TEXT( "Can't set the pixelformat" ), WC_DISPLAYWIDGET, MB_OK );
		return FALSE;
	}

	return TRUE;
}

BOOL DisplayWidget::MakeCurrent( void )
{
	if ( this->IsCurrent() )
		return TRUE;

	if ( !wglMakeCurrent( m_hDC, m_hGLRC ) )
	{
		MessageBox( NULL, TEXT( "Can't activate the GL rendering context" ), WC_DISPLAYWIDGET, MB_OK );
		return FALSE;
	}	

	return TRUE;
}

void DisplayWidget::Reshape( const int width, const int height )
{
	if ( !this->MakeCurrent() ) return;
	glViewport( 0, 0, width, height );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	int right = width / 2;
	int left = -right;
	int bottom = height / 2;
	int top = - bottom;
	glOrtho( left, right, bottom, top, -100, 100 );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void DisplayWidget::Display( void )
{	
	if ( !this->MakeCurrent() ) return;
	glClear( GL_COLOR_BUFFER_BIT );	

	SwapBuffers(m_hDC);
}

BOOL DisplayWidget::SetPosition( 
								const int x, const int y, 
								const int width, const int height )
{		
	MoveWindow( m_hWnd, x, y, width, height, TRUE );
	
	return TRUE;
}

BOOL DisplayWidget::Initialize()
{	
	if ( !this->MakeCurrent() ) return FALSE;

	float color = 160.f / 255.f;
			
	glClearColor( color, color, color, color );	

	return TRUE;
}

BOOL DisplayWidget::LButtonDownProc( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL DisplayWidget::MouseMoveProc( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL DisplayWidget::MouseLeaveProc( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL DisplayWidget::LButtonUpProc( WPARAM wParam, LPARAM lParam )
{	
	return TRUE;
}

BOOL DisplayWidget::RButtonUpProc( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

BOOL DisplayWidget::KeyDownProc( WPARAM wParam, LPARAM lParam )
{	
	return TRUE;
}

BOOL DisplayWidget::KeyUpProc( WPARAM wParam, LPARAM lParam )
{	
	return TRUE;
}

BOOL DisplayWidget::RButtonDownProc( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

DisplayWidget* DisplayWidget::GetWidget( HWND hWnd )
{
	try
	{
		return s_DWM[ hWnd ];
	}	
	catch ( std::exception )
	{
		return NULL;
	}
}

void SceneRender::Reshape( const int width, const int height )
{
	if ( !this->MakeCurrent() ) return;

	DisplayWidget::Reshape( width, height );
}

void SceneRender::Display()
{
	if ( !this->MakeCurrent() ) return;
	glClear( GL_COLOR_BUFFER_BIT );
	
	glBindTexture( GL_TEXTURE_2D, m_TexID );
	glBegin(GL_POLYGON);
		glTexCoord2f( 0.f, 0.f ); glVertex3d( -m_nWidth/2, -m_nHeight/2, 0 );
		glTexCoord2f( 0.f, 1.f ); glVertex3d( -m_nWidth/2, m_nHeight/2, 0 );
		glTexCoord2f( 1.f, 1.f ); glVertex3d( m_nWidth/2, m_nHeight/2, 0 );
		glTexCoord2f( 1.f, 0.f ); glVertex3d( m_nWidth/2, -m_nHeight/2, 0); 
	glEnd();
	glBindTexture( GL_TEXTURE_2D, 0 );

	glFlush();
	SwapBuffers( m_hDC );
}

SceneRender::SceneRender( HWND hParentWnd )
:DisplayWidget( hParentWnd ),
 m_TexID( 0 ), m_nWidth( 0 ), m_nHeight( 0 ),
 m_DownsampleProgram( "Down sample" ),
 m_BlurXProgram( "Blur X" ),
 m_BlurYProgram( "Blur Y" ),
 m_TonemapProgram( "Tone Mapping" )
{
	
}

SceneRender::~SceneRender()
{

}

BOOL SceneRender::InitTexture( void )
{
	FILE* fd = fopen( "RNL.hdr", "rb" );
	rgbe_header_info header;
	int width, height;
	RGBE_ReadHeader( fd, &width, &height, &header );
	float* pixels = new float[ 3 * width * height ];
	memset( pixels, 0, 3 * width * height * sizeof( float ) );
	RGBE_ReadPixels_RLE( fd, pixels, width, height );	
	fclose( fd );

	m_nWidth = width; m_nHeight = height;

	glGenTextures( 1, &m_TexID );
	glBindTexture( GL_TEXTURE_2D, m_TexID );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); 
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, //internal format changed
		width, height, 0,
		GL_RGB, GL_FLOAT, pixels );

	glBindTexture( GL_TEXTURE_2D, 0 );

	return TRUE;
}

BOOL SceneRender::Initialize( void )
{
	V_RET( this->MakeCurrent() );	
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	

	V_RET( this->InitTexture() );
	V_RET( this->InitRenderTagets() );
	V_RET( this->InitShaders() );

	return TRUE;
}

BOOL SceneRender::InitShaders( void )
{
	this->m_DownsampleProgram.AttachShaderObject(
		ShaderObject( GL_VERTEX_SHADER, "DS_VS.glsl", true ) );
	this->m_DownsampleProgram.AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "DS_FS.glsl", true ) );
	V_RET( this->m_DownsampleProgram.LinkProgram() );

	this->m_BlurXProgram.AttachShaderObject(
		ShaderObject( GL_VERTEX_SHADER, "BX_VS.glsl", true ) );
	this->m_BlurXProgram.AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "BX_FS.glsl", true ) );
	V_RET( this->m_BlurXProgram.LinkProgram() );

	this->m_BlurYProgram.AttachShaderObject(
		ShaderObject( GL_VERTEX_SHADER, "BY_VS.glsl", true ) );
	this->m_BlurYProgram.AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "BY_FS.glsl", true ) );
	V_RET( this->m_BlurYProgram.LinkProgram() );

	this->m_TonemapProgram.AttachShaderObject(
		ShaderObject( GL_VERTEX_SHADER, "TM_VS.glsl", true ) );
	this->m_TonemapProgram.AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "TM_FS.glsl", true ) );
	V_RET( this->m_TonemapProgram.LinkProgram() );

	return TRUE;
}

BOOL SceneRender::InitRenderTagets( void )
{	
	this->m_DSTex.InitTexture( m_nWidth / 2, m_nHeight / 2 );
	this->m_BlurXTex.InitTexture( m_nWidth / 2, m_nHeight / 2 );
	this->m_BlurYTex.InitTexture( m_nWidth / 2, m_nHeight / 2 );
	return TRUE;
}

