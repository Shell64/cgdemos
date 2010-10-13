#include <gl/glew.h>
#include <stdio.h>
#include <iostream>
#include "glErrorUtil.h"
#include "ShaderObject.h"

ShaderObject::ShaderObject( int type, const char* source, bool is_file /*= false */ )
: m_type( type ), m_is_compiled( false ), m_shaderID( 0 )
{
	if ( NULL == source ) return;

	m_shaderID = glCreateShader( type );
	if ( 0 == m_shaderID ) return;	

	GLint code_length = 0;
	if ( is_file )
	{
		char* code;
		code_length = ReadSahderFile( source, code );
		if ( 0 == code_length ) return;
		glShaderSource( m_shaderID, 1, (const char**)&code, &code_length );
		delete[] code;
	}
	else
	{
		const char* code = source;
		code_length = strlen( code );
		glShaderSource( m_shaderID, 1, (const char**)&code, &code_length );
	}

	glCompileShader( m_shaderID );
	CheckCompileLog();
}

ShaderObject::~ShaderObject(void)
{
	if( m_shaderID ) glDeleteShader( m_shaderID );
}

int ShaderObject::ReadSahderFile( const char* file, char*& code )
{
	code = NULL;
	FILE * fd;
	int    len=0;

	if ( file == NULL ) return 0;

	fd = fopen( file, "rb" );
	if ( fd == NULL ) return 0;

	fseek( fd, 0, SEEK_END );
	len = ftell( fd );
	rewind( fd );
	if( len > 1 )
	{
		code = new char[ len + 1 ];
		fread( code, sizeof( char ), len, fd );
		code[ len ] = 0;
	}else
	{
		len = 0;
	}

	fclose( fd );

	return len;
}

void ShaderObject::CheckCompileLog()
{
	GLint status;
	glGetShaderiv( m_shaderID, GL_COMPILE_STATUS, &status );
	m_is_compiled = ( status == GL_TRUE );

	if( ! m_is_compiled )	PrintCompileLog( LogStream::LS() );
}

void ShaderObject::PrintCompileLog( std::ostream & os )
{
	GLint len = 0;	

	glGetShaderiv( m_shaderID, GL_INFO_LOG_LENGTH , &len );
	if( len <= 1 ) return;

	char* compileLog = new char[ len + 1 ];
	if( compileLog == NULL ) return;

	glGetShaderInfoLog( m_shaderID, len, &len, compileLog );

	os << "Compile Log\n" << compileLog << "\n";

	delete compileLog;
}