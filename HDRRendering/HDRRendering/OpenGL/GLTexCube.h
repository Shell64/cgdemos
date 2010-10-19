#ifndef __GL_TEX_CUBE_H__
#define __GL_TEX_CUBE_H__

#define X_I 0
#define Y_I 1
#define Z_I 2

class GLTexCube
{
public:	
	GLTexCube( void );

	virtual ~GLTexCube(void);

	void Bind( void );

	void Unbind( void );

	bool Initialize( int width, int height, GLint iformat = GL_RGBA32F );

	bool Initialize( int width[6], int height[6], GLint iformat = GL_RGBA32F );

protected:

	void SetTextureParams( void );

	bool BeginInitialize( void );

	bool EndInitialize( void );

protected:
	GLuint _texID;
	bool _bIsValid;
};

class GLTexCubeInput : public GLTexCube
{
public:
	GLTexCubeInput( void );

	~GLTexCubeInput( void ) { }

	bool Load( const char* filename, GLint iformat = GL_RGBA32F );

protected:
	bool ExtractCubeMap( const char* filename, ILuint* imgIDs );

};

#endif//__GL_TEX_CUBE_H__
