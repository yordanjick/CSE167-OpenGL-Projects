#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>
#include <iostream>

#include "stb_image.h"

class Skybox
{
private:
	GLuint vao;
	GLuint vbos[2];
	GLuint cubemapTexture;
	glm::mat4 model;
	glm::vec3 color;
public:
	Skybox(float size);
	~Skybox();

	void draw();
	void update();
	unsigned int loadCubemap(std::vector<std::string> faces);

};

#endif

