#include "../std.h"
#include "../3D/3d.h"
#include "../OpenGL/OpenGL.h"
#include "JupiterUI.h"
#include "../Gaussian.h"
#include "../OpenGL/glErrorUtil.h"
#include "SHDRRender.h"

bool SHDRRender::OnResize( WPARAM wParam, LPARAM lParam )
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

	return false;
}

bool SHDRRender::OnPaint( WPARAM wParam, LPARAM lParam )
{	
	V_RET( _bInitialized );
	V_RET( this->MakeCurrent() );
	glClear( GL_COLOR_BUFFER_BIT );

	glEnable(GL_TEXTURE_2D); /* enable texture mapping */
	
	FramebufferObject fbo;
	fbo.Bind();
	//CheckErrorsGL();

	SaveMVPMatrices();	
  
	//down sample
	m_pDSTex->AttachToFBO( 0 );	
	m_pHdrTex->Bind();
	m_pDownsampleEffect->Begin();
	m_pDownsampleEffect->SetUniform( "Tex", 0 );
	m_pDSTex->DrawQuad();
	m_pDownsampleEffect->End();

	//blur x
	m_pBlurXTex->AttachToFBO( 0 );
	m_pDSTex->Bind();

	m_pBlurXEffect->Begin();
	m_pBlurXEffect->SetUniform( "Tex", 0 );	
	float stepx = 1.f / m_pBlurXTex->GetTexWidth();
	m_pBlurXEffect->SetUniform( "step", stepx );
	m_pBlurXEffect->SetUniform( "weight", 4, m_GK );	
	m_pBlurXTex->DrawQuad();
	m_pBlurXEffect->End();

	//blur y
	m_pBlurYTex->AttachToFBO( 0 );
	m_pBlurXTex->Bind();

	//m_pBlurYEffect->Begin();
	//m_pBlurYEffect->SetUniform( "Tex", 0 );	
	//float stepy = 1.f / m_pBlurXTex->GetTexHeight();
	//m_pBlurYEffect->SetUniform( "step", stepy );
	//m_pBlurYEffect->SetUniform( "weight", 4, m_GK );	
	m_pBlurYTex->DrawQuad();
	//m_pBlurYEffect->End();

	//display
	FramebufferObject::Disable();
	glDisable( GL_TEXTURE_2D );

	RestoreMVPMatrices();

	glActiveTexture( GL_TEXTURE0 );	
	m_pHdrTex->Bind();
	glActiveTexture( GL_TEXTURE1 );	
	m_pBlurYTex->Bind();

	m_pTonemapingEffect->Begin();
	m_pTonemapingEffect->SetUniform( "fullTex", 0 );
	m_pTonemapingEffect->SetUniform( "bluredTex", 1 );
	m_pTonemapingEffect->SetUniform( "exposureLevel", GetExposure() );

	int left = -m_pHdrTex->GetTexWidth()/2;
	int bottom = -m_pHdrTex->GetTexHeight()/2;
	m_pHdrTex->DrawQuad( left, bottom, -left, -bottom );

	m_pTonemapingEffect->End();

	glActiveTexture( GL_TEXTURE0 );
	glEnable(GL_TEXTURE_2D);
	m_pDSTex->Bind();
	m_pDSTex->FitViewport();
	m_pDSTex->DrawQuad();

	SwapBuffers( _hDC );

	return true;
}

bool SHDRRender::Initialize()
{
	V_RET( this->BasicInitialize() );

	glewInit();

	V_RET( this->InitTexture() );
	V_RET( this->InitRenderTagets() );
	V_RET( this->InitShaders() );

	V_RET( m_GK = gaussian1D<float>( 4, 1 ) );

	_bInitialized = true;
	return true;
}

bool SHDRRender::InitShaders( void )
{
	V_RET( this->m_pDownsampleEffect = new EffectGLSL( "down sample" ) );	
	V_RET( this->m_pDownsampleEffect->Load( NULL, "shaders/ds.fp" ) );

	V_RET( this->m_pBlurXEffect = new EffectGLSL( "blur X" ) );
	V_RET( this->m_pBlurXEffect->Load( NULL, "shaders/bx.fp" ) );	

	V_RET( this->m_pBlurYEffect = new EffectGLSL( "blur Y" ) );
	V_RET( this->m_pBlurYEffect->Load( NULL, "shaders/by.fp" ) );

	V_RET( this->m_pTonemapingEffect = new EffectGLSL( "tone mapping" ) );
	V_RET( this->m_pTonemapingEffect->Load( NULL, "shaders/tm.fp" ) );

	return true;
}

bool SHDRRender::InitRenderTagets( void )
{	
	int width = m_pHdrTex->GetTexWidth();
	int height = m_pHdrTex->GetTexHeight();

	V_RET( m_pFullTex = new GLTexImage() );
	CheckErrorsGL("InitRenderTagets1");
	this->m_pFullTex->Initialize( width, height );
	CheckErrorsGL("InitRenderTagets2");
	V_RET( m_pDSTex = new GLTexImage() );
	this->m_pDSTex->Initialize( width / 2, height / 2 );

	V_RET( m_pBlurXTex = new GLTexImage() );	
	this->m_pBlurXTex->Initialize( width / 2, height / 2  );

	V_RET( m_pBlurYTex = new GLTexImage() );	
	this->m_pBlurYTex->Initialize( width / 2, height / 2 );

	CheckErrorsGL("init rt");
	return true;
}

bool SHDRRender::InitTexture( void )
{
	V_RET( m_pHdrTex = new GLTexInput() );
	V_RET( m_pHdrTex->Load( "RNL.hdr" ) );
	CheckErrorsGL("InitTexture");
	return true;
}

SHDRRender::SHDRRender( HWND hParentWnd )
:GLWidget( hParentWnd ),
m_pDownsampleEffect( NULL ),
m_pBlurXEffect( NULL ),
m_pBlurYEffect( NULL ),
m_pTonemapingEffect( NULL ),
m_pHdrTex( NULL ),
m_pDSTex( NULL ),
m_pBlurXTex( NULL ),
m_pBlurYTex( NULL ),
m_GK( NULL )
{

}

SHDRRender::~SHDRRender()
{
	SAFE_RELEASE( m_pDownsampleEffect );
	SAFE_RELEASE( m_pBlurXEffect );
	SAFE_RELEASE( m_pBlurYEffect );
	SAFE_RELEASE( m_pTonemapingEffect );


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
