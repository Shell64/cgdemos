#include "../std.h"
#include "../3D/3d.h"
#include "OpenGL.h"

#include "GlobalUtil.h"

GlobalUtil::GlobalUtil(void)
{
}

GlobalUtil::~GlobalUtil(void)
{
}

void GlobalUtil::SaveFrame( const char* filename )
{
	int vp[4];
	glGetIntegerv( GL_VIEWPORT, vp );
	int width = vp[2];
	int height = vp[3];
	unsigned char* data = new unsigned char[ width * height * 4 ];
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glReadPixels( 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data );
	int imID = ilGenImage();
	ilBindImage( imID );
	ilEnable( IL_FILE_OVERWRITE );
	ilTexImage( width, height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, data );	
	ilSaveImage( filename );
	ilDeleteImage( imID );
	delete[] data;
}

GLuint GlobalParam::s_texTarget = GL_TEXTURE_2D;
