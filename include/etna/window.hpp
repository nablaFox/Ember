#pragma once

#include <unordered_map>
#include "ignis/swapchain.hpp"
#include "render_target.hpp"

struct GLFWwindow;

namespace etna {

enum Key {
	KEY_ENTER = 257,
	KEY_ESCAPE = 256,
	KEY_0 = 48,
	KEY_W = 87,
	KEY_S = 83,
	KEY_A = 65,
	KEY_D = 68,
	KEY_SPACE = 32,
	KEY_UP = 265,
	KEY_DOWN = 264,
	KEY_LEFT = 263,
	KEY_RIGHT = 262,
	KEY_LEFT_SHIFT = 340,
	KEY_LEFT_CONTROL = 341,
};

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

	void pollEvents();
	void swapBuffers();

	bool shouldClose() const;
	bool isKeyPressed(Key) const;
	bool isKeyClicked(Key);
	bool isMouseCaptured() const;

	double getMouseX() const;
	double getMouseY() const;

	double mouseDeltaX() const;
	double mouseDeltaY() const;

	void setCaptureMouse(bool capture);

private:
	ignis::Swapchain* m_swapchain;
	GLFWwindow* m_window;
	VkSurfaceKHR m_surface{VK_NULL_HANDLE};
	CreateInfo m_creationInfo;

	double m_lastMouseX{0};
	double m_lastMouseY{0};
	double m_mouseDeltaX{0};
	double m_mouseDeltaY{0};

	std::unordered_map<int, bool> m_prevKeyStates;

	ignis::Command* m_blitCmd;
	ignis::Semaphore* m_imageAvailable;
	ignis::Semaphore* m_finishedBlitting;

public:
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) = delete;
};

}  // namespace etna
