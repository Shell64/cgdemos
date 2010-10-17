#ifndef __JUPITER_CONTROLS_H__
#define __JUPITER_CONTROLS_H__

#define GL_WIDGET TEXT("GL_Widget")

class GLWidget : public CBaseWindow
{
public:
	GLWidget( HWND hParentWnd );

	virtual ~GLWidget( void );

	virtual bool OnResize( WPARAM wParam, LPARAM lParam );

	virtual bool OnPaint( WPARAM wParam, LPARAM lParam );

	virtual void SetPosition( int x, int y, int width, int height );

	virtual bool Initialize( void );

protected:	
	bool InitGL( void );

	bool SetPixelFormat( void );	

	inline bool IsCurrent( void ) { return _hGLRC == wglGetCurrentContext() && _hGLRC != NULL; }

	bool MakeCurrent( void );

	void SaveMVPMatrices( void );

	void RestoreMVPMatrices( void );

	bool BasicInitialize( void );

protected:
	HGLRC _hGLRC;
	HDC _hDC;
	bool _bInitialized;

	static GLdouble s_ModelView[16];
	static GLdouble s_Projection[16];
	static GLint s_Viewport[4];
};

class SHDRRender : public GLWidget
{
public:
	SHDRRender( HWND hParentWnd );

	~SHDRRender();

	virtual bool Initialize();

protected:
	virtual bool OnResize( WPARAM wParam, LPARAM lParam );

	virtual bool OnPaint( WPARAM wParam, LPARAM lParam );

	bool InitTexture( void );

	bool InitShaders( void );

	bool InitRenderTagets( void );

	float GetExposure( void );

private:
	ProgramGLSL* m_pDownsampleProgram;
	ProgramGLSL* m_pBlurXProgram;
	ProgramGLSL* m_pBlurYProgram;
	ProgramGLSL* m_pTonemapProgram;

	GLTexInput* m_pHdrTex;	
	GLTexImage* m_pDSTex;
	GLTexImage* m_pBlurXTex;
	GLTexImage* m_pBlurYTex;
	float* m_GK;	
};

#else
#error __JUPITER_CONTROLS_H__ already defined!
#endif//__JUPITER_CONTROLS_H__