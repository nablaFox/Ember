#include "window.hpp"

using namespace ember;

Window::Window(CreateInfo info) : m_width(info.width), m_height(info.height) {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window =
		glfwCreateWindow(m_width, m_height, info.windowName, nullptr, nullptr);

	captureMouse(info.captureMouse);
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

auto Window::isKeyClicked(int key) -> bool {
	bool currentState = isKeyPressed(key);
	bool wasPressed = false;

	auto it = m_prevKeyStates.find(key);
	if (it != m_prevKeyStates.end()) {
		wasPressed = it->second;
	}

	bool clicked = currentState && !wasPressed;

	m_prevKeyStates[key] = currentState;
	return clicked;
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

auto Window::captureMouse(bool capture) const -> void {
	glfwSetInputMode(m_window, GLFW_CURSOR,
					 capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
