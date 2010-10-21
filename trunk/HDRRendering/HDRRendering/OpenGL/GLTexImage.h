#ifndef __TEX_IMAGE_H__
#define __TEX_IMAGE_H__

class GLTexImage
{
public:
	GLTexImage(void);

	virtual ~GLTexImage(void);

	void Bind() const;

	void Unbind() const;

	void DrawQuad();

	void DrawQuadLeft();

	void DrawQuadRight();

	void DrawQuad( int x1, int y1, int x2, int y2 );

	bool Initialize( int width, int height, GLuint iformat = GL_RGBA16F_ARB,
		GLuint format = GL_RGBA, GLuint type = GL_FLOAT, const void* data = NULL );

	void SetTextureParam();

	void FitViewport();

	void Save( const char* filename );

public:
	inline GLuint GetTexID() { return _texID; }

	inline int GetTexWidth() { return _texWidth; }

	inline int GetTexHeight() { return _texHeight; }

	inline GLuint GetFormat() { return _iformat; }

protected:
	bool BeginInitialize( void );

	bool EndInitialize( int width, int height, GLuint iformat );

	void Release( void );

protected:
	GLuint _texID;
	int _texWidth;
	int _texHeight;
	GLuint _iformat;
	bool _bIsValid;
};


/************************************************************************
 * function: load image from file as a texture image
 ************************************************************************/
class GLTexInput : public GLTexImage
{
public:
	/************************************************************************
	 *@param filename: the name of the file
	 *@param iformat: the format of the GLTexture2D
	 *@return value: false--load file fail or initialize texture fail, true--otherwise
	 *@description: latest load operation will descard prvious data
	 ************************************************************************/	
	bool Load( const char* filename, GLuint iformat = GL_RGBA );

protected:
	bool LoadFile( const char* filename, GLuint& imID );

	unsigned char* CreateGrayImage(
							const unsigned char* img, 
							int width, int height, int format );
};

class GLRenderTarget : public GLTexImage
{
public:
	virtual ~GLRenderTarget( void ) = NULL;

	virtual bool Initialize( int width, int height, GLuint iformat ) = NULL;	

	/*static void EndCapture();*/	

protected:
	virtual void Capture() const = NULL;

	bool Finalize( void );

	static void InitDepthRenderBuffer( int width, int height, GLuint& depthRB );

	void AttachToFBO( int i ) const;

	void DetachFBO( int i ) const;

	friend class GLWidget;
};

class GLTexFBO : public GLRenderTarget
{
public:
	GLTexFBO( void );

	~GLTexFBO( void );

	bool Initialize( int width, int height, GLuint iformat = GL_RGBA16F_ARB );

protected:
	void Capture() const;

protected:
	GLuint _depthRB;
	FramebufferObject _fbo;
};

class GLTexAttachment : public GLRenderTarget
{
public:
	GLTexAttachment( void );

	~GLTexAttachment( void );

	bool Initialize( int width, int height, GLuint iformat = GL_RGBA16F_ARB );

protected:
	void Capture( void ) const;	

	static bool InitFbo( int width = 0, int height = 0 );

	static bool ReleaseeFbo( void );

protected:

	static FramebufferObject* s_pFBO;
	static GLuint s_depthRB;
	static int s_count;
};

#endif//__TEX_IMAGE_H__
