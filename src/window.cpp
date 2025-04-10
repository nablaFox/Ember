#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ignis/semaphore.hpp"
#include "window.hpp"
#include "engine.hpp"

using namespace etna;
using namespace ignis;

Window::Window(const CreateInfo& info)
	: RenderTarget({.extent = {info.width, info.height}}), m_creationInfo(info) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window =
		glfwCreateWindow(info.width, info.height, info.title, nullptr, nullptr);

	if (glfwCreateWindowSurface(_device.getInstance(), m_window, nullptr,
								&m_surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}

	m_swapchain = new Swapchain(_device.createSwapchain({
		.extent = {info.width, info.height},
		.surface = m_surface,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
	}));

	m_blitCmd = engine::newGraphicsCommand();

	m_imageAvailable = new Semaphore(_device.createSemaphore());

	m_finishedBlitting = new Semaphore(_device.createSemaphore());

	setCaptureMouse(info.captureMouse);

	double x, y;
	glfwGetCursorPos(m_window, &x, &y);

	m_lastMouseX = x;
	m_lastMouseY = y;
}

Window::~Window() {
	_device.waitIdle();

	delete m_finishedBlitting;

	delete m_imageAvailable;

	delete m_blitCmd;

	delete m_swapchain;

	vkDestroySurfaceKHR(_device.getInstance(), m_surface, nullptr);

	glfwDestroyWindow(m_window);
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(m_window);
}

void Window::pollEvents() {
	glfwPollEvents();

	double currentX, currentY;
	glfwGetCursorPos(m_window, &currentX, &currentY);
	m_mouseDeltaX = currentX - m_lastMouseX;
	m_mouseDeltaY = currentY - m_lastMouseY;
	m_lastMouseX = currentX;
	m_lastMouseY = currentY;
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

bool Window::isMouseCaptured() const {
	return m_creationInfo.captureMouse;
}

float Window::getMouseX() const {
	double x, y;
	glfwGetCursorPos(m_window, &x, &y);
	return x;
}

float Window::getMouseY() const {
	double x, y;
	glfwGetCursorPos(m_window, &x, &y);
	return y;
}

float Window::mouseDeltaX() const {
	return m_mouseDeltaX;
}

float Window::mouseDeltaY() const {
	return m_mouseDeltaY;
}

void Window::swapBuffers() {
	Image& swapchainImage = m_swapchain->acquireNextImage(m_imageAvailable);
	Image& dstImage = isMultiSampled() ? *m_resolvedImage : *m_drawImage;

	m_blitCmd->begin();

	m_blitCmd->transitionImageLayout(dstImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	m_blitCmd->transitionImageLayout(swapchainImage,
									 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	m_blitCmd->blitImage(dstImage, swapchainImage);

	m_blitCmd->transitionToOptimalLayout(swapchainImage);

	m_blitCmd->transitionToOptimalLayout(dstImage);

	m_blitCmd->end();

	const SubmitCmdInfo blitCmdInfo{
		.command = *m_blitCmd,
		.waitSemaphores = {m_imageAvailable},
		.signalSemaphores = {m_finishedBlitting},
	};

	_device.submitCommands({blitCmdInfo}, nullptr);

	engine::presentCurrent(*m_swapchain, {m_finishedBlitting});
}

void Window::setCaptureMouse(bool capture) {
	m_creationInfo.captureMouse = capture;

	if (capture) {
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		return;
	}

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
