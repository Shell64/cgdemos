#ifndef OEMRESOURCE
#define OEMRESOURCE
#endif
#include <Windows.h>

#include <GL/glew.h>
#include <CommCtrl.h>
#include <assert.h>
#include <exception>
#include <iostream>
#include <map>

#include "glErrorUtil.h"

#include "resource.h"
#include "rgbe.h"
#include "ShaderObject.h"
#include "ProgramGLSL.h"
#include "GLTexImage.h"
#include "FramebufferObject.h"
#include "Gaussian.h"

#include "Controls.h"

LRESULT CALLBACK DisplayWidgetProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	static bool bMouseEntered = false;

	DisplayWidget* pDisplayWidget = DisplayWidget::GetWidget( hWnd );
	// Handle some specific messages:
	switch( msg )
	{
	case WM_ERASEBKGND:
		bMouseEntered = false;
		break;
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
		if ( NULL != pDisplayWidget )
			pDisplayWidget->Reshape( LOWORD(lParam), HIWORD(lParam) );
		//if the following sentence is eliminated, there'll be no
		//WM_PAINT received when shrink the size of the control,
		//though it works normally when expand the size of the control
		//InvalidateRect( hWnd, NULL, TRUE );

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
	MoveWindow( m_hWnd, x, y, width, height, FALSE );
	
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
	RET( this->MakeCurrent() );

	m_nWidth = width;
	m_nHeight = height;
}

void SceneRender::UpdateViewport()
{
	int width = m_nWidth;
	int height = m_nHeight;

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
}

void SceneRender::Display()
{
	RET( this->MakeCurrent() );
	glClear( GL_COLOR_BUFFER_BIT );
	
	glEnable(GL_TEXTURE_2D); /* enable texture mapping */

	FramebufferObject fbo;
	fbo.Bind();
	
	//draw full
	m_pFullTex->AttachToFBO( 0 );
	m_pFullTex->FitViewport();
	m_pHdrTex->BindTex();
	m_pFullTex->DrawQuad();

	//down sample
	m_pDSTex->AttachToFBO( 0 );
	m_pFullTex->BindTex();
	GLint texParam = m_pDownsampleProgram->GetUniformLocation( "Tex" );
	m_pDownsampleProgram->UseProgram();
	glUniform1i( texParam, 0 );
	m_pDSTex->DrawQuad();

	//blur x
	m_pBlurXTex->AttachToFBO( 0 );
	m_pDSTex->BindTex();

	m_pBlurXProgram->UseProgram();
	texParam = m_pBlurXProgram->GetUniformLocation( "Tex" );	
	glUniform1i( texParam, 0 );

	GLint stepParam = m_pBlurXProgram->GetUniformLocation( "step" );
	float step = 1.f / m_pBlurXTex->GetTexWidth();
	glUniform1f( stepParam, step );

	GLint gkParam = m_pBlurXProgram->GetUniformLocation( "weight" );
	glUniform1fv( gkParam, 4, m_GK );

	m_pBlurXTex->DrawQuad();

	//blur y
	m_pBlurYTex->AttachToFBO( 0 );
	m_pBlurXTex->BindTex();

	m_pBlurYProgram->UseProgram();
	texParam = m_pBlurYProgram->GetUniformLocation( "Tex" );	
	glUniform1i( texParam, 0 );

	stepParam = m_pBlurYProgram->GetUniformLocation( "step" );
	step = 1.f / m_pBlurYTex->GetTexHeight();
	glUniform1f( stepParam, step );	

	gkParam = m_pBlurYProgram->GetUniformLocation( "weight" );
	glUniform1fv( gkParam, 4, m_GK );

	m_pBlurYTex->DrawQuad();
	
	//display
	FramebufferObject::Disable();

	this->UpdateViewport();
	glActiveTextureARB( GL_TEXTURE0_ARB );
	//glEnable( GL_TEXTURE_2D );
	m_pFullTex->BindTex();
	glActiveTextureARB( GL_TEXTURE1_ARB );
	//glEnable( GL_TEXTURE_2D );
	m_pBlurYTex->BindTex();

	m_pTonemapProgram->UseProgram();

	GLint fullTexParam = m_pTonemapProgram->GetUniformLocation( "fullTex" );
	glUniform1i( fullTexParam, 0 );

	GLint bluredTexParam = m_pTonemapProgram->GetUniformLocation( "bluredTex" );
	glUniform1i( bluredTexParam, 1 );

	GLint exposureParam = m_pTonemapProgram->GetUniformLocation( "exposureLevel" );
	static float exposure = 2.5f;
	if ( exposure < 0.f )
		exposure = 0.01f;
	else if ( exposure <= 10.f )
		exposure += 0.01f;
	else exposure = 0.01f;
	glUniform1f( exposureParam, exposure );
	
	int left = - m_pFullTex->GetTexWidth()/2;
	int bottom = -m_pFullTex->GetTexHeight()/2;
	m_pHdrTex->DrawQuad( left, bottom, -left, -bottom );
	
	glUseProgram( 0 );	

	glActiveTextureARB( GL_TEXTURE1_ARB );
	glDisable( GL_TEXTURE_2D );
	glActiveTextureARB( GL_TEXTURE0_ARB );
	glDisable( GL_TEXTURE_2D );
	

	SwapBuffers( m_hDC );
}

