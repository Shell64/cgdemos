#include "../std.h"
#include "../3D/3d.h"
#include "OpenGL.h"

#include "GlobalUtil.h"
#include "glErrorUtil.h"
#include "ShaderObject.h"
#include "ProgramGLSL.h"
#include "GLTexImage.h"

GLTexImage::GLTexImage(void)
:m_texWidth( 0 ), m_texHeight( 0 ),
m_texID( 0 ), _bIsValid( false )
{	
}

GLTexImage::~GLTexImage(void)
{
	this->Release();
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
	glTexCoord2i ( 0, 0 );          glVertex2i( 0, 0 ); 
	glTexCoord2i ( 0, 1  );         glVertex2i( 0, m_texHeight ); 
	glTexCoord2i ( 1, 1 );			glVertex2i( m_texWidth, m_texHeight ); 
	glTexCoord2i ( 1, 0 );           glVertex2i( m_texWidth, 0 ); 
	glEnd ();
	glFlush();
}



bool GLTexImage::Initialize( int width, int height, GLuint iformat /*= GL_RGBA32F_ARB*/, 
							GLuint format /*= 0*/, GLuint type /*= 0*/, const void* data /*= NULL */ )
{
	V_RET( BeginInitialize( ) );

	this->SetTextureParam();

	glTexImage2D( GlobalUtil::s_texTarget, 0, iformat, width, height, 0, format, type, data );

	return EndInitialize( width, height, iformat );	
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

void GLTexImage::FitViewport( )
{	
	glViewport( 0, 0, m_texWidth, m_texHeight ); 
	glMatrixMode( GL_PROJECTION );    
	glLoadIdentity();
	glOrtho( 0, m_texWidth, 0, m_texHeight,  0, 1 );
	glMatrixMode( GL_MODELVIEW );     
	glLoadIdentity();
}

bool GLTexImage::BeginInitialize( void )
{	
	CheckErrorsGL( "BeginInitialize:" );
	if ( m_texID > 0 )
	{
		this->Release();
	}
	glGenTextures( 1, &m_texID );
	glBindTexture( GlobalUtil::s_texTarget, m_texID );	
	return true;
}

bool GLTexImage::EndInitialize( int width, int height, GLuint iformat )
{
	glBindTexture( GlobalUtil::s_texTarget, 0 );

	GLenum err = glGetError();
	if ( GL_NO_ERROR != err )
	{
		const char *errstr;
		errstr = reinterpret_cast<const char *>(gluErrorString(err));
		LogError( errstr );
		return false;
	}

	m_texWidth = width;
	m_texHeight = height;
	m_format = iformat;
	_bIsValid = true;

	return true;
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
	SAFE_RELEASE( data );
}

void GLTexImage::DrawQuadLeft()
{
	glBegin (GL_QUADS);
	glTexCoord2f ( 0, 0 );          glVertex2i( 0, 0 ); 
	glTexCoord2f ( 0, 1  );         glVertex2i( 0, m_texHeight ); 
	glTexCoord2f ( 0.5, 1 );			glVertex2i( m_texWidth/2, m_texHeight ); 
	glTexCoord2f ( 0.5, 0 );           glVertex2i( m_texWidth/2, 0 ); 
	glEnd ();
	glFlush();
}

void GLTexImage::DrawQuadRight()
{
	glBegin (GL_QUADS);
	glTexCoord2f ( 0.5, 0 );          glVertex2i( m_texWidth/2, 0 ); 
	glTexCoord2f ( 0.5, 1  );         glVertex2i( m_texWidth/2, m_texHeight ); 
	glTexCoord2f ( 1, 1 );			glVertex2i( m_texWidth, m_texHeight ); 
	glTexCoord2f ( 1, 0 );           glVertex2i( m_texWidth, 0 ); 
	glEnd ();
	glFlush();
}

void GLTexImage::Release( void )
{
	if ( m_texID > 0 )
	{
		glDeleteTextures( 1, &m_texID );
		m_texID = 0;
		m_texWidth = m_texHeight = 0;
		m_format = 0;
		_bIsValid = false;
	}
}

/************************************************************************
 * class GLTexInput begin
 ************************************************************************/
bool GLTexInput::Load( const char* filename, 
								   GLuint iformat /*= GL_RGBA*/ )
{	
	GLuint imId = 0;
	V_RET( LoadFile( filename, imId ) );

	int width = ilGetInteger( IL_IMAGE_WIDTH );
	int height = ilGetInteger( IL_IMAGE_HEIGHT );	
	int format = ilGetInteger( IL_IMAGE_FORMAT );	
	int type = ilGetInteger( IL_IMAGE_TYPE );	

	bool ret = this->Initialize( width, height, iformat, format, type, ilGetData() );	
	
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
	V_RET( GLTexImage::Initialize( width, height, iformat ) );
	
	_fbo.Bind();

	this->AttachToFBO( 0 );

	InitDepthRenderBuffer( width, height, _depthRB );

	return EndInitialize();	
}

void GLTexFBO::BeginCapture()
{
	_fbo.Bind();
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

GLTexAttachment::GLTexAttachment( )
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

	s_pFBO->Bind();

	this->AttachToFBO( 0 );

	return EndInitialize();
}

void GLTexAttachment::BeginCapture( void )
{
	if ( NULL == s_pFBO ) {
		return;
	}
	s_pFBO->Bind();
	this->AttachToFBO( 0 );
}

bool GLTexAttachment::InitFbo( int width /*= 0*/, int height /*= 0 */ )
{
	if ( NULL == s_pFBO ) {
		s_pFBO = new FramebufferObject;
		s_pFBO->Bind();

		if ( width > 0 && height > 0 ) {
			InitDepthRenderBuffer( width, height, s_depthRB );
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

void GLTexFBOBase::AttachToFBO( int i )
{
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, 
		i + GL_COLOR_ATTACHMENT0_EXT, GlobalUtil::s_texTarget, m_texID, 0 );
}

void GLTexFBOBase::DetachFBO( int i )
{
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT,
		i + GL_COLOR_ATTACHMENT0_EXT, GlobalUtil::s_texTarget, 0, 0 );
}

void GLTexFBOBase::EndCapture()
{
	FramebufferObject::Disable();
}

void GLTexFBOBase::ClearBuffer( GLbitfield color /*= GL_COLOR_BUFFER_BIT*/,
							   GLbitfield depth /*= GL_DEPTH_BUFFER_BIT*/, 
							   GLbitfield stencil /*= 0 */ )
{
	glClear( color | depth | stencil );
}

GLTexFBOBase::~GLTexFBOBase( void )
{

}

bool GLTexFBOBase::EndInitialize( void )
{
	bool ret= false;
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch( status )
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		ret = true;
		break;
	default:
		LogError( "fbo creation error");		
		ret = false;
		break;
	}	
	FramebufferObject::Disable();
	return ret;
}

void GLTexFBOBase::InitDepthRenderBuffer( int width, int height, GLuint& depthRB )
{
	glGenRenderbuffersEXT( 1, &depthRB );
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, depthRB );
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 
		width, height );
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, 
		GL_RENDERBUFFER_EXT, depthRB );
}