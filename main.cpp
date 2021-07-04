#include "Vec3.h"
#include <vector>
#include <iostream>
#include "Window_GLFW.h"
#include "OpenCL.h"
#include <GLFW/glfw3.h>

#define BASE_ITERATIONS 100.0
#define BASE_THRESHOLD 16.0

void MakePallette(int iterations, Vec3* outColors)
{
	std::vector<Vec3> colorOutline;
	colorOutline.push_back({ 0, 0, 1 });
	colorOutline.push_back({ 0, 1, 1 });
	colorOutline.push_back({ 1, 1, 0 });
	colorOutline.push_back({ 1, 0, 1 });
	colorOutline.push_back({ 0.5, 0, 0.6 });
	colorOutline.push_back({ 0, 1, 0.5 });
	colorOutline.push_back({ 0, 0, 0 });

	for (int i = 0; i < iterations; ++i) {

		// exponential interpolation from 0 to 1
		double alpha = pow(2, (double)i / iterations) - 1;
		double index = alpha * colorOutline.size();

		int prev = (int)index;
		if (prev == colorOutline.size() - 1) {
			outColors[i] = colorOutline[colorOutline.size() - 1];
			continue;
		}

		int next = ceil(index);
		double amt = index - prev;

		Vec3 newColor = colorOutline[prev] * (1 - amt) + colorOutline[next] * amt;
		outColors[i] = newColor;
	}
}

int iterations = BASE_ITERATIONS;
int threshold = BASE_THRESHOLD;

double mx = -1;
double my = -1;
bool clicked = false;
bool recalculateColors = true;

double x(-0.75);
double y(0.0);
double s(1.0);

void OnKeyAction(int key, int scancode, int action)
{
	if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
		iterations += 10;
		threshold += 5;
		recalculateColors = true;
	}
	else if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
		iterations -= 10;
		threshold -= 5;
		recalculateColors = true;
	}

	iterations = std::max(BASE_ITERATIONS, (double)iterations);
	threshold = std::max(BASE_THRESHOLD, (double)threshold);
}

void OnMouseMove(double newmx, double newmy)
{
	if (mx == -1) {
		mx = newmx;
	}
	if (my == -1) {
		my = newmy;
	}

	if (clicked) {
		x += (mx - newmx) / s / 500;
		y += -(my - newmy) / s / 500;
	}

	mx = newmx;
	my = newmy;
}

void OnMouseClick(int button, int action)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			clicked = true;
		}
		else if (action == GLFW_RELEASE) {
			clicked = false;
		}
	}
}

void OnMouseScroll(double amount)
{
	if (amount > 0) {
		s *= 2;
	}
	else if (amount < 0) {
		s /= 2;
	}

	recalculateColors = true;
}

int main(int argc, char* argv[])
{
	Window_GLFW window("Mandelbrot", 1920, 1080);
	window.SetOnMouseMove(OnMouseMove);
	window.SetOnMouseClick(OnMouseClick);
	window.SetOnMouseScroll(OnMouseScroll);
	//window.SetOnKeyAction(OnKeyAction);

	CL::InitializeRuntime();

	CL::Memory framebuffer =
		CL::Memory::CreateFromOpenGLResource(window.GetGLTextureType(), window.GetGLTextureName());

	std::string files[] = { "kernel.cl" };
	CL::Program program(1, files);

	CL::Program::Kernel kernel(program, "func");
	kernel.SetArgument(0, framebuffer);
	
	size_t numThreads[] = { window.GetWidth(), window.GetHeight() };
	size_t blockSizeHints[] = { 32, 8 };

	Vec3* colors = nullptr;
	CL::Memory* colorsList = nullptr;

	while ( !window.ShouldClose() ) {

		if (recalculateColors) {

			iterations = std::max(BASE_ITERATIONS, log(s) * BASE_ITERATIONS);
			threshold = std::max(BASE_THRESHOLD, log(s) * BASE_THRESHOLD);

			if (colors) {
				delete[] colors;
				delete colorsList;
			}
			colors = new Vec3[iterations];
			MakePallette(iterations, colors);
			colorsList = new CL::Memory(CL::Memory::Access::READ, iterations * sizeof(Vec3), colors);

			kernel.SetArgument(4, threshold);
			kernel.SetArgument(5, iterations);
			kernel.SetArgument(6, *colorsList);

			recalculateColors = false;
		}

		kernel.SetArgument(1, x);
		kernel.SetArgument(2, y);
		kernel.SetArgument(3, s);

		framebuffer.AcquireFromOpenGL();
		kernel.Execute(2, numThreads, blockSizeHints);
		framebuffer.ReleaseToOpenGL();

		window.DrawGLTexture();
		window.Refresh();
		window.HandleEvents();
	}

	CL::DestroyRuntime();

	return 0;
}