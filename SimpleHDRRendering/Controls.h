#ifndef __CONTROLS_H__
#define __CONTROLS_H__

#define WC_DISPLAYWIDGET TEXT( "DisplayWidget" )

BOOL InitControls( HINSTANCE hInstance );
HWND CreateToolBar( HWND hWndParent );


class DisplayWidget 
{
public:
	typedef std::map< HWND, DisplayWidget* > DisplayWidgetMap;

	static DisplayWidget* GetWidget( HWND hWnd );

	BOOL SetPosition( const int x, const int y, const int width, const int height );

	inline HWND GetHWND( void ) { return m_hWnd; }

	inline HGLRC GetHGLRC( void ) { return m_hGLRC; }

	inline HDC GetHDC( void ) { return m_hDC; }		

	virtual void Reshape( const int width, const int height );

	virtual void Display( void );	

	virtual BOOL Initialize( void );

	virtual BOOL LButtonDownProc( WPARAM wParam, LPARAM lParam );

	virtual BOOL LButtonUpProc( WPARAM wParam, LPARAM lParam );

	virtual BOOL RButtonDownProc( WPARAM wParam, LPARAM lParam );

	virtual BOOL RButtonUpProc( WPARAM wParam, LPARAM lParam );

	virtual BOOL MouseMoveProc( WPARAM wParam, LPARAM lParam );

	virtual BOOL MouseLeaveProc( WPARAM wParam, LPARAM lParam );	

	virtual BOOL KeyDownProc( WPARAM wParam, LPARAM lParam );

	virtual BOOL KeyUpProc( WPARAM wParam, LPARAM lParam );

	DisplayWidget( HWND hWndParent );

	virtual ~DisplayWidget( void );	

protected:			

	BOOL InitGL( void );

	BOOL SetPixelFormat( void );	

	inline BOOL IsCurrent( void ) { return m_hGLRC == wglGetCurrentContext() && m_hGLRC != NULL; }

	BOOL MakeCurrent( void );

protected:

	HDC m_hDC;
	HGLRC m_hGLRC;
	HWND m_hWnd;

private:	
	static DisplayWidgetMap s_DWM;
};

class SceneRender : public DisplayWidget
{
public:
	void Reshape( const int width, const int height );

	void Display();

	BOOL Initialize( void );	

	SceneRender( HWND hParentWnd );

	~SceneRender();

protected:
	BOOL InitTexture( void );

	BOOL InitShaders( void );

	BOOL InitRenderTagets( void );

private:
	GLuint m_TexID;
	int m_nWidth;
	int m_nHeight;
	ProgramGLSL m_DownsampleProgram;
	ProgramGLSL m_BlurXProgram;
	ProgramGLSL m_BlurYProgram;
	ProgramGLSL m_TonemapProgram;
};

#define RET(b) \
	if ( !b ) return;

#define V_RET(b) \
	if ( !b ) return FALSE;

#endif//__CONTROLS_H__