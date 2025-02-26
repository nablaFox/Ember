#pragma once

#include "types.hpp"
#include "material.hpp"

inline Material defaultMaterial({
	.shaders = {"ember/default.frag.spv", "ember/default.vert.spv"},
});

inline Material pointMaterial({
	.shaders = {"ember/default.frag.spv", "ember/default.vert.spv"},
	.polygonMode = VK_POLYGON_MODE_POINT,
});

struct GridMaterialParams {
	Color color;
	float lines;
	float lineThickness;
};

inline const MaterialTemplate<GridMaterialParams> gridMaterialTemplate({
	.shaders = {"ember/grid.frag.spv", "ember/default.vert.spv"},
	.transparency = true,
});
