#include "Mesh.h"

unsigned int Mesh::depthShader;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;
	setupMesh();
}

void Mesh::draw(GLuint textureProgram, glm::mat4 C)
{
	glUseProgram(textureProgram);

	// std::cerr << textures.size() << std::endl;
	if (textureProgram != depthShader)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			// activate proper texture before binding
			glActiveTexture(GL_TEXTURE0 + i);
			std::string number;
			std::string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			// given textures in shader are organized as texture_diffuse#
			// and texture_specular# for some # of textures
			glUniform1i(glGetUniformLocation(textureProgram,
				(name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);

		}
	}
	// set parameters of the model for the vertex shader per geometry 
	glUniformMatrix4fv(glGetUniformLocation(textureProgram, "model"), 1, GL_FALSE, 
		glm::value_ptr(C * glm::mat4(1)));

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &vao);

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	// element buffer objects (EBO)
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
		&vertices[0], GL_STATIC_DRAW);

	// Enable vertex attribute 0. 
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Enable vertex attribute 1 (normal array buffer). 
	// We will be able to access points through it.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));


	// Bind EBO as an element array buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}
