#include "Window.h"

int Window::width;
int Window::height;

const char* Window::windowTitle = "GLFW 3D Models";

// Objects to display.
Cube * Window::cube;

Model3D* Window::bunny;
Model3D* Window::dragon;
Model3D* Window::bear;
LightSource* Window::lightSource;

glm::vec3 Window::lightPos;

bool Window::normalColor;

GLfloat Window::modelSize;

Movement Window::moveType;
glm::vec3 Window::lastMousePoint;
glm::vec3 Window::trackballPoint;

GLuint Window::mode = 1; // mouse mode for rotation

// The object currently displaying.
Object * Window::currentObj; 

glm::mat4 Window::projection; // Projection matrix.

glm::vec3 Window::eye(0, 0, 20); // Camera position.
glm::vec3 Window::center(0, 0, 0); // The point we are looking at.
glm::vec3 Window::up(0, 1, 0); // The up direction of the camera.

// View matrix, defined by eye, center and up.
glm::mat4 Window::view = glm::lookAt(Window::eye, Window::center, Window::up);

GLuint Window::program; // The shader program id.

GLuint Window::projectionLoc; // Location of projection in shader.
GLuint Window::viewLoc; // Location of view in shader.
GLuint Window::modelLoc; // Location of model in shader.
GLuint Window::objColorLoc; // Location of object color in shader.
GLuint Window::lightColorLoc; // Location of light color in shader.
GLuint Window::lightLoc; // Location of light source in shader.
GLuint Window::eyeLoc; // Location of the viewer in shader.
GLuint Window::ambientLoc; // Location of the ambient lighting value in shader.
GLuint Window::diffuseLoc; // Location of the diffuse lighting value in shader.
GLuint Window::specularLoc; // Location of the specular lighting value in shader.
GLuint Window::shininessLoc; // Location of the shininess value in shader.

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
	// Get the locations of uniform variables.
	projectionLoc = glGetUniformLocation(program, "projection");
	viewLoc = glGetUniformLocation(program, "view");
	modelLoc = glGetUniformLocation(program, "model");
	lightColorLoc = glGetUniformLocation(program, "lightColor");
	objColorLoc = glGetUniformLocation(program, "objColor");
	lightLoc = glGetUniformLocation(program, "lightPos");
	eyeLoc = glGetUniformLocation(program, "viewPos");
	ambientLoc = glGetUniformLocation(program, "material.ambient");
	diffuseLoc = glGetUniformLocation(program, "material.diffuse");
	specularLoc = glGetUniformLocation(program, "material.specular");
	shininessLoc = glGetUniformLocation(program, "material.shininess");


	return true;
}

bool Window::initializeObjects()
{
	normalColor = false;
	// Create a cube of size 5.
	cube = new Cube(5.0f);

	modelSize = 1.0f;

	lightPos = glm::vec3(7, 7, 0);

	glm::vec3 lightColor = glm::vec3(1, .1, .1);
	// initialize models and materials of the objects
	// bunny has little diffuse and high specular "shininess"
	bunny = new Model3D("bunny.obj", glm::vec3(.1f), glm::vec3(.01f, .01f, .01f), glm::vec3(.8f, .6f, .7f), 32);
	// dragon has high diffuse and low specular
	dragon = new Model3D("dragon.obj", glm::vec3(.1f), glm::vec3(.89f, .65f, .75f), glm::vec3(.01f, .01f, .01f), 16);
	// bear has both diffuse and specular
	bear = new Model3D("bear.obj", glm::vec3(.1f), glm::vec3(.72f, .89f, .96f), glm::vec3(.5f, .5f, .5f), 32);
	lightSource = new LightSource("sphere.obj", lightPos, lightColor);


	moveType = Movement::NONE;
	lastMousePoint = glm::vec3(0);

	currentObj = bunny;

	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete cube;

	delete bunny;
	delete dragon;
	delete bear;

	delete lightSource;

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
	Window::projection = glm::perspective(glm::radians(60.0), 
		double(width) / (double)height, 1.0, 1000.0);
}

