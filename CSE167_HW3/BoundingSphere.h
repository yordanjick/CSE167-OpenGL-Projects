#ifndef _BOUNDING_SPHERE_H_
#define _BOUNDING_SPHERE_H_

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
#include "WireSphere.h"

class BoundingSphere : public Node
{
private:
	GLfloat radius;
	Geometry* sphere;
	GLfloat curFOV;

public:
	static bool render;
	BoundingSphere(Geometry* sphere, GLfloat radius);

	void draw(GLuint shaderProgram, glm::mat4 C);
	void update(glm::mat4 C);

	bool checkInView(glm::mat4 C);
};

#endif

