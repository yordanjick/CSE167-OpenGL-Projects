#include "Transform.h"
#include "Window.h"

Transform::Transform(glm::mat4 M)
{
	Transform::M = M;
	totRot = 0;
	rotCW = false;
	part = BodyPart::NONE;
	bsphere = nullptr;
}

Transform::~Transform()
{
	children.empty();
}

void Transform::draw(GLuint shaderProgram, glm::mat4 C)
{
	// only draw if the bounding box is in camera view
	if (bsphere != nullptr && Window::enableCulling && !bsphere->checkInView(C * M))
		return;
	else if (bsphere != nullptr && !Window::enableCulling)
	{
		Window::objRendered += 1;
	}

	// modify transform by this object's transform matrix
	glm::mat4 tf = C * M;

	// for each children, draw with the new transform matrix
	for each  (Node* child in children)
	{
		child->draw(shaderProgram, tf);
	}
}

void Transform::update(glm::mat4 C)
{
	float rotSpd = 0.01f;
	float rot = 0; // how much to rotate the obj by
	switch (part)
	{
		// rotate positive first, then negative
		case BodyPart::ARM_L:
		case BodyPart::LEG_R:
			rot = (rotCW) ? rotSpd : -rotSpd;
			// reverse rotation if passes certain angle threshold
			break;
		// rotate negative first, then positive
		case BodyPart::ARM_R:
		case BodyPart::LEG_L:
			rot = (rotCW) ? -rotSpd : rotSpd;
			// reverse rotation if passes certain angle threshold
			break;
		default:
			break;
	}

	if (part != BodyPart::NONE)
	{
		totRot += rot;
		if (glm::abs(totRot) >= 45)
			rotCW = !rotCW;
		M = glm::rotate(glm::radians(rot), glm::vec3(1, 0, 0)) * M;
	}
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

void Transform::setPart(BodyPart p)
{
	part = p;
}

void Transform::setBound(BoundingSphere* s)
{
	bsphere = s;
}
