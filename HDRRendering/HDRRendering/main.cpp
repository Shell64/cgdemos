#include "std.h"

#include "OpenGL/OpenGL.h"
#include "OpenGL/GLTexCube.h"
#include "JupiterUI/JupiterWindow.h"

#define APP_WINDOW_NAME "Jupiter"
#define APP_CLASS_NMAE "Jupiter_Class"

int WINAPI WinMain( HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nShowCmd )
{		
	CBaseWindow* pWindow = new MainWindow( APP_CLASS_NMAE, APP_WINDOW_NAME );	
	GLTexCubeInput cubeInput( "beach_cross.hdr" );
	return dynamic_cast<MainWindow*>( pWindow )->Run();
}
