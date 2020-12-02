#include "Window.h"

int Window::width;
int Window::height;

GLdouble Window::nearDist = 1;
GLdouble Window::farDist = 1000.0;

GLdouble Window::FOV = 60.0; // fov of the projection

const char* Window::windowTitle = "Rollercoaster";

// Objects to display
Transform* Window::world;
Skybox* Window::skybox;
BezierCurve* Window::testCurve;
Track* Window::track;

bool Window::normalColor;

bool Window::enableCulling = true;
bool Window::enableCullingDebug = false;

bool Window::firstPersonView = false;

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
GLuint Window::skyboxProgram; // skybox shader program id.

GLuint Window::projectionLoc; // Location of projection in shader.
GLuint Window::viewLoc; // Location of view in shader.
GLuint Window::eyeLoc; // Location of the viewer in shader.

bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	program = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
	skyboxProgram = LoadShaders("shaders/skybox_shader.vert", "shaders/skybox_shader.frag");

	// Check the shader program.
	if (!program || !skyboxProgram)
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

	// p3 - p2 = p4 - p3 ... p_n - p0 = p0 - p1

	std::vector<glm::vec3> points =
	{
		//p0 - p3, curve 1
		glm::vec3(1, -2, 10), // p0
		glm::vec3(0, -4, 11), //p1
		glm::vec3(-4, -6, 6), // p2
		glm::vec3(-6, -7, 4), // p3
		// p3 - p6, curve 2
		glm::vec3(-8, -8, 2), // p4
		glm::vec3(-10, -6, 0), // p5
		glm::vec3(-8, -2, -2), // p6
		// p6 - p9, curve 3
		glm::vec3(-6, 2, -4), // p7
		glm::vec3(-4, 3, -6), //p8
		glm::vec3(-3, 1, -7), // p9
		// p9 - p12, curve 4
		glm::vec3(-2, -1, -8), // p10
		glm::vec3(0, -2, -10), // p11
		glm::vec3(2, -5, -8), // p12
		// p12 - p15, curve 5
		glm::vec3(4, -8, -6), // p13
		glm::vec3(6, -7, -4), // p14
		glm::vec3(7, -6, -3), // p15
		// p15 - 18 curve 6
		glm::vec3(8, -5, -2), // p16
		glm::vec3(9, -1, -1), // p17
		glm::vec3(10, 1, 0), // p18
		// p18 - 21 curve 7
		glm::vec3(11, 3, 1), // p19
		glm::vec3(8, 5, 4), // p20
		glm::vec3(6, 2, 6), // p21
		// p 21, 22, 23, 0 curve 8
		glm::vec3(4, -1, 8), // p22
		glm::vec3(2, 0, 9), // p23
		glm::vec3(1, -2, 10), // p0

	};
	
	

	// scaling transform to make sphere bigger
	Transform* sphere_trans = new Transform(glm::translate(glm::vec3(1, -2, 10)));

	Geometry* sphere = new Geometry("sphere.obj");

	sphere_trans->addChild(sphere);

	track = new Track(points, sphere_trans, glm::vec3(1, -2, 10));

	world->addChild(track);
	
	std::vector<std::string> faces
	{
		"hw_entropic/entropic_lf.tga",
		"hw_entropic/entropic_rt.tga",
		"hw_entropic/entropic_up.tga",
		"hw_entropic/entropic_dn.tga",
		"hw_entropic/entropic_ft.tga",
		"hw_entropic/entropic_bk.tga"
	};

	skybox = new Skybox(1);
	skybox->loadCubemap(faces);

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

	if (firstPersonView)
	{
		// from position of car
		glm::vec3 car = track->getPosition();
		view = glm::lookAt(glm::normalize(car) * 0.1f + car, 
			glm::normalize(car) * 5.f + car, Window::up);

	}
	else
	{
		view = glm::lookAt(Window::eye, Window::center, Window::up);
	}

	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDepthMask(GL_FALSE);

	// use skybox shader
	glUseProgram(skyboxProgram);
	glm::mat4 skybox_view = glm::mat4(glm::mat3(view));
	
	// set values of the skybox shader
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "view"), 1, GL_FALSE, glm::value_ptr(skybox_view));

	skybox->draw();
	glDepthMask(GL_TRUE);

	glUseProgram(program);

	// Specify the values of the uniform variables we are going to use.
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniform3fv(eyeLoc, 1, glm::value_ptr(eye));

	// set num of objects rendered to 0. This is modified in the draw function of objects
	objRendered = 0;
	// Render the scenegraph, initially passing in identity matrix.
	world->draw(program, glm::mat4(1));

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
			// toggle first person POV
			firstPersonView = !firstPersonView;
			break;
		case GLFW_KEY_D:
			// toggle culling debug
			enableCullingDebug = !enableCullingDebug;
			break;
		case GLFW_KEY_X:
			// x y z axis movement controls
			if (mods == GLFW_MOD_SHIFT)
				track->moveControl('x', false);
			else
				track->moveControl('x', true);
			break;
		case GLFW_KEY_Y:
			// x y z axis movement controls
			if (mods == GLFW_MOD_SHIFT)
				track->moveControl('y', false);
			else
				track->moveControl('y', true);
			break;
		case GLFW_KEY_Z:
			// x y z axis movement controls
			if (mods == GLFW_MOD_SHIFT)
				track->moveControl('z', false);
			else
				track->moveControl('z', true);
			break;
		case GLFW_KEY_P:
			track->moving = !track->moving;
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
			track->nextLine(true);
			moveType = Movement::ROTATE_CAMERA;

			// if mouse is clicked, set prev point as given by trackball map
			trackballPoint = trackballMapping(lastMousePoint);
		}
		else if (action == GLFW_RELEASE)
		{
			moveType = Movement::NONE;
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		track->nextLine(false);
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

