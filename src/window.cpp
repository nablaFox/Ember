#include "window.hpp"

Window::Window(const char* windowName, uint32_t width, uint32_t height)
	: m_width(width), m_height(height) {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
}

Window::~Window() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

auto Window::shouldClose() const -> bool {
	glfwPollEvents();
	return glfwWindowShouldClose(m_window);
}

auto Window::isKeyPressed(int key) const -> bool {
	return glfwGetKey(m_window, key) == GLFW_PRESS;
}

auto Window::getMouseX() const -> double {
	double x, y;
	glfwGetCursorPos(m_window, &x, &y);
	return x;
}

auto Window::getMouseY() const -> double {
	double x, y;
	glfwGetCursorPos(m_window, &x, &y);
	return y;
}

auto Window::mouseDeltaX() const -> double {
	static double previousMouseX = getMouseX();
	float toReturn = getMouseX() - previousMouseX;

	previousMouseX = getMouseX();

	return toReturn;
}

auto Window::mouseDeltaY() const -> double {
	static double previousMouseY = getMouseY();
	float toReturn = getMouseY() - previousMouseY;

	previousMouseY = getMouseY();

	return toReturn;
}
