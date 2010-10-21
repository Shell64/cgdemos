#include "../std.h"
#include "../3D/3d.h"
#include "../OpenGL/OpenGL.h"
#include "JupiterUI.h"
#include "../Gaussian.h"
#include "HDRRender.h"

HDRRender::HDRRender( HWND hParentWnd )
:GLWidget( hParentWnd ),_camera( Vector3( 0.f, 0.f, 50.f ) ),
_bMove( false ), _etaRatio( 0.8f, 0.8f, 0.8f ),
_matColor( 0.1f, 0.1f, 0.12f ), _pModel( NULL ),
_pMesh( NULL ), _bTrimesh( true )
{
	_exposure = 1.1f;
	_bloomFactor = 0.5f;
	_reflectionFactor = 0.2f;
	_brightThreshold = 1.2f;
	_fresnelBias = 0.2f;
	_fresnelScale = 1.f;
	_fresnelPower = 1.f;
}

HDRRender::~HDRRender(void)
{
	SAFE_RELEASE( _pModel );

	SAFE_RELEASE( _GK );
}

bool HDRRender::Initialize( void )
{
	V_RET( this->BasicInitialize() );
	
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	V_RET( _texCubeHdrInput.Load( "uffizi_cross.hdr" ) );
	V_RET( _texCubeRgbInput.Load( "uffizi_cross.hdr", GL_RGBA ) );			
	V_RET( _texHdrInput.Load( "media/pro.bmp" ) );
	V_RET( _texRgbInput.Load( "media/pro.bmp", GL_RGBA ) );

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

	V_RET( _reflectEffect.Load( "shaders/reflect.vp", "shaders/reflect.fp" ) );
	V_RET( _bloomEffect.Load( NULL, "shaders/extractBloom.fp") );
	V_RET( _blurXEffect.Load( NULL, "shaders/bx.fp" ) );
	V_RET( _blurYEffect.Load( NULL, "shaders/by.fp" ) );
	V_RET( _toneEffect.Load( NULL, "shaders/tone.fp" ) );

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
	RenderRgb();

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

	_rtHdr.Initialize( width, height );
	_rtRgb.Initialize( width, height, GL_RGBA );
	_rtBloom.Initialize( width, height );

	for ( int i = 0; i < s_blurPasses; ++i )
	{
		_rtBlurX[ i ].Initialize( width, height );
		_rtBlurY[ i ].Initialize( width, height );
		width >>= 1;
		height >>= 1;
	}

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

void HDRRender::RenderSkybox( const GLTexCube& cubeTex )
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
	cubeTex.Bind();

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

void HDRRender::RenderMesh( const GLTexCube& cubeTex, const GLTexImage& texImage )
{
	glActiveTexture( GL_TEXTURE0 );
	texImage.Bind();
	glActiveTexture( GL_TEXTURE1 );
	cubeTex.Bind();

	_reflectEffect.Begin();
	_reflectEffect.SetUniform( "env", 1 );
	_reflectEffect.SetUniform( "tex", 0 );
	_reflectEffect.SetUniform( "reflectionFactor", _reflectionFactor );
	/*_reflectEffect.SetUniform( "fresnelBias", 0.2f );
	_reflectEffect.SetUniform( "fresnelScale", 1.0f );
	_reflectEffect.SetUniform( "fresnelPower", 1.0f );
	_reflectEffect.SetUniform( "etaRatio", _etaRatio.x );	*/
	_reflectEffect.SetUniform( "eyePos", _camera.GetEyePos() );
	//_reflectEffect.SetUniform( "etaRatioRGB", _etaRatio );
	_reflectEffect.SetUniform( "matColor", _matColor );

	if ( _bTrimesh )
		RenderTrimesh();
	else
		RenderGLMMesh();

	_reflectEffect.End();

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
	SetRenderTarget( _rtHdr );
	ClearBuffer();
	RenderSkybox( _texCubeHdrInput );
	RenderMesh( _texCubeHdrInput, _texRgbInput );
	
	SaveMVPMatrices();
	glEnable( GL_TEXTURE_2D );
	
	SetRenderTarget( _rtBloom );
	_rtBloom.FitViewport();
	_bloomEffect.Begin();
	_bloomEffect.SetUniform( "tex", 0 );
	_bloomEffect.SetUniform( "brightThreshold", _brightThreshold );
	_rtHdr.Bind();
	_rtBloom.DrawQuad();
	_bloomEffect.End();	

	GLTexAttachment* prev = &_rtBloom;
	for ( int i = 0; i < s_blurPasses; ++i ) {
		SetRenderTarget( _rtBlurX[i] );
		_rtBlurX[i].FitViewport();
		_blurXEffect.Begin();
		prev->Bind();
		float stepx = 1.f / _rtBlurX[i].GetTexWidth();
		_blurXEffect.SetUniform( "step", stepx );
		_blurXEffect.SetUniform( "weight", 4, _GK );
		_rtBlurX[i].DrawQuad();
		_blurXEffect.End();	
	
		SetRenderTarget( _rtBlurY[i] );
		_rtBlurY[i].FitViewport();
		_blurYEffect.Begin();
		_rtBlurX[i].Bind();
		float stepy = 1.f / _rtBlurY[i].GetTexHeight();
		_blurYEffect.SetUniform( "step", stepy );
		_blurYEffect.SetUniform( "weight", 4, _GK );
		_rtBlurY[i].DrawQuad();
		_blurYEffect.End();	
		prev = &_rtBlurY[i];
	
		glEnable( GL_BLEND );
		glBlendFunc( GL_ONE, GL_ONE );
			SetRenderTarget( _rtBloom );
			_rtBlurY[i].Bind();
			_rtBloom.FitViewport();
			_rtBloom.DrawQuad();		
		glDisable( GL_BLEND );
	}

	ResetRenderTarget();
	glActiveTexture( GL_TEXTURE0 );
	_rtHdr.Bind();
	glActiveTexture( GL_TEXTURE1 );
	_rtBloom.Bind();
	_toneEffect.Begin();
	_toneEffect.SetUniform( "brightThreshold", _brightThreshold );
	_toneEffect.SetUniform( "bloomFactor", _bloomFactor );
	_toneEffect.SetUniform( "exposure", _exposure );
	_toneEffect.SetUniform( "tex", 0);
	_toneEffect.SetUniform( "bloom", 1);
	_rtHdr.DrawQuadRight();
	_toneEffect.End();
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glActiveTexture( GL_TEXTURE0 );
		
	RestoreMVPMatrices();
}

void HDRRender::RenderRgb( void )
{		
	SetRenderTarget( _rtRgb );
	ClearBuffer();
	RenderSkybox( _texCubeRgbInput );
	RenderMesh( _texCubeRgbInput, _texRgbInput );	

	ResetRenderTarget();
	SaveMVPMatrices();
	_rtRgb.Bind();
	_rtRgb.FitViewport();
	_rtRgb.DrawQuadLeft();
	RestoreMVPMatrices();
}