#ifndef __TEX_IMAGE_H__
#define __TEX_IMAGE_H__

class GLTexImage
{
public:
	GLTexImage(void);

	virtual ~GLTexImage(void);

	void Bind();

	void Unbind();

	void DrawQuad();

	void DrawQuad( int x1, int y1, int x2, int y2 );

	void DrawImage( );

	void AttachToFBO( int i );

	void DetachFBO( int i );

	bool Initialize( int width, int height, GLuint iformat = GL_RGBA16F_ARB,
		GLuint format = GL_RGBA, GLuint type = GL_FLOAT, const void* data = NULL );

	void SetTextureParam();

	void FitViewport();

	void Save( const char* filename );

public:
	inline GLuint GetTexID() { return m_texID; }

	inline int GetImageWidth() { return m_imageWidth; }

	inline int GetImageHeight() { return m_imageHeight; }

	inline int GetTexWidth() { return m_texWidth; }

	inline int GetTexHeight() { return m_texHeight; }

	inline int GetDrawWidth() { return m_drawWidth; }

	inline int GetDrawHeight() { return m_drawHeight; }

	inline GLuint GetFormat() { return m_format; }

protected:
	bool BeginInitialize( void );

	bool EndInitialize( void );

protected:
	GLuint m_texID;
	int m_imageWidth;
	int m_imageHeight;
	int m_texWidth;
	int m_texHeight;
	int m_drawWidth;
	int m_drawHeight;
	GLuint m_format;
	bool _bIsValid;
};

class GLTexInput : public GLTexImage
{
public:
	bool Load( const char* filename, GLuint iformat = GL_RGBA, bool color = true );

protected:
	bool LoadFile( const char* filename, GLuint& imID );

	unsigned char* CreateGrayImage(
							const unsigned char* img, 
							int width, int height, int format );
};

class GLTexFBO : public GLTexImage
{
public:
	GLTexFBO( void );

	~GLTexFBO( void );

	bool Initialize( int width, int height, GLuint iformat = GL_RGBA16F_ARB );

	void BeginCapture();

	void EndCapture();

protected:
	GLuint _depthRB;
	FramebufferObject _fbo;
};

class GLTexAttachment : public GLTexImage
{
public:
	GLTexAttachment( int i = 0 );

	~GLTexAttachment( void );

	bool Initialize( int width, int height, GLuint iformat = GL_RGBA16F_ARB );

	void BeginCapture( void );

	void EndCapture( void );

protected:
	static bool InitFbo( int width = 0, int height = 0 );

	static bool ReleaseeFbo( void );

protected:
	int _attachID;

	static FramebufferObject* s_pFBO;
	static GLuint s_depthRB;
	static int s_count;
};

#endif//__TEX_IMAGE_H__
