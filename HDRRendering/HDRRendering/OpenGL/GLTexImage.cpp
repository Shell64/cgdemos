#include "../std.h"
#include "../3D/3d.h"
#include "OpenGL.h"

#include "GlobalUtil.h"
#include "glErrorUtil.h"
#include "ShaderObject.h"
#include "ProgramGLSL.h"
#include "GLTexImage.h"

GLTexImage::GLTexImage(void)
: m_drawHeight( 0 ), m_drawWidth( 0 ),
m_imageWidth( 0 ), m_imageHeight( 0 ),
m_texWidth( 0 ), m_texHeight( 0 ),
m_texID( 0 )
{	
}

GLTexImage::~GLTexImage(void)
{
	if ( m_texID > 0 )
	{
		glDeleteTextures( 1, &m_texID );
		m_texID = 0;
	}
}

void GLTexImage::Bind()
{
	if ( !_bIsValid ) {
		LogError( "the texture is invalid!" );
		return;
	}
	glBindTexture( GlobalUtil::s_texTarget, m_texID );
}

void GLTexImage::Unbind()
{
	if ( !_bIsValid ) {
		LogError( "the texture is invalid!" );
		return;
	}
	GLint currentId = 0;
	glGetIntegerv( GL_TEXTURE_BINDING_2D, &currentId );
	if ( m_texID != currentId ) {
		LogError( "this texture is not binding!" );
		return;
	}
	glBindTexture( GlobalUtil::s_texTarget, 0 );
}

void GLTexImage::DrawQuad( int x1, int y1, int x2, int y2 )
{
	glClear( GL_COLOR_BUFFER_BIT );
	glBegin (GL_QUADS);
	glTexCoord2i ( 0, 0 );  glVertex2i( x1, y1 ); 
	glTexCoord2i ( 0, 1 );  glVertex2i( x1, y2 ); 
	glTexCoord2i ( 1, 1 );	glVertex2i( x2, y2 ); 
	glTexCoord2i ( 1, 0 );  glVertex2i( x2, y1 ); 
	glEnd ();
	glFlush();
}

void GLTexImage::DrawQuad()
{
	glBegin (GL_QUADS);
	glTexCoord2i ( 0, 0 );                     glVertex2i( 0, 0 ); 
	glTexCoord2i ( 0, 1  );         glVertex2i( 0, m_drawHeight ); 
	glTexCoord2i ( 1, 1 );			glVertex2i( m_drawWidth, m_drawHeight ); 
	glTexCoord2i ( 1, 0 );           glVertex2i( m_drawWidth, 0 ); 
	glEnd ();
	glFlush();
}

void GLTexImage::AttachToFBO( int i )
{
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, 
		i + GL_COLOR_ATTACHMENT0_EXT, GlobalUtil::s_texTarget, m_texID, 0 );
}

void GLTexImage::DetachFBO( int i )
{
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT,
		i + GL_COLOR_ATTACHMENT0_EXT, GlobalUtil::s_texTarget, 0, 0 );
}

bool GLTexImage::Initialize( int width, int height, GLuint iformat /*= GL_RGBA32F_ARB*/, 
							GLuint format /*= 0*/, GLuint type /*= 0*/, const void* data /*= NULL */ )
{
	V_RET( BeginInitialize() );

	m_texWidth = m_imageWidth = m_drawWidth = width;
	m_texHeight = m_imageHeight = m_drawHeight = height;	
	m_format = iformat;

	this->SetTextureParam();

	glTexImage2D( GlobalUtil::s_texTarget, 0, iformat, width, height, 0, format, type, data );

	return EndInitialize();	
}

void GLTexImage::SetTextureParam()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT , 1); 
	glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ); 
	glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ); 
	glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); 
	glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); 
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
}

