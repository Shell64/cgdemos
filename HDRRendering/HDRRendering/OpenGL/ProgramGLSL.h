#ifndef __PROGRAM_H__
#define __PROGRAM_H__

class ProgramGLSL
{
public:
	ProgramGLSL( const std::string& tag = "" );
	
	virtual ~ProgramGLSL(void);

	bool UseProgram() const;

	bool LinkProgram();	

	void AttachShaderObject( ShaderObject& shader );

	void DetachShaderObject( ShaderObject& shader );

	GLint GetUniformLocation( const char* name ) const;
	
	inline bool IsLinked( void ) const { return m_is_linked; }

	inline GLuint GetProgramID( void ) const { return m_programID; }
	
	inline bool IsValidProgram( void ) const { return 0 != m_programID && m_is_linked; }

private:
	void PrintLinkLog( std::ostream& os ) const;

	void CheckLinkLog( void );

private:

	GLuint m_programID;
	bool m_is_linked;

	std::string m_tag;
};

class EffectGLSL : public ProgramGLSL
{
public:
	EffectGLSL( const std::string& tag = "" );

	~EffectGLSL( void );

	bool Load( const char* vshader, const char* fshader );

	void Begin( void ) const;

	void End( void ) const;

	void SetUniform( const char* name, int i ) const;

	void SetUniform( const char* name, float f ) const;

	void SetUniform( const char* name, int size, float* fv ) const;

	void SetUniform( const char* name, const Vector3& v ) const;

	void SetUniform( const char* name, const Color& c ) const;

private:
	inline void AttachShaderObject( ShaderObject& shader ) { ProgramGLSL::AttachShaderObject( shader ); };

	inline void DetachShaderObject( ShaderObject& shader ) { ProgramGLSL::DetachShaderObject( shader ); };

	inline GLint GetUniformLocation( const char* name ) const { return ProgramGLSL::GetUniformLocation( name ); };
};

#endif//__PROGRAM_H__
