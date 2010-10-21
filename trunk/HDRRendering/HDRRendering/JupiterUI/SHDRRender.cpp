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

	SaveMVPMatrices();	
  
	//down sample
	_rtDownsample.BeginCapture();
	_hdrTex.Bind();
	_downSampleEffect.Begin();
	_downSampleEffect.SetUniform( "Tex", 0 );
	_rtDownsample.DrawQuad();
	_downSampleEffect.End();
	_rtDownsample.EndCapture();

	//blur x
	_rtBlurX.BeginCapture();
	_rtDownsample.Bind();
	_blurXEffect.Begin();
	_blurXEffect.SetUniform( "Tex", 0 );	
	float stepx = 1.f / _rtBlurX.GetTexWidth();
	_blurXEffect.SetUniform( "step", stepx );
	_blurXEffect.SetUniform( "weight", 4, m_GK );	
	_rtBlurX.DrawQuad();
	_blurXEffect.End();
	_rtBlurX.EndCapture();

	//blur y
	_rtBlurY.BeginCapture();
	_rtBlurX.Bind();
	_blurYEffect.Begin();
	_blurYEffect.SetUniform( "Tex", 0 );	
	float stepy = 1.f / _rtBlurX.GetTexHeight();
	_blurYEffect.SetUniform( "step", stepy );
	_blurYEffect.SetUniform( "weight", 4, m_GK );	
	_rtBlurY.DrawQuad();
	_blurYEffect.End();
	_rtBlurY.EndCapture();

	//display
	FramebufferObject::Disable();
	glDisable( GL_TEXTURE_2D );

	RestoreMVPMatrices();

	glActiveTexture( GL_TEXTURE0 );	
	_hdrTex.Bind();
	glActiveTexture( GL_TEXTURE1 );	
	_rtBlurY.Bind();

	_toneEffect.Begin();
	_toneEffect.SetUniform( "fullTex", 0 );
	_toneEffect.SetUniform( "bluredTex", 1 );
	_toneEffect.SetUniform( "exposureLevel", GetExposure() );

	int left = -_hdrTex.GetTexWidth()/2;
	int bottom = -_hdrTex.GetTexHeight()/2;
	_hdrTex.DrawQuad( left, bottom, -left, -bottom );

	_toneEffect.End();

	glActiveTexture( GL_TEXTURE0 );
	glEnable(GL_TEXTURE_2D);

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
	V_RET( this->_downSampleEffect.Load( NULL, "shaders/ds.fp" ) );

	V_RET( this->_blurXEffect.Load( NULL, "shaders/bx.fp" ) );	

	V_RET( this->_blurYEffect.Load( NULL, "shaders/by.fp" ) );

	V_RET( this->_toneEffect.Load( NULL, "shaders/tm.fp" ) );

	return true;
}

bool SHDRRender::InitRenderTagets( void )
{	
	int width = _hdrTex.GetTexWidth();
	int height = _hdrTex.GetTexHeight();

	V_RET( _rtDownsample.Initialize( width / 2, height / 2 ) );
	
	V_RET( _rtBlurX.Initialize( width / 2, height / 2  ) );
	
	V_RET( _rtBlurY.Initialize( width / 2, height / 2 ) );
	
	return true;
}

bool SHDRRender::InitTexture( void )
{	
	V_RET( _hdrTex.Load( "RNL.hdr" ) );
	return true;
}

SHDRRender::SHDRRender( HWND hParentWnd )
:GLWidget( hParentWnd ),
m_GK( NULL )
{

}

SHDRRender::~SHDRRender()
{
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
