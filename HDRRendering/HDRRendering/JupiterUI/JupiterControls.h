#ifndef __JUPITER_CONTROLS_H__
#define __JUPITER_CONTROLS_H__

#include <CommCtrl.h>

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

	void SetRenderTarget( const GLRenderTarget& rt );

	void ResetRenderTarget( void );

	static void ClearBuffer( GLbitfield color = GL_COLOR_BUFFER_BIT, 
		GLbitfield depth = GL_DEPTH_BUFFER_BIT,
		GLbitfield stencil = 0 );

protected:
	HGLRC _hGLRC;
	HDC _hDC;
	bool _bInitialized;

	static GLdouble s_ModelView[16];
	static GLdouble s_Projection[16];
	static GLint s_Viewport[4];
};


class Slider
{
public:
	Slider( HWND hParentWnd, float min, float max,
		int x, int y, int cx, int cy );

	~Slider( void ) { if ( NULL != _hWnd ) ::DestroyWindow( _hWnd ); }

	bool OnScroll( WPARAM wParam, LPARAM lParam );

	inline float GetValue( void ) const { return _fPercent * ( _fMax - _fMin ); }

	inline void SetValue( float v ) {
		if ( v < _fMin || v > _fMax ) return;
		_fPercent = ( v - _fMin ) / ( _fMax - _fMin );				
		int nPos = int( _nLength * _fPercent );		
		::SendMessage( _hWnd, TBM_SETPOS, TRUE, nPos);
		/*int pos = ::SetScrollInfo( _hWnd, SB_CTL, &info, TRUE );*/		
		//::SetScrollPos( _hWnd, SB_CTL, int( _nLength * _fPercent), FALSE );
	}

protected:
	HWND _hWnd;
	float _fMin;
	float _fMax;
	float _fPercent;
	int _nLength;
};

class Label
{
public:
	Label( HWND hParent, int x, int y, int cx, int cy, const char* text );

	~Label( void ) { if ( NULL != _hWnd ) ::DestroyWindow( _hWnd ); }

	void SetText( const char* text );

	const char* GetTex( void );

protected:
	HWND _hWnd;
};

#else
#error __JUPITER_CONTROLS_H__ already defined!
#endif//__JUPITER_CONTROLS_H__