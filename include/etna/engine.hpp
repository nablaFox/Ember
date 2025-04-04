#pragma once

#include <functional>
#include "ignis/image.hpp"
#include "ignis/device.hpp"

namespace etna::engine {

void init();

ignis::Device& getDevice();

void immediateSubmit(std::function<void(ignis::Command&)>&&);

void immediateUpload(ignis::BufferId,
					 const void* data,
					 VkDeviceSize offset = 0,
					 VkDeviceSize size = 0);

void immediateUpdate(ignis::BufferId,
					 const void* data,
					 VkDeviceSize offset = 0,
					 VkDeviceSize size = 0);

ignis::Command createGraphicsCommand();
ignis::Command* newGraphicsCommand();

void queueForDeletion(std::function<void()>);

uint32_t clampSampleCount(uint32_t sampleCount);

constexpr ignis::ColorFormat COLOR_FORMAT{ignis::ColorFormat::RGBA16};

constexpr ignis::DepthFormat DEPTH_FORMAT{ignis::DepthFormat::D32_SFLOAT};

constexpr uint32_t MAX_SAMPLE_COUNT{8};

}  // namespace etna::engine

#define _device etna::engine::getDevice()
