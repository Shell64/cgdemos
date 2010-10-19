#ifndef __SHADER_OBJECT_H__
#define __SHADER_OBJECT_H__

class ShaderObject
{
public:
	static int ReadSahderFile( const char* file, char*& code );
	
	ShaderObject( int type, const char* source, bool is_file = true );
	
	~ShaderObject( );

	inline bool IsValidShaderObject() { return 0 != m_shaderID && m_is_compiled; }

	inline bool IsValidVertexShader() { return GL_VERTEX_SHADER == m_type && IsValidShaderObject(); }

	inline bool IsValidFragmentShader() { return GL_FRAGMENT_SHADER == m_type && IsValidShaderObject(); }

	inline GLuint GetShaderID() { return m_shaderID; }

private:
	void CheckCompileLog();

	void PrintCompileLog( std::ostream & os  );

private:
	GLuint m_shaderID;
	int m_type;
	bool m_is_compiled;
};

#endif//__SHADER_OBJECT_H__
