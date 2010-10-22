#include <gl/glew.h>
#include <iostream>
#include <string>

#include "../3D/3d.h"
#include "glErrorUtil.h"
#include "ShaderObject.h"
#include "ProgramGLSL.h"

ProgramGLSL::ProgramGLSL( const std::string& tag /* = ""*/ )
: m_is_linked( false ), m_programID( 0 ), m_tag(tag)
{
	m_programID = glCreateProgram();
}

ProgramGLSL::~ProgramGLSL( void )
{
	if ( 0 != m_programID )
		glDeleteProgram( m_programID );
}

bool ProgramGLSL::UseProgram() const
{
	if ( IsValidProgram() )
	{
		glUseProgram( m_programID );
		return true;
	}
	else
	{
		CheckErrorsGL( this->m_tag.c_str(), LogStream::LS() );		
		return false;
	}
}

void ProgramGLSL::PrintLinkLog( std::ostream&os ) const
{
	GLint len = 0;	

	glGetProgramiv( m_programID, GL_INFO_LOG_LENGTH , &len );
	if( len <= 1 ) return;

	char* linkLog = new char[ len + 1 ];
	if( linkLog == NULL ) return;

	glGetProgramInfoLog( m_programID, len, &len, linkLog );

	linkLog[ len ] = 0;

	if( strstr( linkLog, "failed" ) )
	{
		os << m_tag << "\t" << linkLog + ( linkLog[0] == ' '? 1 : 0 ) << "\n";		
	}

	delete linkLog;
}

void ProgramGLSL::CheckLinkLog()
{
	GLint status;
	glGetProgramiv( m_programID, GL_LINK_STATUS, &status );
	m_is_linked = ( GL_TRUE == status );

	if ( !m_is_linked )
	{
		CheckErrorsGL("ProgramGLSL::CheckLinkLog");
		PrintLinkLog( LogStream::LS() );
	}
}

bool ProgramGLSL::LinkProgram()
{
	m_is_linked = false;

	if ( 0 == m_programID ) return false;

	glLinkProgram( m_programID );

	CheckLinkLog();

	return m_is_linked;
}

void ProgramGLSL::AttachShaderObject( ShaderObject& shader )
{
	if ( 0 != m_programID || shader.IsValidShaderObject() )		
	{
		glAttachShader( m_programID, shader.GetShaderID() );
	}
}

void ProgramGLSL::DetachShaderObject( ShaderObject& shader )
{
	if ( 0 != m_programID || shader.IsValidShaderObject() )		
	{
		glDetachShader( m_programID, shader.GetShaderID() );
	}
}

GLint ProgramGLSL::GetUniformLocation( const char* name ) const
{
	if ( IsValidProgram() )
	{
		return glGetUniformLocation( m_programID, name );
	}
	else
	{
		return -1;
	}
}

EffectGLSL::EffectGLSL( const std::string& tag /*= "" */ )
: ProgramGLSL( tag )
{

}

EffectGLSL::~EffectGLSL( void )
{

}

bool EffectGLSL::Load( const char* vshader, const char* fshader )
{
	if ( NULL != vshader )
		this->AttachShaderObject( ShaderObject( GL_VERTEX_SHADER, vshader ) );
	if ( NULL != fshader )
		this->AttachShaderObject( ShaderObject( GL_FRAGMENT_SHADER, fshader ) );
	return this->LinkProgram();
}

void EffectGLSL::Begin( void ) const
{
	this->UseProgram();
}

void EffectGLSL::End( void ) const
{
	glUseProgram( 0 );
}

void EffectGLSL::SetUniform( const char* name, int i ) const
{
	glUniform1i( this->GetUniformLocation( name ), i );
}

void EffectGLSL::SetUniform( const char* name, float f ) const
{
	glUniform1f( this->GetUniformLocation( name ), f );
}

void EffectGLSL::SetUniform( const char* name, int size, float* fv ) const
{
	glUniform1fv( this->GetUniformLocation( name ), size, fv );
}

void EffectGLSL::SetUniform( const char* name, const Vector3& v ) const
{
	glUniform3fv( this->GetUniformLocation( name ), 1, (const GLfloat*)&v );
}

void EffectGLSL::SetUniform( const char* name, const Color& c ) const
{
	glUniform4fv( this->GetUniformLocation( name ), 1, (const GLfloat*)&c );
}