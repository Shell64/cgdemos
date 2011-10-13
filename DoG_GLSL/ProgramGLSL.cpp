#include <GL/glew.h>
#include <iostream>
#include <string>

#include "ShaderObject.h"
#include "ProgramGLSL.h"

ProgramGLSL::ProgramGLSL( std::string tag /* = ""*/ )
: m_is_linked( false ), m_programID( 0 ), m_tag(tag)
{
	m_programID = glCreateProgram();
}

ProgramGLSL::~ProgramGLSL( void )
{
	if ( 0 != m_programID )
		glDeleteProgram( m_programID );
}

bool ProgramGLSL::UseProgram()
{
	if ( IsValidProgram() )
	{
		glUseProgram( m_programID );
		return true;
	}
	else
	{
		std::cout << "invalid program: " << this->m_tag << std::endl;
		return false;
	}
}

void ProgramGLSL::PrintLinkLog( std::ostream&os )
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
		os << linkLog + ( linkLog[0] == ' '? 1 : 0 ) << "\n";
		m_is_linked = false;
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
		PrintLinkLog( std::cout );
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

void ProgramGLSL::AttachShaderObject(const ShaderObject &shader)
{
	if ( 0 != m_programID || shader.IsValidShaderObject() )		
	{
		glAttachShader( m_programID, shader.GetShaderID() );
	}
}

void ProgramGLSL::DetachShaderObject(const ShaderObject &shader)
{
	if ( 0 != m_programID || shader.IsValidShaderObject() )		
	{
		glDetachShader( m_programID, shader.GetShaderID() );
	}
}

GLint ProgramGLSL::GetUniformLocation( const char* name )
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
