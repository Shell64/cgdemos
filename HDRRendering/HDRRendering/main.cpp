#include "std.h"
#include "3D\3d.h"
#include "OpenGL/OpenGL.h"

#include "JupiterUI/JupiterUI.h"
#include "HDRDemo.h"
#include "SHDRDemo.h"

#define APP_WINDOW_NAME "Jupiter"
#define APP_CLASS_NMAE "Jupiter_Class"

int WINAPI WinMain( HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nShowCmd )
{		
	HDRDemo Window( APP_CLASS_NMAE, APP_WINDOW_NAME );
	return Window.Run();
}
