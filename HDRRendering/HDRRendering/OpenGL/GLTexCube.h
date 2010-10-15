#ifndef __GL_TEX_CUBE_H__
#define __GL_TEX_CUBE_H__

#define X_I 0
#define Y_I 1
#define Z_I 2

class GLTexCube
{
public:
	GLTexCube( int width[6], int height[6], GLint iformat = GL_RGBA32F );

	GLTexCube( int width, int height, GLint iformat = GL_RGBA32F );
	
	virtual ~GLTexCube(void);

	void BindTex( void );

	void UnbindTex( void );

protected:
	GLTexCube(void);

	void SetTextureParams( void );

	void BeginInitTexture( void );

	void EndInitTexture( void );

protected:
	GLuint _texID;
	bool _bIsValid;
};

class GLTexCubeInput : public GLTexCube
{
public:
	GLTexCubeInput( const char* filename, GLint iformat = GL_RGBA32F );

	~GLTexCubeInput( void ) { }

protected:

	bool ExtractCubeMap( const char* filename, ILuint* imgIDs );

	ILenum GetDataType( ILint format, ILint bpp );
};

#else
#error __GL_TEX_CUBE_H__ already defined!
#endif//__GL_TEX_CUBE_H__
