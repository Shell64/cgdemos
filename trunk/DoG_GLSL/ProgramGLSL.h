#ifndef __PROGRAM_H__
#define __PROGRAM_H__

class ShaderObject;

class ProgramGLSL
{
public:
	ProgramGLSL( std::string tag = "" );
	
	~ProgramGLSL(void);

	bool UseProgram();

	bool LinkProgram();	

	void AttachShaderObject(const ShaderObject &shader);

	void DetachShaderObject(const ShaderObject &shader);

	GLint GetUniformLocation( const char* name );
	
	inline bool IsLinked() { return m_is_linked; }

	inline GLuint GetProgramID() { return m_programID; }
	
	inline bool IsValidProgram() { return 0 != m_programID && m_is_linked; }

private:
	void PrintLinkLog(std::ostream&os);

	void CheckLinkLog();

private:

	GLuint m_programID;
	bool m_is_linked;

	std::string m_tag;
};

#endif//__PROGRAM_H__
