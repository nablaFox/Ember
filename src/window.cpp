#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "window.hpp"
#include "engine.hpp"

using namespace etna;
using namespace ignis;

Window::Window(const CreateInfo& info)
	: RenderTarget({
		  .extent = {info.width, info.height},
		  .samples = info.engine->getDevice().getMaxSampleCount(),
	  }),
	  m_creationInfo(info),
	  m_device(info.engine->getDevice()) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window =
		glfwCreateWindow(info.width, info.height, info.title, nullptr, nullptr);

	if (glfwCreateWindowSurface(m_device.getInstance(), m_window, nullptr,
								&m_surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}

	m_swapchain = new Swapchain({
		.device = &m_device,
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
