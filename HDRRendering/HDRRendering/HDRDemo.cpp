#include "std.h"
#include "3D/3d.h"
#include "OpenGL/OpenGL.h"
#include "JupiterUI/JupiterUI.h"

#include "HDRDemo.h"

HDRDemo::HDRDemo( LPCSTR lpszClassName, LPCTSTR lpszWindowName )
:CBaseWindow( lpszClassName, lpszWindowName ), _pWidget( NULL )
{
	RET( _pWidget = new HDRRender( _hWnd ) );
	RET( _pWidget->Initialize() );
	_pWidget->SetPosition( 0, 0, 1024, 768 );
}

HDRDemo::~HDRDemo()
{
	SAFE_RELEASE( _pWidget );
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
