#ifndef __TEX_IMAGE_H__
#define __TEX_IMAGE_H__


class GLTexImage
{
public:
	GLTexImage(void);

	~GLTexImage(void);

protected:
	GLuint m_texID;
	int m_image_width;
	int m_image_height;
	int m_tex_width;
	int m_tex_height;
};

#endif//__TEX_IMAGE_H__
