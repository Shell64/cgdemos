#include "../std.h"
#include "3d.h"


void CameraBase::Pitch( float angle )
{
	this->rotate( _right, _look, angle );
	this->updateUp();
}

void CameraBase::Yaw( float angle )
{
	this->rotate( _up, _right, angle );	
	this->updateLook();
}

void CameraBase::Roll( float angle )
{
	this->rotate( _look, _up, angle );
	this->updateRight();
}

CameraBase::~CameraBase( void )
{

}
////_right = cross( _look, _up )
//Camera::Camera( const Vector3& pos )
//	: _eyePos( pos ), _look( 0.f, 0.f, -1.f ),
//	_up( 0.f, 1.f, 0.f ),
//	_right( 1.f, 0.f, 0.f )
//{
//}
//
//Camera::~Camera()
//{
//}
//
//void Camera::Strafe( float units )
//{
//	_eyePos += _right * units;
//}
//
//void Camera::Fly( float units )
//{
//	_eyePos += _up * units;
//}
//
//void Camera::Walk( float units )
//{
//	_eyePos += _look * units;
//}
//
//void Camera::SetCamera( const Vector3& look, const Vector3& up )
//{
//	this->SetLook( look );
//	this->SetUp( up );
//	this->updateRight();
//}


