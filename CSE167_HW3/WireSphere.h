#ifndef _WIRE_SPHERE_H_
#define _WIRE_SPHERE_H_

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
#include <fstream>
#include <sstream>
#include <iostream>
#include <tuple>

#include "Node.h"
#include "Geometry.h"

class WireSphere : public Geometry
{
private:
	bool enabled;
public:
	WireSphere(std::string filename, float radius);

	void draw(GLuint shaderProgram, glm::mat4 C);
};

#endif
