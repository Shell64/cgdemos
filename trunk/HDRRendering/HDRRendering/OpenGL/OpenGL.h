#ifndef __OPENGL_H__
#define __OPENGL_H__

#include <gl/glew.h>

#include "../3D/Vector3.h"
#include "FramebufferObject.h"
#include "ShaderObject.h"
#include "ProgramGLSL.h"
#include "GLTexImage.h"
#include "GLTexCube.h"
#include "glm_avl.h"
#include "../Mesh/TriMesh.h"
#include "../Mesh/TriMeshManager.h"

#else
#error __OPENGL_H__ already defined!
#endif//__OPENGL_H__