#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include "ignis/swapchain.hpp"
#include "render_target.hpp"
#include "engine.hpp"

namespace etna {

class Window : RenderTarget {
public:
	struct CreateInfo {
		uint32_t width{0};
		uint32_t height{0};
		const char* title{"Etna Window"};
		bool captureMouse{false};
	};

	Window(const CreateInfo&);

	~Window();

	bool shouldClose() const;

	bool isKeyPressed(int key) const;
	bool isKeyClicked(int key);

	double getMouseX() const;
	double getMouseY() const;

	double mouseDeltaX();
	double mouseDeltaY();

	void swapBuffers();

private:
	ignis::Swapchain* m_swapchain;
	GLFWwindow* m_window;
	VkSurfaceKHR m_surface;
	CreateInfo m_creationInfo;

	double m_lastMouseX;
	double m_lastMouseY;

	std::unordered_map<int, bool> m_prevKeyStates;
};

}  // namespace etna
