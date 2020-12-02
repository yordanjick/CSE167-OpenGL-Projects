#include "Transform.h"
#include "Window.h"

Transform::Transform(glm::mat4 M)
{
	Transform::M = M;
}

Transform::~Transform()
{
	children.empty();
}

void Transform::draw(glm::mat4 C, unsigned int shaderProgram)
{
	// only draw if the bounding box is in camera view
	/* if (bsphere != nullptr && !bsphere->checkInView(C * M))
		return;
	*/

	// modify transform by this object's transform matrix
	glm::mat4 tf = C * M;

	// for each children, draw with the new transform matrix
	for each  (Node* child in children)
	{
		child->draw(tf, shaderProgram);
	}
}

void Transform::update(glm::mat4 C)
{
	
	for each (Node* child in children)
	{
		child->update(C * M);
	
	}
}


// adds new child to list of children
void Transform::addChild(Node* child)
{
	children.push_back(child);
}


 /*void Transform::setBound(BoundingSphere* s)
{
	bsphere = s;
}*/

void Transform::setTransform(glm::mat4 mat)
{
	M = mat;
}
