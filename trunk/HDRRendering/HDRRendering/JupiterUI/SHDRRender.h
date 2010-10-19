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
	EffectGLSL* m_pDownsampleEffect;
	EffectGLSL* m_pBlurXEffect;
	EffectGLSL* m_pBlurYEffect;
	EffectGLSL* m_pTonemapingEffect;

	GLTexInput* m_pHdrTex;	
	GLTexImage* m_pDSTex;
	GLTexImage* m_pBlurXTex;
	GLTexImage* m_pBlurYTex;
	float* m_GK;	
};

#endif//__SHDR_RENDER_H__