SceneRender::SceneRender( HWND hParentWnd )
:DisplayWidget( hParentWnd ),
 m_TexID( 0 ), m_nWidth( 0 ), m_nHeight( 0 ),
 m_pDownsampleProgram( NULL ),
 m_pBlurXProgram( NULL ),
 m_pBlurYProgram( NULL ),
 m_pTonemapProgram( NULL ),
 m_pHdrTex( NULL ),
 m_pFullTex( NULL ),
 m_pDSTex( NULL ),
 m_pBlurXTex( NULL ),
 m_pBlurYTex( NULL ),
 m_GK( NULL )
{
	
}

SceneRender::~SceneRender()
{
	SAFE_RELEASE( m_pDownsampleProgram );
	SAFE_RELEASE( m_pBlurXProgram );
	SAFE_RELEASE( m_pBlurYProgram );
	SAFE_RELEASE( m_pTonemapProgram );


	SAFE_RELEASE( m_pHdrTex );
	SAFE_RELEASE( m_pFullTex );
	SAFE_RELEASE( m_pDSTex );
	SAFE_RELEASE( m_pBlurXTex );
	SAFE_RELEASE( m_pBlurYTex );

	SAFE_RELEASE( m_GK );
}

BOOL SceneRender::InitTexture( void )
{
	V_RET( m_pHdrTex = new GLTexInput() );
	V_RET( m_pHdrTex->LoadImageFromFile( "RNL.hdr" ) );

	return TRUE;
}

BOOL SceneRender::Initialize( void )
{
	V_RET( this->MakeCurrent() );	
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );	

	glewInit();
	V_RET( this->InitTexture() );
	V_RET( this->InitRenderTagets() );
	V_RET( this->InitShaders() );

	V_RET( m_GK = gaussian1D<float>( 4, 1 ) );
	
	return TRUE;
}

BOOL SceneRender::InitShaders( void )
{
	V_RET( this->m_pDownsampleProgram = new ProgramGLSL( "down sample" ) );
	this->m_pDownsampleProgram->AttachShaderObject(
		ShaderObject( GL_VERTEX_SHADER, "DS_VS.glsl", true ) );
	this->m_pDownsampleProgram->AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "DS_FS.glsl", true ) );
	V_RET( this->m_pDownsampleProgram->LinkProgram() );

	V_RET( this->m_pBlurXProgram = new ProgramGLSL( "blur X" ) );
	this->m_pBlurXProgram->AttachShaderObject(
		ShaderObject( GL_VERTEX_SHADER, "BX_VS.glsl", true ) );
	this->m_pBlurXProgram->AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "BX_FS.glsl", true ) );
	V_RET( this->m_pBlurXProgram->LinkProgram() );

	V_RET( this->m_pBlurYProgram = new ProgramGLSL( "blur Y" ) );
	this->m_pBlurYProgram->AttachShaderObject(
		ShaderObject( GL_VERTEX_SHADER, "BY_VS.glsl", true ) );
	this->m_pBlurYProgram->AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "BY_FS.glsl", true ) );
	V_RET( this->m_pBlurYProgram->LinkProgram() );

	V_RET( this->m_pTonemapProgram = new ProgramGLSL( "tone mapping" ) );
	this->m_pTonemapProgram->AttachShaderObject(
		ShaderObject( GL_VERTEX_SHADER, "TM_VS.glsl", true ) );
	this->m_pTonemapProgram->AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "TM_FS.glsl", true ) );
	V_RET( this->m_pTonemapProgram->LinkProgram() );

	return TRUE;
}

BOOL SceneRender::InitRenderTagets( void )
{	
	int width = m_pHdrTex->GetTexWidth();
	int height = m_pHdrTex->GetTexHeight();

	V_RET( m_pFullTex = new GLTexImage() );
	this->m_pFullTex->InitTexture( width, height );

	V_RET( m_pDSTex = new GLTexImage() );
	this->m_pDSTex->InitTexture( width / 2, height / 2 );

	V_RET( m_pBlurXTex = new GLTexImage() );	
	this->m_pBlurXTex->InitTexture( width / 2, height / 2  );

	V_RET( m_pBlurYTex = new GLTexImage() );	
	this->m_pBlurYTex->InitTexture( width / 2, height / 2 );
	return TRUE;
}

