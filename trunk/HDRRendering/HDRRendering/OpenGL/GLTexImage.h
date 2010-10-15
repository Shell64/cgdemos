#ifndef __TEX_IMAGE_H__
#define __TEX_IMAGE_H__

class ProgramGLSL;

class GLTexImage
{
public:
	GLTexImage(void);

	virtual ~GLTexImage(void);

	void BindTex();

	void UnbindTex();

	void DrawQuad();

	void DrawQuad( int x1, int y1, int x2, int y2 );

	void DrawImage( );

	void AttachToFBO( int i );

	void DetachFBO( int i );

	void InitTexture( int width, int height, GLuint format = GL_RGBA32F_ARB, bool clamp_to_edge = true );

	void SetTextureParam();

	void FitViewport();

	void MakeGray();

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
	GLuint m_texID;
	int m_imageWidth;
	int m_imageHeight;
	int m_texWidth;
	int m_texHeight;
	int m_drawWidth;
	int m_drawHeight;
	GLuint m_format;
};

class GLTexInput : public GLTexImage
{
public:
	bool SetImageData( int width,  int height, const void * data, 
		GLuint gl_iformat, GLuint gl_format, GLuint gl_type );

	bool LoadImageFromFile( const char* filename, bool color = true );

	unsigned char* CreateGrayImage(
							const unsigned char* img, 
							int width, int height, int format );
};

#endif//__TEX_IMAGE_H__
