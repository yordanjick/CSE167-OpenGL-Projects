#ifndef _LIGHT_SOURCE_H_
#define _LIGHT_SOURCE_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Object.h"

class LightSource : public Object
{
private:
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> faces;
	GLuint vao, vbo, vnbo, ebo;

	glm::vec3 lightPos;
public:
	LightSource(std::string objFilename, glm::vec3 lightPos, glm::vec3 color);
	~LightSource();

	void draw();
	void update();

	void resize(bool up);
	void spin(float deg, glm::vec3 rotAxis);

	void moveOrigin(bool towards);
	glm::vec3 getLightPos();


};

#endif

