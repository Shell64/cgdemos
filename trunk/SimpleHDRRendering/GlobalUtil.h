#ifndef __GLOBAL_UTIL_H__
#define __GLOBAL_UTIL_H__

class GlobalParam
{
public:
	static GLuint s_texTarget;
};

class GlobalUtil : public GlobalParam
{
public:
	GlobalUtil(void);
	~GlobalUtil(void);

	static void SaveFrame( const char* filename );
};

#endif//__GLOBAL_UTIL_H__
