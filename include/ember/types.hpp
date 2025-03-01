#pragma once

#include "ignis/color_image.hpp"
#include "ignis/depth_image.hpp"

namespace ember {

inline constexpr ignis::ColorFormat EMBER_COLOR_FORMAT = ignis::ColorFormat::RGBA16;
inline constexpr ignis::DepthFormat EMBER_DEPTH_FORMAT =
	ignis::DepthFormat::D32_SFLOAT;

inline constexpr VkClearColorValue EMBER_CLEAR_COLOR =
	VkClearColorValue{0.02f, 0.02f, 0.02f, 1};

inline constexpr const char* EMBER_SHADERS_FOLDER = "shaders";

};	// namespace ember
