#include "../std.h"
#include "../OpenGL/OpenGL.h"
#include "../Gaussian.h"

#include <CommCtrl.h>

#include "resource.h"

#include "../OpenGL/glErrorUtil.h"
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

//must set window background as NULL, otherwise the rendered
//scene may be erased with background color
GLWidget::GLWidget( HWND hParentWnd )
:CBaseWindow( GL_WIDGET, NULL, WS_VISIBLE|WS_CHILD, hParentWnd, NULL )
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


BOOL SHDRRender::OnResize( WPARAM wParam, LPARAM lParam )
{	
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

BOOL SHDRRender::OnPaint( WPARAM wParam, LPARAM lParam )
{
	F_RET( this->MakeCurrent() );
	glClear( GL_COLOR_BUFFER_BIT );

	glEnable(GL_TEXTURE_2D); /* enable texture mapping */	
	FramebufferObject fbo;
	fbo.Bind();

	SaveMVPMatrices();	

	//down sample
	m_pDSTex->AttachToFBO( 0 );	
	m_pHdrTex->BindTex();
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
	glDisable( GL_TEXTURE_2D );

	RestoreMVPMatrices();

	glActiveTextureARB( GL_TEXTURE0_ARB );	
	m_pHdrTex->BindTex();
	glActiveTextureARB( GL_TEXTURE1_ARB );	
	m_pBlurYTex->BindTex();

	m_pTonemapProgram->UseProgram();

	GLint fullTexParam = m_pTonemapProgram->GetUniformLocation( "fullTex" );
	glUniform1i( fullTexParam, 0 );

	GLint bluredTexParam = m_pTonemapProgram->GetUniformLocation( "bluredTex" );
	glUniform1i( bluredTexParam, 1 );

	GLint exposureParam = m_pTonemapProgram->GetUniformLocation( "exposureLevel" );
	
	glUniform1f( exposureParam, GetExposure() );

	int left = -m_pHdrTex->GetTexWidth()/2;
	int bottom = -m_pHdrTex->GetTexHeight()/2;
	m_pHdrTex->DrawQuad( left, bottom, -left, -bottom );

	glUseProgram( 0 );	

	//only need to disable GL_TEXTURE0_ARB, otherwise
	//it'll affect the normal texture operation
	glActiveTextureARB( GL_TEXTURE0_ARB );
	glDisable( GL_TEXTURE_2D );

	SwapBuffers( _hDC );

	return TRUE;
}

BOOL SHDRRender::Initialize()
{
	F_RET( GLWidget::Initialize() );

	glewInit();
	V_RET( this->InitTexture() );
	V_RET( this->InitRenderTagets() );
	V_RET( this->InitShaders() );

	V_RET( m_GK = gaussian1D<float>( 4, 1 ) );

	return TRUE;
}

BOOL SHDRRender::InitShaders( void )
{
	V_RET( this->m_pDownsampleProgram = new ProgramGLSL( "down sample" ) );	
	this->m_pDownsampleProgram->AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "shaders/DS_FS.glsl", true ) );
	V_RET( this->m_pDownsampleProgram->LinkProgram() );

	V_RET( this->m_pBlurXProgram = new ProgramGLSL( "blur X" ) );
	this->m_pBlurXProgram->AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "shaders/BX_FS.glsl", true ) );
	V_RET( this->m_pBlurXProgram->LinkProgram() );

	V_RET( this->m_pBlurYProgram = new ProgramGLSL( "blur Y" ) );
	this->m_pBlurYProgram->AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "shaders/BY_FS.glsl", true ) );
	V_RET( this->m_pBlurYProgram->LinkProgram() );

	V_RET( this->m_pTonemapProgram = new ProgramGLSL( "tone mapping" ) );
	this->m_pTonemapProgram->AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "shaders/TM_FS.glsl", true ) );
	V_RET( this->m_pTonemapProgram->LinkProgram() );

	return TRUE;
}

BOOL SHDRRender::InitRenderTagets( void )
{	
	int width = m_pHdrTex->GetTexWidth();
	int height = m_pHdrTex->GetTexHeight();

	V_RET( m_pDSTex = new GLTexImage() );
	this->m_pDSTex->InitTexture( width / 2, height / 2 );

	V_RET( m_pBlurXTex = new GLTexImage() );	
	this->m_pBlurXTex->InitTexture( width / 2, height / 2  );

	V_RET( m_pBlurYTex = new GLTexImage() );	
	this->m_pBlurYTex->InitTexture( width / 2, height / 2 );
	return TRUE;
}

BOOL SHDRRender::InitTexture( void )
{
	V_RET( m_pHdrTex = new GLTexInput() );
	V_RET( m_pHdrTex->LoadImageFromFile( "RNL.hdr" ) );

	return TRUE;
}

SHDRRender::SHDRRender( HWND hParentWnd )
:GLWidget( hParentWnd )
{

}

SHDRRender::~SHDRRender()
{
	SAFE_RELEASE( m_pDownsampleProgram );
	SAFE_RELEASE( m_pBlurXProgram );
	SAFE_RELEASE( m_pBlurYProgram );
	SAFE_RELEASE( m_pTonemapProgram );


	SAFE_RELEASE( m_pHdrTex );
	SAFE_RELEASE( m_pDSTex );
	SAFE_RELEASE( m_pBlurXTex );
	SAFE_RELEASE( m_pBlurYTex );

	SAFE_RELEASE( m_GK );
}

float SHDRRender::GetExposure( void )
{
	static float exposure = 2.5f;
	if ( exposure < 0.f )
		exposure = 0.01f;
	else if ( exposure <= 10.f )
		exposure += 0.01f;
	else exposure = 0.01f;
	return exposure;
}