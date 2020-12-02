#include "Track.h"


Track::Track(std::vector<glm::vec3> points, Transform* sphere, glm::vec3 start)
{
	position = start;

	moving = true;

	currentPoint = 0;
	currentTrack = 0;
	curT = 0;
	velocity = 5.f;
	prevTime = glfwGetTime();

	Track::sphere = sphere;

	Track::points = points;
	// initialize 8 bezier curves from points
	// assumes last point is same as first point
	for (unsigned int i = 0; i < 8; i++)
	{
		// get white color
		BezierCurve* c = new BezierCurve(points[i * 3], points[i * 3 + 1], points[i * 3 + 2], points[i * 3 + 3], glm::vec3(1, 1, 1));
		curves.push_back(c);
	}

	for (unsigned int i = 0; i < 7; i++)
	{
		Line* l = new Line(points[i * 3 + 2], points[i * 3 + 4]);
		handles.push_back(l);
	}

	Line* l = new Line(points[points.size() - 2], points[1]);
	handles.push_back(l);

	
}

Track::~Track()
{
}

void Track::nextLine(bool left)
{
	int curCurve = int(currentPoint / 3);

	switch (currentPoint % 3)
	{
	case 0:
		curves[curCurve]->c0Color = glm::vec3(1, 0, 0);
		break;
	case 1:
		curves[curCurve]->c1Color = glm::vec3(0, 1, 0);
		break;
	case 2:
		curves[curCurve]->c2Color = glm::vec3(0, 1, 0);
		break;
	default:
		break;
	}
	// change currently selected line
	if (left)
	{
		currentPoint--;
	}
	else
	{
		currentPoint++;
	}

	if (currentPoint >= (int)points.size() - 1)
	{
		currentPoint = 0;
	}
	if (currentPoint < 0)
	{
		currentPoint = points.size() - 2;
	}

	curCurve = int(currentPoint / 3);

	switch (currentPoint % 3)
	{
		case 0:
			curves[curCurve]->c0Color = glm::vec3(1, 1, 0);
			break;
		case 1:
			curves[curCurve]->c1Color = glm::vec3(1, 1, 0);
			break;
		case 2:
			curves[curCurve]->c2Color = glm::vec3(1, 1, 0);
			break;
		default:
			break;
	}

}

void Track::moveControl(char axis, bool positive)
{

	int curCurve = (int)(currentPoint / 3);
	int curPt = currentPoint % 3;

	float delta = positive ? 0.2f : -0.2f;

	glm::vec3 move;

	// set movement vector according to axis of movement
	switch (axis)
	{
		case 'x':
			move = glm::vec3(delta, 0, 0);
			break;
		case 'y':
			move = glm::vec3(0, delta, 0);
			break;
		case 'z':
			move = glm::vec3(0, 0, delta);
			break;
		default:
			break;
	}

	int prevCurve = curCurve > 0 ? curCurve - 1 : curves.size() - 1;
	int nextCurve = curCurve < curves.size() - 1 ? curCurve + 1 : 0;
	switch (curPt)
	{
		// if moving control point: move previous curve's p2, this curve's p1
		case 0:
			curves[curCurve]->p0 += move;
			curves[prevCurve]->p2 += move;
			curves[prevCurve]->p3 += move;
			curves[curCurve]->p1 += move;
			handles[prevCurve]->p0 += move;
			handles[prevCurve]->p1 += move;
			break;
		// if moving p1: move previous curve's p2 in opposite direction, p0 remains unchanged
		case 1:
			curves[curCurve]->p1 += move;
			curves[prevCurve]->p2 -= move;
			handles[prevCurve]->p0 -= move;
			handles[prevCurve]->p1 += move;
			break;
		// if moving p2: move next curve's p1 in opposite direction
		case 2:
			curves[curCurve]->p2 += move;
			curves[nextCurve]->p1 -= move;
			handles[curCurve]->p0 += move;
			handles[curCurve]->p1 -= move;
			break;
		default:
			break;
	}

	curves[prevCurve]->recalcPoints();
	curves[curCurve]->recalcPoints();
	curves[nextCurve]->recalcPoints();

}

void Track::draw(GLuint shaderProgram, glm::mat4 C)
{
	// something with drawing handles

	for each (BezierCurve* c in curves)
	{
		c->draw(shaderProgram, C);
	}

	for each (Line * l in handles)
	{
		l->draw(shaderProgram, C);
	}
	sphere->draw(shaderProgram, C);
}

void Track::update(glm::mat4 C)
{
	for each (BezierCurve * c in curves)
	{
		c->update(C);
	}

	for each (Line * l in handles)
	{
		l->update(C);
	}

	if (!moving)
	{
		prevTime = glfwGetTime();
		return;
	}
	
	// movement of the rollercoaster

	double delta_t = glfwGetTime() - prevTime;

	// distance cart travels
	float dist = velocity * (float)(delta_t);
	
	glm::vec3 end = curves[currentTrack]->getPoint(curT);
	
	// if moves further than current line segment, advance to next segment. 
	while (glm::abs(glm::length(end) - glm::length(position)) < dist)
	{
		curT++;

		end = curves[currentTrack]->getPoint(curT);

		if (curT >= 150)
		{
			curT = 0;
			currentTrack = currentTrack < curves.size() - 1 ? currentTrack + 1 : 0;
		}
	}
	// changing curves
	if (curT >= 150)
	{
		curT = 0;
		currentTrack = currentTrack < curves.size() - 1 ? currentTrack + 1 : 0;
	}

	// travel distance over length to end of line segment 
	position += (glm::normalize(end - position) * dist);

	sphere->setTransform(glm::translate(position));

	prevTime = glfwGetTime();
	
}

glm::vec3 Track::getPosition()
{
	return position;
}
