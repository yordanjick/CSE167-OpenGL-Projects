#include "Model.h"

Model::Model(std::string filePath, glm::mat4 model)
{
	loadModel(filePath);
	Model::model = model;
}

// loops and draws each mesh
void Model::draw(glm::mat4 C, unsigned int shaderProgram)
{
	for each (Mesh m in meshes)
	{
		m.draw(shaderProgram, model * C);

	}
}

void Model::update(glm::mat4 C)
{
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	// draw model with only triangles, and flip textures reversed on y-axis
	// where appropriate
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate |
		aiProcess_FlipUVs);

	// check for errors/model got imported correctly
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Assimp Error::" << importer.GetErrorString() << std::endl;
		return;
	}
	// get directory path of given file
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all meshes in the node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// process all meshes in children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 posVec;

		posVec.x = mesh->mVertices[i].x;
		posVec.y = mesh->mVertices[i].y;
		posVec.z = mesh->mVertices[i].z;
		vertex.Position = posVec;

		glm::vec3 normVec;
		normVec.x = mesh->mNormals[i].x;
		normVec.y = mesh->mNormals[i].y;
		normVec.z = mesh->mNormals[i].z;
		vertex.Normal = normVec;

		// if mesh contains texture coords
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 texVec;
			// use first set of texture coords
			texVec.x = mesh->mTextureCoords[0][i].x;
			texVec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = texVec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.f, 0.f);
		}

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		// add all indices per face/triangle of the mesh
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	if (mesh->mMaterialIndex >= 0)
	{
		// call helper function to get materials of the mesh, add to textures
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString s;
		mat->GetTexture(type, i, &s);
		bool alreadyLoaded = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), s.C_Str()) == 0)
			{
				// if texture is already loaded, use its existing path
				// instead of reloading it
				textures.push_back(textures_loaded[j]);
				alreadyLoaded = true;
				break;
			}
		}
		if (!alreadyLoaded)
		{
			Texture texture;
			texture.id = TextureFromFile(s.C_Str(), directory);
			texture.type = typeName;
			texture.path = s.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}

	return textures;
}


unsigned int Model::TextureFromFile(const char* path, const std::string& directory)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, numComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height,
		&numComponents, 0);
	if (data)
	{
		// load texture from image
		// find color format of image
		GLenum format;
		if (numComponents == 1)
			format = GL_RED;
		else if (numComponents == 3)
			format = GL_RGB;
		else if (numComponents == 4)
			format = GL_RGBA;

		// bind texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
			GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// generate mipmap and set parameters of the texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
	
}

