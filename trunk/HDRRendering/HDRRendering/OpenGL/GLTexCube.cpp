#include "../std.h"
#include "../3D/3d.h"
#include "OpenGL.h"
#include "glErrorUtil.h"
#include "GLTexCube.h"

GLTexCube::GLTexCube(void)
:_texID( 0 ), _bIsValid( false )
{	
}

GLTexCube::~GLTexCube(void)
{
	if ( _texID > 0 )
	{		
		glDeleteTextures( 1, &_texID );
		_texID = 0;		
	}
	_bIsValid = false;
}

void GLTexCube::Bind( void ) const
{
	if ( !_bIsValid ) {
		LogError( "the texture is invalid!" );
		return;
	}
	glBindTexture( GL_TEXTURE_CUBE_MAP, _texID );
}

void GLTexCube::Unbind( void ) const
{
	if ( !_bIsValid ) {
		LogError( "the texture is invalid!" );
		return;
	}
	GLint currentId = 0;
	glGetIntegerv( GL_TEXTURE_BINDING_CUBE_MAP, &currentId );
	if ( _texID != currentId ) {
		LogError( "this texture is not binding!" );
		return;
	}
	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
}

void GLTexCube::SetTextureParams( void )
{
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
}

bool GLTexCube::BeginInitialize( void )
{
	if ( 0 != _texID ) {
		LogError( "texture already initialized" );
		return false;
	}

	glGenTextures( 1, &_texID );

	glBindTexture( GL_TEXTURE_CUBE_MAP, _texID );

	this->SetTextureParams();

	return true;
}

bool GLTexCube::EndInitialize( void )
{
	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	return _bIsValid = true;
}

bool GLTexCube::Initialize( int width, int height, GLint iformat /*= GL_RGBA32F */ )
{
	V_RET( BeginInitialize() );
	for ( int i = 0; i < 6; ++i )
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, iformat, width, height, 0, 0, 0, NULL );
	}
	return EndInitialize();	
}

bool GLTexCube::Initialize( int width[6], int height[6], GLint iformat /*= GL_RGBA32F */ )
{
	V_RET( BeginInitialize() );
	for ( int i = 0; i < 6; ++i )
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, iformat, width[ i ], height[ i ], 0, 0, 0, NULL );
	}
	return EndInitialize();	
}

/************************************************************************
 * class GLTexCubeInput begin
 ************************************************************************/
GLTexCubeInput::GLTexCubeInput( void )
{
	
}

/************************************************************************
 * the faces are stored in the order of X+, X-, Y+, Y-, Z+, Z-
 ************************************************************************/
bool GLTexCubeInput::ExtractCubeMap( const char* filename, ILuint* imgIDs )
{
	ilInit();
	ilOriginFunc( IL_ORIGIN_UPPER_LEFT );
	ilEnable( IL_ORIGIN_SET );

	ILuint imId = ilGenImage();
	ilBindImage( imId );
	if ( !ilLoadImage( filename ) )
	{
		std::strstream ss;
		ss << "failed to load file " << filename << std::endl;
		LogError( ss.str() );
		ilDeleteImage( imId );
		return false;
	}
	int crossW = ilGetInteger( IL_IMAGE_WIDTH );
	int crossH = ilGetInteger( IL_IMAGE_HEIGHT );
	int format = ilGetInteger( IL_IMAGE_FORMAT );
	int type = ilGetInteger( IL_IMAGE_TYPE );

	int w = crossW / 3;
	int h = crossH / 4;

	if ( w != h )
	{
		std::strstream ss;
		ss << filename << " is not a valid cube map!\n";
		LogError( ss.str() );
		ilDeleteImage( imId );
		return false;
	}
	
	int bpp = ilGetInteger( IL_IMAGE_BYTES_PER_PIXEL );
	int depth = ilGetInteger( IL_IMAGE_DEPTH );
	int channels = ilGetInteger( IL_IMAGE_CHANNELS );

	char* imData[6];
	int imSize = w * h * bpp;
	
	int offsetX[] = { 2, 0, 1, 1, 1, 1 };
	int offsetY[] = { 1, 1, 0, 2, 1, 3 };
	for ( int i = 0; i < 6; ++i ) {
		imgIDs[ i ] = ilGenImage();
		imData[ i ] = new char[ imSize ];
		ilCopyPixels( w * offsetX[ i ], h * offsetY[ i ], 0, w, h, depth, format, type, imData[ i ] );
	}	
	ilDeleteImage( imId );	
	for ( int i = 0; i < 6; ++i ) {
		ilBindImage( imgIDs[ i ] );
		ilEnable( IL_FILE_OVERWRITE );
		ilTexImage( w, h, depth, channels, format, type, imData[ i ] );	
		SAFE_RELEASE( imData[ i ] );
	}
	iluFlipImage();
	iluMirror();
	return true;
}

bool GLTexCubeInput::Load( const char* filename, GLint iformat /*= GL_RGBA32F */ )
{
	ILuint imgIDs[ 6 ];
	V_RET( this->ExtractCubeMap( filename, imgIDs ) );

	V_RET( BeginInitialize() );
	for ( int i = 0; i < 6; ++i )
	{
		ilBindImage( imgIDs[ i ] );
		int format = ilGetInteger( IL_IMAGE_FORMAT );
		int width = ilGetInteger( IL_IMAGE_WIDTH );
		int height = ilGetInteger( IL_IMAGE_HEIGHT );
		int type = ilGetInteger( IL_IMAGE_TYPE );
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, iformat, width, height, 0, format, type, ilGetData() );		
		ilDeleteImage( imgIDs[ i ] );
	}
	return EndInitialize();
}
