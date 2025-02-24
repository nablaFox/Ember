#include <GLFW/glfw3.h>
#include <cassert>
#include "ignis/device.hpp"

// Note: we don't have a singleton device in ignis
// because in the future we will support multiple devices

struct EmberDevice {
public:
	static EmberDevice* g_instance;

	~EmberDevice() { delete m_device; }

	static void init() { g_instance = new EmberDevice(); }

	static void destroy() { delete g_instance; }

	static EmberDevice& get() {
		if (g_instance == nullptr) {
			assert("Device not initialized");
			EmberDevice::init();
		}

		return *g_instance;
	}

	static ignis::Device* getDevice() { return get().m_device; }

	static uint32_t getGraphicsQueue() { return get().m_graphicsQueue; }

	static uint32_t getUploadQueue() { return get().m_uploadQueue; }

private:
	EmberDevice() {
		glfwInit();

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions =
			glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions,
											glfwExtensions + glfwExtensionCount);

		m_device = new ignis::Device({
			.shadersFolder = "build/shaders",
			.extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
			.instanceExtensions = extensions,
		});

		// TEMP
		m_graphicsQueue = 0;
		m_uploadQueue = 0;
	}

	uint32_t m_graphicsQueue;
	uint32_t m_uploadQueue;
	ignis::Device* m_device;

public:
	EmberDevice(const EmberDevice&) = delete;
	EmberDevice& operator=(const EmberDevice&) = delete;
	EmberDevice(EmberDevice&&) = delete;
	EmberDevice& operator=(EmberDevice&&) = delete;
};

inline EmberDevice* EmberDevice::g_instance = nullptr;
