#pragma once

#include "ignis/image.hpp"

namespace etna {

static constexpr uint32_t ETNA_FRAMES_IN_FLIGHT{2};

static constexpr ignis::ColorFormat ETNA_COLOR_FORMAT{ignis::ColorFormat::RGBA16};

static constexpr ignis::DepthFormat ETNA_DEPTH_FORMAT{
	ignis::DepthFormat::D32_SFLOAT};

static constexpr VkClearColorValue ETNA_CLEAR_COLOR{0.02f, 0.02f, 0.02f, 1};

}  // namespace etna
