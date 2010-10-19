#ifndef __CAMERA_H__
#define __CAMERA_H__

class Vector3;
class Matrix4;

class CameraBase
{
public:	
	virtual ~CameraBase( void ) = 0;

	void Pitch( float angle );	//rotate on right vector
	void Yaw( float angle );	//rotate on up vector
	void Roll( float angle );	//rotate on look vector	

public:
	inline const Vector3& GetRight( void ) const { return _right; }
	inline const Vector3& GetUp( void ) const { return _up; }
	inline const Vector3& GetLook( void ) const { return _look; }
	inline const Vector3& GetEyePos( void ) const { return _eyePos; }

protected:
	inline void SetLook( const Vector3& look ) { 
		_look = look; 
		_look.normalize();
		this->updateRight();
		this->updateUp();
	}
	
	inline void SetUp( const Vector3& up ) { 
		_up = up; 
		_up.normalize();
		this->updateRight();
		this->updateUp();
	}

	inline void SetEyePos( const Vector3& eyePos ) { _eyePos = eyePos; }

	inline void rotate(const Vector3& axis, Vector3& v1, float angle )
	{
		Matrix4 mat = mat4RotationAxis( axis, Math::deg2rad( angle ) );
		v1 = vec3Transform( v1, mat );
		v1.normalize();
	}

	inline void updateLook( void ) { 
		_look = vec3Cross( _up, _right );
		_look.normalize();
	}

	inline void updateUp( void ) {
		_up = vec3Cross( _right, _look );
		_up.normalize();
	}

	inline void updateRight( void ) {
		_right = vec3Cross( _look, _up );
		_right.normalize();
	}

private:	
	Vector3 _right;
	Vector3 _up;
	Vector3 _look;
	Vector3 _eyePos;
};

//class Camera : public CameraBase
//{
//public:
//
//	Camera( const Vector3& pos = Vector3( 0.f, 0.f, 0.f ) );
//
//	virtual ~Camera(void);
//
//	void Strafe( float units ); //left/right
//	void Fly( float units );	//up/down
//	void Walk( float units );	// forward/backwrad
//
//	void SetCamera( const Vector3& look, const Vector3& up );
//
//	inline void SetEyePos( const Vector3& eyePos ) { CameraBase::SetEyePos( eyePos ); }
//
//	inline void SetLook( const Vector3& look ) { 
//		_look = look; 
//		_look.normalize(); 
//		this->updateRight();
//	}
//	inline void SetUp( const Vector3& up ) { 
//		_up = up; 
//		_up.normalize(); 
//		this->updateRight();
//	}	
//
//protected:
//

//};

class OrbitCamera : public CameraBase
{
public:	
	OrbitCamera( const Vector3& eyePos = Vector3::UNIT_Z, 
		const Vector3& target = Vector3::ZERO, 
		const Vector3& up = Vector3::UNIT_Y )
	{
		this->SetEyePos( eyePos );
		this->SetTarget( target );
		this->SetUp( up );
	}

	inline void Zoom( float units ) { 
		CameraBase::SetEyePos( this->GetEyePos() + units * this->GetLook() );
	}

	inline void SetTarget( const Vector3& target ) {
		_target = target;
		this->SetLook( this->GetTarget() - this->GetEyePos() );
	}

	inline void SetEyePos( const Vector3& eyePos ) {
		CameraBase::SetEyePos( eyePos );
		this->SetLook( this->GetTarget() - this->GetEyePos() );
	}

	inline void SetUp( const Vector3& up ) {
		CameraBase::SetUp( up ); 
	}

	inline void Yaw( float angle ) { 
		CameraBase::Yaw( angle );
		this->updateEyePos();
	}

	inline void Pitch( float angle ) {
		CameraBase::Pitch( angle );
		this->updateEyePos();
	}

	inline void Roll( float angle ) {
		CameraBase::Roll( angle );
		this->updateEyePos();
	}

	inline const Vector3& GetTarget( void ) const { return _target; }

protected:
	inline void updateEyePos( void ) { 
		float dist = ( this->GetTarget() - this->GetEyePos() ).norm();
		CameraBase::SetEyePos( this->GetTarget() - this->GetLook() * dist );
	}
	
protected:
	Vector3 _target;	
};

#endif//__CAMERA_H__