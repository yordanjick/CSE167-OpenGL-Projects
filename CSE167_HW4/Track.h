#ifndef _TRACK_H_
#define _TRACK_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <iostream>


#include "Node.h"
#include "Geometry.h"
#include "BezierCurve.h"
#include "Line.h"

class Track : public Node
{
protected:
	
	std::vector<BezierCurve*> curves;
	std::vector<glm::vec3> points;
	std::vector<Line*> handles;

	int currentPoint;

	int currentTrack;

	int curT;
	float velocity; // velocity of car

	glm::vec3 position; // position of car
	Transform* sphere; // car

	double prevTime;

public:

	bool moving;

	Track(std::vector<glm::vec3> points, Transform* sphere, glm::vec3 start);
	~Track();

	void nextLine(bool left);
	void moveControl(char axis, bool positive);
	void draw(GLuint shaderProgram, glm::mat4 C);
	void update(glm::mat4 C);
	glm::vec3 getPosition();

};

#endif
