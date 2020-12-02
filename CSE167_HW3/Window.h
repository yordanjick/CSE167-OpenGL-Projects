#ifndef _WINDOW_H_
#define _WINDOW_H_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <memory>

#include "Object.h"
#include "Node.h"
#include "Transform.h"
#include "shader.h"
#include "Geometry.h"
#include "BoundingSphere.h"
#include "WireSphere.h"


enum class Movement {
	NONE,
	ROTATE_MODEL,
	ROTATE_LIGHT,
	ROTATE_BOTH,
	ROTATE_CAMERA
};

class Window
{
public:
	static GLint width;
	static GLint height;
	static GLdouble nearDist;
	static GLdouble farDist;
	static const char* windowTitle;
	static glm::mat4 projection;
	static glm::mat4 view;
	static glm::vec3 eye, center, up;
	static GLuint program, projectionLoc, viewLoc, modelLoc, objColorLoc, eyeLoc;

	static bool enableCulling;
	static bool enableCullingDebug;
	static GLuint objRendered;

	static GLdouble FOV;

	static GLuint mode;

	static GLfloat modelSize;
	
	static Transform* world;

	static glm::vec3 lastMousePoint, trackballPoint;

	static Movement moveType;
	static bool normalColor;

	static bool initializeProgram();
	static bool initializeObjects();
	static void cleanUp();
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);
	static void idleCallback();
	static void displayCallback(GLFWwindow*);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
	
private:
	static glm::vec3 trackballMapping(glm::vec3 point);
};

#endif
