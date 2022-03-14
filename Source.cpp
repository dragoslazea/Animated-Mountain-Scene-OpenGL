//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#define M_PI 3.14159265358979323846

#ifdef _WIN32
#include <Windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>

int glWindowWidth = 1200;
int glWindowHeight = 800;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 8192;
const unsigned int SHADOW_HEIGHT = 8192;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::mat4 modelLand;
GLuint modelLandLoc;
glm::mat4 viewLand;
GLuint viewLandLoc;
glm::mat4 projectionLand;
GLuint projectionLandLoc;
glm::mat3 normalMatrixLand;
GLuint normalMatrixLandLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

glm::mat4 modelRiver;
GLuint modelRiverLoc;
glm::mat4 viewRiver;
GLuint viewRiverLoc;
glm::mat4 projectionRiver;
GLuint projectionRiverLoc;
glm::mat3 normalMatrixRiver;
GLuint normalMatrixRiverLoc;

glm::vec3 pointDir = glm::vec3(-50.239f, 21.743f, -3.6682f);
GLuint pointDirLoc;
glm::vec3 pointColor = glm::vec3(1.0f, 127.0f / 255.0f, 36.0f / 255.0f);
GLuint pointColorLoc;

gps::Camera myCamera(
	glm::vec3(0.0f, 8.0f, 2.5f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.1f;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D nanosuit;
gps::Model3D leaf;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D landscape;
gps::Model3D winterScene;
gps::Model3D river;
gps::Model3D wheel;

gps::Shader myCustomShader;
gps::Shader landscapeShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader riverShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

std::vector<const GLchar*> faces;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

float waveInc = 1.0f;
GLuint waveIncLoc;

float fogYes = 1.0f;
GLuint fogLoc;
bool dark = false;

glm::vec3 centruRoata = glm::vec3(19.581f, 12.647f, 0.084005f);

FILE* camCoordsFile = NULL;

void openFileForWriting() {
	camCoordsFile = fopen("camCoords.txt", "w");
}

void openFileForReading() {
	camCoordsFile = fopen("camCoords.txt", "r");
}

void closeFile() {
	fclose(camCoordsFile);
	camCoordsFile = NULL;
}

long long int timeWrite = 0;
bool readEnable = false;

void writeCoords() {
	fprintf(camCoordsFile, "%f %f %f %f %f %f %f %f %f\n", myCamera.getCameraPosition().x, myCamera.getCameraPosition().y, myCamera.getCameraPosition().z, myCamera.getCameraTarget().x, myCamera.getCameraTarget().y, myCamera.getCameraTarget().z, myCamera.getCameraUpDirection().x, myCamera.getCameraUpDirection().y, myCamera.getCameraUpDirection().z);
}

void readCoordsAndMoveCam() {
	//openFileForReading();
	float posX, posY, posZ, tarX, tarY, tarZ, upX, upY, upZ;
	if (readEnable) {
		if (camCoordsFile == NULL) {
			openFileForReading();
		}
		if (fscanf(camCoordsFile, "%f%f%f%f%f%f%f%f%f", &posX, &posY, &posZ, &tarX, &tarY, &tarZ, &upX, &upY, &upZ) != 9) {
			readEnable = false;
			closeFile();
		}
		else {
			glm::vec3 pos = glm::vec3(posX, posY, posZ);
			glm::vec3 tar = glm::vec3(tarX, tarY, tarZ);
			glm::vec3 up = glm::vec3(upX, upY, upZ);
			myCamera.setCamera(pos, tar, up);
			//printf("%f %f %f %f %f %f %f %f %f\n", myCamera.getCameraPosition().x, myCamera.getCameraPosition().y, myCamera.getCameraPosition().z, myCamera.getCameraTarget().x, myCamera.getCameraTarget().y, myCamera.getCameraTarget().z, myCamera.getCameraUpDirection().x, myCamera.getCameraUpDirection().y, myCamera.getCameraUpDirection().z);

		}
		
	}
}

GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);
	landscapeShader.useShaderProgram();
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(landscapeShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		readEnable = !readEnable;
}

double mouseX = -1.0;
double mouseY = -1.0;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (mouseX < 0) {
		mouseX = xpos;
		mouseY = ypos;
	}
	else {
		double dx = xpos - mouseX;
		double dy = ypos - mouseY;
		myCamera.rotate(-dy * 0.005, dx * 0.005);
		mouseX = xpos;
		mouseY = ypos;
	}
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_O]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_P]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	
	if (pressedKeys[GLFW_KEY_I]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_F]) {
		myCamera.rotate(-0.005f, 0.0f);
	}

	if (pressedKeys[GLFW_KEY_G]) {
		myCamera.rotate(0.005f, 0.0f);
	}

	if (pressedKeys[GLFW_KEY_V]) {
		myCamera.rotate(0.0f, -0.005f);
	}

	if (pressedKeys[GLFW_KEY_B]) {
		myCamera.rotate(0.0f, 0.005f);
	}

	if (pressedKeys[GLFW_KEY_Y]) {
		fogYes = 1.0f;
	}

	if (pressedKeys[GLFW_KEY_U]) {
		fogYes = 0.0f;
	}

	if (pressedKeys[GLFW_KEY_N]) {
		dark = true;
	}

	if (pressedKeys[GLFW_KEY_X]) {
		dark = false;
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	lightCube.LoadModel("objects/cube/cube.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
	landscape.LoadModel("objects/landscape1/landscape.obj");
	river.LoadModel("objects/river/river.obj", "objects/river/");
	wheel.LoadModel("objects/waterwheel/wheel.obj", "objects/waterwheel/");
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/shaderLight.vert", "shaders/shaderLight.frag");
	depthMapShader.useShaderProgram();
	landscapeShader.loadShader("shaders/landscapeShader.vert", "shaders/landscapeShader.frag");
	landscapeShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
	depthMapShader.useShaderProgram();
}

void initUniforms() {
	//for river waves
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	waveIncLoc = glGetUniformLocation(myCustomShader.shaderProgram, "inc");
	glUniform1f(waveIncLoc, waveInc);

	//for landscape
	landscapeShader.useShaderProgram();

	modelLandLoc = glGetUniformLocation(landscapeShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLandLoc, 1, GL_FALSE, glm::value_ptr(model));

	viewLandLoc = glGetUniformLocation(landscapeShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLandLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrixLandLoc = glGetUniformLocation(landscapeShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLandLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	
	projectionLandLoc = glGetUniformLocation(landscapeShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLandLoc, 1, GL_FALSE, glm::value_ptr(projection));

	fogLoc = glGetUniformLocation(landscapeShader.shaderProgram, "fogYes");
	glUniform1f(fogLoc, fogYes);



	//set the light direction (direction towards the light)
	lightDir = glm::vec3(26.465f, 37.535f, -21.192f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(landscapeShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));


	//set light color
	if (!dark) {
		lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	}
	else {
		lightColor = glm::vec3(0.1f, 0.1f, 0.1f); //dark light
	}
	lightColorLoc = glGetUniformLocation(landscapeShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	pointDirLoc = glGetUniformLocation(landscapeShader.shaderProgram, "pointDir");
	glUniform3fv(pointDirLoc, 1, glm::value_ptr(pointDir));

	//set point light color
	pointColorLoc = glGetUniformLocation(landscapeShader.shaderProgram, "pointColor");
	glUniform3fv(pointColorLoc, 1, glm::value_ptr(pointColor));


	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
	//TODO - Return the light-space transformation matrix
	glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 6.0f, 0.0f));
	const GLfloat near_plane = 0.1f, far_plane = 200.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
	return lightSpaceTrMatrix;
}

float timeRot = 0.0f;

void drawObjects(gps::Shader shader, bool depthPass) {

	shader.useShaderProgram();
	
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLandLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLandLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	
	landscape.Draw(shader);
	fogLoc = glGetUniformLocation(landscapeShader.shaderProgram, "fogYes");
	glUniform1f(fogLoc, fogYes);
	
	
	model = glm::translate(model, centruRoata);
	model = glm::rotate(model, timeRot * (float)M_PI, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, -centruRoata);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	wheel.Draw(shader);
	timeRot += 0.01f;
	if (timeRot >= 2.0f) {
		timeRot = 0.0f;
	}
}

void renderScene() {

	// depth maps creation pass
	//TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map
	//render the scene to the depth buffer
	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthMapShader, GL_FALSE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		landscapeShader.useShaderProgram();

		if (readEnable) {
			readCoordsAndMoveCam();
		}

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLandLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(landscapeShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(landscapeShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		//set light color
		if (!dark) {
			lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
		}
		else {
			lightColor = glm::vec3(0.1f, 0.1f, 0.1f); //dark light
		}
		lightColorLoc = glGetUniformLocation(landscapeShader.shaderProgram, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

		drawObjects(landscapeShader, false);
		

		//draw a white cube around the light
		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		//normalMatrixLand = glm::mat3(glm::inverseTranspose(viewLand * modelLand));
		//glUniformMatrix3fv(normalMatrixLandLoc, 1, GL_FALSE, glm::value_ptr(normalMatrixLand));

		lightCube.Draw(lightShader);

		//draw the river
		myCustomShader.useShaderProgram();
		
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		river.Draw(myCustomShader);
		waveInc += 0.05f;
		if (waveInc >= 1e6) {
			waveInc = 0.0f;
		}
		glUniform1f(waveIncLoc, waveInc);

	}

	mySkyBox.Draw(skyboxShader, view, projection);
}


void initSkyBox() {
	faces.push_back("textures/skybox/sh_rt.png");
	faces.push_back("textures/skybox/sh_lf.png");
	faces.push_back("textures/skybox/sh_up.png");
	faces.push_back("textures/skybox/sh_dn.png");
	faces.push_back("textures/skybox/sh_bk.png");
	faces.push_back("textures/skybox/sh_ft.png");

	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));

}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char* argv[]) {
	
	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}


	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();
	initSkyBox();

	glCheckError();

	//openFileForWriting();
	//openFileForReading();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();

		//writeCoords();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//closeFile();

	cleanup();

	return 0;
}