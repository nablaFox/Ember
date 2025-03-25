#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "window.hpp"
#include "engine.hpp"

using namespace etna;
using namespace ignis;

Window::Window(const CreateInfo& info)
	: RenderTarget({
		  .extent = {info.width, info.height},
		  .samples = Engine::getDevice().getMaxSampleCount(),
	  }),
	  m_creationInfo(info) {
	Device& device = Engine::getDevice();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window =
		glfwCreateWindow(info.width, info.height, info.title, nullptr, nullptr);

	if (glfwCreateWindowSurface(device.getInstance(), m_window, nullptr,
								&m_surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}

	m_swapchain = new Swapchain({
		.device = &device,
		.extent = {info.width, info.height},
		.surface = m_surface,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
	});
}

Window::~Window() {
	delete m_swapchain;

	// TODO: uncomment this when updating ignis
	// vkDestroySurfaceKHR(m_device.getInstance(), m_surface, nullptr);

	glfwDestroyWindow(m_window);
}

bool Window::shouldClose() const {
	glfwPollEvents();
	return glfwWindowShouldClose(m_window);
}

bool Window::isKeyPressed(int key) const {
	return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool Window::isKeyClicked(int key) {
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

double Window::getMouseX() const {
	double x, y;
	glfwGetCursorPos(m_window, &x, &y);
	return x;
}

double Window::getMouseY() const {
	double x, y;
	glfwGetCursorPos(m_window, &x, &y);
	return y;
}

double Window::mouseDeltaX() {
	float toReturn = getMouseX() - m_lastMouseX;

	m_lastMouseX = getMouseX();

	return toReturn;
}

double Window::mouseDeltaY() {
	float toReturn = getMouseY() - m_lastMouseY;

	m_lastMouseY = getMouseY();

	return toReturn;
}

void Window::swapBuffers() {}
