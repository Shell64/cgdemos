#include "../std.h"
#include "3d.h"


//_right = cross( _look, _up )
Camera::Camera( const Vector3& pos )
	: _pos( pos ), _look( 0.f, 0.f, -1.f ),
	_up( 0.f, 1.f, 0.f ),
	_right( 1.f, 0.f, 0.f )
{		
}

Camera::~Camera()
{
}

void Camera::Strafe( float units )
{
	_pos += _right * units;
}

void Camera::Fly( float units )
{
	_pos += _up * units;
}

void Camera::Walk( float units )
{
	_pos += _look * units;
}

void Camera::Pitch( float angle )
{
	this->Rotate( _right, _look, _up, angle );	
}

void Camera::Yaw( float angle )
{
	this->Rotate( _up, _right, _look, angle );	
}

void Camera::Roll( float angle )
{
	this->Rotate( _look, _up, _right, angle );
}