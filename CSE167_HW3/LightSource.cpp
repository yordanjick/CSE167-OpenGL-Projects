#include "LightSource.h"

LightSource::LightSource(std::string objFilename, glm::vec3 lightPos, glm::vec3 color)
{
	LightSource::lightPos = lightPos;
	LightSource::color = color;

	std::ifstream objFile(objFilename); // The obj file we are reading.

	// Check whether the file can be opened.
	if (objFile.is_open())
	{
		std::string line; // A line in the file.

		// Read lines from the file.
		while (std::getline(objFile, line))
		{
			// Turn the line into a string stream for processing.
			std::stringstream ss;
			ss << line;

			// Read the first word of the line.
			std::string label;
			ss >> label;

			// If the line is about vertex (starting with a "v").
			if (label == "v")
			{
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 point;
				ss >> point.x >> point.y >> point.z;

				// Process the point. For example, you can save it to a.
				points.push_back(point);
			}
			// If the line is about vertex normal (starting with a "vn").
			else if (label == "vn")
			{
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 normal;
				ss >> normal.x >> normal.y >> normal.z;

				// Process the point.
				normals.push_back(normal);
			}
			// If line is about a face (starting with "f")
			else if (label == "f")
			{
				std::string face;

				// add all 3 indices per line
				while (ss >> face)
				{
					//std::string face1, face2, face3;
					//ss >> face1 >> face2 >> face3;
					std::string::size_type sz;

					// get index of the face and store it as an int in faces
					// subtract 1 since indices start at 1 in .obj file
					faces.push_back((unsigned int)(std::stoul(face, &sz) - 1));
					//faces.push_back((unsigned int)(std::stoul(face2, &sz)));
					//faces.push_back((unsigned int)(std::stoul(face3, &sz)));
				}
			}

		}
	}
	else
	{
		std::cerr << "Can't open the file " << objFilename << std::endl;
	}

	std::cerr << "file " << objFilename << " read" << std::endl;

	objFile.close();

	// Set the model matrix to an identity matrix. 
	model = glm::mat4(1);

	// scale light source down and translate to point light pos
	// model = glm::scale(model, glm::vec3(0.2f));
	model = glm::translate(model, lightPos);
	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	// vertex normals buffer objects (VNBO), element buffer objects (EBO)
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vnbo);
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

	// Unbind from the VBO.
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	// glBindVertexArray(0);

	// bind VNBO, 2nd vbo
	glBindBuffer(GL_ARRAY_BUFFER, vnbo);

	// pass in data
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(),
		normals.data(), GL_STATIC_DRAW);

	// Enable vertex attribute 1 (normal array buffer). 
	// We will be able to access points through it.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind from the VBO.
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	// glBindVertexArray(0);

	// Bind EBO as an element array buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faces.size(),
		faces.data(), GL_STATIC_DRAW);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);

	

}

LightSource::~LightSource()
{
	// Delete the VBO, VNBO, EBO, and the VAO.
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vnbo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void LightSource::draw()
{
	// bind vao
	glBindVertexArray(vao);

	// draw elements
	glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);

	// unbind vao
	glBindVertexArray(0);
}

void LightSource::update()
{
}

void LightSource::resize(bool up)
{

	if (up)
	{
		// scale identity matrix by factor of s > 1
		// scale model size up

		model = scale(glm::vec3(1.005f)) * model;

	}
	else
	{
		// scale model size down
		// scale identity matrix by factor of s < 1
		model = scale(glm::vec3(.995f)) * model;
	}

}

void LightSource::spin(float deg, glm::vec3 rotAxis)
{
	model = glm::rotate(glm::radians(deg), rotAxis) * model;
	lightPos = glm::rotate(glm::radians(deg), rotAxis) * glm::vec4(lightPos, 0);
}

// translate model to given point
void LightSource::moveOrigin(bool towards)
{
	glm::vec3 norm;
	if (towards)
	{
		// set normal vector pointing towards center
		norm = glm::normalize(glm::vec3(-lightPos.x, -lightPos.y, -lightPos.z));

		
	}
	else
	{
		// set normal vector pointing away from center
		norm = glm::normalize(glm::vec3(lightPos.x, lightPos.y, lightPos.z));
	}

	// translate light either towards or away from center
	model = glm::translate(norm * .05f) * model;

	norm.z = -norm.z;
	lightPos = glm::translate(glm::mat4(1), norm * .05f) * glm::vec4(lightPos, 1);
}

glm::vec3 LightSource::getLightPos()
{
	return lightPos;
}
