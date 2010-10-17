#include "../std.h"
#include "../3D/3d.h"
#include "../OpenGL/OpenGL.h"
#include "JupiterUI.h"
#include "HDRRender.h"

HDRRender::HDRRender( HWND hParentWnd )
:GLWidget( hParentWnd ),_camera( Vector3( 0.f, 2.f, 0.f ) )
{

}

HDRRender::~HDRRender(void)
{
	SAFE_RELEASE( _pTexCubeInput );
}

bool HDRRender::Initialize( void )
{
	V_RET( this->BasicInitialize() );

	glewInit();

	_pTexCubeInput = new GLTexCubeInput("uffizi_cross.hdr");
	return true;
}

bool HDRRender::OnPaint( WPARAM wParam, LPARAM lParam )
{
	V_RET( this->MakeCurrent() );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	Vector3 center = _camera.Pos() + _camera.Look();
	gluLookAt( _camera.Pos().x, _camera.Pos().y, _camera.Pos().z,
		center.x, center.y, center.z, 
		_camera.Up().x, _camera.Up().y, _camera.Up().z );

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
	_pTexCubeInput->BindTex();

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
	/*case 'W':
		_camera.Walk( units );
		break;
	case 'S':
		_camera.Walk( -units );
		break;*/
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
	gluPerspective( 90.0, 1.0, 1.0, 1000.0 );

	return false;
}