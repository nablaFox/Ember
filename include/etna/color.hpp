#pragma once

#include <cstdint>

namespace etna {

struct Color {
	float r, g, b, a;

	Color& setAlpha(float alpha) {
		a = alpha;
		return *this;
	}

	static Color RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		return {r / 255.f, g / 255.f, b / 255.f, a / 255.f};
	}

	static Color RGB(uint8_t r, uint8_t g, uint8_t b) {
		return {r / 255.f, g / 255.f, b / 255.f, 1};
	}

	Color operator*(float brightness) const {
		return {r * brightness, g * brightness, b * brightness, a};
	}
};

inline Color WHITE = Color{.r = 1, .g = 1, .b = 1, .a = 1};

inline Color BLACK = Color{.r = 0, .g = 0, .b = 0, .a = 1};

inline Color RED = Color::RGB(231, 47, 69);

inline Color GREEN = Color::RGB(125, 215, 93);

inline Color BLUE = Color::RGB(110, 150, 252);

inline Color PURPLE = Color::RGB(167, 152, 220);

};	// namespace etna
