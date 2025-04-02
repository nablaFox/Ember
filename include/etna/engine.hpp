#pragma once

#include <functional>
#include "ignis/image.hpp"
#include "ignis/device.hpp"
#include "color.hpp"

namespace etna::engine {

void init();

ignis::Device& getDevice();

VkQueue getGraphicsQueue();

VkQueue getUploadQueue();

uint32_t getMaxAllowedSampleCount();

void queueForDeletion(std::function<void()>);

constexpr ignis::ColorFormat ETNA_COLOR_FORMAT{ignis::ColorFormat::RGBA16};

constexpr ignis::DepthFormat ETNA_DEPTH_FORMAT{ignis::DepthFormat::D32_SFLOAT};

constexpr uint32_t ETNA_MAX_SAMPLE_COUNT{8};

constexpr Color ETNA_CLEAR_COLOR{0.02f, 0.02f, 0.02f, 1};

}  // namespace etna::engine
