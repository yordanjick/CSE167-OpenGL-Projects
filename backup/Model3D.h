#ifndef _MODEL_3D_H_
#define _MODEL_3D_H_

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

class Model3D : public Object
{
private:
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<GLuint> faces;
	GLuint vao, vbo, vnbo, ebo;
public:
	Model3D(std::string objFilename);
	~Model3D();

	void draw();
	void update();

	void spin(float deg);

};

#endif
