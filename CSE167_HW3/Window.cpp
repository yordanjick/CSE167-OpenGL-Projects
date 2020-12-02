#include "Window.h"

int Window::width;
int Window::height;

GLdouble Window::nearDist = 1;
GLdouble Window::farDist = 1000.0;

GLdouble Window::FOV = 60.0; // fov of the projection

const char* Window::windowTitle = "Robot Army";

// Objects to display
Transform* Window::world;

bool Window::normalColor;

bool Window::enableCulling = true;
bool Window::enableCullingDebug = false;

GLuint Window::objRendered = 0;

GLfloat Window::modelSize;

Movement Window::moveType;
glm::vec3 Window::lastMousePoint;
glm::vec3 Window::trackballPoint;

GLuint Window::mode = 1; // mouse mode for rotation

glm::mat4 Window::projection; // Projection matrix.

glm::vec3 Window::eye(0, 0, 20); // Camera position.
glm::vec3 Window::center(0, 0, 0); // The point we are looking at.
glm::vec3 Window::up(0, 1, 0); // The up direction of the camera.

// View matrix, defined by eye, center and up.
glm::mat4 Window::view = glm::lookAt(Window::eye, Window::center, Window::up);

GLuint Window::program; // The shader program id.

GLuint Window::projectionLoc; // Location of projection in shader.
GLuint Window::viewLoc; // Location of view in shader.
GLuint Window::eyeLoc; // Location of the viewer in shader.

bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	program = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	// Check the shader program.
	if (!program)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	// Activate the shader program.
	glUseProgram(program);

	return true;
}

bool Window::initializeObjects()
{
	normalColor = false;

	modelSize = 1.0f;

	// initialize models and materials of the objects
	projectionLoc = glGetUniformLocation(program, "projection");
	viewLoc = glGetUniformLocation(program, "view");
	eyeLoc = glGetUniformLocation(program, "eye");

	// initial world transform with identity matrix
	world = new Transform(glm::mat4(1));

	glm::mat4 bodyMat = glm::rotate(glm::radians(15.0f), glm::vec3(0, 1, -1));
	bodyMat = glm::translate(glm::vec3(0, -20.0f, 0.0f)) * bodyMat;
	Transform *body = new Transform(bodyMat);
	glm::mat4 headMat = glm::translate(glm::vec3(0, 1.2f, 0));
	glm::mat4 armLMat = glm::translate(glm::vec3(1.25f, 0, 0));
	glm::mat4 armRMat = glm::translate(glm::vec3(-1.25f, 0, 0));
	glm::mat4 legLMat = glm::translate(glm::vec3(0.5f, -1.8f, 0));
	glm::mat4 legRMat = glm::translate(glm::vec3(-0.5f, -1.8f, 0));
	glm::mat4 eyeLMat = glm::translate(glm::vec3(0.3f, 1.5f, 1));
	glm::mat4 eyeRMat = glm::translate(glm::vec3(-0.3f, 1.5f, 1));

	float radius = 2.2f;

	Geometry* boundGeo = new WireSphere("sphere.obj", radius);

	BoundingSphere* boundSphere = new BoundingSphere(boundGeo, radius);


	// create many robots by making many transforms
	for (int i = -5; i < 5; i++)
	{
		for (int j = -20; j < 0; j++)
		{
			Transform* robot = new Transform(glm::translate(glm::vec3(i * 5, 0, j * 5)));
			robot->addChild(body);
			world->addChild(robot);
		}
	}


	// building robot
	Transform* head = new Transform(headMat);
	Transform* armL = new Transform(armLMat);
	Transform* armR = new Transform(armRMat);
	Transform* legL = new Transform(legLMat);
	Transform* legR = new Transform(legRMat);
	Transform* eyeL = new Transform(eyeLMat);
	Transform* eyeR = new Transform(eyeRMat);
	Geometry* bodyGeo = new Geometry("body_s.obj");
	Geometry* headGeo = new Geometry("head_s.obj");
	Geometry* limbGeo = new Geometry("limb_s.obj");
	Geometry* eyeGeo = new Geometry("eyeball_s.obj");
	
	eyeGeo->setColor(glm::vec3(1));
	boundGeo->setColor(glm::vec3(1, 0, 0));

	body->addChild(bodyGeo);
	body->addChild(head);
	body->addChild(armL);
	body->addChild(armR);
	body->addChild(legL);
	body->addChild(legR);
	body->addChild(eyeL);
	body->addChild(eyeR);

	body->addChild(boundSphere);
	body->setBound(boundSphere);

	armL->setPart(BodyPart::ARM_L);
	armR->setPart(BodyPart::ARM_R);
	legL->setPart(BodyPart::LEG_L);
	legR->setPart(BodyPart::LEG_R);

	head->addChild(headGeo);
	armL->addChild(limbGeo);
	armR->addChild(limbGeo);
	legL->addChild(limbGeo);
	legR->addChild(limbGeo);
	eyeL->addChild(eyeGeo);
	eyeR->addChild(eyeGeo);

	
	
	moveType = Movement::NONE;
	lastMousePoint = glm::vec3(0);

	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete world;

	// Delete the shader program.
	glDeleteProgram(program);
}

GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	Window::projection = glm::perspective(glm::radians(FOV), 
		double(width) / (double)height, nearDist, farDist);
}

void Window::idleCallback()
{
	// Perform any updates as necessary. 
	world->update(glm::mat4(1));

}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Specify the values of the uniform variables we are going to use.
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniform3fv(eyeLoc, 1, glm::value_ptr(eye));
	
	glUniform1i(glGetUniformLocation(program, "normalColor"), normalColor);

	// set num of objects rendered to 0. This is modified in the draw function of objects
	objRendered = 0;
	// Render the scenegraph, initially passing in identity matrix.
	world->draw(program, glm::mat4(1));

	// Set window title
	std::string newTitle = "Number of robots rendered: " + std::to_string(objRendered);

	glfwSetWindowTitle(window, newTitle.c_str());

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*
	 * TODO: Modify below to add your key callbacks.
	 */
	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;
		case GLFW_KEY_N:
			// switch on or off normal color mode
			normalColor = !normalColor;
			break;
		case GLFW_KEY_B:
			// toggle rendering of bounding spheres
			BoundingSphere::render = !BoundingSphere::render;
			break;
		case GLFW_KEY_C:
			// toggle frustrum culling
			enableCulling = !enableCulling;
			break;
		case GLFW_KEY_D:
			// toggle culling debug
			enableCullingDebug = !enableCullingDebug;
			break;
		default:
			break;
		}
	}
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			moveType = Movement::ROTATE_CAMERA;

			// if mouse is clicked, set prev point as given by trackball map
			trackballPoint = trackballMapping(lastMousePoint);
		}
		else if (action == GLFW_RELEASE)
		{
			moveType = Movement::NONE;
		}
	}
}

void Window::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	// if scrolling up, scale up, zoom in
	if (yoffset > 0)
	{
		// add movement to camera
		FOV *= 1.05;
		projection = glm::perspective(glm::radians(FOV),
			double(width) / (double)height, nearDist, farDist);
	
	}
	// if scrolling down, scale down, zoom out
	else if (yoffset < 0)
	{
		FOV *= .95;
		projection = glm::perspective(glm::radians(FOV),
			double(width) / (double)height, nearDist, farDist);
		// add movement to camera
	}
}

void Window::mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	// constantly store last position of mouse point
	lastMousePoint.x = (float)xpos;
	lastMousePoint.y = (float)ypos;

	if (moveType != Movement::NONE)
	{
		glm::vec3 direction;
		// float rotAngle;
		glm::vec3 curPoint = trackballMapping(lastMousePoint);

		direction = curPoint - trackballPoint; 
		float vel = glm::length(direction);

		// only move if significant movement detected
		if (vel > .0001f)
		{
			// rotate about axis perpendicular to the 2
			// movement points; the cross product
			// glm::vec3 rotAxis;
			// rotAxis = glm::cross(curPoint, trackballPoint);

			// multiply rotation angle by some constant to scale speed
			// rotAngle = -1 * glm::acos(glm::dot(curPoint, trackballPoint) / (glm::length(curPoint) * glm::length(trackballPoint)));

			if (moveType == Movement::ROTATE_CAMERA)
			{
				center = center - direction;
				
				view = glm::lookAt(eye, center, up);
			}
			
		}
	}
}

glm::vec3 Window::trackballMapping(glm::vec3 point)
{	
	glm::vec3 v;
	double d;
	// sets the vector v as coming from the center of the screen
	// on a 2D plane
	v.x = (2.0f * point.x - width) / width;
	v.y = (height - 2.0f * point.y) / height;
	v.z = 0;
	// sets the z of the vector according to its length
	// from 0 to 1: 0 means further away, 1 means direct center/most depth
	d = glm::length(v);
	d = (d < 1) ? d : 1;
	v.z = sqrtf(1.001 - d * d);
	// normalize vector to unit vector
	v = glm::normalize(v);
	return v;
}

