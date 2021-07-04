#include "Window_GLFW.h"
#define GLEW_STATIC
#include <GLEW/glew.h> // must be included before glfw3.h
#include <GLFW/glfw3.h>
#include <assert.h>
#include <iostream>

#define glfw_exit() exit(1)

static void GLFW_ErrorCallback(int code, const char* description) {
	std::cerr << "GLFW Error: " << description << std::endl;
	glfw_exit();
}

static void GLAPIENTRY GL_ErrorCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "OPENGL Error: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
	glfw_exit();
}

Window_GLFW* Window_GLFW::existingWindow = NULL;

Window_GLFW::Window_GLFW(const std::string& name, int width, int height)
	: pglfwWindow(NULL)
{
	int result;

	assert(existingWindow == NULL);

	result = glfwInit();
	assert(result == GLFW_TRUE);

#ifdef _DEBUG
	glfwSetErrorCallback(GLFW_ErrorCallback);
#endif

	existingWindow = this;

	pglfwWindow = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	assert(pglfwWindow);

	glfwMakeContextCurrent(pglfwWindow);

	result = glewInit();
	assert(result == GLEW_OK);

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(GL_ErrorCallback, 0);
#endif

	glfwSwapInterval(1);
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glGenTextures(1, &textureName);
	glBindTexture(GL_TEXTURE_2D, textureName);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureName);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glfwSetCursorPosCallback(pglfwWindow, MouseMoveCallback);
	glfwSetMouseButtonCallback(pglfwWindow, MouseClickCallback);
	glfwSetScrollCallback(pglfwWindow, MouseScrollCallback);
	glfwSetKeyCallback(pglfwWindow, KeyCallback);
}

Window_GLFW::~Window_GLFW() 
{
	glDisable(GL_TEXTURE_2D);
	glfwDestroyWindow(pglfwWindow);
	glfwTerminate();
}

bool Window_GLFW::ShouldClose() const
{
	return glfwWindowShouldClose(pglfwWindow);
}
int Window_GLFW::GetWidth() const
{
	static int width;
	glfwGetWindowSize(pglfwWindow, &width, NULL);
	return width;
}
int Window_GLFW::GetHeight() const
{
	static int height;
	glfwGetWindowSize(pglfwWindow, NULL, &height);
	return height;
}
void Window_GLFW::Refresh()
{
	glfwSwapBuffers(pglfwWindow);
}
void Window_GLFW::HandleEvents()
{
	glfwPollEvents();
}
void Window_GLFW::DrawGLTexture()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);

	glTexCoord2f(1, 0);
	glVertex2f(1, -1);

	glTexCoord2f(1, 1);
	glVertex2f(1, 1);

	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);

	glEnd();
}
unsigned int Window_GLFW::GetGLTextureName()
{
	return textureName;
}
unsigned int Window_GLFW::GetGLTextureType()
{
	return GL_TEXTURE_2D;
}
void Window_GLFW::BlockUntilQuit()
{
	while (!ShouldClose())
		HandleEvents();
}

void Window_GLFW::SetOnMouseMove(void (*onMouseMove)(double, double))
{
	this->onMouseMove = onMouseMove;
}
void Window_GLFW::MouseMoveCallback(GLFWwindow* window, double x, double y)
{
	if (existingWindow->onMouseMove != nullptr) {
		existingWindow->onMouseMove(x, y);
	}
}

void Window_GLFW::SetOnMouseClick(void (*onMouseClick)(int, int))
{
	this->onMouseClick = onMouseClick;
}
void Window_GLFW::MouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (existingWindow->onMouseClick != nullptr) {
		existingWindow->onMouseClick(button, action);
	}
}

void Window_GLFW::SetOnMouseScroll(void (*onMouseScroll)(double))
{
	this->onMouseScroll = onMouseScroll;
}
void Window_GLFW::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (existingWindow->onMouseScroll != nullptr) {
		existingWindow->onMouseScroll(yoffset);
	}
}

void Window_GLFW::SetOnKeyAction(void (*onKeyAction)(int, int, int))
{
	this->onKeyAction = onKeyAction;
}
void Window_GLFW::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (existingWindow->onKeyAction != nullptr) {
		existingWindow->onKeyAction(key, scancode, action);
	}
}
