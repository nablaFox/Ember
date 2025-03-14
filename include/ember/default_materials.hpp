#pragma once

#include "material.hpp"
#include "color.hpp"

// Note: we are assuming the user has a folder "shaders"
// with a folder "ember" containing all the ember shaders
// (which kinda sucks)

namespace ember {

// TODO: it should be more explicit and controlled by the user the creation of
// default materials
inline Material defaultMaterial({
	.shaders = {"ember/default.frag.spv", "ember/default.vert.spv"},
});

inline Material pointMaterial({
	.shaders = {"ember/default.frag.spv", "ember/default.vert.spv"},
	.polygonMode = VK_POLYGON_MODE_POINT,
});

inline Material lineMaterial({
	.shaders = {"ember/default.frag.spv", "ember/default.vert.spv"},
	.polygonMode = VK_POLYGON_MODE_LINE,
	.lineWidth = 3.0f,	// TODO: add dynamic line width in ignis
});

struct GridMaterialParams {
	Color color;
	float gridSpacing;
	float lineThickness;
};

inline const MaterialTemplate<GridMaterialParams> gridMaterialTemplate({
	.shaders = {"ember/grid.frag.spv", "ember/default.vert.spv"},
	.transparency = true,
});

}  // namespace ember
