#ifndef __HDR_DEMO_H__
#define __HDR_DEMO_H__

class GLWidget;
class CBaseWindow;

class HDRDemo : public CBaseWindow
{
public:
	HDRDemo( LPCSTR lpszClassName,
		LPCTSTR lpszWindowName = "HDR Rendering Demo" );

	virtual ~HDRDemo( void );

	virtual int Run( void );

protected:
	virtual bool OnKeyDown( WPARAM wParam, LPARAM lParam );

	virtual bool OnScroll( WPARAM wParam, LPARAM lParam );

protected:
	HDRRender* _pWidget;
	Slider* _pExposureSlider;
	Slider* _pRFSlider;
	/*Slider* _pFBSlider;
	Slider* _pFPSlider;
	Slider* _pFSSlider;*/
	Slider* _pBTSlider;
	Slider* _pBFSlider;
	Slider* _pMCRSlider;
	Slider* _pMCGSlider;
	Slider* _pMCBSlider;

	Label* _pExposureLabel;
	Label* _pRFLabel;
	Label* _pBTLabel;
	Label* _pBFLabel;
	Label* _pMCRLabel;
	Label* _pMCGLabel;
	Label* _pMCBLabel;
};

#endif//__HDR_DEMO_H__
