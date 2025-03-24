#pragma once

#include <GLFW/glfw3.h>
#include "ignis/swapchain.hpp"
#include "render_target.hpp"

namespace etna {

class Engine;

class Window : RenderTarget {
public:
	struct CreateInfo {
		const Engine* engine;  // TODO: should be a ref
		uint32_t width{0};
		uint32_t height{0};
		const char* title{"Etna Window"};
		bool captureMouse{false};
	};

	Window(const CreateInfo&);

	~Window();

	void swapBuffers();

private:
	const ignis::Device& m_device;
	ignis::Swapchain* m_swapchain;
	CreateInfo m_creationInfo;
	GLFWwindow* m_window;
	VkSurfaceKHR m_surface;
};

}  // namespace etna
