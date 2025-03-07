#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <unordered_map>

namespace ember {

class Window {
public:
	struct CreateInfo {
		const char* windowName{"Window"};
		uint32_t width{0};
		uint32_t height{0};
		bool captureMouse{false};
	};

	Window(CreateInfo);
	~Window();

	auto getWidth() const -> uint32_t { return m_width; }
	auto getHeight() const -> uint32_t { return m_height; }

	auto shouldClose() const -> bool;

	auto isKeyPressed(int key) const -> bool;
	auto isKeyClicked(int key) -> bool;

	auto getMouseX() const -> double;
	auto getMouseY() const -> double;
	auto mouseDeltaX() const -> double;
	auto mouseDeltaY() const -> double;

	auto captureMouse(bool caputre) const -> void;

	auto getWindow() const -> GLFWwindow* { return m_window; }

private:
	GLFWwindow* m_window;
	uint32_t m_width, m_height;

	std::unordered_map<int, bool> m_prevKeyStates;

public:
	Window(const Window&) = default;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = default;
	Window& operator=(Window&&) = delete;
};

}  // namespace ember
