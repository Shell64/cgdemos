#include "std.h"
#include "3D/3d.h"
#include "OpenGL/OpenGL.h"
#include "JupiterUI/JupiterUI.h"
#include "SHDRDemo.h"

/************************************************************************
 * class MainWindow
 ************************************************************************/
SHDRDemo::SHDRDemo( LPCTSTR lpszClassName, LPCTSTR lpszWindowName )
:CBaseWindow( lpszClassName, lpszWindowName ), _current( 0 )
{
	for ( int n = 0; n < s_nWN; ++n )
		_glWidgets[ n ] = NULL;

	int r = 0, c = 0, width = 512;
	for ( int n = 0; n < s_nWN; ++n )
	{
		RET( _glWidgets[n] = new SHDRRender( _hWnd ) );
		RET( _glWidgets[n]->Initialize() );
		_glWidgets[n]->SetPosition( c, r, width, width );
		if ( (n+1) % 3 )
		{
			c += width + 2;
		}
		else
		{
			r += width + 2;
			c = 0;
		}
	}	
}

SHDRDemo::~SHDRDemo( void )
{
	for ( int n = 0; n < s_nWN; ++n )
	{
		SAFE_RELEASE( _glWidgets[ n ] );
	}
}

bool SHDRDemo::OnResize( WPARAM wParam, LPARAM lParam )
{
	return true;
}

int SHDRDemo::Run( void )
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
			if ( NULL != _glWidgets[ _current ] )
			{
				_glWidgets[_current]->OnPaint( NULL, NULL );
				_current = ( _current + 1 ) % s_nWN;
			}
		}
	}
	return 0;
}
