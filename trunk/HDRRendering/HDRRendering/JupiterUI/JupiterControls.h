#ifndef __JUPITER_CONTROLS_H__
#define __JUPITER_CONTROLS_H__

#define GL_WIDGET TEXT("GL_Widget")

class GLWidget : public CBaseWindow
{
public:
	GLWidget( HWND hParentWnd );

	virtual ~GLWidget( void );

	virtual BOOL OnResize( WPARAM wParam, LPARAM lParam );

	virtual BOOL OnPaint( WPARAM wParam, LPARAM lParam );

	virtual void SetPosition( int x, int y, int width, int height );

	virtual BOOL Initialize( void );

protected:	
	BOOL InitGL( void );

	BOOL SetPixelFormat( void );	

	inline BOOL IsCurrent( void ) { return _hGLRC == wglGetCurrentContext() && _hGLRC != NULL; }

	BOOL MakeCurrent( void );

protected:
	HGLRC _hGLRC;
	HDC _hDC;
};

class SceneRender : public GLWidget
{
public:
	SceneRender( HWND hParentWnd );

	~SceneRender();

	virtual BOOL Initialize();

protected:
	virtual BOOL OnResize( WPARAM wParam, LPARAM lParam );

	virtual BOOL OnPaint( WPARAM wParam, LPARAM lParam );

	BOOL InitTexture( void );

	BOOL InitShaders( void );

	BOOL InitRenderTagets( void );

	void UpdateViewport();

private:
	GLuint m_TexID;
	int m_nWidth;
	int m_nHeight;
	ProgramGLSL* m_pDownsampleProgram;
	ProgramGLSL* m_pBlurXProgram;
	ProgramGLSL* m_pBlurYProgram;
	ProgramGLSL* m_pTonemapProgram;

	GLTexInput* m_pHdrTex;
	GLTexImage* m_pFullTex;
	GLTexImage* m_pDSTex;
	GLTexImage* m_pBlurXTex;
	GLTexImage* m_pBlurYTex;
	float* m_GK;
};

#else
#error __JUPITER_CONTROLS_H__ already defined!
#endif//__JUPITER_CONTROLS_H__