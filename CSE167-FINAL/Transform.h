#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <list>
#include <glm/gtx/euler_angles.hpp>

#include "Node.h"
#include <iostream>

class Transform : public Node
{
private:
	std::list<Node*> children;
	glm::mat4 M;

	// BoundingSphere* bsphere; 

public:

	Transform(glm::mat4 M);
	~Transform();

	void draw(glm::mat4 C, unsigned int shaderProgram);

	void update(glm::mat4 C);

	void addChild(Node* child);

	// void setBound(BoundingSphere* s);

	void setTransform(glm::mat4 mat);
};

#endif
