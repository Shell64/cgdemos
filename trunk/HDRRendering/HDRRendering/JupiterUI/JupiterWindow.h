#ifndef __JUPITER_WINDOW_H__
#define __JUPITER_WINDOW_H__

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

protected:
	virtual BOOL OnLButtonDown( WPARAM wParam, LPARAM lParam );

	virtual BOOL OnLButtonUp( WPARAM wParam, LPARAM lParam );

	virtual BOOL OnRButtonDown( WPARAM wParam, LPARAM lParam );

	virtual BOOL OnRButtonUp( WPARAM wParam, LPARAM lParam );

	virtual BOOL OnMouseMove( WPARAM wParam, LPARAM lParam );

	virtual BOOL OnMouseLeave( WPARAM wParam, LPARAM lParam );	

	virtual BOOL OnKeyDown( WPARAM wParam, LPARAM lParam );

	virtual BOOL OnKeyUp( WPARAM wParam, LPARAM lParam );

	virtual BOOL OnResize( WPARAM wParam, LPARAM lParam );

	virtual BOOL OnPaint( WPARAM wParam, LPARAM lParam );

	virtual BOOL OnCreate( HWND hWnd, WPARAM wParam, LPARAM lParam );

	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

protected:
	typedef std::map< HWND, CBaseWindow* > WindowMap;
	HWND _hWnd;
	HINSTANCE _hInstance;	
	
	static WindowMap s_WindowMap;
	inline static CBaseWindow* FindWindow( HWND hWnd );
	static CBaseWindow* s_CurrentWnd;	

	friend LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
};


class GLWidget;

class MainWindow : public CBaseWindow
{
public:
	MainWindow( LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName );

	virtual ~MainWindow( void ) { }

	virtual int Run( void );

protected:
	virtual BOOL OnResize( WPARAM wParam, LPARAM lParam );


protected:
	static const int s_nWN = 6;
	GLWidget* _glWidgets[s_nWN];
	int _current;
};

#else
#error __JUPITER_WINDOW_H__ already defined!
#endif//__JUPITER_WINDOW_H__
