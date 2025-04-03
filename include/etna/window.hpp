#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include "ignis/swapchain.hpp"
#include "render_target.hpp"

namespace etna {

class Window : public RenderTarget {
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

	void pollEvents();

	bool isKeyPressed(int key) const;
	bool isKeyClicked(int key);

	bool isMouseCaptured() const;

	float getMouseX() const;
	float getMouseY() const;

	float mouseDeltaX() const;
	float mouseDeltaY() const;

	void swapBuffers();

	void setCaptureMouse(bool capture);

private:
	ignis::Swapchain* m_swapchain;
	GLFWwindow* m_window;
	VkSurfaceKHR m_surface;
	CreateInfo m_creationInfo;

	float m_lastMouseX{0};
	float m_lastMouseY{0};
	float m_mouseDeltaX{0};
	float m_mouseDeltaY{0};

	std::unordered_map<int, bool> m_prevKeyStates;

	ignis::Command* m_blitCmd;
	ignis::Semaphore* m_imageAvailable;
	ignis::Semaphore* m_finishedBlitting;
};

}  // namespace etna
