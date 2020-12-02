#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

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

#include "Node.h"

class Geometry : public Node
{
protected:
	glm::mat4 model;

	glm::vec3 color;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> textures;
	std::vector<unsigned int> indices;
	GLuint vao, vbo, vnbo, ebo;

public:
	
	Geometry(std::string filename);
	~Geometry();

	void draw(GLuint shaderProgram, glm::mat4 C);
	void update(glm::mat4 C);
	void setColor(glm::vec3 c);
};

#endif
