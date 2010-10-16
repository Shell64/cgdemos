#ifndef __CAMERA_H__
#define __CAMERA_H__

class Vector3;
class Matrix4;

class Camera
{
public:
	Camera( const Vector3& pos = Vector3( 0.f, 0.f, 0.f ) );

	virtual ~Camera(void);

	void Strafe( float units ); //left/right
	void Fly( float units );	//up/down
	void Walk( float units );	// forward/backwrad

	void Pitch( float angle );	//rotate on right vector
	void Yaw( float angle );	//rotate on up vector
	void Roll( float angle );	//rotate on look vector	

	inline const Vector3& GetRight( void ) { return _right; }
	inline const Vector3& Up( void ) { return _up; }
	inline const Vector3& Look( void ) { return _look; }
	inline const Vector3& Pos( void ) { return _pos; }

protected:
	inline void Rotate(const Vector3& axis, Vector3& v1, Vector3& v2, float angle )
	{
		Matrix4 mat = mat4RotationAxis( axis, Math::deg2rad( angle ) );
		v1 = vec3Transform( v1, mat );
		v2 = vec3Cross( axis, v1 );
	}

protected:	
	Vector3 _right;
	Vector3 _up;
	Vector3 _look;
	Vector3 _pos;
};

#endif//__CAMERA_H__