#ifndef __SHDR_DEMO_H__
#define __SHDR_DEMO_H__

class CBaseWindow;
class GLWidget;

class SHDRDemo : public CBaseWindow
{
public:
	SHDRDemo( LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName = "Simple HDR Rendering Demo" );

	virtual ~SHDRDemo( void );

	virtual int Run( void );

protected:
	virtual bool OnResize( WPARAM wParam, LPARAM lParam );


protected:
	static const int s_nWN = 6;
	GLWidget* _glWidgets[s_nWN];
	int _current;
};

#endif//__SHDR_DEMO_H__
