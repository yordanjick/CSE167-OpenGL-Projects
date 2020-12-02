#ifndef _MODEL_H_
#define _MODEL_H_

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

#include "stb_image.h"

#include "Mesh.h"
#include "Node.h"

class Model : public Node
{
public:
	Model(std::string filePath, glm::mat4 model);

	void draw(glm::mat4 , unsigned int shaderProgram);
	void update(glm::mat4 C);

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	glm::mat4 model;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
		std::string typeName);

	unsigned int TextureFromFile(const char* path, const std::string& directory);

};
#endif
