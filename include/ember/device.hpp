#include <GLFW/glfw3.h>
#include <cassert>
#include "ignis/device.hpp"

// Note: we don't have a singleton device in ignis
// because in the future we will support multiple devices

namespace ember {

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

	static auto getDevice() -> ignis::Device* { return get().m_device; }

	static auto getGraphicsQueue() -> uint32_t { return get().m_graphicsQueue; }

	static auto getUploadQueue() -> uint32_t { return get().m_uploadQueue; }

	static auto getSampleCount() -> VkSampleCountFlagBits {
		return get().m_device->getMaxSampleCount();
	}

private:
	EmberDevice();

	uint32_t m_graphicsQueue;
	uint32_t m_uploadQueue;
	ignis::Device* m_device;

public:
	EmberDevice(const EmberDevice&) = delete;
	EmberDevice& operator=(const EmberDevice&) = delete;
	EmberDevice(EmberDevice&&) = delete;
	EmberDevice& operator=(EmberDevice&&) = delete;
};

}  // namespace ember
