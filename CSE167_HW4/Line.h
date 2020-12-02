#ifndef _LINE_H_
#define _LINE_

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

class Line : public Node
{
protected:
	std::vector<glm::vec3> points;
	std::vector<unsigned int> indices;

	GLuint vao, vbo, ebo;

public:
	glm::vec3 p0, p1;
	glm::vec3 color;

	Line(glm::vec3 p0, glm::vec3 p1);
	~Line();

	void draw(GLuint shaderProgram, glm::mat4 C);
	void update(glm::mat4 C);

};

#endif