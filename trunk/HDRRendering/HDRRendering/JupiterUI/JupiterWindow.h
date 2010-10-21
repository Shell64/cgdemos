#ifndef __JUPITER_WINDOW_H__
#define __JUPITER_WINDOW_H__

#include "../Win32.h"

class CBaseWindow
{
public:

	CBaseWindow( LPCSTR lpszClassName, 
		LPCSTR lpszWindowName = NULL,
		DWORD dwStyle = WS_OVERLAPPEDWINDOW,
		HWND hParrentWnd = NULL,	
		HBRUSH hBackground = (HBRUSH)(COLOR_WINDOW),
		HICON hIcon = NULL,
		HCURSOR hCursor = LoadCursor( NULL, IDC_ARROW ),
		LPCSTR lpszMenuName = NULL );

	virtual ~CBaseWindow(void);

	virtual void Show( int nCmdShow );

	virtual void Show( void );

	virtual void Hide( void );

//protected:
	virtual bool OnLButtonDown( WPARAM wParam, LPARAM lParam );

	virtual bool OnLButtonUp( WPARAM wParam, LPARAM lParam );

	virtual bool OnRButtonDown( WPARAM wParam, LPARAM lParam );

	virtual bool OnRButtonUp( WPARAM wParam, LPARAM lParam );

	virtual bool OnMouseMove( WPARAM wParam, LPARAM lParam );

	virtual bool OnMouseLeave( WPARAM wParam, LPARAM lParam );	

	virtual bool OnKeyDown( WPARAM wParam, LPARAM lParam );

	virtual bool OnKeyUp( WPARAM wParam, LPARAM lParam );

	virtual bool OnResize( WPARAM wParam, LPARAM lParam );

	virtual bool OnPaint( WPARAM wParam, LPARAM lParam );

	virtual bool OnCreate( HWND hWnd, WPARAM wParam, LPARAM lParam );

	virtual bool OnCommand( WPARAM wParam, LPARAM lParam );

	virtual bool OnScroll( WPARAM wParam, LPARAM lParam );

private:
	CBaseWindow( const CBaseWindow& window ) { }

protected:
	typedef std::map< HWND, CBaseWindow* > WindowMap;
	HWND _hWnd;
	HINSTANCE _hInstance;	
	
	static WindowMap s_WindowMap;
	inline static CBaseWindow* FindWindow( HWND hWnd );
	static CBaseWindow* s_CurrentWnd;	

	friend LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
};




#else
#error __JUPITER_WINDOW_H__ already defined!
#endif//__JUPITER_WINDOW_H__