void GLTexImage::DrawImage( )
{
	/* send elapsed time to shaders*/	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable( GL_TEXTURE_RECTANGLE_ARB );
	glBindTexture( GlobalUtil::s_texTarget, m_texID );

	glBegin(GL_QUADS);
	{
		glTexCoord2i( 0, 0 );                      glVertex2i( 0, m_drawHeight );
		glTexCoord2i( m_drawWidth, 0 );            glVertex2i( m_drawWidth, m_drawHeight );
		glTexCoord2i( m_drawWidth, m_drawHeight ); glVertex2i( m_drawWidth, 0 );
		glTexCoord2i( 0, m_drawHeight );           glVertex2i( 0, 0 );
	}
	glEnd();
	glFlush();
}

void GLTexImage::FitViewport( )
{	
	glViewport( 0, 0, m_drawWidth, m_drawHeight ); 
	glMatrixMode( GL_PROJECTION );    
	glLoadIdentity();
	glOrtho( 0, m_drawWidth, 0, m_drawHeight,  0, 1 );
	glMatrixMode( GL_MODELVIEW );     
	glLoadIdentity();
}

bool GLTexImage::BeginInitialize( void )
{
	if( 0 == m_texID ) glGenTextures( 1, &m_texID );
	else
	{
		LogError( "texture already initialized" );
		return false;
	}
	glBindTexture( GlobalUtil::s_texTarget, m_texID );
	return true;
}

bool GLTexImage::EndInitialize( void )
{
	glBindTexture( GlobalUtil::s_texTarget, 0 );
	return _bIsValid =true;
}

void GLTexImage::Save( const char* filename )
{
	this->Bind();
	float* data = new float[ 4 * m_texWidth * m_texHeight ];
	glGetTexImage( GlobalUtil::s_texTarget, 0, GL_RGBA, GL_FLOAT, data );
	ilInit();
	ILuint id = ilGenImage();
	ilTexImage( m_texWidth, m_texHeight, 1, 4, IL_RGBA, IL_FLOAT, data );
	ilSaveImage( filename );
	ilDeleteImage(id);
}

/************************************************************************
 * class GLTexInput begin
 ************************************************************************/
bool GLTexInput::Load( const char* filename, 
								   GLuint iformat /*= GL_RGBA*/,
								   bool color /*= true */ )
{
	GLuint imId = 0;
	V_RET( LoadFile( filename, imId ) );

	int width = ilGetInteger( IL_IMAGE_WIDTH );
	int height = ilGetInteger( IL_IMAGE_HEIGHT );	
	int format = ilGetInteger( IL_IMAGE_FORMAT );	
	int type = ilGetInteger( IL_IMAGE_TYPE );	

	bool ret = false;
	if ( IL_LUMINANCE == format || IL_LUMINANCE_ALPHA == format )
	{
		ret = this->Initialize( width, height, iformat, format, type, ilGetData() );
	}
	else if ( color )
	{
		ret = this->Initialize( width, height, iformat, format, type, ilGetData() );
	}
	else
	{
		unsigned char* gray_image = CreateGrayImage( 
			(const unsigned char*)ilGetData(), width, height, format );

		if ( NULL != gray_image )
		{
			ret = this->Initialize( width, height, iformat, GL_LUMINANCE, GL_UNSIGNED_BYTE, gray_image );
			SAFE_RELEASE( gray_image );
		}		
	}	

	ilDeleteImage( imId );
	return ret;
}

unsigned char* GLTexInput::CreateGrayImage( 
	const unsigned char* img, int width, int height, int format )
{
	if ( NULL == img || width == 0 || height == 0 ) return NULL;
	if ( GL_RGB != format && GL_RGBA != format ) return NULL;

	unsigned char* gray_image = new unsigned char[ width * height ];	
	if ( NULL == gray_image )
		return NULL;

	const unsigned char *line = img, *p;
	unsigned char* po = gray_image;
	int step = format == GL_RGB? 3: 4 ; 
	int linestep = width * step; 

	for( int i = 0, j = 0 ; i < height; ++i, line += linestep )
	{
		for( j = 0, p = line; j < width; ++j, p += step )
		{
			//*po++ = int(p[0]*0.299 + p[1] * 0.587 + p[2]* 0.114 + 0.5);
			*po++ = ( ( 19595*p[0] + 38470*p[1] + 7471*p[2]+ 32768 ) >> 16 );
		}
	}
	return gray_image;
}

