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

#include "Transform.h"
#include "shader.h"
#include "Geometry.h"
#include "Model.h"
#include "LightSource.h"
#include "Mesh.h"
#include "DepthQuad.h"
#include "Skybox.h"

enum class PlayerControl {
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FOWARD,
	BACKWARD
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
	static glm::vec3 eye, front, up;
	static GLuint program, projectionLoc, viewLoc, modelLoc, objColorLoc, eyeLoc;
	static GLuint texProgram, depthProgram, depthDebug;
	static GLuint blurProgram, bloomProgram;
	static GLuint skyboxProgram;
	static GLdouble FOV;

	static GLuint depthFBO, depthmap;

	static GLuint hdrfbo;
	static GLuint colorBuffers[2];
	static GLuint rboDepth;
	static GLuint pingpongfbo[2];
	static GLuint pingpongBuffer[2];

	static GLuint mode;

	static double prevTime;

	static GLfloat modelSize;
	
	static Transform* world;

	static Skybox* skybox;

	static PlayerControl xControl, yControl, zControl;
	static float cameraPitch, cameraYaw;
	static bool normalColor;
	static bool toonShading;

	static DepthQuad* debugQuad;

	static std::vector<LightSource*> lights;

	static bool displayShadowmap, displayShadows, displayBloom;

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

	static void  APIENTRY MessageCallback(GLenum source, GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam);
};

#endif
