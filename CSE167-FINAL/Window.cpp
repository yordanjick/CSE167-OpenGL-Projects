#include "Window.h"

int Window::width;
int Window::height;

GLdouble Window::nearDist = 1;
GLdouble Window::farDist = 1000.0;

GLdouble Window::FOV = 60.0; // fov of the projection

const char* Window::windowTitle = "Final";

// Objects to display
Transform* Window::world;

Skybox* Window::skybox;

bool Window::normalColor;

GLfloat Window::modelSize;

double Window::prevTime;

PlayerControl Window::xControl = PlayerControl::NONE, 
			  Window::yControl = PlayerControl::NONE, 
			  Window::zControl = PlayerControl::NONE;

float Window::cameraPitch, Window::cameraYaw;

DepthQuad* Window::debugQuad;

bool Window::displayShadowmap = false;
bool Window::displayShadows = true;
bool Window::displayBloom = true;
bool Window::toonShading = true;

GLuint Window::mode = 1; // mouse mode for rotation

glm::mat4 Window::projection; // Projection matrix.

GLuint Window::depthFBO, Window::depthmap, Window::depthDebug;
GLuint Window::bloomProgram, Window::blurProgram;

GLuint Window::hdrfbo;
GLuint Window::rboDepth; // depth render buffer
GLuint Window::colorBuffers[2];
GLuint Window::pingpongfbo[2];
GLuint Window::pingpongBuffer[2];

glm::vec3 Window::eye(0, 8, 10); // Camera position.
glm::vec3 Window::front(0, 0, -1.f); // The direction of the front of the camera.
glm::vec3 Window::up(0, 1, 0); // The up direction of the camera.

// View matrix, defined by eye, center and up.
glm::mat4 Window::view = glm::lookAt(Window::eye, Window::eye + Window::front, 
										Window::up);
std::vector<LightSource*> Window::lights;

GLuint Window::program;
GLuint Window::texProgram, Window::depthProgram; // The shader program ids
GLuint Window::skyboxProgram;

GLuint Window::projectionLoc; // Location of projection in shader.
GLuint Window::viewLoc; // Location of view in shader.
GLuint Window::eyeLoc; // Location of the viewer in shader.

bool Window::initializeProgram() {
	// glEnable(GL_DEBUG_OUTPUT);
	// glDebugMessageCallback((GLDEBUGPROC)MessageCallback, 0);

	// Create a shader program with a vertex shader and a fragment shader.
	program = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
	texProgram = LoadShaders("shaders/texture_shader.vert", "shaders/texture_shader.frag");
	depthProgram = LoadShaders("shaders/depth_shader.vert", "shaders/depth_shader.frag");
	depthDebug = LoadShaders("shaders/depth_debug.vert", "shaders/depth_debug.frag");
	blurProgram = LoadShaders("shaders/blur_shader.vert", "shaders/blur_shader.frag");
	bloomProgram = LoadShaders("shaders/bloom_shader.vert", "shaders/bloom_shader.frag");
	skyboxProgram = LoadShaders("shaders/skybox_shader.vert", "shaders/bloom_shader.frag");

	// Check the shader program.
	if (!program || !texProgram || !depthProgram || !depthDebug ||
		!blurProgram || !bloomProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	cameraPitch = 0;
	cameraYaw = -90;
	prevTime = glfwGetTime();

	// Activate the shader program.
	glUseProgram(texProgram);

	// Create depth frame buffer and depth map
	glGenFramebuffers(1, &depthFBO);
	// generate depth map texture and set parameters
	glGenTextures(1, &depthmap);
	glBindTexture(GL_TEXTURE_2D, depthmap);
	// gen with dimensions 1024x1024
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach generated texture to depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmap, 0);
	// glDrawBuffer(GL_NONE);
	// glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Failed to create frame buffer" << std::endl;
		return false;
	}

	// set up color frame buffers to render scene to
	glGenFramebuffers(1, &hdrfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrfbo);
	glGenTextures(2, colorBuffers);
	// generate 2 color buffer textures, sets up parameters, and attaches them.
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// render buffer for hdr frame buffer
	glGenRenderbuffers(1, &rboDepth); // create and attach depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);


	// tell opengl to render multiple color buffers
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// generate buffers to blur image
	glGenFramebuffers(2, pingpongfbo);
	glGenTextures(2, pingpongBuffer);
	// set parameters of generated textures
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongfbo[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
		);
	}

	return true;
}

