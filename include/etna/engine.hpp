#pragma once

#include "ignis/image.hpp"
#include "ignis/device.hpp"
#include "color.hpp"
#include "material.hpp"
#include "mesh.hpp"

namespace etna {

class Engine {
public:
	Engine();

	~Engine();

	static ignis::Device& getDevice();

	static VkQueue getGraphicsQueue();

	static VkQueue getUploadQueue();

	static uint32_t getMaxAllowedSampleCount();

	static constexpr ignis::ColorFormat ETNA_COLOR_FORMAT{
		ignis::ColorFormat::RGBA16};

	static constexpr ignis::DepthFormat ETNA_DEPTH_FORMAT{
		ignis::DepthFormat::D32_SFLOAT};

	static constexpr uint32_t ETNA_MAX_SAMPLE_COUNT{8};

	static constexpr Color ETNA_CLEAR_COLOR{0.02f, 0.02f, 0.02f, 1};

	static MaterialHandle getDefaultMaterial();

	static MaterialHandle getPointMaterial();

	struct GridMaterialParams {
		Color color;
		float gridSpacing;
		float thickness;
	};

	struct OutlineMaterialParams {
		Color outline{BLACK};
		float thickness{0.01f};
	};

	static MaterialHandle createGridMaterial(GridMaterialParams);

	static MaterialHandle createTransparentGridMaterial(GridMaterialParams);

	// TEMP: this should be an example
	static MaterialHandle createBrickOutlinedMaterial(OutlineMaterialParams);

	static MeshHandle createSphere(Color color = WHITE, uint32_t precision = 100);

	static MeshHandle createTexturedBrick(float width,
										  float height,
										  float depth,
										  Color color = WHITE);

	static MeshHandle createTexturedCube(Color color = WHITE);

	static MeshHandle createRectangle(float width,
									  float height,
									  Color color = WHITE);

	static MeshHandle createQuad(Color color = WHITE);

	// TODO: implement
	static MeshHandle createCylinder(float radius,
									 float height,
									 Color color = WHITE);

	// TODO: variant for empty cyilinder

	// TODO: implement
	static MeshHandle createCone(float radius, float height, Color color = WHITE);

	static MeshHandle createPyramid(float height,
									float sideLength,
									Color color = WHITE);

	// TODO: add methods to get default instantiated meshes (cube, cylinder, etc.)
	// (a brick for example can be obtained from a cube using a scale transform)
	// for different colors the user can use default material with different params

public:
	Engine(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine& operator=(Engine&&) = delete;
};

}  // namespace etna
