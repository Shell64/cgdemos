#ifndef __SHADER_OBJECT_H__
#define __SHADER_OBJECT_H__

class ShaderObject
{
public:
	static int ReadSahderFile( const char* file, char*& code );
	
	ShaderObject( int type, const char* source, bool is_file = false );
	
	~ShaderObject( );

	inline bool IsValidShaderObject() const { return 0 != m_shaderID && m_is_compiled; }

	inline bool IsValidVertexShader() const { return GL_VERTEX_SHADER == m_type && IsValidShaderObject(); }

	inline bool IsValidFragmentShader() const { return GL_FRAGMENT_SHADER == m_type && IsValidShaderObject(); }

	inline GLuint GetShaderID() const { return m_shaderID; }

private:
	void CheckCompileLog();

	void PrintCompileLog( std::ostream & os  );

private:
	GLuint m_shaderID;
	int m_type;
	bool m_is_compiled;
};

#endif//__SHADER_OBJECT_H__
