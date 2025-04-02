#pragma once

#include "color.hpp"
#include "material.hpp"

namespace etna::engine {

MaterialHandle getDefaultMaterial();

MaterialHandle getPointMaterial();

struct GridMaterialParams {
	Color gridColor;
	float gridSpacing;
	float thickness;
};

MaterialHandle createGridMaterial(GridMaterialParams);

MaterialHandle createTransparentGridMaterial(GridMaterialParams);

struct OutlineMaterialParams {
	Color outline{BLACK};
	float thickness{0.01f};
};

// TEMP: this should be an example
MaterialHandle createBrickOutlinedMaterial(OutlineMaterialParams);

}  // namespace etna::engine
