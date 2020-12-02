#include "BoundingSphere.h"
#include "Window.h"

bool BoundingSphere::render = false;

BoundingSphere::BoundingSphere(Geometry* sphere, GLfloat radius)
{
	BoundingSphere::sphere = sphere;
	BoundingSphere::radius = 2.2f; // magic number size of bounding sphere????
}

// copied from Geometry, but draw as a wireframe instead
void BoundingSphere::draw(GLuint shaderProgram, glm::mat4 C)
{
	// only render wireframe bounding sphere if toggled on
	if (render)
		sphere->draw(shaderProgram, C);
}

void BoundingSphere::update(glm::mat4 C)
{
	// set world coord according to translation passed to it
	// worldCoord = glm::vec3(glm::translate(C, glm::vec3(0)) * glm::vec4(1));
	// std::cerr << worldCoord.x << " " << worldCoord.y << " " << worldCoord.z << std::endl;
}

bool BoundingSphere::checkInView(glm::mat4 C)
{
	if (!Window::enableCullingDebug)
	{
		curFOV = (float)Window::FOV;
	}

	glm::vec3 worldCoord = C[3];

	// std::cerr << worldCoord.x << " " << worldCoord.y << " " << worldCoord.z << std::endl;

	GLfloat hNear = 2 * glm::tan(glm::radians(curFOV) / 2) * (float)Window::nearDist; // height of near plane
	GLfloat wNear = hNear * (float)Window::width / (float)Window::height; // width of near plane (mult height by aspect ratio)

	GLfloat hFar = 2 * glm::tan(glm::radians(curFOV) / 2) * (float)Window::farDist; // height of far plane
	GLfloat wFar = hFar * (float)Window::width / (float)Window::height; // width of far plane (mult height by aspect ratio)

	glm::vec3 camDir = glm::normalize(Window::center - Window::eye); // the direction the camera is pointing to
	glm::vec3 nearNorm = camDir;
	nearNorm.z *= -1;
	glm::vec3 camLoc = Window::eye;

	// normal of near and far plane is just camera direction
	glm::vec3 nearPt = Window::eye + camDir * (float)Window::nearDist; // pt on near plane
	glm::vec3 farPt = Window::eye + camDir * (float)Window::farDist; // pt of far plane

	glm::vec3 up = Window::up;
	glm::vec3 right = glm::cross(up, camDir); // vec pointing to the right of the camera
	glm::vec3 down(0, -1, 0); // vec pointing down relative to camera
	glm::vec3 left = glm::cross(down, camDir); // vec pointing left relative to camera

	glm::vec3 aRight = (nearPt + right * wNear / 2.0f) - camLoc;
	glm::vec3 rightNorm = glm::cross(up, glm::normalize(aRight)); // normal of the right plane (pointing out)

	glm::vec3 aLeft = (nearPt + left * wNear / 2.0f) - camLoc;
	glm::vec3 leftNorm = glm::cross(down, glm::normalize(aLeft)); // normal of the left plane

	glm::vec3 aUp = (nearPt + up * hNear / 2.0f) - camLoc;
	glm::vec3 upNorm = glm::cross(left, glm::normalize(aUp)); // normal of the up plane

	glm::vec3 aDown = (nearPt + down * hNear / 2.0f) - camLoc;
	glm::vec3 downNorm = glm::cross(right, glm::normalize(aDown)); // normal of the down plane

	// vector of planes represented by a tuple of (point, normal)
	std::vector<std::tuple<glm::vec3, glm::vec3>> planes;
	planes.push_back(std::make_tuple(nearPt, nearNorm));
	planes.push_back(std::make_tuple(farPt, camDir));
	planes.push_back(std::make_tuple(camLoc, rightNorm));
	planes.push_back(std::make_tuple(camLoc, leftNorm));
	planes.push_back(std::make_tuple(camLoc, upNorm));
	planes.push_back(std::make_tuple(camLoc, downNorm));

	// how many planes the object is within
	int within = 0;

	for each (std::tuple<glm::vec3, glm::vec3> p in planes)
	{
		GLfloat dist = glm::dot(worldCoord, std::get<1>(p)) - glm::dot(std::get<0>(p), std::get<1>(p));
		// std::cerr << dist << std::endl;
		// if obj is within in intersects bounds
		if (dist < radius)
		{
			within++;
		}
	}
	// std::cerr << within << std::endl;
	if (within >= 6)
	{
		Window::objRendered += 1;
		return true;
		
	}


	return false;
}
