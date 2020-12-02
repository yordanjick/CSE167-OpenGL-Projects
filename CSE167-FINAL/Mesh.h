#ifndef _MESH_H_
#define _MESH_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <iostream>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	static unsigned int depthShader;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector <Vertex> vertices, std::vector<unsigned int> indices,
		std::vector<Texture> textures);
	void draw(GLuint textureProgram, glm::mat4 C);
private:
	unsigned int vao, vbo, ebo;

	void setupMesh();
};
#endif
