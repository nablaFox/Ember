#include "default_materials.hpp"
#include "engine.hpp"

using namespace etna;

MaterialHandle g_defaultMaterial{nullptr};
MaterialHandle g_pointMaterial{nullptr};

MaterialTemplateHandle g_gridTemplate{nullptr};
MaterialTemplateHandle g_transparentGridTemplate{nullptr};
MaterialTemplateHandle g_brickOutlineMaterialTemplate{nullptr};

// PONDER: maybe use a macro or an helper function to reduce duplication

MaterialHandle engine::getDefaultMaterial() {
	if (g_defaultMaterial == nullptr) {
		g_defaultMaterial = Material::create({
			.shaders = {"default.vert.spv", "default.frag.spv"},
		});

		queueForDeletion([=] { g_defaultMaterial.reset(); });
	}

	return g_defaultMaterial;
}

MaterialHandle engine::getPointMaterial() {
	if (g_pointMaterial == nullptr) {
		g_pointMaterial = Material::create({
			.shaders = {"default.vert.spv", "default.frag.spv"},
			.polygonMode = VK_POLYGON_MODE_POINT,
		});

		queueForDeletion([=] { g_pointMaterial.reset(); });
	}

	return g_pointMaterial;
}

MaterialHandle engine::createGridMaterial(GridMaterialParams params) {
	if (g_gridTemplate == nullptr) {
		g_gridTemplate = MaterialTemplate::create({
			.shaders = {"default.vert.spv", "grid.frag.spv"},
			.paramsSize = sizeof(GridMaterialParams),
		});

		queueForDeletion([=] { g_gridTemplate.reset(); });
	}

	return Material::create({
		.templateHandle = g_gridTemplate,
		.params = &params,
	});
}

MaterialHandle engine::createTransparentGridMaterial(GridMaterialParams params) {
	if (g_transparentGridTemplate == nullptr) {
		g_transparentGridTemplate = MaterialTemplate::create({
			.shaders = {"default.vert.spv", "grid.frag.spv"},
			.paramsSize = sizeof(GridMaterialParams),
			.transparency = true,
		});

		queueForDeletion([=] { g_transparentGridTemplate.reset(); });
	}

	return Material::create({
		.templateHandle = g_transparentGridTemplate,
		.params = &params,
	});
}

MaterialHandle engine::createBrickOutlinedMaterial(OutlineMaterialParams params) {
	if (g_brickOutlineMaterialTemplate == nullptr) {
		g_brickOutlineMaterialTemplate = MaterialTemplate::create({
			.shaders = {"default.vert.spv", "brick_outline.frag.spv"},
			.paramsSize = sizeof(OutlineMaterialParams),
		});

		queueForDeletion([=] { g_brickOutlineMaterialTemplate.reset(); });
	}

	return Material::create({
		.templateHandle = g_brickOutlineMaterialTemplate,
		.params = &params,
	});
}
