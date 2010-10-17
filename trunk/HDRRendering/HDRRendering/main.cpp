#include "std.h"
#include "3D\3d.h"
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
	HDRDemoWindow Window( APP_CLASS_NMAE, APP_WINDOW_NAME );
	return Window.Run();		
}
