#include "std.h"
#include "3D/3d.h"
#include "OpenGL/OpenGL.h"
#include "JupiterUI/JupiterUI.h"

#include "HDRDemo.h"

HDRDemo::HDRDemo( LPCSTR lpszClassName, LPCTSTR lpszWindowName )
:CBaseWindow( lpszClassName, lpszWindowName )
{
	RET( _pWidget = new HDRRender( _hWnd ) );
	RET( _pWidget->Initialize() );
	_pWidget->SetPosition( 0, 0, 1024, 768 );	
	int startxl = 1030;
	int wl = 120;
	int startx = startxl + wl;
	int starty = 10;
	int w = 200;	
	int h = 18;
	int n = 30;
	
	_pExposureLabel = new Label( _hWnd, startxl, starty, wl, h, "Exposure:" );
	_pExposureSlider = new Slider( _hWnd, 0.f, 10.f, startx, starty, w, h );
	_pExposureSlider->SetValue( _pWidget->GetExposure() );

	starty += n;
	_pRFLabel = new Label( _hWnd, startxl, starty, wl, h, "Reflection Factor:" );
	_pRFSlider = new Slider( _hWnd, 0.f, 2.f, startx, starty, w, h );
	_pRFSlider->SetValue( _pWidget->GetReflectionFactor() );

	starty += n;
	_pBTLabel = new Label( _hWnd, startxl, starty, wl, h, "Bright Threshold:" );
	_pBTSlider = new Slider( _hWnd, 0.f, 10.f, startx, starty, w, h );
	_pBTSlider->SetValue( _pWidget->GetBrightThreshold() );
	
	starty += n;
	_pBFLabel = new Label( _hWnd, startxl, starty, wl, h, "Bloom Factor:" );
	_pBFSlider = new Slider( _hWnd, 0.f, 10.f, startx, starty, w, h );
	_pBFSlider->SetValue( _pWidget->GetBloomFactor() );
	
	starty += n;
	_pMCRLabel = new Label( _hWnd, startxl, starty, wl, h, "Red component:" );
	_pMCRSlider = new Slider( _hWnd, 0.f, 1.f, startx, starty, w, h );
	_pMCRSlider->SetValue( _pWidget->GetMatColor().r );

	starty += n;
	_pMCGLabel = new Label( _hWnd, startxl, starty, wl, h, "Green component:" );
	_pMCGSlider = new Slider( _hWnd, 0.f, 1.f, startx, starty, w, h );
	_pMCGSlider->SetValue( _pWidget->GetMatColor().g );

	starty += n;
	_pMCBLabel = new Label( _hWnd, startxl, starty, wl, h, "Blue component:" );
	_pMCBSlider = new Slider( _hWnd, 0.f, 1.f, startx, starty, w, h );
	_pMCBSlider->SetValue( _pWidget->GetMatColor().b );

	starty += n;
	_pModelLabel = new Label( _hWnd, startxl, starty, wl, h, "Model:" );
	const char* modelFiles[] = { 
		"teapot.ms3d",
		"car1.ms3d",
		"car2.ms3d",
		"eye.ms3d",
		"glass.ms3d",
		"ming.ms3d",
		"pottery.ms3d",
		"spoon.ms3d"  };

	const char* modelNames[] = { 
		"teapot",
		"car1",
		"car2",
		"eye",
		"glass",
		"ming",
		"pottery",
		"spoon"  };
	assert( sizeof ( modelNames ) == sizeof ( modelFiles ) );
	_pModelCombo = new ComboBox( _hWnd, startx, starty, w, h );
	_pModelCombo->AddItems( modelNames, modelFiles, sizeof( modelNames ) / sizeof(char*) );
	_pModelCombo->SetSelectedIndex( 0 );

	starty += n;
	_pEffectLabel = new Label( _hWnd, startxl, starty, wl, h, "Effect" );
	const char* effects[] = {
		"reflect",
		"reflectTex",
		"refract",
		"fresnel",
		"chromaticDispersion"		
	};
	_pEffectCombo = new ComboBox( _hWnd, startx, starty, w, h );
	_pEffectCombo->AddItems( effects, sizeof( effects ) / sizeof( char* ) );
	_pEffectCombo->SetSelectedIndex( 0 );
}

HDRDemo::~HDRDemo()
{
	SAFE_RELEASE( _pWidget );

	SAFE_RELEASE( _pExposureSlider );
	SAFE_RELEASE( _pRFSlider );
	SAFE_RELEASE( _pBTSlider );
	SAFE_RELEASE( _pBFSlider );
	SAFE_RELEASE( _pMCRSlider );
	SAFE_RELEASE( _pMCGSlider );
	SAFE_RELEASE( _pMCBSlider );

	SAFE_RELEASE( _pExposureLabel );
	SAFE_RELEASE( _pRFLabel );
	SAFE_RELEASE( _pBTLabel );
	SAFE_RELEASE( _pBFLabel );
	SAFE_RELEASE( _pMCRLabel );
	SAFE_RELEASE( _pMCGLabel );
	SAFE_RELEASE( _pMCBLabel );

	SAFE_RELEASE( _pModelCombo );
	SAFE_RELEASE( _pModelLabel );
	SAFE_RELEASE( _pEffectCombo );
	SAFE_RELEASE( _pEffectLabel );
}

int HDRDemo::Run()
{
	this->Show();
	
	MSG msg = { 0 };
	while( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else 
		{			
		}
	}
	return 0;
}


bool HDRDemo::OnKeyDown( WPARAM wParam, LPARAM lParam )
{
	_pWidget->OnKeyDown( wParam, lParam );
	return false;
}

bool HDRDemo::OnScroll( WPARAM wParam, LPARAM lParam )
{
	bool ret = false;
	if ( ret = _pExposureSlider->OnScroll( wParam, lParam ) ) {
		_pWidget->SetExposure( _pExposureSlider->GetValue() );
	}
	else if ( ret = _pRFSlider->OnScroll( wParam, lParam ) ) {
		_pWidget->SetReflectionFactor( _pRFSlider->GetValue() );
	}	
	else if ( ret = _pBTSlider->OnScroll( wParam, lParam ) ) {
		_pWidget->SetBrightThreshold( _pBTSlider->GetValue() );
	}	
	else if ( ret = _pBFSlider->OnScroll( wParam, lParam ) ) {
		_pWidget->SetBloomFactor( _pBFSlider->GetValue() );
	}
	else if ( ret = ( _pMCBSlider->OnScroll( wParam, lParam ) ||
		_pMCGSlider->OnScroll( wParam, lParam ) ||
		_pMCRSlider->OnScroll( wParam, lParam ) ) ) {
		_pWidget->SetMatColor(_pMCRSlider->GetValue(),
		_pMCGSlider->GetValue(), _pMCBSlider->GetValue() );
	}
	
	if ( ret )
		::SetFocus( _hWnd );

	return false;
}

bool HDRDemo::OnCommand( WPARAM wParam, LPARAM lParam )
{
	bool ret = false;
	if ( ret = _pModelCombo->OnCommand( wParam, lParam ) ) {		
		_pWidget->ChangeModel( _pModelCombo->GetSelectedValue() );	
	}
	else if ( ret = _pEffectCombo->OnCommand( wParam, lParam ) ) {
		_pWidget->ChangeEffect( _pEffectCombo->GetSelectedValue() );
		ret = true;
	}	

	if ( ret ) SetFocus( _hWnd );

	return true;
}