#include <gl/glew.h>
#include <iostream>
#include "il/il.h"

#include "GlobalUtil.h"
#include "glErrorUtil.h"
#include "framebufferObject.h"
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
}

void GLTexImage::BindTex()
{
	glBindTexture( GlobalUtil::s_texTarget, m_texID );
}

void GLTexImage::UnbindTex()
{
	glBindTexture( GlobalUtil::s_texTarget, 0 );
}

void GLTexImage::DrawQuad()
{
	glBegin (GL_QUADS);
	glTexCoord2i ( 0, 0 );                     glVertex2i( 0, 0 ); 
	glTexCoord2i ( 0, m_drawHeight  );         glVertex2i( 0, m_drawHeight ); 
	glTexCoord2i ( m_drawWidth, m_drawHeight );glVertex2i( m_drawWidth, m_drawHeight ); 
	glTexCoord2i ( m_drawWidth, 0 );           glVertex2i( m_drawWidth, 0 ); 
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

void GLTexImage::InitTexture( int width, int height,
							 GLuint format /*= GL_RGBA32F_ARB*/,
							 bool clamp_to_edge /*= true */ )
{
	if( 0 != m_texID && width == m_texWidth && height == m_texHeight ) return;
	if( 0 == m_texID ) glGenTextures( 1, &m_texID );

	m_texWidth = m_imageWidth = m_drawWidth = width;
	m_texHeight = m_imageHeight = m_drawHeight = height;

	BindTex();

	if( clamp_to_edge )
	{
		glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ); 
		glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ); 
	}
	else
	{
		//out of bound tex read returns 0??
		glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER ); 
		glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER ); 
	}
	glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); 
	glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); 
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D( GlobalUtil::s_texTarget, 0, format, m_texWidth, m_texHeight, 0, GL_RGBA, GL_FLOAT, NULL); 

	UnbindTex();
}

void GLTexImage::SetTextureParam()
{
	glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ); 
	glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ); 
	glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); 
	glTexParameteri( GlobalUtil::s_texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); 
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

void GLTexImage::MakeGray()
{	
	FramebufferObject fbo;
	fbo.Bind();	

	ProgramGLSL grayProgram;
	grayProgram.AttachShaderObject(
		ShaderObject( GL_FRAGMENT_SHADER, "GrayScale.glsl", true ) );
	grayProgram.LinkProgram();
	grayProgram.UseProgram();
	GLint texParam = grayProgram.GetUniformLocation( "Tex" );
	glUniform1i( texParam, 0 );

	this->AttachToFBO( 0 );
	this->FitViewport();

	this->BindTex();
	this->DrawQuad();
	this->UnbindTex();
	this->DetachFBO( 0 );
	glUseProgram(0);
	FramebufferObject::Disable();
}

bool GLTexInput::SetImageData( int width, int height,
							  const void * data,
							  GLuint gl_iformat,
							  unsigned int gl_format,
							  unsigned int gl_type )
{
	//check whether the image format is supported
	
	//check whether the image exceed the maximum size of texture

	if( m_texID == 0 ) glGenTextures( 1, &m_texID ); 
	BindTex();
	CheckErrorsGL( "glBindTexture" );

	m_texWidth = m_imageWidth = m_drawWidth = width;
	m_texHeight = m_imageHeight = m_drawHeight = height;
	m_format = gl_format;

	glPixelStorei(GL_UNPACK_ALIGNMENT , 1); 
	SetTextureParam();	

	glTexImage2D( GlobalUtil::s_texTarget, 0, gl_iformat, //internal format changed
		m_imageWidth, m_imageHeight, 0,
		gl_format, GL_UNSIGNED_BYTE, data );
	
	UnbindTex();

	return true;
}

bool GLTexInput::LoadImageFromFile( const char* filename, 								  
								   bool color /*= true*/)
{
	ilInit();
	ilOriginFunc( IL_ORIGIN_UPPER_LEFT );
	ilEnable( IL_ORIGIN_SET );

	unsigned int imId = ilGenImage();
	ilBindImage( imId );
	if ( !ilLoadImage( filename ) )
	{
		std::cout << "failed to load file.\n";
		ilDeleteImage( imId );
		return false;
	}
	int width = ilGetInteger( IL_IMAGE_WIDTH );
	int height = ilGetInteger( IL_IMAGE_HEIGHT );	

	int ilformat = ilGetInteger( IL_IMAGE_FORMAT );	
	bool ret = true;

	if ( color || GL_LUMINANCE == ilformat )
	{
		GLuint iformat = GL_RGBA32F_ARB;
		if ( GL_LUMINANCE == ilformat )
			iformat = GL_LUMINANCE32F_ARB;
		ret = SetImageData( width, height, ilGetData(), iformat, ilformat, GL_UNSIGNED_BYTE );
	}
	else 
	{
		unsigned char* gray_image = CreateGrayImage( 
			(const unsigned char*)ilGetData(), width, height, ilformat );
		if ( NULL == gray_image ) return false;
		ret = SetImageData( width, height, gray_image, GL_LUMINANCE32F_ARB, GL_LUMINANCE, GL_UNSIGNED_BYTE );	
		delete[] gray_image;
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