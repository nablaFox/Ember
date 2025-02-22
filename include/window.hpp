#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

class Window {
public:
	Window(const char* windowName, uint32_t width, uint32_t height);
	~Window();

	auto getWidth() const -> uint32_t { return m_width; }
	auto getHeight() const -> uint32_t { return m_height; }

	auto shouldClose() const -> bool;

	auto isKeyPressed(int key) const -> bool;

	auto getMouseX() const -> double;
	auto getMouseY() const -> double;

	auto getWindow() const -> GLFWwindow* { return m_window; }

private:
	GLFWwindow* m_window;
	uint32_t m_width, m_height;

public:
	Window(const Window&) = default;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = default;
	Window& operator=(Window&&) = delete;
};
