#include "Line.h"

Line::Line(glm::vec3 p0, glm::vec3 p1)
{

	color = glm::vec3(0, 0, 1);

	Line::p0 = p0;
	Line::p1 = p1;

	points =
	{
		p0, p1
	};

	indices =
	{
		0, 1
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	// Bind to the VAO.
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	// Enable vertex attribute 0. 
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// bind to store indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	// Pass in data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);
}

Line::~Line()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Line::draw(GLuint shaderProgram, glm::mat4 C)
{
	glUniform1i(glGetUniformLocation(shaderProgram, "normalColor"), true);


	// Get the locations of uniform variables.
	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint objColorLoc = glGetUniformLocation(shaderProgram, "color");
	// change modelview matrix of the figure before drawing.


	// set parameters of the model for the vertex shader per geometry 
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(C * glm::mat4(1)));

	glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));

	// Bind to the VAO.
	glBindVertexArray(vao);
	// Set point size.
	glLineWidth(2.f);

	// Draw points 
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);

	glUniform1i(glGetUniformLocation(shaderProgram, "normalColor"), false);
}

void Line::update(glm::mat4 C)
{
	points.clear();
	points =
	{
		p0, p1
	};

	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	// Enable vertex attribute 0. 
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// bind to store indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	// Pass in data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);
}
