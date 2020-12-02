#include "BezierCurve.h"

BezierCurve::BezierCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 color)
{
	BezierCurve::color = color;
	BezierCurve::p0 = p0;
	BezierCurve::p1 = p1;
	BezierCurve::p2 = p2;
	BezierCurve::p3 = p3;

	c0Color = glm::vec3(1, 0, 0);
	c1Color = glm::vec3(0, 1, 0);
	c2Color = glm::vec3(0, 1, 0);

	sphere = new Geometry("sphere.obj");

	sphere->setColor(glm::vec3(1, 0, 0));

	c0 = new Transform(glm::scale(glm::translate(p0), glm::vec3(.1f)));
	c1 = new Transform(glm::scale(glm::translate(p1), glm::vec3(.1f)));
	c2 = new Transform(glm::scale(glm::translate(p2), glm::vec3(.1f)));

	c0->addChild(sphere);
	c1->addChild(sphere);
	c2->addChild(sphere);

	// x(t) = [p0, p1, p2, p3] * Bernstein_polynomial * vec4(t^3, t^2, t, 1)
	// where t is in range (1..N)/N
	// N in this case is 150
	// add calculated x(t)s to points and draw
	// update them only when needed

	for (float i = 0; i <= 150; i++)
	{
		float t = i / 150.f;
		glm::vec3 a = -1.f * p0 + 3.f * p1 - 3.f * p2 + p3;
		glm::vec3 b = 3.f * p0 - 6.f * p1 + 3.f * p2;
		glm::vec3 c = -3.f * p0 + 3.f * p1;
		glm::vec3 d = p0;

		glm::vec3 pt = a * t * t * t + b * t * t + c * t + d;
	
		// std::cerr << pt.x << " " << pt.y << " " << pt.z << std::endl;

		points.push_back(pt);
	}

	for (unsigned int i = 0; i < 150; i++)
	{
		indices.push_back(i);
		indices.push_back(i + 1);
	}

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

BezierCurve::~BezierCurve()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

// get point x(t) at time t
glm::vec3 BezierCurve::getPoint(float t)
{
	return points[t];

}

void BezierCurve::draw(GLuint shaderProgram, glm::mat4 C)
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
	glLineWidth(2.5f);

	// Draw points 
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);

	sphere->setColor(c0Color);
	c0->draw(shaderProgram, C);

	sphere->setColor(c1Color);
	c1->draw(shaderProgram, C);

	sphere->setColor(c2Color);
	c2->draw(shaderProgram, C);


	glUniform1i(glGetUniformLocation(shaderProgram, "normalColor"), false);

	
}

void BezierCurve::update(glm::mat4 C)
{
	c0->setTransform(glm::scale(glm::translate(p0), glm::vec3(.1f)));
	c1->setTransform(glm::scale(glm::translate(p1), glm::vec3(.1f)));
	c2->setTransform(glm::scale(glm::translate(p2), glm::vec3(.1f)));
}

// clears all current points and recalculates them.
// should be called whenever changing control points.
void BezierCurve::recalcPoints()
{
	points.clear();

	for (float i = 0; i <= 150; i++)
	{
		float t = i / 150.f;
		glm::vec3 a = -1.f * p0 + 3.f * p1 - 3.f * p2 + p3;
		glm::vec3 b = 3.f * p0 - 6.f * p1 + 3.f * p2;
		glm::vec3 c = -3.f * p0 + 3.f * p1;
		glm::vec3 d = p0;

		glm::vec3 pt = a * t * t * t + b * t * t + c * t + d;
	
		// std::cerr << pt.x << " " << pt.y << " " << pt.z << std::endl;

		points.push_back(pt);
	}


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
