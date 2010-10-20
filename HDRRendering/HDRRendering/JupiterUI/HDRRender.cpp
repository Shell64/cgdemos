#include "../std.h"
#include "../3D/3d.h"
#include "../OpenGL/OpenGL.h"
#include "JupiterUI.h"
#include "../Gaussian.h"
#include "HDRRender.h"

HDRRender::HDRRender( HWND hParentWnd )
:GLWidget( hParentWnd ),_camera( Vector3( 0.f, 0.f, 50.f ) ),
_pTexCubeInput( NULL ), _pEffect( NULL ), _pTexture( NULL ),
_bMove( false ), _etaRatio( 0.8f, 0.8f, 0.8f ),
_matColor( 0.1f, 0.1f, 0.12f ), _pModel( NULL ),
_pMesh( NULL ), _bTrimesh( true )
{

}

HDRRender::~HDRRender(void)
{
	SAFE_RELEASE( _pTexCubeInput );
	SAFE_RELEASE( _pEffect );
	SAFE_RELEASE( _pTexture );
	SAFE_RELEASE( _pModel );

	SAFE_RELEASE( _GK );
	/*SAFE_RELEASE( _pMesh );*/
}

bool HDRRender::Initialize( void )
{
	V_RET( this->BasicInitialize() );
	
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	_pTexCubeInput = new GLTexCubeInput();
	_pTexCubeInput->Load( "uffizi_cross.hdr" );

	_pEffect = new EffectGLSL( "reflect effect" );
	_pEffect->Load( "shaders/reflect.vp", "shaders/reflect.fp" );

	_pTexture = new GLTexInput();
	_pTexture->Load( "media/pro.bmp" );	

	if ( _bTrimesh ) 
	{
		_pMesh = TriMeshManager::get()->getTriMesh( "teapot.ms3d" );
	}
	else
	{
		_pModel = glmReadOBJ( "media/buddha/buddha.obj" );
		glmFacetNormals( _pModel );
		glmVertexNormals( _pModel, 90.f );
		glmLinearTexture( _pModel );
	}

	V_RET( bloomEffect.Load( NULL, "shaders/extractBloom.fp") );
	V_RET( blurXEffect.Load( NULL, "shaders/bx.fp" ) );
	V_RET( blurYEffect.Load( NULL, "shaders/by.fp" ) );
	V_RET( toneEffect.Load( NULL, "shaders/tone.fp" ) );

	V_RET( _GK = gaussian1D<float>( 4, 3 ) );

	return true;
}

bool HDRRender::OnPaint( WPARAM wParam, LPARAM lParam )
{
	V_RET( this->MakeCurrent() );

	glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	const Vector3& eyePos = _camera.GetEyePos();
	const Vector3& center = _camera.GetTarget();
	const Vector3& up = _camera.GetUp();
	gluLookAt( eyePos.x, eyePos.y, eyePos.z, 
		center.x, center.y, center.z,
		up.x, up.y, up.z );

	RenderHdr();

	SwapBuffers( _hDC );
	return true;
}

bool HDRRender::OnKeyDown( WPARAM wParam, LPARAM lParam )
{
	const float units = 10.f;
	switch( wParam )
	{
	case VK_UP:
		_camera.Pitch( units );
		break;
	case VK_DOWN:
		_camera.Pitch( -units );
		break;
	case VK_LEFT:
		_camera.Yaw( -units );
		break;
	case VK_RIGHT:
		_camera.Yaw( units );
		break;
	//case 'A':
	//	_camera.Roll( units );
	//	break;
	//case 'D':
	//	_camera.Roll( -units );
	//	break;
	case 'W':
		_camera.Zoom( units );
		break;
	case 'S':
		_camera.Zoom( -units );
		break;
	default:
		break;
	}
	this->OnPaint( NULL, NULL );
	return false;
}

bool HDRRender::OnResize( WPARAM wParam, LPARAM lParam )
{
	int width = LOWORD(lParam);
	int height = HIWORD(lParam);

	glViewport( 0, 0, width, height );
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45.0, width / float( height ), 0.1, 1000.0 );

	rtHdr.Initialize( width, height );
	rtRgb.Initialize( width, height, GL_RGBA );
	rtBloom.Initialize( width, height );
	rtBlurX.Initialize( width, height );
	rtBlurY.Initialize( width, height );

	return false;
}

bool HDRRender::OnLButtonDown( WPARAM wParam, LPARAM lParam )
{
	//int xPos = GET_X_LPARAM(lParam); 
	//int yPos = GET_Y_LPARAM(lParam); 
	//_bMove = true;
	//_lastPos =
	return true;
}

bool HDRRender::OnMouseMove( WPARAM wParam, LPARAM lParam )
{
	//int xPos = GET_X_LPARAM(lParam); 
	//int yPos = GET_Y_LPARAM(lParam); 
	return true;
}

bool HDRRender::OnLButtonUp( WPARAM wParam, LPARAM lParam )
{
	return true;
}

