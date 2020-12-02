#ifndef _LIGHT_SOURCE_H_
#define _LIGHT_SOURCE_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Model.h"
#include "Node.h"
class LightSource :public Node
{
private:
	Model* lightModel;
public:

	static unsigned int depthShader;

	glm::mat4 position;

	LightSource(std::string path, glm::mat4 C);

	void draw(glm::mat4 C, unsigned int shaderProgram);
	void update(glm::mat4 C);



};
#endif

