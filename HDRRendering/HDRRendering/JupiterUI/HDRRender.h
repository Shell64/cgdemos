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

	virtual bool OnLButtonDown( WPARAM wParam, LPARAM lParam );

	virtual bool OnMouseMove( WPARAM wParam, LPARAM lParam );

	virtual bool OnLButtonUp( WPARAM wParam, LPARAM lParam );

	void RenderSkybox( void );

	void RenderMesh( void );

	void RenderTrimesh( void );
	
	void RenderGLMMesh( void );

	void RenderHdr( void );

	void RenderRgb( void );

protected:
	OrbitCamera _camera;
	GLTexCubeInput* _pTexCubeInput;
	EffectGLSL* _pEffect;
	GLTexInput* _pTexture;

	GLTexAttachment rtRgb;
	GLTexAttachment rtHdr;
	GLTexAttachment rtBloom;
	GLTexAttachment rtBlurX;
	GLTexAttachment rtBlurY;

	EffectGLSL bloomEffect;
	EffectGLSL blurXEffect;
	EffectGLSL blurYEffect;
	EffectGLSL toneEffect;

	float* _GK;

	Vector3 _etaRatio;
	Color _matColor;

	GLMmodel* _pModel;
	TriMesh *_pMesh;

	bool _bTrimesh;

	Vector3 _lastPos;
	bool _bMove;
};

#endif//__HDR_RENDER_H__
