#ifndef __HDR_RENDER_H__
#define __HDR_RENDER_H__

class HDRRender : public GLWidget
{
public:
	HDRRender( HWND hParentWnd );
	
	virtual ~HDRRender(void);

	virtual bool Initialize( void );

	inline float GetExposure( void ) const { return _exposure; }
	inline void SetExposure( float exposure ) {
		_exposure = exposure;
		this->OnPaint( );
	}

	inline float GetBrightThreshold( void ) const { return _brightThreshold; }
	inline void SetBrightThreshold( float brightThreshold ) {
		_brightThreshold = brightThreshold;
		this->OnPaint( );
	}
	
	inline float GetBloomFactor( void ) const { return _bloomFactor; }
	inline void SetBloomFactor( float bloomFactor ) {
		_bloomFactor = bloomFactor;
		this->OnPaint();
	}

	inline float GetFresnelBias( void ) const { return _fresnelBias; }
	inline void SetFresnelBias( float fresnelBias ) {
		_fresnelBias = fresnelBias;
		this->OnPaint();
	}

	inline float GetFresnelPower( void ) const { return _fresnelPower; }
	inline void SetFresnelPower( float fresnelPower ) {
		_fresnelPower = fresnelPower;
		this->OnPaint();
	}

	inline float GetFresnelScale( void ) const { return _fresnelScale; }
	inline void SetFresnelScale( float fresnelScale ) {
		_fresnelScale = fresnelScale;
		this->OnPaint();
	}	

	inline float GetReflectionFactor( void ) const { return _reflectionFactor; }
	inline void SetReflectionFactor( float reflectionFactor ) {
		_reflectionFactor = reflectionFactor;
		this->OnPaint();
	}

	inline const Vector3& GetEtaRatio( void ) const { return _etaRatio; }
	inline void SetEtaRatio( float x, float y, float z ) {
		this->SetEtaRatio( Vector3( x, y, z) );
	}
	inline void SetEtaRatio( const Vector3& v ) {
		_etaRatio = v;
		this->OnPaint();
	}

	inline const Color& GetMatColor( void ) const { return _matColor; }
	inline void SetMatColor( float r, float g, float b ) { 
		this->SetMatColor( Color( r, g, b ) );
	}
	inline void SetMatColor( const Color& c ) {
		_matColor = c;
		this->OnPaint();
	}

public:
	virtual bool OnPaint( WPARAM wParam = NULL, LPARAM lParam = NULL );

	virtual bool OnKeyDown( WPARAM wParam, LPARAM lParam );

	virtual bool OnResize( WPARAM wParam, LPARAM lParam );

	virtual bool OnLButtonDown( WPARAM wParam, LPARAM lParam );

	virtual bool OnMouseMove( WPARAM wParam, LPARAM lParam );

	virtual bool OnLButtonUp( WPARAM wParam, LPARAM lParam );

	void RenderSkybox( const GLTexCube& cubeTex );

	void RenderMesh( const GLTexCube& cubeTex, const GLTexImage& texImage );

	void RenderTrimesh( void );
	
	void RenderGLMMesh( void );

	void RenderHdr( void );

	void RenderRgb( void );

protected:
	OrbitCamera _camera;
	GLTexCubeInput _texCubeHdrInput;
	EffectGLSL _reflectEffect;
	GLTexInput _texHdrInput;
	GLTexInput _texRgbInput;
	GLTexCubeInput _texCubeRgbInput;

	GLTexAttachment _rtRgb;
	GLTexAttachment _rtHdr;
	GLTexAttachment _rtBloom;
	
	static const int s_blurPasses = 2;	
	GLTexAttachment _rtBlurX[s_blurPasses];
	GLTexAttachment _rtBlurY[s_blurPasses];

	EffectGLSL _bloomEffect;
	EffectGLSL _blurXEffect;
	EffectGLSL _blurYEffect;
	EffectGLSL _toneEffect;

	float* _GK;
	static const int s_kRSize = 3;

	Vector3 _etaRatio;
	Color _matColor;
	float _brightThreshold;
	float _exposure;
	float _bloomFactor;
	float _reflectionFactor;
	float _fresnelBias;
	float _fresnelScale;
	float _fresnelPower;

	GLMmodel* _pModel;
	TriMesh *_pMesh;

	bool _bTrimesh;

	Vector3 _lastPos;
	bool _bMove;
};

#endif//__HDR_RENDER_H__
