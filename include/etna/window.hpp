#pragma once

#include <GLFW/glfw3.h>
#include "ignis/swapchain.hpp"
#include "render_target.hpp"

namespace etna {

class Engine;

class Window : RenderTarget {
public:
	struct CreateInfo {
		const Engine& engine;
		uint32_t width{0};
		uint32_t height{0};
		const char* title{"Etna Window"};
		bool captureMouse{false};
	};

	Window(const CreateInfo&);

	~Window();

	void swapBuffers();

private:
	ignis::Swapchain* m_swapchain;
	ignis::Semaphore* m_imageAvailable;
	ignis::Semaphore* m_renderFinished;
	CreateInfo m_creationInfo;
	GLFWwindow* m_window;
};

}  // namespace etna
