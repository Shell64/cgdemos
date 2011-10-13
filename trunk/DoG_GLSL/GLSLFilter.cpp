#include <cmath>
#include <GL/glew.h>
#include <iostream>

#include "framebufferObject.h"

#include "ShaderObject.h"
#include "ProgramGLSL.h"
#include "GLTexImage.h"
#include "GLSLFilter.h"


#define HPassShader "GaussianFilterH.glsl"
#define VPassShader "GaussianFilterV.glsl"

GLSLFilter::Pass::Pass( const char* shader_file )
: m_program( shader_file )
{
	ShaderObject shader( GL_FRAGMENT_SHADER, shader_file, true );	
	m_program.AttachShaderObject( shader );
	m_program.LinkProgram();

	m_kernelParam = m_program.GetUniformLocation( "Kernel" ) ;
	m_offsetParam = m_program.GetUniformLocation( "Offset" );
	m_kernelSizeParam = m_program.GetUniformLocation( "KernelSize" );
	m_texParam = m_program.GetUniformLocation( "Tex" );
}

void GLSLFilter::Pass::Apply( float* kernel, float* offset, int kernelSize )
{
	m_program.UseProgram();
	glUniform1fv( m_kernelParam, kernelSize, kernel );
	glUniform1fv( m_offsetParam, kernelSize, offset );
	glUniform1i( m_kernelSizeParam, kernelSize );
	glUniform1i( m_texParam, 0 );
}

void GLSLFilter::CalculateGuassianKernel( float sigma )
{
	//pixel inside 3*sigma box
	int sz = int( ceil( 4/* a factor */* sigma - 0.5) ) ;
	int size = ( sz << 1 ) + 1;	

	if ( size == m_kernelSize ) return;
	else 
	{
		if ( m_kernelSize > 0 )
		{
			delete[] m_kernel;
			delete[] m_offset;
		}
		m_kernelSize = size;
	}

	m_kernel = new float[ m_kernelSize ];
	m_offset = new float[ m_kernelSize ];
	float rv = 1.0f / ( sigma * sigma );
	float v, ksum =0; 

	// pre-compute filter
	for ( int i = -sz ; i <= sz ; ++i ) 
	{
		m_kernel[ i + sz ] = v = exp( -0.5f * i * i * rv ) ;
		ksum += v;
		m_offset[ i + sz ] = float( i );
	}

	//normalize the kernel
	rv = 1.0f / ksum;
	for ( int i = 0; i< m_kernelSize; ++i )
		m_kernel[ i ] *= rv;
}

GLSLFilter::GLSLFilter( void )
: m_kernel( NULL ), m_offset( NULL ),
m_kernelSize( 0 ), m_hPass( HPassShader ),
m_vPass( VPassShader )
{

}

GLSLFilter::~GLSLFilter( void )
{
	if ( NULL != m_offset )
		delete m_offset;
	if ( NULL != m_kernel )
		delete m_kernel;
}

void GLSLFilter::Run( GLTexImage* src, GLTexImage* dst )
{
	if ( NULL == m_kernel || NULL == m_offset )
		return;
		
	FramebufferObject fbo;
	fbo.Bind();	
	
	dst->AttachToFBO( 0 );
	dst->FitViewport();

	src->BindTex();
	m_hPass.Apply( m_kernel, m_offset, m_kernelSize );
	dst->DrawQuad();

	dst->BindTex();
	m_vPass.Apply( m_kernel, m_offset, m_kernelSize );
	dst->DrawQuad();

	dst->DetachFBO(0);
	glUseProgram( 0 );
	FramebufferObject::Disable();
}