bool Window::initializeObjects()
{
	normalColor = false;

	modelSize = 1.0f;

	debugQuad = new DepthQuad();

	// create skybox

	std::vector<std::string> faces
	{
		"lmcity/lmcity_lf.tga",
		"lmcity/lmcity_rt.tga",
		"lmcity/lmcity_up.tga",
		"lmcity/lmcity_dn.tga",
		"lmcity/lmcity_ft.tga",
		"lmcity/lmcity_bk.tga"
	};

	// skybox = new Skybox(1);
	// skybox->loadCubemap(faces);

	// initialize models and materials of the objects
	projectionLoc = glGetUniformLocation(texProgram, "projection");
	viewLoc = glGetUniformLocation(texProgram, "view");
	eyeLoc = glGetUniformLocation(texProgram, "eye");

	Mesh::depthShader = depthProgram;
	// initial world transform with identity matrix
	world = new Transform(glm::mat4(1));
	// Model* testModel = new Model("models/test/nanosuit.obj");

	// adjusting transform matrix and loading models
	glm::mat4 boxMat = glm::translate(glm::vec3(0, -3.f, 0));

	Model* box = new Model("models/box/scene.gltf", boxMat);

	glm::mat4 roomMat = glm::scale(glm::vec3(.05f));

	Model* room = new Model("models/room/scene.gltf", roomMat);

	glm::mat4 tvMat = glm::translate(glm::vec3(4.f, 3.7f, 25.f)) * 
		glm::rotate(glm::mat4(1), glm::radians(90.f), glm::vec3(1, 0, 0)) *
		glm::rotate(glm::mat4(1), glm::radians(180.f), glm::vec3(0, 1, 0)) *
		glm::scale(glm::vec3(1.f));
	Model* tv = new Model("models/tv/scene.gltf", tvMat);

	glm::mat4 tv_tableMat = glm::translate(glm::vec3(0.f, 2.7f, 25.f)) *
		glm::rotate(glm::mat4(1), glm::radians(180.f), glm::vec3(1, 0, 0)) * 
		glm::rotate(glm::mat4(1), glm::radians(90.f), glm::vec3(0, 1, 0)) *
		glm::scale(glm::vec3(.05f));
	Model* tv_table = new Model("models/tv_table/scene.gltf", tv_tableMat);

	glm::mat4 sofaMat = glm::translate(glm::vec3(0.f, 0.2f, 3.f)) *
		glm::scale(glm::vec3(0.5f));
	Model* sofa = new Model("models/sofa/scene.gltf", sofaMat);

	glm::mat4 lightMat = glm::translate(glm::vec3(6.f, 8.f, -50.f)) *
		glm::rotate(glm::radians(90.f), glm::vec3(1, 0, 0)) *
		glm::scale(glm::vec3(3.f));

	glm::mat4 nightstandMat = glm::translate(glm::vec3(26.f, 4.f, 10.f)) *
		glm::rotate(glm::radians(-90.f), glm::vec3(1, 0, 0)) *
		glm::rotate(glm::radians(-90.f), glm::vec3(0, 0, 1)) *
		glm::scale(glm::vec3(10.f));
	Model* nightstand = new Model("models/nightstand/scene.gltf", nightstandMat);

	glm::mat4 dining_tableMat = glm::translate(glm::vec3(0.f, 0.f, -22.f)) *
		glm::scale(glm::vec3(5.5f));
	Model* dining_table = new Model("models/dining_table/scene.gltf", dining_tableMat);

	glm::mat4 bedMat = glm::translate(glm::vec3(46.f, 0.f, -8.f)) * 
		glm::rotate(glm::radians(-90.f), glm::vec3(1, 0, 0)) *
		glm::scale(glm::vec3(1));
	Model* bed = new Model("models/bed/scene.gltf", bedMat);

	glm::mat4 flamingoMat = glm::translate(glm::vec3(-8.f, -5.f, -20.f)) *
		glm::rotate(glm::radians(-90.f), glm::vec3(1, 0, 0)) *
		glm::scale(glm::vec3(1));
	Model* flamingo = new Model("models/flamingo/scene.gltf", flamingoMat);

	glm::mat4 plantMat = glm::translate(glm::vec3(-2.f, 3.5f, -22.f)) *
		glm::scale(glm::vec3(1));
	Model* plant = new Model("models/plant/scene.gltf", plantMat);
		
	LightSource::depthShader = depthProgram;

	LightSource* light = new LightSource("models/moon/scene.gltf", lightMat);
	lights.push_back(light);

	world->addChild(room);
	world->addChild(light);
	world->addChild(tv_table);
	world->addChild(tv);
	world->addChild(sofa);
	world->addChild(nightstand);
	world->addChild(bed);
	world->addChild(dining_table);
	world->addChild(flamingo);
	world->addChild(plant);

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

	double delta_time = glfwGetTime() - prevTime;

	float velocity = 12;

	// determines speed of which camera moves
	float cameraVel = velocity * (float)delta_time;

	// control x, y, z coord of camera control
	if (xControl == PlayerControl::LEFT)
	{
		eye += -cameraVel * glm::normalize(glm::cross(front, up));
	}
	else if (xControl == PlayerControl::RIGHT)
	{
		eye += cameraVel * glm::normalize(glm::cross(front, up));
	}
	if (yControl == PlayerControl::DOWN)
	{
		eye += -cameraVel * up;
	}
	else if (yControl == PlayerControl::UP)
	{
		eye += cameraVel * up;
	}
	if (zControl == PlayerControl::FOWARD)
	{
		eye += cameraVel * front;
	}
	else if (zControl == PlayerControl::BACKWARD)
	{
		eye += -cameraVel * front;
	}
	view = glm::lookAt(eye, eye + front, up);
	prevTime = glfwGetTime();

}

