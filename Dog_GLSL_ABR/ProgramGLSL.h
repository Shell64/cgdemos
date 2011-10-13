#ifndef __PROGRAM_H__
#define __PROGRAM_H__

class ShaderObject;

class ProgramGLSL
{
public:
	ProgramGLSL();
	
	~ProgramGLSL(void);

	bool UseProgram();

	bool LinkProgram();	

	void AttachShaderObject( ShaderObject& shader );

	void DetachShaderObject( ShaderObject& shader );

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
};

#endif//__PROGRAM_H__
