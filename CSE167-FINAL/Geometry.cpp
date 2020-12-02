#include "Geometry.h"

Geometry::Geometry(std::string filename, unsigned int shaderProgram)
{
	// store raw inputs of vertices and normals, and indices of vertices and normals
	std::vector<glm::vec3> input_vertices, input_normals, input_textures;
	std::vector<unsigned int> vertex_indices, texture_indices, normal_indices;

	Geometry::shaderProgram = shaderProgram;

	std::ifstream objFile(filename); // The obj file we are reading.

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

				// Process the vertex.
				input_vertices.push_back(point);
			}
			// If the line is about vertex normal (starting with a "vn").
			else if (label == "vn")
			{
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 normal;
				ss >> normal.x >> normal.y >> normal.z;

				// Process the point.
				input_normals.push_back(normal);
			}
			// If the line is about vertex texture (starting with a "vt").
			else if (label == "vt")
			{
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 texture;
				ss >> texture.x >> texture.y >> texture.z;

				// Process the point.
				input_textures.push_back(texture);
			}
			// If line is about a face (starting with "f")
			else if (label == "f")
			{
				std::string face;

				// add all 3 indices per line
				while (ss >> face)
				{
					std::stringstream indices_string;
					indices_string << face;
					std::string index;

					// get substring by delimiter of '/', giving us vertex index, texture index, then normal index
					std::getline(indices_string, index, '/');
					vertex_indices.push_back(std::stoi(index) - 1);
					std::getline(indices_string, index, '/');
					
					// check if there was something actually extracted
					
					// texture_indices.push_back(std::stoi(index) - 1);

					std::getline(indices_string, index, '/');
					normal_indices.push_back(std::stoi(index) - 1);

				}
			}

		}
	}
	else
	{
		std::cerr << "Can't open the file " << filename << std::endl;
	}

	std::cerr << "file " << filename << " read" << std::endl;

	objFile.close();

	// sort and align indices of the vertices and normals from the .obj file
	for (unsigned int i = 0; i < vertex_indices.size(); i++)
	{
		vertices.push_back(input_vertices[vertex_indices[i]]);
		normals.push_back(input_normals[normal_indices[i]]);
		indices.push_back(i);
	}

	std::cerr << vertices.size() << " " << normals.size() << " " << indices.size() << std::endl;

	// Set the model matrix to an identity matrix. 
	model = glm::mat4(1);
	// Set the color. 
	color = glm::vec3(1, .5, 1);

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
		vertices.data(), GL_STATIC_DRAW);
	// Enable vertex attribute 0. 
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// bind VNBO, 2nd vbo
	glBindBuffer(GL_ARRAY_BUFFER, vnbo);

	// pass in data
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(),
		normals.data(), GL_STATIC_DRAW);

	// Enable vertex attribute 1 (normal array buffer). 
	// We will be able to access points through it.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind EBO as an element array buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);

}

Geometry::~Geometry()
{
	// Delete the VBO, VNBO, EBO, and the VAO.
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vnbo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Geometry::draw(glm::mat4 C)
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
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// unbind vao
	glBindVertexArray(0);


}

void Geometry::update(glm::mat4 C)
{
}

void Geometry::setColor(glm::vec3 c)
{
	Geometry::color = c;
}