void HDRRender::RenderSkybox( void )
{	
	GLfloat s_plane[] = { 1.0, 0.0, 0.0, 0.0 };
	GLfloat t_plane[] = { 0.0, 1.0, 0.0, 0.0 };
	GLfloat r_plane[] = { 0.0, 0.0, 1.0, 0.0 };
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane);
	glTexGenfv(GL_R, GL_OBJECT_PLANE, r_plane);
	glPopMatrix();

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	_pTexCubeInput->Bind();

	float s = 500.0f;
	glBegin(GL_QUADS); 
	glVertex3f(+s, -s, -s);
	glVertex3f(+s, -s, +s);
	glVertex3f(+s, +s, +s);
	glVertex3f(+s, +s, -s);

	glVertex3f(-s, -s, -s);
	glVertex3f(-s, +s, -s);
	glVertex3f(-s, +s, +s);
	glVertex3f(-s, -s, +s);

	glVertex3f(-s, +s, -s);
	glVertex3f(+s, +s, -s);
	glVertex3f(+s, +s, +s);
	glVertex3f(-s, +s, +s);

	glVertex3f(-s, -s, -s);
	glVertex3f(-s, -s, +s);
	glVertex3f(+s, -s, +s);
	glVertex3f(+s, -s, -s);

	glVertex3f(-s, -s, -s);
	glVertex3f(+s, -s, -s);
	glVertex3f(+s, +s, -s);
	glVertex3f(-s, +s, -s);

	glVertex3f(-s, -s, +s);
	glVertex3f(-s, +s, +s);
	glVertex3f(+s, +s, +s);
	glVertex3f(+s, -s, +s);
	glEnd();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_CUBE_MAP);
}

void HDRRender::RenderMesh( void )
{
	RET( NULL != _pTexCubeInput );
	RET( NULL != _pTexture );
	RET( NULL != _pEffect );
	/*RET( NULL != _pModel );*/

	_pEffect->Begin();
	
	glActiveTexture( GL_TEXTURE0 );
	_pTexture->Bind();
	glActiveTexture( GL_TEXTURE1 );
	_pTexCubeInput->Bind();

	_pEffect->SetUniform( "env", 1 );
	_pEffect->SetUniform( "tex", 0 );
	_pEffect->SetUniform( "reflectionFactor", 0.2f );
	_pEffect->SetUniform( "fresnelBias", 0.2f );
	_pEffect->SetUniform( "fresnelScale", 1.0f );
	_pEffect->SetUniform( "fresnelPower", 1.0f );
	_pEffect->SetUniform( "etaRatio", _etaRatio.x );	
	_pEffect->SetUniform( "eyePos", _camera.GetEyePos() );
	_pEffect->SetUniform( "etaRatioRGB", _etaRatio );
	_pEffect->SetUniform( "matColor", _matColor );
	
	if ( _bTrimesh )
		RenderTrimesh();
	else
		RenderGLMMesh();

	_pEffect->End();

	glActiveTexture(GL_TEXTURE0);
}

void HDRRender::RenderTrimesh( void )
{
	GLsizei stride = sizeof(TriMesh::Vertex);
	glVertexPointer(3, GL_FLOAT, stride, _pMesh->getVertex(0).p);
	glNormalPointer(GL_FLOAT, stride, _pMesh->getVertex(0).n);
	glTexCoordPointer(2, GL_FLOAT, stride, &_pMesh->getVertex(0).u);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawArrays(GL_TRIANGLES, 0, _pMesh->getNumVertices());

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);	
}

void HDRRender::RenderGLMMesh( void )
{
	GLuint mode = GLM_NONE; //reset mode
	bool smooth = true;
	bool two_sided =false;
	bool material = false;
	bool textured = false;

	if(smooth) mode = mode | GLM_SMOOTH;
	else mode = mode | GLM_FLAT;

	if(two_sided) mode = mode | GLM_2_SIDED;

	if(material) mode = mode | GLM_MATERIAL;
	else mode = mode | GLM_COLOR;

	if(textured && material) mode = mode | GLM_TEXTURE;

	glPushMatrix();
		glScalef( 2.0f, 2.0f, 2.0f );
		glmDraw( _pModel, mode ); 
	glPopMatrix();
}

void HDRRender::RenderHdr( void )
{		
	rtHdr.BeginCapture();
	glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
	RenderSkybox();
	RenderMesh();
	rtHdr.EndCapture();
	
	SaveMVPMatrices();
	glEnable( GL_TEXTURE_2D );
	
	rtBloom.BeginCapture();
		rtBloom.FitViewport();
		bloomEffect.Begin();
		bloomEffect.SetUniform( "tex", 0 );
		bloomEffect.SetUniform( "brightThreshold", 1.2f );
		rtHdr.Bind();
		rtBloom.DrawQuad();
		bloomEffect.End();
	rtBloom.EndCapture();

	rtBlurX.BeginCapture();
		rtBlurX.FitViewport();
		blurXEffect.Begin();
		rtBloom.Bind();
		float stepx = 1.f / rtBlurX.GetTexWidth();
		blurXEffect.SetUniform( "step", stepx );
		blurXEffect.SetUniform( "weight", 4, _GK );
		rtBlurX.DrawQuad();
		blurXEffect.End();
	rtBlurX.EndCapture();

	rtBlurY.BeginCapture();
		rtBlurY.FitViewport();
		blurYEffect.Begin();
		rtBlurX.Bind();
		float stepy = 1.f / rtBlurX.GetTexHeight();
		blurYEffect.SetUniform( "step", stepy );
		blurYEffect.SetUniform( "weight", 4, _GK );
		rtBlurY.DrawQuad();
		blurXEffect.End();
	rtBlurY.EndCapture();
			
	glActiveTexture( GL_TEXTURE0 );
	rtHdr.Bind();
	glActiveTexture( GL_TEXTURE1 );
	rtBlurY.Bind();
	toneEffect.Begin();
	toneEffect.SetUniform("brightThreshold", 1.0f );
	toneEffect.SetUniform("bloomFactor", 0.9f );
	toneEffect.SetUniform("exposure", 1.5f );
	toneEffect.SetUniform("tex", 0);
	toneEffect.SetUniform("bloom", 1);
	rtHdr.DrawQuad();
	toneEffect.End();
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glActiveTexture( GL_TEXTURE0 );
	
	RestoreMVPMatrices();
}

void HDRRender::RenderRgb( void )
{

}