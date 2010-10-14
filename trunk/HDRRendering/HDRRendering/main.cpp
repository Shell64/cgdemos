#include "std.h"

#include "JupiterUI/JupiterWindow.h"

#define APP_WINDOW_NAME "Jupiter"
#define APP_CLASS_NMAE "Jupiter_Class"

int WINAPI WinMain( HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nShowCmd )
{		
	CBaseWindow* pWindow = new MainWindow( APP_CLASS_NMAE, APP_WINDOW_NAME );	
	return dynamic_cast<MainWindow*>( pWindow )->Run();
}
