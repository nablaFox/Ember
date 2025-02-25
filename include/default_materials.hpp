#pragma once

#include "types.hpp"
#include "material.hpp"

inline Material defaultMaterial({
	.shaders = {"ember/default.frag.spv", "ember/default.vert.spv"},
});

struct GridMaterialParams {
	Color color;
	float lines;
	float lineThickness;
};

inline const MaterialTemplate<GridMaterialParams> gridMaterialTemplate({
	.shaders = {"ember/grid.frag.spv", "default.vert.spv"},
	.transparent = true,
});