void Window::idleCallback()
{
	// Perform any updates as necessary. 
	currentObj->update();
	lightSource->update();

}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Specify the values of the uniform variables we are going to use.
	glm::mat4 model = currentObj->getModel();
	glm::vec3 color = currentObj->getColor();
	glm::vec3 lightColor = lightSource->getColor();

	glm::vec3 ambient = ((Model3D*)currentObj)->getAmbient();
	glm::vec3 diffuse = ((Model3D*)currentObj)->getDiffuse();
	glm::vec3 specular = ((Model3D*)currentObj)->getSpecular();
	float shininess = ((Model3D*)currentObj)->getShininess();

	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(objColorLoc, 1, glm::value_ptr(color));
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	glUniform3fv(lightLoc, 1, glm::value_ptr(lightSource->getLightPos()));
	glUniform3fv(eyeLoc, 1, glm::value_ptr(eye));
	glUniform3fv(ambientLoc, 1, glm::value_ptr(ambient));
	glUniform3fv(diffuseLoc, 1, glm::value_ptr(diffuse));
	glUniform3fv(specularLoc, 1, glm::value_ptr(specular));
	glUniform1f(shininessLoc, shininess);

	glUniform1i(glGetUniformLocation(program, "normalColor"), normalColor);
	glUniform1f(glGetUniformLocation(program, "lightConst"), 1); // constant light of 1
	glUniform1f(glGetUniformLocation(program, "lightLinear"), .005f); // linear falloff rate of the point light

	// Render the object.
	currentObj->draw();
	

	// change lighting values for the light source
	ambient = glm::vec3(1, 1, 1);
	diffuse = glm::vec3(0);
	specular = glm::vec3(0);

	model = lightSource->getModel();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(ambientLoc, 1, glm::value_ptr(ambient));
	glUniform3fv(diffuseLoc, 1, glm::value_ptr(diffuse));
	glUniform3fv(specularLoc, 1, glm::value_ptr(specular));

	lightSource->draw();

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
		case GLFW_KEY_F1:
			// Set currentObj to bunnyPoints
			currentObj = bunny;
			break;
		case GLFW_KEY_F2:
			// Set currentObj to dragonPoints
			currentObj = dragon;
			break;
		case GLFW_KEY_F3:
			// Set currentObj to bearPoints
			currentObj = bear;
			break;
		case GLFW_KEY_N:
			// switch on or off normal color mode
			normalColor = !normalColor;
			break;
		case GLFW_KEY_1:
			// set current mode to 1: rotate and scale model
			mode = 1;
			break;
		case GLFW_KEY_2:
			// set current mode to 2: rotate and scale light
			mode = 2;
			break;
		case GLFW_KEY_3:
			// set current mode to 1: rotate and scale model and light
			mode = 3;
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
			// change movetype depending on the mode
			switch (mode)
			{
				case 1:
					moveType = Movement::ROTATE_MODEL;
					break;
				case 2:
					moveType = Movement::ROTATE_LIGHT;
					break;
				case 3:
					moveType = Movement::ROTATE_BOTH;
					break;
				default:
					break;
			}
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
	// if scrolling up, scale up, move further away
	if (yoffset > 0)
	{
		// scale model, light source, or both depending on mode
		if (mode == 1)
		{
			((Model3D*)currentObj)->resize(1);
		}
		else if (mode == 2)
		{
			lightSource->moveOrigin(0);
		}
		else if (mode == 3)
		{
			((Model3D*)currentObj)->resize(1);
			lightSource->moveOrigin(0);
		}
		
	}
	// if scrolling down, scale down, move light closer
	else if (yoffset < 0)
	{
		// scale model, light source, or both depending on mode
		if (mode == 1)
		{
			((Model3D*)currentObj)->resize(0);
		}
		else if (mode == 2)
		{
			lightSource->moveOrigin(1);
		}
		else if (mode == 3)
		{
			((Model3D*)currentObj)->resize(0);
			lightSource->moveOrigin(1);
		}
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
		float rotAngle;
		glm::vec3 curPoint = trackballMapping(lastMousePoint);

		direction = curPoint - trackballPoint; 
		float vel = glm::length(direction);

		// only move if significant movement detected
		if (vel > .0001f)
		{
			// rotate about axis perpendicular to the 2
			// movement points; the cross product
			glm::vec3 rotAxis;
			rotAxis = glm::cross(curPoint, trackballPoint);

			// multiply rotation angle by some constant to scale speed
			rotAngle = -1 * glm::acos(glm::dot(curPoint, trackballPoint) / (glm::length(curPoint) * glm::length(trackballPoint)));

			// spin model, light source, or both depending on mode
			if (moveType == Movement::ROTATE_MODEL)
			{
				((Model3D*)currentObj)->spin(rotAngle, rotAxis);
			}
			else if (moveType == Movement::ROTATE_LIGHT)
			{
				((LightSource*)lightSource)->spin(rotAngle, rotAxis);
			}
			else if (moveType == Movement::ROTATE_BOTH)
			{
				((Model3D*)currentObj)->spin(rotAngle, rotAxis);
				((LightSource*)lightSource)->spin(rotAngle, rotAxis);
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

