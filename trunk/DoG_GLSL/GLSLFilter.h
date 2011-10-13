#ifndef __GLSL_FILTER_H__
#define __GLSL_FILTER_H__

class ProgramGLSL;
class ShaderObject;
class GLTexImage;

class GLSLFilter
{
public:
	GLSLFilter(void);	

	~GLSLFilter(void);

	void Run( GLTexImage* src, GLTexImage* dst );

	void CalculateGuassianKernel( float sigma );

protected:
	class Pass
	{
	public:
		Pass( const char* shader_file );

		void Apply( float* kernel, float* offset, int kernelSize );

	protected:
		ProgramGLSL m_program;
		GLint m_kernelParam;
		GLint m_offsetParam;
		GLint m_kernelSizeParam;
		GLint m_texParam;
	};

protected:
	Pass m_hPass;
	Pass m_vPass;

	float* m_kernel;
	float* m_offset;
	int m_kernelSize;
};

#endif//__GLSL_FILTER_H__