void Window::displayCallback(GLFWwindow* window)
{	

	glEnable(GL_CULL_FACE);
	glUseProgram(depthProgram);

	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	
	// create orthogonal projection matrix to render scene from light's pov
	// for directional lights only, change maybe to point?
	glm::vec3 lightPos = lights[0]->position[3];
	// glm::vec3 lightPos = glm::vec3(-5, 5, -5);
	float near = 1.f, far = 100.f;

	if (!displayShadows)
		far = near;
	glm::mat4 lightProj = glm::ortho(-35.f, 35.f, -17.f, 21.f, near, far);
	// glm::mat4 lightProj = glm::ortho(-5.f, 5.f, -5.f, 5.f, near, far);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.f, 4.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	// glm::mat4 lightView = glm::lookAt(eye,eye + front, up);

	glm::mat4 lightSpaceMatrix = lightProj * lightView;
	
	glUniformMatrix4fv(glGetUniformLocation(depthProgram, "lightMat"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));


	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	world->draw(glm::mat4(1), depthProgram);

	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

 
	// RENDERING OF SCENE
	glViewport(0, 0, width, height);
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	glUseProgram(texProgram);

	// Specify the values of the uniform variables we are going to use.
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniform3fv(eyeLoc, 1, glm::value_ptr(eye));

	glUniform3fv(glGetUniformLocation(texProgram, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(texProgram, "viewPos"), 1, glm::value_ptr(eye));

	glUniform1i(glGetUniformLocation(texProgram, "ignoreLight"), false);

	glUniform1i(glGetUniformLocation(texProgram, "toonShading"), toonShading);

	glUniformMatrix4fv(glGetUniformLocation(texProgram, "lightMat"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	// glUniform1i(glGetUniformLocation(program, "normalColor"), normalColor);

	glActiveTexture(GL_TEXTURE5);
	// bind depth map to draw with scene
	glBindTexture(GL_TEXTURE_2D, depthmap);

	glUniform1i(glGetUniformLocation(texProgram, "shadowMap"), 5);
	
	if (displayBloom)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, hdrfbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	// Render the scenegraph, initially passing in identity matrix.
	world->draw(glm::mat4(1), texProgram);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	

	if (displayBloom)
	{
		// run blur on bright fragments with 2-pass gaussian blur
		bool horizontal = true, first_iteration = true;
		unsigned int amount = 10;
		glUseProgram(blurProgram);

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(blurProgram, "image"), 0);
		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongfbo[horizontal]);
			glUniform1i(glGetUniformLocation(blurProgram, "horizontal"), horizontal);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongBuffer[!horizontal]);
			// bind texture of other framebuffer (or scene if first iteration)
			debugQuad->draw();
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// debugQuad->draw();
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(bloomProgram);

		glUniform1i(glGetUniformLocation(bloomProgram, "scene"), 0);
		glUniform1i(glGetUniformLocation(bloomProgram, "bloomBlur"), 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
		glUniform1f(glGetUniformLocation(bloomProgram, "exposure"), 1.f);
		debugQuad->draw();
		// std::cerr << glGetError() << std::endl;
	}

	glActiveTexture(GL_TEXTURE5);
	// bind depth map to draw with scene
	glBindTexture(GL_TEXTURE_2D, depthmap);

	// render shadow/depthmap directly
	glUseProgram(depthDebug);
	glUniform1i(glGetUniformLocation(depthDebug, "depthMap"), 5);

	if (displayShadowmap)
	{
		glViewport(0, 0, width / 2, height / 2);
		debugQuad->draw();
	}
	/* bruh how do you render skybox
	glDepthMask(GL_FALSE);
	// use skybox shader
	glUseProgram(skyboxProgram);
	glm::mat4 skybox_view = glm::mat4(glm::mat3(view));

	// set values of the skybox shader
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "view"), 1, GL_FALSE, glm::value_ptr(skybox_view));

	skybox->draw();
	glDepthMask(GL_TRUE);
	*/

	glDisable(GL_CULL_FACE);
	
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
		case GLFW_KEY_W:
			// if moving in negative z axis
			if (zControl == PlayerControl::NONE)
				zControl = PlayerControl::FOWARD;
			break;
		case GLFW_KEY_S:
			// if moving in positive z axis
			if (zControl == PlayerControl::NONE)
				zControl = PlayerControl::BACKWARD;
			break;
		case GLFW_KEY_A:
			// if moving in negative x axis
			if (xControl == PlayerControl::NONE)
				xControl = PlayerControl::LEFT;
			break;
		case GLFW_KEY_D:
			// if moving in positive x axis
			if (xControl == PlayerControl::NONE)
				xControl = PlayerControl::RIGHT;
			break;
		case GLFW_KEY_SPACE:
			// if moving in positive y axis
			if (yControl == PlayerControl::NONE)
				yControl = PlayerControl::UP;
			break;
		case GLFW_KEY_LEFT_CONTROL:
			// if moving in negative y axis
			if (yControl == PlayerControl::NONE)
				yControl = PlayerControl::DOWN;
			break;
		case GLFW_KEY_F1:
			// show or hide shadows
			displayShadows = !displayShadows;
			break;
		case GLFW_KEY_F2:
			// show or hide shadow/depth map
			displayShadowmap = !displayShadowmap;
			break;
		case GLFW_KEY_F3:
			// show or hide bloom
			displayBloom = !displayBloom;
			break;
		case GLFW_KEY_F4:
			// render toon or not
			toonShading = !toonShading;
			break;
		default:
			break;
		}
	}
	// Check for a key press.
	else if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_W:
		case GLFW_KEY_S:
			zControl = PlayerControl::NONE;
			break;
		case GLFW_KEY_A:
		case GLFW_KEY_D:
			xControl = PlayerControl::NONE;
			break;
		case GLFW_KEY_SPACE:
		case GLFW_KEY_LEFT_CONTROL:
			yControl = PlayerControl::NONE;
			break;
		default:
			break;
		}
	}
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}

void Window::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	
}

void Window::mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{

	float sensitivity = 0.05f;
	// change rotation of camera by offset of mouse
	
	float xoffset = (xpos - (width/2)) * sensitivity;
	float yoffset = (ypos - (height/2)) * sensitivity;
	cameraYaw = glm::mod(cameraYaw + xoffset, 360.f);
	cameraPitch += yoffset;
	
	// clamp camera rotation to prevent weird stuff
	if (cameraPitch > 89.f)
		cameraPitch = 89.f;
	if (cameraPitch < -89.f)
		cameraPitch = -89.f;

	glm::vec3 camFront;
	camFront.x = glm::cos(glm::radians(cameraYaw)) * glm::cos(glm::radians(cameraPitch));
	camFront.y = -1 * glm::sin(glm::radians(cameraPitch));
	// camFront.z = -1;
	camFront.z = glm::sin(glm::radians(cameraYaw)) * glm::cos(glm::radians(cameraPitch));
	front = glm::normalize(camFront);

	// reset mouse position
	glfwSetCursorPos(window, width / 2, height / 2);
}

void Window::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}


