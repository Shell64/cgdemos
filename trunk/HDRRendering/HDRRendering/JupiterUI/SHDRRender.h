#ifndef __SHDR_RENDER_H__
#define __SHDR_RENDER_H__

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
	EffectGLSL _downSampleEffect;
	EffectGLSL _blurXEffect;
	EffectGLSL _blurYEffect;
	EffectGLSL _toneEffect;

	GLTexInput _hdrTex;	
	GLTexFBO _rtDownsample;
	GLTexFBO _rtBlurX;
	GLTexFBO _rtBlurY;
	float* m_GK;	
};

#endif//__SHDR_RENDER_H__
