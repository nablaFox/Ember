#pragma once

#include <functional>
#include "ignis/image.hpp"
#include "ignis/device.hpp"

namespace etna::engine {

ignis::Device& getDevice();

void init();

VkQueue getGraphicsQueue();

VkQueue getUploadQueue();

uint32_t getMaxAllowedSampleCount();

void queueForDeletion(std::function<void()>);

// TODO: remove ETNA prefix since we are already in etna::engine ns
constexpr ignis::ColorFormat ETNA_COLOR_FORMAT{ignis::ColorFormat::RGBA16};

constexpr ignis::DepthFormat ETNA_DEPTH_FORMAT{ignis::DepthFormat::D32_SFLOAT};

constexpr uint32_t ETNA_MAX_SAMPLE_COUNT{8};

}  // namespace etna::engine

#define _device etna::engine::getDevice()
