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
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	GLuint vao, vbo, vnbo, ebo;

	glm::vec3 ambient, diffuse, specular;
	float shininess;

public:
	Model3D(std::string objFilename, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
	~Model3D();

	void draw();
	void update();

	void resize(bool up);
	void spin(float deg, glm::vec3 rotAxis);

	glm::vec3 getAmbient();
	glm::vec3 getDiffuse();
	glm::vec3 getSpecular();
	float getShininess();

	void setModel(glm::mat4 M);

};

#endif
