#include "std.h"

#include "resource.h"

#include "JupiterWindow.h"

#define APP_WINDOW_NAME "Jupiter"
#define APP_CLASS_NMAE "Jupiter_Class"

int WINAPI WinMain( HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nShowCmd )
{		
	MSG msg = { 0 };
	CBaseWindow* pWindow = new MainWindow( APP_CLASS_NMAE, APP_WINDOW_NAME );
	pWindow->Show();

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
