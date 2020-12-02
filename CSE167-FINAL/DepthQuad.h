#ifndef _DEPTH_QUAD_H_
#define _DEPTH_QUAD_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <iostream>

class DepthQuad
{
public:
	DepthQuad();
	~DepthQuad();
	void draw();

private:
	unsigned int vao, vbo;
};
#endif
