#pragma once
#include <string>

class GLFWwindow;

class Window_GLFW
{
private:
	GLFWwindow* pglfwWindow;
	unsigned int textureName;

	static Window_GLFW* existingWindow;

	static void MouseMoveCallback(GLFWwindow* window, double x, double y);
	static void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);
	static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void (*onMouseMove)(double x, double y);
	void (*onMouseClick)(int button, int action);
	void (*onMouseScroll)(double amount);
	void (*onKeyAction)(int key, int scancode, int action);

public:
	Window_GLFW(const std::string& name, int width, int height);
	~Window_GLFW();

	bool ShouldClose() const;
	int GetWidth() const;
	int GetHeight() const;
	void Refresh();
	void HandleEvents();

	void DrawGLTexture();
	unsigned int GetGLTextureName();
	unsigned int GetGLTextureType();

	void BlockUntilQuit();

	void SetOnMouseMove(void (*onMouseMove)(double, double));
	void SetOnMouseClick(void (*onMouseClick)(int, int));
	void SetOnMouseScroll(void (*onMouseScroll)(double));
	void SetOnKeyAction(void (*onKeyAction)(int, int, int));
};
