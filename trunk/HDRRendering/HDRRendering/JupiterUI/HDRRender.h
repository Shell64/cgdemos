#ifndef __HDR_RENDER_H__
#define __HDR_RENDER_H__

class HDRRender : public GLWidget
{
public:
	HDRRender( HWND hParentWnd );
	
	virtual ~HDRRender(void);

	virtual bool Initialize( void );

protected:
	virtual bool OnPaint( WPARAM wParam, LPARAM lParam );

	virtual bool OnKeyDown( WPARAM wParam, LPARAM lParam );

	virtual bool OnResize( WPARAM wParam, LPARAM lParam );

protected:
	Camera _camera;
	GLTexCubeInput* _pTexCubeInput;
};

#endif//__HDR_RENDER_H__
