#include "LightSource.h"

unsigned int LightSource::depthShader;

LightSource::LightSource(std::string path, glm::mat4 C)
{
	lightModel = new Model(path, C);
	position = C;
}

void LightSource::draw(glm::mat4 C, unsigned int shaderProgram)
{
	// don't render depth for light sources
	if (shaderProgram == depthShader)
		return;
	glUniform1i(glGetUniformLocation(shaderProgram, "ignoreLight"), true);
	lightModel->draw(C, shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "ignoreLight"), false);

}

void LightSource::update(glm::mat4 C)
{
	lightModel->update(C);
}
