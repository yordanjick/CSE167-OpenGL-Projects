#include "WireSphere.h"

WireSphere::WireSphere(std::string filename, float radius) : Geometry(filename)
{
	enabled = false;
	model = glm::scale(glm::vec3(radius)) * model;
}

void WireSphere::draw(GLuint shaderProgram, glm::mat4 C)
{
	// Get the locations of uniform variables.
	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint objColorLoc = glGetUniformLocation(shaderProgram, "color");
	// change modelview matrix of the figure before drawing.


	// set parameters of the model for the vertex shader per geometry 
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(C * model));
	glUniform3fv(objColorLoc, 1, glm::value_ptr(color));

	// bind vao
	glBindVertexArray(vao);

	// draw elements
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);


	// unbind vao
	glBindVertexArray(0);
}