bool GLTexInput::LoadFile( const char* filename, GLuint& imId )
{
	ilInit();
	ilOriginFunc( IL_ORIGIN_UPPER_LEFT );
	ilEnable( IL_ORIGIN_SET );

	imId = ilGenImage();
	ilBindImage( imId );
	if ( !ilLoadImage( filename ) )
	{		
		ilDeleteImage( imId );
		std::strstream ss;
		ss << "failed to load file " << filename;
		LogError( ss.str() );
		return false;
	}
	return true;
}

bool GLTexFBO::Initialize( int width, int height, GLuint iformat /*= GL_RGBA16F_ARB */ )
{
	V_RET( GLTexImage::Initialize( width, height, iformat, GL_RGBA, GL_UNSIGNED_BYTE, NULL ) );		
	//static GLuint depth_rb = 
	_fbo.Bind();
	this->AttachToFBO( 0 );

	glGenRenderbuffersEXT( 1, &_depthRB );
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, _depthRB );
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height );
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _depthRB );

	bool ret = true;
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch( status )
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		break;
	default:
		LogError( "frame buffer is incomplete");
		ret = false;
	}

	FramebufferObject::Disable();
	return ret;
}

void GLTexFBO::BeginCapture()
{
	_fbo.Bind();
}

void GLTexFBO::EndCapture()
{
	FramebufferObject::Disable();
}

GLTexFBO::~GLTexFBO( void )
{
	if ( 0 != _depthRB ) {
		glDeleteRenderbuffers( 1, & _depthRB );
		_depthRB = 0;
	}
}

GLTexFBO::GLTexFBO( void )
: _depthRB( 0 )
{
	
}

FramebufferObject* GLTexAttachment::s_pFBO = NULL;

int GLTexAttachment::s_count = 0;

GLuint GLTexAttachment::s_depthRB = 0;

GLTexAttachment::GLTexAttachment( int i /*= 0 */ )
: _attachID( 0 )
{
	++s_count;
}

GLTexAttachment::~GLTexAttachment( void )
{
	--s_count;
	if ( 0 == s_count )
	{
		ReleaseeFbo();
	}
}

bool GLTexAttachment::Initialize( int width, int height, 
								 GLuint iformat /*= GL_RGBA16F_ARB */ )
{
	V_RET( InitFbo( width, height ) );
	V_RET( GLTexImage::Initialize( width, height, iformat,
		GL_RGBA, GL_UNSIGNED_BYTE, 0 ) );
	this->AttachToFBO( _attachID );

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch( status )
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		return true;
	default:
		LogError( "fbo creation error");
		ReleaseeFbo();
		return false;
	}	
}

void GLTexAttachment::BeginCapture( void )
{
	if ( NULL == s_pFBO ) {
		return;
	}
	s_pFBO->Bind();
	this->AttachToFBO( _attachID );
}

void GLTexAttachment::EndCapture( void )
{
	FramebufferObject::Disable();
}

bool GLTexAttachment::InitFbo( int width /*= 0*/, int height /*= 0 */ )
{
	if ( NULL == s_pFBO ) {
		s_pFBO = new FramebufferObject;
		s_pFBO->Bind();

		if ( width > 0 && height > 0 ) {
			glGenRenderbuffersEXT( 1, &s_depthRB );
			glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, s_depthRB );
			glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 
				width, height );
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, 
				GL_RENDERBUFFER_EXT, s_depthRB );
		}		
	}
	else
	{
		s_pFBO->Bind();
	}
	return true;
}

bool GLTexAttachment::ReleaseeFbo( void )
{
	SAFE_RELEASE( s_pFBO );
	if ( 0 != s_depthRB ) {
		glDeleteRenderbuffers( 1, &s_depthRB );
		s_depthRB = 0;
	}
	return true;
}