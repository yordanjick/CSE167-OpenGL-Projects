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
#include "Cube.h"
#include "Model3D.h"
#include "LightSource.h"
#include "shader.h"


enum class Movement {
	NONE,
	ROTATE_MODEL,
	ROTATE_LIGHT,
	ROTATE_BOTH
};

class Window
{
public:
	static int width;
	static int height;
	static const char* windowTitle;
	static Cube* cube;
	static Object * currentObj;
	static glm::mat4 projection;
	static glm::mat4 view;
	static glm::vec3 eye, center, up;
	static GLuint program, projectionLoc, viewLoc, modelLoc, objColorLoc, lightColorLoc, lightLoc, eyeLoc,
		ambientLoc, diffuseLoc, specularLoc, shininessLoc;

	static GLuint mode;

	static GLfloat modelSize;
	static Model3D* bunny;
	static Model3D* dragon;
	static Model3D* bear;

	static LightSource* lightSource;
	static glm::vec3 lightPos;

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
