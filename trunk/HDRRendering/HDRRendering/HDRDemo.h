#ifndef __HDR_DEMO_H__
#define __HDR_DEMO_H__

class GLWidget;
class CBaseWindow;

class HDRDemo : public CBaseWindow
{
public:
	HDRDemo( LPCSTR lpszClassName,
		LPCTSTR lpszWindowName );

	virtual ~HDRDemo( void );

	virtual int Run( void );

protected:
	virtual bool OnKeyDown( WPARAM wParam, LPARAM lParam );

protected:
	GLWidget* _pWidget;
};

#endif//__HDR_DEMO_H__
