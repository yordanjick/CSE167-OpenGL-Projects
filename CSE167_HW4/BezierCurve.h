#ifndef _BEZIER_CURVE_H_
#define _BEZIER_CURVE_H_

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

#include "Node.h"
#include "Geometry.h"
#include "Transform.h"

class BezierCurve : public Node
{
protected:
	std::vector<glm::vec3> points;
	std::vector<unsigned int> indices;
	glm::vec3 color;

	Geometry* sphere;
	Transform* c0, * c1, * c2;

	GLuint vao, vbo, ebo;

public:
	glm::vec3 p0, p1, p2, p3;
	glm::vec3 c0Color, c1Color, c2Color;

	BezierCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 color);
	~BezierCurve();

	glm::vec3  getPoint(float t);

	void draw(GLuint shaderProgram, glm::mat4 C);
	void update(glm::mat4 C);

	void recalcPoints();
	
};

#endif
