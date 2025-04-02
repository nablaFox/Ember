#pragma once

#include "color.hpp"
#include "material.hpp"

namespace etna::engine {

MaterialHandle createColorMaterial(Color color = WHITE);

MaterialHandle createPointMaterial(Color color = WHITE);

struct GridMaterialParams {
	Color color{WHITE};
	Color gridColor{BLACK};
	float gridSpacing{0.1};
	float thickness{0.005};
};

MaterialHandle createGridMaterial(GridMaterialParams);

MaterialHandle createTransparentGridMaterial(GridMaterialParams);

struct OutlineMaterialParams {
	Color color{WHITE};
	Color outline{BLACK};
	float thickness{0.01f};
};

// TEMP: this should be an example
MaterialHandle createBrickOutlinedMaterial(OutlineMaterialParams);

void initDefaultMaterials();

void initColorMaterial();

void initPointMaterial();

void initGridMaterial();

void initTransparentGridMaterial();

void initBrickOutlineMaterial();

}  // namespace etna::engine
